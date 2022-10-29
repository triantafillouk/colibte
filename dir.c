/*	
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

	DIR mode functions 
*/

#include	"xe.h"
#include	<sys/stat.h>
#include	<sys/types.h>
#include	<glib.h>
#include	<pwd.h>
#include	<grp.h>

#define FTYPE_ERROR		-1
#define FTYPE_NORMAL	0
#define FTYPE_LINK		1
#define FTYPE_DIR		2
#define FTYPE_DIRLINK	3

#define	FILE_VIEW	0
#define	FILE_EXEC	1

#define DIR_CURRENT	0
#define	DIR_OTHER	1

char **f_extcmd;
char **f_extension;

extern int dont_edit();
extern alist *file_list;
extern FILEBUF *cbfp;

/* local function declarations */
int list_dir(char *dir_name,FILEBUF *fp);
void qsort_dir(struct kdirent **arr, int elements,int sort_mode);
char **getdir(char *dirname,char *s_find,int *num);
char *str_tfile(struct stat *t,char *file_name,int maxsize);
FILEBUF *get_dir_buffer(int flag,int dir_num);
int dir_other_reload(int n);
void escape_file_name(char *fname);
void delete_current_list_line();

int pdline=-1;
int current_sort_mode=-1;

typedef       int     (*Fintss)(const char *, const char *);

 /* This is used in dired mode dates */
char *month[] = { "Jan", "Feb","Mar","Apr", "May","Jun","Jul","Aug","Sep","Oct","Nov","Dec","error" };

void convert_to_windows_name(char *fname)
{
 if(!fname) return;
 if(strlen(fname)>2) 
 if(fname[1]==':'){
 char *s=fname;
 while(*s!=0) {
	if(*s=='/') *s='\\';
 	s++;
 }};
}

void init_names()
{
 struct stat st;
 char home_dir[MAXFLEN];

 // Initialize clipboard
 init_clipboard();
 
 strlcpy(home_dir,getenv("HOME"),MAXFLEN);
 strlcat(home_dir,"/.",MAXFLEN);
 strlcat(home_dir,APPLICATION_NAME,MAXFLEN);
 if(stat(home_dir,&st)==0) 
 {	// check for editors home dir
 	if(S_ISDIR(st.st_mode)) {
 	} else {
		unlink(home_dir);
 		mkdir(home_dir,S_IRWXU);
 	}
 } else { /* Not a . dir in user's home dir, create it. */
 	// create dir
	if(mkdir(home_dir,S_IRWXU)) {
		SYS_ERROR("cannot create .%s dir in home dir",APPLICATION_NAME);
	} else {
		MESG("created .%s dir in home dir",APPLICATION_NAME);
	};
	// move files from default dir here
 };
}

void clear_snames(char **list,int size)
{
 int i;
 for(i=0;i<=size;i++) {
 	if(list[i]==NULL) break;
	free(list[i]);
 };
 free(list);
}

// quicksort from alienryderflex.com is wikipedia modified 
// non recursive 
void qsort_str(char **arr, int elements, Fintss cmpFunc) 
{
  char  *piv;
  int *beg, *end;
  int i=0, L, R ;
//  MESG("qsort_str:");
  beg=(int *)malloc(sizeof(int)*(elements+1));
  end=(int *)malloc(sizeof(int)*(elements+1));
  beg[0]=0; end[0]=elements;
  while (i>=0) {
    L=beg[i]; R=end[i]-1;
    if (L<R) {
      piv=arr[L]; 
      while (L<R) {
        while (
			cmpFunc((const char *) arr[R],(const char *) piv) >= 0
			&& L<R) R--; 
		if (L<R) arr[L++]=arr[R];
        while (
			cmpFunc((const char *) arr[L],(const char *) piv) <= 0
			&& L<R) L++; 
		if (L<R) arr[R--]=arr[L]; 
	  }
      arr[L]=piv; 
	  beg[i+1]=L+1; end[i+1]=end[i]; end[i++]=L; 
	} else {
      i--; 
	}
  };
  free(beg);free(end);
}


/* simple string compare */
int stricmp1(const char *str1, const char *str2)
{
    register char  cc, dd ;
    
    do {
        cc = *str1++ ;
        dd = *str2++ ;
        if((cc != dd)) return (cc-dd);
    } while(cc != 0) ;
    return 0 ;
}

// directory compare, first the directories, then the files
int dir_cmp(struct kdirent *a, struct kdirent *b,int sort_mode)
{
 if (((a->t.st_mode & S_IFMT) != S_IFDIR) && ((b->t.st_mode & S_IFMT) == S_IFDIR)) return 1;
 if (((a->t.st_mode & S_IFMT) == S_IFDIR) && ((b->t.st_mode & S_IFMT) != S_IFDIR)) return -1;
 switch(sort_mode){
	case 0: return(stricmp1((const char *) a->d_name,(const char *) b->d_name));
	case 1: return(stricmp1((const char *) b->d_name,(const char *) a->d_name));
	case 2: return(a->t.st_size > b->t.st_size? 1:-1);
	case 3: return(a->t.st_size < b->t.st_size? 1:-1);
	case 4: return(a->t.st_mtime > b->t.st_mtime? 1:-1);	/* modification time  */
	case 5: return(a->t.st_mtime < b->t.st_mtime? 1:-1);	/* reverse modification time  */

	case 6: return(a->t.st_ctime > b->t.st_ctime? 1:-1);	/* status change time  */
	case 7: return(a->t.st_ctime < b->t.st_ctime? 1:-1);	/* status change reverse time  */
	case 8: return(a->t.st_atime > b->t.st_atime? 1:-1);	/* access time  */
	case 9: return(a->t.st_atime < b->t.st_atime? 1:-1);	/* reverse access time  */
 };
 return 0;
}

/* returns the directory contents with filter in a string list */
/* used mainly in open file,list_dir1 in curses/xlib */
char **getdir(char *dirname,char *s_find,int *num)
{
 int n; /* no of files in directory */
 char **namelist=NULL;
 DIR *d1;
 struct dirent *df1;
 int i;

 d1 = opendir((const char *)dirname);
 if(d1==NULL) { error_line("cant open dir");return NULL;};
 
 for(i=0;;i++) {
 	df1=readdir(d1);
	if(df1==NULL) { n=*num=i; closedir(d1);break;};
	if(strcmp(".",df1->d_name)==0) { i--;continue;};
	/* filter it!  */
	if(strncmp(s_find,df1->d_name,strlen(s_find))!=0) { i--; continue;};
	if(i%256 == 0) namelist = (char **)realloc((char *)namelist, (i+256)*sizeof(char *));
	namelist[i]=(char *)malloc(strlen(df1->d_name)+1);
	strlcpy(namelist[i],df1->d_name,strlen(df1->d_name)+1);
 } ;
 qsort_str(namelist,n,stricmp1);

 return(namelist);
}

int set_sort_mode(int mode)
{
 int current_sort_mode=cwp->w_fp->sort_mode;
 MESG("set_sort_mode: current=%d",current_sort_mode);
 if(!(cwp->w_fp->b_flag & FSDIRED)) return true;
	if(mode==current_sort_mode) 
	{	/* reverse sort  */
		if(current_sort_mode%2) current_sort_mode-=1;
		else current_sort_mode+=1;
	} else {
		current_sort_mode=mode;
	} ;
	cwp->w_fp->sort_mode=current_sort_mode;
	MESG("	new sort mode=%d",current_sort_mode);
	// if(sort_mode != old_mode) 
	{
		dir_reload(1);
	};
	return true;
}

int change_sort_mode(int mouse_col)
{
	if(mouse_col>7 && mouse_col<15) {
		MESG("sort by size");
		set_sort_mode(2);
		// update_screen(TRUE);
		return 1;
	// set/toggle sort by size 
	};  
	if(mouse_col>15 && mouse_col<30) {
	// set/toggle sort by date
		MESG("sort by date");
		set_sort_mode(4);
		// update_screen(TRUE);
		return 1;
 	};
	if(mouse_col>30) {
		MESG("sort by name");
		// set/toggle sort by name
		set_sort_mode(0);
		// update_screen(TRUE);
		return 1;
	};
	return 0;
}

