/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

*/
/*	Spawn:	System access commands */

#include	"xe.h"
#include	<sys/stat.h>

#include	<signal.h>

extern int update_all;
extern FILEBUF *cbfp;
void set_sval(char *s);
int main_clipboard_copy();
int x_insert_to_file(char *filnam);
int insert_text_file_nl(char *filnam);
int get_current_line();

struct timeval start_time;	/* editor time started stamp */

void set_start_time()
{
	gettimeofday(&start_time,NULL);
}

int set_unique_tmp_file(char *str,char *name,int size)
{
 int len;
 len=snprintf(str,size,"/tmp/%s_%ld_%ld.txt",name,start_time.tv_sec,(long)start_time.tv_usec);
 if(len>size) { error_line("tmp file %s truncated!",name);return false;};
 return true;
}

// execute a system command, returns error status, shows messages, clean at the end
int sysexec(char *st)
{
	int status=0;

	if(st[0]==0) return(FALSE);
	if(!execmd && !macro_exec) msg_line("wait for [%s]",st);
	events_flush();
	if(strncmp(cbfp->b_fname,"[new",4)==0){
		if(chdir(get_start_dir())!=0) { error_line("cannot change dir to start_dir %s",get_start_dir());};
	};
    status=system(st);
		// 127 or -1 -> error
		// status 32512 -> command not found
		// status 512 -> ok
		// status 256 -> ok ??
	sync();
	usleep(10000);
	if((status%256) ==0) {

		if(!macro_exec) set_update(cwp,UPD_ALL);
		if(status!=0) {
			msg_line("command [%s] exit status %d",st,status);
			events_flush();
		} else msg_line("ok!");
		return(TRUE);
	} else {
		char *st1=st;
		if(!macro_exec) {
			while(*st1) { 
				if( *st1=='>' ) {*st1=0;break;};
				st1++;
			};
			msg_line("command error in [%s] status %d",st,status);
			events_flush();
		};
		return(FALSE);
	};
}

/* update tag file */
int update_tags(num nused)
{
 FILEBUF *tagb;
 FILEBUF *old_cbfp;
 struct stat st;
 int status;
 msg_line("Updating tags");
 events_flush();
// create again the tag file
 if(stat("files.lst",&st)==0) {
	status=system("ctags -x --language-force=Cobol -L files.lst > tagx.tags 2>/dev/null");
 } else {
	status=system("ctags -x *.c *.cc *.h *.cpp *.hpp > tagx.tags 2>/dev/null");
 };
 if(status!=0) { msg_line("Error, no ctags file created!"); return(FALSE);};
 old_cbfp=cbfp;
 sync();
// if tag buffer exists go there and clear it
	if((tagb=get_filebuf("[tagx.tags]",NULL,0)) == NULL) {
		if((tagb=new_filebuf("[tagx.tags]", FSINVS)) == NULL) { 
			list_off();
			msg_line("cannot create [tagx.tags] buffer!");
			return(0);
		};
	};

	EmptyText(tagb);
	cbfp=tagb;
	set_Offset(0);

	status=ifile(tagb,"tagx.tags",0);
	tagb->b_state &= ~FS_CHG;
	set_Offset(0);
	cbfp=old_cbfp;
	msg_line("tagx.tags file updated!");
	return(OK_CLRSL);
}

