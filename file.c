/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.
	
	File functions  
*/

#include	"xe.h"
#include	"menu.h"

#if	TNOTES
char *get_notes_key();
#endif

/* external defined functions  */
char *getcurfword();
int open_file_dialog(char *,int n);

extern alist *file_list;
extern alist *window_list;
extern SHLIGHT hts[] ;
extern int scratch_files[];
extern MENUS m_sort;
extern MENUS *start_menu;
extern FILEBUF *cbfp;

char *uncompress_command[] = {
#if	DARWIN
	"","gzcat","unzip -l","bzcat","gzcat","bzcat","bzcat","zcat -l",""
#else
	"","zcat","unzip -l","bzcat","gzcat","bzcat","bzcat","zcat -l",""
#endif
};

/* local define functions */
int add_to_recent_list(char *full_file_name);
int is_encrypt(int file_id);

int scratch_ind(char *bname)
{
	if(bname==NULL) return 0;
	if(bname[0]==CHR_LBRA && bname[1]=='n' && bname[2]=='e' && bname[3]=='w') {
		int scratch_ind;
		sscanf(bname+4,"%d",&scratch_ind);
		return scratch_ind;
	};
	return 0;
}

int is_scratch_buffer(FILEBUF *bp)
{
	// MESG("- scartch is %d",bp->scratch_num);
	if(bp) return bp->scratch_num;
	return 0;
}

int is_special_buffer(FILEBUF *bp)
{
	if(bp->b_fname[0]==CHR_LBRA) return(true);
	return false;
}

int new_file(int n)
{
 int stat;
 char scratch_name[24];
 int i;
 int scratch_ind=99;
// find the scratch file number
 for(i=0;i<100;i++) {
 	if(scratch_files[i]==0) {
		scratch_ind=i+1;
		scratch_files[i]=scratch_ind;
		break;
	};
 };

 snprintf(scratch_name,24,"[new %d]",scratch_ind);
 stat=goto_file(scratch_name);
 if(stat) {
	 cbfp->scratch_num=scratch_ind;
	 cbfp->b_type=1;
	 set_hmark(1,"new_file:");
	 return TRUE;
 } else return false;
}

int buffer_is_connected(FILEBUF *bf)
{
 FILEBUF *bp;
	lbegin(file_list);
	while((bp=(FILEBUF *)lget(file_list))!=NULL)
	{
		if(bp->connect_buffer==bf) {
			return true;
		};
	};

	return false;
}

/* switch to the next file buffer in the file list */
/* Bound to F10 */
int next_file(int n)	
{
	register FILEBUF *bp;
	FILEBUF *fp=cbfp;
	int stat;
	lfind_data((void *)fp,file_list);
	lmove_to_next(file_list,1);
	while((bp=(FILEBUF *)lget_circ(file_list))!=fp) {
		if(bp->b_flag & FSINVS) continue;
		stat=select_filebuf(bp);
		set_hmark(1,"next_file");
		return(stat); 
	};
	return FALSE;
}

/* check if we have read the file and read it if not! */
int activate_file(FILEBUF *bp)
{
	if(bp->b_dname[0]!=0) if(chdir(bp->b_dname)!=0) return false;
	// MESG("activate_file:[%s] b_type=%d b_flag=%X",bp->b_fname,bp->b_type,bp->b_flag);
	if ((bp->b_state & FS_ACTIVE) ==0)
	{	
//		MESG("active_file: is not active, activate it!");
		/* read it in and activate it */
		if(bp->b_fname[0]!=CHR_LBRA || !strncmp(bp->b_fname,"[D",2))
		{
			file_read(bp,bp->b_fname);
		};
		if(!strncmp(bp->b_fname,"[new ",4)) {
			file_read(bp,bp->b_fname);
		};
		textpoint_set(bp->save_current,0);
		textpoint_set(bp->save_hline,0);

		textpoint_set(bp->save_hsknown,0);
		textpoint_set(bp->save_hmknown,0);
		// if(bp->b_flag != FSDIRED) 
		textpoint_set(bp->tp_current,0);
		// bp->save_ppline=0;
		bp->b_state |= FS_ACTIVE;

		if(bp->b_fname[0]!=CHR_LBRA){
			add_to_recent_list(get_buf_full_name(bp));
		};
	};
//	MESG("activate_file: end [%s] b_type=%d b_flag=%d",bp->b_fname,bp->b_type,bp->b_flag);
	return true;
}

int umark_buffer(int n)
{
 if(cbfp){
	cbfp->b_state &= ~FS_CHG;
 };
 return(1);
}

/* link/associate window to buffer */
void link_window_buffer(WINDP *wp,FILEBUF *fp)
{
 int i;
	cbfp= fp;
	textpoint_link(wp->tp_current,fp);
	// MESG("link window %d to buffer %s",wp->id,fp->b_fname);
	if(wp==cwp||fp->b_nwnd==0) fp->tp_current = wp->tp_current;
	textpoint_set(wp->tp_current,tp_offset(fp->save_current));
	wp->w_ppline=fp->save_ppline;
	// and the mark position
	// and associate window points with the file
	textpoint_link(wp->w_smark,fp);
	textpoint_set(wp->w_smark,0);
	textpoint_link(wp->w_emark,fp);
	textpoint_set(wp->w_emark,0);
	textpoint_link(wp->tp_hline,fp);
	textpoint_set(wp->tp_hline,tp_offset(fp->save_hline));
	textpoint_link(wp->prev_hline,fp);
	textpoint_set(wp->prev_hline,0);
	textpoint_link(wp->tp_hsknown,fp);
	textpoint_set(wp->tp_hsknown,tp_offset(fp->save_hsknown));

	textpoint_link(wp->tp_hmknown,fp);
	textpoint_set(wp->tp_hmknown,tp_offset(fp->save_hmknown));
	wp->selection=0;	// no selection active
#if	TNOTES
	wp->top_note_line=fp->save_top_note;
	wp->top_tag_line=fp->save_top_tag;
	wp->current_note_line=fp->save_current_note;
	wp->current_tag_line=fp->save_current_tag;
#endif
	for(i=0;i<2;i++) {
		wp->hs[i].w_hquotem=fp->save_hs[i].w_hquotem;
		wp->hs[i].w_hselection=fp->save_hs[i].w_hselection;
		wp->hs[i].w_slang=fp->save_hs[i].w_slang;
		wp->hs[i].w_notes=fp->save_hs[i].w_notes;
	};
	if((int)bt_dval("show_vinfo")) wp->w_infocol=VMICOLS;
	else wp->w_infocol=0;

	wp->w_fp = fp;
	fp->b_nwnd++;
}

int set_window_filebuf(WINDP *wp,FILEBUF *bp)
{
	if(bp==NULL) return(FALSE);
	if(!activate_file(bp)) return false;
	if(!is_special_buffer(bp)) set_working_dir(bp->b_dname);

	if(discmd==FALSE) { cbfp=bp; return (TRUE); }

	unlink_filebuf(wp);
	link_window_buffer(wp,bp);
	set_utf8_error(0);

	textpoint_set(wp->tp_hsknown,0);
	textpoint_set(wp->tp_hmknown,0);
	set_update(wp,UPD_FULL);
	wp->selection=0;
	if((int)bt_dval("show_vinfo")) wp->w_infocol=VMICOLS;
	else wp->w_infocol=0;

    return (TRUE);
}

/* select a file buffer to show in current window */
int select_filebuf(FILEBUF *bp)
{
	if(bp==NULL) return false;
	if(!activate_file(bp)) return false;
	if(!is_special_buffer(bp)) set_working_dir(bp->b_dname);

	if(discmd==FALSE) { cbfp=bp; return (TRUE); }

	if(bp==cbfp && !(bp->b_flag & FSDIRED)) {
		return(TRUE);
	};
	if(execmd) { 
		cbfp=bp;
		return(TRUE);
	};
	unlink_filebuf(cwp);
	link_window_buffer(cwp,bp);
	set_utf8_error(0);

	textpoint_set(cwp->tp_hsknown,0);
	textpoint_set(cwp->tp_hmknown,0);
	set_update(cwp,UPD_FULL);
	cwp->selection=0;
	if((int)bt_dval("show_vinfo")) cwp->w_infocol=VMICOLS;
	else cwp->w_infocol=0;

	return (TRUE);
}

