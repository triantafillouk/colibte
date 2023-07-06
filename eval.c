/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

 	An interpreter by K.Triantafillou (2011).
	Embedded calculator added by K.Triantafillou
*/
 
#include	<math.h>
#include	<stdlib.h>
#include	"xe.h"
#if	DATE
#include	<time.h>
#endif

#include	"eval.h"
#include	"alist.h"
#include	"mlang.h"

#if	DATE
time_t	tclock;
#endif

extern BTREE *bt_table;
extern int nnarg;
extern alist *window_list;
extern char utfokey[];
extern int utflen;
extern int specialh;
extern FILEBUF *cbfp;

#if	RSESSION
extern int record_session;
#endif

int dofile(char *fname);
void set_vtype(int type);
int vtype_is(int type);

// Editor variables
char *fvars[] = {
	"_file_name",		/* CURRENT FILE NAME */
	"_file_dir",
	"_file_bflag",
	"_start_dir",
	"_char",			/* CURRENT CHARACTER UNDER THE CURSOR */
	"_search_pattern",		/* SEARCH PATTERN */
	"_replace_pattern",		/* REPLACEMENT PATTERN */
	"_time",			/* Current Time */
	// read only names
	"_regexp_match",	/* LAST REGULAR EXPRESSION MATCHED */
	"_column",		/* CURRENT COLUMN POS OF CURSOR */
	"_line",		/* CURRENT LINE IN FILE */
	"_position",	/* CURRENT pointer position  */
	"_base_name",		/* CURRENT FILE NAME WITHOUT EXTENSION */
	"_key",		/* LAST KEYBOARD CHAR STRUCK */
	"_text",			/* TEXT OF CURRENT LINE */
	"_current_line",		/* CURRENT SCREEN LINE IN WINDP */
	"_window_cols",		/* WIDTH OF CURRENT LINE */
	"_window_rows",		/* # OF LINES IN CURRENT WINDP */
	// --------------------------------------------
	"_found",		/* TRUE IF LAST SEARCH TRUE */
	"_next_word",		/* NEXT WORD IN LINE */

	"_debug",		/* MACRO DEBUGGING */

	"_fcolor",
	"_bcolor",
	"_case",
	"_over",
	"_tabsize",
	"_version",
	NULL
};

/* 	and its preprocesor definitions		*/

enum {
	EVCFNAME,
	EVCDNAME,	//	"_file_dir", readonly
	EVCBFLAG,	//	"_file_bflag", readonly
	EVSTARTDIR,	//	"_start_dir", readonly
	EVCURCHAR,
	EVSEARCH,
	EVREPLACE,
	EVCTIME,
	EVMATCH,
	EVCURCOL,
	EVCURLINE,
	EVCURPOS,
	EVBNAME,
	EVLASTKEY,
	EVTEXT,
	EVCWROW,
	EVWCOLS,
	EVWROWS,
	EVFOUND,
	EVNWORD,
	EVDEBUG,

	EVFCOLOR,
	EVBCOLOR,

	EVCASE,
	EVOVER,
	EVTABSIZE1,
	EVVERSION,
	EVREGEXP
};

void set_screen_update(int flag);
double compute_block(FILEBUF *bp,FILEBUF *use_fp,int start);
int   GetBlock(FILEBUF *fp,char *copy,offs from,offs size);

extern int drv_type;

short   kbdm[MAXLLEN];			/* keyboard macro buffer  */
short	*kbdptr;		/* current position in keyboard buf */
short	*kbdend = &kbdm[0];	/* ptr to end of the keyboard buffer */
#if	RSESSION
short   kbdm_session[MAXSESSION];			/* keyboard session buffer  */
short	*kbdsptr=&kbdm_session[0];		/* current position in keyboard session buf */
short	*kbdsession_end = &kbdm_session[0];	/* ptr to end of the keyboard session buffer*/
#endif
int	macbug = 0;		/* macro debuging flag		*/
extern int show_stage;

extern char search_pattern[];	// search pattern
extern char replace_pattern[];		// replace pattern
extern char patmatch[];

extern int found;

double exec_function(FILEBUF *vp,VAR *vargs,int nargs);
void show_points(FILEBUF *bf,FILE *fp);
int check_init(FILEBUF *bf);
void set_record_string1(char *st2);

char *time2a();
void record_session_key(short);
double compute_string(char *cc,char *out_string);

char subname[MAXFLEN];

void set_error(tok_struct *tok,int index,char *description);
void init_error();

extern char slval[];
// extern int ex_vtype;
extern int ex_edenv;
extern int err_num;
extern char *err_str;

int nnarg=1;

// set a numeric argument for next funtion to execute
int set_arg(int n)
{
	char arg[MAXSLEN];
	arg[0]=0;
	if ((nextarg("Arg : ", arg, MAXSLEN,true)) != TRUE) 	return(FALSE);
	nnarg=get_val();

	return OK_ARG;
}

 	/* this is editors loop!  */
void init_message_loop()
{
  nnarg=1;
}

