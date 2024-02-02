/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

	Menu data.
*/

#include "menu.h"

#if TNOTES
M_element e_notes[] =
{
	// {0, 'N', "New note       ","", new_note,0,"FTN"},
	// {0, 'C', "new Cal note   ","", new_note, (struct MENUS *)2,"FTC"},
	// {0, 'C', "new Todo note  ","", new_note, (struct MENUS *)3,"FTC"},
	{0, 'D', "notes Dir        ","", open_notes_dir,0,"FTD"},
	{0, 'C', "Clear tags       ","", select_tag, (struct MENUS *)TAG_UNSELECT_ALL, "FTC"},
	{0, 'V', "View tag notes   ","", show_tag_view,0, "FTV"},
	{0, 'L', "caLendar list    ","", show_calendar_list,0,"FTL"},
	{0, 'T', "Todo list        ","", show_todo_list,0,"FTT"},
	{0, 'S', "Search notes     ","", grep_cmd, (struct MENUS *)3, "FTS"},
	{0, 'A', "save As note     ","", save_as_note,0,"FTA"},
	{0, 'I', "Init notes db    ","", init_notes_db,0,"FTI"},
	{0, 'U', "Update notes db  ","", recreate_notes_db,(struct MENUS *)0,"FTU"},
	{0, 'E', "rEcreate notes db","", recreate_notes_db,(struct MENUS *)1,"FTE"},
	{0, 'Y', "notes EncrYpt key","", set_notes_key,0,"FTY"},
	{0, 0, NULL,"",NULL,0,NULL}
};


MENUS m_notes = {
	"Notes",5,1,VERTICAL, e_notes
};

M_element e_note_popup[] =
{
	{0, 'N', "New note       ","", new_note,0,"FTN"},
	{0, 'C', "new Cal note   ","", new_note, (struct MENUS *)2,"FTC"},
	{0, 'C', "new Todo note  ","", new_note, (struct MENUS *)3,"FTC"},
	{0, 'D', "Delete tag note","", delete_tagnote,0,"WD"},

	{0, 0, NULL,"",NULL,0,NULL}
};

MENUS m_note_popup = {
	"Notes",5,1,VERTICAL, e_note_popup
};

M_element e_notes_tag[] = {
	{0, 'D', "Delete tag note     ","",delete_tagnote,0,"WD"},
	{0, 'T', "Toggle tag selection","",toggle_tag,0,"WT"},
	{0, 'U', "Unselect tags       ","",remove_tags,0,"WR"},
	{0, 0, NULL,"",NULL,0,NULL}
};

MENUS m_notes_tag = {
	"Notes work",10,1,VERTICAL, e_notes_tag
};
#endif

#if	TNOTES
M_element e_new[] =
{
	{0, 'F', "temporary File","", new_file, 0,"FNF"},
	{0, 'N', "Note          ","", new_note, 0,"FNN"},
	{0, 'L', "caLendar note ","", new_note, (struct MENUS *)2,"FNL"},
	{0, 'T', "Todo note     ","", new_note, (struct MENUS *)3,"FNT"},
	{0, 0, NULL,"",NULL,0,NULL}
};

MENUS m_new = {
	"New",5,1,VERTICAL, e_new
};
#endif

M_element e_file[] = 
{
	{0, 'O', "Open file    ^O","^OOpen file", open_file, (struct MENUS *)0,"FO" },

#if	TNOTES
	{MMENU, 'N', "New           >","", NULL, &m_new,"FN"},
	{MMENU, 'T', "noTes         >","", NULL, &m_notes,"FT"},
#else
	{0, 'N', "New file       ","", new_file, 0,"FN"},
#endif
	{0, 'R', "open Recent    ","", open_recent_file, 0,"FR"},
	{0, 'D', "Directory open ","", open_file, (struct MENUS *)2,"FD" },
	{0, 'G', "Goto on cursor ","", open_file, (struct MENUS *)3, "FG"},
	{0, 'S', "Save         ^S","^SSave",	save_file, 0 ,"FS"},	
	{0, 'A', "save As        ","",	saveas_file, 0,"FA" },	
	{0, 'E', "rEname         ","",rename_file, 0,"FE" },	
	{0, 'L', "reLoad file    ","",reload_file, 0,"FL"},
	{0, 'I', "Insert         ","",insert_file, 0,"FI"},	
	{0, 'H', "Highlight type ","",select_highlight,0,"FH"},
	{0, 'M', "Run Macro file ","",exec_file, (struct MENUS *)1,"FM"},
	{0, 'C', "Close          ","",close_file, 0,"FC"} ,	
	{0, 'X', "eXit      ALT-X","_XeXit",quit, 0,"FX"} ,	
	{0, 0, NULL,"",NULL,0,NULL}
};