/* this is a local scandir. Not all operating systems use the BSD one! */
int scandir2(char *dirname, struct kdirent ***namelist_a)
{
 int num_of_files; /* no of files in directory */
 struct kdirent **namelist;
 DIR *d1;
 struct dirent *df1;
 int i=0;
 int result=0;
 struct stat t;

 namelist = NULL;
 // MESG("scandir2: sizeof stat=%d dirent=%d",sizeof(struct stat),sizeof(struct kdirent));
 d1 = opendir(dirname);
 if(d1==NULL) { return error_line("cant open dir %s",dirname);};
 if(chdir(dirname)) { return error_line("cant open dir %s",dirname);};

 for(i=0;;i++) {
 	df1=readdir(d1);

	if(i%256 == 0) {
	 namelist = (struct kdirent **)realloc((void *)namelist, ((i+256)*sizeof(struct kdirent *)));
	};
	if(df1==NULL) { num_of_files=i; closedir(d1);break;};
	if(namelist==NULL) { return error_line("cant create directory namelist");};

	namelist[i]=(struct kdirent *)malloc(sizeof(struct kdirent)+strlen(df1->d_name)+1);
	if(namelist[i]==NULL) exit(1);
	strlcpy(namelist[i]->d_name,df1->d_name,strlen(df1->d_name)+1);
	result=lstat(namelist[i]->d_name,&t);
	if(result) {
		ERROR("[%30s]Error    %d ",namelist[i]->d_name,errno);
		t.st_dev=0;
		t.st_ino=0;
		t.st_mode=0;
		t.st_uid=0;
		t.st_blksize=0;
		t.st_blocks=0;
		t.st_atime=0;
		t.st_ctime=0;
		memcpy(&namelist[i]->t,&t,sizeof(struct stat));
	} else {
		memcpy(&namelist[i]->t,&t,sizeof(struct stat));
	};
 } ;
   namelist[i]=NULL;
   qsort_dir(namelist,num_of_files,current_sort_mode);
   *namelist_a = namelist;
 return(num_of_files);
}

// quick sort a directory list
void qsort_dir(struct kdirent **arr, int elements,int s_mode) 
{
  struct kdirent  *piv;
  int *beg, *end;
  int i=0, L, R ;
//  MESG("qsort_dir: sort_mode=%d",s_mode);
  beg=(int *)malloc(sizeof(int)*(elements+1));
  end=(int *)malloc(sizeof(int)*(elements+1));
  beg[0]=0; end[0]=elements;
  while (i>=0) {
    L=beg[i]; R=end[i]-1;
    if (L<R) {
      piv=arr[L]; 
      while (L<R) {
        while (
			dir_cmp( arr[R], piv,s_mode) >= 0
			&& L<R) R--; 
		if (L<R) arr[L++]=arr[R];
        while (
			dir_cmp( arr[L], piv,s_mode) <= 0
			&& L<R) L++; 
		if (L<R) arr[R--]=arr[L]; 
	  }
      arr[L]=piv; 
	  beg[i+1]=L+1; end[i+1]=end[i]; end[i++]=L; 
	} else {
      i--; 
	}
  };
  free(beg);free(end);
}

char *dir_name(int dtype)
{
 static char dname[8];
 strlcpy(dname,"[D#]",8);
 dname[2]='0'+dtype;
 return(dname);
}

void set_todirname(char *base_name,int dir_num)
{
 strlcpy(base_name,"[D#]",8);
 base_name[2]='0'+dir_num;
}

int dir_edit(int n) 
{
  int status=FALSE;
  int ftype;
  FILEBUF *b,*vb;
  char fname[MAXFLEN];
  char dname[MAXFLEN];

  MESG("dir_edit: b_flag=%X b_type=%d",cbfp->b_flag,cbfp->b_type);
  if(!(cbfp->b_flag & FSNLIST))
  {
	int old_type=cbfp->b_type;
	if(cbfp->b_flag & FSINVS) return(TRUE);
	if(!IS_FLAG(cbfp->b_state,FS_VIEWA)) return (TRUE);

	// MESG("flags b_state=0x%X b_flag=0x%X b_type=0x%X",cbfp->b_state,cbfp->b_flag,cbfp->b_type);
    num offset=tp_offset(cwp->tp_current);;
	FILEBUF *is_connected_to = cbfp->connect_buffer;
	int ppline = tp_line(cwp->tp_current)-tp_line(cwp->tp_hline)+1;
	
	if(snprintf(fname,MAXFLEN,"%s/%s",cbfp->b_dname,cbfp->b_fname) >= MAXFLEN) {
		return(TRUE);
	};

	vb = cbfp;
	b = new_filebuf(dir_name(vb->dir_num),0);
	b->b_type = cbfp->b_type|old_type;
	// MESG("set connect buffer !! b_type=%d",b->b_type);
	int is_type = cbfp->b_type;

	select_filebuf(b);
	/* delete view file buffer */
	delete_filebuf(vb,1);
	/* then open the file the usual way */

	// MESG("1---- b_type=%d",cbfp->b_type);
#if	1
	cbfp->b_flag=0;
	// MESG("file[%s / %s -> %s",cbfp->b_dname,cbfp->b_fname,fname);
	create_base_name(cbfp->b_fname,fname);
	// MESG("file[%s / %s -> %s",cbfp->b_dname,cbfp->b_fname,fname);
	file_read(cbfp,fname);
#else
	status=goto_file(fname);
#endif
	// MESG("2----");

	strlcpy(cbfp->b_dname,getcwd(dname,MAXFLEN),MAXFLEN);
	igotooffset(offset,ppline);
	// MESG("new flags add %d is_type=%d",old_type,is_type);
	cbfp->connect_buffer = is_connected_to;
	cbfp->b_type = is_type;

	set_hmark(1,"dir_edit:a");
	return(status);
  } else {
	
	ftype=dir_getfile(fname,0);
	// MESG("dir_edit: ftype=%d",ftype);
  	if(ftype<0) return(FALSE);
  	if(ftype==FTYPE_NORMAL) {
		set_full_name(dname,cbfp->b_dname,fname,MAXFLEN);
		status=goto_file(dname);
		set_working_dir(cbfp->b_dname);
		set_hmark(1,"dir_edit:b");
  	} else msg_line("not normal");
	return(status);
 };
}

/* goto edit mode  */
int start_edit(int n)
{
 return true;
}

/* simple copy */
int dir_copy(int n)
{
  struct stat t;
  int s;
  int is_other_dir=0;
  int ftype;
  int sstat=0;
  char destination[MAXFLEN];
  char destination_escaped[MAXFLEN];
  char fname[MAXFLEN];
  char sline[MAXLLEN];
  char *cp_flags="";
  char sconfirm[MAXLLEN];
  int s1,destination_is_dir=0;
  FILEBUF *dbuf;	/* destination dir buffer  */

  if(!(cbfp->b_flag & FSNLIST)) return FALSE;
  dbuf=get_dir_buffer(DIR_OTHER,0);	/* get destination dir buffer  */
  if(dbuf!=NULL) {
  	strlcpy(destination,dbuf->b_dname,MAXFLEN);
  } else {
  	destination[0]=0;
  };
  set_list_type(LDIR2);
  ftype=dir_getfile(fname,1);

  sstat=snprintf(sconfirm,MAXLLEN,"Copy [%s] to: ",fname);
  if(sstat>=MAXLLEN) MESG("truncated 7");
  if((s = nextarg(sconfirm,destination,MAXFLEN,true)) !=TRUE) return(s);

  strlcpy(destination_escaped,destination,MAXFLEN);

  if (ftype == FTYPE_DIR) cp_flags="-rp";

  s1=chdir(cbfp->b_dname);
//  MESG("current dir is [%s]",getcwd(NULL,MAXFLEN));
//  MESG(" buffer dir is [%s]",cbfp->b_dname);
  escape_file_name(fname);
  escape_file_name(destination_escaped);

  if(snprintf(sline,MAXLLEN,"cp %s %s %s 2> /dev/null",
  	cp_flags,fname,destination_escaped)>=MAXLLEN) {
		msg_line("File name too long!");
		return(FALSE);
	};
  // MESG("dir_copy:[%s]",sline);
  s1=stat(destination_escaped,&t);
  // MESG("dir_copy:[%s] status=%d",sline,s1);
  if(s1==0) {	/* it exists!  */
  	s1=1;
	destination_is_dir = (t.st_mode & S_IFMT) == S_IFDIR;
  } else s1=0;

  if((s1=(system(sline))) !=0 ) { 
	return error_line("Error copying %s to %s status=%d",fname,destination,s1);
  };
//	MESG("result status =%d",s1);
	if(strstr(destination,"/")!=NULL) is_other_dir=1;
	s1=chdir(cbfp->b_dname);
	if(destination_is_dir || is_other_dir) {
//		cbfp->b_state &= ~FS_CHG;
		dir_other_reload(1);
		return(TRUE);
	} else dir_reload(1);

  return(TRUE);
}