/* Find a node named on main table  */
BTNODE *find_bt_element(char *name)
{
 BTNODE *btn;
 btn = find_btnode(bt_table,name);
 return(btn);
}

int set_debug(int n)
{
	macbug=n;
	return true;
}

int debug_flag(int n)
{
	return macbug;
}

void increase_debug_flag()
{
	macbug++;
}

void decrease_debug_flag()
{
	macbug--;
	if(macbug<0) macbug=0;
}

/* This is editors environment get function */
double get_env(int vnum)
{
	char *getkill();
	static double value=0;
	char svalue[MAXLLEN];
	int v1=0;
	svalue[0]=0;

	ex_edenv=TOK_ENV;

	/* fetch the appropriate value */
	if(cbfp == NULL) return(0.0);
	switch (vnum) {
		case EVCFNAME:	strlcpy(svalue,cbfp->b_fname,MAXLLEN);
			break;
		case EVCDNAME:	strlcpy(svalue,cbfp->b_dname,MAXLLEN);
			break;
		case EVCBFLAG:	v1 = cbfp->b_flag; break;
		case EVSTARTDIR: strlcpy(svalue,get_start_dir(),MAXLLEN);
		case EVCURCHAR:	v1=Char();	break;
		case EVSEARCH:	strlcpy(svalue,search_pattern,MAXLLEN);break;
		case EVREPLACE:	strlcpy(svalue,replace_pattern,MAXLLEN);break;
		case EVCTIME:	strlcpy(svalue,time2a(),MAXLLEN);break;
		case EVMATCH:	strlcpy(svalue,patmatch,MAXLLEN);
		case EVCURCOL:	v1 = FColumn(cbfp,Offset()); break;
		case EVCURLINE: v1 = getcline();break;
		case EVCURPOS: v1 = Offset();	break;
		case EVBNAME:	strlcpy(svalue,cbfp->b_fname,MAXLLEN);break;
		case EVLASTKEY: v1 = lastkey;break;
		case EVTEXT:	
			v1 = FLineEnd(cbfp,Offset())-LineBegin(Offset());
			if(v1>MAXLLEN-1) v1=MAXLLEN-1;
			get_text_offs(cbfp,svalue,LineBegin(Offset()), v1);
			break;
		case EVCWROW:	v1 = getwline();break;
		case EVWCOLS:	
			v1 = cwp->w_ntcols;
			break;
		case EVWROWS:	v1 = cwp->w_ntrows;break;
		case EVFOUND:	v1 = found;break;
		case EVNWORD:	
			strlcpy(svalue,getnword(),MAXLLEN);
			break;
		case EVDEBUG:	v1=macbug; return(v1); break;

		case EVFCOLOR:	v1=cwp->w_fcolor;break;
		case EVBCOLOR:	v1=cwp->w_bcolor;break;

		case EVCASE:	v1=gmode_exact_case;break;
		case EVOVER:	v1=gmode_over;break;
		case EVREGEXP:	v1=gmode_reg_exp; break;
		case EVTABSIZE1:	
			v1=tabsize;
			break;
		case EVVERSION:
			strlcpy(svalue,VERSION,MAXLLEN);break;
		default:
			ERROR("GET_ENV: Not a valid function err=503");
			ex_edenv=0;
			return value;
	};
	value=v1;
	if(svalue[0]!=0) { 
		set_vtype(VTYPE_STRING);
		set_sval(svalue);	
		value=next_value();
	};
	return(value);
}


 /* Select an editor variable and edit its value */
int set_option(int n)
{
 int nv;
 char value[MAXSLEN];
 char prompt[80];
 BTNODE *btn;

// if(list_on()) return 0;
// set_list_type(LDIR2);

 int num_options=0;
// while(option_names[num_options].name!=NULL) num_options++;
 num_options=4;	/* show only the first 4 option in the menu  */
 char **option_list;
 option_list=(char **)malloc(sizeof(char *)*(num_options+1));

// for(nv=0;option_names[nv].name!=NULL;nv++) 
 for(nv=0;nv<num_options;nv++)
 {
	btn=find_btnode(bt_table,option_names[nv].name);
 	if(btn->sval != NULL) snprintf(prompt,80,"%-10s:[%-5s]",option_names[nv].name,btn->sval);
	else snprintf(prompt,80,"%-10s:[%d]",option_names[nv].name,(int)btn->val);
	
	utf_string_break(prompt,20);
	option_list[nv]=strdup(prompt);
 };
 option_list[nv]=NULL;
 nv = selectl("Set preferences",option_list,num_options,20,10,1,20,-1);
 sarray_clear(option_list);


 if(nv>=0) {
	BTNODE *btn;
	btn=find_btnode(bt_table,option_names[nv].name);
	if(btn->sval!=NULL) snprintf(prompt,80,"%s [%s] : ",btn->node_name,btn->sval);
	else snprintf(prompt,80,"%s [%d]: ",btn->node_name,(int)btn->val);
	if(btn->sval!=NULL) strlcpy(value,btn->sval,MAXSLEN);
	else snprintf(value,MAXSLEN,"%d",(int)btn->val);

	getstring(prompt,value,MAXSLEN,true);
	
	if(value[0]==0) {
		set_update(cwp,UPD_MOVE);
		return FALSE;
	}
	return(set_option_val(nv,value));
 };
 return(FALSE);
}

