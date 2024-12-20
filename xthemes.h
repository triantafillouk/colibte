/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

*/
/* variables and definitions common in X */

#include "color.h"

#if	GTK

// negative background value denotes that correspondiong color box 
// not displayed

color_element ctype[] = {
	{1,"Normal text",COLOR_FG,COLOR_BG,NULL,NULL},

	{2,"Search text",-COLOR_FG,COLOR_SEARCH_BG,NULL,NULL},
	{3,"Double quote",-COLOR_FG,COLOR_QUOTE_BG,NULL,NULL},
	{4,"Info cols",-COLOR_FG,COLOR_INFO_BG,NULL,NULL},
	{5,"Selection",-COLOR_FG,COLOR_SELECT_BG,NULL,NULL},
	
	{6,"Preprocessing",COLOR_PREP_FG,-COLOR_BG,NULL,NULL},
	{7,"Standout",COLOR_STANDOUT_FG,-COLOR_BG,NULL,NULL},
	{8,"Special chars",COLOR_SPEC_FG,-COLOR_BG,NULL,NULL},
	{9,"Single quotes",COLOR_SQUOTE_FG,-COLOR_BG,NULL,NULL},
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

int font_size = 14;
int status_font_size = 14;
#endif


COLOR_ATTR basic_color_values[COLOR_SCHEMES][COLOR_TYPES] = {
 // Snow
 {
	{"#F0F8FF",WHITE   ,0}, // COLOR_BG,			BACKGROUND
	{"#0000AE",BLUE    ,0}, // COLOR_MENU_BG,		MENU_BG
	{"#80B0D0",CYAN    ,0}, // COLOR_SELECT_BG,	MODEBACK
	{"#AE0000",RED     ,0}, // COLOR_SEARCH_BG,	SEARBACK
	{"#A0F0A0",GREEN   ,0}, // COLOR_QUOTE_BG,		QUOTEBACK
	{"#B0B0C0",BROWN   ,0}, // COLOR_LIGHT_BG,		LBACKGROUND
	{"#D0D0D0",WHITE   ,0}, // COLOR_INFO_BG,		INFOBACK
	{"#A0A0A0",BLUE    ,0}, // COLOR_INACTIVE_BG,	MODEBACKI
	{"#808080",BLUE    ,0}, // COLOR_BOX_BG,		CBOXTBACK
	{"#D8D8D8",WHITE   ,0}, // COLOR_CODE_BG		color code bg
	{"#000000",BLACK   ,0}, // COLOR_FG=BG_COLORS,	FOREGROUND, INFOFORE, QUOTEFORE,CBOXTFORE
	{"#FFFFFF",WHITE   ,0}, // COLOR_MENU_FG
	{"#0000AE",BLUE    ,0}, // COLOR_STANDOUT_FG,	SEARFORE, CNUMERIC, MENUSTART, MENU_FG, CBOXFORE
	{"#FF0000",LRED    ,0}, // COLOR_CTRL_FG,		menu start fg, CTRLFORE
	{"#800080",MAGENTA  ,FONT_STYLE_BOLD}, // COLOR_PREP_FG,		PREPFORE
	{"#FE0000",LCYAN   ,0}, // COLOR_WORD1_FG,		WORD1FORE
	{"#005000",GREEN,	0}, // COLOR_WORD2_FG,		WORD2FORE
	{"#98E098",LBLUE   ,0}, // COLOR_WORD3_FG,		W_FORE
	{"#0000AE",LGREEN  ,0}, // COLOR_SPEC_FG,		SPECFORE
	{"#006000",GREEN   ,0}, // COLOR_SQUOTE_FG,	SQUOTEFORE
	{"#A02000",RED	  ,FONT_STYLE_ITALIC}, // COLOR_COMMENT_FG,	COMMENTFORE
	{"#FF0000",ORANGE  ,0}, // COLOR_CHANGE_FG,	CHANGEFORE
	{"#800080",LMAGENTA,0}, // COLOR_HORIZON_FG,	ORIZON
	{"#E3E3E3",WHITE   ,0}, // COLOR_INACTIVE_FG,	MODEFOREI
	{"#FFFF00",YELLOW  ,0},  // COLOR_ROWCOL_FG		DROWCOL
	{"#202020",BLACK   ,0}	// COLOR_INFO_FG
 },
// Midnight
 {
	{"#000000",BLACK   ,0}, // COLOR_BG,			BACKGROUND
	{"#003060",BLUE    ,0}, // COLOR_MENU_BG,		MENU_BG
	{"#004080",CYAN    ,0}, // COLOR_SELECT_BG,	MODEBACK
	{"#A00000",RED     ,0}, // COLOR_SEARCH_BG,	SEARBACK
	{"#005000",GREEN   ,0}, // COLOR_QUOTE_BG,		QUOTEBACK
	{"#202025",BROWN   ,0}, // COLOR_LIGHT_BG,		LBACKGROUND
	{"#252525",BROWN   ,0}, // COLOR_INFO_BG,		INFOBACK
	{"#304050",BLUE    ,0}, // COLOR_INACTIVE_BG,	MODEBACKI
	{"#303030",BLUE    ,0}, // COLOR_BOX_BG,		CBOXTBACK ???????
	{"#202020",BLACK   ,0}, // COLOR_CODE_BG		color code bg
	{"#B0B0B0",WHITE   ,0}, // COLOR_FG=BG_COLORS,	FOREGROUND, INFOFORE, QUOTEFORE,CBOXTFORE
	{"#E0E0E0",WHITE   ,0}, // COLOR_MENU_FG
	{"#FFF0F0",LWHITE  ,0}, // COLOR_STANDOUT_FG,	SEARFORE, CNUMERIC, CBOXFORE
	{"#F00000",LRED    ,0}, // COLOR_CTRL_FG,		menu start fg, CTRLFORE
	{"#F0F000",YELLOW  ,FONT_STYLE_BOLD}, // COLOR_PREP_FG,		PREPFORE
	{"#D00000",LCYAN   ,0}, // COLOR_WORD1_FG,		WORD1FORE
	{"#00D0D0",LMAGENTA,0}, // COLOR_WORD2_FG,		WORD2FORE
	{"#D0D0D0",LBLUE   ,0}, // COLOR_WORD3_FG,		W_FORE
	{"#98E098",LGREEN  ,0}, // COLOR_SPEC_FG,		SPECFORE
	{"#208020",GREEN   ,0}, // COLOR_SQUOTE_FG,	SQUOTEFORE
	{"#F09000",ORANGE  ,FONT_STYLE_ITALIC}, // COLOR_COMMENT_FG,	COMMENTFORE
	{"#F09090",ORANGE  ,0}, // COLOR_CHANGE_FG,	CHANGEFORE
	{"#A000A0",LMAGENTA,0}, // COLOR_HORIZON_FG,	ORIZON
	{"#808080",WHITE   ,0}, // COLOR_INACTIVE_FG,	MODEFOREI
	{"#FFFF00",YELLOW  ,FONT_STYLE_BOLD},  // COLOR_ROWCOL_FG		DROWCOL
	{"#808080",WHITE,   0}	// COLOR_INFO_FG
 },
// Cyan
 {
	{"#004050",CYAN    ,0}, // COLOR_BG,			BACKGROUND
	{"#005080",BLUE    ,0}, // COLOR_MENU_BG,		MENU_BG
	{"#4060A0",BLUE    ,0}, // COLOR_SELECT_BG,	MODEBACK
	{"#D00000",RED     ,0}, // COLOR_SEARCH_BG,	SEARBACK
	{"#008000",GREEN   ,0}, // COLOR_QUOTE_BG,		QUOTEBACK
	{"#506070",BROWN   ,0}, // COLOR_LIGHT_BG,		LBACKGROUND
	{"#105060",BLUE   ,0}, // COLOR_INFO_BG,		INFOBACK
	{"#303030",BLUE    ,0}, // COLOR_INACTIVE_BG,	MODEBACKI
	{"#0050A0",BLUE    ,0}, // COLOR_BOX_BG,		CBOXTBACK ???????
	{"#404040",CYAN	   ,0}, // COLOR_CODE_BG		color code bg
	{"#D0D0D0",WHITE   ,0}, // COLOR_FG=BG_COLORS,	FOREGROUND, INFOFORE, QUOTEFORE,CBOXTFORE
	{"#FFFFFF",WHITE   ,0}, // COLOR_MENU_FG
	{"#FFFFFF",LWHITE  ,FONT_STYLE_BOLD}, // COLOR_STANDOUT_FG,	SEARFORE, CNUMERIC, CBOXFORE
	{"#F00000",LRED    ,0}, // COLOR_CTRL_FG,		menu start fg, CTRLFORE
	{"#FFFF00",YELLOW  ,0}, // COLOR_PREP_FG,		PREPFORE
	{"#D00000",LCYAN   ,0}, // COLOR_WORD1_FG,		WORD1FORE
	{"#00D0D0",LMAGENTA,0}, // COLOR_WORD2_FG,		WORD2FORE
	{"#FF00FF",LBLUE   ,0}, // COLOR_WORD3_FG,		W_FORE
	{"#00FF00",LGREEN  ,0}, // COLOR_SPEC_FG,		SPECFORE
	{"#F08020",GREEN   ,0}, // COLOR_SQUOTE_FG,	SQUOTEFORE
	{"#F09000",ORANGE  ,FONT_STYLE_ITALIC}, // COLOR_COMMENT_FG,	COMMENTFORE
	{"#FF0000",ORANGE  ,0}, // COLOR_CHANGE_FG,	CHANGEFORE
	{"#FF00FF",LMAGENTA,0}, // COLOR_HORIZON_FG,	ORIZON
	{"#808080",WHITE   ,0}, // COLOR_INACTIVE_FG,	MODEFOREI
	{"#FFFF00",YELLOW  ,FONT_STYLE_BOLD},  // COLOR_ROWCOL_FG		DROWCOL
	{"#A0A0A0",WHITE,	0}	// COLOR_INFO_FG
 },
// Blue Sky
 {
	{"#002060",BLUE    ,0}, // COLOR_BG,			BACKGROUND
	{"#008080",CYAN    ,0}, // COLOR_MENU_BG,		MENU_BG
	{"#005070",CYAN    ,0}, // COLOR_SELECT_BG,	MODEBACK
	{"#D00000",RED     ,0}, // COLOR_SEARCH_BG,	SEARBACK
	{"#008000",GREEN   ,0}, // COLOR_QUOTE_BG,		QUOTEBACK
	{"#204060",CYAN    ,0}, // COLOR_LIGHT_BG,		LBACKGROUND
	{"#204060",BLUE    ,0}, // COLOR_INFO_BG,		INFOBACK
	{"#004040",CYAN    ,0}, // COLOR_INACTIVE_BG,	MODEBACKI
	{"#008080",CYAN    ,0}, // COLOR_BOX_BG,		CBOXTBACK ???????
	{"#004050",BLUE    ,0}, // COLOR_CODE_BG		color code bg
	{"#D0D0D0",WHITE   ,0}, // COLOR_FG=BG_COLORS,	FOREGROUND, INFOFORE, QUOTEFORE,CBOXTFORE
	{"#FFFFFF",WHITE   ,0}, // COLOR_MENU_FG
	{"#FFF0F0",LWHITE  ,0}, // COLOR_STANDOUT_FG,	SEARFORE, CNUMERIC, CBOXFORE
	{"#FF0000",LRED    ,0}, // COLOR_CTRL_FG,		menu start fg, CTRLFORE
	{"#FFFF00",YELLOW  ,FONT_STYLE_BOLD}, // COLOR_PREP_FG,		PREPFORE
	{"#D00000",CYAN    ,FONT_STYLE_BOLD}, // COLOR_WORD1_FG,		WORD1FORE
	{"#00FFFF",MAGENTA ,FONT_STYLE_BOLD}, // COLOR_WORD2_FG,		WORD2FORE
	{"#F000F0",ORANGE  ,0}, // COLOR_WORD3_FG,		W_FORE
	{"#00FF00",LGREEN  ,0}, // COLOR_SPEC_FG,		SPECFORE
	{"#008000",GREEN   ,0}, // COLOR_SQUOTE_FG,	SQUOTEFORE
	{"#F09000",ORANGE  ,0}, // COLOR_COMMENT_FG,	COMMENTFORE
	{"#F09000",ORANGE  ,0}, // COLOR_CHANGE_FG,	CHANGEFORE
	{"#FF00FF",LMAGENTA,0}, // COLOR_HORIZON_FG,	ORIZON
	{"#808080",WHITE   ,0}, // COLOR_INACTIVE_FG,	MODEFOREI
	{"#FFFF00",YELLOW  ,FONT_STYLE_BOLD},  // COLOR_ROWCOL_FG		DROWCOL
	{"#909090",WHITE ,	0}	// COLOR_INFO_FG
 },
// Gray
 {
	{"#909090",WHITE   ,0}, // COLOR_BG,			BACKGROUND
	{"#003060",BLUE    ,0}, // COLOR_MENU_BG,		MENU_BG
	{"#5080A0",CYAN    ,0}, // COLOR_SELECT_BG,	MODEBACK
	{"red4"   ,RED     ,0}, // COLOR_SEARCH_BG,	SEARBACK
	{"#508050",GREEN   ,0}, // COLOR_QUOTE_BG,		QUOTEBACK
	{"#858585",BROWN   ,0}, // COLOR_LIGHT_BG,		LBACKGROUND
	{"#A8A8A0",WHITE   ,0}, // COLOR_INFO_BG,		INFOBACK
	{"#707070",BLUE    ,0}, // COLOR_INACTIVE_BG,	MODEBACKI
	{"#506080",BLUE    ,0}, // COLOR_BOX_BG,		CBOXTBACK
	{"#D0D0D0",WHITE   ,0}, // COLOR_CODE_BG		color code bg
	{"#000000",BLACK   ,0}, // COLOR_FG=BG_COLORS,FOREGROUND,INFOFORE,QUOTEFORE,CBOXTFORE
	{"#F2F2F2",WHITE   ,0}, // COLOR_MENU_FG
	{"#004090",WHITE   ,FONT_STYLE_REVERSE}, // COLOR_STANDOUT_FG,	SEARFORE, CNUMERIC, MENUSTART, MENU_FG, CBOXFORE
	{"#FF4D4D",RED     ,FONT_STYLE_BOLD}, // COLOR_CTRL_FG,		menu start fg, CTRLFORE
	{"#FFFF00",YELLOW  ,FONT_STYLE_BOLD}, // COLOR_PREP_FG,		PREPFORE
	{"#8C008C",CYAN    ,FONT_STYLE_BOLD}, // COLOR_WORD1_FG,		WORD1FORE
	{"red4"   ,MAGENTA ,FONT_STYLE_BOLD}, // COLOR_WORD2_FG,		WORD2FORE
	{"#00FFFF",BLUE    ,FONT_STYLE_BOLD}, // COLOR_WORD3_FG,		W_FORE
	{"#00D0D0",GREEN  ,0}, // COLOR_SPEC_FG,		SPECFORE
	{"#508050",GREEN   ,0}, // COLOR_SQUOTE_FG,	SQUOTEFORE
	{"#F5E5B5",RED	  ,0}, // COLOR_COMMENT_FG,	COMMENTFORE
	{"#E5D5A5",ORANGE  ,0}, // COLOR_CHANGE_FG,	CHANGEFORE
	{"#8C008C",LMAGENTA,0}, // COLOR_HORIZON_FG,	ORIZON
	{"#F2F2F2",WHITE   ,0}, // COLOR_INACTIVE_FG,	MODEFOREI
	{"#FFFF00",YELLOW  ,FONT_STYLE_BOLD},  // COLOR_ROWCOL_FG		DROWCOL
	{"#202020",BLACK,	0}	// COLOR_INFO_FG
 },
// Dark Gray
 {
	{"#303030",BLACK   ,0}, // COLOR_BG,			BACKGROUND
	{"#336194",BLUE    ,0}, // COLOR_MENU_BG,		MENU_BG
	{"#005070",CYAN    ,0}, // COLOR_SELECT_BG,	MODEBACK
	{"#FA0000",RED     ,0}, // COLOR_SEARCH_BG,	SEARBACK
	{"#007A00",GREEN   ,0}, // COLOR_QUOTE_BG,		QUOTEBACK
	{"#404055",BROWN   ,0}, // COLOR_LIGHT_BG,		LBACKGROUND
	{"#353535",BLACK   ,0}, // COLOR_INFO_BG,		INFOBACK
	{"#434343",BLUE    ,0}, // COLOR_INACTIVE_BG,	MODEBACKI
	{"#336194",BLUE    ,0}, // COLOR_BOX_BG,		CBOXTBACK ???????
	{"#404040",BROWN   ,0}, // COLOR_CODE_BG		color code bg
	{"#F2F2D9",WHITE   ,0}, // COLOR_FG=BG_COLORS,	FOREGROUND, INFOFORE, QUOTEFORE,CBOXTFORE
	{"#E0E0E0",WHITE   ,0}, // COLOR_MENU_FG
	{"#FFFFFF",LWHITE  ,0}, // COLOR_STANDOUT_FG,	SEARFORE, CNUMERIC, CBOXFORE
	{"#FA0000",LRED    ,0}, // COLOR_CTRL_FG,		menu start fg, CTRLFORE
	{"#FFFF00",YELLOW  ,0}, // COLOR_PREP_FG,		PREPFORE
	{"#FF9999",LCYAN   ,0}, // COLOR_WORD1_FG,		WORD1FORE
	{"#99E6E6",LMAGENTA,0}, // COLOR_WORD2_FG,		WORD2FORE
	{"#FA0000",LBLUE   ,0}, // COLOR_WORD3_FG,		W_FORE
	{"#66FF66",LGREEN  ,0}, // COLOR_SPEC_FG,		SPECFORE
	{"#66FF66",GREEN   ,0}, // COLOR_SQUOTE_FG,	SQUOTEFORE
	{"#F09000",RED     ,0}, // COLOR_COMMENT_FG,	COMMENTFORE
	{"#F09000",ORANGE  ,0}, // COLOR_CHANGE_FG,	CHANGEFORE
	{"#FF00FF",LMAGENTA,0}, // COLOR_HORIZON_FG,	ORIZON
	{"#909090",WHITE   ,0}, // COLOR_INACTIVE_FG,	MODEFOREI
	{"#FFFF00",YELLOW  ,FONT_STYLE_BOLD},  // COLOR_ROWCOL_FG		DROWCOL
	{"#202020",WHITE,	0}	// COLOR_INFO_FG
 },
// Sunny
 {
	{"#E0E0F0",WHITE   ,0}, // COLOR_BG,			BACKGROUND
	{"#2020A0",BLUE    ,0}, // COLOR_MENU_BG,		MENU_BG
	{"#B0C0D0",CYAN    ,0}, // COLOR_SELECT_BG,	MODEBACK
	{"#AD0000",RED     ,0}, // COLOR_SEARCH_BG,	SEARBACK
	{"#00B000",GREEN   ,0}, // COLOR_QUOTE_BG,		QUOTEBACK
	{"#B0B0C0",CYAN    ,0}, // COLOR_LIGHT_BG,		LBACKGROUND
	{"#D0D0E0",WHITE   ,0}, // COLOR_INFO_BG,		INFOBACK
	{"#777777",BLUE    ,0}, // COLOR_INACTIVE_BG,	MODEBACKI
	{"#808080",BLUE    ,0}, // COLOR_BOX_BG,		CBOXTBACK
	{"#A8A8A8",WHITE   ,0}, // COLOR_CODE_BG		color code bg
	{"#000000",BLACK   ,0}, // COLOR_FG=BG_COLORS,	FOREGROUND, INFOFORE, QUOTEFORE,CBOXTFORE
	{"#FFFFFF",WHITE   ,0}, // COLOR_MENU_FG
	{"#1C99CC",LWHITE  ,0}, // COLOR_STANDOUT_FG,	SEARFORE, CNUMERIC, MENUSTART, MENU_FG, CBOXFORE
	{"#FF3333",LRED    ,0}, // COLOR_CTRL_FG,		menu start fg, CTRLFORE
	{"#FF3333",YELLOW  ,0}, // COLOR_PREP_FG,		PREPFORE
	{"#AE0000",LCYAN   ,FONT_STYLE_BOLD}, // COLOR_WORD1_FG,		WORD1FORE
	{"#FF00FF",LMAGENTA,FONT_STYLE_BOLD}, // COLOR_WORD2_FG,		WORD2FORE
	{"#A04040",LBLUE   ,FONT_STYLE_BOLD}, // COLOR_WORD3_FG,		W_FORE
	{"#009900",LGREEN  ,FONT_STYLE_BOLD}, // COLOR_SPEC_FG,		SPECFORE
	{"#00A900",GREEN   ,0}, // COLOR_SQUOTE_FG,	SQUOTEFORE
	{"#A04040",RED     ,0}, // COLOR_COMMENT_FG,	COMMENTFORE
	{"#A04040",ORANGE  ,0}, // COLOR_CHANGE_FG,	CHANGEFORE
	{"#FF00FF",LMAGENTA,0}, // COLOR_HORIZON_FG,	ORIZON
	{"#E3E3E3",WHITE   ,0}, // COLOR_INACTIVE_FG,	MODEFOREI
	{"#FFFF00",YELLOW  ,FONT_STYLE_BOLD},  // COLOR_ROWCOL_FG		DROWCOL
	{"#202020",BLACK,	0} // COLOR_INFO_FG
 },
// Twilight
 {
	{"#141414",BLACK   ,0}, // COLOR_BG,			BACKGROUND
	{"#005577",BLUE    ,0}, // COLOR_MENU_BG,		MENU_BG
	{"#007090",CYAN    ,0}, // COLOR_SELECT_BG,	MODEBACK
	{"#804040",RED     ,0}, // COLOR_SEARCH_BG,	SEARBACK
	{"#407040",GREEN   ,0}, // COLOR_QUOTE_BG,		QUOTEBACK
	{"#202530",BROWN   ,0}, // COLOR_LIGHT_BG,		LBACKGROUND
	{"#353535",BLACK   ,0}, // COLOR_INFO_BG,		INFOBACK
	{"#707080",BLUE    ,0}, // COLOR_INACTIVE_BG,	MODEBACKI
	{"#205060",BLUE    ,0}, // COLOR_BOX_BG,		CBOXTBACK ???????
	{"#282828",BLACK   ,0}, // COLOR_CODE_BG		color code bg
	{"#E0E0E0",WHITE   ,0}, // COLOR_FG=BG_COLORS,	FOREGROUND, INFOFORE, QUOTEFORE,CBOXTFORE
	{"#FFFFFF",WHITE   ,0}, // COLOR_MENU_FG
	{"#F0AD80",LWHITE  ,0}, // COLOR_STANDOUT_FG,	SEARFORE, CNUMERIC, CBOXFORE
	{"#CD7C33",LRED    ,0}, // COLOR_CTRL_FG,		menu start fg, CTRLFORE
	{"#FFFF00",YELLOW  ,FONT_STYLE_BOLD}, // COLOR_PREP_FG,		PREPFORE
	{"#CD7C33",CYAN    ,0}, // COLOR_WORD1_FG,		WORD1FORE
	{"#F9EE98",MAGENTA ,0}, // COLOR_WORD2_FG,		WORD2FORE
	{"#F0AD80",BLUE   ,0}, // COLOR_WORD3_FG,		W_FORE
	{"#90EE90",GREEN  ,FONT_STYLE_BOLD}, // COLOR_SPEC_FG,		SPECFORE
	{"#90EE90",GREEN   ,0}, // COLOR_SQUOTE_FG,	SQUOTEFORE
	{"#707080",ORANGE  ,FONT_STYLE_ITALIC}, // COLOR_COMMENT_FG,	COMMENTFORE
	{"#F0AD80",ORANGE  ,0}, // COLOR_CHANGE_FG,	CHANGEFORE
	{"#FF00FF",LMAGENTA,0}, // COLOR_HORIZON_FG,	ORIZON
	{"#E0E0E0",WHITE   ,0}, // COLOR_INACTIVE_FG,	MODEFOREI
	{"#FFFF00",YELLOW  ,FONT_STYLE_BOLD},  // COLOR_ROWCOL_FG		DROWCOL
	{"#A0A0A0",WHITE,	0}	// COLOR_INFO_FG
 },
// Lunar
 {
	{"#202020",BLACK   ,0}, // COLOR_BG,			BACKGROUND
	{"#0070A0",BLUE    ,0}, // COLOR_MENU_BG,		MENU_BG
	{"#005070",CYAN    ,0}, // COLOR_SELECT_BG,	MODEBACK
	{"#D06060",RED     ,0}, // COLOR_SEARCH_BG,	SEARBACK
	{"#008000",GREEN   ,0}, // COLOR_QUOTE_BG,		QUOTEBACK
	{"#202025",BROWN   ,0}, // COLOR_LIGHT_BG,		LBACKGROUND
	{"#323232",BLACK   ,0}, // COLOR_INFO_BG,		INFOBACK
	{"#4D4D4D",BLUE    ,0}, // COLOR_INACTIVE_BG,	MODEBACKI
	{"#506070",BLUE    ,0}, // COLOR_BOX_BG,		CBOXTBACK ???????
	{"#404040",BROWN   ,0}, // COLOR_CODE_BG		color code bg
	{"#E8D8D8",WHITE   ,0}, // COLOR_FG=BG_COLORS,	FOREGROUND, INFOFORE, QUOTEFORE,CBOXTFORE
	{"#E0E0E0",WHITE   ,0}, // COLOR_MENU_FG
	{"#FFF0F0",LWHITE  ,0}, // COLOR_STANDOUT_FG,	SEARFORE, CNUMERIC, CBOXFORE
	{"#FF4D4D",LRED    ,0}, // COLOR_CTRL_FG,		menu start fg, CTRLFORE
	{"#FF90FF",YELLOW  ,FONT_STYLE_BOLD}, // COLOR_PREP_FG,		PREPFORE
	{"#D06060",LCYAN   ,0}, // COLOR_WORD1_FG,		WORD1FORE
	{"#00D0D0",LMAGENTA,0}, // COLOR_WORD2_FG,		WORD2FORE
	{"#D0D0D0",LBLUE   ,0}, // COLOR_WORD3_FG,		W_FORE
	{"#A0F0A0",LGREEN  ,0}, // COLOR_SPEC_FG,		SPECFORE
	{"#60B060",GREEN   ,0}, // COLOR_SQUOTE_FG,	SQUOTEFORE
	{"#60B060",ORANGE  ,0}, // COLOR_COMMENT_FG,	COMMENTFORE
	{"#F09090",ORANGE  ,0}, // COLOR_CHANGE_FG,	CHANGEFORE
	{"#A000A0",LMAGENTA,0}, // COLOR_HORIZON_FG,	ORIZON
	{"#808080",WHITE   ,0}, // COLOR_INACTIVE_FG,	MODEFOREI
	{"#FFFF00",YELLOW  ,FONT_STYLE_BOLD},  // COLOR_ROWCOL_FG		DROWCOL
	{"#A0A0A0",WHITE,	0}	// COLOR_INFO_FG
 },
// Velvet
 {
	{"#1C1C1C",BLACK   ,0}, // COLOR_BG,			BACKGROUND
	{"#0060A0",BLUE    ,0}, // COLOR_MENU_BG,		MENU_BG
	{"#005070",CYAN    ,0}, // COLOR_SELECT_BG,	MODEBACK
	{"#A000A0",RED     ,0}, // COLOR_SEARCH_BG,	SEARBACK
	{"#008000",GREEN   ,0}, // COLOR_QUOTE_BG,		QUOTEBACK
	{"#202025",BROWN   ,0}, // COLOR_LIGHT_BG,		LBACKGROUND
	{"#303030",BLACK   ,0}, // COLOR_INFO_BG,		INFOBACK
	{"#404040",BLUE    ,0}, // COLOR_INACTIVE_BG,	MODEBACKI
	{"#304040",BLUE    ,0}, // COLOR_BOX_BG,		CBOXTBACK ???????
	{"#454545",BROWN   ,FONT_STYLE_DIM}, // COLOR_CODE_BG		color code bg
	{"#D3D3D3",WHITE   ,0}, // COLOR_FG=BG_COLORS,	FOREGROUND, INFOFORE, QUOTEFORE,CBOXTFORE
	{"#E0E0E0",WHITE   ,0}, // COLOR_MENU_FG
	{"#B4BF76",LWHITE  ,FONT_STYLE_BOLD}, // COLOR_STANDOUT_FG,	SEARFORE, CNUMERIC, CBOXFORE
	{"#F00000",LRED    ,0}, // COLOR_CTRL_FG,		menu start fg, CTRLFORE
	{"#F0F000",YELLOW  ,FONT_STYLE_BOLD}, // COLOR_PREP_FG,		PREPFORE
	{"#D06060",LCYAN   ,0}, // COLOR_WORD1_FG,		WORD1FORE
	{"#559BD5",MAGENTA ,0}, // COLOR_WORD2_FG,		WORD2FORE
	{"#B4BF76",LBLUE   ,FONT_STYLE_ITALIC}, // COLOR_WORD3_FG,		W_FORE
	{"#FFFFFF",LGREEN  ,FONT_STYLE_BOLD}, // COLOR_SPEC_FG,		SPECFORE
	{"#699854",GREEN   ,0}, // COLOR_SQUOTE_FG,	SQUOTEFORE
	{"#F99854",ORANGE  ,FONT_STYLE_ITALIC}, // COLOR_COMMENT_FG,	COMMENTFORE
	{"#CD9000",ORANGE  ,0}, // COLOR_CHANGE_FG,	CHANGEFORE
	{"#A000A0",LMAGENTA,0}, // COLOR_HORIZON_FG,	ORIZON
	{"#A0A0A0",WHITE   ,FONT_STYLE_DIM}, // COLOR_INACTIVE_FG,	MODEFOREI
	{"#FFFF00",YELLOW  ,FONT_STYLE_BOLD},  // COLOR_ROWCOL_FG		DROWCOL
	{"#A0A0A0",WHITE,	0}	// COLOR_INFO_FG
 }
};

COLOR_ATTR *current_color;


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
	"word1_fg",
	"word2_fg",
	"word3_fg",
	"special_fg",
	"single_quote_fg",
	"comment_fg",
	"changed_fg",
	"horizon_fg",
	"inactive_fg",
	"rowcol_fg",
	"info_fg",NULL
};
