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

struct timeval start_time;	/* editor time started stamp */

void set_start_time()
{
	gettimeofday(&start_time,NULL);
}

int set_unique_tmp_file(char *str,char *name,int size)
{
 int len;
 len=snprintf(str,size,"/tmp/%s_%ld_%ld.txt",name,start_time.tv_sec,(long)start_time.tv_usec);
 if(len>size) {MESG("tmp file %s truncated!",name);return false;};
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
		if(chdir(get_start_dir())!=0) { MESG("cannot change dir to start_dir %s",get_start_dir());};
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
	status=snprintf(tag_file,MAXFLEN,"%s.tag",cbfp->b_fname);
	if(status>=256) { msg_line("string overflow 1 in view_functions");events_flush();return false;};

	status=snprintf(tag_cmd,MAXFLEN,"ctags -x --c-kinds=%s %s 2>err1 | awk '{printf \"%%4s %%s %%s %%s %%s %%s %%s\\n\", $3,$6,$7,$8,$9,$10,$11}' > %s 2>/dev/null",kind,cbfp->b_fname,tag_file);
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
#if	0
	if(!macro_exec) 
	{
		select_filebuf(old_cbfp);
	};
#endif
	msg_line("ok!");
	return(TRUE);
	} else {
		set_update(cwp,UPD_MOVE);
		return (FALSE);
	};
}

/* execute a background system command */
int bg_cmd(num n)
{
 char line[MAXLLEN];
 char fname[MAXFLEN+1];
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
		stat=snprintf(fname,MAXFLEN,"%s/%s",cbfp->b_dname,cbfp->b_fname);
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
	}
	strlcat(line,"&",MAXLLEN);
	sysexec(line);
	set_update(cwp,UPD_MOVE);
    return (TRUE);
}

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
			status=snprintf(line,MAXLLEN,"grep %s \"%s\" * 2>/dev/null |grep -v Binary > %s 2> /dev/null",flag_str,tline,filnam);
		else
