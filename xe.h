/*
	Colibri text editor
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

	xe.h main include file for colibri editor editor
*/
/* Flags and structures of xe editor */

/*	Program Identification..... */
#define	PROGNAME	"Colibri text editor"
#define VERSION 	"#01.58T38 (1/03/2023)"
// merged from kle4 #776T46 (28/7/2022)
#include "config.h"

/* Test flags */
#define NUSE		0	/* not used anymore */
#define	RSESSION	1	/* record session */
#define	CLASSIC_STATUS 	0

#define DOUBLE_ESC	0	/* use double escape for abort command else single  */

#define	NEW_COLORS	1
#define NEW_COLOR8	0

#define	PANGO_TEST	1
#define	TARROWS		1	/* Use arrow menus in panel curses  */
#define	USE_UTF8	1	/* Use utf8 characters  */

#define	USE_FAST	1 & PCURSES	/* erase line for double width characters in panel_curses  */
// #define	USE_GLIB	0

#if	DARWIN
#define	_FILE_OFFSET_BITS	64
#define _DARWIN_USE_64_BIT_INODE	1
#endif

#define	_LARGEFILE64_SOURCE	1

/****************************************************************/
#define	DRIVER_CURSES	0
#define	DRIVER_XLIB		1
#define	DRIVER_GTK2		2
#define DRIVER_GTK3		3

#define DIR_NAME_POSITION	30
#define	TAGS_WIDTH	15
#define	NOTES_COLUMN TAGS_WIDTH+2

#define	DIR_DEFAULT		0
#define	DIR_PRIMARY		1
#define	DIR_SECONDARY	2
#define	DIR_NOTES		3
#define DIR_NOTES_SEC	4
#define	NOTE_TYPE		1024
#define	NOTE_CAL_TYPE	2048
#define	NOTE_TODO_TYPE	4096

#define	C_LINEBEG	1	// at beginning of line
#define C_LINEEND	2	// at end of line
#define	C_COLSTART	3	// at first non space character
#define	C_STARTEND	4	// begining,end of selection
#define	C_PREVLINE	5	// On previous line
#define	C_BALINES	6	// Before, after lines



/* common includes */
#include	<stdio.h>
#include	<stdarg.h>
#include	<string.h>
#include	<fcntl.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<dirent.h>
#include	<time.h>
#include	<stdlib.h>
#include	<math.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <sys/mman.h>
#include <unistd.h>
#include <utime.h>
#include <errno.h>
#include <ctype.h>
#include <sys/time.h>
#include <locale.h>
#include <stdint.h>

	/* from mc  */
#if defined _POSIX_VERSION || (defined HAVE_LIMITS_H && !defined __GNUC__)
# include <limits.h>
#endif

#include <search.h>

#if	!SOLARIS
#include <memory.h>
#endif

#include "support.h"
#include "alist.h"
#include "utf8_support.h"
#include "input.h"

#define	FONT_STYLE_BOLD			0x100
#define	FONT_STYLE_UNDERLINE	0x200
#define	FONT_STYLE_ITALIC		0x400
#define FONT_STYLE_REVERSE		0x800
#define FONT_STYLE_DIM			0x1000

#if	GTK
#include <gtk/gtk.h>
#endif

#if	PCURSES
#include <curses.h>
#include <panel.h>
#else
typedef	int		bool;	/* define our own bool type  */
#endif

#if	XLIB
#include    <X11/Xlib.h>
#include	<X11/Xutil.h>
#include	<X11/Xos.h>
#include	<X11/Xatom.h>
#include	<X11/keysym.h>
#include	<X11/Xresource.h>
#include	<X11/cursorfont.h>
#endif

/* some generic definitions */
typedef unsigned char   byte;
typedef long long offs;
typedef long long num;

/*	Configuration options	*/

#define	CRYPT		1	/* file encryption enabled?			*/
#define	CTAGS		1	/* ctags support */
#define DATE		1	/* Insert Date, Time */



/*	System dependant library redefinitions, structures and includes	*/

/*	internal constants	*/

#define BKMAX	20			/* bookmarks, history marks max */