/* simple rename/move */
int dir_move(int n) 
{
  struct stat t;
  int s1;
  int s2;
  int is_other_dir=0;
  int destination_is_dir=0;
  char sconfirm[MAXLLEN];
  char destination[MAXFLEN],fname[MAXFLEN],sline[MAXLLEN];
  FILEBUF *dbuf;	/* destination dir buffer  */
  int sstat=0;

  if(!(cbfp->b_flag & FSNLIST)) return FALSE;
  if(dir_getfile(fname,1)<0) return FALSE;
  set_list_type(LDIR2);
  dbuf=get_dir_buffer(DIR_OTHER,0);	/* get destination dir buffer  */
  if(dbuf!=NULL) {
  	strlcpy(destination,dbuf->b_dname,MAXFLEN);
  } else {
  	destination[0]=0;
  };

  sstat=snprintf(sconfirm,MAXLLEN,"Move [%s] to: ",fname);
  if(sstat>=MAXLLEN) MESG("truncated 8");
  if((s1 = nextarg(sconfirm,destination,MAXFLEN,true)) !=TRUE) return(s1);

  escape_file_name(fname);
  strlcpy(sline,"mv ",MAXLLEN);
  strlcat(sline,fname,MAXLLEN);
  strlcat(sline," ",MAXLLEN);
  strlcat(sline,destination,MAXLLEN);
  strlcat(sline," 2> /dev/null",MAXLLEN);
  s1=chdir(cbfp->b_dname);
  s1=stat(destination,&t);
  if(s1==0) {	/* it exists!  */
  	s1=1;
	destination_is_dir = (t.st_mode & S_IFMT) == S_IFDIR;
	// MESG("destination is dir!");
  } else s1=0;
  s2=system(sline);

  if(s2 != 0) { return error_line("Error moving %s to %s",fname,destination);}
  else {
	// check if moved to other dir
	if(strstr(destination,"/")!=NULL) is_other_dir=1;
	if(destination_is_dir || is_other_dir) {
		// MESG("delete current line!");
		delete_current_list_line();
		if(dbuf!=NULL) {
			// MESG("reload other dir:");
			dir_other_reload(1);
			// MESG("other reloaded!");
			dbuf->b_state &= ~FS_CHG;
		};
		s1=chdir(cbfp->b_dname);
		// MESG("chdir to %s",cbfp->b_dname);
		// MESG("set update!");
		set_update(cwp,UPD_ALL);
		return(TRUE);
	} else { /* a simple rename */
		reload_file(1);
		cbfp->b_state &= ~FS_CHG;
	};
  };
  msg_line("ok!");
  return(TRUE);
}

/* Rename, move in the same dir to a different name!  */
int dir_file_rename(int n) 
{
  struct stat t;
  int s1;
  int s2;
  int is_other_dir=0;
  int destination_is_dir=0;
  char sconfirm[MAXLLEN];
  char destination[MAXFLEN],fname[MAXFLEN],sline[MAXLLEN];

  if(!(cbfp->b_flag & FSNLIST)) return FALSE;
  if(dir_getfile(fname,1)<0) return FALSE;
  strlcpy(destination,fname,MAXLLEN);
  set_list_type(LDIR2);

  snprintf(sconfirm,MAXLLEN,"Rename to: ");
  if((s1 = nextarg(sconfirm,destination,MAXFLEN,true)) !=TRUE) return(s1);

  escape_file_name(fname);
  escape_file_name(destination);
  if(snprintf(sline,MAXLLEN,"mv %s %s 2> /dev/null",
  	fname,destination)>=MAXLLEN) {
	return error_line("Name too long!");
  };

  s1=chdir(cbfp->b_dname);
  s1=stat(destination,&t);
  if(s1==0) {	/* it exists!  */
  	s1=1;
	destination_is_dir = (t.st_mode & S_IFMT) == S_IFDIR;
  } else s1=0;

  s2=system(sline);

  if(s2 != 0) { return error_line("Error moving file to %s",destination);}
  else {
	// check if moved to other dir
	if(strstr(destination,"/")!=NULL) is_other_dir=1;
	if(destination_is_dir || is_other_dir) {
		delete_current_list_line();
		return(TRUE);
	} else { /* a simple rename */
		reload_file(1);
	};
  };
  msg_line("ok!");
  return(TRUE);
}

int dir_new_file(int n)
{
  int s1;
  struct stat t;
  char fname[MAXFLEN],sline[MAXLLEN];
  int sstat=0;
	set_list_type(LDIR2);
	fname[0]=0;
	if((s1 = nextarg("New file as : ",fname,MAXFLEN,true)) !=TRUE) return(s1);
    escape_file_name(fname);
	sstat=snprintf(sline,MAXLLEN,"touch %s 2> /dev/null",fname);
	if(sstat>=MAXLLEN) return error_line("truncated file at 9");

	s1=chdir(cbfp->b_dname);
	s1=system(sline);
	if(s1 != 0) { return error_line("Error creating file %s",fname);};
	// insert file ..
	s1=stat(fname,&t);
	str_tfile(&t,fname,250);
	dir_reload(1);
	return 1;
}

int dir_new_dir(int n)
{
  int s1;
  struct stat t;
  char fname[MAXFLEN],sline[MAXLLEN];
  int sstat=0;

	fname[0]=0;
	set_list_type(LDIR2);
	if((s1 = nextarg("New dir as : ",fname,MAXFLEN,true)) !=TRUE) return(s1);
    escape_file_name(fname);
	sstat=snprintf(sline,MAXLLEN,"mkdir %s 2> /dev/null",fname);
	if(sstat>=MAXLLEN) { return error_line("cannot create new dir! %s");};

    s1=chdir(cbfp->b_dname);
	s1=system(sline);
	if(s1 != 0) { return error_line("Error creating dir %s",fname);};
	// insert file ..
	s1=stat(fname,&t);
	str_tfile(&t,fname,250);
	cbfp->b_state &= ~FS_CHG;
	dir_reload(1);
	return 1;
}

int dir_link(int n)
{
  int s1;
  int s2;
  char sconfirm[MAXLLEN];
  char destination[MAXFLEN],fname[MAXFLEN],sline[MAXLLEN];
  char current_dir[MAXLLEN];
  char source_name[MAXLLEN];
  int sstat=0;
  FILEBUF *dbuf;	/* destination dir buffer  */
//  MESG("dir_link:");
  if(!(cbfp->b_flag & FSNLIST)) return FALSE;
  if(dir_getfile(fname,1)<0) return FALSE;

  set_full_name(source_name,getcwd(current_dir,MAXLLEN),fname,MAXLLEN);
  set_list_type(LDIR2);
  
  destination[0]=0;
  sstat=snprintf(sconfirm,MAXLLEN,"Link [%s] to: ",source_name);
 
  escape_file_name(source_name);
  if(sstat>=MAXLLEN) MESG("dir link fname overflow!");

  dbuf=get_dir_buffer(DIR_OTHER,0);	/* get destination dir buffer  */
  if(dbuf!=NULL) {
  	strlcpy(destination,dbuf->b_dname,MAXFLEN);
  } else {
  	destination[0]=0;
  };
//  MESG("destination dir is [%s]",destination);

  if((s1 = nextarg(sconfirm,destination,MAXFLEN,true)) !=TRUE) return(s1);


  escape_file_name(destination);
  sstat=snprintf(sline,MAXLLEN,"ln -s %s %s 2>/dev/null",source_name,destination);
  if(sstat>=MAXLLEN) { return error_line("dir link fname overflow!");return FALSE;};

  s1=chdir(cbfp->b_dname);
  s2=system(sline);
  if(s2 != 0) { return error_line("Error linking file to %s",destination);};
// refresh other buffer if onscreen
  dir_other_reload(1);
  return 1;
}

int dir_reload(int n)
{
	pdline = cbfp->cdir->cline-1;
	reinit_dir(cbfp);
	set_update(cwp,UPD_EDIT);
	if(cbfp->sort_mode == -1) cbfp->sort_mode=0;
	current_sort_mode=cbfp->sort_mode;
	list_dir(cbfp->cdir->dir_name,cbfp);
	return 1;
}

int dir_other_reload(int n)
{
  FILEBUF *dbuf;	/* destination dir buffer  */
  WINDP *wp;

 	// MESG("dir_other_reload:");
	dbuf=get_dir_buffer(DIR_OTHER,0);	/* get destination dir buffer  */
	if(dbuf==NULL) return 0;
	// MESG("dir_other_reload:[%s]",dbuf->b_dname);
	wp = find_buffer_window(dbuf);
	if(wp){
		insert_dir(dbuf,1);
		set_update(wp,UPD_EDIT|UPD_MOVE);
	} else {
		insert_dir(dbuf,1);
	};
	return 1;
}