/* save current position if last window of file */
// unlink window from file! (remove all window textpoints)
void unlink_filebuf(WINDP *wp)
{
 FILEBUF *bp=wp->w_fp;
 int i;
 if(wp->w_fp==NULL) return;
 
	--bp->b_nwnd;
	textpoint_set(bp->save_current,tp_offset(bp->tp_current));
	textpoint_set(bp->save_hsknown,tp_offset(wp->tp_hsknown));
	textpoint_set(bp->save_hmknown,tp_offset(wp->tp_hmknown));
	textpoint_set(bp->save_hline,tp_offset(wp->tp_hline));

#if	TNOTES
	bp->save_top_tag = wp->top_tag_line;
	bp->save_top_note = wp->top_note_line;
	bp->save_current_tag = wp->current_tag_line;
	bp->save_current_note = wp->current_note_line;
#endif
	// unlink file textpoints from window
	textpoint_unlink(wp->tp_hsknown);
	textpoint_unlink(wp->tp_hmknown);
	textpoint_unlink(wp->prev_hline);
	textpoint_unlink(wp->tp_hline);
	textpoint_unlink(wp->w_emark);
	textpoint_unlink(wp->w_smark);
	textpoint_unlink(wp->tp_current);

	wp->w_fp->save_ppline=wp->w_ppline;
	for(i=0;i<2;i++) {
		bp->save_hs[i].w_hquotem=wp->hs[i].w_hquotem;
		bp->save_hs[i].w_hselection=wp->hs[i].w_hselection;
		bp->save_hs[i].w_slang=wp->hs[i].w_slang;
		bp->save_hs[i].w_notes=wp->hs[i].w_notes;
	};
	if(bp->b_nwnd == 0) bp->tp_current = bp->save_current;
}

/* select filebuf  */
int sfb(FILEBUF *bp)
{
 if(bp==cwp->w_fp) return (TRUE);
	unlink_filebuf(cwp);
	link_window_buffer(cwp,bp);

	return (TRUE);
}

// save all the scratch files, no questions asked
void save_scratch_files()
{
 FILEBUF *bp;
	lbegin(file_list);
	while((bp=(FILEBUF *)lget(file_list))!=NULL)
	{
		if(is_scratch_buffer(bp))
		{
			// MESG("save_scratch_files: %s",bp->b_fname);
			select_filebuf(bp);
			writeu1(bp->b_fname,bp);
		};
	};
}

int load_scratch_files()
{
 DIR *d1;
 char home_dir[MAXFLEN];
 char scratch_name[MAXFLEN];
 int stat=0;
 int ind=0;
 // MESG("load_scratch_files:");
 stat=snprintf(home_dir,MAXFLEN,"%s/.%s",getenv("HOME"),APPLICATION_NAME);
 if(stat>=MAXFLEN) MESG("truncated scratch file");

 d1 = opendir((const char *)home_dir);
 if(d1==NULL) {
 	MESG("cannot read home .%s dir",APPLICATION_NAME);
 } else {
 	int i=0;
	struct dirent *df1;
	char **namelist=NULL;

	for(i=0;;i++) {
		FILEBUF *bp;
		df1=readdir(d1);
		if(df1==NULL) { closedir(d1);break;};
		// if(df1->d_name[0]!=CHR_LBRA) { i--;continue;};
		if(strncmp(df1->d_name,"[new",4)!=0) { i--;continue;};
		// if(strcmp(".",df1->d_name)==0) { i--;continue;};
		if(ind%256 == 0) namelist = (char **)realloc((char *)namelist, (ind+256)*sizeof(char *));
		namelist[ind]=(char *)malloc(strlen(df1->d_name)+1);
		if(namelist[ind]) {
			strlcpy(namelist[ind],df1->d_name,strlen(df1->d_name)+1);
			set_full_name(scratch_name,home_dir,namelist[ind],MAXFLEN);

			bp=new_filebuf(scratch_name,0);
			if(bp==NULL) MESG("cannot create new scratch file!");
			ind++;
		};
	} ;
 };
 return ind;	// return number of scratch files
}


/* close current window file */
int close_file(int n)
{
	FILEBUF *f_toclose,*bp,*f_previous=NULL;
	WINDP *wp;
	alist *wlist;;
	int scratch_num=0;
	// MESG("close_file:");
	if(buffer_is_connected(cbfp)) {
		msg_line("Cannot close, buffer is connected");
		return FALSE;
	};
	f_toclose=cbfp;
	scratch_num = is_scratch_buffer(cbfp);

	scratch_num = scratch_ind(cbfp->b_fname);

	if(scratch_num) {
		scratch_files[scratch_num-1]=0;
	};

	if(cbfp->b_fname[0]!=CHR_LBRA){
		if((f_toclose->b_state & FS_CHG) != 0 && (f_toclose->b_flag & FSMMAP)==0)              /* Something changed    */
		{
			char prompt[MAXLLEN];
			int lstat=
			snprintf(prompt,MAXLLEN,"Closing file %s",f_toclose->b_fname);
			if(lstat>MAXLLEN) prompt[MAXLLEN-1]=0;
	        if ( confirm(prompt,"discard all changes?",1) != TRUE) { 
				return (OK_CLRSL);
			};
		};
	};

	if(cbfp->connect_buffer!=NULL) {
		f_previous=cbfp->connect_buffer;
	} else {
	/* find the previous buffer in buffer list */
	lfind_data((void *)f_toclose,file_list);
	lmove_to_previous(file_list,1);

	while((bp=(FILEBUF *)lget_current(file_list))!=f_toclose) {
		// MESG("previous is: %s",bp->b_fname);
		if(bp->b_flag&FSINVS) {
			lmove_to_previous(file_list,1);
			continue;
		};
		f_previous=bp;
		break;
	};
	if(f_previous==NULL) {
		// this is the only edited file, just quit
		quit(1);
	};
	};

	if(cbfp == f_toclose) {
		delete_hmark(1);
		select_filebuf(f_previous);
		set_hmark(0,"close_file");
	}

	/* stat= */ remove_from_list((void *)f_toclose,file_list);
	// change all windows with that file to next one!
	// find these windows first and put them in wlist
	wlist=new_list(0,"temp");
	lbegin(window_list);
	while((wp=lget(window_list))!=NULL) {
		if(wp->w_fp == f_toclose) {
			add_element_to_list((void *)wp,wlist);
		}
	};
	lbegin(wlist);
	while((wp=lget(wlist))!=NULL) {
			set_window_filebuf(wp,f_previous);
	};
	remove_elements(wlist);
	free_list(wlist,"temp");

	if(f_toclose) delete_filebuf(f_toclose,1);
	clear_message_line();
	set_update(cwp,UPD_ALL);
	return TRUE;
}

/* remove a file buffer */
int delete_filebuf(FILEBUF *bp,int force)
{
	int stat;
	// MESG("delete_filebuf:");
    if (bp->b_nwnd > 0) {                  /* Error if on screen.  */
            msg_line("File is on screen");
            return (FALSE);
    };
	stat = remove_from_list((void *)bp,file_list);
	if((stat || force) && execmd==0) 
	{
		EmptyText(bp);
		if(bp->main_undo) efree(bp->main_undo,"delete_filebuf, main_undo");
		bp->main_undo=NULL;

		textpoint_free(bp->tp_base);
		textpoint_free(bp->tp_text_end);
		textpoint_free(bp->tp_text_o);
		textpoint_free(bp->tmp_current);	/* cache current point  */
		textpoint_free(bp->save_current);
		textpoint_free(bp->save_hsknown);
		textpoint_free(bp->save_hmknown);
		textpoint_free(bp->save_hline);
		textpoint_free(bp->smark);
		textpoint_free(bp->emark);
		if(bp->rdir) {
			if(bp->rdir->dir_name != NULL) free(bp->rdir->dir_name);
			if(bp->rdir!=bp->cdir) {
				if(bp->cdir) {
					if(bp->cdir->dir_name != NULL) free(bp->cdir->dir_name);
					free(bp->cdir);
				}
				free(bp->rdir);
			} else {
				free(bp->rdir);
			}
			bp->rdir=NULL;
		};
		if(bp->dir_list_str) free_list(bp->dir_list_str,"delete_filebuf dir str");
		bp->dir_list_str=NULL;

		if(bp->dir_list_stack) free_list(bp->dir_list_stack,"delete_filebuf dir stack");
		bp->dir_list_stack=NULL;
#if	TNOTES
		if(bp->b_tag_list) free_list(bp->b_tag_list,"delete_filebuf tag list");
		bp->b_tag_list=NULL;
#endif
		free(bp);
		return(TRUE);
	} else {
		return FALSE;
	};
}

