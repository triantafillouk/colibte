/*
	curses,gtk editor,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.
*/

/* curses, terminfo color scheme definitions */

#define COLOR_SCHEMES	10

/*	Ansi terminal color definitions */

#define	BLACK		0
#define RED			1
#define	GREEN		2
#define ORANGE		3
#define BLUE		4
#define MAGENTA		5
#define CYAN		6
#define WHITE		7

#define BROWN		8
#define LRED		9
#define LGREEN		10
#define YELLOW		11
#define	LBLUE		12
#define LMAGENTA	13
#define LCYAN		14
#define LWHITE		15

#define	COLOR_TYPES	XCOLOR_TYPES

int pair_num[XCOLOR_TYPES][XCOLOR_TYPES];

typedef struct color_curses {
	int index;	/* color index  */
	int attrib;	/* attibute  */
} color_curses;

typedef struct RGB_COLORS {
	int r,g,b;
} RGB_COLORS;

typedef struct colors16 {
	RGB_COLORS rv[16];
} colors16;

typedef struct COLOR_SCHEME {
	char *scheme_name;
	RGB_COLORS basic_colors[16];
	color_curses color_attr[32];
} COLOR_SCHEME;

char *basic_color_values[COLOR_SCHEMES][16] = {
 // Snow
 {
 	/* black */		"#000000",	// FOREGROUND, MODEFOREI
	/* red  */		"#AE0000",	// WORD1FORE, SEARBACK
	/* green */		"#A0F0A0",	// QUOTEBACK
	/* orange */ 	"#906000",	// COMMENTFORE
	/* blue */		"#0000AE",	// CNUMERIC, MENU_BG, CBOXTBACK,
	/* magenta */	"#E0D0A0",  // LBACKGROUND, INFOBACK
	/* cyan */		"#0097B7",	// MODEBACK/selections
	/* white */		"#E3E3E3",	// BACKGROUND, SEARCHFORE, CBOXTFORE, MENUFG
	/* brown */		"#606060",	// QUOTEFORE, MODEBACKI, INFOFORE, TAGFORE
	/* lred */		"#FF0000",	// CHANGEDFORE, CTRLFORE
	/* lgreen */	"#006000",	// SQUOTEFORE, W_FORE
	/* yellow */	"#FFFF00",	// DROWCOL
	/* lblue */		"#0000FF",	// SPECFORE
	/* lmagenta */	"#800080",	// WORD2FORE, ORIZON
	/* lcyan */		"#00A080",	// PREPFORE
	/* lwhite */	"#FFFFFF"	// MODEFORE
 },
// Midnight
 {
 	/* black */		"#000000",
	/* red  */		"#A00000",
	/* green */		"#008000",
	/* orange */ 	"#F09000",
	/* blue */		"#0070A0",
	/* magenta */	"#303020",
#if	1
	/* cyan */		"#006080",
#else
	/* cyan */		"#009090",
#endif
	/* white */		"#D0D0D0",
	/* brown */		"#406060",
	/* lred */		"#FF0000",
	/* lgreen */	"#98E098",
	/* yellow */	"#FFFF00",
	/* lblue */		"#0000FF",
	/* lmagenta */	"#FF00FF",
	/* lcyan */		"steelblue1",
	/* lwhite */	"#FFF0F0"
 },
// Cyan
 {
 	/* black */		"#000000",
	/* red  */		"#D00000",
	/* green */		"#008000",
	/* orange */ 	"#F09000",
	/* blue */		"#005080",
	/* magenta */	"#404030",
	/* cyan */		"#004050",
	/* white */		"#D0D0D0",
	/* brown */		"#406070",
	/* lred */		"#FF0000",
	/* lgreen */	"#00FF00",
	/* yellow */	"#FFFF00",
	/* lblue */		"#0000FF",
	/* lmagenta */	"#FF00FF",
	/* lcyan */		"#00FFFF",
	/* lwhite */	"#FFFFFF"
 },
// Blue Sky
 {
 	/* black */		"#000000",
	/* red  */		"#D00000",
	/* green */		"#008000",
	/* orange */ 	"#F09000",
	/* blue */		"#002060",
	/* magenta */	"#404030",
	/* cyan */		"#008080",
	/* white */		"#D0D0D0",
	/* brown */		"#004040",
	/* lred */		"#FF0000",
	/* lgreen */	"#00FF00",
	/* yellow */	"#FFFF00",
	/* lblue */		"#0000FF",
	/* lmagenta */	"#FF00FF",
	/* lcyan */		"#00FFFF",
	/* lwhite */	"#FFFFFF"
 },
// Gray
 {
 	/* black */		"#000000",
	/* red  */		"red4",
	/* green */		"#008000",
	/* orange */ 	"#E0D0A0",
	/* blue */		"#004090",
	/* magenta */	"#A8A8A0",
	/* cyan */		"#36648B",
	/* white */		"grey55",
	/* brown */		"#404040",
	/* lred */		"#FF4D4D",
	/* lgreen */	"#0E500E",
	/* yellow */	"#FFFF00",
	/* lblue */		"#0000CC",
	/* lmagenta */	"#8C008C",
	/* lcyan */		"#00FFFF",
	/* lwhite */	"#F2F2F2"
 },
// Dark Gray
 {
 	/* black */		"#303030",
	/* red  */		"#FA0000",
	/* green */		"#007A00",
	/* orange */ 	"orange2",
	/* blue */		"#3333A0",
	/* magenta */	"#5F5F5F",
	/* cyan */		"#336194",
	/* white */		"#F2F2D9",
	/* brown */		"#434343",
	/* lred */		"#FF9999",
	/* lgreen */	"#66FF66",
	/* yellow */	"#FFFF00",
	/* lblue */		"#9999FF",
	/* lmagenta */	"#FF00FF",
	/* lcyan */		"#99E6E6",
	/* lwhite */	"#FFFFFF"
 },
// Sunny
 {
 	/* black */		"#000000",
	/* red  */		"#AD0000",
	/* green */		"#009400",
	/* orange */ 	"#A04040",
	/* blue */		"blue4",
	/* magenta */	"#C0C0B0",
	/* cyan */		"#0064A4",
	/* white */		"#E0E0DF0",
	/* brown */		"#333333",
	/* lred */		"#FF3333",
	/* lgreen */	"#009900",
	/* yellow */	"#FFFF00",
	/* lblue */		"#0000B3",
	/* lmagenta */	"#FF00FF",
	/* lcyan */		"#1C99CC",
	/* lwhite */	"#FFFFFF"
 },
// Twilight
 {
 	/* black */		"#141414",
	/* red  */		"#804040",
	/* green */		"#407040",
	/* orange */ 	"#F0AD80",
	/* blue */		"#8996F8",
	/* magenta */	"#404040",
	/* cyan */		"#006688",
	/* white */		"#E0E0E0",
	/* brown */		"#707070",
	/* lred */		"#CD7C33",
	/* lgreen */	"#90EE90",
	/* yellow */	"#FFFF00",
	/* lblue */		"#00B0CF",
	/* lmagenta */	"#FF00FF",
	/* lcyan */		"#F9EE98",
	/* lwhite */	"#FFFFFF"
 },
// Lunar
 {
 	/* black */		"#202020",
	/* red  */		"#D06060",
	/* green */		"#60B060",
	/* orange */ 	"#E68000",
	/* blue */		"#0060A0",
	/* magenta */	"#444433",
	/* cyan */		"#006181",
	/* white */		"#E8D8D8",
	/* brown */		"#4D4D4D",
	/* lred */		"#FF4D4D",
	/* lgreen */	"#A0F0A0",
	/* yellow */	"#FFFF00",
	/* lblue */		"#0000FF",
	/* lmagenta */	"#FF90FF",
	/* lcyan */		"#40C0FF",
	/* lwhite */	"#FFFFFF"
 },
// Velvet
 {
 	/* black */		"#1C1C1C",
	/* red  */		"#D06060",
	/* green */		"#699854",
	/* orange */ 	"#CD9000",
	/* blue */		"#0060A0",
	/* magenta */	"#848484",
	/* cyan */		"#006181",
	/* white */		"#D3D3D3",
	/* brown */		"#343435",
	/* lred */		"#F24646",
	/* lgreen */	"#B4BF76",
	/* yellow */	"#FFFF00",
	/* lblue */		"#559BD5",
	/* lmagenta */	"#C485BF",
	/* lcyan */		"#9BDBFC",
	/* lwhite */	"#FFFFFF"
 }
};