MENUS m_file = {
	"File",
	0, 1, VERTICAL,	
	e_file
};

M_element e_bmark[] =
{
	{0, 'T', "seT bookmark      ALT-8","_8Set bookmark", set_bmark,0,"SBT"},
	{0, 'S', "Select bookmark   ALT-9","_9Select bookmark", select_bmark,0,"SBS"},
	{0, 'P', "Previous bookmark ALT-7","_7Previous bookmark", prev_bmark,0,"SBP"},
	{0, 'N', "Next bookmark     ALT-6","_6Next bookmark", next_bmark,0,"SBN"},
	{0, 'L', "cLear bookmarks   ALT-0","_0Clear bookmarks", clear_bmarks,0,"SBL"},
	{0, 0, NULL,"",NULL,0,NULL}
};

MENUS m_bmark = {
	"Bookmark",
	50,9, VERTICAL, e_bmark
};

M_element e_hmark[] =
{
	{0, 'H', "select History mark   ALT-J","_JSelect History mark",select_hmark,0,"SH"},
	{0, 'P', "Previous history mark ALT-P","_PPrevious History mark", prev_hmark,0,"SHP"},
	{0, 'N', "Next history mark     ALT-N","_NNext history mark", next_hmark,0,"SHN"},
	{0, 'L', "cLear history marks        ","", delete_hmark,0,"SHL"},
	{0, 0, NULL,"",NULL,0,NULL}
};

MENUS m_hmark = {
	"Bookmark",
	50,10, VERTICAL, e_hmark
};


M_element e_search[] =
{
	{0, 'F', "Forward search         ^F","^FForward search",	forwsearch, (struct MENUS *)1,"SF" },
	{0, 'R', "backwaRd search        ^R","^RBackward search",	backsearch, (struct MENUS *)1,"SR" },	
#if TNOTES
	{0, '5', "find next              F5","F5find next",	find_next, (struct MENUS *)1,"S5" },	
	{0, '6', "find previous          F6","F6find previous",	find_previous, (struct MENUS *)1,"S6" },
	{0, 'N', "search Notes dir         ","",   grep_cmd, (struct MENUS *)3, "SN"},
//	{0, 'D', "view notes by Date","",   view_notes_by_date, 0, "SN"},
#else
	{0, 'N', "find Next              F5","",	find_next, (struct MENUS *)1,"SN" },	
	{0, 'P', "find Previous          F6","",	find_previous, (struct MENUS *)1,"SP" },	
#endif
	{0, 'R', "Replace string      ALT-R","_RReplace string", sreplace, (struct MENUS *)1,"SR"},
	{0, 'Q', "Query replace            ","",	qreplace, (struct MENUS *)1,"SQ"} ,	
	{0, 'W', "sWap mark                ","",	swap_mark,0,"SW"},
#if	1
	{MMENU, 'B', "Bookmarks                ","",	NULL, &m_bmark,"SB"} ,
	{MMENU, 'H', "History marks            ","",	NULL, &m_hmark,"SH"} ,
#else
	{0, 'T', "seT bookmark             ","", set_bmark,0,"ST"},
	{0, 'S', "Select bookmark          ","", select_bmark,0,"SS"},
	{0, 'H', "select History mark ALT-J","_Jselect History mark",select_hmark,0,"SH"},
#endif
	{0, CHR_LBRA, "[ paragraph begin        "," [", goto_bop,0,"S["},
	{0, CHR_RBRA, "] paragraph end          "," ]", goto_eop,0,"S]"},
	{0, 'M', "Match fence            ^]","^]Match fence", match_fence,0,"SM"},
	{0, 'I', "find In dir              ","_5find In dir", grep_cmd,0,"SI"},
	{0, 'C', "reCursive find           ","", rgrep_cmd,0,"SC"},
	{0, 'L', "goto Line           ALT-G","_Ggoto Line", goto_line,0,"SL"}, 
	{0, '@', "View functions      ALT-@","_@view functions  @", view_functions,(struct MENUS *)2,"S1"},
	{0, '#', "View structures     ALT-#","_#view Structures #", view_functions,(struct MENUS *)3,"S2"},
	{0, 0, NULL,"",NULL,0,NULL}
};