/*	List file buffers, in a window, select from the list
	and make active. ( ^B )
*/
int select_file(int n)
{
	char st[2048];
    register FILEBUF *bp;
	FILEBUF *cbp;
	int stat;
	int file_flag=0;

	cbp = cbfp;
	if(macro_exec) {
		char *name;
		nextarg("select_file",st,512,true);
		name=st;
		// MESG("select_file: [%s]",name);
		if(name[0]==CHR_LBRA) {
			if((bp=new_filebuf(name,0)) ==NULL) return(FALSE);
		} else {
			bp = get_filebuf(name,".",0);
		};
		if(bp!=NULL) {
			stat=select_filebuf(bp);
			set_update(cwp,UPD_EDIT);
			set_hmark(1,"select_file:a");
			return(stat); 
		} else {
			set_update(cwp,UPD_MOVE);
			return FALSE;
		};
	} else {
	if(!list_filebuf("Select file", st,1,&file_flag)) return 0;
	// MESG("select_file: %s",st);
		if(st[0]!=0) {
			bp = get_filebuf(st,NULL,file_flag);
			if(bp==NULL) {
				set_update(cwp,UPD_MOVE);
				return (FALSE);
			};
			if(bp!=cbp) {
				stat=select_filebuf(bp);
				set_update(cwp,UPD_EDIT);
				// update_screen(1);
				set_hmark(1,"select_file:b");
				return(stat); 
			} else {
				set_update(cwp,UPD_EDIT);
				return(TRUE);
			}
		} else {
			set_update(cwp,UPD_MOVE);
			return(FALSE);
		}
	};
}

/* list/select internal file buffers */
int select_file_internal(int n)
{
	char st[2048];
    register FILEBUF *bp;
	FILEBUF *cbp;
	cbp = cbfp;
	int file_flag=0;

	if(!list_filebuf("Internal files",st,0,&file_flag)) return FALSE;
	if(st[0]!=0) {
        if ((bp=new_filebuf(st, 0)) == NULL)  return (FALSE);
		if(bp!=cbp) return(select_filebuf(bp)); else return(TRUE);
	} else return (FALSE);
}

// f1=0 internals only, f1=1 normal files, f1=2 all 
#define	FCSIZE	23
int  list_filebuf(char *title, char *return_string,int f1,int *return_flag)
{
    FILEBUF *bp;
	int i;
	int i1;
	int l1;
	char changed;
	int *index;
	char **file_items;
	int size;
	const int box_width = 70;
	
	l1 =0;
	if(f1>0) l1 = FCSIZE-5;

	size=list_size(file_list);
	index = (int *)malloc(sizeof(int)*(size+1));
	file_items = (char **)malloc(sizeof(char *)*(size+1));
	// create the file_items list
	for(lbegin(file_list),i=0,i1=0;(bp=(FILEBUF *)lget(file_list))!=NULL;)
	{
		int nsize = l1+sizeof( bp->b_fname)+1;
		if(f1==1 && bp->b_flag & FSINVS){ i1++; continue;};
		if(f1==0 && !(bp->b_flag & FSINVS)){ i1++; continue;};

		file_items[i] = (char *)malloc( nsize );

		if ((bp->b_state & FS_CHG) !=0) changed = '*'; 
		else {
			if(bp->b_flag == FSMMAP+FSDIRED && (bp->b_state & (FS_ACTIVE|FS_VIEW))) changed = 'm';
			else if(bp->b_flag == FSMMAP && (bp->b_state & (FS_ACTIVE|FS_VIEW))) changed = 'r'; 
			else changed = '-';
		};
		snprintf(file_items[i],nsize,"%-15.15s %c ",bp->b_fname,changed);
		if(!(bp->b_flag & FSINVS)) snprintf(file_items[i]+l1,nsize-l1,"%-70.70s",bp->b_dname);

		index[i]=i1++;
		i++;
	};
	file_items[i]=NULL;

	// MESG("list_filebuf: l1=%d f1=%d",l1,f1);

	if(f1) 
	i1 = selectl(title,file_items,i,40,FCSIZE,1,box_width,-1);
	else
	i1 = selectl(title,file_items,i,40,FCSIZE,1,box_width,-1);
	
	if(i1<0)  strcpy( return_string,"");
	else {
		if(file_items[i1][16]=='m') *return_flag=FSMMAPA+FSDIRED+FS_VIEW;
		if(file_items[i1][16]=='r') *return_flag=FSMMAPA+FS_VIEW;

		i1 = index[i1];
		for(lbegin(file_list),i=0;(bp=(FILEBUF *)lget(file_list))!=NULL;i++)
		{
			if(i==i1) {
				if(bp->b_flag & FSINVS) strlcpy(return_string,bp->b_fname,MAXLLEN);
				else {
					int stat;
					stat=snprintf(return_string,MAXLLEN,"%s/%s",bp->b_dname,bp->b_fname);
					if(stat>=MAXLLEN) msg_line("truncated 12 string");
				};
				break;
			};
		};
	};
	clear_snames(file_items,size);
	free(index);
	return TRUE;
}

/* Return true if an editing has been done.  */
int changes_occured()
{
        FILEBUF *bp;
		for(lbegin(file_list);(bp=(FILEBUF *)lget(file_list))!=NULL;)
		{
        	if (bp->b_fname[0]!=CHR_LBRA 
			&& (bp->b_flag&FSINVS)==0 
			&& (bp->b_flag&FSMMAP)==0
			&& (bp->b_state & FS_CHG)!=0) return (TRUE);
        }
        return (FALSE);
}

void update_base_dir(char *dir_name,char *bname)
{
 char *lslash=NULL;
 char *s=NULL;
 s=bname;
// MESG("udb: dir_name=[%s] bname=[%s]",dir_name,bname);
 while((s=strchr(s,'/'))!=NULL){
 	lslash=s;
	s++;
 };
 if(lslash) {
	int l=strlen(dir_name);
	int ss=lslash-bname;
	strlcat(dir_name,"/",MAXFLEN);
	if(l+1+ss>MAXFLEN) {
		ss=MAXFLEN-l-1;
		ERROR("filename too big!");
	};
	memcpy(dir_name+l+1,bname,ss);
	dir_name[l+ss+1]=0;
 };
}

// find the file buffer of a file name, return NULL if not currently edited!
FILEBUF * get_filebuf(char *bname,char *dname,int flag)
{
	FILEBUF *bp=NULL;
	char *s;
	char base_name[MAXFLEN];	// base file name
	char dir_name[MAXFLEN];	// directory name
	dir_name[0]=0;
	base_name[0]=0;

	// MESG("get_filebuf:f=[%s] d=[%s,flag=%d",bname,dname,flag);
	create_base_name(base_name,bname);
	if(bname[0]!=CHR_LBRA && bname[0]!='/') {
		if(dname!=NULL) strlcpy(dir_name,dname,MAXFLEN);
		update_base_dir(dir_name,bname);
	} else 
	{
		if(dname!=NULL && bname[0]!='/') {
			strlcpy(dir_name,dname,MAXFLEN) ;
		} else {
			if(bname[0]=='/') {	/* OSNOSAFE!  */
				
				strlcpy(dir_name,bname,MAXFLEN);
				dir_name[strlen(dir_name)-strlen(base_name)-1]=0;
			} else {
				if(bname[0]!=CHR_LBRA || !strncmp(bname,"[D",2)) { 
					s=getcwd(dir_name,MAXFLEN);
				} else {
					s=strcpy(dir_name,"");
				};
				if(s==NULL) return(NULL);
			};
		};
	};

	// MESG("find [%s] in buffer list",base_name);
	// check the file_list
	lbegin(file_list);
	while((bp=(FILEBUF *)lget(file_list))!=NULL)
	{
		// check for directory name also!
		// compare first base_name then dir name
		// MESG("[%s] <> [%s]",base_name,bp->b_fname);
		if(strncmp(base_name, bp->b_fname,MAXFLEN) == 0 ) {
			if((bp->b_flag & (FSINVS)) || (base_name[0]==CHR_LBRA)) {
				return(bp);	// for special files this is enough
			};
			// MESG("	[%s] [%s] f=%d <> %d",dir_name, bp->b_dname,flag,bp->b_flag);
			if(strncmp(dir_name, bp->b_dname,MAXFLEN)==0) {
				return(bp);
			}
		};
   };
	return(bp);
}