/* update tag file */
int view_functions(num nused)
{
 // FILEBUF *tagb;
 // FILEBUF *old_cbfp;
 // struct stat st;
 int status;
 char tag_cmd[1024];
 char tag_file[MAXFLEN];
 char *kind="fs";
 char *title="Select tag";
 if(nused==2) { kind="f";title="Select function";};
 if(nused==3) { kind="s";title="Select structure";};
 if(cbfp->b_fname[0]=='[' || cbfp->b_flag & FSNLIST || cbfp->b_mode & VMHEX) return FALSE;

	msg_line("view_functions");
	events_flush();
	// create the tag file
	status=snprintf(tag_file,sizeof(tag_file),"%s.tag",cbfp->b_fname);
	if(status>=256) { msg_line("string overflow 1 in view_functions");events_flush();return false;};

	status=snprintf(tag_cmd,sizeof(tag_cmd),"ctags -x --c-kinds=%s %s 2>err1 | awk '{printf \"%%4s %%s %%s %%s %%s %%s %%s\\n\", $3,$6,$7,$8,$9,$10,$11}' > %s 2>/dev/null",kind,cbfp->b_fname,tag_file);
	if(status>255) { msg_line("string overflow 2 in view_functions");events_flush();return false;};

	status=system(tag_cmd);
	if(status!=0) {
		// MESG("view_functions: status=%d",status);
		msg_line("cannot use view_function");
		return FALSE;
	};
	// MESG("read the tag file!");
	int size=0;
	char **function_array=read_sarray(tag_file,&size);
	// MESG("we have %ld functions",size);
	if(size>0){
	int j1=selectl(title,function_array,size,20,5,1,60,-1);
	// MESG("selected %d",j1);

	if(j1>=0) {
		// char *iline=strstr(function_array[j1]," ");
		char *iline=function_array[j1];
		int line=atoi(iline);
		// msg_line("at %d",line);
		msg_line(function_array[j1]);
		set_hmark(0,"function");
		igotolinecol(line,1,0);
		set_hmark(1,"function");
	};
		clear_message_line();
	} else {
		if(nused==3) msg_line("No structures found!");
		else if(nused==2) msg_line("No functions found!");
		else msg_line("no tags found!");
	};
	free_sarray(function_array);
	unlink(tag_file);
	return(OK_CLRSL);
}

extern char slval[];

int exec_shell(char *tline);
/*
 * Execute a shell command.
 * show stdout on [command] buffer, stderr on [error] buffer
 * emacs key ^X!, assigned to Alt-1
 */
int shell_cmd(num nused)
{
    int    s;	/* return status from CLI */
    char	tline[MAXLLEN]; 	/* command line send to shell */
	tline[0]=0;
    if ((s=nextarg("!", tline, MAXLLEN,true)) != TRUE) return(s);
	s=exec_shell(tline);
	return s;
}

char *get_line_at(FILEBUF *fb,offs offset);

int exec_shell(char *tline)
{
//     int    s;	/* return status from CLI */
	FILEBUF *bp,*bperr;
	FILEBUF	*old_buffer=cbfp;
	static char bname[] = "[command]";
	static char ename[] = "[error]";

	static char filnam[MAXFLEN];	//  = "/tmp/command";
	static char filerr[MAXFLEN];
	int status=0;
	char exec_dir[MAXFLEN];

	if(cwp) {
		strlcpy(exec_dir,cwp->w_fp->b_dname,MAXFLEN);
	} else {
		strlcpy(exec_dir,get_start_dir(),MAXFLEN);
	};
	set_list_type(LSHELL);

	set_unique_tmp_file(filnam,"command",MAXLLEN);
	set_unique_tmp_file(filerr,"error",MAXFLEN);

	/* get the command  */
	set_sval("");

	strlcat(tline," > ",MAXLLEN);	// output file
	strlcat(tline,filnam,MAXLLEN);
	strlcat(tline," 2> ",MAXLLEN); 	// error file
	strlcat(tline,filerr,MAXLLEN);

	status=sysexec(tline);

	if(status) {
		sync();
	
		/* create and clear the command buffer */
	    if ((bp=new_filebuf(bname, 0)) != FALSE) {
			EmptyText(bp);
			if(bp->b_nwnd==0) {
			} else {
				// set [command] as active window
				change_window(find_buffer_window(bp));
			};
		} else {
			msg_line("cannot create command buffer");
			return(0);
		};
	
	    if ((bperr=new_filebuf(ename, 0)) != FALSE) {
			EmptyText(bperr);
			if(bperr->b_nwnd==0) {
	//			if (split_window(1) == FALSE) return(FALSE);
			}
	
		} else {
			msg_line("cannot create error buffer");
			return(FALSE);
		};
	
		select_filebuf(bperr);
		goto_eof(1);
		ifile(bperr,filerr,0);
		bperr->b_state &= ~FS_CHG;
		set_Offset(0);
		strlcpy(bp->b_dname,exec_dir,sizeof(bp->b_dname));
		strlcpy(bperr->b_dname,exec_dir,sizeof(bp->b_dname));
		select_filebuf(bp);
		goto_eof(1);
		ifile(bp,filnam,0);
		bp->b_state &= ~FS_CHG;
		set_Offset(0);
		set_hmark(1,"shell_cmd");
		/* and get rid of the temporary file */
		unlink(filnam);
		unlink(filerr);
		// MESG("exec: err_lines=%ld out_lines=%ld",bperr->lines,bp->lines);

		if(bp->lines<2) {
			select_filebuf(old_buffer);
			if(cbfp->b_flag & (FSNLIST)) dir_reload(1);
		};
		
		if(bperr->lines<2)	msg_line("ok!");
		else if(bperr->lines==2) {
			char *err_line1 = get_line_at(bperr,0);
			// MESG("err: %s",err_line1);
			msg_line(err_line1);
		} else msg_line("ok");
		return(TRUE);
	} else {
		set_update(cwp,UPD_MOVE);
		return (FALSE);
	};
}


