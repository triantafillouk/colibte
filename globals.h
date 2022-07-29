/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

	Global variable definitions.
*/

#ifdef	_MAINDEF_ 
/* for MAIN.C */
int app_error=0;
int half_last_line=0;	// this is 1 for gtkterm, 0 for curses, 0 for xlib, denotes that the last line can be incomplete!
int tabsize=4;			// default tabsize
int syntaxh=1;	// highlight flags
int	entry_mode=0;	// we are in keyboard entry mode

int  xwin;

int default_lang=0;		/* utf-8 */
int default_local_codepage = 1;

/* The following table corresponds to e_lang menu  */
char *codepage_str[] = {
	"UTF-8",
	"ISO8859-1","ISO8859-2","ISO8859-3","ISO8859-4","ISO8859-5","ISO8859-6",
	"ISO8859-7",
	"ISO8859-8","ISO8859-9","ISO8859-10",
	"ISO8859-11",	/* thai */
	"ISO8859-13","ISO8859-14","ISO8859-15","ISO8859-16",
	"windows-1253",
	"KOI8-T","CP87","CP737"
};

/* initialized global definitions */
int	gmode = 0;			/* global editor mode		*/
int gmode_exact_case = 0;
int gmode_reg_exp = 0;
int gmode_over = 0;
int	macro_exec	= FALSE;		/* command line execution flag	*/
int	discmd	= TRUE;			/* display command flag		*/
int execmd	= FALSE;	/* exec the file  */
int mcurflag = TRUE;	/* mouse curses flag  */
int	kbdmode = STOP;		/* current keyboard macro mode	*/

int	lastkey = 0;		/* last keystoke		*/

WINDP	*cwp=NULL;				/* Current window pointer       */
GWINDP	*curgwp=NULL;	/* Current window meta structure data 	*/


#else
/* for all the other .C files exept main */
/* initialized global external declarations */
extern int app_error;
extern int half_last_line;
extern int tabsize;
extern int syntaxh;
extern int entry_mode;
extern int xwin;
extern int execmd;
extern int default_lang;
extern int default_local_codepage;
extern char *codepage_str[];
extern	int	gmode;			/* global editor mode		*/
extern	int gmode_exact_case;
extern	int gmode_reg_exp;
extern	int gmode_over;
extern	int	macro_exec;			/* command line execution flag	*/
extern	int	discmd;			/* display command flag		*/
extern	int mcurflag;
extern	int kbdmode;			/* current keyboard macro mode	*/
extern	int lastkey;			/* last keystoke		*/
extern  WINDP  *cwp;                 	/* Current window               */
extern  GWINDP	*curgwp;

#endif


/* -- */
