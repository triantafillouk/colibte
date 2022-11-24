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

// 16 colors to 8 colors translation
// for black backgrounds!!
int color16_8[] = {
 BLACK,		/* COLOR_BG  */
 BLUE,		/* MENU_BG  */
 CYAN,		/* SELECT_BG  */
 RED,		/* SEARCH_BG  */
 GREEN,		/* QUOTE_BG  */
 BROWN,		/* LIGHT_BG  */
 BLACK,		/* INFO_BG  */
 BLUE,		/* INACTIVE_BG  */
 BLUE,		/* BOX_BG  */

 BROWN,		/* CODE_BG */
 WHITE,		/* FG  */
 WHITE,		/* MENU_FG */
 LWHITE,	/* STANDOUT_FG */
 LRED,		/* CTRL_FG */
 YELLOW,	/* PREP_FG  */
 LCYAN,		/* WORD1  */
 LMAGENTA,	/* WORD2  */
 LBLUE,		/* WORD3  */
 LGREEN,	/* SPEC  */
 GREEN,		/* SQUOTE_FG */
 ORANGE,	/* COMMENT_FG  */
 ORANGE,	/* CHANGED_FG  */
 LMAGENTA,	/* HORIZON_FG  */
 WHITE,		/* INACTIVE_FG  */
 YELLOW		/* ROWCOL_FG  */
};

int color8_16[] = {
	COLOR_BG,			/* BLACK  */
	COLOR_SEARCH_BG,	/* RED  */
	COLOR_QUOTE_BG,		/* GREEN  */
	COLOR_COMMENT_FG,	/* ORANGE  */
	COLOR_MENU_BG,		/* BLUE  */
	COLOR_INFO_BG,		/* MAGENTA  */
	COLOR_SELECT_BG,	/* CYAN  */
	COLOR_FG,			/* WHITE  */

	COLOR_LIGHT_BG,		/* BROWN  */
	COLOR_WORD2_FG,		/* LRED  */
	COLOR_SPEC_FG,		/* LGREEN  */
	COLOR_ROWCOL_FG,	/* YELLOW  */
	COLOR_STANDOUT_FG,	/* LWHITE  */
	COLOR_HORIZON_FG,	/* LMAGENTA  */
	COLOR_WORD1_FG,		/* LCYAN  */
	COLOR_MENU_FG		/* LWHITE  */
};

#if NEW_COLORS
#define	COLOR_TYPES	FG_COLORS+BG_COLORS
// int pair_num[FG_COLORS][BG_COLORS];
#else
#define	COLOR_TYPES	XCOLOR_TYPES
// int pair_num[XCOLOR_TYPES][XCOLOR_TYPES];
#endif

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
	RGB_COLORS basic_colors[34];
	color_curses color_attr[34];
} COLOR_SCHEME;

