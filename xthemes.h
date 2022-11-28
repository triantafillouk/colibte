/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

*/
/* variables and definitions common in X */
#define	NEW_COLORS	1

#if	GTK
char *font_list[] = {
	"1. Courier 10",
	"2. Monospace",
	"3. DejaVu Sans Mono",
	"4. FreeMono",
	"5. Vera Sans Mono",
	"6. Menlo",
	"7. Luxi Mono",
	"0. Select font"
};

char *font_names[] = { 
			"Courier", 
			"Monospace",
			"DejaVu Sans Mono", 
			"FreeMono",
			"Vera Sans Mono", 
			"Menlo",
			"Luxi Mono",
			NULL
};

#define CTYPE_ROWS	20

// negative background value denotes that correspondiong color box 
// not displayed

color_element ctype[] = {
	{1,"Normal text",COLOR_FG,COLOR_BG,NULL,NULL},

	{2,"Search text",-COLOR_FG,COLOR_SEARCH_BG,NULL,NULL},
	{3,"Double quote text",-COLOR_FG,COLOR_QUOTE_BG,NULL,NULL},
	{4,"Info cols",-COLOR_FG,COLOR_INFO_BG,NULL,NULL},
	{5,"Selection",-COLOR_FG,COLOR_SELECT_BG,NULL,NULL},
	
	{6,"Preprocessing",COLOR_PREP_FG,-COLOR_BG,NULL,NULL},
	{7,"Standout",COLOR_STANDOUT_FG,-COLOR_BG,NULL,NULL},
	{8,"Special chars text",COLOR_SPEC_FG,-COLOR_BG,NULL,NULL},
	{9,"Single quote text",COLOR_SQUOTE_FG,-COLOR_BG,NULL,NULL},
	{10,"Comments",COLOR_COMMENT_FG,-COLOR_BG,NULL,NULL},
	{11,"Tags2 quote7",COLOR_WORD3_FG,-COLOR_BG,NULL,NULL},
	{12,"Words1",COLOR_WORD1_FG,-COLOR_BG,NULL,NULL},
	{13,"Words2",COLOR_WORD2_FG,-COLOR_BG,NULL,NULL},
	{14,"Horizon", COLOR_HORIZON_FG,-COLOR_BG,NULL,NULL},

	{15,"Active mode",COLOR_MENU_FG,COLOR_MENU_BG,NULL,NULL},
	{16,"RowCol show",COLOR_ROWCOL_FG,-COLOR_MENU_BG,NULL,NULL},
	{17,"Passive mode",COLOR_INACTIVE_FG,COLOR_INACTIVE_BG,NULL,NULL},

	{18,"Control chars",COLOR_CTRL_FG,-COLOR_BG,NULL,NULL},
	// {11,"Tags",TAGFORE,-BACKGROUND,NULL,NULL},
	{19,"Box",-COLOR_FG,COLOR_BOX_BG,NULL,NULL},
	{20,"Code block",-COLOR_FG,COLOR_CODE_BG,NULL,NULL}
	// {20,"ncurses menu",MENU_FG,MENU_BG,NULL,NULL},
};

#else
//char *font_names[] = { "fixed", "7x14","7x14bold", "9x16", "9x15","10x20","12x24",NULL };
char *font_list[] = {
	"1. Fixed 12",
	"2. Fixed 14",
	"3. Fixed 7x14",
	"4. Fixed 7x14 bold",
	"5. Courier 17",
	"6. Misc 15",
	"7. Misc 18",
	"8. Misc 20",
	"9. Courier 25",
	"Select font"
};
#endif

#if	GTK
int font_weights[] = { CAIRO_FONT_WEIGHT_NORMAL,CAIRO_FONT_WEIGHT_BOLD };
int font_slant[] =  { CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_SLANT_OBLIQUE };
#endif
int font_size = 16;
int status_font_size = 16;

#define	DL_FONT	5


#if NEW_COLORS
#include "color.h"
#else
#endif