short int get_next_key(int mode)
{
 if(mode==PLAY) {
 	if(kbdptr==kbdend-1) return-1;
 	return (*kbdptr++);
 }  else {
 	if(kbdsptr==kbdsession_end) return-1;
 	return (*kbdsptr++);
 };
}

/* set option values */
int set_option_val(int vnum,char *svalue)
{
 int v1;

 v1=atoi(svalue);
 
// MESG("set_option_val: vnum=%d [%s]",vnum,svalue);
		switch (vnum) {
		case EMFILLCOL:	{
			set_btval("fillcol",-1,NULL,v1);
			break;
			};
		case EMLMARGIN:	set_btval("lmargin",-1,NULL,v1);break;
		case EMTABSIZE: 
			set_btval("tabsize",-1,NULL,v1);
			set_env(EVTABSIZE1,"",v1);
			break;
		case EMPRECISION: set_btval("print_precision",-1,NULL,v1);break;
		case EMSHOWHEX: set_btval("show_hex",-1,NULL,v1);break;
		case EMKEYEMUL: {
			set_key_emulation(v1);
			break;
			}
		case EMBACKUP: set_btval("make_backup",-1,NULL,v1);break;
		case EMBEDICONS: set_btval("embed_icons",-1,NULL,v1);break;
		case EMSHOWPOS: {
			set_btval("show_position",-1,NULL,v1);
			break;
			};
		case EMVINFOCOL: {
			WINDP *wp;
			set_btval("show_vinfo",-1,NULL,v1);

			if(v1>0) {
				lbegin(window_list);
				while((wp=lget(window_list))!=NULL) {
						wp->w_infocol = VMICOLS;
				};
			} else {
				lbegin(window_list);
				while((wp=lget(window_list))!=NULL) {
						wp->w_infocol = 0;
				};
			};
			break;
			};
		case EMVLINES: {
			set_btval("show_vmlines",-1,NULL,v1);
			break;
			};
		case EMCOFFSET: {
			set_btval("show_coffset",-1,NULL,v1);
			break;
			};
		case EMCDATA: {
			set_btval("show_cdata",-1,NULL,v1);
			break;
			};
		case EMCCASE: {
			gmode_exact_case=v1;
			};break;
		case EMCOVER: {
			gmode_over=v1;
			};break;
		case EMCREGEXP: {
			gmode_reg_exp=v1;
			};break;
		 case EMSAVHIST: {
		 	set_btval("save_history",-1,NULL,v1);
			break;
		 };
		  case EMLARGEICONS: {
		  	set_btval("large_toolbar_icons",-1,NULL,v1);
		  	break;
		  };
		  case EMSAFEOPS: {
		  	set_btval("safe_ops",-1,NULL,v1);
		  	break;
		  };
		  case EMCOLORSCHEME: {
		  	set_btval("color_scheme",-1,NULL,v1);
		  	break;
		  };
		  case EMXCOLORSCHEME: {
		  	set_btval("xcolor_scheme",-1,NULL,v1);
		  	break;
		  };
		}

	set_update(cwp,UPD_ALL);
	return(1);
}

/* set editor variables */
void set_env(int vnum,char *svalue,double value)
{
 register int c;
 int v1;
 v1=value;
 if(cbfp == NULL) return;

		switch (vnum) {
		case EVCFNAME:	// set current file name
				strlcpy(cbfp->b_fname, svalue,MAXFLEN);
				/* update mode lines */
				set_update(cwp,UPD_STATUS);
				break;
		case EVCURCHAR:	// replace current character
				forw_delete(1);	/* delete 1 char */
				c = v1;
				if (c == '\n')
					insert_newline(cbfp);
				else
					insert_chr(1, c);
				prev_character(1);
				break;
		case EVSEARCH:	strlcpy(search_pattern, svalue,MAXLLEN);
				break;
		case EVREPLACE:	strlcpy(replace_pattern, svalue,MAXLLEN);
				break;
		case EVCURCOL:
				textpoint_set_lc(cwp->tp_current,
					tp_line(cwp->tp_current),v1);
				break;
		case EVCURLINE:	goto_line(v1);
				break;
		case EVCURPOS: 
				textpoint_set(cwp->w_fp->tp_current,v1);
				break;
		// _base_name is read only
		// _lastkey is read only ??
		case EVLASTKEY:	lastkey = v1;
				break;
		case EVTEXT:	set_linetext(svalue);break;
		case EVCWROW:	
			igotolinecol(v1+tp_line(cwp->tp_hline),1,0);
				break;
		case EVWCOLS: window_column_resize(v1);
				break;
		case EVWROWS: window_row_resize(v1);
				break;
		// _found is read only
		// _next_word is read only
		case EVDEBUG: 
				macbug = v1;
				return;
				break;
		case EVFCOLOR: 
			cwp->w_fcolor=value;
			set_update(cwp,UPD_WINDOW);
			break;
		case EVBCOLOR: 
			cwp->w_bcolor=value;
			set_update(cwp,UPD_WINDOW);
			break;
		case EVCASE: 
			gmode_exact_case=value;
			set_update(cwp,UPD_STATUS);
			break;
		case EVOVER: 
			gmode_over=value;
			set_update(cwp,UPD_STATUS);
			break;
		case EVREGEXP:
			gmode_reg_exp=value;	// ?????
			break;
		case EVTABSIZE1:
			if(value>1) {
				tabsize=value;
				set_btval("tabsize",-1,NULL,tabsize);
			};
			break;
		};
}