M_element e_sort[] =
{
	{0, 'N', "Name sort         ","", set_sort_mode, (struct MENUS *)0,"M1N"},
	{0, 'S', "file Size sort    ","", set_sort_mode, (struct MENUS *)2,"M1S"},
	{0, 'M', "Modify time sort  ","", set_sort_mode, (struct MENUS *)4,"M1M"},
	{0, 'T', "sTatus time sort  ","", set_sort_mode, (struct MENUS *)6,"M1T"},
	{0, 'A', "Access time sort  ","", set_sort_mode, (struct MENUS *)8,"M1A"},
	{0, 'R', "Reverse sort order","", set_sort_mode, (struct MENUS *)-1,"M1R"},
	{0, 0, NULL,"",NULL,0,NULL}
};

MENUS m_sort = {
	"Sort",46,12,VERTICAL, e_sort
};


MENUS m_search = {
	"Search",
	16 ,1 , VERTICAL, e_search
};

M_element e_region[] =
{
	{0, 'U', "Upercase region     ","",	upperregion, 0,"EGU" },	
	{0, 'L', "Lowercase region    ","",	lowerregion, 0,"EGL" },
	{0,	'R', "Move Right          ","", insert_tab, 0,"EGR"},
	{0, 'E', "Move lEft           ","", remove_tab, 0,"EGE"},
	{0, 'F', "Fill Paragraph ALT-Q","_QFill aragraph",	fillpara, 0 ,"EGF"},
	{0, 0, NULL,"",NULL,0,NULL}
};

MENUS m_region = {
	"Region",
	25, 4, VERTICAL, e_region
};

M_element e_global_params[] =
{
//	{MTOGGLE,'W',"overWite characters  ","mode_over",toggle_parameter,(struct MENUS *)EMCOVER ,"MGW"},
#if	!GTK3
	{MTOGGLE,'E',"Exact match          ","exact_match",toggle_parameter,(struct MENUS *)EMCCASE ,"MGE"},
	{MTOGGLE,'G',"reG expressions      ","mode_regexp",toggle_parameter,(struct MENUS *)EMCREGEXP ,"MGG"},
#endif
	{MTOGGLE,'X',"results in heX also  ","show_hex",toggle_parameter,(struct MENUS *)EMSHOWHEX ,"MGX"},
	{MTOGGLE,'K',"Memacs Key emulation ","keyboard_emulation",toggle_parameter,(struct MENUS *)EMKEYEMUL ,"MGK"},
	{MTOGGLE,'B',"make Backup          ","make_backup",toggle_parameter,(struct MENUS *)EMBACKUP ,"MGB"},
#if	GTK
	{MTOGGLE,'I',"use embeded Icons    ","embed_icons",toggle_parameter,(struct MENUS *)EMBEDICONS ,"MGI"},
#endif
	{MTOGGLE,'S',"use Safe ops         ","safe_ops",toggle_parameter,(struct MENUS *)EMSAFEOPS,"MGS"},
	{MTOGGLE,'O',"show cursor Offset   ","show_coffset",toggle_parameter,(struct MENUS *)EMCOFFSET ,"MGO"},
	{MTOGGLE,'D',"show cursor Data     ","show_cdata",toggle_parameter,(struct MENUS *)EMCDATA ,"MGD"},
	{MTOGGLE,'H',"save file History    ","save_history",toggle_parameter,(struct MENUS *)EMSAVHIST ,"MGH"},
#if	GTK
	{MTOGGLE,'T',"large Toolbar icons  ","large_toolbar_icons",toggle_parameter,(struct MENUS *)EMLARGEICONS ,"MGT"},
	{MTOGGLE,'R',"use titlebaR         ","use_titlebar",toggle_parameter,(struct MENUS *)EMUSETITLEBAR ,"MGR"},
#endif
	{0, 0, NULL,"",NULL,0,NULL}
};


MENUS global_params = {
	"Global_params",
	50,1, VERTICAL,e_global_params
};