FILEBUF * new_filebuf(char *bname,int bflag)
{
	static int main_dir=1;
	static int buffer_index=0;
	register FILEBUF *bp;
	struct stat bp_stat;
	int err=0;
	int ind2;
	char *s=NULL;
	char base_name[MAXFLEN];	// base file name
	char dir_name[MAXFLEN];	// directory name
	int dir_num=0;
	int is_scratch=0;
	create_base_name(base_name,bname);
	// MESG("new_filebuf:base_name=[%s] bname=[%s]",base_name,bname);
	dir_name[0]=0;
	is_scratch = scratch_ind(base_name);

	if(is_scratch) {
		snprintf(dir_name,MAXFLEN,"%s/.%s",getenv("HOME"),APPLICATION_NAME);
	} else {
		if(cbfp){
		if(cbfp->b_dname[0]==0){
			if(getcwd(dir_name,MAXFLEN)==NULL) return false;
		} else {
			strlcpy(dir_name,cbfp->b_dname,MAXFLEN);
		};
		} else {
			strlcpy(dir_name,get_start_dir(),MAXFLEN);
		}
	};

	update_base_dir(dir_name,bname);
	if(strlen(bname)>1 && (bname[2]=='1' || bname[2]=='2')) {
		dir_num=bname[2]-'0';
	} else {	
		if(cbfp) {
			if(cbfp->dir_num !=0) dir_num=cbfp->dir_num;
		};
	};

	if(bflag == 0 || is_scratch) { // check for directory!
		err=stat(bname,&bp_stat);
		if(err==0 && S_ISDIR(bp_stat.st_mode)) {
			dir_num=main_dir++;
			create_base_name(base_name,bname);
			set_todirname(base_name,dir_num);
			strlcpy(dir_name,bname,MAXFLEN);
			strlcpy(bname,base_name,MAXFLEN);
			if((bp=get_filebuf(bname,dir_name,0))!=NULL) return (bp); // this already exists!
		} else {
			dir_num=0;
			if((bp=get_filebuf(bname,NULL,0))!=NULL) return (bp); // this already exists!
		};
	} else {
		if((bp=get_filebuf(bname,NULL,0))!=NULL) return (bp); // this already exists!
	};

	if( bflag & FSINVS || is_scratch ) err=401;
	else {
		err=stat(bname,&bp_stat);
	}

	if((err==0 && S_ISDIR(bp_stat.st_mode)) || !strncmp(bname,"[D",2)) {
		if(bflag) {
			set_working_dir(getcwd(dir_name,MAXFLEN));
			if(chdir(bname)!=0) err=402;
	 		s=getcwd(dir_name,MAXFLEN);
			if(s==NULL) dir_name[0]=0;
		};
		set_todirname(base_name,dir_num);
		bflag |= FSDIRED;
		if(chdir(get_working_dir())!=0) err=2;
	} else { // this is a new file!
		if(bname[0]!=CHR_LBRA && bname[0]!='/') {
			update_base_dir(dir_name,bname);
		} else {
			if(bname[0]=='/') {	/* OSNOSAFE!  */
				strlcpy(dir_name,bname,MAXFLEN);
				dir_name[strlen(dir_name)-strlen(base_name)-1]=0;
			} else {
				if(bname[0]!=CHR_LBRA) {
					s=getcwd(dir_name,MAXFLEN);
				} else {
					if(is_scratch) {
						snprintf(dir_name,MAXFLEN,"%s/.%s",getenv("HOME"),APPLICATION_NAME);
					} else {
						strcpy(dir_name,"");
					}
				}
			};
		};
	};

	if(err!=0) { // can we really create this file?
		err=stat(dir_name,&bp_stat);
		if(err<0 && bname[0]!=CHR_LBRA) return FALSE;
	};

	if ((bp=(FILEBUF *)malloc(sizeof(FILEBUF))) == NULL) return (NULL);
	/* find the place in the list to insert this buffer */

	add_element_to_list((void *)bp,file_list);
	/* and set up the other buffer fields */
	bp->b_index = buffer_index++;
    bp->b_flag  = bflag;
	bp->b_mode  = gmode;
	bp->b_state = 0;
	bp->scratch_num=is_scratch;
	if((int)bt_dval("show_vinfo")) bp->view_mode = VMINFO;
	else bp->view_mode  = 0;
	bp->b_lang = default_lang;

	if(bflag & FSINVS) bp->b_state |= FS_ACTIVE;
	
	bp->rdir = (dir_l *)malloc(sizeof(struct dir_l));
	bp->rdir->dir_name=NULL;
	bp->rdir->cline = 0;
	bp->dir_list_stack = new_list(0,"new filebuf dir");
#if	TNOTES
	bp->b_tag_list = new_list(0,"new filebuf tags");
#endif
	bp->dir_list_str=NULL;

	// MESG("new_filebuf: dir_list!");
	if(bflag & FSDIRED) {
		bp->cdir = bp->rdir;
		bp->cdir->dir_name = NULL;
	} else {
		bp->cdir = NULL;
	};

	bp->buffer=NULL;	// no data!
	bp->tp_last=NULL;
	bp->npoints=0;
	bp->symbol_tree=NULL;
	bp->symbol_table=NULL;
	bp->type_list=NULL;
	bp->tp_base = textpoint_new(bp,TP_BASE,0);
	bp->tp_text_end = textpoint_new(bp,TP_TEXT_END,0);
	bp->tp_text_o = textpoint_new(bp,TP_TEXT_END,0);
	bp->save_current = textpoint_new(bp,TP_SAVE_CURRENT,0);
	bp->tp_current = bp->save_current;
	bp->tmp_current = textpoint_new(bp,TP_TEMP,0);	/* cache current point  */
	bp->save_hsknown = textpoint_new(bp,TP_SAVE_HSKNOWN,0);
	bp->save_hmknown = textpoint_new(bp,TP_SAVE_HMKNOWN,0);
	bp->save_hline = textpoint_new(bp,TP_SAVE_HLINE,0);
	bp->smark = textpoint_new(bp,TP_B_SMARK,0);
	bp->emark = textpoint_new(bp,TP_B_EMARK,0);
	bp->connect_buffer=NULL;
	bp->connect_line=0;
	bp->connect_column=0;
	bp->connect_top_line=0;
	bp->connect_top_note=0;
	bp->save_ppline=0;
	bp->ptr1=bp->ptr2=0;
	bp->oldptr1=bp->oldptr2=0;
	bp->BufferSize=0;
	bp->GapSize=0;
	bp->EolSize=1;
	bp->EolStr[0]=10;bp->EolStr[1]=0;
	bp->main_undo=undo_new();

	bp->save_hs[0].w_hquotem=0;
	bp->save_hs[0].w_hselection=0;
	bp->save_hs[0].w_slang=0;
	bp->save_hs[0].w_notes=0;
	bp->save_hs[1].w_hquotem=0;
	bp->save_hs[1].w_hselection=0;
	bp->save_hs[1].w_slang=0;
	bp->save_hs[1].w_notes=0;
	bp->b_type=0;
	bp->lines=0L;
	bp->bytes_read=0L;
    bp->b_nwnd  = 0;
	bp->tok_table=NULL;
	bp->err=-1;
	bp->m_mode=0;
	bp->dir_num=dir_num;
	bp->sort_mode=0;
	bp->utf_accent=0;
#if	CRYPT
	bp->b_key[0]=0;
#endif
#if	NUSE
	bp->slow_display=0;
#endif
#if	TNOTES
	bp->b_note=NULL;
	bp->b_header=NULL;
	bp->b_tags=0;
	bp->b_notes=0;
	bp->save_top_tag=0;
	bp->save_top_note=0;
	bp->save_current_tag=0;
	bp->save_current_note=0;
#endif
	strlcpy(bp->b_fname, base_name,255);
	if(is_scratch) {
		set_highlight(bp,highlight_index("TEXT",&ind2));
		bp->b_type=1;
	} else
	if(bname[0]==CHR_LBRA) {
		set_highlight(bp,highlight_index("CMD",&ind2));
//			if(!is_scratch_name(bname)) bp->main_undo->enabled=false;
	} else {
		if(bp->cdir == bp->rdir) { 
			set_highlight(bp,highlight_index("DIR",&ind2));
		} else {
			bp->b_type = 0;
			set_highlight(bp,highlight_index("NONE",&ind2));
		}
	};
	if(bp->b_flag & FSINVS) strlcpy(bp->b_dname,get_start_dir(),MAXFLEN);
	else strlcpy(bp->b_dname,dir_name,MAXFLEN);

	if(!is_special_buffer(bp)) set_working_dir(bp->b_dname);

	return(bp);
}