color_curses *current_color;

color_curses color_t[COLOR_SCHEMES][COLOR_TYPES] = {
 /* Snow */
 {
	{BLACK,0},		/* Foreground FOREGROUND*/
	{WHITE,0},		/* Background BACKGROUND*/
	{LWHITE,0},		/* Modeline Foreground MODEFORE*/
	{CYAN,0},		/* Modeline BACKGOUND  MODEBACK*/
	{YELLOW,0},		/* Display row-column DROWCOL*/
	{WHITE,0},		/* search foreground SEARFORE*/
	{RED,0},		/* search background SEARBACK*/
	{BROWN,0},		/* QUOTEFORE */
	{GREEN,0},		/* QUOTEBACK */
	{ORANGE,0},		/* COMMENTFORE */
	{MAGENTA,0},	/* LBACKGROUND */
	{LCYAN,0},		/* PREPFORE */
	{LRED,0},		/* CHANGEDFORE */
	{BROWN,0},		/* TAGFORE */
	{RED,0},		/* WORD1FORE */
	{LBLUE,0},		/* SPECFORE */
	{LGREEN,0},		/* SQUOTEFORE */
	{LRED,0},		/* CTRLFORE */
	{LMAGENTA,0},	/* WORD2FORE */
	{LGREEN,0},		/* W_FORE */
	{LMAGENTA,0},	/* ORIZON */
	{BLUE,0},		/* CNUMERIC */
	{BROWN,0},		/* INFOFORE  */
	{MAGENTA,0},	/* INFOBACK  */
	{WHITE,0},		/* CBOXTFORE */
	{BROWN,0},		/* CBOXTBACK */
	{WHITE,0},		/* MENUFG  */
	{BLUE,0},		/* MENUBG  */
	{WHITE,0},		/* MODEFOREI */
	{BROWN,0},		/* MODEBACKI */
 },

 /* Midnight */
 {
	{WHITE,0},	/* Foreground FOREGR*/
	{BLACK,0},	/* Background BACKGR*/
	{LWHITE,0},	/* Modeline Foreground MODEFORE*/
	{CYAN,0},	/* Modeline BACKGOUND  MODEBACK*/
	{YELLOW,A_BOLD},	/* Display row-column DROWCOL*/
	{LWHITE,0},	/* search foreground SEARFORE*/
	{RED,0},	/* search background SEARBACK*/
	{WHITE,0},	/* QUOTEFORE */
	{GREEN,0},	/* QUOTEBACK */
	{ORANGE,0},	/* COMMENTFORE */
	{BLACK,0},	/* LBACKGROUND */
	{YELLOW,0},	/* PREPFORE */
	{ORANGE,0},	/* CHANGEDFORE */
	{LGREEN,0},	/* TAGFORE */
	{RED,0},	/* WORD1FG */
	{LGREEN,0},	/* SPECFORE */
	{GREEN,0},	/* SQUOTEFORE */
	{LRED,0},	/* CTRLFORE */
	{LCYAN,0},	/* WORD2FORE */
	{LMAGENTA,0},	/* W_FORE */
	{LMAGENTA,0}, /* ORIZON */
	{LWHITE,0},	/* CNUMERIC */
	{WHITE,0},	/* INFOFORE  */
	{MAGENTA,0},	/* INFOBACK  */
	{WHITE,0},	/* CBOXTFORE */
	{CYAN,0},	/* CBOXTBACK */
	{LWHITE,0},		/* MENUFG  */   
	{BLUE,0},       /* MENUBG  */   
	{WHITE,0},       /* MODEFOREI */ 
 	{BROWN,0}       /* MODEBACKI */ 
},

 /* Sea - cyan */
 {
	{WHITE,0},	/* Foreground FOREGR*/
	{CYAN,0},	/* Background BACKGR*/
	{LWHITE,0},	/* Modeline Foreground MODEFORE*/
	{BLUE,0},	/* Modeline BACKGOUND  MODEBACK*/
	{YELLOW,0},	/* Display row-column DROWCOL*/
	{LWHITE,0},	/* search foreground SEARFORE*/
	{RED,0},	/* search background SEARBACK*/
	{WHITE,0},	/* QUOTEFORE */
	{GREEN,0},	/* QUOTEBACK */
	{ORANGE,0},	/* COMMENTFORE */
	{BLUE,0},	/* LBACKGROUND */
	{YELLOW,0},	/* PREPFORE */
	{LRED,0},	/* CHANGEDFORE */
	{LGREEN	,0},/* TAGFORE */
	{RED,0},	/* WORD1FORE */
	{LGREEN,0},	/* SPECFORE */
	{GREEN,0},	/* SQUOTEFORE */
	{LRED,0},	/* CTRLFORE */
	{LCYAN,0},	/* WORD2FORE */
	{LMAGENTA,0},	/* W_FORE */
	{LMAGENTA,0}, /* ORIZON */
	{LWHITE,0},	/* CNUMERIC */
	{WHITE,0},	/* INFOFORE  */
	{MAGENTA,0},	/* INFOBACK  */
	{WHITE,0},	/* CBOXTFORE */
	{BROWN,0},	/* CBOXTBACK */
	{WHITE,0},
	{BLUE,0},
	{WHITE,0},
	{BROWN,0}
 },

 /* Blue */
 {
	{WHITE,0},	/* Foreground FOREGR*/
	{BLUE,0},	/* Background BACKGR*/
	{LWHITE,0},	/* Modeline Foreground MODEFORE*/
	{CYAN,0},	/* Modeline BACKGOUND  MODEBACK*/
	{YELLOW,0},	/* Display row-column DROWCOL*/
	{LWHITE,0},	/* search foreground SEARFORE*/
	{RED,0},	/* search background SEARBACK*/
	{WHITE,0},	/* QUOTEFORE */
	{GREEN,0},	/* QUOTEBACK */
	{ORANGE,0},	/* COMMENTFORE */
	{CYAN,0},	/* LBACKGROUND */
	{YELLOW,0},	/* PREPFORE */
	{ORANGE,0},	/* CHANGEDFORE */
	{LGREEN,0},	/* TAGFORE */
	{RED,0},	/* WORD1FORE */
	{LGREEN,0},	/* SPECFORE */
	{GREEN,0},	/* SQUOTEFORE */
	{LRED,0},	/* CTRLFORE */
	{LCYAN,0},	/* WORD2FORE */
	{LMAGENTA,0},	/* W_FORE */
	{LMAGENTA,0},/* ORIZON */
	{LWHITE,0},	/* CNUMERIC */
	{WHITE,0},	/* INFOFORE  */
	{MAGENTA,0},	/* INFOBACK  */
	{WHITE,0},	/* CBOXTFORE */
	{CYAN,0},	/* CBOXTBACK */
	{WHITE,0},	/* MENU_FG */
	{CYAN,0},	/* MENU_BG */
	{WHITE,0},
	{BROWN,0}
 },
 
 /* GREY1 */
 {
	{BLACK,1},	/* Foreground FOREGR*/
	{WHITE,0},	/* Background BACKGR*/
	{LWHITE,0},	/* Modeline Foreground MODEFORE*/
	{CYAN,0},	/* Modeline BACKGOUND  MODEBACK*/
	{YELLOW,0},	/* Display row-column DROWCOL*/
	{LWHITE,0},	/* search foreground SEARFORE*/
	{RED,0},	/* search background SEARBACK*/
	{LWHITE,0},	/* QUOTEFORE */
	{GREEN,0},	/* QUOTEBACK */
	{ORANGE,0},	/* COMMENTFORE */
	{MAGENTA,0},	/* LBACKGROUND */
	{YELLOW,0},	/* PREPFORE */
	{ORANGE,0},	/* CHANGEDFORE */
	{LCYAN,0},	/* TAGFORE */
	{LMAGENTA,0},	/* WORD1FORE */
	{LGREEN,1},	/* SPECFORE */
	{GREEN,0},	/* SQUOTEFORE */
	{LRED,0},	/* CTRLFORE */
	{RED,0},	/* WORD2FORE */
	{LCYAN,0},	/* W_FORE */
	{LMAGENTA,0}, /* ORIZON */
	{LBLUE,0},	/* CNUMERIC */
	{BLACK,0},	/* INFOFORE  */
	{MAGENTA,0},/* INFOBACK  */
	{WHITE,0},	/* CBOXTFORE */
	{CYAN,0},	/* CBOXTBACK */
	{LWHITE,0},
	{BLUE,0},
	{LWHITE,0},
	{BROWN,0}
 },

 /* Dark gray */
 {
	{LWHITE,0},	/* Foreground FOREGR*/
	{BLACK,0},	/* Background BACKGR*/
	{LWHITE,0},	/* Modeline Foreground MODEFORE*/
	{CYAN,0},	/* Modeline BACKGOUND  MODEBACK*/
	{YELLOW,0},	/* Display row-column DROWCOL*/
	{LWHITE,0},	/* search foreground SEARFORE*/
	{RED,0},	/* search background SEARBACK*/
	{WHITE,0},	/* QUOTEFORE */
	{GREEN,0},	/* QUOTEBACK */
	{ORANGE,0},	/* COMMENTFORE */
	{MAGENTA,0},	/* LBACKGROUND */
	{YELLOW,0},	/* PREPFORE */
	{ORANGE,0},  /* CHANGEDFORE */
	{ORANGE,0},	/* TAGFORE */
	{LRED,0},	/* WORD1FORE */
	{LGREEN,0},	/* SPECFORE */
	{LGREEN,0},	/* SQUOTEFORE */
	{RED,0},	/* CTRLFORE */
	{LBLUE,0},	/* WORD2FORE */
	{RED,0},	/* W_FORE */
	{LMAGENTA,0}, /* ORIZON */
	{LCYAN,0},	/* CNUMERIC */
	{WHITE,0},	/* INFOFORE  */
	{MAGENTA,0},/* INFOBACK  */
	{WHITE,0},	/* CBOXTFORE */
	{CYAN,0},	/* CBOXTBACK */
	{WHITE,0},
	{CYAN,0},
	{WHITE,0},
	{BROWN,0}
 },
 /* Sunny */
 {
	{BLACK,0},	/* Foreground FOREGR*/
	{WHITE,0},	/* Background BACKGR*/
	{LWHITE,0},	/* Modeline Foreground MODEFORE*/
	{CYAN,0},	/* Modeline BACKGOUND  MODEBACK*/
	{YELLOW,0},	/* Display row-column DROWCOL*/
	{LWHITE,0},	/* search foreground SEARFORE*/
	{RED,0},	/* search background SEARBACK*/
	{WHITE,0},/* QUOTEFORE */
	{GREEN,0},	/* QUOTEBACK */
	{ORANGE,0},	/* COMMENTFORE */
	{WHITE,0},	/* LBACKGROUND */
	{LRED,0},	/* PREPFORE */
	{ORANGE,0},	/* CHANGEDFORE */
	{ORANGE,0},	/* TAGFORE */
	{LRED,0},	/* WORD1FORE */
	{LGREEN,0},	/* SPECFORE */
	{LGREEN,0},	/* SQUOTEFORE */
	{LRED,0},	/* CTRLFORE */
	{LBLUE,0},	/* WORD2FORE */
	{GREEN,0},	/* W_FORE */
	{LMAGENTA,0}, /* ORIZON */
	{LCYAN,0},	/* CNUMERIC */
	{BLACK,0},	/* INFOFORE  */
	{MAGENTA,0},/* INFOBACK  */
	{LWHITE,0},	/* CBOXTFORE */
	{CYAN,0},	/* CBOXTBACK */
	{WHITE,0},
	{CYAN,0},
	{WHITE,0},
	{BROWN,0}
 },
// Twilight
 {
	{WHITE,0},	/* Foreground FOREGR*/
	{BLACK,0},	/* Background BACKGR*/
	{LWHITE,0},	/* Modeline Foreground MODEFORE*/
	{CYAN,0},	/* Modeline BACKGOUND  MODEBACK*/
	{YELLOW,0},	/* Display row-column DROWCOL*/
	{LWHITE,0},	/* search foreground SEARFORE*/
	{RED,0},	/* search background SEARBACK*/
	{LWHITE,0},	/* QUOTEFORE */
	{GREEN,0},	/* QUOTEBACK */
	{BROWN,0},	/* COMMENTFORE */
	{MAGENTA,0},/* LBACKGROUND */
	{BLUE,0},	/* PREPFORE */
	{ORANGE,0},	/* CHANGEDFORE */
	{ORANGE,0},	/* TAGFORE */
	{LRED,0},	/* WORD1BACK */
	{LGREEN,0},	/* SPECFORE */
	{LGREEN,0},	/* SQUOTEFORE */
	{LRED,0},	/* CTRLFORE */
	{LCYAN,0},	/* WORD2FORE */
	{LMAGENTA,0},	/* W_FORE */
	{LMAGENTA,0}, /* ORIZON */
	{ORANGE,0},	/* CNUMERIC */
	{WHITE,0},	/* INFOFORE  */
	{MAGENTA,0},	/* INFOBACK  */
	{WHITE,0},	/* CBOXTFORE */
	{CYAN,0},	/* CBOXTBACK */
	{WHITE,0},
	{CYAN,0},
	{WHITE,0},
	{BROWN,0}
 },
 /* Lunar */
 {
	{WHITE,0},	/* Foreground FOREGR*/
	{BLACK,0},	/* Background BACKGR*/
	{LWHITE,0},	/* Modeline Foreground MODEFORE*/
	{CYAN,0},	/* Modeline BACKGOUND  MODEBACK*/
	{YELLOW,0},	/* Display row-column DROWCOL*/
	{LWHITE,0},	/* search foreground SEARFORE*/
	{RED,0},	/* search background SEARBACK*/
	{ORANGE,0},	/* QUOTEFORE */
	{BLACK,0},	/* QUOTEBACK */
	{GREEN,0},	/* COMMENTFORE */
	{MAGENTA,0},	/* LBACKGROUND */
	{LMAGENTA,0},	/* PREPFORE */
	{LMAGENTA,0},	/* CHANGEFORE */
	{LGREEN,0},	/* TAGFORE */
	{RED,0},	/* WORD1FG */
	{LGREEN,0},	/* SPECFORE */
	{GREEN,0},	/* SQUOTEFORE */
	{LRED,0},	/* CTRLFORE */
	{LCYAN,0},	/* WORD2FORE */
	{LMAGENTA,0},	/* W_FORE */
	{LMAGENTA,0}, /* ORIZON */
	{LGREEN,0},	/* CNUMERIC */
	{WHITE,0},	/* INFOFORE  */
	{MAGENTA,0},	/* INFOBACK  */
	{WHITE,0},	/* CBOXTFORE */
	{CYAN,0},	/* CBOXTBACK */
	{WHITE,0},
	{BLUE,0},
	{WHITE,0},
	{BROWN,0}
 },
 /* Velvet */
 {
	{WHITE,0},	/* Foreground FOREGR*/
	{BLACK,0},	/* Background BACKGR*/
	{LWHITE,0},	/* Modeline Foreground MODEFORE*/
	{CYAN,0},	/* Modeline BACKGOUND  MODEBACK*/
	{YELLOW,0},	/* Display row-column DROWCOL*/
	{LWHITE,0},	/* search foreground SEARFORE*/
	{RED,0},	/* search background SEARBACK*/
	{ORANGE,0},	/* QUOTEFORE */
	{BLACK,0},	/* QUOTEBACK */
	{GREEN,0},	/* COMMENTFORE */
	{BLACK,0},	/* LBACKGROUND */
	{YELLOW,0},	/* PREPFORE */
	{ORANGE,0},	/* CHANGEDFORE */
	{LGREEN,0},	/* TAGFORE */
	{RED,0},	/* WORD1FG */
	{LWHITE,0},	/* SPECFORE */
	{GREEN,0},	/* SQUOTEFORE */
	{LRED,0},	/* CTRLFORE */
	{LBLUE,0},	/* WORD2FORE */
	{LMAGENTA,0},	/* W_FORE */
	{LMAGENTA,0}, /* ORIZON */
	{LGREEN,0},	/* CNUMERIC */
	{MAGENTA,0},	/* INFOFORE  */
	{BROWN,0},	/* INFOBACK  */
	{WHITE,0},	/* CBOXTFORE */
	{CYAN,0},	/* CBOXTBACK */
	{WHITE,0},
	{BLUE,0},
	{WHITE,0},
	{BROWN,0}
 }
};