M_element e_edit[] =
{
	{0, 'S', "Start region  ^space","^ Start region",	set_mark, 0,"ES"} ,	
	{0, 'B', "start Box      Alt-C","_Cstart Box",	set_column_mark, 0,"EB"} ,	
	{0, 'L', "Line region    Alt-L","_LLine region",	set_line_mark, 0,"EL"} ,	
	{0, 'X', "cut  region       ^X","^Xcut region X",	cut_region, 0,"EX"} ,	
	{0, 'C', "copy  region      ^C","^CCopy region",	copy_region, 0,"EC"} ,	
	{0, 'V', "paste region      ^V","^Vpaste region V",	paste_region, 0,"EV"} ,	
	{MMENU, 'G', "reGion ops          ","",	NULL, &m_region,"EI"} ,
	{0,	'D', "insert Date         ","", insert_date, 0 ,"ED"},
	{0,	'M', "insert date staMp   ","", insert_date, (struct MENUS *)1 ,"EM"},
	{0,	'F', "insert date Full    ","", insert_date, (struct MENUS *)2 ,"EF"},
	{0, 'W', "Word count          ","",	countwords, 0 ,"EW"},
	{0, 'F', "Fill paragraph ALT-Q","_QFill paragraph",	fillpara, 0 ,"EF"},
	{0, 'B', "del Blank lines     ","",	del_blank_lines, 0,"EB"},
	{0, 'T', "spaces to Tabs      ","",  entab_line, (struct MENUS *)1,"ET"},
	{0, 'A', "tabs to spAces      ","",  detab_line, (struct MENUS *)1,"EA"},
	{0, 'I', "trIm line           ","",  trim_line,0,"EI"},
	{0, 'U', "Undo              ^Z","^ZUndo", undo, (struct MENUS *)1,"EU"},
	{0, 'R', "Redo           ALT-Y","_YRedo", redo,(struct MENUS *)1,"ER"},
	{0, 'P', "Preferences         ","",set_option,0,"EP"},
	{0, 0, NULL,"",NULL,0,NULL}
};

MENUS m_edit = 
{
	"Edit",
	8, 1, VERTICAL,e_edit
};


M_element e_select_on[] = 
{
	{0, 'C', "Copy         ^C","^Ccopy",	copy_region, 0,"EC"} ,	
	{0, 'X', "cut          ^X","^Xcut",	cut_region, (struct MENUS *)1,"EX"} ,	
	{0, 'U', "Unselect       ","",  set_mark,0,"EU"},
	{0, 0, NULL,"",NULL,0,NULL}
};

MENUS m_select_on = {
	"Select on",
	9, 1, VERTICAL,	e_select_on
};

M_element e_select_off[] = 
{
	{0, 'B', " <-  go Back    ","", prev_hmark, (struct MENUS *)1, "EWB"},
	{0, 'F', " ->  go Forward ","", next_hmark, (struct MENUS *)1, "EWF"},
	{0, 'S', "Start selection ","", set_mark, 0, "ES"} ,	
	{0, 'C', "start Column sel","", set_column_mark, 0,"EC"},
	{0, 'P', "Paste           ","", system_paste, 0,"EV"} ,	
	{0, 'T', "find definiTion ","", find_tag, (struct MENUS *)1, "EWT"},
	{0, 'G', "Goto file       ","", open_file, (struct MENUS *)3, "EWG"},
	{0, 'M', "Match fence   ^]","", match_fence,0,"SM"},
	{0, 0, NULL,"",NULL,0,NULL}
};

/* region but no rectangle in buffer */
MENUS m_select_off = {
	"Select off",
	9, 1, VERTICAL, e_select_off
};

/* menu for no region in buffer */

/* menu for box region in buffer */
M_element e_boxed_region[] =
{
	{0, 'U', "Unmark region  ","",	set_mark, 0,"ES"} ,	
	{0, 'B', "Copy box       ","",	copy_region, 0,"EO"},
	{0, 'D', "Delete box     ","",	cut_region,0,"ED"},
	{0, 0, NULL,"",NULL,0,NULL}
};

MENUS m_boxed_region = {
	"Box region",
	9, 1, VERTICAL, e_boxed_region
};

M_element e_top_popup[] =
{
	{MMENU,'S',"SELECT   ","", NULL, &m_select_on ,""},
	{MMENU,'N',"NOSELECT ","", NULL, &m_select_off ,""},
	{MMENU,'N',"BOXREGION","", NULL, &m_boxed_region,"" },
	{0, 0, NULL,"",NULL,0}
};