#define	MAXASSIGNS	2048	
#if	RSESSION
#define	MAXSESSION	5000	/* maximum keyboard session lenght */
#endif

/* Keyboard definitions */
#define CNTRL   0x0100          /* Control key flag    */
#define META    0x0200          /* Meta key flag       */
#define CTLX    0x0400          /* ^X flag             */
#define	SPEC	0x0800			/* special key (function keys)	*/
#define SPEC1	0x1000			/* first alternate keys		*/
#define SPEC2	0x2000			/* second alternate keys	*/
#define	X2UTF	0x4000			/* X11 to utf and insert  */
#define KMOUS	0x8000			/* kmous terminfo key  */

#define KMOUSE_NONE		0
#define KMOUSE_BUTTON1	1
#define KMOUSE_BUTTON2	2
#define KMOUSE_BUTTON3	3
#define KMOUSE_BUTTON4	4
#define KMOUSE_BUTTON5	5
#define KMOUSE_ENTER	6
#define KMOUSE_GOTO		7
#define KMOUSE_DBLCLICK	16
#define KMOUSE_RELEASE	64
#define KMOUSE_EXIT		128
#define KMOUSE_UPDATE	129

/* return status flags from commands */

#ifdef	FALSE
#undef	FALSE
#endif
#ifdef	TRUE
#undef	TRUE
#endif

#define FALSE   	0 
#define	false		0
#define TRUE    	1
#define	true		1

#define	OK_RSTGOAL	2	/* ok and reset goal column */
#define	OK_CLRSL	4	/* ok and clear status line */	
#define	OK_ARG		8	/* ok with numeric value return */

/* keyboard macro flags */

#define	STOP	0			/* keyboard macro not in use	*/
#define	PLAY	1			/*		  playing	*/
#define	RECORD	2			/*		  recording	*/
#define	BPLAY	4			/*	replay from start of session */
#define	BRECORD	8			/* 	session keyboard recording */

#define MACRO_TRUE	1
#define	MACRO_MODE2	2

#define	KNORMAL		0
#define	KENTRY		1
#define	KCONFIRM	2

/*	Input History list type definitions */
#define	LNO  0
#define LDIR 1
#define LVAR 2
#define LBUF 3
#define LCOM 4
#define LSRC 5
#define LMAC 6
#define LSHELL 7
#define LSQL 8
#define	LDIR2	9

#define	FORWARD	0			/* forward direction		*/
#define REVERSE	1			/* backwards direction		*/

/* special characters */

#define	CHR_META	(CNTRL | '[')
#define	CHR_REPT	(CNTRL | 'U')

#define CHR_RESET	0
#define CHR_NULL	0
#define	CHR_ABORT1	0x07		/* abort character	*/
#define	CHR_ABORT	CNTRL | 'G'	/* abort character	*/
#define	CHR_TAB		0x09		/* a tab character	*/
#define	CHR_LINE	0x12		/* Line separation	*/
#define CHR_NEWLINE	0x0A		/* New line  */
#define CHR_CR		13			/* Cariage return   */
#define	CHR_ESC		0x1B		/* espape character */
#define	CHR_SPACE	0x20		/* space character	*/
#define	CHR_DEL		0x7F		/* delete character */
#define	CHR_CTLX	'X'-'@'		/* ^x character (24) */
#define CHR_LBRA	'['			/* left bracket  */
#define	CHR_RBRA	']'			/* right bracket  */
#define	CHR_DQUOTE	'"'			/* double quote  */
#define	CHR_SQUOTE	'\''		/* single quote  */
#define CHR_BQUOTE	'`'			/* back single quote  */
#define	CHR_BSLASH	'\\'		/* back slash  */
#define CHR_PARL		'('
#define CHR_PARR		')'
#define CHR_CURLL		'{'
#define CHR_CURLR		'}'
#define CHR_SMALLER		'<'
#define CHR_BIGER    	'>'
#define CHR_SLASH		'/'
//#define CHR_NBSPACE		0xC2A0
#define CHR_NBSPACE		' '