int shell_cmd1(int  nused)
{
    int    s;	/* return status from CLI */
	FILEBUF *bp,*bperr;
    char	tline[MAXLLEN]; 	/* command line send to shell */
	static char bname[] = "[command]";
	static char ename[] = "[error]";

	static char filnam[MAXFLEN];	//  = "/tmp/command";
	static char filerr[MAXFLEN];
	int status=0;
	char exec_dir[MAXFLEN];

	if(cwp) {
		strlcpy(exec_dir,cwp->w_fp->b_dname,MAXFLEN);
	} else {
		strlcpy(exec_dir,get_start_dir(),MAXFLEN);
	};
	set_list_type(LSHELL);

	set_unique_tmp_file(filnam,"command",MAXLLEN);
	set_unique_tmp_file(filerr,"error",MAXFLEN);

	/* get the command  */
	tline[0]=0;
    if ((s=nextarg("!", tline, MAXLLEN,true)) != TRUE) return(s);
	set_sval("");

	strlcat(tline," > ",MAXLLEN);	// output file
	strlcat(tline,filnam,MAXLLEN);
	strlcat(tline," 2> ",MAXLLEN); 	// error file
	strlcat(tline,filerr,MAXLLEN);
	if(!macro_exec){
		
	};
	msg_line("executing ..");
	status=sysexec(tline);

	if(status) {
	sync();

	/* create and clear the command buffer */
    if ((bp=new_filebuf(bname, 0)) != FALSE) {
		EmptyText(bp);
		if(bp->b_nwnd==0) {
		} else {
			// set [command] as active window
			change_window(find_buffer_window(bp));
		};
	} else {
		msg_line("cannot create command buffer");
		return(0);
	};

    if ((bperr=new_filebuf(ename, 0)) != FALSE) {
		EmptyText(bperr);
		if(bperr->b_nwnd==0) {
//			if (split_window(1) == FALSE) return(FALSE);
		}

	} else {
		msg_line("cannot create error buffer");
		return(0);
	};

	select_filebuf(bperr);
	goto_eof(1);
	ifile(bperr,filerr,0);
	bperr->b_state &= ~FS_CHG;
	set_Offset(0);
	strlcpy(bp->b_dname,exec_dir,sizeof(bp->b_dname));
	strlcpy(bperr->b_dname,exec_dir,sizeof(bperr->b_dname));
	select_filebuf(bp);
	goto_eof(1);
	ifile(bp,filnam,0);
	bp->b_state &= ~FS_CHG;
	set_Offset(0);
	set_hmark(1,"shell_cmd");
	/* and get rid of the temporary file */
	unlink(filnam);
	unlink(filerr);
	msg_line("ok!");
	return(TRUE);
	} else {
		msg_line("error in shell command!");
		set_update(cwp,UPD_MOVE);
		return (FALSE);
	};
}