void delete_current_list_line()
{
	int i=0;
	int current_line=cwp->current_note_line;
	if(cwp->w_fp->b_notes==0) return;
	lbegin(cwp->w_fp->dir_list_str);
	for(i=0;i<current_line;i++) lmove_to_next(cwp->w_fp->dir_list_str,0);
	// MESG("delete_current_list_line: %d size=%d %d",i,cwp->w_fp->dir_list_str->size-1,cwp->w_fp->b_notes);
	remove_current_from_list(cwp->w_fp->dir_list_str);
	cwp->w_fp->b_notes--;
	if(cwp->current_note_line > cwp->w_fp->dir_list_str->size-1) {
		cwp->current_note_line--;
	};
	// MESG("delete_current_list_line: new size=%d %d",cwp->w_fp->dir_list_str->size-1,cwp->w_fp->b_notes);
	set_update(cwp,UPD_EDIT);
	cbfp->b_state &= ~FS_CHG;
}

 /* delete one file */
int dir_del1(int  n) 
{
  int status;
  char fname[MAXFLEN];
  MESG("\ndir_del1:");
  if(!(cbfp->b_flag & FSNLIST)) return FALSE;
  status=chdir(cbfp->b_dname);
  
  status=dir_getfile(fname,1);
  if(!confirm("Delete file",fname,0)) return FALSE;

  // MESG("dir_del1: [%s] ---------",fname);
  if(status<0) {
  	return error_line("cannot access file '%s' to delete! status=%d",fname,status);
  }
  
  if(status==FTYPE_NORMAL||status==FTYPE_LINK||status==FTYPE_DIRLINK) 	/* normal file or link */
  {
	status=dir_getfile(fname,0); // unlink understand spaces!
	// MESG("dir_del1: [%s] status=%d",fname,status);
  	status = unlink(fname);
	if(status==0) {  
		delete_current_list_line();
	} else {
		return error_line("can not delete, status is %d",status);
	};
  } else if(status==FTYPE_DIR) {	/* FTYPE_DIR  */

	char rmcmd[MAXFLEN];
    char sconfirm[MAXLLEN];
	int sstat=0;
	sstat=snprintf(rmcmd,MAXFLEN,"rmdir %s 2>/dev/null",fname);
	if(sstat<MAXFLEN) {
		status=system(rmcmd);
	} else {
		return error_line("command truncated!");
	};
//	MESG("status = %d",status);
	if(status) {
		sstat=snprintf(sconfirm,MAXLLEN,"Dir [%s] not empty",fname);
		if(sstat>=MAXLLEN) { return error_line("command truncated!");};
		if( confirm(sconfirm,"remove ?",1)) {
			sstat=snprintf(rmcmd,MAXFLEN,"rm -rf %s 2> /dev/null",fname);
			if(sstat<MAXFLEN) status=system(rmcmd);
			else { return error_line("cannot delete file, command truncated!");};
		}
	};

	if(status) { 
		set_update(cwp,UPD_STATUS);
		return error_line("Dir %s not removed",fname);
	} else { 
		delete_current_list_line();
		msg_line("File deleted!");
	};
  };
  return(TRUE);
}

int movein_dir(char *fname)
{
 int status;
   cbfp->cdir->cline = getcline();
   status=chdir(cbfp->b_dname);
   status=chdir(fname);
   if(status!=0) {
    return error_line("No access in '%s'",fname);
   };
   // MESG("movein_dir: cline=%ld pdline=%ld",cbfp->cdir->cline,pdline);
   pdline = -1;
	msg_line("");
	push_dir(cbfp);
	// create a new one!!
	cbfp->cdir = (dir_l *)malloc(sizeof(struct dir_l));
	cbfp->cdir->dir_name = getcwd(NULL,MAXFLEN);
	strlcpy(cbfp->b_dname,cbfp->cdir->dir_name,MAXFLEN);
	set_working_dir(cbfp->b_dname);
//	MESG("movin_dir b_dname=[%s] full_name=[%s]",cbfp->b_dname,cbfp->b_fname);
//	MESG("dir_num = %d",cbfp->dir_num);
	if(cbfp->dir_num >= DIR_NOTES) {
		char *notes_dir = strstr(cbfp->b_dname,"Notes");
		sprintf(cbfp->b_fname,"[%s]",notes_dir);
//		MESG("new notes [%s]",cbfp->b_fname);
	};
	cbfp->cdir->cline=cwp->current_note_line;
	cwp->current_note_line=0;
	return insert_dir(cbfp,0);
}

/* view a file in dir mode */
int dir_view(int n) 
{
  int is_dir;
  char fname[512];
  if(!(cbfp->b_flag & FSNLIST)) return FALSE;
  is_dir=dir_getfile(fname,0);

  if(is_dir<0) {
  	return error_line("Not a directory! '%s'",fname);
  };
  if(is_dir) {
	return movein_dir(fname);
  } else {
  	// MESG("dir_view: 1");
	view_file(fname);
	return(TRUE);
  };
}

void init_extensions()
{
 char *fname;

 set_start_dir(NULL);

 if((fname = find_file(NULL,APPLICATION_EXTENSIONS,1,0))==NULL) return;

 read_pairs(fname,'=',&f_extension,&f_extcmd);
 // MESG("init_extentions: file=[%s]",fname);
// for(int i=0;f_extcmd[i]!=NULL;i++) MESG("%d: %s=%s",i,f_extension[i],f_extcmd[i]);
}

int exec_shell(char *tline);

// external execution of a file
int exec_ext(char *fname,char *fname_ns,int f_vx)
{
	char wd[MAXFLEN];
	char cmd[MAXLLEN];
	char *cmd_ext=NULL;
	int status=0;
	char *comma;
	int s1;
	int is_compressed;
	int inview=0;
	char exts[MAXFLEN];
	char *s;
	int sstat=0;
	int internal_view=0;
	int web_view=0;
	int err=0;
	if((s=getenv("DISPLAY"))!=NULL) {
	  if(s[0]==0) {
		inview=1;
		}
	} else {
		inview=1;
	};

	// MESG("exec_ext: [%s] [%s],f_v=%d inview=%d",fname,fname_ns,f_vx,inview);
	if(inview) {
		// MESG("exec_ext: 1");
		view_file(fname_ns);
		return TRUE;
	};
 	s1=file_type(fname,&is_compressed,exts);	/* get extension and compression type  */
//	Find if an external viewer is needed
	s1=sarray_index(f_extension,exts);
	// MESG("exec_ext: [%s] ext=[%s] s1=%d",fname_ns,exts,s1);

	if(s1<1) { // no extension found (0 is none!)
		if(f_vx==FILE_EXEC) { // execute in background
			if(fname[0]!='/') strlcpy(wd,"./",MAXLLEN);else wd[0]=0;
			strlcat(wd,fname,MAXLLEN);
			exec_shell(wd);
			return true;
		} else {
			// MESG("exec_ext: 2");
			view_file(fname_ns) ; 
			return TRUE;
		}
	} else {
//		MESG("extension found %d [%s]",s1,f_extcmd[s1]);
	};
	
//  find command for view or execute (first is view, second is execute)
	comma=strchr(f_extcmd[s1],',');
	if(comma) {
		if(f_vx==FILE_EXEC) {
			strlcpy(cmd,comma+1,MAXLLEN);
		} else {
			strlcpy(cmd,f_extcmd[s1],MAXLLEN);
			cmd[comma-f_extcmd[s1]]=0;
			inview=1;
		};
	} else {
		strlcpy(cmd,f_extcmd[s1],MAXLLEN);
	};
//	MESG("cmd [%s]",cmd);
	internal_view=sindex(cmd,"int");
	if(internal_view) {	/* use internal view  */
		// MESG("exec_ext: 3 internal_view");
		view_file(fname_ns);
		return TRUE;
	};

	if(inview) {
		cmd_ext=strdup(cmd);
		sstat=snprintf(cmd,MAXLLEN,"%s %s > /tmp/exec 2>/tmp/err",cmd_ext,fname);
		free(cmd_ext);
		if(sstat>=MAXLLEN) { return error_line("command 9 truncated!");};
		sstat=snprintf(fname,MAXFLEN,"/tmp/exec");
		if(sstat>=MAXFLEN) { return error_line("command 10 truncated!");};
		if(system(cmd)== -1) {
			return error_line("Error return from '%s %s'",cmd,fname);
		}
		strlcpy(wd,"/tmp",MAXFLEN);
		// MESG("exec_ext: 4 inview: %s",fname);
		view_file(fname);
		return true;
	};
	if(is_compressed) {
		char cmd2[MAXLLEN];
		sstat=set_unique_tmp_file(wd,fname,MAXFLEN);
		if(!sstat) return false;
		/* This is a compressed file. Uncompress to a temporary first */
		sstat=snprintf(cmd2,MAXLLEN,"zcat %s > %s 2>/tmp/err",fname,wd);
		// MESG("exec_ext: uncompress command $s",cmd2);
		if(sstat<MAXLLEN) {
			if(system(cmd2)) { return error_line("cannot decompress file");};
		} else { return error_line("truncated, cannot sysexec");};
		sync();
		// sstat=snprintf(wd,MAXFLEN,"%s",tmp_name);
		// if(sstat>=MAXFLEN) { return error_line("command 11 truncated!");};
		sstat=snprintf(cmd2,MAXLLEN,"%s \"%s\" >/dev/null 2>/dev/null &",cmd,wd);
		// MESG("exec_ext: exec command $s",cmd2);
		if(sstat<MAXLLEN) status = sysexec(cmd2);
		else { return error_line("truncated, cannot sysexec");};
	} else {
		s=getcwd(wd,MAXFLEN);
	};

// execute the command with the file name as argument
	cmd_ext=strdup(cmd);
	web_view=sindex(cmd_ext,":");	// we need to prepend 'file://'

	if(!strncmp("/mnt/",cmd_ext+web_view,5)) {
		char ms_fname[MAXLLEN];
		sstat=snprintf(ms_fname,MAXLLEN,"%s/%s",wd,fname_ns);
		if(sstat>=MAXLLEN-1) {
			return error_line("error return from %s",ms_fname);
		};
//		convert_to_windows_name(ms_fname);
		if(!strncmp("/mnt/",ms_fname,5)) {
			ms_fname[4]=ms_fname[5];
			ms_fname[5]=':';
			// MESG("cmd [%s] fname_ns=[%s]",cmd_ext,ms_fname+4);
			convert_to_windows_name(ms_fname+4);
			if(web_view) sstat=snprintf(cmd,MAXLLEN,"%s \"file:///%s\" >/dev/null  2>/dev/null &",cmd_ext+web_view,ms_fname+4);
			else sstat=snprintf(cmd,MAXLLEN,"%s \"%s\" >/dev/null  2>/dev/null &",cmd_ext,ms_fname+4);
//			MESG("exec:1 [%s]",cmd);
		} else {
			MESG("Cannot open file command cmd=[%s] fname=[%s]",cmd_ext,ms_fname);
			err=1;
		};
	} else {
		if(web_view) sstat=snprintf(cmd,MAXLLEN,"%s \"file://%s/%s\" >/dev/null  2>/dev/null &",cmd_ext+web_view,wd,fname_ns);
		else {
			if(f_vx==FILE_EXEC) { 
			wd[0]=0;
			strlcat(wd,cmd_ext,MAXLLEN);
			strlcat(wd," ",MAXLLEN);
			strlcat(wd,fname,MAXLLEN);
			MESG("use exec_shell [%s]",wd);
			exec_shell(wd);
			return true;
			};
			sstat=snprintf(cmd,MAXLLEN,"%s \"%s/%s\" >/dev/null  2>/dev/null &",cmd_ext,wd,fname_ns);
		};
//		MESG("exec:2 [%s]",cmd);
	};
	free(cmd_ext);
	if(err) {
		msg_line("Cannot open files in WSL partition with window programs");
		return (false);
	};
	// MESG("Execute: [%s]",cmd);
	if(sstat<MAXLLEN) status=sysexec(cmd);
	else { MESG("command truncated,cannot execute!");return FALSE;};
	if(is_compressed) unlink(wd);	/* remove temporary file  */
	if(status>0) return FALSE;
	else return TRUE;
}