MENUS m_top_popup = {
	"top popup",
	0, 0, HORIZONTAL,e_top_popup
};

M_element e_window[] =
{
	{0, 'F', "show File        ^B","^Bselect File", select_file, 0,"WF" },	
	{0, 'N', "Next file        ^N","^NNext file", next_file, 0,"WN" },
	{0, '1', "maximize window   1"," 1only 1", one_window, 0,"W1" },	
	{0, '0', "delete window     0"," 0delete window", delete_window, 0,"W0" },	
	{0, '2', "horizontal split  2"," 2horizontal split 2", split_window, 0,"W2" },	

	{0, '3', "vertical split    3"," 3vertical split 3", vsplit_window, 0,"W3" },	
	{0, '[', "Previous          ["," [Previous", prev_window, 0,"W[" },	
	{0, ']', "Next              ]"," ]Next", next_window, 0,"W]"} ,	
	{0, 'I', "view Internal file ","", select_file_internal, 0,"WI" },
	{0, 'D', "Directory list     ","", listdir1, 0,"WD"},
	{0, 'S', "deStination dir    ","", listdir2, 0,"WS"},
#if TNOTES
	{0, 'T', "noTes dir          ","", open_notes_dir,0,"WT"},
	{0, 'W', "note tag vieW      ","", show_tag_view,0,"WW"},
#endif
	{0, 'O', "Output window      ","", sh_outwindow,0,"WO"},
	{0, 'P', "Parse buffer       ","", show_parse_buffer,0,"WP"},
#if	!GTK
	{0, 'R', "resize Rows        ","", window_row_resize,0,"WR"},
	{0,	'C', "resize Columns     ","", window_column_resize,0,"WC"},
#endif
	/* error log */
	{0, 0, NULL,"",NULL,0,NULL}
};

MENUS m_window = {
	"Window",
	24, 1, VERTICAL, 	e_window
};

M_element e_help[] = 
{
	{0, 'A', "About            ","",  show_version,   0,"HA"},
	{0, 'L', "key List         ","",	show_keys, 0,"HL" },	
	{0, 'D', "Describe key   F4","F4Describe key",	describe_key, 0,"HD" },	
	{0, 'K', "assign Key  ALT-K","_Kassigh Key",	assign_function, 0,"HK"} ,	
	{0, 'R', "Remove key ALT-^K","*KRemove key",	unassign_key, 0,"HR" },
	{0, 'H', "Help file        ","",	help,0,"HH"},
	{0, 'F', "File info      F1","F1File info",  show_info,0,"HF"},
	{0, 0, NULL,"",NULL,0,NULL}
};

MENUS m_help = {
	"Help",
	48, 1, VERTICAL,e_help
};

#if	!PCURSES
M_element e_color[] =
{
	{0, 'S',"Snow     ","", change_color_scheme, (struct MENUS *)1,"MMS"},
	{0, 'M',"Midnight ","", change_color_scheme, (struct MENUS *)2,"MMM"},
	{0, 'C',"Cyan     ","", change_color_scheme, (struct MENUS *)3,"MMC"},
	{0, 'B',"Blue     ","", change_color_scheme, (struct MENUS *)4,"MMB"},
	{0, 'G',"Gray     ","", change_color_scheme, (struct MENUS *)5,"MM1"},
	{0, 'D',"Dark gray","", change_color_scheme, (struct MENUS *)6,"MMD"},
	{0, 'U',"sUnny    ","", change_color_scheme, (struct MENUS *)7,"MMU"},
	{0, 'T',"Twilight ","", change_color_scheme, (struct MENUS *)8,"MMT"},
	{0, 'L',"Lunar    ","", change_color_scheme, (struct MENUS *)9,"MML"},
	{0, 'V',"Velvet   ","", change_color_scheme, (struct MENUS *)10,"MMV"},
	{0, 0, NULL,"",NULL,0,NULL}
};

MENUS m_color = {
	"Color",
	50, 1, VERTICAL,e_color
};
#endif

#if	USE_GLIB
M_element e_gconvert[] =
{
	{0, '9', "IBM PC to ISO8859", "", 	convert_char, (struct MENUS *)1,"MT9"},
	{0, 'P', "ISO8859 to IBM PC", "",	convert_char, (struct MENUS *)2,"MTP"},
	{0, 'U', "local to UTF8    ", "",	convert_char, (struct MENUS *)3,"MTU"},
	{0, 'F', "Utf8 to local    ", "",	convert_char, (struct MENUS *)4,"MTF"},
	{0, 0, NULL,"",NULL,0}
};
#endif