/* execute a background system command */
int bg_cmd(num n)
{
 char line[MAXLLEN];
 char fname[MAXFLEN+1];
 char tmp_name[MAXFLEN];
 register int    s;
 unsigned int i;
 int stat;
	set_list_type(LSHELL);
	line[0]=0;
	if ((s=nextarg("&", line, MAXLLEN,true)) != TRUE) 	return(s);
	/* replace %f with the current file name */
	/* for now this has to be at the end of the line */
	
	if(cbfp->b_flag & FSDIRED) {
		s = dir_getfile(fname,1);
	} else {
		stat=snprintf(fname,sizeof(fname),"%s/%s",cbfp->b_dname,cbfp->b_fname);
		if(stat>=MAXFLEN) fname[stat]=0;
	};

	for(i=1;i<strlen(line);i++) {
		if(line[i]=='f') {
			if(line[i-1]=='%') {
				line[i-1]=0;
				if(fname[0]!='/') strlcat(line,"./",MAXLLEN);
				strlcat(line,fname,MAXLLEN);
			}
		};
	};
	stat = set_unique_tmp_file(tmp_name,"bf_cmd",MAXFLEN);
	strlcat(line," > ",MAXLLEN);strlcat(line,tmp_name,MAXLLEN);
	strlcat(line," 2> ",MAXLLEN);strlcat(line,tmp_name,MAXLLEN);
	strlcat(line,".out &",MAXLLEN);
	msg_line("executing ..");
	sysexec(line);
	msg_line("finished");
	set_update(cwp,UPD_MOVE);
    return (TRUE);
}

#if NUSE
char *extension_string(FILEBUF *bp)
{
 static char ext_string[MAXLLEN];
 char *s = ext_string;
 int size=0;
 char **ext = bp->hl->file_extentions;

	if(bp->b_flag & FSDIRED || bp->b_fname[0]==CHR_LBRA) {
		sprintf(s,"*.* ");
	} else {
	while(*ext[0]) {
//			MESG("extention *.%s",*ext++);
		sprintf(s+size,"*.%s ",*ext++);
		size += strlen(s+size);
	};
	};
	return ext_string;
}
#endif

/* grep command assigned to alt-5  */
int grep_cmd(num n)
{
	FILEBUF *bp;
	char	line[MAXLLEN];
    char	tline[MAXLLEN]; 	/* command line send to shell */
	static char bname[] = "[grep]";
	static char filnam[MAXFLEN];	//  = "/tmp/command";
	static char *search_dir;
	int status=0;
#if	SOLARIS
	char *flag_str="-n";
#else
	char *flag_str="-nr";
#endif

	// MESG("grep_cmd: n=%d",n);

	set_list_type(LSHELL);

	if(is_scratch_buffer(cbfp)) {
		search_dir = get_start_dir();
	} else {
		search_dir = cbfp->b_dname;
	};
#if	TNOTES
	if(n==3) {
		set_bfname(search_dir,NOTES_DIR);
	};
//	status=chdir(notes_dir);
#endif

	if(chdir(search_dir)) {
		msg_line("cannot chdir to %s",search_dir);
		return false;
	};

//	MESG("grep_cmd: current dir [%s], seatch dir [%s] ---------------",getcwd(tline,MAXLLEN),search_dir);
	status = set_unique_tmp_file(filnam,"command",MAXFLEN);
	if(status>=MAXFLEN) return FALSE;
	/* get the command */
	tline[0]=0;
//	MESG("grep_cmd: n=%d",n);
	if(n>1) {
#if	TNOTES
		if(n==3) 
			status=nextarg("Notes find: ", tline, MAXLLEN,true);
		else
#endif
			status=nextarg("recursive find: ", tline, MAXLLEN,true);
	} else {
		status=nextarg("find in files: ", tline, MAXLLEN,true);
	}
    if (status != TRUE) return(FALSE);
	if(n>1) {
#if	TNOTES
		if(n==3) 
			status=snprintf(line,sizeof(line),"grep %s \"%s\" * 2>/dev/null |grep -v Binary > %s 2> /dev/null",flag_str,tline,filnam);
		else
#endif
			status=snprintf(line,sizeof(line),"grep %s \"%s\" * 2>/dev/null |grep -v Binary > %s 2> /dev/null",flag_str,tline,filnam);
	} else {
		status=snprintf(line,sizeof(line),"grep %s \"%s\" * 2>/dev/null |grep -v Binary > %s 2> /dev/null",flag_str,tline,filnam);
	}
//	MESG("grep: [%s]",line);
	if(status>MAXFLEN) return FALSE;


//	MESG("      : current dir [%s], seatch dir [%s] ---------------",getcwd(tline,MAXLLEN),search_dir);
	if(sysexec(line)){

	/* create and clear the command buffer */
	bp=get_filebuf(bname,NULL,0);
	if(bp) {
		if(bp->b_nwnd>0) {
			// set [grep] as active window
			change_window(find_buffer_window(bp));
		};
	} else {
		if((bp=new_filebuf(bname,0)) != FALSE) {
		
		} else {	
			msg_line("cannot create grep buffer");
			return(0);
		};
	};
	EmptyText(bp);
	sync();
	if(strncmp(bp->b_dname,search_dir,MAXFLEN)) {
		strlcpy(bp->b_dname,search_dir,sizeof(bp->b_dname));
	};
	select_filebuf(bp);
	empty_filebuf(bp);

//	MESG("	: current buffer dir is [%s]",cbfp->b_dname);
	ifile(bp,filnam,0);
	bp->b_state &= ~FS_CHG;
	if(strncmp(bp->b_dname,search_dir,MAXFLEN)) {
		strlcpy(bp->b_dname,search_dir,sizeof(bp->b_dname));
	};

	set_Offset(0);
	set_hmark(1,"grep_cmd");

	/* and get rid of the temporary file */
	unlink(filnam);
	if(chdir(search_dir)) {
		msg_line("cannot chdir to %s",search_dir);
		return false;
	};

	return(TRUE);
	} else {
		return(FALSE);
	};
}