#define	PATHCHR	':'
#define DIRSEPSTR "/"


#define NUM_COLS_RCFINFO	24	/* row col full info  */
#define NUM_COLS_FINFO		10	/* flags info  */
#define NUM_COLS_RCSINFO	8	/* line info  */

#define	NUM_COLS_FULL		37	/* minimum cols for full info  */
#define NUM_COLS_SHORT		30	/* minimum cols for short info  */
#define NUM_COLS_MINIMUM	20	/* minimum cols, only line info  */
#define NUM_COLS_NOINFO		15	/* minimum cols no info  */

#if	TNOTES
typedef struct notes_struct {
	char n_date[64];
	char n_name[512];
	char n_title[512];
	char n_cat[512];
	char n_tags[512];
	int encrypt_flag;
} notes_struct;
#endif

/*	Virtual screen structures  */

typedef struct vchar {
	unsigned char uval[8];	/* stores a utf char  */
	short int  attr;
	short int bcolor;	/* background  */
	int fcolor;	/* foreground  */
#if	0
	unsigned int display_width;
	unsigned int display_height;
#endif
} vchar;

typedef struct  VIDEO {
	short int	v_flag;		/* line Flags */
#if	NUSE
	short int	utf_error;	/* not valid utf8 char!  */
#endif
#if	PANGO_TEST0
	int left_x_pos;
	int start_column;
	int continuation;
#endif
	vchar v_text[1];
}   VIDEO;


// HEX Mode start column
#define	HSTART			59
#define HEX_LINE_LEN	16

typedef struct tok_data {
	int	ind;
	int vtype;
	double *pdval;
	union {
		double dval;
		char *sval;
		struct array_dat *adat;
	};
} tok_data;

#if	GTK
/* keep gtk part of windows */
typedef struct GWINDP {
 GtkWidget *box;
 GtkBox *hbox;
 void *draw;	// geditdisplay widget
 GtkVScrollbar *scroll_bar;	// scrollbar
 GtkAdjustment *draw_adj;
 int ptype; 	/* parent type 1->vertical, 2->horizontal, 0->no split allowed */
 struct BTWE *bte;
 GtkWidget *evb_hstatus;	/* event box for hstatus in GTK2  */
 GtkWidget *hstatus;	/* Bottom horizontal status box  */
 GtkWidget *status1;	/* Bottom horizontal status showing name  */
 GtkWidget *status2;	/* Bottom horizontal status mode  */
 GtkWidget *status3;	/* Bottom horizontal cursor position status   */
 guint width;
 guint height;
} GWINDP;
#endif
#if	PCURSES
typedef struct	GWINDP {
	int t_xpos;		// x position of window
	int	t_ypos;		// y position of window
	/* curses window  */
	WINDOW *draw;
	PANEL *panel;
	WINDOW *vline;
	PANEL *vline_panel;
	int back_xpos;
	int back_ypos;
	int back_rows;
	int back_cols;
	int h_flags;	/* horizon update flags  */
	int currow;
	int curcol;
} GWINDP;
#endif
#if	XLIB
typedef struct	GWINDP {
	/* X11 window  */
	Drawable draw;
	int d_xpos,d_ypos;
	int d_width,d_height;
	Drawable sbar;	/* geometry from draw window  */
	GC d_gc;
	GC top_gc,botom_gc;
	GC sgc,sgm;
	Drawable status;
	int back_xpos;
	int back_ypos;
	int back_height;
	int back_width;
	int h_flags;	/* horizon update flags  */
} GWINDP;
#endif

#if	GTK
/* btree element of gwindp */
typedef struct BTWE {
	struct BTWE *parent;
	struct BTWE *left;
	struct BTWE *right;
	GWINDP *gw;
	int id;		/* needed for debugging */
	int	type; /* 0=root left=1, right=2 */
	GtkBox *pbox;
	GtkVBox *pan;
} BTWE;
#else
typedef struct BTWE {
	struct BTWE *parent;
	struct BTWE *left;
	struct BTWE *right;
	GWINDP *gw;
	int id;		/* needed for debugging */
	int	type; /* 0=root left=1, right=2 */
} BTWE;
#endif

