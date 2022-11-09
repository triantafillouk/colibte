/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

*/
/* variables and definitions common in X */

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
	{1,"Normal text",FOREGROUND,BACKGROUND,NULL,NULL},
	{2,"Active modeline",MODEFORE,MODEBACK,NULL,NULL},
	{3,"Pasive modeline",MODEFOREI,MODEBACKI,NULL,NULL},
	{4,"RowCol show",DROWCOL,-MODEBACK,NULL,NULL},
	{5,"Search text",SEARFORE,SEARBACK,NULL,NULL},
	{6,"Double quote text",QUOTEFORE,QUOTEBACK,NULL,NULL},
	{7,"Special chars text",SPECFORE,-BACKGROUND,NULL,NULL},
	{8,"Single quote text",SQUOTEFORE,-BACKGROUND,NULL,NULL},
	{9,"Preprocessing",PREPFORE,-BACKGROUND,NULL,NULL},
	{10,"Control chars",CTRLFORE,-BACKGROUND,NULL,NULL},
	{11,"Tags",TAGFORE,-BACKGROUND,NULL,NULL},
	{12,"Tags2 quote7",W_FORE,-BACKGROUND,NULL,NULL},
	{13,"Words1",WORD1FORE,-BACKGROUND,NULL,NULL},
	{14,"Words2",WORD2FORE,-BACKGROUND,NULL,NULL},
	{15,"Numeric values",CNUMERIC,-BACKGROUND,NULL,NULL},
	{16,"Comments",COMMENTFORE,-BACKGROUND,NULL,NULL},
	{17,"Info cols",INFOFORE,INFOBACK,NULL,NULL},
	{18,"Horizon", ORIZON,-BACKGROUND,NULL,NULL},
	{19,"ncurses box",CBOXTFORE,CBOXTBACK,NULL,NULL},
	{20,"ncurses menu",MENU_FG,MENU_BG,NULL,NULL},
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
#define COLOR_SCHEMES	10

// default color schemes
char *scheme_names[] = {
	"Snow","Midnight","Cyan","Blue","Gray","Dark gray",
	"Sunny","Twilight","Lunar","Velvet",
	NULL,NULL
};