#if	USE_GLIB
MENUS m_gconvert = {
	"GConvert",
	50, 5, VERTICAL,e_gconvert
};
#endif

M_element e_convert[] = 
{
	{0, 'D',"Convert to DOS ","", unix2dos, 0,"TVD"},
	{0, 'U',"Convert to Unix","", dos2unix, 0,"TVU"},
	{0, 0, NULL,"",NULL,0}
};

MENUS m_convert = {
	"Convert",
	61,4, VERTICAL, e_convert
};

M_element e_viewas[] =
{
	{0,'U',"as Unix    ","",set_view_mode,(struct MENUS *)2,"MVU"},
	{0,'D',"as Dos     ","",set_view_mode,(struct MENUS *)1,"MVD"},
	{0,'X',"as heX     ","",set_view_mode,(struct MENUS *)3,"MVX"},
	{0,'M',"as MAC     ","",set_view_mode,(struct MENUS *)4,"MVM"},
	{0,'L',"show Lines ","",set_view_mode,(struct MENUS *)5,"MVL"},
	{0,'O',"show Offset","",set_view_mode,(struct MENUS *)6,"MVO"},
	{0,'I',"show vInfo ","",set_view_mode,(struct MENUS *)7,"MVI"},
	{0,'0',"0 hide info","",set_view_mode,(struct MENUS *)8,"MV0"},
	{0,'W',"Wrap mode  ","",set_view_mode,(struct MENUS *)9,"VMW"},
	{0, 0, NULL,"",NULL,0,NULL}
};

MENUS m_viewas = {
	"View as",
	50,5, VERTICAL,e_viewas
};



M_element e_lang[] =
{
	{0,'U',"UTF-8       ","",set_doc_lang,(struct MENUS *)1 ,"MAU"},
	{0,'1',"ISO-8859-1  ","",set_doc_lang,(struct MENUS *)2 ,"MA1"},
	{0,'2',"ISO-8859-2  ","",set_doc_lang,(struct MENUS *)3 ,"MA2"},
	{0,'3',"ISO-8859-3  ","",set_doc_lang,(struct MENUS *)4 ,"MA3"},
	{0,'4',"ISO-8859-4  ","",set_doc_lang,(struct MENUS *)5 ,"MA4"},
	{0,'5',"ISO-8859-5  ","",set_doc_lang,(struct MENUS *)6 ,"MA5"},
	{0,'6',"ISO-8859-6  ","",set_doc_lang,(struct MENUS *)7 ,"MA6"},
	{0,'7',"ISO-8859-7  ","",set_doc_lang,(struct MENUS *)8 ,"MA7"},
	{0,'8',"ISO-8859-8  ","",set_doc_lang,(struct MENUS *)9 ,"MA8"},
	{0,'9',"ISO-8859-9  ","",set_doc_lang,(struct MENUS *)10,"MA9" },
	{0,'0',"ISO-8859-10 ","",set_doc_lang,(struct MENUS *)11,"MA0" },
	{0,'A',"ISO-8859-11 ","",set_doc_lang,(struct MENUS *)12,"MAA" }, 
	{0,'B',"ISO-8859-13 ","",set_doc_lang,(struct MENUS *)13,"MAB" },
	{0,'C',"ISO-8859-14 ","",set_doc_lang,(struct MENUS *)14,"MAC" },
	{0,'D',"ISO-8859-15 ","",set_doc_lang,(struct MENUS *)15,"MAD" },
	{0,'E',"ISO-8859-16 ","",set_doc_lang,(struct MENUS *)16,"MAE" },
	{0,'F',"windows-1253","",set_doc_lang,(struct MENUS *)17,"MAF" },
	{0,'G',"KOI8-T      ","",set_doc_lang,(struct MENUS *)18,"MAG" },
	{0,'H',"CP874       ","",set_doc_lang,(struct MENUS *)19,"MAH" },
	{0,'I',"CP737       ","",set_doc_lang,(struct MENUS *)20,"MAI" },
#if	GTK
	{0,' ',     "                     ","",noop,NULL,"EG-"},
#endif
	{0, 0, NULL,"",NULL,0,NULL}
};