/* used only by macros */
int sindex(char *source, char *pattern)	/* find pattern within source */
{
	register char *sp;	/* ptr to current position to scan */
	register char *csp;	/* ptr to source string during comparison */
	register char *ct;
	MESG("sindex, position of [%s] in [%s]",pattern,source);
	/* scanning through the source string */
	for(sp=source;*sp;sp++) {
		/* scan through the pattern */
		for (csp=pattern,ct=sp;*sp;sp++,csp++) {
			if( *sp != *csp) break;
		}
		/* was it a match? */
		if (*csp == 0) {
			return((int)(ct - source) + 1);
			};
	}
	/* no match at all.. */
	return(0);
}

char *date_string(int n)
{
 static char date_str[128];
 int res=0;
	time_t	tclock;
	struct tm *tim;
	time(&tclock);
	tim = localtime(&tclock);
	switch(n) {
		case 2: res=snprintf(date_str,128,"%4d/%02d/%02d:%d:%d:%d, %s",tim->tm_year+1900,tim->tm_mon+1,tim->tm_mday,tim->tm_hour,tim->tm_min,tim->tm_sec,asctime(tim));
				break;
		case 1: res=snprintf(date_str,128,"%4d/%02d/%02d:%02d:%02d:%02d ",tim->tm_year+1900,tim->tm_mon+1,tim->tm_mday,tim->tm_hour,tim->tm_min,tim->tm_sec);
				break;
		case 0: res=snprintf(date_str,128,"%s",asctime(tim));
				break;
		case 3: res=snprintf(date_str,128,"%4d%02d%02d-%02d%02d%02d",tim->tm_year+1900,tim->tm_mon+1,tim->tm_mday,tim->tm_hour,tim->tm_min,tim->tm_sec);
				break;
	}
	if(res==129) MESG("cal name truncated");
	return date_str;
}

char *time2a()
{
 static char tline[32];
 time(&tclock);
 strlcpy(tline,date_string(0),32);
 tline[strlen(tline)-1]=0;
 return(tline);
}

/* modulo operation */
double modulo(double value,double factor)
{
 long int i1,i2;
 static double stat;
 i1=(long) value;
 i2=(long) factor;
 stat = i1%i2;
 return((double)stat);
}

/* initialize common variables for all driver versions  */
void init_common()
{
 specialh=1;
 syntaxh=1;
 /* initialize environment variables */
}

/* evaluate a string */
double compute_string(char *s,char *new_string)
{
 double value;
 FILEBUF *fp=NULL;
 char *bname="[dofile_string]";

	if ((fp = new_filebuf(bname, 0)) == NULL) /* get the needed buffer */
		return(FALSE);

	fp->b_flag |= FSINVS;
	EmptyText(fp);
 	init_error();
	initialize_vars();
	insert_string(fp,s,strlen(s));
	fp->b_type=1;
	value=compute_block(fp,cbfp,0);

	if(new_string) {
		get_text_offs(fp,new_string,0,FLineEnd(fp,0));
	};

	delete_filebuf(fp,0);
//	MESG("compute_string: [%s] result=%f",s,value);
	return(value);
}

/*	docmd:	take a passed string as a command line and execute it.
	Show the results on the status line.
	Works on interactive mode!
*/
int docmd(char *cmd)
{
 double value=0;
 value = compute_string(cmd,NULL);
 if(err_num>0) { msg_line("Error!: %s",err_str);return(FALSE);}
 else {
 	msg_line("Result of [%s] is : %f",cmd,value);
	return (TRUE);
 };
}

/*	macro_line:	Execute a macro command typed 
			by the user					*/
int macro_line(int nused)
{
	char cmd_name[MAXLLEN];		/* string holding command to execute */
	int status;
	set_list_type(LMAC);
	cmd_name[0]=0;
	/* get the line wanted */
	status = nextarg("Macro: ", cmd_name, MAXLLEN,true);
//	MESG("macro_line: [%s] status=%d",cmd_name,status);
	if (status != TRUE)	{
		return(status);
	}
	status=docmd(cmd_name);
//	MESG("macro_line: status=%d",status);
	return(status);
}