typedef struct CPAIR {
	int fg_color;
	int bg_color;
	int attr;
} CPAIR;

#define MAX_PAIRS	84
CPAIR color_pairs[MAX_PAIRS] = {
	{ FOREGROUND,BACKGROUND,0},
	{ QUOTEFORE,BACKGROUND,0},
	{ SPECFORE,BACKGROUND,0},
	{ SPECFORE,BACKGROUND,1},
	{ SQUOTEFORE,BACKGROUND,1},
	{ PREPFORE,BACKGROUND,1},
	{ CTRLFORE,BACKGROUND,1},
	{ TAGFORE,BACKGROUND,0},
	{ TAGFORE,BACKGROUND,1},
	{ W_FORE,BACKGROUND,0},
	{ WORD1FORE,BACKGROUND,0},
	{ WORD2FORE,BACKGROUND,0},
	{ CNUMERIC,BACKGROUND,0},
	{ ORIZON,BACKGROUND,0},
	{ COMMENTFORE,BACKGROUND,0},
	{ QUOTEFORE,QUOTEBACK,0},
	{ PREPFORE,BACKGROUND,1},
	{ CTRLFORE,MODEBACK,1},
	{ CTRLFORE,QUOTEBACK,1},

	{ FOREGROUND, CBOXTBACK,0},
	{ QUOTEFORE , CBOXTBACK,0},
	{ SPECFORE  , CBOXTBACK,0},
	{ SPECFORE  , CBOXTBACK,1},
	{ SQUOTEFORE, CBOXTBACK,1},
	{ PREPFORE  , CBOXTBACK,1},
	{ CTRLFORE  , CBOXTBACK,1},
	{ TAGFORE   , CBOXTBACK,0},
	{ TAGFORE   , CBOXTBACK,1},
	{ W_FORE    , CBOXTBACK,0},
	{ WORD1FORE , CBOXTBACK,0},
	{ WORD2FORE , CBOXTBACK,0},
	{ CNUMERIC  , CBOXTBACK,0},
	{ ORIZON	, CBOXTBACK,0},
	{ COMMENTFORE,CBOXTBACK,0},
// for selection

	{ FOREGROUND,INFOBACK,0},
	{ QUOTEFORE,INFOBACK,0},
	{ SQUOTEFORE,INFOBACK,0},
	{ SPECFORE,INFOBACK,0},
	{ SPECFORE,INFOBACK,1},
	{ PREPFORE,INFOBACK,1},
	{ CTRLFORE,INFOBACK,1},
	{ TAGFORE,INFOBACK,0},
	{ TAGFORE,INFOBACK,1},
	{ W_FORE,INFOBACK,0},
	{ WORD1FORE,INFOBACK,0},
	{ WORD2FORE,INFOBACK,0},
	{ CNUMERIC,INFOBACK,1},
	{ CNUMERIC,INFOBACK,0},
	{ ORIZON,INFOBACK,0},
	{ COMMENTFORE,INFOBACK,0},

	{ FOREGROUND,MODEBACK,0},
	{ QUOTEFORE,MODEBACK,0},
	{ SPECFORE,MODEBACK,0},
	{ SPECFORE,MODEBACK,1},
	{ SQUOTEFORE,MODEBACK,1},
	{ PREPFORE,MODEBACK,1},
	{ CTRLFORE,MODEBACK,1},
	{ TAGFORE,MODEBACK,0},
	{ TAGFORE,MODEBACK,1},
	{ W_FORE,MODEBACK,0},
	{ WORD1FORE,MODEBACK,0},
	{ WORD2FORE,MODEBACK,0},
	{ CNUMERIC,MODEBACK,0},
	{ ORIZON,MODEBACK,0},
	{ COMMENTFORE,MODEBACK,0},

	{ INFOFORE,INFOBACK,0},
	{ INFOBACK,INFOFORE,0},
	{ SEARFORE,SEARBACK,0},

	{ CBOXTFORE,CBOXTBACK,0},	/* box outside, modeline   */
	{ DROWCOL,CBOXTBACK,0},	/* modeline row col  */
	{ MENU_FG,MENU_BG,0},	/* normal menu,inactive box inside  */
	{ MENU_BG,MENU_FG,0},	/* active box inside  */
	{ CBOXTFORE,CBOXTBACK,0},
	{ CBOXTFORE,CBOXTBACK,1},
	{ CBOXTBACK,CBOXTFORE,0},
	{ CBOXTBACK,CBOXTFORE,1},
	{ CTRLFORE,MENU_BG,0},
	{ CTRLFORE,MENU_FG,0},
	{ CTRLFORE,MENU_BG,1},
	{ CTRLFORE,MENU_FG,1},
	{ DROWCOL,MODEBACK,1},
	{ MODEFORE,MODEBACK,0},
	{ CHANGEFORE,MODEBACK,0},
	{ CHANGEFORE,MODEBACKI,0}
};