/* Empty a file buffer, ask if on screen and changed */
int empty_filebuf(FILEBUF *bp)
{
	if(!strcmp(bp->b_fname,"[dofile]")) return(TRUE);
	if(!macro_exec){
        if ((bp->b_flag&FSINVS) == 0            /* Not internal file.  */
        && (bp->b_state & FS_CHG) != 0              /* Something changed    */
        && (confirm("Clear all text", "discard all changes?",1)) != TRUE
		) { return (OK_CLRSL);};
	};
	EmptyText(bp);
	return TRUE;
}

/* Clear out buffer, do not make current! */
FILEBUF *cls_fout(char *bname)
{
 FILEBUF *bp;
 WINDP *wp;
 if((bp=new_filebuf(bname,0)) !=FALSE) {
	if((bp->b_state & FS_ACTIVE)==0) activate_file(bp);
	EmptyText(bp);
// update the buffer if on screen
 	lbegin(window_list);
	while((wp=(WINDP *)lget(window_list))!=NULL) {
		if(wp->w_fp==bp){
			set_update(wp,UPD_EDIT);
		};
	};
	return bp;
 } else {
 	return NULL;
 };
}


/* get the file buffer full file name  */
char *get_buf_full_name(FILEBUF *fp)
{
 static char full_name[MAXFLEN];
 set_full_name(full_name,fp->b_dname,fp->b_fname,MAXFLEN);
 return(full_name);
}

/*
	00 00 FE FF - UTF-32BE
	FE FF 00 xx - UTF-16BE
	FF FE 00 00 - UTF-32LE
	FF FE xx 00 - UTF-16LE
	EF BB BF xx - UTF-8
*/

/* get Byte Order Mark type */
int bom_type(int file_id)
{
 char c[8];
 lseek(file_id,0L,SEEK_SET);
 if(read(file_id,c,1)!=1) return(FALSE);
 if(c[0]==26) {
 	MESG("BOM is encrypted!");
 	return FTYPE_ENCRYPTED;
 };
 if(c[0]<0xEF) {
 	lseek(file_id,0L,SEEK_SET);
 	return FALSE;
 };
 if(read(file_id,c+1,1)!=1) {	// read second byte
 	lseek(file_id,0L,SEEK_SET);
	return (FALSE);
 };
 if(c[0]==0xFF && c[1]==0xFE) {
 	MESG("FTPE_UTF16BOM");
	lseek(file_id,0L,SEEK_SET);	// ??
	return(FTYPE_UTF16BOM);
 };
 if(c[0]==0xFE && c[1]==0xFF) {	// ??
 	MESG("FTPE_UTF16BOM_BE");
	lseek(file_id,0L,SEEK_SET);
	return(FTYPE_UTF16BOM);
 };
 if(c[0]==0xEF && c[1]==0xBB) { // check for utf8 bom
 	if(read(file_id,c+2,1)!=1) { 
		lseek(file_id,0L,SEEK_SET);
		return (FALSE);
	};
	if(c[2]==0xBF) return (FTYPE_UTF8BOM);
 };
 if(c[0]==0xFF && c[1]==0xFE) {
 	if(c[2]==0 && c[3]==0) {
	 	MESG("FTPE_UTF32BOM");
		return(FTYPE_UTF32BOM);
	};
 	MESG("FTPE_UTF16BOM");
	return(FTYPE_UTF16BOM);
 };
 if(c[0]==0 && c[1]==0 && c[2]==0xFE && c[3]==0xFF){
 	lseek(file_id,0L,SEEK_SET);
	MESG("FTYPE_UTF32BE");
	return(FTYPE_UTF32BOM_BE);
 };
 // check for UTF32 BOM
 lseek(file_id,0L,SEEK_SET);
 return(FALSE);
}

/* Reload file from disk */
int reload_file(int n)
{
 FILEBUF *fp=cbfp;
 // MESG("reload_file: %X",fp->b_flag);
#if	TNOTES
	switch(fp->b_flag) {
		case FSNOTES: 
		case FSNOTESN:
			return reload_tag_view();
		case FSNTODO: return show_todo_list(1);
		case FSNCALENDAR: return show_calendar_list(1);
	};
#endif
   return(file_read(fp, fp->b_fname));
}

/* Insert a file into current position.  */
int insert_file(int  n)
{
    char fname[MAXFLEN];
	FILEBUF *fp=cbfp;
	int stat;
	if(dont_edit()) return FALSE;
	set_list_type(LDIR);
	fname[0]=0;
    if (nextarg("Insert file: ", fname, MAXFLEN,true) != TRUE)
                return(FALSE);
	stat=ifile(fp,fname,1);
	set_update(cwp,UPD_WINDOW);
    return(stat);
}


int open_file_named(char *fname)
{
 char full_name[MAXFLEN];
 int err;
 struct stat bp_stat;
	// MESG("open_file_named:[%s]",fname);
	set_bfname(full_name,fname);
	err=stat(full_name,&bp_stat);
	if(S_ISDIR(bp_stat.st_mode)) { 
		err=open_dir(full_name,1);
		set_hmark(1,"open_dir:1");
		return(err);
	};
	err=goto_file(full_name);
	set_hmark(1,"open_file_named");
	return err;
}

/*
 * Open file for editing
 * emacs key ^O
 */

int open_file(int n)
{
 char fname[MAXFLEN];	/* file user wishes to find */
 char tname[MAXFLEN];
 char prompt[MAXFLEN];
 offs o1=Offset();
 int err=0;
 int stat=0;
 int line=0;
 // MESG("open_file: %d [%s]",n,get_sval());
	fname[0]=0;
	if(macro_exec){
		// MESG("	open_file_named");
		err=open_file_named(get_sval());
		err=goto_file(get_sval());
		// igotolinecol(line,1,-1);
		return(err);
	}
	set_list_type(LDIR);
	if(n>0) {
		if(n==3) {
			if(!BolAt(Offset())) goto_bol(1);
		};
		strlcpy(tname,getcurfword(),MAXFLEN);
	} else tname[0]=0;
	if(! (cbfp->b_flag & FSDIRED)){
	if(cbfp->b_fname[0]==CHR_LBRA) {
		if(chdir(get_start_dir())) {
			msg_line("cannot open file in dir [%s]",cbfp->b_dname);
			return false;
		};
	} else {
		if(chdir(cbfp->b_dname)) {
			msg_line("cannot open file in dir [%s]",cbfp->b_dname);
			return false;
		};
	}
	};
	if(n==3) {	/* goto file,line  */
		next_word(1);
		line = atoi(getcurfword());	
		set_bfname(fname,tname);
	} else {

	if(!macro_exec && (xwin==2)) return(open_file_dialog(tname,n));

	stat=snprintf(prompt,MAXFLEN,"Open file [%s] : ",tname);
	if(stat>=MAXFLEN) MESG("truncated prompt when opening file!");
	// MESG("	get the file name from nextarg!");
    if (nextarg(prompt, tname, MAXFLEN,true) != TRUE){
		set_Offset(o1);
		set_update(cwp,UPD_WINDOW);
		return(FALSE);
	};
	// MESG("open_file: fname=[%s] tname=[%s]",fname,tname);
	set_bfname(fname,tname);
	if(fname[0]==0) strlcpy(fname,tname,MAXFLEN);
	if(fname[0]==0) return(FALSE);

	};
	err=open_file_named(fname);
	igotolinecol(line,1,-1);
	return(err);
}