typedef struct ISTR {
	int index;
	int selected;
	char start;
} istr;

typedef struct DRV_FLAGS {
	int console;		/* if true then this is console, do not mix with output! (!xwin) */
	int	file_dialog;	/* a file dialog exists! (like in GTK2,3)  */
	int	half_last_line;
	int mcurflag;		/* used only in panel curses driver  */
	int macro_exec;		/* include in console flag (0)  */
} DRV_FLAGS;

typedef struct MLQUOTES {
	unsigned int w_hquotem;
	unsigned char w_hselection;
	unsigned char w_slang;
	unsigned char w_jflag;	/* for json parse  */
	unsigned char w_notes;
	num known_offset;
} MLQUOTES;

/* highlight quote masks */
#define H_QUOTE1	1	/* single quotes, singleline */
#define H_QUOTE2	2	/* double quotes, usually multiline */
#define	H_COMMENT	4	/* comments usually multiline */
#define H_QUOTEC	4
#define H_QUOTE4	8	/* %, ! at start of line or other  */
#define H_QUOTE5	16	/* comments rest of the line (c++ // , ;  )  */
#define H_QUOTE6	32	/* preprocessing ( # at start of line )  */
#define H_QUOTE7	64	/* inside <> for html  */
#define	H_QUOTE8	128	/* first word inside < > for html  */
#define	H_QUOTE9	256	/* word in preprocessor  */
#define	H_QUOTE10	512	/* light background, rest of the line  */
#define	H_QUOTE11	1024/* toggle light background */
#define H_QUOTE12	2048
#define H_LINESEP	4096
#define H_UTFERR	8192	/* Utf8 error  */

#define H_WORD1		10	/* found word type1  */
#define	H_WORD2		11	/* found word type2  */
#define	H_NUMERIC	12	/* found numeric  */

#define H_NAME     1
#define H_FIRST    1	/* First in json  */
#define H_VALUE    2
#define H_ARRAY    2	/* value array in json  */

#define H_SELECT	128

#define	VTYPE_NUM		1
#define	VTYPE_ARRAY		2	/* numeric array  */
#define	VTYPE_SARRAY	3	/* string array  */
#define	VTYPE_LIST		4	/* numeric list  */
#define	VTYPE_SLIST		5	/* string list  */
#define VTYPE_ALIST		6	/* array list  */
#define	VTYPE_ASLIST	7	/* array string list  */
#define	VTYPE_STRING	8	/* string  */
#define	VTYPE_BUFFER	9	/* file buffer  */
#define	VTYPE_AMIXED	12	/* mixed array  */
#define VTYPE_DYNAMIC	16	/* dynamic array  */
#define	VTYPE_TREE		32	/* btree pairs  */

/* textpoint */
#define  FULLDEFINED	0
#define  COLUNDEFINED	1
#define  LINEUNDEFINED	2
#define	LINECOLUNDEFINED 3
#define  OFFSUNDEFINED  4


typedef enum {
	TP_CURRENT,
	TP_SAVE_CURRENT,
	TP_BASE,
	TP_TEXT_END,
	TP_HLINE,
	TP_PREV_HLINE,
	TP_SAVE_HLINE,
	TP_W_SMARK,
	TP_W_EMARK,
	TP_B_SMARK,
	TP_B_EMARK,
	TP_HSKNOWN,
	TP_HMKNOWN,
	TP_SAVE_HSKNOWN,
	TP_SAVE_HMKNOWN,
	TP_BP_DELETE_BLOCK,
	TP_DELETE_BLOCK,
	TP_LINE_START,
	TP_LINE_END,
	TP_COMMENT_START,
	TP_COMMENT_END,
	TP_DDOT,
	TP_TEMP,
} tp_name_type;

typedef struct TextPoint
{
	offs  offset;
	num   line,col;
	int   flags;
	int num;
	int tp_type;
	struct TextPoint   *t_next;
	struct FILEBUF *fp;
	int window;
} TextPoint;