int rgrep_cmd(num n)
{
	// MESG("rgrep_cmd: n=%d",n);
	return grep_cmd(2);	
}

/*
 * filter a buffer through an external program
 * emacs key ^X#, assigned also to alt-3
 */
int filter_buffer(num nuse)
{
	int    s;		/* return status from CLI */
	FILEBUF *bp;	/* pointer to buffer to zot */
    char tline[MAXLLEN];	/* command line send to shell */
	static char filnam2[MAXFLEN];

	s = set_unique_tmp_file(filnam2,"command",MAXFLEN);

	/* get the filter name and its args */
	set_list_type(LSHELL);
	tline[0]=0;
    if ((s=nextarg("filter command > ", tline, MAXLLEN,true)) != TRUE) return(s);

	/* setup the proper file names */
	bp = cbfp;

	strlcat(tline," >",MAXLLEN);
	strlcat(tline,filnam2,MAXLLEN);
	strlcat(tline," < ",MAXLLEN);
	strlcat(tline,bp->b_dname,MAXLLEN);
	strlcat(tline,"/",MAXLLEN);
	strlcat(tline,bp->b_fname,MAXLLEN);
	strlcat(tline," 2> /dev/null",MAXLLEN);

	s=sysexec(tline);

	if(!s)
	{
		unlink(filnam2);
		return(FALSE);
	} else {
	/* reset file name */
		FILEBUF *filter_buffer;
		filter_buffer = get_filebuf("[filter]",NULL,0);
		if(filter_buffer==NULL) {
			filter_buffer=new_filebuf("[filter]",0);
			split_window(1);
			next_window(1);
		} else { // find window with [filter] buffer and use it
			WINDP *filter_window;
			filter_window=find_buffer_window(filter_buffer);
			if(filter_window) {
				cwp=filter_window;
			} else {
				next_window(1);
			}
		};
		select_filebuf(filter_buffer);
		EmptyText(filter_buffer);
		ifile(filter_buffer,filnam2,1);

		set_hmark(1,"filter_cmd");
		set_update(cwp,UPD_EDIT+UPD_ALL);
		unlink(filnam2);
		cwp = find_buffer_window(bp);
		select_filebuf(bp);
		return(TRUE);
	};
}

char *display_env=NULL;
char *clip_copy=NULL;
char *clip_paste=NULL;
int ext_clipboard_command=0;

char *xclip_copy="xclip -o";
char *xclip_paste="xclip -i";

#if	DARWIN
char *native_paste0="pbpaste";
char *native_paste="pbpaste";
char *native_copy="pbcopy";
#endif
#if	WSL
// The following is for wsl!
char *native_paste0="win32yank.exe -o --lf";
char *native_paste="win32yank.exe -o --lf";
char *native_copy="win32yank.exe -i";
#endif