int clear_buffer(int n)
{
 char fname[MAXFLEN];	/* file user wishes to find */
 char tname[MAXFLEN];
 char prompt[MAXFLEN];
 char full_name[MAXFLEN];
 offs o1=Offset();
 FILEBUF *bp;
 int stat=0;

	set_list_type(LDIR);
	strlcpy(tname,getcurfword(),MAXFLEN);

	strlcpy(fname,tname,MAXFLEN);
	stat=snprintf(prompt,MAXFLEN,"Clear buffer [%s] : ",fname);
	if(stat>=MAXFLEN) MESG("truncated prompt when clear_buffer!");
    if (nextarg(prompt, fname, MAXFLEN,true) != TRUE){
		set_Offset(o1);
		set_update(cwp,UPD_WINDOW);
		return(FALSE);
	};
	if(fname[0]==0) strlcpy(fname,tname,MAXFLEN);
	if(fname[0]==0) return(FALSE);

	set_bfname(full_name,fname);
	bp=get_filebuf(fname,".",0);
	
	if(bp!=NULL) {
		bp=cls_fout(fname);
		return(true);
	} else {
		MESG("buffer %s not opened!",fname);
	};	
	return false;
}

#if	CRYPT

/*	Crypt:	Encryption routines for MicroEMACS
		written by Dana Hoggatt and Daniel Lawrence
		Changed by K.Triantafillou to support 8 bit Characters. (not compatible with original memacs)
*/

#define CHAR_START	29
#define CHAR_END	255

int set_key(int nused)
{
	return set_buf_key(cbfp);
}

int set_buf_key(FILEBUF *bp)	/* reset encryption key of current file */
{
	int status;	/* return status */

	/* get the string to use as an encrytion string */
	bp->b_key[0]=0;
	MESG("set_buf_key: b_type=%d %d size=%d",bp->b_type,NOTE_TYPE,sizeof(bp->b_key));
#if	TNOTES
	if(bp->b_type & NOTE_TYPE
		|| bp->b_type & NOTE_CAL_TYPE
		|| bp->b_type & NOTE_TODO_TYPE
	) {

		if(get_notes_key() == NULL) {
			set_notes_key(1);
		};
		if(get_notes_key()) {
			strlcpy(bp->b_key,get_notes_key(),sizeof(bp->b_key));
		} else return false;
	} else 
#endif
	{
		// MESG("get encryption_key!");
		status = nextarg("Encryption String: ", bp->b_key, MAXSLEN - 1,false);
	    if (status != TRUE)  return(status);
	
		/* and encrypt it */
	
		crypt_string(NULL, 0L);
		crypt_string(bp->b_key, strlen(bp->b_key));
	};
	bp->b_mode |= EMCRYPT;	
	return(OK_CLRSL);
}

/* uncrypt/decrypt a string. This is taken/modified from Jaspa microemacs editor 
 * Copyright (C) 1986 Dana L. Hoggatt
*/
void crypt_string(char *ctr,long len)
{
	unsigned int cc;
	int val;
	unsigned char *bptr;
	static long int key = 0;	/* 29 bit encipherment key */
	static long int salt = 0;	/* salt to spice up key with */

	bptr = (unsigned char *)ctr;

	if (!bptr) {		/* check for null */
		key = len;	/* set the new key */
		salt = len;	/* set the new salt */
		return;
	};
	
	while (len--) {	
		
		cc = *bptr;	
		if ((cc >= CHAR_START) && (cc <= CHAR_END)) 
		{

/**  If the upper bit (bit 29) is set, feed it back into the key.  This 
	assures us that the starting key affects the entire message.  **/

			key = (key & 0x1FFFFFFFl) ^ ((key >> 29) & 0x3l);

/**  Down-bias the character, perform a Beaufort encipherment, and 
	up-bias the character again.  
 **/

		val = ( (int)(key % CHAR_END)-((int)cc - CHAR_START) ) % (CHAR_END - CHAR_START) ;
		while(val<0) val += (CHAR_END-CHAR_START);
		cc = val+CHAR_START;

/**  the salt will spice up the key a little bit, helping to obscure 
	any patterns in the clear text, particularly when all the 
	characters (or long sequences of them) are the same.  We do 
	not want the salt to go negative, or it will affect the key 
	too radically.  It is always a good idea to chop off cyclics 
	to prime values.  **/

		if (++salt >= 20857) {	/* prime modulus */
			salt = 0;
		}

/**  our autokey (a special case of the running key) is being 
	generated by a wieghted checksum of clear text, cipher 
	text, and salt.   **/

		key = key + key + ( cc ^ *bptr) + salt;
		}
		*bptr++ = cc % CHAR_END;	/* put character back into buffer */
	}
	return;
}


int resetkey(FILEBUF *bp)	/* reset the encryption key if needed */
{
	register int s;	/* return status */

	/* turn off the encryption flag */
	/* if we are in crypt mode */
	if (bp->b_mode & EMCRYPT) {
		if (bp->b_key[0] == 0) {
			s = set_buf_key(bp);
			if (!s)	return(s);
		}
		/* and set up the key to be used! */
		/* de-encrypt it */
		crypt_string((char *)NULL, 0);
		crypt_string(bp->b_key, strlen(bp->b_key));
		/* re-encrypt it...seeding it to start */
		crypt_string((char *)NULL, 0);
		crypt_string(bp->b_key, strlen(bp->b_key));
	};
	return(TRUE);
}
#endif

int goto_file(char *file_name)
{
    FILEBUF *bp;
	// MESG("goto_file:[%s][%s]",cbfp->b_dname,file_name);
	if(edit_file(file_name)) return(TRUE);
	if((bp=new_filebuf(file_name,0))==NULL) {
    	msg_line("Cannot create buffer [%s]",file_name);
        return (FALSE);
	};
	select_filebuf(bp);
	return true;
}

/* check if file is already edited */
int edit_file(char *fname)
{
 FILEBUF *bp;
 // MESG("edit_file: [%s]",fname);
 if(fname[0]=='/') {
	bp=get_filebuf(fname,NULL,0);
 } else {
 	if(cbfp->b_dname[0]==0) { 
		static char dir_name[MAXFLEN];
		if(getcwd(dir_name,MAXFLEN)==NULL) {
			msg_line("cannot stat current directory");
			return false;
		};
	 	bp = get_filebuf(fname,dir_name,0);	// find it in current dir!
	} else {
	 	bp = get_filebuf(fname,cbfp->b_dname,0);	// find it in previous buffer dir!
	}
 };
 if(bp==cbfp) return true;
 if(bp) {
 	select_filebuf(bp);
	return TRUE;
 };
 return(FALSE);
}

/*	Read file "fname" into the current filebuffer, 
	after removing all text
*/
int file_read(FILEBUF *bp, char *fname)
{
 int stat=0;
 // MESG("file_read: fname=[%s]",fname);
 if(fname[0]!=CHR_LBRA) if(!execmd) msg_line(" reading file:[%s]",fname);

 /* clear the buffer */
 if(empty_filebuf(bp)!=TRUE) return FALSE;
 if(fname!=bp->b_fname) strlcpy(bp->b_fname, fname,MAXFLEN);
 if(! ifile(bp,fname,0) && fname[0]!=CHR_LBRA) {
// 	msg_line("No lines for file %s",fname);
	return(FALSE);
 };
 bp->b_flag &= ~FSINVS;
 bp->b_state &= ~FS_CHG;
 if(is_scratch_buffer(bp)) {
 	char scratch_file[MAXFLEN];
	stat=snprintf(scratch_file,MAXFLEN,"%s/%s",bp->b_dname,bp->b_fname);
	if(stat<MAXFLEN) unlink(scratch_file);
 };
 set_update(cwp,UPD_FULL);
 // MESG("file_read: end: b_type=%d",bp->b_type);
 return TRUE; 
}