typedef struct ClipBoard {
   char *text;
   num width;
   num height;
   int rect;
} ClipBoard;

typedef struct  WINDP {
	struct	FILEBUF *w_fp;		/* window file buffer */
	struct	TextPoint *tp_current;	/* current cursor position */
	struct	TextPoint *prev_hline;		// previous hline for faster scrolling!
	struct	TextPoint *tp_hline;	// point (line) at top of window -> topleft
	struct	TextPoint *w_smark;
	struct	TextPoint *w_emark;
	/* highlight admin fields */
	struct	TextPoint *tp_hsknown;	/* point at which quote state is known */
	struct	TextPoint *tp_hmknown;	/* point at which mid point where quote state is known */
	struct MLQUOTES hs[2];	/* highlight state */

	char	selection;
	
    short   w_ntrows;               /* # of rows of text in window  */
	short	w_ntcols;				/* columns of this window */
	num		w_lcol;					/* offset of text in the window */
	num		w_plcol;				/* previous w_lcol */
	int		w_infocol;				/* info column size  */
	struct GWINDP *gwp;				/* internal window structure (gtk or other) */
	int 	id;

	int		w_ppline;				/* previous window physical line */
	int		w_flag;                 /* Flags.                       */
	VIDEO	**vs;				/* a screen for each window */
	int		draw_flag;
	char	w_fcolor;		/* current foreground color	*/
	char	w_bcolor;		/* current background color	*/
	/* the value on/off of the quotes are at the end of the window */
	/* it should better be a pointer to a quote structure specific */
	/* for each file type */
	int	vtcol;
	int	vtrow;
	int currow;
	num curcol;
	int top_note_line;
	int current_note_line;
	int top_tag_line;
	int current_tag_line;
	int w_prev_line;
	num goal_column;
}       WINDP;	

typedef struct SHLIGHT {
	char *description;
	int ignorecase;
	int notab;
	char **w0;	/* words type 1  */
	char **w1;	/* words type 2  */
	void (*h_function)(int);	/* highlight function  */
	void (*h_update)(WINDP *wp);
	int (*c_inword)(int c);	/* word definition  */
	char **file_extentions;
	int (*comment)(int c);		/* comment function  */
} SHLIGHT;

/* update flags */
#define	UPD_MOVE		0x01		/* Just position moving */
#define	UPD_LINE		0x02		/* update line  */
#define	UPD_WINDOW		0x04		/* update window size,position */
#define	UPD_STATUS		0x10		/* update window status line */
#define	UPD_FULL		(UPD_WINDOW+UPD_STATUS)	/* full window update */
#define	UPD_EDIT		0x40		/* update buffer contents */
#define	UPD_ALL			0x80		/* full update all windows */
#define	UPD_FORCE		0x100		/* update force  */

/*
 * Text is kept in file buffers. 
 * Buffers may be "Inactive" which means the files associated with them
 * have not been read in yet. 
 */