#if	USE_GLIB
M_element e_local[] =
{
	{0,'0',"utf-8       ","",set_default_local,(struct MENUS *)1 ,"ML0"},
	{0,'1',"ISO-8859-1  ","",set_default_local,(struct MENUS *)2 ,"ML1"},
	{0,'2',"ISO-8859-2  ","",set_default_local,(struct MENUS *)3 ,"ML2"},
	{0,'3',"ISO-8859-3  ","",set_default_local,(struct MENUS *)4 ,"ML3"},
	{0,'4',"ISO-8859-4  ","",set_default_local,(struct MENUS *)5 ,"ML4"},
	{0,'5',"ISO-8859-5  ","",set_default_local,(struct MENUS *)6 ,"ML5"},
	{0,'6',"ISO-8859-6  ","",set_default_local,(struct MENUS *)7 ,"ML6"},
	{0,'7',"ISO-8859-7  ","",set_default_local,(struct MENUS *)8 ,"ML7"},
	{0,'8',"ISO-8859-8  ","",set_default_local,(struct MENUS *)9 ,"ML8"},
	{0,'9',"ISO-8859-9  ","",set_default_local,(struct MENUS *)10,"ML9" },
	{0,'0',"ISO-8859-10 ","",set_default_local,(struct MENUS *)11,"ML0" },
	{0,'A',"ISO-8859-11 ","",set_default_local,(struct MENUS *)12,"MLA" }, 
	{0,'B',"ISO-8859-13 ","",set_default_local,(struct MENUS *)13,"MLB" },
	{0,'C',"ISO-8859-14 ","",set_default_local,(struct MENUS *)14,"MLC" },
	{0,'D',"ISO-8859-15 ","",set_default_local,(struct MENUS *)15,"MLD" },
	{0,'E',"ISO-8859-16 ","",set_default_local,(struct MENUS *)16,"MLE" },
	{0,'F',"windows-1253","",set_default_local,(struct MENUS *)17,"MLF" },
	{0,'G',"KOI8-T      ","",set_default_local,(struct MENUS *)18,"MLG" },
	{0,'H',"CP874       ","",set_default_local,(struct MENUS *)19,"MLH" },
	{0,'I',"CP737       ","",set_default_local,(struct MENUS *)20,"MLI" },
#if	GTK
	{0,' ',     "                     ","",noop,NULL,"EG-"},
#endif
	{0, 0, NULL,"",NULL,0,NULL}
};
#endif

#if	USE_GLIB
MENUS m_lang = {
	"Lang",
	50,4, VERTICAL,e_lang
};

MENUS m_local = {
	"Local",
	50,4, VERTICAL,e_local
};
#endif

M_element e_mode[] =
	{
	{0, 'H', "Highlight toggle ","",  toggle_highlight, 0,"MH"},
	{0, 'R', "cRypt            ","",  toggle_crypt, 0,"MR"},
	{0, 'J', "Justify          ","",  toggle_just, 0,"MJ"},	
	{MMENU,'G',"Global options   ","", NULL, &global_params,"MG" },
	{MMENU,'V',"View mode        ","", NULL, &m_viewas,"MV"},
#if	PCURSES
	{0, 'M', "color scheMe    >","", select_scheme,0,"MM" },
#else
#if	GTK
	{0, 'M', "color scheMe     ","", set_color,0,"MM"},
#else
	{MMENU,'M',"color scheMe     ","", NULL, &m_color,"MM" },
#endif
#endif
#if	USE_GLIB
	{MMENU, 'T', "character converT", "", 	NULL, &m_gconvert,"MT"},
#endif
#if	!PCURSES
	{0, 'F', "select Font     ","",select_font_mono,0,"MF"},
#endif
	{0, 'L', "Line lock        ","", toggle_move_lock,0,"ML"},
	{0,	'P', "Page lock        ","", toggle_sync_lock,0,"MP"},
#if	USE_GLIB
	{MMENU, 'C', "file Codepage    ", "",  NULL, &m_lang,"MC"},
#if	!GTK
	{MMENU, 'F', "Font local       ", "",  NULL, &m_local,"MF"},
#endif
#endif
    {MMENU, 'S', "Sort mode        ", "",  NULL, &m_sort,"MS"},
	{0, 0, NULL,"",NULL,0,NULL}
};