int file_read1(FILEBUF *bp, char *fname)
{
 if(!execmd) msg_line(" reading file: %s",fname);
 // MESG("file_read1:");
 /* clear the buffer */
 if(empty_filebuf(bp)!=TRUE) return FALSE;
 
 if(!ifile(bp,fname,0)) return(FALSE);
 bp->b_flag &= ~FSINVS;
 bp->b_state &= ~FS_CHG;
 return TRUE; 
}

/*
 * Take a file name, and fabricate a buffer name.
 */
void set_bfname(char *full_name, char *fname)
{
	char dir_name[MAXFLEN];
	// MESG("set_bfname: full_name=[%s] fname=[%s]",full_name,fname);
	if(fname[0]=='/') {
		strlcpy(full_name,fname,MAXFLEN);	// this is a full name
	}
	else if(!strcmp(fname,"~")) {
		strlcpy(full_name,getenv("HOME"),MAXFLEN);
	} else if(fname[0]=='~' && fname[1]=='/' ) { // in home dir
		set_full_name(full_name,getenv("HOME"),fname+2,MAXFLEN);
	} else if(fname[0]=='.' && fname[1]==0) {
		strlcpy(full_name,getcwd(dir_name,MAXFLEN),MAXFLEN);
	} else {
		if(cbfp) set_full_name(full_name,cbfp->b_dname,fname,MAXFLEN);
		else set_full_name(full_name,getcwd(dir_name,MAXFLEN),fname,MAXFLEN);
	};
}


int writeu1(char *fname, FILEBUF *fp)
{
 int s;
 // MESG("writeu1: fname=[%s] dir=%s name=%s",fname,fp->b_dname,fp->b_fname);
 if ((s=writeout(fname,fp)) == TRUE) {
 	// MESG("writeu1: ok");
	fp->b_state &= ~FS_CHG;
	update_file_status(fp);
	return true;
 }
 return(false);
}

/*
 * Current file save with a new name
 */
int saveas_file(int n)
{
	int    stat;
    char	fname[MAXFLEN];
	char	dname[MAXFLEN];
	int scratch_num=0;
	char	save_as_msg[MAXFLEN];
#if	TNOTES
	if(cbfp->b_type & NOTE_TYPE 
		|| cbfp->b_type & NOTE_CAL_TYPE
		|| cbfp->b_type & NOTE_TODO_TYPE
	) {
		msg_line("complete notes form first, then save!");
		return 0;
	};
#endif
	strlcpy(fname,cbfp->b_fname,MAXFLEN);
	if(snprintf(save_as_msg,MAXFLEN,"Save as: %s:",get_working_dir())>=MAXFLEN) return FALSE;
    if ((stat=nextarg(save_as_msg, fname, MAXFLEN,true)) != TRUE) return FALSE;
	scratch_num = is_scratch_buffer(cbfp);
	// MESG("saveas_file: %s",fname);
	if(scratch_num) 
	{
		if(chdir(get_start_dir())) {
			return error_line("cannot find start dir to save file!");
		};
		scratch_files[scratch_num-1]=0;
	};

	if((cbfp->b_flag & FSMMAP)) {
		FILEBUF *b;
		FILEBUF *vb=cbfp;
		int status;
		if(!strcmp(cbfp->b_fname,fname)) {
			return error_line("cannot write to the same read only file!");
		};
		vb->b_state = FS_ACTIVE;	/* set it so we can write it  */
	    strlcpy(vb->b_fname, fname,MAXFLEN);
		strlcpy(vb->b_dname,getcwd(dname,MAXFLEN),MAXFLEN);
		writeu1(fname,vb);
		b = new_filebuf(fname,0);
		select_filebuf(b);
		vb->b_flag = FSMMAP;	/* change back the flag!  */
		delete_filebuf(vb,1);
		status = goto_file(fname);
		strlcpy(cbfp->b_dname,getcwd(dname,MAXFLEN),MAXFLEN);
		return status;
	} else {
	    strlcpy(cbfp->b_fname, fname,MAXFLEN);
		strlcpy(cbfp->b_dname,getcwd(dname,MAXFLEN),MAXFLEN);
	};
 	if(cbfp->b_fname[0]!=CHR_LBRA){
		add_to_recent_list(get_buf_full_name(cbfp));
	};
	return(writeu1(fname,cbfp));
}

/*
 * Save current file, or sort menu in dir mode
 */
int save_file(int n)
{
 FILEBUF *fp=cbfp;

 // MESG("save_file: [%s] b_type=%X,b_flag=%X",fp->b_fname,fp->b_type,fp->b_flag);
	if(dont_edit()) return TRUE;
#if	TNOTES
	if(is_scratch_buffer(cbfp)) {
		// set file name
		snprintf(fp->b_fname,24,"%s.cal.md",date_string(3));
		
		// insert calendar header
		goto_bof(1);
		insert_preamble(fp,2);
		prev_line(2);
		next_character(2);
		
		// check for title
		
		// set fp to calendar type
		fp->b_type = NOTE_CAL_TYPE;
		// and remove scratch type
		fp->scratch_num=0;
	};

	if(fp->b_type & NOTE_TYPE 
		|| fp->b_type & NOTE_CAL_TYPE
		|| fp->b_type & NOTE_TODO_TYPE
	) {
		return save_note();
	};
#endif

    if ((fp->b_state & FS_CHG) == 0) {        /* Return, no changes.  */
    	msg_line("No changes!");
		return (TRUE);
	};
    if (fp->b_fname[0] == 0
		|| fp->b_fname[0]==CHR_LBRA
	) {    /* if no name, get a name */
		return(saveas_file(0));
    };
 	if(fp->b_fname[0]!=CHR_LBRA){
		add_to_recent_list(get_buf_full_name(fp));
	};

	return(writeu1(fp->b_fname,fp));
}

/*
 * Change file name of current buffer
 */
int rename_file(int n)
{
	register int    s;
	char            fname[MAXFLEN];
	strlcpy(fname,cbfp->b_fname,MAXFLEN);
	if ((s=nextarg("New file name: ", fname, MAXFLEN,true)) != TRUE) return (s);
	strlcpy(cbfp->b_fname, fname,MAXFLEN);
	cbfp->b_state &= ~FS_VIEW;	/* no longer read only mode */
	update_file_status(cbfp);
    return (TRUE);
}