char *color_type[] = {
	"normal_fg",	/* 0, Foreground FOREGR*/
	"normal_bg",	/* 1, Background BACKGR*/
	"modeline_fg",	/* 2, Modeline Foreground */
	"modeline_bg",  /* 3, MODEFORE, background, selections bg */
	"rowcol_fg",	/* 4, Display row-column DROWCOL*/
	"search_fg",	/* 5, search foreground */
	"search_bg",	/* 6, SEARFORE,SEARBACK */
	"quote_fg",		/* 7, double QUOTEFORE or H_QUOTE2 */
	"quote_bg",		/* 8, double QUOTEBACK */
	"comment_fg",	/* 9, COMMENTFORE H_QUOTE5 */
	"light_bg",		/* 10, LBACKGROUND */
	"prep_fg",		/* 11, PREPFORE H_QUOTE6 */
	"changed_fg",	/* 12, changed foreground */
	"tag_fg",		/* 13, TAGFORE H_QUOTE4, H_QUOTE8 */
	"word1_fg",		/* 14, WORD1FORE */
	"spec_fg",		/* 15, SPECFORE H_QUOTE1 single quotes, special chars */
	"squote_fg",	/* 15, H_QUOTE1 single quotes, special chars */
	"ctrl_fg",		/* 16, CTRLFORE */
	"word2_fg",		/* 17, WORD2FORE */
	"w_fore",		/* 18, W_FORE H_QUOTE7 tag words for html/xml */
	"orizon_fg",	/* 19, ORIZON  */
	"numeric_fg",	/* 20, CNUMERIC */
	"info_fg",		/* 23, Info line foreground  */
	"info_bg",		/* 24, Info line background  */
	"box_fg",		/* 27 box foreground */
	"box_bg", 		/* 28 box background */
	"menu_fg",		/* 29 menu foreground  */
	"menu_bg",		/* 30 menu background */
	"imodeline_fg",	/* 21, Modeline inactive FG MODEFOREI, CBOXTFORE */
	"imodeline_bg",	/* 22, Modeline inactive BG MODEBACKI, CBOXTBACK */
};

char *basic_color_names[] = {
	"black","red","green","orange","blue","magenta","cyan","white",
	"brown","lred","lgreen","yellow","lblue","lmagenta","lcyan","lwhite"
};

/* --- */