int exec_named_function(char *name)
{
    FILEBUF *bp;		/* ptr to buffer to execute */
    char bufn[MAXFLEN+2];		/* name of buffer to execute */
	int ival;

	/* find out what buffer to execute */
	strlcpy(bufn+1,name,MAXFLEN);

	/* construct the buffer name */
	bufn[0] = CHR_LBRA;
	strlcat(bufn, "]",MAXFLEN);
	// MESG("exec_named_function: name=%s bufn=%s",name,bufn);	
	/* find the pointer to that buffer */
    if ((bp=get_filebuf(bufn,NULL,FSINVS)) == NULL) 
	{
		msg_line("No function named %s",bufn);
		return(FALSE);
    };
	bp->b_type=1;	/* set file type to cmd  */

	/* parse the block if not already parsed  */
	parse_block1(bp,NULL,0,0);	/* do not init if already parsed!  */

	if((err_num=check_init(bp))>0) {
		return(0);
	};

	ival = (int)exec_function(bp,NULL,0);

	return(ival);
}

int execsub(int n)
{
//	MESG("execsub: %d %s",n,subname);
	return(exec_named_function(subname));
}

/* find and execute a macro file */
int exec_file(int n)	
{
	int status;	/* return status of name query */
	char fname[MAXFLEN];	/* name of file to execute */
	char *fspec=NULL;		/* full file spec */
	if(n<1) return FALSE;
	set_list_type(LDIR);
	fname[0]=0;
	status = nextarg("Macro file to execute: ", fname, MAXFLEN -1,true);

	if (status != TRUE)	{
		return(status);
	}
	/* look up the path for the file */
	if(fname[0]!='/' && fname[0]!=CHR_LBRA) fspec = find_file("cmds",fname,1,0);
	else fspec=fname;
	if(fspec==NULL) fspec=fname;	/* if not found try relative to current dir!  */
	/* if it isn't around */
	if (fspec == NULL)	{
		if(get_filebuf(fname,NULL,0)!=NULL) fspec=fname;
		else return FALSE;
	};
//	MESG("exec_file: [%s]",fspec);
	/* otherwise, execute it */
	while (n-- > 0) 
	{
		status=dofile(fspec);
//		MESG("exec_file: return status = %d",status);
		
		if (status != TRUE) return(status);
	};
	return(TRUE);	/* ERROR when it returns CHECK!! FIXIT!!!!  */
}

extern int show_stage;
extern int current_active_flag;

/* dofile:	execute a macro file */
int dofile(char *fname)
{
	FILEBUF *bp;	/* buffer to place file to execute */
	int status;	/* results of various calls */
	char bname[MAXFLEN];
	snprintf(bname,MAXFLEN,"[%s]",fname);
	// MESG("dofile:[%s]",fname);
	show_stage=0;
	set_screen_update(false);
	if((bp=get_filebuf(bname,NULL,0))==NULL) { // file not in memory, load it!
		if ((bp = new_filebuf(bname, 0)) == NULL) /* get the needed buffer */
			return(FALSE);

	/* and try to read in the file to execute */
		if(cbfp == NULL) cbfp=bp;
		// MESG("dofile: 10 %s",fname);
		if ((status = file_read1(bp,fname)) != TRUE) {
			return(status);
		};
	} else {
		if((bp->b_state & FS_ACTIVE)==0) {
			activate_file(bp);
			if ((status = file_read1(bp,fname)) != TRUE) {

			return(status);
		};
		} else {
			activate_file(bp);
		};
	};
	/* go execute it! */
	int backup_caf=current_active_flag;
	double d = compute_block(bp,bp,1);
	set_dval(d);
	current_active_flag=backup_caf;
	/* if not displayed, remove the now unneeded macro buffer and exit */
	if (bp->b_nwnd == 0) 
		delete_filebuf(bp,1);
	set_screen_update(true);

	if(err_num>0) return(FALSE);
	return(TRUE);
}




/*
 * Finds a function that corresponds to a keystroke and executes it.
 * Return the status of the  command.
 */
int execute(int c, int  n)
{
    int status=0;
	int (*execfunc)();		/* ptr to function to execute */

	if(cbfp==NULL) return(0);
	execfunc = key_function(c,1);
	if (execfunc != NULL) {
		status	 = (*execfunc)(n);
		return (status);
	};

	if(dont_edit()) return FALSE;
	if(cbfp->b_flag & FSDIRED) return(FALSE);

    if ((c>=0x20 && c<=0xFF )) {	/* Self inserting.      */
		if ( cbfp->b_flag & FSMMAP) return FALSE;
        status = insert_chr(n, c);
		if(gmode_over && cwp->selection!=REGION_COLM && !Eol()) status = del_char(n);
        if(status) return (OK_CLRSL);
		else return FALSE;
	}

	msg_line("[Key (%s) not assigned!]",xe_key_name(c));		/* complain		*/
    return (FALSE);
}