/* directory command execution */
int dir_exec(int n) 
{
  int ftype;
  int status;
  char fname[512];
  // MESG("dir_exec:");
  if(!(cbfp->b_flag & FSNLIST)) return 0;
  /* No need for escaped characters here!.
	 The file name will finally be enclosed in double quotes
  */
  ftype=dir_getfile(fname,0); 	
  status=chdir(cbfp->b_dname);
  // MESG("dir_exec: ftype=%d",ftype);

  if(ftype==FTYPE_LINK || ftype<0) return FALSE;
  status=exec_ext(fname,fname,FILE_EXEC);
  return status;
}

/* external view */
int dir_right(int n) 
{
  int ftype;
  char fname[512];
  char fname_ns[512];
  int status=FALSE;

  MESG("dir_right: b_flag=%X",cbfp->b_flag);
  if(!(cbfp->b_flag & FSNLIST)) return false;

  ftype=dir_getfile(fname,1);
  if(ftype<0) return false;	/* no file in dir!  */
  ftype=dir_getfile(fname_ns,0); // we need this in case of internal viewing of file names with spaces
  // MESG("dir_right: [%s][%s] ftype=%d",fname,fname_ns,ftype);
  if(ftype<FTYPE_NORMAL) {
	return error_line("Cannot find file type of [%s]",fname_ns);
  };

  if(ftype==FTYPE_NORMAL) { // view the file
	cbfp->cdir->cline = getcline();

	cwp->w_ppline = window_cursor_line(cwp)+1;
	status=exec_ext(fname,fname_ns,FILE_VIEW);
  } else {  // move in directory
	status = movein_dir(fname_ns);
  };
  set_update(cwp,UPD_STATUS);
  return(status);
}

int script_exec(int nuse)
{
	int    s;		/* return status from CLI */
	FILEBUF *bp;	/* pointer to buffer to zot */
    char script_line[MAXLLEN];	/* command line send to shell */
	char script_cmd[MAXFLEN];
	static char out_file[MAXFLEN];
	static char err_file[MAXFLEN];
	char script_name[MAXFLEN];
	char dir_name[MAXFLEN];
	WINDP *win_ori=cwp;

	s=set_unique_tmp_file(out_file,"out",MAXFLEN);
	s=set_unique_tmp_file(err_file,"out",MAXFLEN);

	/* get the filter name and its args */
	set_list_type(LSHELL);
	script_cmd[0]=0;
    if ((s=nextarg("Script command > ", script_cmd, MAXFLEN,true)) != TRUE) return(s);

	/* setup the proper file names */
	bp = cbfp;

	strcpy(dir_name,bp->b_dname);
	escape_file_name(dir_name);

	if(bp->b_flag & FSDIRED) {
		dir_getfile(script_name,1);
//		MESG("dir: scrip=[%s]",script_name);
	} else {
		strcpy(script_name,bp->b_fname);
		escape_file_name(script_name);
//		MESG("file: scrip=[%s]",script_name);
	};

	if(snprintf(script_line,MAXLLEN,"%s %s/%s > %s 2>%s",
		script_cmd,dir_name,script_name,out_file,err_file)>=MAXLLEN) {
			msg_line("script name too long!");
			return FALSE;
		};
	// MESG("script_exec:[%s]",script_line);

	s=sysexec(script_line);

	if(!s)
	{
		unlink(out_file);
		unlink(err_file);
		return(FALSE);
	} else {
	/* reset file name */
		FILEBUF *out_buffer;
		FILEBUF *err_buffer;
		WINDP *filter_window;
		out_buffer = get_filebuf("[out]",NULL,0);
		if(out_buffer==NULL) {
			out_buffer=new_filebuf("[out]",0);
			split_window(1);
			next_window(1);
			select_filebuf(out_buffer);
			filter_window=cwp;
		} else { // find window with [filter] buffer and use it
			filter_window=find_buffer_window(out_buffer);
			if(filter_window) {
				cwp=filter_window;
			} else {
				next_window(1);
			}
		};
		if((err_buffer = new_filebuf("[error]",0))!=FALSE) {
			EmptyText(err_buffer);
		};
		
		select_filebuf(err_buffer);
		EmptyText(err_buffer);
		ifile(err_buffer,err_file,0);
		err_buffer->b_state &= ~FS_CHG;
		select_filebuf(out_buffer);
		EmptyText(out_buffer);
		ifile(out_buffer,out_file,1);
		out_buffer->b_state &= ~FS_CHG;

		set_hmark(1,"script_exec");
		set_update(cwp,UPD_EDIT+UPD_ALL);
		unlink(out_file);
		unlink(err_file);

		cwp = filter_window;
		select_filebuf(out_buffer);
		set_Offset(0);
		cwp = win_ori;
		select_filebuf(bp);
		next_window(1);
		set_update(cwp,UPD_EDIT+UPD_ALL);
//		MESG("filtered");
		msg_line("ok!");
		return(TRUE);
	};
}