#if	DARWIN | WSL
int check_native_copy()
{
 int status;
 static char exec_st[MAXFLEN];
 MESG("check_native_copy:");
	ext_clipboard_command=0;
	status=snprintf(exec_st,sizeof(exec_st),"%s > /dev/null 2> /dev/null",native_paste);
	if(status>=MAXFLEN) return 0;
	status = system(exec_st);
	MESG("check_native_copy:[%s] -> %d",exec_st,status);
	if(status == 0) {
		clip_copy=native_paste;
		clip_paste=native_copy;
		ext_clipboard_command=1;
		MESG("system clipboard uses %s",native_paste);
		return 1;
	};
	MESG("native clipboard command not found!\n");
	return 0;
}
#endif

int init_system_clipboard()
{
 static char exec_st[MAXFLEN];
 int status=0;
	if(status>=MAXFLEN) return 0;
	MESG("init_system_clipboard:");
	display_env = getenv("DISPLAY");
	if(display_env == NULL) {
		return(check_native_copy());
		// return 0; 
	} else {
		if(strlen(display_env)<1) {
			return 0;
		};
		ext_clipboard_command=1;
	};
// we suppose that we have xclip for start
// pass clipboard through xclip
//	status=snprintf(exec_st,sizeof(exec_st),"xclip -i > /dev/null 2> /dev/null <xclip -o 2>/dev/null");
	status=snprintf(exec_st,sizeof(exec_st),"echo \"\" | xclip -i > /dev/null 2> /dev/null");
	if(status>=MAXFLEN) return 0;
	status = system(exec_st);
	// MESG("exec [%s] status=%d",exec_st,status);
	if((status!=0)) {
		ext_clipboard_command=0;
		return(check_native_copy());
	} else {
		clip_copy=xclip_copy;
		clip_paste=xclip_paste;
		MESG("system clipboard uses xclip");
		ext_clipboard_command=1;
		return 1;
	};
	return(check_native_copy());
}

/* return the first line from system clipboard */
char *ext_system_paste_line()
{
 static char line[MAXLLEN];
 char filnam[MAXFLEN];
 char exec_st[MAXFLEN];
 int status=0;
 
 memset(line,0,sizeof(line));
	if(!ext_clipboard_command) return line;

	status = set_unique_tmp_file(filnam,"paste",MAXFLEN);
	if(status>=MAXFLEN) return line;
	status=snprintf(exec_st,sizeof(exec_st),"%s > %s 2> /dev/null",clip_copy,filnam);
	if(status>=MAXFLEN) return line;

	status = system(exec_st);
	if(status==0) {
		FILE *file;
		char *s=line;
		file=fopen(filnam,"r");
		int i;
		char ch;
		size_t stat;
		for(i=0;i<MAXLLEN;i++) {
			stat=fread(&ch,1,1,file);
			if(stat==0) break;
			if(ch==CHR_NEWLINE||ch==CHR_CR) break;
			*s++=ch;
		};*s=0;
		fclose(file);
		unlink(filnam);
		return line;
	};

	return line;
}    

int utf8charlen_nocheck(int ch);
void undo_set_lock(FILEBUF *fp);
void undo_clear_lock(FILEBUF *fp);