// all keystrokes come here
void main_execute(int c)
{
 static int cflag=0;
//	MESG("main_execute: %X nnarg=%d",c,nnarg);
	/* and execute the command */
	cflag=execute(c, nnarg);
//	MESG("		: cflag = %X",cflag);
	if(cflag!=OK_ARG) nnarg=1;
	else {
		if(nnarg>1) {
			msg_line("arg: %d",nnarg);
		};
	};

	if(cflag & OK_RSTGOAL)	cwp->goal_column=GetCol();
//	if(cflag==OK_CLRSL  || cflag==OK_RSTGOAL) clear_message_line();
}

/* get a string from keyboard macro buffer */
int set_play_string(char *st1)
{
 char *s=st1;

	while(*kbdptr !=0x14D && kbdptr<kbdend-1 && *kbdptr!=0) {
		*s++ = *kbdptr++;
	}; 
	kbdptr++;
	*s=0;
//	MESG("set_play_string [%s]",st1);
	return (TRUE);
}

#if	RSESSION
int set_play_string1(char *st1)
{
 char *s=st1;
// MESG("set_play_string:");
	while(*kbdsptr !=0x14D && kbdsptr<kbdsession_end-1 && *kbdsptr!=0) {
		*s++ = *kbdsptr++;
	}; 
	kbdsptr++;
	*s=0;
//	MESG("set_play_string: [%s]",st1);
	return (TRUE);
}
#endif

void record_key(short ks)
{
// MESG("  record_key: %0X",ks);
 if(kbdmode==RECORD) {
	  *kbdptr++ = ks;
	if(cbfp->b_lang==0 && ks<256) {
		int len=utf8_countbytes(ks);
		int n;
//		MESG("	record_key: len=%d",len);
		if(len>1){
			for(n=1;n<len;n++){
				*kbdptr++ = utfokey[n];
			};
		}
	};
  };
#if	RECORD_SESSION
  if(record_session) {
  	record_session_key(ks);
	if(cbfp->b_lang==0 && ks<256) {
		int len=utf8_countbytes(ks);
		int n;
		// MESG("	record_session_key: len=%d",len);
		if(len>1){
			for(n=1;n<len;n++){
				record_session_key(utfokey[n]);
			};
		}
	};
  };
#endif
}

/* move a string to keyboard macro buffer */
void set_record_string(char *st2)
{
 char *s=st2;
 if(kbdmode==RECORD){
// MESG("set_record_string [%s]",st2);
 while(*s !=0) {
	*kbdptr++ = *s++;
	};
 *kbdptr++=0x14D; // this is new line editor key!
 };
#if	RECORD_SESSION
 if(record_session) {
	set_record_string1(st2); 
 };
#endif
}


#if	RSESSION
void set_record_string1(char *st2)
{
 char *s=st2;
// MESG("set_record_string1: [%s]",st2);
 while(*s !=0) {
	*kbdsptr++ = *s++;
	};
// *kbdsptr++='\n';
 *kbdsptr++=0x14D;
 *kbdsptr=0;
 kbdsession_end=kbdsptr;
}
#endif

/* record keys and form a macro to be executed later */
/* Assigned to F7 */
int record_macro(int n)
{
	if (kbdmode == STOP) {
		msg_line("[Start macro]");
		kbdptr = &kbdm[0];
		kbdend = kbdptr;
		kbdmode = RECORD;
	} else {
		msg_line("[End macro]");
		kbdmode = STOP;
		*kbdptr=0;
		kbdend=kbdptr;
		if(n>1) kbdend++; // it is not called from keyboard so the last one is counted!
	}
	return(TRUE);
}


int replay(int mode,int ntimes)
{
 short *start;
// short *end;
 kbdmode=mode;

 // MESG("replay: mode=%X  ----------------------",mode);
 while(ntimes--)
 {
  int c;

 if(mode==PLAY) {
	start = &kbdm[0];
//	end   = kbdend;
	kbdptr = start;
 } else {
 	start = &kbdm_session[0];
//	end   = kbdsession_end;
	kbdsptr = start;
 };

 while((c=get_next_key(mode))>=0)
 {
//	MESG(" replay key: 0x%X",c);
	if(cbfp->b_lang==0 && c<256) {
		int n=utf8_countbytes(c)-1;
		int c1;
		utflen=n+1;
		if(n>0){
			utfokey[0]=c;
			for(n=1;n<utflen-1;n++) {
				c1=get_next_key(mode);
				utfokey[n]=c1;
			};
			c=get_next_key(mode);
		utfokey[utflen-1]=c;
		utfokey[utflen]=0;
		} else utflen=0;
	};
	lastkey=c;
	main_execute(c);
	events_flush();
 };
 };

 kbdmode=STOP;
// MESG("replay: ended ok!!!!!!!!!!!!!!!");
 return TRUE;
}

int exec_macro(int ntimes)
{
 if (kbdmode != STOP) {
    msg_line("Macro already active");
    return(FALSE);
 };
 return replay(PLAY,ntimes);
}