FILEBUF *get_valid_buffer(FILEBUF *bf)
{
 FILEBUF *bp;
 FILEBUF *prev=NULL;
 // MESG("get_valide_buffer: 0");
	lbegin(file_list);
	while((bp=(FILEBUF *)lget(file_list))!=NULL)
	{
		if(bp==bf) {
			// MESG("	found buffer %s",bf->b_fname);
			return bf;
		};
	};
 // MESG("get_valide_buffer: 1");
	lbegin(file_list);
	while((bp=(FILEBUF *)lget(file_list))!=NULL)
	{
		if(bp->b_flag & FSINVS) continue;
		if(bp==cbfp) {
			if(prev!=NULL) {
				// MESG("	found prev buffer %s",prev->b_fname);
				return prev;
			} else {
				prev = (FILEBUF *)lget(file_list);
				return prev;
			};
		};
		prev=bp;
	};
	MESG("error!!");
	return NULL;
}

/* Find the appropriate directory buffer  */
FILEBUF *get_dir_buffer(int flag,int dnum)
{
 FILEBUF *bp=NULL;
 if(flag==DIR_NOTES) {
//	MESG("get destination notes buffer id=%d",dnum);
	lbegin(file_list);
	while((bp=(FILEBUF *)lget(file_list))!=NULL)
	{
		if(bp->dir_num==DIR_NOTES) {
			return(bp);
		}
	};
	return(NULL);
 };

 if(dnum==DIR_DEFAULT) {
	 dnum = cbfp->dir_num;

	 if(flag==DIR_OTHER){
	 	if(dnum==DIR_PRIMARY) dnum=DIR_SECONDARY;else dnum=DIR_PRIMARY;
		// is there an other window with a different buffer shown?
		lbegin(file_list);
		while((bp=(FILEBUF *)lget(file_list))!=NULL)
		{
			if(bp->dir_num>0 && bp->b_nwnd > 0) {
				if(bp->dir_num!=cbfp->dir_num){

					return(bp);
				};
			}
		};
		
	 } else { return NULL; };
  };

	// MESG("get destination buffer id=%d",dnum);
	lbegin(file_list);
	while((bp=(FILEBUF *)lget(file_list))!=NULL)
	{
		if(bp->dir_num==dnum) {
			MESG("found dir buffer:");
			return(bp);
		}
	};
	// MESG("destination not found!");
	return(NULL);
}

int view_next(int n)
{
#if	TNOTES
 if(cbfp->b_flag==FSNOTES) return(0);
#endif
 dir_left(1);
 // MESG("view_next: b_flag=%X",cbfp->b_flag);
 if(cbfp->b_flag & FSDIRED) {
 	dir_right(1);
	set_update(cwp,UPD_EDIT|UPD_STATUS);
 }
#if	TNOTES
 else
 if(cbfp->b_flag & FSNOTESN || cbfp->b_flag & FSNLIST) {
 	next_line(1);
	view_note(1);
	set_update(cwp,UPD_EDIT|UPD_STATUS);
 };
#endif
 return(1); 
}

int view_previous(int n)
{
#if	TNOTES
 if(cbfp->b_flag==FSNOTES) return(0);
#endif
 dir_left(1);
 // MESG("view_previous: b_flag=%X",cbfp->b_flag);
 prev_line(1);
 if(cbfp->b_flag & FSDIRED) {
	// MESG("view_previous: %d %d",cwp->current_note_line,cbfp->b_notes);
	// if(cwp->current_note_line<cbfp->b_notes-2)
 		prev_line(1);
 	dir_right(1);
	set_update(cwp,UPD_EDIT|UPD_STATUS);
 }
#if	TNOTES
 else 
 if(cbfp->b_flag & FSNOTESN || cbfp->b_flag & FSNLIST) {
 	view_note(1);
	set_update(cwp,UPD_EDIT|UPD_STATUS);
 };
#endif
 return(1); 
}

int dir_left(int n)
{
 FILEBUF *bf;
 FILEBUF *vbuf;
 int dir_num = cbfp->dir_num;
 int stat=0;

 if(cbfp->connect_buffer==NULL && cbfp->b_flag != FSNLIST+FSDIRED) return false;
#if	TNOTES
 if(IS_FLAG(cbfp->b_flag,FSNOTES)) return false;
 // MESG("dir_left: b_state=0x%X b_type=0x%X b_flag=0x%X",cbfp->b_state,cbfp->b_type,cbfp->b_flag);
 if(
 	IS_FLAG(cbfp->b_state,FS_VIEWA) &&
	// !(cbfp->b_flag & FSNLIST) &&
 	(IS_FLAG(cbfp->b_type,NOTE_TYPE)||
	 IS_FLAG(cbfp->b_type,NOTE_CAL_TYPE)||
	 IS_FLAG(cbfp->b_type,NOTE_TODO_TYPE)
	))
 {
	num top_line=0;
	num to_line=0;
	num to_col=0;

	// MESG("dir_left: quick_close");
	bf = cbfp->connect_buffer;
	to_line = cbfp->connect_line;
	to_col = cbfp->connect_column;
	top_line = cbfp->connect_top_line;
	int top_note = cbfp->connect_top_note;
	quick_close(1);
//	MESG("dir left: buffer [%s] line=%ld col=%ld",cwp->w_fp->b_fname,tp_line(cwp->tp_current),tp_col(cwp->tp_current));

//	We should select the notes tag view here !!
	// bf=get_filebuf("[Tag view]",NULL,0);
	select_filebuf(bf);
	// MESG("dir_left: line=%ld",to_line);
	cwp->current_tag_line = to_line;
	cwp->current_note_line = to_col;
	// MESG("dir_left: new note_line=%d %d",cwp->current_note_line,cbfp->b_notes);
	cwp->top_tag_line=top_line;
	cwp->top_note_line=top_note;
//	update_status();
	return (OK_CLRSL);
 };
#endif
 	if(!(cbfp->b_flag & FSNLIST)) 
	{ // we are in view mode!
		vbuf=cbfp;
		int connect_column=cbfp->connect_column;
		// MESG("try get valid buffer!");
		bf = get_valid_buffer(cbfp->connect_buffer);
		// MESG("dir_left: 1");
		if(bf==NULL) {
			// MESG("No buffer left!");
			return(false);
		};
		if(bf==NULL) {
			bf = new_filebuf(dir_name(dir_num),0);
		};
   		select_filebuf(bf);
		delete_filebuf(vbuf,1);	/* remove the view file buffer */
		if(bf->cdir!=NULL) {
			MoveLineCol(bf->cdir->cline,connect_column);
			next_line(1);
			// MESG("dir_left: new note_line=%d %d",cwp->current_note_line,cbfp->b_notes);
		};
		set_update(cwp,UPD_EDIT|UPD_STATUS);

		cbfp->b_state &= ~FS_CHG;
		return(OK_CLRSL);
	} else {
		bf = cbfp;
 	};
	if(bf->cdir!= bf->rdir) 
	{
		pop_dir(bf);
		pdline = bf->cdir->cline;
//		MESG("poped dir name=[%s]",bf->cdir->dir_name);
		if(bf->dir_num >= DIR_NOTES) {
			char *notes_dir = strstr(bf->cdir->dir_name,"Notes");
			sprintf(bf->b_fname,"[%s]",notes_dir);
//			MESG("pop new notes b_dname=[%s]",bf->b_dname);
//			MESG("pop new notes b_fname=[%s]",bf->b_fname);
		};
	} else {
		if(bf->dir_num >= DIR_NOTES) return (FALSE);
  		if(strcmp(bf->cdir->dir_name,"/")==0) { 
			msg_line("we are at root dir!");
			return(FALSE);
		};
  		if(chdir("..")!=0) {
			msg_line("cannot move to parent dir!");
			return(false);
		};
		msg_line(time2a());
		reinit_dir(bf);
	};
  set_update(cwp,UPD_EDIT|UPD_STATUS);
  stat = list_dir(bf->cdir->dir_name,bf);
  cwp->current_note_line=bf->cdir->cline+1;
  if(cwp->current_note_line > bf->dir_list_str->size-1) cwp->current_note_line=bf->dir_list_str->size-1;
  return(stat);
}

int dir_tag(int n) 
{
  if(!(cbfp->b_flag & FSNLIST)) return FALSE;
  istr **row_data = (istr **) array_data(cbfp->dir_list_str);
  istr *dir_str = row_data[cwp->current_note_line];
  if(dir_str->selected) dir_str->selected=0;
  else dir_str->selected=1;
  // MESG("dir_tag: %d",dir_str->selected);
  next_line(1);
  set_update(cwp,UPD_EDIT);
  return 1;
}

/* show current dir file info on window */
int dir_showfileinfo(int n) 
{
 return(OK_CLRSL);
}

