/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

*/

/* Main body of editor  */

/* _MAINDEF_ is defined only in main.c
   That's where global variables are actually defined and initialized */
#define	_MAINDEF_
 
#include    "xe.h"	/* global structures and definitions */
#include	<sys/stat.h>
#include	<sys/types.h>
#include	<syslog.h>
#include	"mlang.h"

char *startfile=NULL;	/* startfile to execute */
#if	RSESSION
char *sessionfile=NULL;
int	record_session=0;
#endif
int startup_exe = 1;	/* we are executing startup scripts! */
FILEBUF *firstbp = NULL;		/* ptr to first buffer in cmd line */
extern FILEBUF *cbfp;

alist *file_list;
alist *window_list;
alist *var_list;
alist *shell_list;
alist *pat_list;
alist *macro_list;
alist *dir_list;
alist *dir2_list;
alist *local_key_list;
alist *color_scheme_list;

void help_arguments(int f);
void set_screen_update(int flag);

struct array_dat *main_args=NULL;
int dofile(char *fname);

int new_in_key_list=0;

void init_lists()
{
 MESG("init_lists:");
 file_list=new_list(0,"file_list");
 window_list=new_list(0,"window_list");
 var_list=new_list(0,"var_list");
 shell_list=new_list(0,"shell_list");
 pat_list=new_list(0,"pat_list");
 macro_list=new_list(0,"macro_list");
 dir_list=new_list(0,"dir_list");
 dir2_list=new_list(0,"dir2_list");
 local_key_list=new_list(0,"local_key_list");
 color_scheme_list=new_list(0,"color_schemes");
 init_default_schemes();
}

int start_err_num=0;
char *start_err_str=NULL;
int scratch_files[100];

extern int err_num;
extern char *err_str;
extern int max_colors;
int drv_max_colors=16;
extern void main_loop();
int driver_type;
char *lc_lang;
int mmnote=0;
int mmtodo=0;
int mmcal=0;

FILEBUF *get_first_scratch_buffer()
{
 FILEBUF *bp=NULL;
	// get first scratch buffer
	lbegin(file_list);
	while((bp = (FILEBUF *)lget(file_list))!=NULL){
//		MESG("select buffer: [%s]",bp->b_fname);
		if(strncmp("[new",bp->b_fname,4)==0) break;
	};
	return bp;
}

int main(int argc, char **argv)
{
	lc_lang=getenv("LC_ALL");
	if(lc_lang==NULL) lc_lang=getenv("LANG");
	else if(lc_lang[0]==0) lc_lang=getenv("LANG");
	errno=0;

	init_hash();
	discmd = FALSE;
	init_lists();
	scratch_files[0]=0;
	parse_command_line(argc,argv);
	// MESG("main:start1");
	set_start_dir(NULL);
	if(!execmd) {
		// MESG("init_system_clipboard");
		init_system_clipboard();
		// MESG("open_log");
		open_log(argv[0]);	/* connect to syslog  */
		// MESG("set_start_time");
		set_start_time();
		// MESG("init_drv_env");
		driver_type=init_drv_env();	// driver depending variable initialization
		// MESG("load_config");
		load_config();
		// MESG("load_keys");
		load_keys();
		// MESG("read_file_history");
		read_file_history(1);
		/* initialize the editor */
		// MESG("init_names");
		init_names();
		// MESG("init_exensions");
		init_extensions();	// file extensions
		if(startfile==NULL) {
			startfile=find_file("",APPLICATION_RC,1,0);
		};
	};
	// MESG("execute statup file");
	/* execute startup file here */
	if(startfile!=NULL) {
		if(firstbp!=NULL){
			/* do not execute start file in case we want to edit it!  */
			if(strcmp(startfile, get_buf_full_name(firstbp)) != 0 
			&& strcmp(startfile, firstbp->b_fname) != 0 
			) 	
			{
				dofile(startfile);
			};
		} else {
			dofile(startfile);
		};
		// MESG("show errors");
		if(err_num) {	/* show any errors in start file!!  */
			start_err_num=err_num;
			start_err_str=err_str;
		};
		if(execmd) exit(0);
	} else {
		start_err_num=12;
		start_err_str="Could not find a start file!";
	};
	// MESG("set_screen_update");
	set_screen_update(true);
	// MESG("startfile status: %d [%s]",start_err_num,start_err_str);
	scratch_files[0] = load_scratch_files();

#if	TNOTES
	if(mmnote) {
		edinit("[Tag view]");
		activate_file(cbfp);
// 		view_note(1);
		// activate_file(cbfp);
		set_update(cwp,UPD_EDIT);
		MESG("selected tag view cfbp=%s",cbfp->b_fname);
	} else 
	if(mmtodo) {
		edinit("[todo view]");
		activate_file(cbfp);
		set_update(cwp,UPD_EDIT);
// 		view_note(1);
		// set_update(cwp,UPD_EDIT);
		MESG("selected todo view %f",cbfp->b_fname);		
	} else 
	if(mmcal) {
		edinit("[calendar view]");
		activate_file(cbfp);
		set_update(cwp,UPD_EDIT);
	} else
#endif
	{
	if(firstbp==NULL) {
		// MESG("no start file! scratch=%d",scratch_files[0]);
		if(scratch_files[0]==0) {
		// no files, or no scratch files, create new scratch file!
			edinit("[new 1]");
			scratch_files[0]=1;
			activate_file(cbfp);
		// MESG("start with new scratch file!");
		};
		// MESG("Scratch files are %d",scratch_files[0]);
		cbfp = get_first_scratch_buffer();
	} else {
		cbfp = firstbp;
	};
	}
	// MESG("call vtinit:");
	vtinit(argc,argv);		/* Display */
#if	TNOTES
	if(mmnote){
		show_tag_view(1);
		select_filebuf(cbfp);
		MESG("selected tag view 2 cfbp=%s",cbfp->b_fname);
	} else
	if(mmtodo) {
		show_todo_list(0);
		select_filebuf(cbfp);
// 		set_update(cwp,UPD_EDIT);
		// activate_file(cbfp);
		// view_note(1);
		// activate_file(cbfp);
		set_update(cwp,UPD_EDIT);
		MESG("todo view 2");
	} else
	if(mmcal) {
		show_calendar_list(0);
		select_filebuf(cbfp);	
		set_update(cwp,UPD_EDIT);
		MESG("calendar view 2");
	} else
#endif
	{
		select_filebuf(cbfp);
		cbfp->b_mode |= gmode;
	};
	startup_exe=0;
	discmd = TRUE;
	// MESG("main:");
	events_flush();
#if	RSESSION
	if(sessionfile!=NULL) {
		execute_session(sessionfile);
	};
#endif
	set_hmark(1,"start");

	/* setup to process commands */
	
	init_highlight();
	MESG("main: start main_loop:");
	main_loop();
	return(0);
}