#endif
			status=snprintf(line,MAXLLEN,"grep %s \"%s\" * 2>/dev/null |grep -v Binary > %s 2> /dev/null",flag_str,tline,filnam);
	} else {
		status=snprintf(line,MAXLLEN,"grep %s \"%s\" * 2>/dev/null |grep -v Binary > %s 2> /dev/null",flag_str,tline,filnam);
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
int x11_display_ok=0;

char *xclip_copy="xclip -o";
char *xclip_paste="xclip -i";

#if	DARWIN
char *pbpaste="pbpaste";
char *pbcopy="pbcopy";

// check bpcopy/pbpaste on mac
int check_pbcopy()
{
 int status;
 static char exec_st[MAXFLEN];

	status=snprintf(exec_st,MAXFLEN,"pbpaste > /dev/null 2> /dev/null");
	if(status>=MAXFLEN) return 0;
	status = system(exec_st);
	if(status == 0) {
		clip_copy=pbpaste;
		clip_paste=pbcopy;
		x11_display_ok=1;
		return 1;
	};
	return 0;
}
#endif

int init_system_clipboard()
{
 static char exec_st[MAXFLEN];
 int status=0;
	if(status>=MAXFLEN) return 0;

	display_env = getenv("DISPLAY");
#if	DARWIN
	if(check_pbcopy()) return 1;
#endif
	if(display_env == NULL) {
		return 0; 
	} else {
		if(strlen(display_env)<1) {
			return 0;
		};
		x11_display_ok=1;
	};
// we suppose that we have xclip for start
// pass clipboard through xclip
	status=snprintf(exec_st,MAXFLEN,"xclip -i > /dev/null 2> /dev/null <xclip -o 2>/dev/null");
	if(status>=MAXFLEN) return 0;
//	MESG("exec [%s]",exec_st);
	status = system(exec_st);
	if((status%256) !=0) {
		fprintf(stderr,"status %d cannot execute xclip\n",status);
		return 0;
	} else {
		clip_copy=xclip_copy;
		clip_paste=xclip_paste;
		x11_display_ok=1;
		return 1;
	};
	x11_display_ok=0;
	return 0;
}

/* return the first line from system clipboard */
char *ext_system_paste_line()
{
 static char filnam[MAXFLEN];
 static char exec_st[MAXFLEN];
 int status=0;

	if(!x11_display_ok) return "";
	if(dont_edit() || cbfp->b_flag & FSDIRED )return false;
	status = set_unique_tmp_file(filnam,"command",MAXFLEN);
	if(status>=MAXFLEN) return "";
	status=snprintf(exec_st,MAXFLEN,"%s > %s 2> /dev/null",clip_copy,filnam);
	if(status>=MAXFLEN) return "";

	status = system(exec_st);
	if(status==0) {
		FILE *file;
		static char line[MAXLLEN];
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

	return "";
}    

int utf8charlen_nocheck(int ch);

#if	0        
int ext_system_paste()
{
 static char filnam[MAXFLEN];
 static char exec_st[MAXFLEN];
 int status=0;

	if(!x11_display_ok) return 0;
	if(dont_edit() || cbfp->b_flag & FSDIRED )return false;

	status = set_unique_tmp_file(filnam,"command",MAXFLEN);
	if(status>=MAXFLEN) return 0;
	status=snprintf(exec_st,MAXFLEN,"%s > %s 2> /dev/null",clip_copy,filnam);
	if(status>=MAXFLEN) return 0;

	status = system(exec_st);
	if(status==0) {
		int file;
		struct stat    st;
		long int size;
		long long int act_read;
		offs o1=Offset();
		file=open(filnam,O_RDONLY);
		fstat(file,&st);
		size = st.st_size;
		status = ReadBlock(filnam,file,size,&act_read);
		o1 += act_read;
		close(file);
		unlink(filnam);
		set_Offset(o1);
		set_update(cwp,UPD_EDIT);
		setmark(0);
		return status;
	};
	msg_line("cannot insert system clipboard!");
	return 0;
}
#else
int ext_system_paste()
{
 static char filnam[MAXFLEN];
 static char exec_st[MAXFLEN];
 int status=0;

	if(!x11_display_ok) return 0;
	status = set_unique_tmp_file(filnam,"command",MAXFLEN);
	if(status>=MAXFLEN) return 0;
	status=snprintf(exec_st,MAXFLEN,"%s > %s 2> /dev/null",clip_copy,filnam);
	if(status>=MAXFLEN) return 0;

	status = system(exec_st);
	if(status==0) {
		FILEBUF *ori_buf = cbfp;
		offs o1=Offset();	/* original offset  */
		if(cwp->selection) {
		// MESG("#ext_system_paste_column:---------------");
		FILEBUF *tmp_bp = new_filebuf(filnam,0);
		select_filebuf(tmp_bp);
		set_Offset(FSize(tmp_bp));
		num max_len = tmp_bp->maxlinelen;
		char *ml = malloc(max_len+2);
		set_Offset(0);
		num line_start=0;
		// MESG("#ext_system_paste_column: %ld",max_len);

		select_filebuf(ori_buf);
		num start_column=tp_col(cbfp->tp_current);
		char *pad_space = (char *)malloc(start_column+1);
		memset(pad_space,'+',start_column+1);
		do {
			char *l=get_line_at(tmp_bp,line_start);
			memset(ml,0,max_len+1);
			// sprintf(ml,"%-*s",(int)max_len,l);
			int col=0, in_offset=0;;
			char *ml_out = ml;
			num line_end_column=tp_col(cbfp->tp_current);
			if(line_end_column<start_column) {
				insert_string(cbfp,pad_space,start_column-line_end_column);
			};
			while(in_offset<strlen(l)) {
				utfchar uc;
				in_offset=SUtfCharAt(l,in_offset,&uc);
				if(uc.uval[0]==CHR_TAB) {col=next_tab(col);}
				else col+=get_utf_length(&uc);
				memcpy(ml_out,&uc,utf8charlen_nocheck(uc.uval[0]));
				ml_out+=utf8charlen_nocheck(uc.uval[0]);
			};;
			
			while(col++<max_len) { *ml_out++=' ';}; *ml_out=0;
			line_start = FNextLine(tmp_bp,line_start);
			
			// MESG("insert:[%s]",ml);
			// insert_string(cbfp,ml,ml_out-ml);
			if(FEof(cbfp)) {
				insert_newline(cbfp);
				if(start_column>0) {
					// memset(ml,' ',start_column);
					// *(ml+start_column) = 0;
					// insert_string(cbfp,ml,start_column);
					insert_string(cbfp,pad_space,start_column);
				};
					insert_string(cbfp,ml,ml_out-ml);
			} else {
				insert_string(cbfp,ml,ml_out-ml);
				if(!next_line(1)) {
					set_Offset(FSize(cbfp));
					insert_newline(cbfp);
				};;
				if(FEof(cbfp)) {
					insert_newline(cbfp);
					if(start_column>0) {
						// memset(ml,' ',start_column);
						// *(ml+start_column) = 0;
						// insert_string(cbfp,ml,start_column);
						insert_string(cbfp,pad_space,start_column);
					};
				}
			};
		} while(line_start<FSize(tmp_bp));
		free(pad_space);
		free(ml);
		unlink(filnam);
		delete_filebuf(tmp_bp,1);
		set_update(cwp,UPD_ALL);
		} else {
		int file;
		struct stat    st;
		long int size;
		long long int act_read;
		file=open(filnam,O_RDONLY);
		fstat(file,&st);
		size = st.st_size;
		status = ReadBlock(filnam,file,size,&act_read);
		o1 += act_read;
		close(file);
		};
		unlink(filnam);
		set_Offset(o1);
		set_update(cwp,UPD_EDIT);
		setmark(0);
		return status;
	};
	msg_line("cannot insert system clipboard!");
	return 0;
}
#endif

extern struct ClipBoard *MainClipBoard;

int ext_system_copy()
{
static char filnam[MAXFLEN];
static char exec_st[MAXFLEN];
int status;
FILE *clip_file; 
	if(!x11_display_ok) return 0;

	status = set_unique_tmp_file(filnam,"command",MAXFLEN);
	if(status>=MAXFLEN) return 0;
	// put clipboard to filnam
	clip_file=fopen(filnam,"w");
	if(clip_file){
		fwrite(MainClipBoard->text,MainClipBoard->width,MainClipBoard->height,clip_file);
		fclose(clip_file);
		status=snprintf(exec_st,MAXFLEN,"(cat %s |%s)  2> /dev/null",filnam,clip_paste);
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