#if	RSESSION
void record_session_key(short ks)
{
 static int show_message=1;
 if(!record_session) return;
// MESG("record_session_key: %3d ks=%d = 0x%X",kbdsession_end-&kbdm_session[0]+1,ks,ks);
 if(kbdsptr>kbdm_session+MAXSESSION-2) {
	if(show_message) { 
		msg_line("keys more than %d!, stopped recording.",MAXSESSION);
		show_message=0;
	};
	return;
 };
  *kbdsptr++ = ks;
  kbdsession_end=kbdsptr;
//  MESG("	number of keys = %ld",kbdsession_end-&kbdm_session[0]);
}

/* read execute the session file  */
int execute_session(char *fname)
{
 int len,f;
 int i,count;
 record_session=0;
// MESG("execute_session: read [%s]",fname);
 f=open(fname, O_RDONLY);
 if(f>2) {
 	count=read(f,(void *)&len,sizeof(int));
//	MESG("execute_session: len=%d count=%d",len,count);
	kbdsptr=&kbdm_session[0];
	for(i=0;i<len;kbdsptr++,i++) {
		short int ikey;
		count=read(f,(void *)&ikey,sizeof(short int));
//		MESG("read key 0x%X",ikey);
		*kbdsptr=ikey;
		if(count==0) break;
 	};
	kbdsession_end=kbdm_session+len;
	kbdsptr=kbdm_session;
	close(f);
//	Replay session
	return replay(BPLAY,1);
 } else {
 	ERROR("could not open '%s' session file f=%d!\n",fname,f);
	return FALSE;
 };
}

/* load, replay session */
int load_session(int n)
{
 int status;
 static char fname[MAXFLEN];

 strlcpy(fname,"session.keys",MAXFLEN);
 if ((status = nextarg("Session key file name: ", fname, MAXFLEN -1,true)) != TRUE){
		return(status);
 } else {
 	return(execute_session(fname));
 };
}

int show_session(int n)
{
 
 char *ptr;
 int i=0;
 short *key;
 char mstr[128];
 char *description;
 
//MESG("show_session: number of keys = %ld",kbdsession_end-&kbdm_session[0]);
cls_fout("[out]");
sprintf(mstr,"show_session: number of keys = %ld",kbdsession_end-&kbdm_session[0]);
out_print(mstr,1);
for(key=&kbdm_session[0];key<kbdsession_end;key++){
	 	ptr = function_name(key_function(*key,1),&description);
		if(ptr[0]!=0) {
			char *s=&mstr[0];
			snprintf(mstr,80,"%3d key=%d %s = %s   ",i++,*key,xe_key_name(*key),ptr);
			out_print(s,1);
		} else {
			char *s=&mstr[0];
			snprintf(mstr,80,"%3d char %d %X",i++,*key,*key);
//			MESG("key not found %d  %04X",i++,*key);
			out_print(s,1);
		};
 };
 record_session=0;
 return TRUE;
}

int save_session(int n)
{
 int f;
 int len=kbdsession_end-&kbdm_session[0];
 char session_file[MAXFLEN];
 int sstat=0;

 sstat=snprintf(session_file,MAXFLEN,"%s/.xe/%s",getenv("HOME"),"session.keys");
 if(sstat>=MAXLLEN) {
 	msg_line("error saving session");
	return 0;
 };
// MESG("save_session: len=%d to [%s]",len,session_file);
 f=open(session_file,O_RDWR|O_CREAT|O_TRUNC,0644);
 if(f>2) {
 	if(write(f,(void *)&len,sizeof(int)) < sizeof(int)) { 
		close(f);SYS_ERROR("save_session: can not write");return(FALSE);
	};
	for(kbdsptr=&kbdm_session[0];kbdsptr<kbdsession_end; kbdsptr++) {
		short int ikey=*kbdsptr;
//		MESG("save key %d = 0x%X",ikey,ikey);
		if(write(f,(void *)&ikey,sizeof(short int)) < sizeof(short int)) { 
			close(f);SYS_ERROR("save_session: can not write");return(FALSE);
		};
	
 	};
//	msg_line("saved session to session.keys");
	msg_line("saved session len=%d to %s",len,session_file);
	close(f);
 } else {
 	SYS_ERROR("error opening session.keys file!");
	msg_line("cannot create session key file");
 };
 record_session=0;
 return TRUE;
}
#endif

/* set default local codepage for ISO files,characters */
int set_locale(int local_codepage)
{
	switch(local_codepage){
		case 0:
		case 1:
		case 11:
		case 12:
			default_local_codepage=1;
			break;
		default:	
			default_local_codepage=local_codepage;
	};
	return default_local_codepage;
}