#if	NEW_COLORS
// #define XCOLOR_TYPES	BG_COLORS+FG_COLORS
char *basic_color_values[COLOR_SCHEMES][XCOLOR_TYPES] = {
 // Snow
 {
	"#E3E3E3", // COLOR_BG,			BACKGROUND
	"#0000AE", // COLOR_MENU_BG,		MENU_BG
	"#80B0D0", // COLOR_SELECT_BG,	MODEBACK
	"#AE0000", // COLOR_SEARCH_BG,	SEARBACK
	"#A0F0A0", // COLOR_QUOTE_BG,		QUOTEBACK
	"#B0B0C0", // COLOR_LIGHT_BG,		LBACKGROUND
	"#D0C0B0", // COLOR_INFO_BG,		INFOBACK
	"#A0A0A0", // COLOR_INACTIVE_BG,	MODEBACKI
	"#808080", // COLOR_BOX_BG,		CBOXTBACK
	"#A8A8A8", // COLOR_CODE_BG		color code bg

	"#000000", // COLOR_FG=BG_COLORS,	FOREGROUND, INFOFORE, QUOTEFORE,CBOXTFORE
	"#FFFFFF", // COLOR_MENU_FG
	"#0000AE", // COLOR_STANDOUT_FG,	SEARFORE, CNUMERIC, MENUSTART, MENU_FG, CBOXFORE
	"#FF0000", // COLOR_CTRL_FG,		menu start fg, CTRLFORE
	"#00A080", // COLOR_PREP_FG,		PREPFORE
	"#AE0000", // COLOR_WORD1_FG,		WORD1FORE
	"#800080", // COLOR_WORD2_FG,		WORD2FORE
	"#98E098", // COLOR_WORD3_FG,		W_FORE
	"#0000AE", // COLOR_SPEC_FG,		SPECFORE
	"#006000", // COLOR_SQUOTE_FG,	SQUOTEFORE
	"#906000", // COLOR_COMMENT_FG,	COMMENTFORE
	"#FF0000", // COLOR_CHANGE_FG,	CHANGEFORE
	"#800080", // COLOR_HORIZON_FG,	ORIZON
	"#E3E3E3", // COLOR_INACTIVE_FG,	MODEFOREI
	"#FFFF00"  // COLOR_ROWCOL_FG		DROWCOL
 },
// Midnight
 {
	"#000000", // COLOR_BG,			BACKGROUND
	"#0070A0", // COLOR_MENU_BG,		MENU_BG
	"#005070", // COLOR_SELECT_BG,	MODEBACK
	"#A00000", // COLOR_SEARCH_BG,	SEARBACK
	"#008000", // COLOR_QUOTE_BG,		QUOTEBACK
	"#202025", // COLOR_LIGHT_BG,		LBACKGROUND
	"#303030", // COLOR_INFO_BG,		INFOBACK
	"#303030", // COLOR_INACTIVE_BG,	MODEBACKI
	"#303030", // COLOR_BOX_BG,		CBOXTBACK ???????
	"#404040", // COLOR_CODE_BG		color code bg
	"#B0B0B0", // COLOR_FG=BG_COLORS,	FOREGROUND, INFOFORE, QUOTEFORE,CBOXTFORE
	"#E0E0E0", // COLOR_MENU_FG
	"#FFF0F0", // COLOR_STANDOUT_FG,	SEARFORE, CNUMERIC, CBOXFORE
	"#F00000", // COLOR_CTRL_FG,		menu start fg, CTRLFORE
	"#F0F000", // COLOR_PREP_FG,		PREPFORE
	"#D00000", // COLOR_WORD1_FG,		WORD1FORE
	"#00D0D0", // COLOR_WORD2_FG,		WORD2FORE
	"#D0D0D0", // COLOR_WORD3_FG,		W_FORE
	"#98E098", // COLOR_SPEC_FG,		SPECFORE
	"#208020", // COLOR_SQUOTE_FG,	SQUOTEFORE
	"#F09000", // COLOR_COMMENT_FG,	COMMENTFORE
	"#F09090", // COLOR_CHANGE_FG,	CHANGEFORE
	"#A000A0", // COLOR_HORIZON_FG,	ORIZON
	"#808080", // COLOR_INACTIVE_FG,	MODEFOREI
	"#FFFF00"  // COLOR_ROWCOL_FG		DROWCOL
 },
// Cyan
 {
	"#004050", // COLOR_BG,			BACKGROUND
	"#005080", // COLOR_MENU_BG,		MENU_BG
	"#4060A0", // COLOR_SELECT_BG,	MODEBACK
	"#D00000", // COLOR_SEARCH_BG,	SEARBACK
	"#008000", // COLOR_QUOTE_BG,		QUOTEBACK
	"#506070", // COLOR_LIGHT_BG,		LBACKGROUND
	"#303030", // COLOR_INFO_BG,		INFOBACK
	"#303030", // COLOR_INACTIVE_BG,	MODEBACKI
	"#0050A0", // COLOR_BOX_BG,		CBOXTBACK ???????
	"#404040", // COLOR_CODE_BG		color code bg
	"#D0D0D0", // COLOR_FG=BG_COLORS,	FOREGROUND, INFOFORE, QUOTEFORE,CBOXTFORE
	"#FFFFFF", // COLOR_MENU_FG
	"#FFFFFF", // COLOR_STANDOUT_FG,	SEARFORE, CNUMERIC, CBOXFORE
	"#F00000", // COLOR_CTRL_FG,		menu start fg, CTRLFORE
	"#FFFF00", // COLOR_PREP_FG,		PREPFORE
	"#D00000", // COLOR_WORD1_FG,		WORD1FORE
	"#00D0D0", // COLOR_WORD2_FG,		WORD2FORE
	"#FF00FF", // COLOR_WORD3_FG,		W_FORE
	"#00FF00", // COLOR_SPEC_FG,		SPECFORE
	"#F08020", // COLOR_SQUOTE_FG,	SQUOTEFORE
	"#F09000", // COLOR_COMMENT_FG,	COMMENTFORE
	"#FF0000", // COLOR_CHANGE_FG,	CHANGEFORE
	"#FF00FF", // COLOR_HORIZON_FG,	ORIZON
	"#808080", // COLOR_INACTIVE_FG,	MODEFOREI
	"#FFFF00"  // COLOR_ROWCOL_FG		DROWCOL
 },
// Blue Sky
 {
	"#002060", // COLOR_BG,			BACKGROUND
	"#008080", // COLOR_MENU_BG,		MENU_BG
	"#005070", // COLOR_SELECT_BG,	MODEBACK
	"#D00000", // COLOR_SEARCH_BG,	SEARBACK
	"#008000", // COLOR_QUOTE_BG,		QUOTEBACK
	"#204060", // COLOR_LIGHT_BG,		LBACKGROUND
	"#404030", // COLOR_INFO_BG,		INFOBACK
	"#004040", // COLOR_INACTIVE_BG,	MODEBACKI
	"#008080", // COLOR_BOX_BG,		CBOXTBACK ???????
	"#004050", // COLOR_CODE_BG		color code bg
	"#D0D0D0", // COLOR_FG=BG_COLORS,	FOREGROUND, INFOFORE, QUOTEFORE,CBOXTFORE
	"#FFFFFF", // COLOR_MENU_FG
	"#FFF0F0", // COLOR_STANDOUT_FG,	SEARFORE, CNUMERIC, CBOXFORE
	"#FF0000", // COLOR_CTRL_FG,		menu start fg, CTRLFORE
	"#FFFF00", // COLOR_PREP_FG,		PREPFORE
	"#D00000", // COLOR_WORD1_FG,		WORD1FORE
	"#00FFFF", // COLOR_WORD2_FG,		WORD2FORE
	"#F000F0", // COLOR_WORD3_FG,		W_FORE
	"#00FF00", // COLOR_SPEC_FG,		SPECFORE
	"#008000", // COLOR_SQUOTE_FG,	SQUOTEFORE
	"#F09000", // COLOR_COMMENT_FG,	COMMENTFORE
	"#F09000", // COLOR_CHANGE_FG,	CHANGEFORE
	"#FF00FF", // COLOR_HORIZON_FG,	ORIZON
	"#808080", // COLOR_INACTIVE_FG,	MODEFOREI
	"#FFFF00"  // COLOR_ROWCOL_FG		DROWCOL
 },
// Gray
 {
	"#909090", // COLOR_BG,			BACKGROUND
	"#004090", // COLOR_MENU_BG,		MENU_BG
	"#5080A0", // COLOR_SELECT_BG,	MODEBACK
	"red4", // COLOR_SEARCH_BG,	SEARBACK
	"#508050", // COLOR_QUOTE_BG,		QUOTEBACK
	"#858585", // COLOR_LIGHT_BG,		LBACKGROUND
	"#A8A8a0", // COLOR_INFO_BG,		INFOBACK
	"#707070", // COLOR_INACTIVE_BG,	MODEBACKI
	"#5080A0", // COLOR_BOX_BG,		CBOXTBACK
	"#D0D0D0", // COLOR_CODE_BG		color code bg

	"#000000", // COLOR_FG=BG_COLORS,FOREGROUND,INFOFORE,QUOTEFORE,CBOXTFORE
	"#F2F2F2", // COLOR_MENU_FG
	"#004090", // COLOR_STANDOUT_FG,	SEARFORE, CNUMERIC, MENUSTART, MENU_FG, CBOXFORE
	"#FF4D4D", // COLOR_CTRL_FG,		menu start fg, CTRLFORE
	"#FFFF00", // COLOR_PREP_FG,		PREPFORE
	"#8C008C", // COLOR_WORD1_FG,		WORD1FORE
	"red4", // COLOR_WORD2_FG,		WORD2FORE
	"#00FFFF", // COLOR_WORD3_FG,		W_FORE
	"#00D0D0", // COLOR_SPEC_FG,		SPECFORE
	"#508050", // COLOR_SQUOTE_FG,	SQUOTEFORE
	"#E5D5A5", // COLOR_COMMENT_FG,	COMMENTFORE
	"#E5D5A5", // COLOR_CHANGE_FG,	CHANGEFORE
	"#8C008C", // COLOR_HORIZON_FG,	ORIZON
	"#F2F2F2", // COLOR_INACTIVE_FG,	MODEFOREI
	"#FFFF00"  // COLOR_ROWCOL_FG		DROWCOL
 },
// Dark Gray
 {
	"#303030", // COLOR_BG,			BACKGROUND
	"#336194", // COLOR_MENU_BG,		MENU_BG
	"#005070", // COLOR_SELECT_BG,	MODEBACK
	"#FA0000", // COLOR_SEARCH_BG,	SEARBACK
	"#007A00", // COLOR_QUOTE_BG,		QUOTEBACK
	"#404055", // COLOR_LIGHT_BG,		LBACKGROUND
	"#5F5F5F", // COLOR_INFO_BG,		INFOBACK
	"#434343", // COLOR_INACTIVE_BG,	MODEBACKI
	"#336194", // COLOR_BOX_BG,		CBOXTBACK ???????
	"#404040", // COLOR_CODE_BG		color code bg

	"#F2F2D9", // COLOR_FG=BG_COLORS,	FOREGROUND, INFOFORE, QUOTEFORE,CBOXTFORE
	"#E0E0E0", // COLOR_MENU_FG
	"#FFFFFF", // COLOR_STANDOUT_FG,	SEARFORE, CNUMERIC, CBOXFORE
	"#FA0000", // COLOR_CTRL_FG,		menu start fg, CTRLFORE
	"#FFFF00", // COLOR_PREP_FG,		PREPFORE
	"#FF9999", // COLOR_WORD1_FG,		WORD1FORE
	"#99E6E6", // COLOR_WORD2_FG,		WORD2FORE
	"#FA0000", // COLOR_WORD3_FG,		W_FORE
	"#66FF66", // COLOR_SPEC_FG,		SPECFORE
	"#66FF66", // COLOR_SQUOTE_FG,	SQUOTEFORE
	"#F09000", // COLOR_COMMENT_FG,	COMMENTFORE
	"#F09000", // COLOR_CHANGE_FG,	CHANGEFORE
	"#FF00FF", // COLOR_HORIZON_FG,	ORIZON
	"#909090", // COLOR_INACTIVE_FG,	MODEFOREI
	"#FFFF00"  // COLOR_ROWCOL_FG		DROWCOL
 },
// Sunny
 {
	"#E0E0F0", // COLOR_BG,			BACKGROUND
	"#2020A0", // COLOR_MENU_BG,		MENU_BG
	"#B0C0D0", // COLOR_SELECT_BG,	MODEBACK
	"#AD0000", // COLOR_SEARCH_BG,	SEARBACK
	"#00B000", // COLOR_QUOTE_BG,		QUOTEBACK
	"#B0B0C0", // COLOR_LIGHT_BG,		LBACKGROUND
	"#C0C0B0", // COLOR_INFO_BG,		INFOBACK
	"#777777", // COLOR_INACTIVE_BG,	MODEBACKI
	"#808080", // COLOR_BOX_BG,		CBOXTBACK
	"#A8A8A8", // COLOR_CODE_BG		color code bg

	"#000000", // COLOR_FG=BG_COLORS,	FOREGROUND, INFOFORE, QUOTEFORE,CBOXTFORE
	"#FFFFFF", // COLOR_MENU_FG
	"#1C99CC", // COLOR_STANDOUT_FG,	SEARFORE, CNUMERIC, MENUSTART, MENU_FG, CBOXFORE
	"#FF3333", // COLOR_CTRL_FG,		menu start fg, CTRLFORE
	"#FF3333", // COLOR_PREP_FG,		PREPFORE
	"#AE0000", // COLOR_WORD1_FG,		WORD1FORE
	"#FF00FF", // COLOR_WORD2_FG,		WORD2FORE
	"#A04040", // COLOR_WORD3_FG,		W_FORE
	"#009900", // COLOR_SPEC_FG,		SPECFORE
	"#00A900", // COLOR_SQUOTE_FG,	SQUOTEFORE
	"#A04040", // COLOR_COMMENT_FG,	COMMENTFORE
	"#A04040", // COLOR_CHANGE_FG,	CHANGEFORE
	"#FF00FF", // COLOR_HORIZON_FG,	ORIZON
	"#E3E3E3", // COLOR_INACTIVE_FG,	MODEFOREI
	"#FFFF00"  // COLOR_ROWCOL_FG		DROWCOL
 },
// Twilight
 {
	"#141414", // COLOR_BG,			BACKGROUND
	"#006688", // COLOR_MENU_BG,		MENU_BG
	"#005070", // COLOR_SELECT_BG,	MODEBACK
	"#804040", // COLOR_SEARCH_BG,	SEARBACK
	"#407040", // COLOR_QUOTE_BG,		QUOTEBACK
	"#202530", // COLOR_LIGHT_BG,		LBACKGROUND
	"#404040", // COLOR_INFO_BG,		INFOBACK
	"#707080", // COLOR_INACTIVE_BG,	MODEBACKI
	"#205060", // COLOR_BOX_BG,		CBOXTBACK ???????
	"#454545", // COLOR_CODE_BG		color code bg
	"#E0E0E0", // COLOR_FG=BG_COLORS,	FOREGROUND, INFOFORE, QUOTEFORE,CBOXTFORE
	"#FFFFFF", // COLOR_MENU_FG
	"#F0AD80", // COLOR_STANDOUT_FG,	SEARFORE, CNUMERIC, CBOXFORE
	"#CD7C33", // COLOR_CTRL_FG,		menu start fg, CTRLFORE
	"#FFFF00", // COLOR_PREP_FG,		PREPFORE
	"#CD7C33", // COLOR_WORD1_FG,		WORD1FORE
	"#F9EE98", // COLOR_WORD2_FG,		WORD2FORE
	"#F0AD80", // COLOR_WORD3_FG,		W_FORE
	"#90EE90", // COLOR_SPEC_FG,		SPECFORE
	"#90EE90", // COLOR_SQUOTE_FG,	SQUOTEFORE
	"#707080", // COLOR_COMMENT_FG,	COMMENTFORE
	"#F0AD80", // COLOR_CHANGE_FG,	CHANGEFORE
	"#FF00FF", // COLOR_HORIZON_FG,	ORIZON
	"#E0E0E0", // COLOR_INACTIVE_FG,	MODEFOREI
	"#FFFF00"  // COLOR_ROWCOL_FG		DROWCOL
 },
// Lunar
 {
	"#202020", // COLOR_BG,			BACKGROUND
	"#0070A0", // COLOR_MENU_BG,		MENU_BG
	"#005070", // COLOR_SELECT_BG,	MODEBACK
	"#D06060", // COLOR_SEARCH_BG,	SEARBACK
	"#008000", // COLOR_QUOTE_BG,		QUOTEBACK
	"#202025", // COLOR_LIGHT_BG,		LBACKGROUND
	"#444433", // COLOR_INFO_BG,		INFOBACK
	"#4D4D4D", // COLOR_INACTIVE_BG,	MODEBACKI
	"#506070", // COLOR_BOX_BG,		CBOXTBACK ???????
	"#404040", // COLOR_CODE_BG		color code bg

	"#E8D8D8", // COLOR_FG=BG_COLORS,	FOREGROUND, INFOFORE, QUOTEFORE,CBOXTFORE
	"#E0E0E0", // COLOR_MENU_FG
	"#FFF0F0", // COLOR_STANDOUT_FG,	SEARFORE, CNUMERIC, CBOXFORE
	"#FF4D4D", // COLOR_CTRL_FG,		menu start fg, CTRLFORE
	"#FF90FF", // COLOR_PREP_FG,		PREPFORE
	"#D06060", // COLOR_WORD1_FG,		WORD1FORE
	"#00D0D0", // COLOR_WORD2_FG,		WORD2FORE
	"#D0D0D0", // COLOR_WORD3_FG,		W_FORE
	"#A0F0A0", // COLOR_SPEC_FG,		SPECFORE
	"#60B060", // COLOR_SQUOTE_FG,	SQUOTEFORE
	"#60B060", // COLOR_COMMENT_FG,	COMMENTFORE
	"#F09090", // COLOR_CHANGE_FG,	CHANGEFORE
	"#A000A0", // COLOR_HORIZON_FG,	ORIZON
	"#808080", // COLOR_INACTIVE_FG,	MODEFOREI
	"#FFFF00"  // COLOR_ROWCOL_FG		DROWCOL
 },
// Velvet
 {
	"#1C1C1C", // COLOR_BG,			BACKGROUND
	"#0060A0", // COLOR_MENU_BG,		MENU_BG
	"#005070", // COLOR_SELECT_BG,	MODEBACK
	"#A00000", // COLOR_SEARCH_BG,	SEARBACK
	"#008000", // COLOR_QUOTE_BG,		QUOTEBACK
	"#202025", // COLOR_LIGHT_BG,		LBACKGROUND
	"#303030", // COLOR_INFO_BG,		INFOBACK
	"#404040", // COLOR_INACTIVE_BG,	MODEBACKI
	"#304040", // COLOR_BOX_BG,		CBOXTBACK ???????
	"#454545", // COLOR_CODE_BG		color code bg
	"#D3D3D3", // COLOR_FG=BG_COLORS,	FOREGROUND, INFOFORE, QUOTEFORE,CBOXTFORE
	"#E0E0E0", // COLOR_MENU_FG
	"#B4BF76", // COLOR_STANDOUT_FG,	SEARFORE, CNUMERIC, CBOXFORE
	"#F00000", // COLOR_CTRL_FG,		menu start fg, CTRLFORE
	"#F0F000", // COLOR_PREP_FG,		PREPFORE
	"#D06060", // COLOR_WORD1_FG,		WORD1FORE
	"#559BD5", // COLOR_WORD2_FG,		WORD2FORE
	"#B4BF76", // COLOR_WORD3_FG,		W_FORE
	"#FFFFFF", // COLOR_SPEC_FG,		SPECFORE
	"#699854", // COLOR_SQUOTE_FG,	SQUOTEFORE
	"#699854", // COLOR_COMMENT_FG,	COMMENTFORE
	"#CD9000", // COLOR_CHANGE_FG,	CHANGEFORE
	"#A000A0", // COLOR_HORIZON_FG,	ORIZON
	"#A0A0A0", // COLOR_INACTIVE_FG,	MODEFOREI
	"#FFFF00"  // COLOR_ROWCOL_FG		DROWCOL
 }
};
#else
char *basic_color_values[COLOR_SCHEMES][16] = {
 // Snow
 {
 	/* black */		"#000000",	// FOREGROUND, MODEFOREI
	/* red  */		"#AE0000",	// WORD1FORE, SEARBACK
	/* green */		"#A0F0A0",	// QUOTEBACK
	/* orange */ 	"#906000",	// COMMENTFORE
	/* blue */		"#0000AE",	// CNUMERIC, MENU_BG, CBOXTBACK,
	/* magenta */	"#F0D0A0",  // LBACKGROUND, INFOBACK
	/* cyan */		"#80B0D0",	// MODEBACK/selections
	/* white */		"#E3E3E3",	// BACKGROUND, SEARCHFORE, CBOXTFORE, MENUFG
	/* brown */		"#A0A0A0",	// QUOTEFORE, MODEBACKI, INFOFORE, TAGFORE
	/* lred */		"#FF0000",	// CHANGEDFORE, CTRLFORE
	/* lgreen */	"#006000",	// SQUOTEFORE, W_FORE
	/* yellow */	"#FFFF00",	// DROWCOL
	/* lblue */		"#B0B0C0",	// SPECFORE
	/* lmagenta */	"#800080",	// WORD2FORE, ORIZON
	/* lcyan */		"#00A080",	// PREPFORE
	/* lwhite */	"#FFFFFF"	// MODEFORE
 },
// Midnight
 {
 	/* black */		"#000000",	// background
	/* red  */		"#A00000",	// search background
	/* green */		"#008000",	// quote background, squote foreground
	/* orange */ 	"#F09000",	// comment foreground
	/* blue */		"#0070A0",	// menu background
	/* magenta */	"#303020",	// light, background
	/* cyan */		"#005070",	// select background
	/* white */		"#B0B0B0",	// foreground, info foreground,
	/* brown */		"#303040",	// info background, inactive bg
	/* lred */		"#D00000",	// word1 foreground
	/* lgreen */	"#98E098",	// special foreground
	/* yellow */	"#FFFF00",	// prep,row-column foreground
	/* lblue */		"#202025",	// lbackground
	/* lmagenta */	"#FF00FF",	// Horizon foreground, wor3 (html) foreground
	/* lcyan */		"#00D0D0",	// word2 foreground
	/* lwhite */	"#FFF0F0"	// stand out,numeric, search foreground
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
	/* brown */		"#303030",
	/* lred */		"#FF0000",
	/* lgreen */	"#00FF00",
	/* yellow */	"#FFFF00",
	/* lblue */		"#506070",
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
	/* lblue */		"#204060",
	/* lmagenta */	"#FF00FF",
	/* lcyan */		"#00FFFF",
	/* lwhite */	"#FFFFFF"
 },
// Gray
 {
 	/* black */		"#000000",
	/* red  */		"red4",
	/* green */		"#508050",
	/* orange */ 	"#E5D5A5",
	/* blue */		"#004090",
	/* magenta */	"#A8A8A0",
	/* cyan */		"#5080A0",
	/* white */		"#909090",
	/* brown */		"#707070",
	/* lred */		"#FF4D4D",
	/* lgreen */	"#00D0D0",
	/* yellow */	"#FFFF00",
	/* lblue */		"#858585",
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
	/* lblue */		"#404055",
	/* lmagenta */	"#FF00FF",
	/* lcyan */		"#99E6E6",
	/* lwhite */	"#FFFFFF"
 },
// Sunny
 {
 	/* black */		"#000000",
	/* red  */		"#AD0000",
	/* green */		"#00B000",
	/* orange */ 	"#A04040",
	/* blue */		"blue4",
	/* magenta */	"#C0C0B0",
	/* cyan */		"#B0C0D0",
	/* white */		"#E0E0F0",
	/* brown */		"#777777",
	/* lred */		"#FF3333",
	/* lgreen */	"#009900",
	/* yellow */	"#FFFF00",
	/* lblue */		"#B0B0C0",
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
	/* blue */		"#006688",
	/* magenta */	"#404040",
	/* cyan */		"#205060",
	/* white */		"#E0E0E0",
	/* brown */		"#707080",
	/* lred */		"#CD7C33",
	/* lgreen */	"#90EE90",
	/* yellow */	"#FFFF00",
	/* lblue */		"#202530",
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
	/* lblue */		"#404050",
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
	/* lcyan */		"#444445",
	/* lwhite */	"#FFFFFF"
 }
};
#endif