int insert_text_file_as_column(char *filnam)
{
	offs o1=Offset();	/* original offset  */
	FILEBUF *ori_buf = cbfp;

	const num start_column=tp_col(cbfp->tp_current);
	// MESG("#insert_text_file_as_column: position %ld ---------------",start_column);

	FILEBUF *tmp_bp = new_filebuf(filnam,0);
	if(tmp_bp==NULL) return false;

	if(!select_filebuf(tmp_bp)) return false;
	// set_Offset(FSize(tmp_bp));

	const num max_len = tmp_bp->maxlinelen;

	char *pad_space = (char *)malloc(start_column+max_len+2);
	if(pad_space==NULL) { delete_filebuf(tmp_bp,1); return false;};
	// memset(pad_space,' ',start_column+max_len+2);
	// set_Offset(0);
	num line_start=0;

	select_filebuf(ori_buf);
	char *ml_out;
	while(line_start<FSize(tmp_bp)) {
		char *line_text=get_line_at(tmp_bp,line_start);
		int col=0, in_offset=0;;

		ml_out = pad_space;
		num line_end_column=tp_col(cbfp->tp_current);
		// memset(pad_space,'^',start_column+max_len+2);
		// MESG("	line_end=%2d start_column=%2d",line_end_column,start_column);
		if(line_end_column<start_column) {
			memset(pad_space,' ',start_column-line_end_column);
			ml_out=pad_space+(start_column-line_end_column);
		};
		// insert_string(cbfp,"|",1);
		while(in_offset<strlen(line_text)) {
			utfchar uc;
			in_offset=SUtfCharAt(line_text,in_offset,&uc);
			if(uc.uval[0]==CHR_TAB) {col=next_tab(col);}
			else col+=get_utf_length(&uc);
			memcpy(ml_out,&uc,utf8charlen_nocheck(uc.uval[0]));
			ml_out+=utf8charlen_nocheck(uc.uval[0]);
		};
		
		while(col++<max_len) { *ml_out++=' ';}; *ml_out=0;
		line_start = FNextLine(tmp_bp,line_start);
		
		if(FEof(cbfp)) {
			memcpy(ml_out,cbfp->EolStr,cbfp->EolSize);
			ml_out+=cbfp->EolSize;
			insert_string(cbfp,pad_space,ml_out-pad_space);
		} else {
			insert_string(cbfp,pad_space,ml_out-pad_space);
			num current_line = get_current_line();
			if(current_line==tp_line(cbfp->tp_text_end)){
				set_Offset(FSize(cbfp));
				insert_newline(cbfp);
			} else {
				MoveLineCol(current_line+1,start_column);
			}
		};
		// MESG("		after insert %ld",tp_col(cbfp->tp_current));
	} 
	free(pad_space);
	undo_set_noglue();
	delete_filebuf(tmp_bp,1);

	// goto original position
	set_Offset(o1);
	// MESG_time("paste_as_column:end");
	return 1;
}

// Insert text file converting with correct nl
int insert_text_file_nl(char *filnam)
{
	FILEBUF *ori_buf = cbfp;

	

	FILEBUF *tmp_bp = new_filebuf(filnam,0);
	if(tmp_bp==NULL) return false;

	if(!select_filebuf(tmp_bp)) return false;
	if(tmp_bp->maxlinelen==0) tmp_bp->maxlinelen=FSize(tmp_bp);

	const num max_len = tmp_bp->maxlinelen;
	// MESG("#insert_text_file_nl: position  --------------- max_len=%d",max_len);

	char *pad_space = (char *)malloc(max_len+3);
	if(pad_space==NULL) { delete_filebuf(tmp_bp,1); return false;};
	num line_start=0;

	select_filebuf(ori_buf);
	char *ml_out;
	while(line_start<FSize(tmp_bp)) {
		char *line_text=get_line_at(tmp_bp,line_start);
		int col=0, in_offset=0;;
		// MESG("- in [%s]",line_text);
		ml_out = pad_space;

		while(in_offset<strlen(line_text)) {
			utfchar uc;
			in_offset=SUtfCharAt(line_text,in_offset,&uc);
			if(uc.uval[0]==CHR_TAB) {col=next_tab(col);}
			else col+=get_utf_length(&uc);
			// MESG("	memcpy");
			memcpy(ml_out,&uc,utf8charlen_nocheck(uc.uval[0]));
			ml_out+=utf8charlen_nocheck(uc.uval[0]);
		};

		line_start = FNextLine(tmp_bp,line_start);
		if(FEof(cbfp)) {
			// MESG("	eof insert now! [%s] ",pad_space);
			memcpy(ml_out,cbfp->EolStr,cbfp->EolSize);
			// MESG("	add new line");
			ml_out+=cbfp->EolSize;
			// MESG("	size is %d",ml_out-pad_space);
			insert_string(cbfp,pad_space,ml_out-pad_space);
		} else {
			// MESG("	insert now! [%s]",pad_space);
			insert_string(cbfp,pad_space,ml_out-pad_space);
			if(!FEofAt(tmp_bp,line_start)) insert_newline(cbfp);
		};
		// MESG("		ok1!");
	};
	// MESG("ok free now!"); 
	free(pad_space);
	// MESG("ok");
	undo_set_noglue();
	delete_filebuf(tmp_bp,1);

	// goto original position
	// set_Offset(o1);
	return 1;
}