#if	RSESSION
// save trace in case of an error
int save_trace(int n)
{
	FILE *fp;
	FILEBUF *bp;
	short *kbdsptr;
	char *ptr;
	char *description;

	bp=cbfp;
	fp=fopen("trace.log","a+");
	if(fp==NULL) { SYS_ERROR("cannot open trace file to write!");return FALSE;};

	// show session keys
	for(kbdsptr=&kbdm_session[0];kbdsptr<kbdsession_end-1; kbdsptr++) {
 		ptr = function_name(key_function(*kbdsptr,1),&description);
		if(ptr[0]!=0) fprintf(fp,"\n	%s = %s ",xe_key_name(*kbdsptr),ptr);
		else fprintf(fp,"%c",*kbdsptr);
 	};
	kbdsptr=&kbdm_session[0];
	fprintf(fp,"%c[31mbuffer%c[0m: [%s / %s]\n",27,27,bp->b_dname,bp->b_fname);
	fprintf(fp,"%c[31mB: save_ppline=%d npoints=%d flag=%d mode=%d lines=%lld\n",27,bp->save_ppline,bp->npoints,bp->b_flag,bp->b_mode,bp->lines);
	fprintf(fp,"%c[3%cmW=%d: ntrows=%d ntcols=%d lcol=%lld topline=%lld currow=%d wline=%d\n",27,49+cwp->id,cwp->id,cwp->w_ntrows,cwp->w_ntcols,cwp->w_lcol,tp_line(cwp->tp_hline),cwp->currow,getwline());
//	fprintf(fp,"ppline0=%d quotec=%d quote1=%d quote2=%d selection=%d\n", cwp->w_ppline,cwp->hs[0].w_hquotec,cwp->hs[0].w_hquote1,cwp->hs[0].w_hquote2,cwp->hs[0].w_hselection);
//	fprintf(fp,"ppline1=%d quotec=%d quote1=%d quote2=%d selection=%d\n", cwp->w_ppline,cwp->hs[1].w_hquotec,cwp->hs[1].w_hquote1,cwp->hs[1].w_hquote2,cwp->hs[1].w_hselection);
	show_points(bp,fp);
	fprintf(fp,"-------------------------------------------------------------\n");
	fclose(fp);
//	show_info(1);
	msg_line("Error ..!!! trace.log created!!!");
	return true;
}
#endif

/* create function  */
int create_function_buffer(FILEBUF *pbuf,char *function_name,offs start_function_offset,offs foffset)
{
 FILEBUF *macrobuf;
 char bname[MAXFLEN];
 char *function_block;
 int function_size=0;
	// MESG("create_function_buffer:");
	bname[0] = CHR_LBRA;
	strlcpy(bname+1,function_name,250);
	insert_bt_element(bt_table,bname+1,TOK_PROC,0);	/* insert in main table  */
	strlcat(bname, "]",MAXFLEN);
	if ((macrobuf = new_filebuf(bname, FSINVS)) == NULL) {
		SYS_ERROR("Can not create function buffer");
		return(0);
	};

	function_size = foffset-start_function_offset-1;
	function_block = (char *)emalloc(function_size+1,"function_block");
	GetBlock(pbuf,function_block,start_function_offset+1,function_size);
	EmptyText(macrobuf);

	// insert the text
	insert_string(macrobuf,function_block,function_size);
	efree(function_block,"function_block");

	// set buffer type to cmd
	macrobuf->b_type=1;
	// MESG("created sub file [%s] type %d",pbuf->b_fname,pbuf->b_type);
	return(1);
}

/* 
 refresh line at current cursor position of current window
 (cbfp is cwp->w_fp)
 function bound to ALT-=
*/
int refresh_current_line(int nused)
{
 double value=0;
 int is_ddot=0;
 char text_line[MAXLLEN];
 offs tpo;	// current offset
 offs sl,el; // start, end of line
 int dsize=0;
 offs i;
 show_stage=0;

	tpo=tp_offset(cwp->tp_current);
	
	sl=FLineBegin(cbfp,tpo);

	el=FLineEnd(cbfp,tpo);
	dsize=el-sl;
	for(i=sl;i<el;i++) {
		if(CharAt(i)==':') { is_ddot=!(cbfp->b_state & FS_VIEW);break;};
	};

	// goto the begining of the line
	set_Offset(sl);
	get_text_offs(cbfp,text_line,sl,MAXLLEN);

	if(is_ddot) DeleteBlock(0,dsize);	/* clear the line!  */

	value = compute_string(text_line,text_line);

	if(is_ddot) {
		insert_string(cbfp,text_line,strlen(text_line));
	};

	if(err_num>0) 
 		msg_line("Error[%d]=%s :: 0x%lX = %12.3f",err_num,err_str,(int)value,value);
 	else {
 		if(vtype_is(VTYPE_STRING)) msg_line(" %15.3f,[%s]",value,get_sval());
		else {
			msg_line(" %15.3f = 0x%lX = o%lo",value,(int)value,(int)value);
		}
 	};
	if(is_ddot){
		el=FLineEnd(cbfp,sl);
		textpoint_set(cwp->tp_current,el);	/* at the end of the line  */
	} else {
		textpoint_set(cwp->tp_current,tpo);	/* where it was !  */
	};

	set_update(cwp,UPD_EDIT);
 	return(TRUE);
}

/* -- */