color_curses *current_color;

#if	!NEW_COLORS
color_curses color_t[COLOR_SCHEMES][XCOLOR_TYPES] = {
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
	{LBLUE,0},		/* LBACKGROUND */
	{LCYAN,0},		/* PREPFORE */
	{LRED,0},		/* CHANGEDFORE */
	{BROWN,0},		/* TAGFORE */
	{RED,0},		/* WORD1FORE */
	{BLUE,0},		/* SPECFORE */
	{LGREEN,0},		/* SQUOTEFORE */
	{LRED,0},		/* CTRLFORE */
	{LMAGENTA,0},	/* WORD2FORE */
	{LGREEN,0},		/* W_FORE */
	{LMAGENTA,0},	/* ORIZON */
	{BLUE,0},		/* CNUMERIC */
	{BLACK,0},		/* INFOFORE  */
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
	{LBLUE,0},	/* LBACKGROUND */
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
	{LBLUE,0},	/* LBACKGROUND */
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

 /* Blue Sky*/
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
	{LBLUE,0},	/* LBACKGROUND */
	{YELLOW,0},	/* PREPFORE */
	{ORANGE,0},	/* CHANGEDFORE */
	{LGREEN,0},	/* TAGFORE */
	{RED,0},	/* WORD1FORE */
	{LGREEN,0},	/* SPECFORE */
	{GREEN,0},	/* SQUOTEFORE */
	{LRED,0},	/* CTRLFORE */
	{LCYAN,0},	/* WORD2FORE */
	{LMAGENTA,0}, /* W_FORE */
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
 
 /* Gray */
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
	{LBLUE,0},	/* LBACKGROUND */
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
	{BLUE,0},	/* CNUMERIC */
	{BLACK,0},	/* INFOFORE  */
	{MAGENTA,0},/* INFOBACK  */
	{LWHITE,0},	/* CBOXTFORE */
	{CYAN,0},	/* CBOXTBACK */
	{LWHITE,0},
	{BLUE,0},
	{LWHITE,0},
	{BROWN,0}
 },

 /* Dark gray */
 {
	{WHITE,0},	/* Foreground FOREGR*/
	{BLACK,0},	/* Background BACKGR*/
	{LWHITE,0},	/* Modeline Foreground MODEFORE*/
	{CYAN,0},	/* Modeline BACKGOUND  MODEBACK*/
	{YELLOW,0},	/* Display row-column DROWCOL*/
	{LWHITE,0},	/* search foreground SEARFORE*/
	{RED,0},	/* search background SEARBACK*/
	{WHITE,0},	/* QUOTEFORE */
	{GREEN,0},	/* QUOTEBACK */
	{ORANGE,0},	/* COMMENTFORE */
	{LBLUE,0},	/* LBACKGROUND */
	{YELLOW,0},	/* PREPFORE */
	{ORANGE,0},  /* CHANGEDFORE */
	{ORANGE,0},	/* TAGFORE */
	{LRED,0},	/* WORD1FORE */
	{LGREEN,0},	/* SPECFORE */
	{LGREEN,0},	/* SQUOTEFORE */
	{RED,0},	/* CTRLFORE */
	{LCYAN,0},	/* WORD2FORE */
	{RED,0},	/* W_FORE */
	{LMAGENTA,0}, /* ORIZON */
	{LWHITE,0},	/* CNUMERIC */
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
	{LBLUE,0},	/* LBACKGROUND */
	{LRED,0},	/* PREPFORE */
	{ORANGE,0},	/* CHANGEDFORE */
	{ORANGE,0},	/* TAGFORE */
	{LRED,0},	/* WORD1FORE */
	{LGREEN,0},	/* SPECFORE */
	{LGREEN,0},	/* SQUOTEFORE */
	{LRED,0},	/* CTRLFORE */
	{LMAGENTA,0},	/* WORD2FORE */
	{GREEN,0},	/* W_FORE */
	{LMAGENTA,0}, /* ORIZON */
	{LCYAN,0},	/* CNUMERIC */
	{BLACK,0},	/* INFOFORE  */
	{MAGENTA,0},/* INFOBACK  */
	{LWHITE,0},	/* CBOXTFORE */
	{BROWN,0},	/* CBOXTBACK */
	{WHITE,0},
	{BLUE,0},
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
	{LBLUE,0},/* LBACKGROUND */
	{YELLOW,0},	/* PREPFORE */
	{ORANGE,0},	/* CHANGEDFORE */
	{ORANGE,0},	/* TAGFORE */
	{LRED,0},	/* WORD1FORE */
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
	{BLUE,0},
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
	{LBLUE,0},	/* LBACKGROUND */
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
	{LCYAN,0},	/* LBACKGROUND */
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
#else
int color_t[COLOR_SCHEMES][BG_COLORS+FG_COLORS] = {
 /* Snow */
 {
 WHITE,		/* COLOR_BG  */
 BLUE,		/* MENU_BG  */
 CYAN,		/* SELECT_BG  */
 RED,		/* SEARCH_BG  */
 GREEN,		/* QUOTE_BG  */
 BROWN,		/* LIGHT_BG  */
 WHITE,		/* INFO_BG  */
 BLUE,		/* INACTIVE_BG  */
 BLUE,		/* BOX_BG  */

 BROWN,		/* CODE_BG */
 BLACK,		/* FG  */
 WHITE,		/* MENU_FG */
 LWHITE,	/* STANDOUT_FG */
 LRED,		/* CTRL_FG */
 YELLOW,	/* PREP_FG  */
 LCYAN,		/* WORD1  */
 LMAGENTA,	/* WORD2  */
 LBLUE,		/* WORD3  */
 LGREEN,	/* SPEC  */
 GREEN,		/* SQUOTE_FG */
 ORANGE,	/* COMMENT_FG  */
 ORANGE,	/* CHANGED_FG  */
 LMAGENTA,	/* HORIZON_FG  */
 WHITE,		/* INACTIVE_FG  */
 YELLOW		/* ROWCOL_FG  */
},
 /* Midnight */
 {
 BLACK,		/* COLOR_BG  */
 BLUE,		/* MENU_BG  */
 CYAN,		/* SELECT_BG  */
 RED,		/* SEARCH_BG  */
 GREEN,		/* QUOTE_BG  */
 BROWN,		/* LIGHT_BG  */
 BLACK,		/* INFO_BG  */
 BLUE,		/* INACTIVE_BG  */
 BLUE,		/* BOX_BG  */

 BROWN,		/* CODE_BG */
 WHITE,		/* FG  */
 WHITE,		/* MENU_FG */
 LWHITE,	/* STANDOUT_FG */
 LRED,		/* CTRL_FG */
 YELLOW,	/* PREP_FG  */
 LCYAN,		/* WORD1  */
 LMAGENTA,	/* WORD2  */
 LBLUE,		/* WORD3  */
 LGREEN,	/* SPEC  */
 GREEN,		/* SQUOTE_FG */
 ORANGE,	/* COMMENT_FG  */
 ORANGE,	/* CHANGED_FG  */
 LMAGENTA,	/* HORIZON_FG  */
 WHITE,		/* INACTIVE_FG  */
 YELLOW		/* ROWCOL_FG  */
},
 /* Cyan */
 {
 CYAN,		/* COLOR_BG  */
 BLUE,		/* MENU_BG  */
 BLUE,		/* SELECT_BG  */
 RED,		/* SEARCH_BG  */
 GREEN,		/* QUOTE_BG  */
 BROWN,		/* LIGHT_BG  */
 BLACK,		/* INFO_BG  */
 BLUE,		/* INACTIVE_BG  */
 BLUE,		/* BOX_BG  */
 BROWN,		/* CODE_BG */

 WHITE,		/* FG  */
 WHITE,		/* MENU_FG */
 LWHITE,	/* STANDOUT_FG */
 LRED,		/* CTRL_FG */
 YELLOW,	/* PREP_FG  */
 LCYAN,		/* WORD1  */
 LMAGENTA,	/* WORD2  */
 LBLUE,		/* WORD3  */
 LGREEN,	/* SPEC  */
 GREEN,		/* SQUOTE_FG */
 ORANGE,	/* COMMENT_FG  */
 ORANGE,	/* CHANGED_FG  */
 LMAGENTA,	/* HORIZON_FG  */
 WHITE,		/* INACTIVE_FG  */
 YELLOW		/* ROWCOL_FG  */
 },
 /* Blue */
 {
 BLUE,		/* COLOR_BG  */
 CYAN,		/* MENU_BG  */
 CYAN,		/* SELECT_BG  */
 RED,		/* SEARCH_BG  */
 GREEN,		/* QUOTE_BG  */
 BROWN,		/* LIGHT_BG  */
 BLUE,		/* INFO_BG  */
 CYAN,		/* INACTIVE_BG  */
 CYAN,		/* BOX_BG  */

 BROWN,		/* CODE_BG */
 WHITE,		/* FG  */
 WHITE,		/* MENU_FG */
 LWHITE,	/* STANDOUT_FG */
 LRED,		/* CTRL_FG */
 YELLOW,	/* PREP_FG  */
 LCYAN,		/* WORD1  */
 LMAGENTA,	/* WORD2  */
 LBLUE,		/* WORD3  */
 LGREEN,	/* SPEC  */
 GREEN,		/* SQUOTE_FG */
 ORANGE,	/* COMMENT_FG  */
 ORANGE,	/* CHANGED_FG  */
 LMAGENTA,	/* HORIZON_FG  */
 WHITE,		/* INACTIVE_FG  */
 YELLOW		/* ROWCOL_FG  */
 },
 /* Gray */
 {
 WHITE,		/* COLOR_BG  */
 BLUE,		/* MENU_BG  */
 CYAN,		/* SELECT_BG  */
 RED,		/* SEARCH_BG  */
 GREEN,		/* QUOTE_BG  */
 BROWN,		/* LIGHT_BG  */
 WHITE,		/* INFO_BG  */
 BLUE,		/* INACTIVE_BG  */
 BLUE,		/* BOX_BG  */

 BROWN,		/* CODE_BG */
 BLACK,		/* FG  */
 WHITE,		/* MENU_FG */
 LWHITE,	/* STANDOUT_FG */
 LRED,		/* CTRL_FG */
 YELLOW,	/* PREP_FG  */
 LCYAN,		/* WORD1  */
 LMAGENTA,	/* WORD2  */
 LBLUE,		/* WORD3  */
 LGREEN,	/* SPEC  */
 GREEN,		/* SQUOTE_FG */
 ORANGE,	/* COMMENT_FG  */
 ORANGE,	/* CHANGED_FG  */
 LMAGENTA,	/* HORIZON_FG  */
 WHITE,		/* INACTIVE_FG  */
 YELLOW		/* ROWCOL_FG  */
 },
 /* Dark gray */
 {
 BLACK,		/* COLOR_BG  */
 BLUE,		/* MENU_BG  */
 CYAN,		/* SELECT_BG  */
 RED,		/* SEARCH_BG  */
 GREEN,		/* QUOTE_BG  */
 BROWN,		/* LIGHT_BG  */
 BLACK,		/* INFO_BG  */
 BLUE,		/* INACTIVE_BG  */
 BLUE,		/* BOX_BG  */

 BROWN,		/* CODE_BG */
 WHITE,		/* FG  */
 WHITE,		/* MENU_FG */
 LWHITE,	/* STANDOUT_FG */
 LRED,		/* CTRL_FG */
 YELLOW,	/* PREP_FG  */
 LCYAN,		/* WORD1  */
 LMAGENTA,	/* WORD2  */
 LBLUE,		/* WORD3  */
 LGREEN,	/* SPEC  */
 GREEN,		/* SQUOTE_FG */
 ORANGE,	/* COMMENT_FG  */
 ORANGE,	/* CHANGED_FG  */
 LMAGENTA,	/* HORIZON_FG  */
 WHITE,		/* INACTIVE_FG  */
 YELLOW		/* ROWCOL_FG  */
 },
 /* Sunny */
 {
 WHITE,		/* COLOR_BG  */
 BLUE,		/* MENU_BG  */
 CYAN,		/* SELECT_BG  */
 RED,		/* SEARCH_BG  */
 GREEN,		/* QUOTE_BG  */
 BROWN,		/* LIGHT_BG  */
 WHITE,		/* INFO_BG  */
 BLUE,		/* INACTIVE_BG  */
 BLUE,		/* BOX_BG  */

 BROWN,		/* CODE_BG */
 BLACK,		/* FG  */
 WHITE,		/* MENU_FG */
 LWHITE,	/* STANDOUT_FG */
 LRED,		/* CTRL_FG */
 YELLOW,	/* PREP_FG  */
 LCYAN,		/* WORD1  */
 LMAGENTA,	/* WORD2  */
 LBLUE,		/* WORD3  */
 LGREEN,	/* SPEC  */
 GREEN,		/* SQUOTE_FG */
 ORANGE,	/* COMMENT_FG  */
 ORANGE,	/* CHANGED_FG  */
 LMAGENTA,	/* HORIZON_FG  */
 WHITE,		/* INACTIVE_FG  */
 YELLOW		/* ROWCOL_FG  */
 },
 /* Twilight */
 {
 BLACK,		/* COLOR_BG  */
 BLUE,		/* MENU_BG  */
 CYAN,		/* SELECT_BG  */
 RED,		/* SEARCH_BG  */
 GREEN,		/* QUOTE_BG  */
 BROWN,		/* LIGHT_BG  */
 BLACK,		/* INFO_BG  */
 BLUE,		/* INACTIVE_BG  */
 BLUE,		/* BOX_BG  */

 BROWN,		/* CODE_BG */
 WHITE,		/* FG  */
 WHITE,		/* MENU_FG */
 LWHITE,	/* STANDOUT_FG */
 LRED,		/* CTRL_FG */
 YELLOW,	/* PREP_FG  */
 LCYAN,		/* WORD1  */
 LMAGENTA,	/* WORD2  */
 LBLUE,		/* WORD3  */
 LGREEN,	/* SPEC  */
 GREEN,		/* SQUOTE_FG */
 ORANGE,	/* COMMENT_FG  */
 ORANGE,	/* CHANGED_FG  */
 LMAGENTA,	/* HORIZON_FG  */
 WHITE,		/* INACTIVE_FG  */
 YELLOW		/* ROWCOL_FG  */
 },
 /* Lunar */
 {
 BLACK,		/* COLOR_BG  */
 BLUE,		/* MENU_BG  */
 CYAN,		/* SELECT_BG  */
 RED,		/* SEARCH_BG  */
 GREEN,		/* QUOTE_BG  */
 BROWN,		/* LIGHT_BG  */
 BLACK,		/* INFO_BG  */
 BLUE,		/* INACTIVE_BG  */
 BLUE,		/* BOX_BG  */

 BROWN,		/* CODE_BG */
 WHITE,		/* FG  */
 WHITE,		/* MENU_FG */
 LWHITE,	/* STANDOUT_FG */
 LRED,		/* CTRL_FG */
 YELLOW,	/* PREP_FG  */
 LCYAN,		/* WORD1  */
 LMAGENTA,	/* WORD2  */
 LBLUE,		/* WORD3  */
 LGREEN,	/* SPEC  */
 GREEN,		/* SQUOTE_FG */
 ORANGE,	/* COMMENT_FG  */
 ORANGE,	/* CHANGED_FG  */
 LMAGENTA,	/* HORIZON_FG  */
 WHITE,		/* INACTIVE_FG  */
 YELLOW		/* ROWCOL_FG  */
 },
 /* Velvet */
 {
 BLACK,		/* COLOR_BG  */
 BLUE,		/* MENU_BG  */
 CYAN,		/* SELECT_BG  */
 RED,		/* SEARCH_BG  */
 GREEN,		/* QUOTE_BG  */
 BROWN,		/* LIGHT_BG  */
 BLACK,		/* INFO_BG  */
 BLUE,		/* INACTIVE_BG  */
 BLUE,		/* BOX_BG  */

 BROWN,		/* CODE_BG */
 WHITE,		/* FG  */
 WHITE,		/* MENU_FG */
 LWHITE,	/* STANDOUT_FG */
 LRED,		/* CTRL_FG */
 YELLOW,	/* PREP_FG  */
 LCYAN,		/* WORD1  */
 LMAGENTA,	/* WORD2  */
 LBLUE,		/* WORD3  */
 LGREEN,	/* SPEC  */
 GREEN,		/* SQUOTE_FG */
 ORANGE,	/* COMMENT_FG  */
 ORANGE,	/* CHANGED_FG  */
 LMAGENTA,	/* HORIZON_FG  */
 WHITE,		/* INACTIVE_FG  */
 YELLOW		/* ROWCOL_FG  */
 }
};
#endif

typedef struct CPAIR {
	int fg_color;
	int bg_color;
	int attr;
} CPAIR;

#if	NEW_COLORS
#if	1
char *color_type[] = {
	"normal_bg",
	"menu_bg",
	"selection_bg",
	"search_bg",
	"quote_bg",
	"light_bg",
	"info_bg",
	"inactive_bg",
	"box_bg",
	"code_bg",
	"normal_fg",
	"menu_fg",
	"standout_fg",
	"control_fg",
	"prep_fg",
	"word1_FG",
	"word2_fg",
	"word3_fg",
	"special_fg",
	"single quote_fg",
	"comment_fg",
	"changed_fg",
	"horizon_fg",
	"inactive_fg",
	"rowcol_fg"
};
#endif
#else
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
#endif


/* --- */
