/*
	curses,gtk editor,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.
*/

/* curses, terminfo color scheme definitions */

#define COLOR_SCHEMES	10
#define CTYPE_ROWS		20

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

#define	BG_COLORS	10
enum color_types_bg {
	COLOR_BG,			// background
	COLOR_MENU_BG,		// menu backgrouns
	COLOR_SELECT_BG,	// modeline background
	COLOR_SEARCH_BG,	// search results background
	COLOR_QUOTE_BG,		// double quote background
	COLOR_LIGHT_BG,		// light background
	COLOR_INFO_BG,		// info column background
	COLOR_INACTIVE_BG,	// modeline inactive background
	COLOR_BOX_BG,		// box background
	COLOR_CODE_BG		// color code background
};

#define	FG_COLORS	15
enum color_types_fg {
	COLOR_FG=BG_COLORS,	// normal, info column, double quote foreground
	COLOR_MENU_FG,		// menu foreground
	COLOR_STANDOUT_FG,	// search, numeric, box foreground
	COLOR_CTRL_FG,		// menu start letters, control chars foreground
	COLOR_PREP_FG,		// preprocessing
	COLOR_WORD1_FG,		// WORD1FORE
	COLOR_WORD2_FG,		// WORD2FORE
	COLOR_WORD3_FG,		// word3 foreground
	COLOR_SPEC_FG,		// special letters foreground
	COLOR_SQUOTE_FG,	// single quote foreground
	COLOR_COMMENT_FG,	// comment foreground
	COLOR_CHANGE_FG,	// modeline changed foreground
	COLOR_HORIZON_FG,	// orizon letters foreground
	COLOR_INACTIVE_FG,	// modeline inactive foreground
	COLOR_ROWCOL_FG		// DROWCOL status foreground
};

#define COLOR_TYPES	BG_COLORS+FG_COLORS


typedef struct COLOR_ATTR {
	char *color_value;
	short color_index;	/* color index  */
	short color_attr;	/* attribute  */
} COLOR_ATTR;

typedef struct COLOR_SCHEME {
	char *scheme_name;
	COLOR_ATTR color_style[COLOR_TYPES];
} COLOR_SCHEME;


/* --- */