void escape_file_name(char *fname)
{
 char efname[MAXFLEN];
 char *c;
 char *dest = &efname[0];
 int escaped=0;
 int len=0;
 for( c=fname; *c!=0 && len<MAXFLEN;c++,len++) {

  if(*c==0) break;
  if(*c=='\\') { *dest++ = *c; escaped=1;continue;};
  if(escaped) { *dest++ = *c; escaped=0;continue;};
  if(*c==' ') { *dest++ = '\\'; };	/* escape space  */
  if(*c=='(') { *dest++ = '\\'; };	/* escape left parenthesis  */
  if(*c==')') { *dest++ = '\\'; };	/* escape right parenthesis  */
  *dest++ = *c;
 };*dest=0;
 strlcpy(fname,efname,MAXFLEN);
}

char *get_line_at(FILEBUF *fb,offs offset);

/* get file name from current line */
/* if flag then add \ in front of space characters  */
int dir_getfile(char *fname,int flag) 
{
 offs s;
 char c;
 int ftype;
 int len;
 char *f;
 int stat_result=0;
 int is_link=0;
 static char f1[MAXFLEN];
 struct stat t;
 int perms=0;
 fname[0]=0;
 // MESG("dir_getfile:start f=%d note_line=%d",flag,cwp->current_note_line);

 char *line_str;
 if(cbfp->b_flag & FSNLIST) {
	// MESG("dir_getfile: size of dir=%d line %d",cbfp->dir_list_str->size,cwp->current_note_line);
	if(cbfp->dir_list_str->size==0) return -1;

	istr **row_data = (istr **)array_data(cbfp->dir_list_str);
	// MESG("dir_getfile: 2");
	if(cwp->current_note_line > cbfp->dir_list_str->size-1) return -1;
	istr *current_str = row_data[cwp->current_note_line];
	// MESG("current index=%d",current_str->index);
	line_str = &current_str->start;
	// MESG("line_str:[%s]",line_str); 
 } else
 	line_str=get_line_at(cbfp,Offset());

 // MESG("dir_getfile: b_flag=%X [%s]",cbfp->b_flag,line_str);  
 c=line_str[0];
 if(c=='c') { SYS_ERROR("cannot view c type files");return -1;};
 if(c=='#' ||c=='!'||c=='/') ftype=FTYPE_DIR;else ftype=FTYPE_NORMAL;
 c=line_str[1];
 if(c=='l') is_link=1;
 s=DIR_NAME_POSITION;
 len=strlen(line_str)-DIR_NAME_POSITION;
 f=f1;

 for(; len>0;len--) {
  c=line_str[s];
  if(c==0) break;
  if(c==' ' && line_str[s+3]=='>') break;
  if(c==' ' && (flag==1)) { *f++ = '\\';};	/* escape space  */
  if(c=='&' && (flag==1)) { *f++ = '\\';};	/* escape background flag */
  if(c=='(' && (flag==1)) { *f++ = '\\';};	/* escape background flag */
  if(c=='|' && (flag==1)) { *f++ = '\\';};	/* escape background flag */
  if(c==')' && (flag==1)) { *f++ = '\\';};	/* escape background flag */
  *f++ = c;s++;
 };
 *f=0;

 if(stat_result<0) {
	strlcpy(fname,f1,MAXFLEN);
	MESG("stat_result: <0 , return [%s]!",fname);
	return(1);
 };
 // MESG("get_file: [%s] is_link=%d",f1,is_link);
 if((flag>1 && flag<5) || is_link) 
 {
	stat_result=stat(f1,&t);
	// MESG("is_link: stat_result=%d",stat_result);
	if(stat_result==0 && flag>1 && flag<5)  {
		char s_perms[11];
		memset(s_perms,'-',10);
		// f=fname;
		s_perms[10]=0;
		if(t.st_mode & S_IRUSR) { f[1]='r';perms += 400;};
		if(t.st_mode & S_IWUSR) { f[2]='w';perms += 200;};
		if(t.st_mode & S_IXUSR) { f[3]='x';perms += 100;};
		if(t.st_mode & S_IRGRP) { f[4]='r';perms += 40;};
		if(t.st_mode & S_IWGRP) { f[5]='w';perms += 20;};
		if(t.st_mode & S_IXGRP) { f[6]='x';perms += 10;};
		if(t.st_mode & S_IROTH) { f[7]='r';perms += 4;};
		if(t.st_mode & S_IWOTH) { f[8]='w';perms += 2;};
		if(t.st_mode & S_IXOTH) { f[9]='x';perms += 1;};

		if(flag==2){
			snprintf(fname,MAXFLEN-strlen(fname),"%4d:",perms);
			snprintf(fname+strlen(fname),MAXFLEN-strlen(fname)," u=%3u g=%3u",t.st_uid,t.st_gid);
		};
		if(flag==3){
			struct passwd *pw;
			struct group *gr;
			pw = getpwuid(t.st_uid);
			gr = getgrgid(t.st_gid);
			snprintf(fname,MAXFLEN,"user %d %s, group %d %s",t.st_uid,pw->pw_name,t.st_gid,gr->gr_name);
		};
		if(flag==4){
			snprintf(fname,MAXFLEN,"%s %4d:",s_perms,perms);
		};
	} else {
	 	strlcpy(fname,f1,MAXFLEN);
	};
 } else {
 	strlcpy(fname,f1,MAXFLEN);
 };

 if(is_link)
 {
	if(S_ISDIR(t.st_mode)) return FTYPE_DIRLINK;
	else return FTYPE_NORMAL;
 };
 
 return(ftype); 
}

void view_file(char *fname)
{
 FILEBUF *view;
 // char cwd[MAXFLEN],*cwdp;
 // int dir_num=cbfp->dir_num;

 view = new_filebuf("[view]",FSMMAP);	/* use memory map for viewing, its faster */
 // MESG("view_file: set connect buffer: [%s]",cbfp->b_fname);
 view->connect_buffer = cbfp;
 view->connect_line = GetLine()+1;
 view->connect_column = GetCol();
 view->dir_num=cbfp->dir_num;

 // strlcpy(view->b_dname,cwdp,MAXFLEN);
 strlcpy(view->b_dname,cbfp->b_dname,MAXFLEN);
 set_working_dir(view->b_dname);
 select_filebuf(view);

// get a new view for every file, remove only if return to dir mode with left_dir
 file_read(cbfp,fname);

 goto_bof(1);
 set_goal_column(0,"view_file");
 set_update(cwp,UPD_STATUS);

 cbfp->b_flag |= FSDIRED ;
 cbfp->b_state |= FS_VIEW;
 cbfp->b_state &= ~FS_CHG;
 // cbfp->dir_num=dir_num;


}

 
void push_dir(FILEBUF *bf) 
{
 lpush(bf->cdir,bf->dir_list_stack);
 // MESG("push_dir: name=[%s] line=%d",bf->cdir->dir_name,bf->cdir->cline);
 bf->cdir=(dir_l *)malloc(sizeof(struct dir_l));

 bf->cdir->dir_name=getcwd(NULL,MAXFLEN);
 bf->cdir->cline=1;
}

void pop_dir(FILEBUF *bf)
{
 dir_l *old;
// MESG("pop_dir:");
 old=(dir_l *) lpop(bf->dir_list_stack);
 if(old!=NULL) {
	if(bf->cdir!=NULL) free(bf->cdir);
	bf->cdir=old;
 };
 msg_line(time2a());
 // MESG("pop_dir: name=[%s] line=%d",bf->cdir->dir_name,bf->cdir->cline);
}

void reinit_dir(FILEBUF *bf)
{
 if(bf->cdir->dir_name) free(bf->cdir->dir_name);
	bf->cdir = bf->rdir;
	bf->cdir->dir_name = getcwd(NULL,MAXFLEN);

	// set the new directory name to current dir!
	strlcpy(bf->b_dname,bf->cdir->dir_name,MAXFLEN);
//	MESG("reinit_dir: [%s]",bf->b_dname);
}