int init_ftype(FILEBUF *bp,char *fname,int *temp_used)
{
 int s;
 int		tc=0;			/* file second extension type */
 int htype=0;	/* highlight type  */
 char	oext[MAXLLEN], cmd[MAXLLEN];
 *temp_used=0;
 // MESG("init_ftype:[%s] b_type=%d",fname,bp->b_type);
#if	CRYPT
	s=resetkey(bp);
	if (s != TRUE)	return(s);
#endif
	// check for dangerous files!
	// /proc/kmsg
	// /proc/kcore
	if(strcmp(fname,"kmsg")==0 && strcmp(bp->b_dname,"/proc")==0) {
		return error_line("should not access /proc/kmsg");
	};
	if(strcmp(fname,"kcore")==0 && strcmp(bp->b_dname,"/proc")==0) {
		return error_line("should not access /proc/kcore");
	};

	int ftype = file_type(fname, &tc, oext);
	bp->b_type |= ftype;
	// MESG("init_ftype: check tc %d b_type=%d [%s] [%s] oext=[%s]",tc,bp->b_type,hts[FX_COMPRESS].file_extentions[tc],uncompress_command[tc],oext);

	if(tc) {
			snprintf(cmd,MAXLLEN,"%s %s > /tmp/uncompressed 2>/tmp/err",uncompress_command[tc],fname);
			if(system(cmd)) strlcpy(fname,"/tmp/err",MAXFLEN);
			else  strlcpy(fname,"/tmp/uncompressed",MAXFLEN);
			*temp_used=tc;
	};

	// open the file
	bp->file_id = open(fname,O_RDONLY);
	if(bp->file_id<3) {
		bp->file_id=0;
		return FALSE;
	};

	bp->bytes_read=0;
	bp->err=-1;
	// MESG("init_ftype: file_id=%d",bp->file_id);
	if(bp->b_mode!=VMHEX) {
		bp->bom_type = bom_type(bp->file_id);
	};
#if	CRYPT
	if ((bp->bom_type == FTYPE_ENCRYPTED) 
		/* and if only text, tdc,cmd and notes are to be encrypted!  */
		&& (file_type_is("GTEXT",bp->b_type) 
		|| file_type_is("CMD",bp->b_type) 
		|| file_type_is("TEXT",bp->b_type) 
		|| file_type_is("MD",bp->b_type) 
		|| (bp->b_type & NOTE_TYPE))
		 ) {	
			// MESG("	file %s is encrypted!  %X %X",bp->b_fname,bp->b_type,NOTE_TYPE);
			bp->b_mode |= EMCRYPT;
#if	TNOTES
			if(bt_dval("notes_recreate")) {
				MESG("Notes recreate!");
				if(get_notes_key(1)==NULL) {
 					MESG("get new notes key");
					set_notes_key(1);
					if(get_notes_key()) {
						strlcpy(bp->b_key,get_notes_key(),sizeof(bp->b_key));
					} else return false;
				} else {
					MESG("set key from notes key!");
					strlcpy(bp->b_key,get_notes_key(),sizeof(bp->b_key));
				};
				s=true;
			} else 
#endif
			{
				s = resetkey(bp);
			};
			if (s != TRUE)	return(s);
	} else {
	};
#endif
	/* check again for highlight type */
	htype=get_highlight(bp);
	set_highlight(bp,htype);
	
	// MESG("init_ftype: set highlight b_type=%d b_mode=%X bom_type=%d",bp->b_type,bp->b_mode,bp->bom_type);
	if(bp->b_mode & EMCRYPT) {
		s= resetkey(bp);
	};
	if(close(bp->file_id)!=0) {
    	msg_line("Error %d closing file",errno);
		bp->file_id=0;
        return(false);
    };
	bp->file_id=0;
	// MESG("----  b_type=%d",bp->b_type);
 return TRUE;
}

 /* update mode lines of current file-buffer */
void update_file_status(FILEBUF *fp)
{
	register WINDP *wp;
	lbegin(window_list);
	while((wp=lget(window_list))!=NULL)
	{
    	if (wp->w_fp == fp) wp->w_flag |= UPD_STATUS;
    }
}

 /* get a menu file, select an item , execute it */
int menufile(int n)
{
 char *fname;
 int nu,s;
 char **ddnames;
 char **ddvalue;
 char *exec_s;	/* execute string */

 if((fname = find_file("",APPLICATION_USER_MENU,1,0))==NULL) return FALSE;

 nu=read_pairs(fname,';',&ddnames,&ddvalue);
 if(nu<1) { msg_line("user menu not found");return 0;};
 if(macro_exec) { 
 	if(n>0) s=n-1;else return 0;
 } else {
	s=selectl("User Menu",ddnames,nu,20,50,1,30,-1);
 };
 if(s>=0) exec_s=strdup(ddvalue[s]); else { return FALSE;};
 sarray_clear(ddnames);
 sarray_clear(ddvalue);
 s=docmd(exec_s);
 free(exec_s);

 return(s);
}

/* getb file extension, compressed extension */
int file_type(char *name, int *compression_type,char *oext)
{
 int i,e1_type,e2_type;
 int ext_len;
 char ext1[250],ext2[250],bname[MAXFLEN];
 int e; /* number of extensions */
 int ind;
 int ind2;
 e=0;
 create_base_name(bname,name);
 *compression_type=0;

 if(bname[0]=='.') {
	strcpy(oext,"");
	int type=0;
	int ind=highlight_index(bname+1,&type);
	// MESG("file_type: . [%s] = %d ind=%d",bname+1,type,ind);
	if(ind) return ind;
	return(highlight_index("DOT",&ind2));
 };
 if(!strcasecmp(bname,"makefile")) return(highlight_index("MAKE",&ind2));
 i=strlen(bname)-1;
 if(bname[i]=='~') i--;
 for(ext_len=0;i>0;i--,ext_len++) {
  if(bname[i]=='.') {e++; break;};
  ext1[ext_len]=bname[i];
 };
 ext1[ext_len]=0;	// extension in reverse 
 i--;

 if(e>0 && ext_len>0) {
	revstr(ext1);
	strlcpy(oext,ext1,MAXLLEN);
	ind=highlight_index(oext,&ind2);

	if(ind==FX_COMPRESS)
	{
		*compression_type=ind2;
		e1_type=0;
	} else {
		e1_type=ind;
		return e1_type;
	};	
 } else { 	/* No extention at all found!  */
	 strcpy(oext,"");
	 return(0);
 };

 for(ext_len=0;i>0;i--,ext_len++) {
  if(bname[i]=='.') {e++;break;};	/* we have 2 extensions  */
  ext2[ext_len]=bname[i];
 };
 ext2[ext_len]=0;

 if(e==2 && ext_len>0) {
	revstr(ext2);
	e2_type=highlight_index(ext2,&ind2);
	strlcpy(oext,ext2,MAXLLEN);
	if(*compression_type) return(e2_type);
 } ;
 return(e1_type);
}

/*
	commands	"cmds"
	help files	""
	pixmaps		"pixmaps"
	rc files	""
	menu files	""
	// check current dir (flag)
	// check path under current dir (flag)
	// check home_dir/ path
	// check /etc/xe path
	// check /usr/local/share/xe , path
	// check /usr/local/lib/xe , path
	find_file(path,file_name,flag)

*/

/* Initialize scratch buffer.  */
void edinit(char *bname)
{
    register FILEBUF *bp;

    bp = new_filebuf(bname,0);             /* First buffer         */
    if (bp==NULL)  exit(1);
	select_filebuf(bp);
}

char **recent_files=NULL;
alist *recent_file_list;

int add_to_recent_list(char *full_file_name)
{
	int found=0;
	char *new_str;
	// MESG("add_to_recent_list: [%s]",full_file_name);
	if(full_file_name[0]==CHR_LBRA) return 0;
	if(recent_file_list==NULL) return 0;

	new_str=strdup(full_file_name);
	found = str_in_list(recent_file_list,full_file_name,0);
	
	if(found) {
		remove_current_from_list(recent_file_list);
		// move to the start of the list
		lbegin(recent_file_list);
		linsert(recent_file_list,(void *)new_str);
	} else {
		// add it to the start of recent list;
		lbegin(recent_file_list);
		linsert(recent_file_list,(void *)new_str);
	}
	return 1;
}

int save_file_history(int n)
{
 char *fname;

 if(bt_dval("save_history")==0) return 0;
// MESG("save_file_history:");
 fname = find_file("",APPLICATION_HISTORY,1,1);
// MESG("save_file_history: %s",fname);
 return save_list_array(fname,recent_file_list);
}

int read_file_history(int n)
{
 char *fname;

 if((fname = find_file("",APPLICATION_HISTORY,1,0))==NULL) return FALSE;
 recent_file_list=new_list(0,"read_file_history");

// MESG("read_file_history: from [%s]",fname);
 read_list_array(fname,recent_file_list);
// show_string_list(recent_file_list,"after read history");
 recent_files = (char **)array_data(recent_file_list);
 return 1;
}

int open_recent_file(int n)
{
 char *fname;
 int nam1;
 int window_width=60;
 int lheight=20;
 int err;

 fname = find_file("",APPLICATION_HISTORY,1,0);
 if(fname==NULL) return(0);

 recent_files = (char **)array_data(recent_file_list);
 if(recent_files)
 {
 	nam1=selectl("Recent files",recent_files,recent_file_list->size,lheight,5,1,window_width,-1);
	if(nam1>=0){
		err = goto_file(recent_files[nam1]);
		return err;
	} else {
		save_file_history(1);
		return 1;
	};
 } else {
 	msg_line("No recent files!");
 };
 return 1;
}