char *color_name[COLOR_SCHEMES][XCOLOR_TYPES] = {
 // Snow
 {
	"#000000",	/* Foreground FOREGROUND */
	"#E3E3E3",	/* Background BACKGROUND */  /* !BG! */
	"#FFFFFF",	/* Modeline Foreground MODEFORE*/
	"#80B0D0",	/* Modeline BACKGROUND , selections MODEBACK*/ /* !BG! */
	"yellow2",	/* Display row-column DROWCOL*/
	"#EFE3B8",	/* search foreground SEARFORE*/
	"#AE0000",	/* search background SEARBACK*/ /* !BG! */
	"#303030",	/* QUOTEFORE , H_QUOTE2 */
	"#A0F0A0",	/* QUOTEBACK */ 
	"#906000",	/* COMMENTFORE H_QUOTE5 */
	"#E0D0A0",	/* LBACKGROUND  */
	"#00A080",	/* PREPFORE, H_QUOTE6 preprocessor foreground */
	"#FF0000",	/* CHANGEDFORE */
	"#303030",	/* TAGFORE H_QUOTE4, 8 */
	"#AE0000",	/* WORD1FORE */
	"#0000FF",	/* SPECFORE H_QUOTE1, single quotes */
	"#006000",	/* Single quote foreground */
	"#FF0000",	/* CTRLFORE control character foreground */
	"#800080",	/* WORD2FORE words type 2 foreground */
	"#006000",	/* W_FORE H_QUOTE7 tag words */
	"#800080",	/* ORIZON orizon characters foreground */
	"#0000AE",	/* CNUMERIC numeric foreground */
	"#303070",	/* INFOFORE  */
	"#E0D0A0",	/* INFOBACK  */ 
	"#EFE3B8",	/* CBOXTFORE, */ 
	"#0000AE", 	/* CBOXTBACK  */
	"#EFE3B8",	/* MENU_FG */
	"#0000AE",	/* MENU_BG  */
	"#000000",	/* Modeline Foreground MODEFOREI inactive */
	"#B0B0B0",	/* Modeline BACKGOUND  MODEBACKI inactive */
 },
 /* Midnight */
 {
	"#D0D0D0",	/* FOREGROUND */
	"black",	/* BACKGROUND */
	"white",	/* Modeline Foreground MODEFORE*/
	"#005070",	/* Modeline BACKGOUND  MODEBACK*/
	"yellow",	/* Display row-column DROWCOL*/
	"#FFF0F0",	/* search foreground SEARFORE*/
	"#A00000",	/* search background SEARBACK*/
	"#D0D0D0",	/* QUOTEFORE */
	"#008000",	/* QUOTEBACK */
	"#F09000",	/* COMMENTFORE */
	"grey10",	/* LBACKGROUND */
	"yellow",	/* PREPFORE */
	"bisque3",	/* CHANGEDFORE */
	"#98E098",	/* TAGFORE */
	"#D00000",	/* WORD1FORE */
	"#98E098",	/* SPECFORE */
	"#008000",	/* single quote foreground */
	"#FF0000",	/* CTRLFORE */
	"#00D0D0",	/* WORD2FORE */
	"magenta",	/* W_FORE */
	"magenta3", /* ORIZON */
	"#FFF0F0",	/* CNUMERIC */
	"grey80",	/* INFOFORE  */
	"grey20",	/* INFOBACK  */
	"white","blue",	/* CBOXTFORE, CBOXTBACK  */
	"white","blue",	/* MENU_FG, MENU_BG  */
	"black",	/* Modeline Foreground MODEFOREI inactive */
	"#303040",	/* Modeline BACKGOUND  MODEBACKI inactive */
 },
 /* Sea - cyan */
 {
	"#D0D0D0",	/* Foreground FOREGR*/
	"#004050",	/* Background BACKGR*/
	"white",	/* Modeline Foreground MODEFORE*/
	"blue4",	/* Modeline BACKGOUND  MODEBACK*/
	"yellow2",	/* Display row-column DROWCOL*/
	"#D0D0D0",	/* search foreground SEARFORE*/
	"#D00000",	/* search background SEARBACK*/
	"#D0D0D0",	/* QUOTEFORE */
	"#008000",	/* QUOTEBACK */
	"#F09000",	/* COMMENTFORE */
	"#005080",	/* LBACKGROUND */
	"yellow",	/* PREPFORE */
	"#FF0000",	/* CHANGEDFORE */
	"#00FF00",	/* TAGFORE */
	"#D00000",	/* WORD1FORE */
	"#00FF00",	/* SPECFORE */
	"#008000",	/* single quote foreground */
	"#FF0000",	/* CTRLFORE */
	"#00FFFF",	/* WORD2FORE */
	"#FF00FF",/* W_FORE */
	"#FF00FF", /* ORIZON */
	"#FFFFFF",/* CNUMERIC */
	"grey80",	/* INFOFORE  */
	"grey20",	/* INFOBACK  */ 
	"white","blue",
	"white","blue",
	"white",	/* Modeline Foreground MODEFOREI inactive */
	"#406070",	/* Modeline BACKGOUND  MODEBACKI inactive */
 },
 /* Blue Sky*/
 {
	"#D0D0D0",	/* Foreground FOREGR*/
	"#002060",	/* Background BACKGR*/
	"white",	/* Modeline Foreground MODEFORE*/
	"steelblue4",	/* Modeline BACKGOUND  MODEBACK*/
	"yellow",	/* Display row-column DROWCOL*/
	"#FFFFFF",	/* search foreground SEARFORE*/
	"#FF0000",		/* search background SEARBACK*/
	"#D0D0D0",	/* QUOTEFORE */
	"#008000",	/* QUOTEBACK */
	"#F09000",	/* COMMENTFORE */
	"#008080",	/* LBACKGROUND */
	"yellow",	/* PREPFORE */
	"bisque3",	/* CHANGEDFORE */
	"#00FF00",	/* TAGFORE */
	"#D00000",	/* WORD1BACK */
	"#00FF00",	/* SPECFORE */
	"#008000",	/* single quote foreground */
	"#FF0000",	/* CTRLFORE */
	"#00FFFF",	/* WORD2FORE */
	"#FF00FF",	/* W_FORE */
	"#FF00FF", /* ORIZON */
	"white",	/* CNUMERIC */
	"#D0D0D0",	/* INFOFORE  */
	"#404030",	/* INFOBACK  */ 
	"white","cyan",
	"white","cyan",
	"white",	/* Modeline Foreground MODEFOREI inactive */
	"#004040",	/* Modeline BACKGOUND  MODEBACKI inactive */
 },
 // GRAY
 {
	"black",	/* Foreground FOREGR*/
	"grey55",	/* Background BACKGR*/
	"white",	/* Modeline Foreground MODEFORE*/
	"#5080A0",	/* Modeline BACKGOUND  MODEBACK*/
	"yellow",	/* Display row-column DROWCOL*/
	"white",	/* search foreground SEARFORE*/
	"red4",	/* search background SEARBACK*/
	"#F2F2F2",	/* QUOTEFORE */
	"#50A050",	/* QUOTEBACK */
	"#E5D5A5",	/* COMMENTFORE */
	"#A8A8A0",	/* LBACKGROUND */
	"yellow",/* PREPFORE */
	"#FF4D4D",	/* CHANGEDFORE */
	"#00FFFF",	/* TAGFORE */
	"#8C008C",	/* WORD1FORE */
	"#0E500E",	/* SPECFORE */
	"#008000",	/* single quote foreground */
	"#FF4D4D",	/* CTRLFORE */
	"red4",/* WORD2FORE */
	"#00FFFF",	/* W_FORE */
	"#8C008C", /* ORIZON */
	"#0000CC",	/* CNUMERIC */
	"grey20",	/* INFOFORE  */
	"#A8A8A0",	/* INFOBACK  */ 
	"white","blue",
	"white","blue",
	"grey65",	/* Modeline Foreground MODEFOREI inactive */
	"#909090",	/* Modeline BACKGOUND  MODEBACKI inactive */
 },
 // Dark Gray
 {
	"#FFFFFF",	/* Foreground FOREGR*/
	"#303030",	/* Background BACKGR*/
	"white",	/* Modeline Foreground MODEFORE*/
	"#336164",	/* Modeline BACKGOUND  MODEBACK*/
	"yellow",	/* Display row-column DROWCOL*/
	"#FFFFFF",	/* search foreground SEARFORE*/
	"red2",	/* search background SEARBACK*/
	"#F2F2D9",	/* QUOTEFORE */
	"#007A00",	/* QUOTEBACK */
	"orange2",	/* COMMENTFORE */
	"#5F5F5F",	/* LBACKGROUND */
	"yellow",	/* PREPFORE */
	"#FF9999",	/* CHANGEDFORE */
	"orange2",	/* TAGFORE */
	"#FF9999",	/* WORD1FORE */
	"#66FF66",	/* SPECFORE */
	"#336194",	/* single quote foreground */
	"#FA0000",	/* CTRLFORE */
	"#9999FF",	/* WORD2FORE */
	"#FA0000",	/* W_FORE */
	"#FF00FF",  /* ORIZON */
	"#9999FF",	/* CNUMERIC */
	"#B0B0B0",	/* INFOFORE  */
	"#5F5F5F",	/* INFOBACK  */ 
	"#F2F2D9","gray50",
	"#F2F2D9","blue",
	"grey45",	/* Modeline Foreground MODEFOREI inactive */
	"#434343",	/* Modeline BACKGOUND  MODEBACKI inactive */
 },
 // Sunny
 {
	"black",	/* Foreground FOREGR*/
	"#E0E0F0",	/* Background BACKGR*/
	"#FFFFFF",	/* Modeline Foreground MODEFORE*/
	"#B0C0D0",	/* Modeline BACKGOUND  MODEBACK*/
	"yellow2",	/* Display row-column DROWCOL*/
	"#FFFFFF",	/* search foreground SEARFORE*/
	"#AD0000",	/* search background SEARBACK*/
	"#E0E4E0",	/* QUOTEFORE */
	"#00B000",	/* QUOTEBACK */
	"#A04040",	/* COMMENTFORE */
	"#E0E0D0",	/* LBACKGROUND */
	"#FF3333",	/* PREPFORE */
	"#FF3333",	/* CHANGEDFORE */
	"#A04040",	/* TAGFORE */
	"red3",		/* WORD1FORE */
	"#009900",	/* SPECFORE */
	"#009900",	/* single quote foreground */
	"#FF3333",	/* CTRLFORE */
	"#0000B3",/* WORD2FORE */
	"#009400",	/* W_FORE */
	"magenta3", /* ORIZON */
	"#1C99CC",	/* CNUMERIC */
	"black",	/* INFOFORE  */
	"#C0C0B0",	/* INFOBACK  */ 
	"white","gray20",
	"white","blue",
	"grey99",	/* Modeline Foreground MODEFOREI inactive */
	"#999999",	/* Modeline BACKGOUND  MODEBACKI inactive */
 },
 /* Twilight */
 {
	"#E0E0E0",	/* Foreground FOREGR*/
	"#141414",	/* Background BACKGR*/
	"white",	/* Modeline Foreground MODEFORE*/
	"#205060",	/* Modeline BACKGOUND  MODEBACK*/
	"yellow",	/* Display row-column DROWCOL*/
	"white",	/* search foreground SEARFORE*/
	"#F05050",	/* search background SEARBACK*/
	"white",	/* QUOTEFORE */
	"#407040",	/* QUOTEBACK */
	"#606070",	/* COMMENTFORE */
	"grey20",	/* LBACKGROUND */
	"#00B0CF",	/* PREPFORE */
	"bisque3",	/* CHANGEDFORE */
	"#F0AD80",	/* TAGFORE */
	"#CD7C33",	/* WORD1 */
	"#A0FFA0",	/* SPECFORE */
	"#508050",	/* single quote foreground */
	"#CD7C33",	/* CTRLFORE */
	"#F9EE98",	/* WORD2FORE */
	"#FF00FF",	/* W_FORE */
	"#FF00FF", /* ORIZON */
	"#F0AD80",	/* CNUMERIC */
	"#E0E0E0",	/* INFOFORE  */
	"#404040",	/* INFOBACK  */ 
	"white","grey70",
	"white","blue",
	"black",	/* Modeline Foreground MODEFOREI inactive */
	"#404050",	/* Modeline BACKGOUND  MODEBACKI inactive */
 },
// Lunar
 {
	"#E8D8D8",	/* Foreground FOREGR*/
	"#202020",	/* Background BACKGR*/
	"white",	/* Modeline Foreground MODEFORE*/
	"#006181",	/* Modeline BACKGOUND  MODEBACK*/
	"yellow",	/* Display row-column DROWCOL*/
	"#E8D8D8",	/* search foreground SEARFORE*/
	"#D06060",	/* search background SEARBACK*/
	"#E68000",	/* QUOTEFORE */
	"#202020",	/* QUOTEBACK */
	"#60B060",	/* COMMENTFORE */
	"#444433",	/* LBACKGROUND */
	"#FF90FF",	/* PREPFORE */
	"#FF90FF",	/* CHANGEDFORE */
	"#A0F0A0",	/* TAGFORE */
	"#D06060",	/* WORD1BACK */
	"#A0F0A0",	/* SPECFORE */
	"#60B060",	/* single quote foreground */
	"#FF4D4D",	/* CTRLFORE */
	"#40C0FF",	/* WORD2FORE */
	"#FF90FF",	/* W_FORE */
	"#FF90FF", /* ORIZON */
	"#A0F0A0",	/* CNUMERIC */
	"grey80",	/* INFOFORE  */
	"grey20",	/* INFOBACK  */ 
	"white","grey70",
	"white","blue",
	"black",	/* Modeline Foreground MODEFOREI inactive */
	"#4D4D4D",	/* Modeline BACKGOUND  MODEBACKI inactive */
 },
// Vcode
 {
	"#D3D3D3",	/* Foreground FOREGR*/
	"#1C1C1C",	/* Background BACKGR*/
	"white",	/* Modeline Foreground MODEFORE*/
	"steelblue4",	/* Modeline BACKGOUND  MODEBACK*/
	"yellow",	/* Display row-column DROWCOL*/
	"white",	/* search foreground SEARFORE*/
	"#D06060",	/* search background SEARBACK*/
	"#CD9000",	/* QUOTEFORE */
	"#1C1C1C",	/* QUOTEBACK */
	"#699854",	/* COMMENTFORE */
	"grey20",	/* LBACKGROUND */
	"#C485BF",	/* PREPFORE */
	"#F24646",	/* CHANGEDFORE */
	"#B4BF76",	/* TAGFORE */
	"#D06060",	/* WORD1BACK */
	"white",	/* SPECFORE */
	"#699854",	/* single quote foreground */
	"#F24646",	/* CTRLFORE */
	"#559BD5",	/* WORD2FORE */
	"#C485BF",	/* W_FORE */
	"#C485BF", /* ORIZON */
	"#B4BF76",	/* CNUMERIC */
	"#848484",	/* INFOFORE  */
	"#343435",	/* INFOBACK  */ 
	"#D3D3D3","#006181",
	"#D3D3D3","#0060A0",
	"#D3D3D3",	/* Modeline Foreground MODEFOREI inactive */
	"#343435",	/* Modeline BACKGOUND  MODEBACKI inactive */
 }
};