char *str_tfile(struct stat *t,char *file_name,int maxsize)
{
 static char str[256];
 char lname[MAXLLEN];
 char ssize[50];
 char mt='?';
 char mx1='-';
 struct tm *t1;
 int year;
 int d_month;
 int sstat=0;
 
 mt='~';
 lname[0]=0;
// MESG("[%30s] st_mode=%d ifdir=%d",file_name,t->st_mode & S_IFMT,S_IFDIR);
 if((t->st_mode & S_IFMT)==S_IFLNK) {
 	int i1;
	if(maxsize>100){
	memset(lname,0,MAXLLEN);
	i1=readlink(file_name,lname,MAXLLEN); 
	if(i1>0){
		lname[i1]=0;
		i1=stat(lname,t);
		if(i1<0) strlcat(lname," does not exist!",MAXLLEN);
	} else {
		snprintf(lname,MAXLLEN,"file error %d ...",errno);
	};
 	};mx1='l';
 };

 switch (t->st_mode & S_IFMT) {
 case S_IFLNK: mx1='l';break;
 case S_IFBLK: mt='b';break;
 case S_IFIFO: mt='f';break;
 case S_IFREG: mt='-';break;
 case S_IFDIR: 
 	if(maxsize>128) mt='#';else mt='d';
	break;
 case S_IFCHR: mt='c';break;
#if	LINUX | FREEBSD | SOLARIS
 case S_IFSOCK: mt='s';break;
#endif
 };

if((t->st_mode & S_IXUSR \
 || t->st_mode & S_IXGRP \
 || t->st_mode & S_IXOTH) && mx1=='-') mx1='x';

 if(current_sort_mode>7) t1 = localtime(&t->st_atime);
 else if(current_sort_mode>5) t1 = localtime(&t->st_ctime);
 else t1 = localtime(&t->st_mtime);
 year = t1->tm_year;
 if(year>99) year-=100;

#if	LINUX | FREEBSD | SOLARIS
 if(
// 	mt=='?' || 
 	mt=='s') strcpy(ssize,"0");
 else
#endif
 {
	if(t->st_size > 9999999999) {
		double fsize=(double)t->st_size/1000000000;
		snprintf(ssize,50,"%.1f G",fsize);
	} else 
	if(t->st_size > 9999999) {
		double fsize=(double)t->st_size/1000000;
		snprintf(ssize,50,"%.1f M",fsize);
	} else 
 	snprintf(ssize,50,"%llu",(unsigned long long)t->st_size);
 };
 d_month=t1->tm_mon;
 if(d_month>11) {
	d_month=12;
 };

 if(maxsize>100 && lname[0]!=0) 
 sstat=snprintf(str,maxsize,"%c%c %10s %02d %3s %2d %02d.%02d %s --> %s",
 	mt,mx1,ssize,t1->tm_mday,month[d_month],year,t1->tm_hour,t1->tm_min,file_name,lname);
 else
 sstat=snprintf(str,maxsize,"%c%c %10s %02d %3s %2d %02d.%02d %s",
 	mt,mx1,ssize,t1->tm_mday,month[d_month],year,t1->tm_hour,t1->tm_min,file_name);
 if(sstat>maxsize) MESG("truncated 11"); 
 return str;
}

// insert file list in an editor window
int insert_dir(FILEBUF *buf_dir,int retain)
{
 FILEBUF *oldbuf=cbfp;
 int i;
 struct kdirent *ff;
 struct kdirent **namelist=NULL;
 char *d1;
 char *fname;
 int files_in_dir;
 char *fline;
 int stat=0;

 cbfp=buf_dir;
 d1 = buf_dir->b_dname;
 // MESG("insert_dir:");
 alist *dir_list_str=new_list(0,"dir_as_list");
 if(buf_dir->cdir == NULL) { MESG("cdif is null");return 0;};
 buf_dir->cdir->dir_name = strdup(buf_dir->b_dname);


 EmptyText(buf_dir);
 set_hmark(1,"insert_dir");
 set_Offset(0);
 discmd=0;

 files_in_dir = scandir2(d1, &namelist);
 int dir_ind=0;
 for(i=0;i<files_in_dir;i++)
 {
	ff = namelist[i];
  	fname=ff->d_name;
	if(!strcmp(fname,".")) continue;
	if(!strcmp(fname,"..")) continue;
	istr *dir_istr;
	fline=str_tfile(&ff->t,fname,250);
	dir_istr = (istr *)malloc(sizeof(istr)+strlen(fline));
	memcpy(&dir_istr->start,fline,strlen(fline)+1);
	dir_istr->index = dir_ind++;
	dir_istr->selected=0;
#if	1
	add_element_to_list((void *)dir_istr,dir_list_str);
#else
	insert_string(buf_dir,fline,strlen(fline));
	if(i+1<files_in_dir) insert_newline(buf_dir);
#endif
	// MESG("[%s]",fline);
	free(namelist[i]);	
 };
 free(namelist);

 set_Offset(0);
 switch(cbfp->sort_mode){
	case 0:
	case -1: cbfp->b_header = " Perms   Size  Date          ↓Name                 ";break;
	case 1:  cbfp->b_header = " Perms   Size  Date          ↑Name                 ";break;
	case 2:  cbfp->b_header = " Perms  ↓Size  Date           Name                 ";break;
	case 3:  cbfp->b_header = " Perms  ↑Size  Date           Name                 ";break;
	case 4:  
	case 6:  
	case 8:  cbfp->b_header = " Perms   Size ↓Date           Name                 ";break;
	case 5:
	case 7:
	case 9:  cbfp->b_header = " Perms   Size ↑Date           Name                 ";break;
	
	default:
		cbfp->b_header = " Perms  Size  Date            Name                 ";
 }; 
 cbfp->b_flag = FSNLIST|FSDIRED;
 cbfp->b_state = FS_VIEW|FS_ACTIVE;
 cbfp->dir_list_str = dir_list_str;
 cbfp->b_notes = cbfp->dir_list_str->size;

 if(cbfp->dir_num==0) cbfp->dir_num=1;
 discmd=true;

 set_highlight(buf_dir,highlight_type("DIR"));

 set_update(cwp,UPD_WINDOW);
 stat=chdir(cbfp->b_dname) ;

 cbfp=oldbuf;
 if(stat) return (FALSE);
 return(TRUE);
}

int listdir(int dtype)
{ 
 int stat;
 char go_name[MAXFLEN];

 if(is_scratch_buffer(cbfp)) {
 	if(chdir(get_start_dir())) return false;
 };
 set_bfname(go_name,".");
 if(cbfp->sort_mode==-1) cbfp->sort_mode=0;
 current_sort_mode=cbfp->sort_mode;
 stat=goto_dir(go_name,dtype);
 set_hmark(0,"dir_name");

 return stat;
}

int listdir1()
{
 int stat=0;
 stat=listdir(1);

 return(stat);
}

int listdir2()
{
 int stat=0;
 stat=listdir(2);

 return(stat);
}

/*
	open_dir using the same buffer
*/
int open_dir(char *full_name,int type)
{
 int stat=0;
#if	TNOTES
 if(type == DIR_NOTES) {
		FILEBUF *bp;
		bp=get_dir_buffer(DIR_NOTES,DIR_DEFAULT);
		if(bp==NULL) { 	/* create a new dir buffer as 1 !  */
			bp = new_filebuf(dir_name(1),FSDIRED);

		};
		strlcpy(bp->b_dname,full_name,MAXFLEN);
		bp->b_flag |= FSDIRED;
		bp->dir_num=DIR_NOTES;
//		MESG("notes dir b_dname=[%s] full_name=[%s]",bp->b_dname,full_name);
		strlcpy(bp->b_fname,"[Notes]",MAXFLEN);
		stat=insert_dir(bp,0);
		select_filebuf(bp);
		return(stat);
 };
#endif
// MESG("open_dir: dir_num=%d",cbfp->dir_num);
	if(cbfp->dir_num!=0) { 	/* we are in dir mode, use this buffer!  */
		strlcpy(cbfp->b_dname,full_name,MAXFLEN);
		stat=insert_dir(cbfp,0);
		return stat;
	} else {
		FILEBUF *bp;
		bp=get_dir_buffer(DIR_CURRENT,0);
		if(bp==NULL) { 	/* create a new dir buffer as 1 !  */
			bp = new_filebuf(dir_name(1),FSDIRED);
		};
		strlcpy(bp->b_dname,full_name,MAXFLEN);
		bp->b_flag |= FSDIRED;
		strlcpy(bp->b_fname,"[dir2]",MAXFLEN);
		stat=insert_dir(bp,0);
		select_filebuf(bp);
		return(stat);
	};
}

int goto_dir(char *full_name,int dir_num)
{
 int err=0;
 FILEBUF *bp=NULL;
	bp=get_dir_buffer(DIR_CURRENT,dir_num);
	if(bp==NULL || bp->dir_num != dir_num) {
		bp=new_filebuf(dir_name(dir_num),FSDIRED);
		bp->dir_num=dir_num;
	};
	err=insert_dir(bp,0);
	select_filebuf(bp);
	set_goal_column(0,"goto_dir");
	MoveLineCol(pdline+1,DIR_NAME_POSITION);
	return(err);
}

int list_dir(char *d1,FILEBUF *buf_dir)
{
 int stat;
 strlcpy(buf_dir->b_dname,d1,MAXFLEN);
 if(buf_dir!=cbfp) {
 	stat=select_filebuf(buf_dir);
 } else {
 	stat=insert_dir(buf_dir,0);
	set_goal_column(0,"list_dir");
//	MoveLineCol(pdline+1,DIR_NAME_POSITION);
 }
 return stat;
}

/* ---- */