void parse_command_line(int argc, char **argv)
{
	FILEBUF *bp;		/* temp buffer pointer */
	int	carg;		/* current arg to scan */
    char bname[MAXFLEN];		/* base name of file to read */
	int binflag=0;		/* binary mode  */
	int mmapflag = FALSE;
#if	CRYPT
	int cryptflag;			/* encrypting on the way in? */
	char ekey[MAXSLEN];		/* startup encryption key */
#endif
	int a_arg=0;
#if	CRYPT
	cryptflag = FALSE;	/* no encryption by default */
#endif
	for (carg = 1; carg < argc; ++carg) {

		/* Process Switches */
		if (argv[carg][0] == '-') {
			switch (argv[carg][1]) {

#if	CRYPT
				case 'k':	/* -k<key> for code key */
				case 'K':
					cryptflag = TRUE;
					strlcpy(ekey, &argv[carg][2],MAXSLEN);
					break;
#endif
				case 'h':
					help_arguments(1);
					exit(0);
				case 'v':
					help_arguments(0);
					exit(0);
#if	1
				case 'C':
					drv_max_colors=256;
					break;
				case '8':
					drv_max_colors=8;
					break;
#endif
				case 'D':
					set_debug(1);
					break;
				case 'b':	/* open as binary file  */
					binflag=1;
					break;
				case 'x':	/* execute file and quit */
					execmd=1;
				case 'X':	/* execute file as statrup */
					carg++;
					startfile=argv[carg];
					if(execmd) 
					{	/* initialize array  */
						// MESG("initialize array starting at %d",carg);
						main_args = new_list_array(argc-carg-1);
						allocate_array(main_args);
						a_arg=0;
						// MESG("show main_args");
						// MESG("	cols=%d rows=%d",main_args->cols,main_args->rows);
					};
					break;
// mcurflag is used by ncurses, do not use mouse integration
				case 'n':
					mcurflag = FALSE;
					break;
				case 'm':
					mmapflag = TRUE;
					break;
#if	TNOTES
				case 'N':
					mmnote = TRUE;
					break;
				case 'T':
					mmtodo = TRUE;
					break;
				case 'L':
					mmcal = TRUE;
					break;
#endif
				case 'p':
					set_btval("reset_position",-1,NULL,1);
					break;
#if	RSESSION
				case 'r':
					record_session = TRUE;
//					MESG("Enable record session");
					break;
				case 's':
					carg++;
					record_session = FALSE;
					sessionfile=argv[carg];
//					MESG("set sessionfile");
					break;
#endif
				default:	/* unknown switch */
					/* ignore this for now */
					break;
			}
		}  else {
			if(execmd) {	/* popylate the array  */
				// MESG("- push %d -> %d of %d: %s",carg,a_arg,argc,argv[carg]);
				main_args->sval[a_arg] = argv[carg];
				a_arg++;
				// carg++;
				continue;
			};
			int bflag=0;
			/* Process an input file */
			// MESG("- process %d: %s",carg,argv[carg]);
			set_bfname(bname,argv[carg]);
//			MESG("main: new input file: [%s]",bname);
			if(mmapflag) bflag |= FSMMAP;
			bp=new_filebuf(bname,bflag);
			if(bp) {
			if (firstbp==NULL) {
				firstbp = bp;
			} } else {
				start_err_num=11;
				start_err_str="cannot create file ";
			};
			/* set the modes appropriatly */
			if(binflag) {
				bp->b_mode = VMHEX;
				cbfp=bp;
				set_view_mode(3);
				return;				
			}
#if	CRYPT
			if (cryptflag) {
				bp->b_mode |= EMCRYPT;
				crypt_string((char *)NULL, 0);
				crypt_string(ekey, strlen(ekey));
				strlcpy(bp->b_key, ekey, MAXSLEN);
			}
#endif
		}
	};
}