#if	NUSE
int insert_text_file(char *filnam)
{
	offs o1=Offset();	/* original offset  */
	int status;
	struct stat    st;
	long int size;
	long long int act_read;
	int file=open(filnam,O_RDONLY);
	if(file<1) return false;
	fstat(file,&st);
	size = st.st_size;
	status = ReadBlock(filnam,file,size,&act_read);
	if(status) {
	o1 += act_read;
	close(file);
	set_Offset(o1);
	return true;
	} else return false;
}
#endif

int ext_system_paste()
{
 static char filnam[MAXFLEN];
 static char exec_st[MAXFLEN];
 int status=0;

	if(!ext_clipboard_command) return 0;
	status = set_unique_tmp_file(filnam,"command",MAXFLEN);
	if(status>=MAXFLEN) return 0;

#if	GTK
	status = x_insert_to_file(filnam);
#else
	status=snprintf(exec_st,sizeof(exec_st),"%s > %s 2> /dev/null",clip_copy,filnam);
	if(status>=MAXFLEN) return 0;

	status = system(exec_st);
#endif

	if(status==0) {

		if(cwp->selection==REGION_COLUMN) {	/* Column past  */
			status=insert_text_file_as_column(filnam);
		} else {	/* Normal paste  */
			status=insert_text_file_nl(filnam);
		};
		unlink(filnam);
		set_update(cwp,UPD_EDIT);
		setmark(0);
		return status;
	};
	error_line("cannot insert system clipboard!");
	return 0;
}

extern struct ClipBoard *MainClipBoard;

int ext_system_copy()
{
static char filnam[MAXFLEN];
static char exec_st[MAXFLEN];
int status;
FILE *clip_file; 
	// MESG("ext_system_copy: x11_ok = %d",ext_clipboard_command);
	if(!ext_clipboard_command) return 0;
	// MESG("ext_system_copy: %d chars",MainClipBoard->rect);
	status = set_unique_tmp_file(filnam,"command",MAXFLEN);
	if(status>=MAXFLEN) return 0;
	// put clipboard to filnam
	clip_file=fopen(filnam,"w");
	if(clip_file){
		if(MainClipBoard->height>1) fwrite(MainClipBoard->text,MainClipBoard->rect-1,1,clip_file);
		else fwrite(MainClipBoard->text,MainClipBoard->width,MainClipBoard->height,clip_file);
		fclose(clip_file);
		status=snprintf(exec_st,sizeof(exec_st),"(cat %s |%s)  2> /dev/null",filnam,clip_paste);
		if(status>=MAXFLEN) return 0;

		status = system(exec_st);
		unlink(filnam);
		if(status !=0) return 0;
		else return 1;
	};
	return 1;
}

int mkdirRecursive(const char *path, mode_t mode)
{
    char opath[PATH_MAX];
    size_t len;
	int stat;
	char *p;
//	MESG("mkdirRecursive:path=[%s]",path);
    strlcpy(opath, path, sizeof(opath));
    opath[sizeof(opath) - 1] = '\0';
    len = strlen(opath);
//	MESG("mkdirRecursive: opath=[%s]",opath);
    if (len == 0)
        return 0;
    else if (opath[len - 1] == '/')
        opath[len - 1] = '\0';
    for(p = opath+1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
//			MESG("mkdir:check [%s]",opath);
            if (access(opath, F_OK)){
                stat=mkdir(opath, mode);
//				MESG("	create stat=%d",stat);
				if(stat!=0) return 1;
			};
            *p = '/';
        }
	};
    if (access(opath, F_OK)) {        /* if path is not terminated with / */
//		MESG("mkdir:last check [%s]",opath);
        stat=mkdir(opath, mode);
//		MESG("	create stat=%d",stat);
	};
	return stat;
}
