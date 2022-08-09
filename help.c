/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

*/

/* Help commands  */
 
#include	"xe.h"
extern FILEBUF *cbfp;

/* Show on screen help */
int help(int n)	
{

	FILEBUF *bp;	/* buffer pointer to help */
	char *fname=NULL;		/* ptr to file returned by find_file() */
	fname = find_file("help",APPLICATION_HELP,1);

	if (fname == NULL) {
		msg_line("could not find help file!");
		return(FALSE);
	};
	bp = new_filebuf(fname,FSINVS);
	if(bp==NULL) {
		msg_line("cannot open help!");
		return(FALSE);
	};
	/* split the current window to make room for the help stuff */
	if (split_window(1) == FALSE) return(FALSE);

	select_filebuf(bp);
	file_read(bp,fname);
	
	/* set this window to VIEW mode, update all mode lines */
	cbfp->b_state |= FS_VIEW;
	cbfp->b_flag |= FSINVS;
	hide_cursor("help");
	update_status();
	return(TRUE);
}

void help_arguments(int full)
{
if(full) {
char msg[] = "Usage is\n\
 xe options file_name(s)\n\
\n\
 options\n\
 -h         : show help\n\
 -v         : show version\n\
 -k         : set encryption flag\n\
 -D         : activate debug flag\n\
 -b         : open as binary\n\
 -x <file>  : execute cmd file and quit\n\
 -X <file>  : execute file as startup file\n\
 -n         : no mouse integration in console driver\n\
 -m         : open as memory map file\n\
 -N         : open Notes tag view\n\
 -r         : record session keys\n\
 -s <file>  : open and replay session file\n\
";
 printf("%s\n",msg);
 };
 printf("Editor Version is %s\n",VERSION);
}

/* -- */
