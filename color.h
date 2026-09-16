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

#define	FG_COLORS	16
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
	COLOR_ROWCOL_FG,	// DROWCOL status foreground
	COLOR_INFO_FG
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

enum color_pairs {
	CPAIR_NORMAL,
	CPAIR_QUOTE,
	CPAIR_SQUOTE,
	CPAIR_LIGHT,
	CPAIR_STANDOUT,
	CPAIR_CTRL,
	CPAIR_PREP,
	CPAIR_WORD1,
	CPAIR_WORD2,
	CPAIR_WORD3,
	CPAIR_SPEC,
	CPAIR_HORIZON,

	CPAIR_NORMAL_SEL,
	CPAIR_QUOTE_SEL,
	CPAIR_SQUOTE_SEL,
	CPAIR_STANDOUT_SEL,
	CPAIR_CTRL_SEL,
	CPAIR_PREP_SEL,
	CPAIR_WORD1_SEL,
	CPAIR_WORD2_SEL,
	CPAIR_WORD3_SEL,
	CPAIR_SPEC_SEL,
	CPAIR_HORIZON_SEL,

	CPAIR_NORMAL_CODE,
	CPAIR_QUOTE_CODE,
	CPAIR_SQUOTE_CODE,
	CPAIR_STANDOUT_CODE,
	CPAIR_CTRL_CODE,
	CPAIR_PREP_CODE,
	CPAIR_WORD1_CODE,
	CPAIR_WORD2_CODE,
	CPAIR_WORD3_CODE,
	CPAIR_SPEC_CODE,
	CPAIR_HORIZON_CODE,

	CPAIR_NORMAL_LIGHT,
	CPAIR_QUOTE_LIGHT,
	CPAIR_SQUOTE_LIGHT,
	CPAIR_STANDOUT_LIGHT,
	CPAIR_CTRL_LIGHT,
	CPAIR_PREP_LIGHT,
	CPAIR_WORD1_LIGHT,
	CPAIR_WORD2_LIGHT,
	CPAIR_WORD3_LIGHT,
	CPAIR_SPEC_LIGHT,
	CPAIR_HORIZON_LIGHT,

	CPAIR_SEARCH,
	CPAIR_INFO,
	CPAIR_INFO_NOTE,

	CPAIR_INACTIVE,
	CPAIR_INACTIVE_NOTE,
	
	CPAIR_MENU,
	CPAIR_MENU_SELECTED,
	CPAIR_MENU_STANDOUT,
	CPAIR_MENU_STANDOUT_SEL,
	CPAIR_CHANGED,
	CPAIR_CHANGED_INACTIVE,
	CPAIR_NOTE,

	CPAIR_ROWCOL,
	CPAIR_ERROR
};

/* --- */