typedef struct  FILEBUF {
	struct	TextPoint *tp_base;
	struct	TextPoint *tp_last;
	struct	TextPoint *tp_text_end;
	struct	TextPoint *tp_text_o;	/* cache of end of file pointer, makes a lot of staff faster!!  */
	
	struct	TextPoint	*save_hsknown;	// last highlight state known at this point
	struct	TextPoint	*save_hmknown;	// last mid highlight state known at this point
	struct	TextPoint	*save_current;	// current point
	struct	TextPoint	*save_hline;	// top window line
	struct	TextPoint	*tp_current;	// this is the operational current pointer!!
	struct  TextPoint	*tmp_current;	/* temporary current point  */

#if	TNOTES
	int	save_top_tag;
	int save_top_note;
	int save_current_tag;
	int save_current_note;
#endif

	struct	TextPoint	*smark;	/* used in macro operations  */
	struct	TextPoint	*emark;	/* used in macro operations  */

	struct MLQUOTES save_hs[2];
	
	int		save_ppline;
	int		npoints;
	int		b_index;	/* buffer file index, a unique number for the each buffer */
	offs	ptr1,ptr2;
	offs	oldptr1,oldptr2;
	char	*buffer;		/* real data! */
	offs	BufferSize;
	offs	GapSize;
	int		EolSize;
	char	EolStr[3];
	mode_t	FileMode;
	time_t	FileTime;
	struct	UNDOS	*main_undo;
// For dir mode
	struct	dir_l	*rdir;
	struct	dir_l	*cdir;
	struct	alist	*dir_list_str;
	int		sort_mode;
	int		flen0;					/* Initial line len  */
	int		file_id;				/* file id while reading  */
	int		b_nwnd;                 /* Count of windows on buffer   */
// Flags
	int 	bom_type;				/* file bom type or 0 */
	int		b_state;
 	int		b_flag;                 /* buffer state Flags           */
	char	b_mode;					/* editor mode of this buffer	*/
	char	view_mode;				/* view mode */
	struct  FILEBUF *connect_buffer;
	num		connect_line;
	num		connect_column;
	num		connect_top_line;
	num		connect_top_note;
    char    b_fname[MAXFLEN];        /* File name                    */
#if	CRYPT
	char	b_key[MAXSLEN];		/* current encrypted key	*/
#endif
	char	b_dname[MAXFLEN];	/* directory name */
	int		b_type;			/* buffer,file type */

	int		scratch_num;	/* scratch number or 0  */
	int		dir_num;		/* dir number  */
	struct SHLIGHT *hl;		/* highlight type structure */
	/* Number of lines must be kept to be able to use correctly the scrollbar */
	num lines;			/* number of lines in buffer */
	num bytes_read;
	num line_from;	/* last change from line  */
	num line_to;	/* last change to line  */
	int b_lang;		/* document language, 0 is UTF  */
	int m_mode;		/* parse status flag  */
	int utf_accent;	/* current utf char is with an accent!  */
// Macro language sructures
	struct tok_struct *tok_table;
	struct tok_struct *end_token;	/* the last (EOF) token in tok_table  */
	int err;	/* negative if not syntax checked  */
	BTREE *symbol_tree;	/* local symbol table  */
	tok_data *symbol_table;	/* instance of variables data  */
	int slow_display;
//	Notes structures
#if	TNOTES
	notes_struct *b_note;
	num b_tags;	/* number of tags  */
#endif
	num b_notes;	/* number of notes  */
	char *b_header;	/* buffer header or NULL  */
#if	TNOTES
	struct	alist	*b_tag_list;
#endif
	struct	alist *dir_list_stack;
}  FILEBUF;

#if	TNOTES
#include "notes.h"
#endif

/* File state flags (b_state)  */
#define	FS_ACTIVE	0x0001	/* file is in memory, active */
#define	FS_CHG		0x0002	/* file changed */
#define	FS_VIEW		0x0004	/* read-only file		*/
#define FS_VIEWA	0x0005	/* read-only file, active  */

/* File buffer flags (b_flag) */
#define FSINVS		0x0008	/* file invisible,internal */
#define FSDIRED 	0x0010	/* directory mode 	*/
#define	FSMMAP		0x0020	/* memory mapped file */
#define	FSMMAPA		0x0021	/* memory mapped file active  */
#define FSFILTER	0x0040	/* Filtered file  */
#define FSNOTES		0x0080	/* Notes view  */
#define	FSNOTESN	0x0100	/* Tag Notes view note selected  */
#define	FSNLIST		0x0200	/* Notes list  */
#define	FSNTODO		0x0201	/* Todo list  */
#define FSNCALENDAR 0x0202	/* Calendar list  */

#define	CATEGORY_COLUMN	50
#define TITLE_COLUMN	80

#define	MODECODE  "judmc--"	/* letters to represent modes	*/

/* edit mode flags (b_mode) */
#define	EMJUST		0x001	/* right justify document mode  */
#define	EMUNIX		0x002	/* Unix mode */
#define	EMDOS		0x004	/* Dos or Unix mode view (0x20=Unix, 0x40=DOS 0x80=MAC) */
#define EMMAC		0x008	/* Mac line types  */
#define	EMCRYPT		0x010	/* crypt mode active */