MENUS m_mode = {
	"Mode",
	32, 1, VERTICAL, 
	e_mode
};

M_element e_debug[] =
{
	{0,	'S', "Save session     ","",	save_session,0,"TDS"},
	{0, 'O', "lOad session     ","",	load_session,0,"TDO"},
	{0, 'V', "View session     ","",	show_session,0,"TDV"},
	{0, 'P', "Parse check      ","",	parse_check_current_buffer,0,"TDP"},
	{0, 'T', "Token list       ","",	parse_buffer_show_tokens,0,"TDT"},
	{0, 'D', "set Debug flag   ","",	set_debug,(struct MENUS *)1,"TDD"},
	{0, 'R', "Reset debug flag ","",	set_debug,(struct MENUS *)0,"TDR"},
	{0, 0, NULL,"",NULL,0,NULL}
};

MENUS m_debug = {
	"Debug",
	81,12,VERTICAL, e_debug
};

M_element e_tools[] =
{
	{0, 'C', "shell Command    ","_1shell Command",	shell_cmd, 0,"TC" },
	{0, 'B', "Background cmd   ","",	bg_cmd, 0 ,"TB"},
	{0, 'F', "Filter Buffer    ","_3Filter Buffer",	filter_buffer, 0,"TF" },
	{0, 'G', "Grep Buffer      ","",	grep_buffer, 0,"TG" },
	{MMENU,'V', "conVert          ","", NULL, &m_convert,"TV"},
	{0, 'T', "Temporary shell  ","",	new_shell, 0,"TT"},
	{0, 'X', "eXecute Macro  F8","F8eXecute Macro",	exec_macro, (struct MENUS *)1,"TX" },
	{0, 'R', "Record Macro   F7","F7Record Macro",	record_macro, 0,"TR" },
	{0, 'M', "Macro line       ","_4Macro line",	macro_line, 0,"TM" },
	{0, 'U', "Update ctags     ","",	update_tags, 0,"TU" },
	{0, 'P', "recalc Program   ","_+recalc Program",  refresh_current_buffer, 0,"TP"},
	{0, 'L', "recalc Line      ","_=recal Line",  refresh_current_line, 0,"TL"},
#if	TNOTES0
	{0, 'Y', "Notes EncrYpt key","", 	set_notes_key,0,"TY"},
	{0, 'N', "init Notes db    ","",    init_notes_db,0,"TN"},
	{0, 'E', "rEcreate notes db","",	recreate_notes_db,0,"TE"},
#endif
	{MMENU, 'D', "Debug            ","", NULL, &m_debug,"TD"},
	{0, 0, NULL,"",NULL,0,NULL}
};

MENUS m_tools = {
	"Tools",
	40, 1, VERTICAL,e_tools
};

M_element e_topn[] =
{
	{MMENU,'F',"FILE","", NULL, &m_file,"F" },
	{MMENU,'E',"Edit","", NULL, &m_edit,"E" },
	{MMENU,'S',"Search","", NULL, &m_search,"S" },
	{MMENU,'W',"Window","", NULL, &m_window,"W" },
	{MMENU,'M',"Mode","", NULL, &m_mode,"M" },
	{MMENU,'T',"Tools   ","", NULL, &m_tools,"T" },
	{MMENU,'H',"Help","", NULL, &m_help,"H"},
#if	!GTK & TARROWS
#if	USE_UTF8
	{0,    'B'," ←  ","previous", prev_hmark, (struct MENUS *)1, "EWB"},
	{0,    '1'," →  ","next", next_hmark, (struct MENUS *)1, "EWF"},
	{0,    '2'," ↓  ","find next",	find_next, (struct MENUS *)1,"S5" },	
	{0,    '3'," ↑  ","find previous",	find_previous, (struct MENUS *)1,"S6" },
#else
	{0,    '0'," <- ","back", prev_hmark, (struct MENUS *)1, "EWB"},
	{0,    '1'," -> ","next", next_hmark, (struct MENUS *)1, "EWF"},
	{0,    '2',"Next","find next",	find_next, (struct MENUS *)1,"S5" },	
	{0,    '3',"Prev","find previous",	find_previous, (struct MENUS *)1,"S6" },
#endif
#endif
	{0, 0, NULL,"",NULL,0,NULL}
};

MENUS m_topn = {
	"Topn",
	0, 0, HORIZONTAL,e_topn
};
