/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

*/
/* variables and definitions common in X */
#define	NEW_COLORS	1

#if	GTK

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
	{12,"Words1",COLOR_WORD1_FG,-COLOR_BG,NULL,NULL},
	{13,"Words2",COLOR_WORD2_FG,-COLOR_BG,NULL,NULL},
	{11,"Words3",COLOR_WORD3_FG,-COLOR_BG,NULL,NULL},
	{14,"Horizon", COLOR_HORIZON_FG,-COLOR_BG,NULL,NULL},
	{15,"Control chars",COLOR_CTRL_FG,-COLOR_BG,NULL,NULL},

	{16,"Active mode",COLOR_MENU_FG,COLOR_MENU_BG,NULL,NULL},
	{17,"Active changed",COLOR_CHANGE_FG,COLOR_MENU_BG,NULL,NULL},
	{18,"RowCol show",COLOR_ROWCOL_FG,-COLOR_MENU_BG,NULL,NULL},
	{19,"Passive mode",COLOR_INACTIVE_FG,COLOR_INACTIVE_BG,NULL,NULL},

	{20,"Box",-COLOR_FG,COLOR_BOX_BG,NULL,NULL},
	{21,"Code block",-COLOR_FG,COLOR_CODE_BG,NULL,NULL}
	// {11,"Tags",TAGFORE,-BACKGROUND,NULL,NULL},
	// {20,"ncurses menu",MENU_FG,MENU_BG,NULL,NULL},
};

int font_size = 16;
int status_font_size = 16;
#endif

#include "color.h"
