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

// default color schemes
char *default_scheme_names[] = {
	"Snow","Midnight","Cyan","Blue","Gray","Dark gray",
	"Sunny","Twilight","Lunar","Velvet",
	NULL,NULL
};

#if	NUSE
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
#endif

#if !GTK
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
#endif

typedef struct color_curses {
	int index;	/* color index  */
	int attrib;	/* attibute  */
} color_curses;

typedef struct RGB_COLORS {
	int r,g,b;
} RGB_COLORS;

typedef struct COLOR_SCHEME {
	char *scheme_name;
	char *color_values[COLOR_TYPES];
	color_curses color_attr[COLOR_TYPES];
} COLOR_SCHEME;

#if	NEW_COLORS
char *basic_color_values[COLOR_SCHEMES][COLOR_TYPES] = {
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
#endif

color_curses *current_color;

#if	NEW_COLORS
/*
 These are correspondance table for 8 basic color terminals and no possibility to
 change their values
*/
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

#if	0
typedef struct CPAIR {
	int fg_color;
	int bg_color;
	int attr;
} CPAIR;
#endif

#if	NEW_COLORS
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
	"single_quote_fg",
	"comment_fg",
	"changed_fg",
	"horizon_fg",
	"inactive_fg",
	"rowcol_fg"
};
#else
#endif


/* --- */
