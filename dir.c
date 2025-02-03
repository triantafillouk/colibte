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
#include	<dirent.h>
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
extern int drv_initialized;

#define	SORT_FUNC	0	/* Use different functions for dir sorting  */
#define MAXSTAT	100000	/* Do not sort dir if more files  */

/* local function declarations */
int list_dir(char *dir_name,FILEBUF *fp);

#if	SORT_FUNC
typedef int (*DIR_CMPF)(struct kdirent *a,struct kdirent *b);
#endif

void qsort_dir(struct kdirent **arr, int elements,int sort_mode);
char **getdir(char *dirname,char *s_find,int *num);
char *str_efile(struct kdirent *entry);
char *str_tfile(struct stat *t,char *file_name,int maxsize);
FILEBUF *get_dir_buffer(int flag,int dir_num);
int dir_other_reload(num n);
void escape_file_name(char *fname);
void delete_current_list_line();
int stricmp1(const char *str1, const char *str2);
int dir_getfile1(FILEBUF *fp,char *fname,int flag,int file_index) ;

int pdline=-1;
int current_sort_mode=0;

typedef       int     (*Fintss)(const char *, const char *);

 /* This is used in dired mode dates */
char *month[] = { "Jan", "Feb","Mar","Apr", "May","Jun","Jul","Aug","Sep","Oct","Nov","Dec","error",NULL };

#if	SORT_FUNC
typedef int (*DIR_CMPF)(struct kdirent *a,struct kdirent *b);

static int dir_cmp_name(struct kdirent *a,struct kdirent *b)
{
// first the directories !
 if (((a->st_mode & S_IFMT) != S_IFDIR) && ((b->st_mode & S_IFMT) == S_IFDIR)) return 1;
 if (((a->st_mode & S_IFMT) == S_IFDIR) && ((b->st_mode & S_IFMT) != S_IFDIR)) return -1;
 return(stricmp1((const char *) a->d_name,(const char *) b->d_name));
}

static int dir_cmp_name_rev(struct kdirent *a,struct kdirent *b)
{
// first the directories !
 if (((a->st_mode & S_IFMT) != S_IFDIR) && ((b->st_mode & S_IFMT) == S_IFDIR)) return 1;
 if (((a->st_mode & S_IFMT) == S_IFDIR) && ((b->st_mode & S_IFMT) != S_IFDIR)) return -1;
 return(stricmp1((const char *) b->d_name,(const char *) a->d_name));
}

static int dir_cmp_size(struct kdirent *a,struct kdirent *b)
{
// first the directories !
 if (((a->st_mode & S_IFMT) != S_IFDIR) && ((b->st_mode & S_IFMT) == S_IFDIR)) return 1;
 if (((a->st_mode & S_IFMT) == S_IFDIR) && ((b->st_mode & S_IFMT) != S_IFDIR)) return -1;
 return(a->st_size > b->st_size ? 1:-1);	/* modification time  */
}

static int dir_cmp_size_rev(struct kdirent *a,struct kdirent *b)
{
// first the directories !
 if (((a->st_mode & S_IFMT) != S_IFDIR) && ((b->st_mode & S_IFMT) == S_IFDIR)) return 1;
 if (((a->st_mode & S_IFMT) == S_IFDIR) && ((b->st_mode & S_IFMT) != S_IFDIR)) return -1;
 return(a->st_size < b->st_size ? 1:-1);	/* modification time  */
}

static int dir_cmp_mtime(struct kdirent *a,struct kdirent *b)
{
// first the directories !
 if (((a->st_mode & S_IFMT) != S_IFDIR) && ((b->st_mode & S_IFMT) == S_IFDIR)) return 1;
 if (((a->st_mode & S_IFMT) == S_IFDIR) && ((b->st_mode & S_IFMT) != S_IFDIR)) return -1;
 return(a->mtime > b->mtime? 1:-1);	/* modification time  */
}

static int dir_cmp_mtime_rev(struct kdirent *a,struct kdirent *b)
{
// first the directories !
 if (((a->st_mode & S_IFMT) != S_IFDIR) && ((b->st_mode & S_IFMT) == S_IFDIR)) return 1;
 if (((a->st_mode & S_IFMT) == S_IFDIR) && ((b->st_mode & S_IFMT) != S_IFDIR)) return -1;
 return(a->mtime < b->mtime? 1:-1);	/* modification time  */
}

static int dir_cmp_atime(struct kdirent *a,struct kdirent *b)
{
// first the directories !
 if (((a->st_mode & S_IFMT) != S_IFDIR) && ((b->st_mode & S_IFMT) == S_IFDIR)) return 1;
 if (((a->st_mode & S_IFMT) == S_IFDIR) && ((b->st_mode & S_IFMT) != S_IFDIR)) return -1;
 return(a->atime > b->atime? 1:-1);	/* modification time  */
}

static int dir_cmp_atime_rev(struct kdirent *a,struct kdirent *b)
{
// first the directories !
 if (((a->st_mode & S_IFMT) != S_IFDIR) && ((b->st_mode & S_IFMT) == S_IFDIR)) return 1;
 if (((a->st_mode & S_IFMT) == S_IFDIR) && ((b->st_mode & S_IFMT) != S_IFDIR)) return -1;
 return(a->atime < b->atime? 1:-1);	/* modification time  */
}

static int dir_cmp_ctime(struct kdirent *a,struct kdirent *b)
{
// first the directories !
 if (((a->st_mode & S_IFMT) != S_IFDIR) && ((b->st_mode & S_IFMT) == S_IFDIR)) return 1;
 if (((a->st_mode & S_IFMT) == S_IFDIR) && ((b->st_mode & S_IFMT) != S_IFDIR)) return -1;
 return(a->ctime > b->ctime? 1:-1);	/* modification time  */
}

static int dir_cmp_ctime_rev(struct kdirent *a,struct kdirent *b)
{
// first the directories !
 if (((a->st_mode & S_IFMT) != S_IFDIR) && ((b->st_mode & S_IFMT) == S_IFDIR)) return 1;
 if (((a->st_mode & S_IFMT) == S_IFDIR) && ((b->st_mode & S_IFMT) != S_IFDIR)) return -1;
 return(a->ctime < b->ctime? 1:-1);	/* modification time  */
}