//#define EMFILTER	0x100	/* filtered buffer  */

/* view mode flags */
#define	VMHEX		0x001	/* view in hex mode */
#define VMINP		0x002	/* hex input mode  */
#define VMLINES		0x004	/* show number lines  */
#define VMOFFSET	0x008	/* show offset  */
#define	VMINFO		0x010	/* show info column, experimental */

#define VMCOLS		8
#define VMICOLS		1

#define	CHR_ESC		0x1B		/* espape character */
#define	CHR_SPACE	0x20		/* space character	*/
#define	CHR_DEL		0x7F		/* delete character */
#define	CHR_CTLX	'X'-'@'		/* ^x character (24) */
#define CHR_LBRA	'['			/* left bracket  */
#define	CHR_RBRA	']'			/* right bracket  */

/* parse m_mode flag  */
#define	M_PARSED	0x0001	/* is parsed  */
#define	M_CHECKED	0x0002	/* is checked  */


/*	structure for the function names */
typedef struct {
	char *n_name;		/* macro name */
	short arg;
	int (*n_func)();	/* function  */
	char *n_help;		/* function description */
} FUNCS;

/* assigned keys */
typedef struct {
	char *a_function_name;
	char *a_key;
	int c;
} AKEYS;

/* user variable structure */
typedef struct VAR {
	char *name;
	double dval;
	char *sval;
} VAR;

typedef struct BMARK {
	char fname[MAXFLEN];
	char dname[MAXFLEN];
	long line;
	long column;
} BMARK;

/* undo data structures */
#include "undo.h"

#define HORIZONTAL	0
#define VERTICAL	1
#define MFUNCTION	0
#define MMENU		1

typedef struct dir_l {
	char *dir_name;
	int cline;
 } dir_l;

struct kdirent {
	// struct stat t;
	long int mtime;
	long int ctime;
	long int atime;
	off_t	st_size;
	mode_t st_mode;
	char d_name[1];
};

#define HEX	16
#define DEC	10
#define OCT	8
#define BIN	2

/* region types */
#define REGION_NONE		0
#define REGION_CHAR		1
#define REGION_LINE		2
#define	REGION_COLM		3

/* bom types */
#define	FTYPE_ENCRYPTED	1
#define	FTYPE_UTF8BOM	2
#define	FTYPE_UTF16BOM	3
#define	FTYPE_UTF32BOM	4
#define FTYPE_UTF16BOM_BE	19	/* big endian  */
#define	FTYPE_UTF32BOM_BE	20	/* big endian  */

 /* environment variables */
enum env_defs {
	EMFILLCOL	,	/* fill column  */
	EMLMARGIN	,	/* left margin  */
	EMTABSIZE	,	/* tab size  */
	EMPRECISION ,	/* double precision  */
	EMCOLORSCHEME,	/* Color scheme index  */
	EMXCOLORSCHEME,	/* X11 color scheme  */
	EMSHOWHEX	,	/* show hex values  */
	EMKEYEMUL	,	/* keyboard emulation  */
	EMBACKUP    ,	/* make file backup on saving  */
	EMBEDICONS	,	/* Embed icons or not  */
	EMSHOWPOS	,	/* show position on right vertical line */
	EMVINFOCOL  ,	/* show vertical info column  */
	EMVLINES,		/* view lines  */
	EMCOFFSET	,	/* show cursor offset  */
	EMCDATA		,	/* show cursor data  */
	EMSAVHIST	,	/* save history  */
	EMLARGEICONS,	/* Use large toolbar icons  */
	EMSAFEOPS,		/* Use safe operations  */
	EMUSETITLEBAR,	/* Use titlebar  */
	EMCCASE,		/* exact match  */
	EMCOVER,		/* overwite  */
	EMCREGEXP		/* Use regular expression in search  */
};

 /* file extensions */
#define	FX_COMPRESS	37

/* Icon Element types  */
#define	ENORMAL	0
#define	ESTOCK	2

#include "declares.h"

#include "globals.h"

#include "bt_func.h"

#include "system.h"

/* -- */