DIR_CMPF dir_cmp_array [] = {
	dir_cmp_name,dir_cmp_name_rev,
	dir_cmp_size,dir_cmp_size_rev,
	dir_cmp_mtime,dir_cmp_mtime_rev,
	dir_cmp_atime,dir_cmp_atime_rev,
	dir_cmp_ctime,dir_cmp_ctime_rev
};
#endif

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
		error_line("cannot create .%s dir in home dir",APPLICATION_NAME);
	} else {
		// MESG("created .%s dir in home dir",APPLICATION_NAME);
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

#if	!SORT_FUNC
// directory compare, first the directories, then the files
static inline int dir_cmp(struct kdirent *a, struct kdirent *b,int sort_mode)
{
#if	1
// first the directories !
 if (((a->st_mode & S_IFMT) != S_IFDIR) && ((b->st_mode & S_IFMT) == S_IFDIR)) return 1;
 if (((a->st_mode & S_IFMT) == S_IFDIR) && ((b->st_mode & S_IFMT) != S_IFDIR)) return -1;
#else
// categorize by file mode
 if( a->st_mode != b->st_mode) {
	return(a->st_mode > b->st_mode? 1:-1);
 };
#endif
 switch(sort_mode){
	case 0: return(stricmp1((const char *) a->d_name,(const char *) b->d_name));
	case 1: return(stricmp1((const char *) b->d_name,(const char *) a->d_name));
	case 2: return(a->st_size  > b->st_size? 1:-1);
	case 3: return(a->st_size  < b->st_size? 1:-1);
	case 4: return(a->mtime > b->mtime? 1:-1);	/* modification time  */
	case 5: return(a->mtime < b->mtime? 1:-1);	/* reverse modification time  */
	case 6: return(a->ctime > b->ctime? 1:-1);	/* status change time  */
	case 7: return(a->ctime < b->ctime? 1:-1);	/* status change reverse time  */
	case 8: return(a->atime > b->atime? 1:-1);	/* access time  */
	case 9: return(a->atime < b->atime? 1:-1);	/* reverse access time  */
 };
 return 0;
}
#endif


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

int set_sort_mode(num mode)
{
 int old_sort_mode=current_sort_mode;
 int current_sort_mode=cwp->w_fp->sort_mode;
 // MESG("set_sort_mode: current=%d mode=%d",current_sort_mode,mode);
 if(!(cwp->w_fp->b_flag & FSDIRED)) return true;

	if(mode==-1 || current_sort_mode==mode) {
		if((current_sort_mode%2)==0) current_sort_mode++;
		else current_sort_mode--;
	} else current_sort_mode=mode;

	cwp->w_fp->sort_mode=current_sort_mode;
	// MESG("	new sort mode=%d",current_sort_mode);
	if(current_sort_mode != old_sort_mode) 
	{
		dir_reload(1);
	};
	return true;
}

int change_sort_mode(int mouse_col)
{
	if(mouse_col>7 && mouse_col<15) {
		// MESG("sort by size");
		set_sort_mode(2);
		return 1;
	// set/toggle sort by size 
	};  
	if(mouse_col>15 && mouse_col<30) {
		// set/toggle sort by date
		// MESG("sort by date");
		set_sort_mode(4);
		return 1;
 	};
	if(mouse_col>30) {
		// MESG("sort by name");
		// set/toggle sort by name
		set_sort_mode(0);
		return 1;
	};
	return 0;
}

int update_line(int size)
{
	istr **row_data = (istr **)array_data(cbfp->dir_list_str);
	istr *current_str = row_data[cwp->current_note_line];
	char *line_str = &current_str->start;
	// MESG("update_line: [%s]",line_str);
	char num[16];
	sprintf(num,"%d",size);
	memset(line_str+3,' ',10);
	memcpy(line_str+(13-strlen(num)),num,strlen(num));
	// MESG("update_line: [%s]",line_str);
	set_update(cwp,UPD_EDIT);
	return 0;
}

int update_uline(char *s)
{
	istr **row_data = (istr **)array_data(cbfp->dir_list_str);
	istr *current_str = row_data[cwp->current_note_line];
	char *line_str = &current_str->start;
	// MESG("update_line: [%s]",line_str);
	memset(line_str+3,' ',10);
	memcpy(line_str+(13-strlen(s)),s,strlen(s));
	// MESG("update_line: [%s]",line_str);
	set_update(cwp,UPD_EDIT);
	return 0;
}

// count dir size
int dir_count_files(num n)
{
 int num_of_files=0; /* no of files in directory */
 DIR *d1;
 struct dirent *df1;
 char dir_name[MAXFLEN];
 int ftype = dir_getfile(dir_name,0);
 msg_line("check file count");
 // MESG("ftype=%d [%s]",ftype,dir_name);
 if(ftype==FTYPE_DIR) {
 d1 = opendir(dir_name);
 if(d1==NULL) { return error_line("cant open dir %s",dir_name);return 0;};
 // Find how many files in the dir
 while((df1=readdir(d1))!=NULL) {
 	num_of_files++;
	if((num_of_files%500000)==0) msg_line("count %d",num_of_files); 
 };
 closedir(d1);
 num_of_files-=2;
 // MESG("dir size is %d",num_of_files);
 update_line(num_of_files);
 msg_line("files in dir are %d",num_of_files);
 };
 return num_of_files; 
}

int dir_size(num n)
{
 int num_of_files=0; /* no of files in directory */
 char dir_name[MAXFLEN];
 char tmp_name[MAXFLEN];
 int ftype = dir_getfile(dir_name,0);
 FILE *fid;
 // MESG("ftype=%d [%s]",ftype,dir_name);
 if(ftype==FTYPE_DIR) {
  char sline[MAXLLEN];
  msg_line("checking dir size ...");
  escape_file_name(dir_name);
  // MESG("du %s",dir_name);
  set_unique_tmp_file(tmp_name,"du",MAXFLEN);
  // MESG("results to %s",tmp_name);
  if(snprintf(sline,sizeof(sline),"du -sh %s > %s 2>/dev/null",dir_name,tmp_name)>=sizeof(sline)){
  	msg_line("command %s too long!",sline);
	return(FALSE);
  };
  int status=system(sline);
  if(status==0) {
	fid=fopen(tmp_name,"r");
	int num=fscanf(fid,"%s %s",sline,dir_name);
	fclose(fid);
	unlink(tmp_name);
	// MESG("%s %s",sline,dir_name);
	if(num<1) { 
		msg_line("cannot check!");
		return(false);
	} else { 
		msg_line("dir size %s",sline);
		update_uline(sline);
	};
  } else {
  	msg_line("du error!");
  }
 };
 return num_of_files; 
}


/* this is a local scandir. Not all operating systems use the BSD one! */
int scandir2(char *dirname, struct kdirent ***namelist_a)
{
 int num_of_files=0; /* no of files in directory */
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

 // Find how many files in the dir
 while((df1=readdir(d1))!=NULL) num_of_files++;
 namelist = (struct kdirent **)malloc(((num_of_files+1)*sizeof(struct kdirent *)));
 // MESG("number_of_files %ld size %d",num_of_files,((num_of_files+1)*sizeof(struct kdirent *)));
 rewinddir(d1);
 // show_time("scan_dir: start",0);
 for(i=0;;i++) {
 	df1=readdir(d1);

	if(df1==NULL) { closedir(d1);break;};
	// MESG(" - %3d: %5d l=%d t=%d %d %d [%s]",i,df1->d_ino,df1->d_reclen,df1->d_type,sizeof(struct stat),sizeof(struct kdirent),df1->d_name);
	// MESG(" - %3d: %5d l=%d t=%d 0x%X 0x%X [%s]",i,df1->d_ino,df1->d_reclen,df1->d_type,DT_DIR << 12,S_IFDIR,df1->d_name);
	int len=strlen(df1->d_name)+1;
	namelist[i]=(struct kdirent *)malloc(sizeof(struct kdirent)+len);
	if(namelist[i]==NULL) {
			closedir(d1);
			return(0);
	};
	strlcpy(namelist[i]->d_name,df1->d_name,len);
#if	!SOLARIS
	if(num_of_files>MAXSTAT) {
		namelist[i]->st_mode=df1->d_type << 12;
		namelist[i]->st_size=len;
		namelist[i]->mtime=0;
		namelist[i]->atime=0;
		namelist[i]->ctime=0;
	} else 
#endif
	{	
		result=lstat(namelist[i]->d_name,&t);
		if(result) {
			ERROR("[%30s]Error    %d ",namelist[i]->d_name,errno);
			namelist[i]->st_mode=0;
			namelist[i]->st_size=0;
			namelist[i]->mtime=0;
			namelist[i]->atime=0;
			namelist[i]->ctime=0;
		} else {
			namelist[i]->st_mode  = t.st_mode;
			namelist[i]->mtime    = t.st_mtime;
			namelist[i]->ctime    = t.st_ctime;
			namelist[i]->atime    = t.st_atime;
			namelist[i]->st_size  = t.st_size;
		};
	}
 } ;
 namelist[i]=NULL;
 if(num_of_files<MAXSTAT)qsort_dir(namelist,num_of_files,current_sort_mode);
 else msg_line("dir too big to sort contains %d files",num_of_files);

 *namelist_a = namelist;
 return(num_of_files);
}

// quick sort a directory list
void qsort_dir(struct kdirent **arr, int elements,int s_mode) 
{
  struct kdirent  *piv;
  int *beg, *end;
  int i=0, L, R ;
#if	SORT_FUNC
  DIR_CMPF cmp_dir = dir_cmp_array[current_sort_mode];
#endif
 // MESG("qsort_dir: sort_mode=%d elements=%d",s_mode,elements);
  beg=(int *)malloc(sizeof(int)*(elements+1));
  end=(int *)malloc(sizeof(int)*(elements+1));
  beg[0]=0; end[0]=elements;
  while (i>=0) {
    L=beg[i]; R=end[i]-1;
    if (L<R) {
      piv=arr[L]; 
      while (L<R) {
        while (
#if	SORT_FUNC
			cmp_dir(arr[R],piv) >= 0
#else
			dir_cmp( arr[R], piv,s_mode) >= 0
#endif
			&& L<R) R--; 
		if (L<R) arr[L++]=arr[R];
        while (
#if	SORT_FUNC
			cmp_dir( arr[L], piv) <= 0
#else
			dir_cmp( arr[L], piv,s_mode) <= 0
#endif
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

int dir_edit(num n) 
{
  char fname[MAXFLEN];

  // MESG("dir_edit: b_flag=%X b_type=%d",cbfp->b_flag,cbfp->b_type);
  if(!(cbfp->b_flag & FSNLIST))
  {
	// int old_type=cbfp->b_type;
	if(cbfp->b_flag & FSINVS) return(TRUE);
	if(!IS_FLAG(cbfp->b_state,FS_VIEWA)) return (TRUE);

	// MESG("file[%s / %s -> %s",cbfp->b_dname,cbfp->b_fname,fname);
    num poffset=tp_offset(cwp->tp_current);;
	int ppline = tp_line(cwp->tp_current)-tp_line(cwp->tp_hline)+1;
	if(snprintf(fname,MAXFLEN,"%s/%s",cbfp->b_dname,cbfp->b_fname) >= MAXFLEN) {
		return(FALSE);
	};
	// MESG("dir_edit: [%s / %s]",cbfp->b_dname,cbfp->b_fname);
	if(empty_filebuf(cbfp)!=TRUE) return FALSE;
	cbfp->b_flag &= ~(FSDIRED|FSMMAP);
	
	 if(! ifile(cbfp,fname,0)) return(FALSE);
 
	igotooffset(poffset,ppline);
	cbfp->b_state &= ~FS_CHG;
	set_update(cwp,UPD_FULL);
	set_hmark(1,"dir_edit:a");
	return TRUE;
  } else {
	int status=FALSE;
	char dname[MAXFLEN];
	int ftype;

	ftype=dir_getfile(fname,0);
	// MESG("dir_edit: ftype=%d fname=[%s]",ftype,fname);
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

// copy file, for the moment no questions asked!
int copy_1file(char *fname,char *destination,int ftype)
{
  struct stat t;
  int s1=stat(destination,&t);
  int destination_is_dir=0;
  char sline[MAXLLEN];
#if	DARWIN
  char *cp_flags="-pcf";
  if (ftype == FTYPE_DIR) cp_flags="-rcpf";
#else
  char *cp_flags="-pf";
  if (ftype == FTYPE_DIR) cp_flags="-rpf";
#endif
  // MESG("dir_copy:[%s] status=%d",sline,s1);
  if(s1==0) {	/* it exists!  */
  	s1=1;
	destination_is_dir = (t.st_mode & S_IFMT) == S_IFDIR;
	if(!destination_is_dir) cp_flags="-pf";
  } else s1=0;

  escape_file_name(fname);
  if(snprintf(sline,MAXLLEN,"cp %s %s %s 2> /dev/null",
  	cp_flags,fname,destination)>=MAXLLEN) {
		msg_line("File name too long!");
		return(FALSE);
	};

  // MESG("dir_copy1:sline=[%s]",sline);
  if((s1=(system(sline))) !=0 ) { 
	return error_line("Error copying %s to %s status=%d",fname,destination,s1);
  };
  return(true);
}

int move_1file(char *fname,char *destination)
{
  // struct stat t;
  // int s1=stat(destination,&t);
  int s1=0;

  char sline[MAXLLEN];

  // MESG("nove_1file: [%s] -> [%s] status=%d",fname,destination,s1);
  escape_file_name(fname);

  if(snprintf(sline,MAXLLEN,"mv %s %s 2> /dev/null",
  	fname,destination)>=MAXLLEN) {
		msg_line("move command too long!");
		return(FALSE);
	};

  // MESG("move_1file:sline=[%s]",sline);
  s1=(system(sline));
  // MESG("	status %d",s1);
  return(s1);
}

int dir_compare(num n)
{
 if(!(cbfp->b_flag & FSNLIST)) return FALSE;
 FILEBUF *f1=cbfp;
 FILEBUF *f2=get_dir_buffer(DIR_OTHER,0);
 if(f2==NULL) return false;
 if(!(f2->b_flag & FSNLIST)) return FALSE;
 char name1[MAXFLEN];
 char name2[MAXFLEN];
 if(f1!=f2) {
 	if(f2->sort_mode !=f1->sort_mode) return false;
	msg_line("dir compare..");
	int i,j;
	int ftype1=0;
	int ftype2=0;
	f1->selected_files=0;
	f2->selected_files=0;
	istr **r1=(istr **) array_data(f1->dir_list_str);
	istr **r2=(istr **) array_data(f2->dir_list_str);
	for(i=0;i<f1->dir_list_str->size;i++) {
		istr *d1=r1[i];
		d1->selection_tag=0;
		ftype1 = dir_getfile1(f1,name1,1,i);
		// MESG("	check i=%3 %s",i,f1);
		for(j=0;j<f2->dir_list_str->size;j++) {
			istr *d2=r2[j];
			ftype2 = dir_getfile1(f2,name2,1,j);
			// MESG("	with j=%d %s",j,f2);
			if((strcmp(name1,name2)==0) && (ftype1==ftype2)) {
				d1->selection_tag=1;
				f1->selected_files++;
				d2->selection_tag=1;
				f2->selected_files++;
				break;
			};
		};
	};
	set_update(cwp,UPD_ALL);
	msg_line("%d similar files!",f1->selected_files);
 } else return false;
 return true;
}

#if	0
int compare_files(char *name1,char *name2,int type)
{
	return strcmp(name1,name2);
}
#endif

int dir_show_diffs(num n)
{
 if(!(cbfp->b_flag & FSNLIST)) return FALSE;
 FILEBUF *f1=cbfp;
 FILEBUF *f2=get_dir_buffer(DIR_OTHER,0);
 if(f2==NULL) return false;
 if(!(f2->b_flag & FSNLIST)) return FALSE;
 char name1[MAXFLEN];
 char name2[MAXFLEN];
 if(f1!=f2) {
 	if(f2->sort_mode !=f1->sort_mode) return false;
	msg_line("dir find diffs..");
	int i,j;
	int ftype1=0;
	int ftype2=0;
	f1->selected_files=f1->dir_list_str->size;
	f2->selected_files=f2->dir_list_str->size;
	istr **r1=(istr **) array_data(f1->dir_list_str);
	istr **r2=(istr **) array_data(f2->dir_list_str);
	for(j=0;j<f2->dir_list_str->size;j++) {
		r2[j]->selection_tag=1;
	};
	for(i=0;i<f1->dir_list_str->size;i++) {
		istr *d1=r1[i];
		d1->selection_tag=1;
		ftype1 = dir_getfile1(f1,name1,1,i);
		// MESG("	check i=%3 %s",i,f1);
		for(j=0;j<f2->dir_list_str->size;j++) {
			istr *d2=r2[j];
			ftype2 = dir_getfile1(f2,name2,1,j);
			// MESG("	with j=%d %s",j,f2);
			if((strcmp(name1,name2)==0) && (ftype1==ftype2)) {
				d1->selection_tag=0;
				f1->selected_files--;
				d2->selection_tag=0;
				f2->selected_files--;
				break;
			};
		};
	};
	set_update(cwp,UPD_ALL);
	msg_line("%d %d different files!",f1->selected_files,f2->selected_files);
 } else return false;
 return true;
}

/* simple copy */
int dir_copy(num n)
{
  // struct stat t;
  int s;
  int is_other_dir=0;
  int ftype=0;
  int sstat=0;
  char destination[MAXFLEN];
  char destination_escaped[MAXFLEN];
  char fname[MAXFLEN];

  // MESG("dir_multi_copy:-----------------------------");
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

  if(cbfp->selected_files==0) {
	ftype=dir_getfile(fname,1);
	sstat=snprintf(sconfirm,MAXLLEN,"Copy [%s] to: ",fname);
  } else {
	sstat=snprintf(sconfirm,MAXLLEN,"Copy selected to: ");
  };
  if(sstat>=MAXLLEN) MESG("truncated 7");

  if((s = nextarg(sconfirm,destination,MAXFLEN,true)) !=TRUE) return(s);

  strlcpy(destination_escaped,destination,MAXFLEN);

  s1=chdir(cbfp->b_dname);
  // MESG("current dir is [%s]",getcwd(NULL,MAXFLEN));
  // MESG(" buffer dir is [%s]",cbfp->b_dname);
  escape_file_name(destination_escaped);


// ----- start

  if(cbfp->selected_files==0) {
    escape_file_name(fname);
	s1=copy_1file(fname,destination_escaped,ftype);
  } else {
	int i;
	istr **row_data = (istr **) array_data(cbfp->dir_list_str);
	s1=0;
 	for(i=0;i<cbfp->dir_list_str->size;i++) {
		istr *dir_str = row_data[i];
		if(dir_str->selection_tag) {
			ftype = dir_getfile1(cbfp,fname,1,i);
			// MESG("copy line %d name=%s type=%d",i,fname,ftype);
			if(ftype>=0) {
				s1+=copy_1file(fname,destination_escaped,ftype);
				dir_str->selection_tag=0;
				cbfp->selected_files--;
			}
		};
	};
  };
// ---- end

//	MESG("result status =%d",s1);
	if(strstr(destination,"/")!=NULL) is_other_dir=1;
	s1=chdir(cbfp->b_dname);
	if(destination_is_dir || is_other_dir) {
//		cbfp->b_state &= ~FS_CHG;
		dir_other_reload(1);
		return(TRUE);
	} else dir_reload(1);
  if(s1==1) msg_line("1 file copied");
  if(s1>1) msg_line("%d files copied",s1);
  return(TRUE);
}

/* simple rename/move */
int dir_move(num n) 
{
  // struct stat t;
  int s1;
  int s2;
  int is_other_dir=0;
  int destination_is_dir=0;
  char sconfirm[MAXLLEN];
  char destination[MAXFLEN],fname[MAXFLEN];
  FILEBUF *dbuf;	/* destination dir buffer  */
  int sstat=0;

  if(!(cbfp->b_flag & FSNLIST)) return FALSE;

  set_list_type(LDIR2);
  dbuf=get_dir_buffer(DIR_OTHER,0);	/* get destination dir buffer  */
  if(dbuf!=NULL) {
  	strlcpy(destination,dbuf->b_dname,MAXFLEN);
  } else {
  	destination[0]=0;
  };
  // MESG("selected files=%d",cbfp->selected_files);
  int files_to_move=cbfp->selected_files;
  if(cbfp->selected_files==0) {
	if(dir_getfile(fname,1)<0) return FALSE;
	sstat=snprintf(sconfirm,MAXLLEN,"Move [%s] to: ",fname);
	if(sstat>=MAXLLEN) { error_line("move prompt truncated");return(false);}
	if((s1 = nextarg(sconfirm,destination,MAXFLEN,true)) !=TRUE) return(s1);
	escape_file_name(destination);
	s2 = move_1file(fname,destination);
	if(s2 != 0) { return error_line("Error moving %s to %s",fname,destination);};
  } else {
	sstat=snprintf(sconfirm,MAXLLEN,"Move selected to: ");
	if(sstat>=MAXLLEN) { error_line("move prompt truncated");return(false);}
	if((s1 = nextarg(sconfirm,destination,MAXFLEN,true)) !=TRUE) return(s1);
	escape_file_name(destination);
	int i;
	istr **row_data = (istr **) array_data(cbfp->dir_list_str);
	s1=0;
 	for(i=0;i<cbfp->dir_list_str->size;i++) {
		istr *dir_str = row_data[i];
		if(dir_str->selection_tag) {
			int ftype = dir_getfile1(cbfp,fname,1,i);
			// MESG("move file in line %d name=%s type=%d",i,fname,ftype);
			if(ftype>=0) {
				s2=move_1file(fname,destination);
				if(s2==0) {
					cbfp->selected_files--;
					dir_str->selection_tag=0;
				};
			}
		};
	};
	if(files_to_move==cbfp->selected_files) s2=files_to_move;else s2=0;
	if(cbfp->selected_files>0) { error_line("Cannot move %d files",cbfp->selected_files);};
  };
  {
	// check if moved to other dir
	if(strstr(destination,"/")!=NULL) is_other_dir=1;
	if(destination_is_dir || is_other_dir) {
		// MESG("delete current line!");
		if(s2==0 && files_to_move==0) delete_current_list_line();
		else dir_reload(1);
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
int dir_file_rename(num n) 
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

int dir_touch_file(num n)
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

int dir_new_dir(num n)
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

int dir_link(num n)
{
  int s1;
  int s2;
  char sconfirm[MAXLLEN];
  char destination[MAXFLEN],fname[MAXFLEN],sline[MAXLLEN];
  char current_dir[MAXLLEN];
  char source_name[MAXLLEN];
  int sstat=0;
  char *flags="-s";
  struct stat t;

  FILEBUF *dbuf;	/* destination dir buffer  */
//  MESG("dir_link:");
  if(!(cbfp->b_flag & FSNLIST)) return FALSE;
  if(dir_getfile(fname,1)<0) return FALSE;

  set_full_name(source_name,getcwd(current_dir,MAXLLEN),fname,MAXLLEN);
  set_list_type(LDIR2);
  
  destination[0]=0;
  sstat=snprintf(sconfirm,MAXLLEN,"Link [%s] to: ",source_name);
  if(sstat>=MAXLLEN) MESG("dir link fname truncated!");
 
  escape_file_name(source_name);

  dbuf=get_dir_buffer(DIR_OTHER,0);	/* get destination dir buffer  */
  if(dbuf!=NULL) {
  	strlcpy(destination,dbuf->b_dname,MAXFLEN);
  } else {
  	destination[0]=0;
  };
//  MESG("destination dir is [%s]",destination);

  if((s1 = nextarg(sconfirm,destination,MAXFLEN,true)) !=TRUE) return(s1);


  sstat = stat(destination,&t);
  escape_file_name(destination);
  if(S_ISDIR(t.st_mode)) strcat(destination,"/");
  sstat=snprintf(sline,MAXLLEN,"ln %s %s %s 2>/dev/null",flags,source_name,destination);
  if(sstat>=MAXLLEN) { return error_line("dir link fname overflow!");return FALSE;};
  // MESG("dir_link: cmd=[%s]",sline);

  s1=chdir(cbfp->b_dname);
  s2=system(sline);
  if(s2 != 0) { return error_line("Error linking file to %s",destination);};
// refresh other buffer if onscreen
  dir_other_reload(1);
  return 1;
}

int dir_reload(num n)
{
	pdline = cbfp->cdir->cline-1;
	reinit_dir(cbfp);
	set_update(cwp,UPD_EDIT);
	if(cbfp->sort_mode == -1) cbfp->sort_mode=0;
	current_sort_mode=cbfp->sort_mode;
	list_dir(cbfp->cdir->dir_name,cbfp);
	return 1;
}

int dir_other_reload(num n)
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


int delete_dir(char *fname)
{
	char rmcmd[MAXFLEN];
    char sconfirm[MAXLLEN];
	int sstat=0;
	int status=0;
#if	1
	status = rmdir(fname);
	if(status!=0)
#else
	sstat=snprintf(rmcmd,MAXFLEN,"rmdir %s 2>/dev/null",fname);
	if(sstat<MAXFLEN) {
		status=system(rmcmd);
		// MESG("delete_dir:[%s] 1 status=%d",fname,status);
	} else {
		return error_line("command truncated!");
	};
	if(status) 
#endif
	{
		sstat=snprintf(sconfirm,MAXLLEN,"Dir [%s] not empty",fname);
		if(sstat>=MAXLLEN) { return error_line("command truncated!");};
		if( confirm(sconfirm,"remove ?",1)) {
			sstat=snprintf(rmcmd,MAXFLEN,"rm -rf %s 2> /dev/null",fname);
			if(sstat<MAXFLEN) { 
				status=system(rmcmd);
				// MESG("delete_dir:[%s] 2 status=%d",fname,status);
			} else { return error_line("rm command truncated!");};
		} else return 1;
	};
	return status;
}

 /* delete one file */
int dir_del1(num  n) 
{
  int status;
  char fname[MAXFLEN];
  // MESG("\ndir_del1:");
  if(!(cbfp->b_flag & FSNLIST)) return FALSE;
  status=chdir(cbfp->b_dname);
 
  if(cbfp->selected_files==0) {
   
  status=dir_getfile(fname,1);
  escape_file_name(fname);
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

	status = delete_dir(fname);
	if(status>0) return true;
	if(status<0) { 
		set_update(cwp,UPD_STATUS);
		return error_line("Dir %s not removed",fname);
	} else { 
		delete_current_list_line();
		msg_line("File deleted!");
	};
  };
  } else {
	if(confirm("Delete files","delete selected files ?",1)){
	int i;
	istr **row_data = (istr **) array_data(cbfp->dir_list_str);
	int s1=0;
	int files_to_delete=cbfp->selected_files;
	 	for(i=0;i<cbfp->dir_list_str->size;i++) {
			istr *dir_str = row_data[i];
			if(dir_str->selection_tag) {
				int ftype = dir_getfile1(cbfp,fname,0,i);
				// MESG("delete line %d name='%s' type=%d",i,fname,ftype);
				if(ftype>=0) {
					if(ftype==FTYPE_DIR) {
						escape_file_name(fname);
						s1=delete_dir(fname);
					} else {
						s1=unlink(fname);
					};
					// MESG("	s1=%d",s1);
					if(s1==0) {
						dir_str->selection_tag=0;
						cbfp->selected_files--;
					};
				}
			};
		};
		if(cbfp->selected_files<files_to_delete) {
			if(cbfp->selected_files>0) error_line("Not all files deleted");
			else msg_line("%d files deleted",files_to_delete);
			dir_reload(1);
		} else {
			error_line("Cannot delete files!");
		};
	} else return false;  
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
int dir_view(num n) 
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

 if((fname = find_file("",APPLICATION_EXTENSIONS,1,0))==NULL) return;

 read_pairs(fname,'=',&f_extension,&f_extcmd);
 // MESG("init_extentions: file=[%s]",fname);
// for(int i=0;f_extcmd[i]!=NULL;i++) MESG("%d: %s=%s",i,f_extension[i],f_extcmd[i]);
}

int exec_shell(char *tline);

// external execution of a file
int exec_ext(char *fname,char *fname_ns,int f_vx)
{
	char tmp_name[MAXFLEN];
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
			if(fname[0]!='/') strlcpy(tmp_name,"./",MAXLLEN);else tmp_name[0]=0;
			strlcat(tmp_name,fname,MAXLLEN);
			exec_shell(tmp_name);
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
		// sstat=set_unique_tmp_file(tmp_name,"exec",MAXFLEN);
		cmd_ext=strdup(cmd);
		sstat=snprintf(cmd,MAXLLEN,"%s %s > /tmp/exec 2>/dev/null",cmd_ext,fname);
		free(cmd_ext);
		if(sstat>=MAXLLEN) { return error_line("command 9 truncated!");};
		sstat=snprintf(fname,MAXFLEN,"/tmp/exec");
		if(sstat>=MAXFLEN) { return error_line("command 10 truncated!");};
		if(system(cmd)== -1) {
			return error_line("Error return from '%s %s'",cmd,fname);
		}
		strlcpy(tmp_name,"/tmp",MAXFLEN);
		// MESG("exec_ext: 4 inview: %s",fname);
		view_file(fname);
		return true;
	};
	if(is_compressed) {
		char cmd2[MAXLLEN];
		sstat=set_unique_tmp_file(tmp_name,fname,MAXFLEN);
		if(!sstat) return false;
		/* This is a compressed file. Uncompress to a temporary first */
		sstat=snprintf(cmd2,MAXLLEN,"gzcat %s > %s 2>/dev/null",fname,tmp_name);
		// MESG("exec_ext: uncompress command $s",cmd2);
		if(sstat<MAXLLEN) {
			if(system(cmd2)) { return error_line("cannot decompress file");};
		} else { return error_line("truncated, cannot sysexec");};
		sync();
		// sstat=snprintf(tmp_name,MAXFLEN,"%s",tmp_name);
		// if(sstat>=MAXFLEN) { return error_line("command 11 truncated!");};
		sstat=snprintf(cmd2,MAXLLEN,"%s \"%s\" >/dev/null 2>/dev/null &",cmd,tmp_name);
		// MESG("exec_ext: exec command $s",cmd2);
		if(sstat<MAXLLEN) status = sysexec(cmd2);
		else { return error_line("truncated, cannot sysexec");};
	} else {
		s=getcwd(tmp_name,MAXFLEN);
	};

// execute the command with the file name as argument
	cmd_ext=strdup(cmd);
	web_view=sindex(cmd_ext,":");	// we need to prepend 'file://'

	if(!strncmp("/mnt/",cmd_ext+web_view,5)) {
		char ms_fname[MAXLLEN];
		sstat=snprintf(ms_fname,MAXLLEN,"%s/%s",tmp_name,fname_ns);
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
		if(web_view) sstat=snprintf(cmd,MAXLLEN,"%s \"file://%s/%s\" >/dev/null  2>/dev/null &",cmd_ext+web_view,tmp_name,fname_ns);
		else {
			if(f_vx==FILE_EXEC) { 
			tmp_name[0]=0;
			strlcat(tmp_name,cmd_ext,MAXLLEN);
			strlcat(tmp_name," ",MAXLLEN);
			strlcat(tmp_name,fname,MAXLLEN);
			// MESG("use exec_shell [%s]",tmp_name);
			exec_shell(tmp_name);
			return true;
			};
			sstat=snprintf(cmd,MAXLLEN,"%s \"%s/%s\" >/dev/null  2>/dev/null &",cmd_ext,tmp_name,fname_ns);
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
	else { error_line("command truncated,cannot execute!");return FALSE;};
	if(is_compressed) unlink(tmp_name);	/* remove temporary file  */
	if(status>0) return FALSE;
	else return TRUE;
}

/* directory command execution */
int dir_exec(num n) 
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
  if(ftype==FTYPE_DIRLINK || ftype==FTYPE_DIR) {
	status = movein_dir(fname);
  } else {
	status=exec_ext(fname,fname,FILE_EXEC);
  };
  return status;
}

/* external view */
int dir_right(num n) 
{
  int ftype;
  char fname[512];
  char fname_ns[512];
  int status=FALSE;

  // MESG("dir_right: b_flag=%X",cbfp->b_flag);
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

  set_goal_column(0,"dir_right");
  cwp->w_lcol=0;
  set_update(cwp,UPD_STATUS);
  return(status);
}

int script_exec(num nuse)
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

	strlcpy(dir_name,bp->b_dname,MAXFLEN);
	escape_file_name(dir_name);

	if(bp->b_flag & FSDIRED) {
		dir_getfile(script_name,1);
//		MESG("dir: scrip=[%s]",script_name);
	} else {
		strlcpy(script_name,bp->b_fname,MAXFLEN);
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
			// MESG("found dir buffer:");
			return(bp);
		}
	};
	// MESG("destination not found!");
	return(NULL);
}

int view_next(num n)
{
#if	TNOTES
 if(cbfp->b_flag==FSNOTES) return(0);
#endif
 dir_left(0);
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

int view_previous(num n)
{
#if	TNOTES
 if(cbfp->b_flag==FSNOTES) return(0);
#endif
 dir_left(0);
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

int dir_left(num n)
{
 FILEBUF *bf;
 FILEBUF *vbuf;
 int dir_num = cbfp->dir_num;
 int stat=0;


 if(cbfp->connect_buffer==NULL && cbfp->b_flag != FSNLIST+FSDIRED) return(prev_character(1));
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

	if(cwp->tp_current->col > 0 && n==1) return(prev_character(1));
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
#if	1
		// if not at bol go one character left
		if(cwp->tp_current->col > 0 && n==1) return(prev_character(1));
#endif
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

int dir_tag(num n) 
{
  if(!(cbfp->b_flag & FSNLIST)) return FALSE;
  istr **row_data = (istr **) array_data(cbfp->dir_list_str);
  istr *dir_str = row_data[cwp->current_note_line];
  if(dir_str->selection_tag) {
  	dir_str->selection_tag=0;
	cbfp->selected_files--;
  } else {
  	dir_str->selection_tag=1;
	cbfp->selected_files++;
  };
  // MESG("dir_tag: %d",dir_str->selection_tag);
  next_line(1);
  set_update(cwp,UPD_EDIT);
  return 1;
}

/* show current dir file info on window */
int dir_showfileinfo(num n) 
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
  if(*c=='\'') { *dest++ = '\\'; };	/* escape right parenthesis  */
  if(*c=='!') { *dest++ = '\\'; };	/* escape right parenthesis  */
  if(*c=='$') { *dest++ = '\\'; };	/* escape right parenthesis  */
  if(*c=='&') { *dest++ = '\\'; };	/* escape right parenthesis  */
  if(*c=='*') { *dest++ = '\\'; };	/* escape right parenthesis  */
  if(*c=='`') { *dest++ = '\\'; };	/* escape right parenthesis  */
  *dest++ = *c;
 };*dest=0;
 strlcpy(fname,efname,MAXFLEN);
}

char *get_line_at(FILEBUF *fb,offs offset);

/* get file name from current line */
/* if flag then add \ in front of space characters  */
int dir_getfile(char *fname,int flag) 
{
 // offs s;
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
 // MESG("dir_getfile:start f=%d line=%d",flag,cwp->current_note_line);

 char *line_str;
 if(cbfp->b_flag & FSNLIST) {
	// MESG("dir_getfile: size of dir=%d line %d",cbfp->dir_list_str->size,cwp->current_note_line);
	if(cbfp->dir_list_str->size==0) return -1;	// no lines

	istr **row_data = (istr **)array_data(cbfp->dir_list_str);
	// MESG("dir_getfile: 2");
	if(cwp->current_note_line > cbfp->dir_list_str->size-1) return -1;
	istr *current_str = row_data[cwp->current_note_line];
	// MESG("current index=%d",current_str->index);
	line_str = &current_str->start;
	// MESG("line_str:[%s]",line_str); 
 } else {
	// MESG("dir_getfile not in FSNLIST!");
 	line_str=get_line_at(cbfp,Offset());
 };
 // MESG("dir_getfile: b_flag=%X [%s]",cbfp->b_flag,line_str);  
 c=line_str[0];
 if(c=='c') { msg_line("c type file");return -1;};
 if(c=='s') { error_line("cannot view socket files");return -1;};
 if(c=='#' ||c=='!'||c=='/') ftype=FTYPE_DIR;else ftype=FTYPE_NORMAL;
 c=line_str[1];
 if(c=='l') is_link=1;
 // s=DIR_NAME_POSITION;
 len=strlen(line_str)-DIR_NAME_POSITION;
 f=f1;
 char *start_string=line_str+DIR_NAME_POSITION;
 char *end_string=strstr(line_str," -->");
 if(end_string != NULL) {
	len=end_string-start_string;
 };


  memcpy(f1,start_string,len);
  f1[len]=0;
  // escape_file_name(f1);  

 // MESG("file name: [%s][%s]",start_string,f1);
 if(stat_result<0) {
	strlcpy(fname,f1,MAXFLEN);
	// MESG("stat_result: <0 , return [%s]!",fname);
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

int dir_getfile1(FILEBUF *fp,char *fname,int flag,int file_index) 
{
 char c;
 int ftype=FTYPE_NORMAL;
 int len;
 int stat_result=0;
 int is_link=0;
 static char f1[MAXFLEN];
 struct stat t;

 fname[0]=0;
 // MESG("dir_getfile1 :start f=%d file_index=%d",flag,file_index);

 char *line_str;
 if(fp->b_flag & FSNLIST) {
	// MESG("dir_getfile: size of dir=%d line %d",fp->dir_list_str->size,cwp->current_note_line);
	if(fp->dir_list_str->size==0) return -1;	// no lines

	istr **row_data = (istr **)array_data(fp->dir_list_str);
	// MESG("dir_getfile: 2");
	if(file_index > fp->dir_list_str->size-1) return -1;
	istr *current_str = row_data[file_index];
	// MESG("current index=%d",current_str->index);
	line_str = &current_str->start;
	// MESG("line_str:[%s]",line_str); 
 } else {
	// MESG("dir_getfile not in FSNLIST!");
 	line_str=get_line_at(fp,Offset());
	return 0;
 };
 // MESG("dir_getfile: b_flag=%X [%s]",fp->b_flag,line_str);  
 c=line_str[0];
 if(c=='c') { error_line("cannot view c type files");return -1;};
 if(c=='s') { error_line("cannot view socket files");return -1;};
 if(c=='#') ftype=FTYPE_DIR;
 c=line_str[1];
 if(c=='l') is_link=1;
 // s=DIR_NAME_POSITION;
 len=strlen(line_str)-DIR_NAME_POSITION;

 char *start_string=line_str+DIR_NAME_POSITION;
 char *end_string=strstr(line_str," -->");
 if(end_string != NULL) {
	len=end_string-start_string;
 };

  memcpy(f1,start_string,len);
  f1[len]=0;
  if(flag) escape_file_name(f1);  

 // MESG("dir_getfile1 name: [%s] len =%d [%s]",f1,len,start_string);

 // MESG("dir_get_file1: [%s] is_link=%d",f1,is_link);
 if(is_link) 
 {
	stat_result=stat(f1,&t);
	if(stat_result==0) {
		if(S_ISDIR(t.st_mode)) ftype=FTYPE_DIRLINK;
		else ftype=FTYPE_NORMAL;
	} else return -1;
 };
 strlcpy(fname,f1,MAXFLEN);
 // MESG("dir_getfile1: fname=[%s]",fname);
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

char *str_efile(struct kdirent *entry)
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
// MESG("[%30s] st_mode=%d ifdir=%d",entry->d_name,t->st_mode & S_IFMT,S_IFDIR);
 if((entry->st_mode & S_IFMT)==S_IFLNK) {
 	int i1;
	memset(lname,0,MAXLLEN);
	i1=readlink(entry->d_name,lname,MAXLLEN); 
	if(i1>0){
		struct stat t;
		lname[i1]=0;
		i1=stat(lname,&t);
		if(i1<0) strlcat(lname," does not exist!",MAXLLEN);
	} else {
		snprintf(lname,MAXLLEN,"file error %d ...",errno);
	};
	mx1='l';
 };

 switch (entry->st_mode & S_IFMT) {
 case S_IFLNK: mx1='l';break;
 case S_IFBLK: mt='b';break;
 case S_IFIFO: mt='f';break;
 case S_IFREG: mt='-';break;
 case S_IFDIR: mt='#';break;
 case S_IFCHR: mt='c';break;
#if	LINUX | FREEBSD | SOLARIS
 case S_IFSOCK: mt='s';break;
#endif
 };

if((entry->st_mode & S_IXUSR \
 || entry->st_mode & S_IXGRP \
 || entry->st_mode & S_IXOTH) && mx1=='-') mx1='x';

 if(current_sort_mode>7) t1 = localtime(&entry->atime);
 else if(current_sort_mode>5) t1 = localtime(&entry->ctime);
 else t1 = localtime(&entry->mtime);
 year = t1->tm_year;
 if(year>99) year-=100;

#if	LINUX | FREEBSD | SOLARIS
 if(
// 	mt=='?' || 
 	mt=='s') strcpy(ssize,"0");
 else
#endif
 {
	if(entry->st_size > 9999999999) {
		double fsize=(double)entry->st_size/1000000000;
		snprintf(ssize,50,"%.1f G",fsize);
	} else 
	if(entry->st_size > 9999999) {
		double fsize=(double)entry->st_size/1000000;
		snprintf(ssize,50,"%.1f M",fsize);
	} else 
 	snprintf(ssize,50,"%llu",(unsigned long long)entry->st_size);
 };
 d_month=t1->tm_mon;
 if(d_month>11) {
	d_month=12;
 };

 if(lname[0]!=0) 
 sstat=snprintf(str,255,"%c%c %10s %02d %3s %2d %02d.%02d %s --> %s",
 	mt,mx1,ssize,t1->tm_mday,month[d_month],year,t1->tm_hour,t1->tm_min,entry->d_name,lname);
 else
 sstat=snprintf(str,255,"%c%c %10s %02d %3s %2d %02d.%02d %s",
 	mt,mx1,ssize,t1->tm_mday,month[d_month],year,t1->tm_hour,t1->tm_min,entry->d_name);
 if(sstat>255) MESG("truncated 12"); 
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
 // MESG("insert_dir:[%s]",d1);
 alist *dir_list_str=new_list(0,"dir_as_list");
 if(buf_dir->cdir == NULL) { error_line("cdif is null");return 0;};
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
  	fname=ff->d_name;// ff->
	if(!strcmp(fname,".")) continue;
	if(!strcmp(fname,"..")) continue;
	istr *dir_istr;

	fline=str_efile(ff);
	dir_istr = (istr *)malloc(sizeof(istr)+strlen(fline));
	memcpy(&dir_istr->start,fline,strlen(fline)+1);
	dir_istr->index = dir_ind++;
	dir_istr->selection_tag=0;

	add_element_to_list((void *)dir_istr,dir_list_str);

	// MESG("[%s]",fline);
	free(namelist[i]);	
 };
 buf_dir->selected_files=0;
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

int dir_select_none(num dummy)
{
 FILEBUF *fp=cbfp;
 istr **row_data = (istr **) array_data(fp->dir_list_str);
 int i;
 for(i=0;i<fp->dir_list_str->size;i++) {
	istr *dir_str = row_data[i];
	dir_str->selection_tag=0;
 };
 fp->selected_files=0;
 // MESG("dir_select_none:");
 set_update(cwp,UPD_EDIT);
 return true;
}

int dir_select_all(num dummy)
{
 FILEBUF *fp=cbfp;
 
 istr **row_data = (istr **) array_data(fp->dir_list_str);
 int i;
 fp->selected_files=0;
 for(i=0;i<fp->dir_list_str->size;i++) {
	istr *dir_str = row_data[i];
	dir_str->selection_tag=1;
	fp->selected_files++;
 };
 // MESG("dir_select_all: selected=%d",fp->selected_files);
 set_update(cwp,UPD_EDIT);
 return true;
}

int dir_select_reverse(num dummy)
{
 FILEBUF *fp=cbfp;
 istr **row_data = (istr **) array_data(fp->dir_list_str);
 int i;
 fp->selected_files=0;
 for(i=0;i<fp->dir_list_str->size;i++) {
	istr *dir_str = row_data[i];
	if(dir_str->selection_tag==0) {
		dir_str->selection_tag=1;
		fp->selected_files++;
	} else {
		dir_str->selection_tag=0;
	};
 };
 // MESG("dir_select_reverse: selected=%d",fp->selected_files);
 set_update(cwp,UPD_EDIT);
 // dir_reload(1);
 return true;
}

int listdir(int dtype)
{ 
 int stat;
 char go_name[MAXFLEN];

 if(!drv_initialized) return false;
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

int show_dir_help(num n)
{

}

/* ---- */
