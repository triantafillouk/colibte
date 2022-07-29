/*
	colibte. Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.
*/
/*	Keyboard function default assignement table  */
/*	structure for the table of initial key bindings		*/

typedef struct  {
        int		k_code;                 /* Key code                     */
        int     (*k_fp)();              /* Routine to handle it         */
		char 	*macro_name;				/* macro name */} KEYTAB;

KEYTAB  keytab_emacs[MAXASSIGNS] = {
	{CNTRL|'A',		goto_bol,"^A"},
	{CNTRL|'B',		select_file,"^B"},
	{CNTRL|'C',		copy_region,"^C"},
	{CNTRL|'D',		delete_line,"^D"},
  	{CNTRL|'E',		goto_eol,"^E"},
	{CNTRL|'F',		forwsearch,"^F"},
	{CNTRL|'G',		abort_cmd,"^G"},
	{CNTRL|'H',		del_prev_char,"^H"},
	{CNTRL|'I',		tab,"tab"},
	{CNTRL|'J',		insert_indent,"^J"},
	{CNTRL|'K',		erase_eol,"^K"},

	{CNTRL|'L',		reposition,"^L"},
	{CNTRL|'M',		new_line,"^M"},
	{CNTRL|'N',		next_file,"^N"},
	{CNTRL|'O',		open_file,"^O"},
	{CNTRL|'P',		next_window,"^P"},
	{CNTRL|'Q',		quote,"^Q"},
   	{CNTRL|'R',		backsearch,"^R"},
	{CNTRL|'S',		save_file,"^S"},
	{CNTRL|'T',		twiddle,"^T"}, 
	{CNTRL|'U',		prev_hmark,"^U"},
	{CNTRL|'V',		paste_region,"^V"},
 	{CNTRL|'W',		cut_region,"^W"},	
//	{CNTRL|'Y',		system_paste,"^Y"},
	{CNTRL|'Y',		redo,"^Y"},
	{CNTRL|'Z',		undo,"^Z"},
	{CNTRL|']',		match_fence,"^]"},
	{CNTRL|'@',		set_mark,"^SPACE"},
	{CNTRL|'\\',	goto_eow,"^\\"},
#if	!PCURSES
	{CNTRL|'_',		decrease_font_size,"^_"},
	{CNTRL|'^',		increase_font_size,"^^"},
#endif

	{KMOUS|'a',		scroll_up,"SCROLL-UP"},
	{KMOUS|'`',		scroll_down,"SCROLL-DOWN"},

	{KMOUS|'C',		text_mouse_pos,"MOUSE_POS"},
	{KMOUS|' ',		text_mouse_left_press,"MOUSE_LEFT"},
	{KMOUS|35,		text_mouse_release,"MOUSE_RELEASE"},
	{KMOUS|34,		text_mouse_right_press,"MOUSE_RIGHT"},
	{KMOUS,			text_mouse_error,"mouse error"},

	{META|' ',		set_arg,"ALT space"},
	{META|'.',		find_tag,"ALT-."},		// find tag
	{META|'@',		view_functions,"ALT-@"},
	{META|'#',		view_functions,"ALT-#"},
	{META|'1',		shell_cmd,"ALT-1"},
	{META|'2',		script_exec,"ALT-2"},
	{META|'3',		filter_buffer,"ALT-3"},
	{META|'4',		macro_line,"ALT-4"},
	{META|'5',		grep_cmd,"ALT-5"},
	{META|'%',		rgrep_cmd,"ALT-%"},
	/* alternate bookmark funtions */
	{META|'0',		clear_bmarks,"ALT-0"},
	{META|'9',		select_bmark,"ALT-9"},
	{META|'8',		set_bmark,"ALT-8"},
	{META|'7',		next_bmark,"ALT-7"},
	{META|'6',		prev_bmark,"ALT-6"},
	{META|'|',		prev_word,"ALT-|"},
	{META|'\\',		goto_eow,"ALT-\\"},

	{META|'&',		read_colors,"ALT-&"},
	{META|'*',		save_colors,"ALT-*"},

	{META|'A',		menufile,"ALT-A"},
	{META|'B',		prev_word,"ALT-B"},
	{META|'C',		set_column_mark,"ALT-C"},
	{META|'D',		delete_word,"ALT-D"},
//	E-> menu edit
//	F-> menu file
	{META|'G',		goto_line,"ALT-G"},
//	H-> menu help
	{META|'I',		insert_tab,"ALT-I"},
	{META|'J',		select_hmark,"ALT-J"},
	{META|'K',		assign_function,"ALT-K"},
	{META|'L',		set_line_mark,"ALT-L"},
	{META|'N',		next_hmark,"ALT-N"},
	{META|'o',		select_file_internal,"ALT-O"},
	{META|'P',		prev_hmark,"ALT-P"},
	{META|'Q',		fillpara,"ALT-Q"},
	{META|'R',		sreplace,"ALT-R"},
//	s -> search menu
//	t -> tools menu
	{META|'V',		system_paste,"ALT-V"},
//	w --> menu window
	{META|'X',		quit,"ALT-X"},
	{META|'Y',		toggle_mouse,"^Y"},
	{META|'Z',		set_bg,"ALT-Z"},	

	{META|'>',		goto_eof,"ALT->"},
	{META|'<',		goto_bof,"ALT-<"},
	{META|'{',		both_up,"ALT {"},
	{META|'}',		both_down,"ALT }"},
	{META|'=',		refresh_current_line,"ALT-="},
	{META|'+',		refresh_current_buffer,"ALT-+"},
//	{META|'^',		vsplit_window,"ALT-^"},
//	{META|'_',		split_window,"ALT-^_"},
	{META|'\'',		comment_line,"ALT-'"},
	{META|';',		comment_line2,"ALT-;"},

	{CTLX|CNTRL|'I',	remove_tab,"^X^I"},	// shift tab
#if	RSESSION
	{CTLX|CNTRL|'A',		save_session,"^X^A"},	// tools menu
	{CTLX|CNTRL|'B',		load_session,"^X^B"},	// tools menu
#endif
	{CTLX|CNTRL|'C',		quit,"^X^C"},          /* Hard quit. */
    {CTLX|CNTRL|'D',		detab_line,"^X^D"},		/* edit menu  */
	{CTLX|CNTRL|'E',		entab_line,"^X^E"},		/* edit menu  */
#if	!PCURSES
	{CTLX|CNTRL|'F',		select_font_mono,"^X^F"},	// menu block
#endif
	{CTLX|CNTRL|'T',		trim_line,"^X^T"},		// menu block
	{CTLX|CNTRL|'L',		lowerregion,"^X^L"},	// menu block
	{CTLX|'D',		listdir1,"^XD"},	// menu window
	{CTLX|'F',		linefill,"^XF"},	// line fill

	{CTLX|CNTRL|'M',		toggle_move_lock,"^X^M"},		// change move lock
	{CTLX|CNTRL|'O',		del_blank_lines,"^X^O"},		/* edit menu block */
	{CTLX|CNTRL|'S',		save_file,"^X^S"},		/* file menu file */
	{CTLX|CNTRL|'U',		upperregion,"^X^U"},	// menu block
#if	RSESSION
	{CTLX|CNTRL|'V',		show_session,"^X^V"},	/* tools menu  */
#endif
	{CTLX|CNTRL|'W',		saveas_file,"^X^W"},		/* menu file (save as) */
	{CTLX|CNTRL|'X',		swap_mark,"^X^X"},

	{CTLX|'0',		delete_window,"^X0"},	/* menu window */
	{CTLX|'1',		one_window,"^X1"},		/* menu window */
	{CTLX|'2',		split_window,"^X2"},		/* menu window */
	{CTLX|'3',		vsplit_window,"^X3"},		/* menu window */
#if	!PCURSES
	{CTLX|'+',		increase_font_size,"^X+"},
	{CTLX|'-',		decrease_font_size,"^X-"},
	{CTLX|'B',		toggle_bold,"^XB"},		/* toggle bold font */
#endif
	{CTLX|'H',		show_parse_buffer,"^XH"},
	{CTLX|'I',		toggle_over,"^XI"},	/* menu mode  */
	{CTLX|'J',		toggle_just,"^XJ"},	/* menu mode  */
	{CTLX|'L',		toggle_sync_lock,"^XL"},	/* toggle sync lock  */
	{CTLX|'K',		close_file,"^XK"},	/* file menu */
	{CTLX|'M',		toggle_move_lock,"^XM"},	/* toggle move lock  */
	{CTLX|'N',		rename_file,"^XN"},	/* file menu */
	{CTLX|'O',		next_window,"^XO"}, 	// menu window
	{CTLX|'P',		prev_window,"^XP"},		// menu window
	{CTLX|'R',		window_row_resize,"^XR"}, // resize window rows
	{CTLX|'V',		window_column_resize,"^XV"},	/* resize window columns   */
	{CTLX|CNTRL|'R', replace_next,"^X^R"}, 
	{CTLX|'S',		set_option,"^XS"},		// menu mode
	{CTLX|'G',		update_tags,"^XG"},	// update tagx file (ctags -x)
	/* bookmark functions */
	{CTLX|'T',		set_bmark,"^XT"},		// menu block
	{CTLX|'U',		redo,"^XU"},
	{CTLX|'Z',		set_bg,"^XZ"},	
	{CTLX|CHR_LBRA,		goto_bop,"^X["},
	{CTLX|CHR_RBRA,		goto_eop,"^X]"},
	{CTLX|'!',		shell_cmd,"^X!"},	/* menu tools */
	{CTLX|'#',		filter_buffer,"^X#"},	// menu tools
	{CTLX|'&',		bg_cmd,"^X&"},	// menu tools
	{CTLX|'=',		refresh_current_buffer,"^X="},
	{CTLX|'\\',		parse_check_current_buffer,"^X\\"},	// menu tools
	{CTLX|CNTRL|'R',			qreplace,"^X^R"},


	{META|CNTRL|'C',		countwords,"ALT-^C"},
	{META|CNTRL|'H',		delbword,"ALT-^H"},
	{META|CNTRL|'K',		assign_function,"ALT-^K"},
	{META|CNTRL|'L',		toggle_sync_lock,"ALT-^L"},
	{META|CNTRL|'M',		toggle_move_lock,"ALT-^M"},
	{META|CNTRL|'U',		unassign_key,"ALT-^U"},
	{META|CNTRL|'O',		toggle_over,"ALT-^O"},
	{META|CNTRL|'P',		hard_reposition,"ALT-^P"},
	{META|CNTRL|'R',		qreplace,"ALT-^R"},
	{META|CNTRL|'X',		macro_line,"ALT-^X"},
	{META|CNTRL|CHR_LBRA,		abort_cmd,"Escape"},

#if	DARWIN0
   {0x7F,          del_prev_char,"DEL"},
#else
	{0x7F,			del_char,"DEL"},
#endif
#if	1
	{SPEC1|'A',		prev_line,"KEY-UP"},
	{SPEC1|'D',		prev_character,"KEY-LEFT"},
	{SPEC1|'C',		next_character,"KEY-RIGHT"},
	{SPEC1|'B',		next_line,"KEY-DOWN"},
#if	1
	{SPEC1|5,		page_move_down,"PG-UP"},
	{SPEC1|6,		page_move_up,"PG-DOWN"},
#else
	{SPEC1|5,		prev_page,"PG-UP"},
	{SPEC1|6,		next_page,"PG-DOWN"},
#endif
	{SPEC1|2,		paste_region,"INSERT"},
	{SPEC1|0,		toggle_over,"OVERWRITE"},	/* works mainly on X11/gtk  */
	{SPEC1|'P',		prev_word,"^KEY-LEFT"},
	{SPEC1|'N',		next_word,"^KEY-RIGHT"},
	{META|CNTRL|'Y' , system_paste,"SHIFT-INSERT"},
#endif
#if	NOTES
	{META|'N',		notes_menu_command,"ALT-N"},
#endif
	{META|'S',		menu_command,"ALT-S"},
	{META|'F',		menu_command,"ALT-F"},
	{META|'E',		menu_command,"ALT-E"},
	{META|'H',		menu_command,"ALT-H"},
	{META|'M',		menu_command,"ALT-M"},
	{META|'T',		menu_command,"ALT-T"},
	{META|'W',		menu_command,"ALT-W"},

	{SPEC1|12,		set_arg,"F2"},
	{SPEC1|13,		quick_close,"F3"},
	{SPEC1|14,		describe_key,"F4"},
	{SPEC1|15,		find_next,"F5 or ^KEY-DOWN"},
	{SPEC1|17,		find_previous,"F6 or ^KEY-UP"},
	{SPEC1|18,		record_macro,"F7"},
	{SPEC1|19,		exec_macro,"F8"},
	{SPEC1|20,		next_window,"F9"},
	{SPEC1|21,		next_file,"F10"},
	{SPEC1|23,		toggle_sync_lock,"F11"},
	{SPEC1|'I',		copy_region,"ALT-[I"},
	{SPEC1|'W',		cut_region,"ALT-[W"},

	{SPEC2|11,		goto_bof,"^HOME"},
	{SPEC2|9,		goto_eof,"^END"},

	{META|'(',		window_column_decrease,"ALT-("},
	{META|')',		window_column_increase,"ALT-)"},
	{META|'-',		window_row_decrease,"ALT--"},
	{META|'_',		window_row_increase,"ALT-_"},

#if	!DARWIN
	{SPEC1|'z',		next_file,"^PG-DOWN"},
#endif
	{SPEC1|'w',		next_window,"^PG-UP"},
	{SPEC1|'c',		select_next_character,"SHIFT rigt"},
	{SPEC1|'b',		select_next_line,"SHIFT down"},
	{SPEC1|'d',		select_prev_character,"SHIFT left"},
	{SPEC1|'a',		select_prev_line,"SHIFT up"},
	{SPEC1|'n',		next_hmark,"ALT right arrow"},
	{SPEC1|'p',		prev_hmark,"ALT left arrow"},
	{SPEC1|'Z',		remove_tab,"Shift TAB"},

	{SPEC|'a',		toggle_move_lock,"F12"},
	{SPEC|'P',		show_info,"F1"},
	{0,			NULL,NULL}
};

KEYTAB  keytab_win[MAXASSIGNS] = {
	{CNTRL|'A',		goto_bol,"^A"},
	{CNTRL|'B',		select_file,"^B"},
	{CNTRL|'C',		copy_region,"^C"},
	{CNTRL|'D',		delete_line,"^D"},
  	{CNTRL|'E',		goto_eol,"^E"},
	{CNTRL|'F',		forwsearch,"^F"},
	{CNTRL|'G',		abort_cmd,"^G"},
	{CNTRL|'H',		del_prev_char,"^H"},
	{CNTRL|'I',		tab,"tab"},
	{CNTRL|'J',		insert_indent,"^J"},
	{CNTRL|'K',		erase_eol,"^K"},
	{CNTRL|'L',		reposition,"^L"},
	{CNTRL|'M',		new_line,"^M"},
	{CNTRL|'N',		next_file,"^N"},
	{CNTRL|'O',		open_file,"^O"},
	{CNTRL|'P',		next_window,"^P"},
	{CNTRL|'Q',		quote,"^Q"},
   	{CNTRL|'R',		backsearch,"^R"},
	{CNTRL|'S',		save_file,"^S"},
	{CNTRL|'T',		twiddle,"^T"}, 
	{CNTRL|'U',		prev_hmark,"^U"},
	{CNTRL|'V',		paste_region,"^V"},
	{CNTRL|'W',		work_menu,"^W"},
	{CNTRL|'X',		cut_region,"^X"},	
	{CNTRL|'Y',		redo,"^Y"},
	{CNTRL|'Z',		undo,"^Z"},
	{CNTRL|']',		match_fence,"^]"},
	{CNTRL|'\\',	goto_eow,"^\\"},
	{CNTRL|'@',		set_mark,"^SPACE"},
#if	!PCURSES
 	{CNTRL|'_',		decrease_font_size,"^_"},
	{CNTRL|'^',		increase_font_size,"^^"},
#endif
// text mouse functions in panel_curses
	{KMOUS|'a',		scroll_up,"SCROLL-UP"},
	{KMOUS|'`',		scroll_down,"SCROLL-DOWN"},

	{KMOUS|'C',		text_mouse_pos,"MOUSE_POS"},
	{KMOUS|' ',		text_mouse_left_press,"MOUSE_LEFT"},
	{KMOUS|35,		text_mouse_release,"MOUSE_RELEASE"},
	{KMOUS|34,		text_mouse_right_press,"MOUSE_RIGHT"},
	{KMOUS,			text_mouse_error,"mouse error"},

	{META|' ',		set_arg,"ALT space"},
	{META|'.',		find_tag,"ALT-."},		// find tag
	{META|'@',		view_functions,"ALT-@"},
	{META|'#',		view_functions,"ALT-#"},
	{META|'1',		shell_cmd,"ALT-1"},
	{META|'2',		script_exec,"ALT-2"},
	{META|'3',		filter_buffer,"ALT-3"},
	{META|'4',		macro_line,"ALT-4"},
	{META|'5',		grep_cmd,"ALT-5"},
	{META|'%',		rgrep_cmd,"ALT-%"},
	/* alternate bookmark funtions */
	{META|'0',		clear_bmarks,"ALT-0"},
	{META|'9',		select_bmark,"ALT-9"},
	{META|'8',		set_bmark,"ALT-8"},
	{META|'7',		next_bmark,"ALT-7"},
	{META|'6',		prev_bmark,"ALT-6"},
	{META|'|',		prev_word,"ALT-|"},
	{META|'\\',		goto_eow,"ALT-\\"},

	{META|'A',		menufile,"ALT-A"},
//	B ??
	{META|'B',		swap_mark,"Alt-B"},
	{META|'C',		set_column_mark,"ALT-C"},
	{META|'D',		delete_word,"ALT-D"},
//	E-> menu edit
//	F-> menu file
	{META|'G',		goto_line,"ALT-G"},
//	H-> menu help
	{META|'I',		insert_tab,"ALT-I"},
	{META|'J',		select_hmark,"ALT-J"},
	{META|'K',		assign_function,"ALT-K"},
	{META|'L',		set_line_mark,"ALT-L"},
	{META|'N',		next_hmark,"ALT-N"},
	{META|'o',		select_file_internal,"ALT-O"},
	{META|'P',		prev_hmark,"ALT-P"},
	{META|'Q',		fillpara,"ALT-Q"},
	{META|'R',		sreplace,"ALT-R"},
//	s -> search menu
//	t -> tools menu
	{META|'V',		system_paste,"ALT-V"},
//	w --> menu window
	{META|'X',		quit,"ALT-X"},
	{META|'Y',		toggle_mouse,"ALT-Y"},

 	{META|'Z',		set_bg,"ALT-Z"},
	{META|'>',		goto_eof,"ALT->"},
	{META|'<',		goto_bof,"ALT-<"},
	{META|'{',		both_up,"ALT {"},
	{META|'}',		both_down,"ALT }"},
	{META|'=',		refresh_current_line,"ALT-="},
	{META|'+',		refresh_current_buffer,"ALT-+"},

	{META|'&',		read_colors,"ALT-&"},
	{META|'*',		save_colors,"ALT-*"},

	{META|'(',		window_column_decrease,"ALT-("},
	{META|')',		window_column_increase,"ALT-)"},
	{META|'-',		window_row_decrease,"ALT--"},
	{META|'_',		window_row_increase,"ALT-_"},
	{META|'\'',		comment_line,"ALT-'"},
	{META|';',		comment_line2,"ALT-;"},

	{META|CNTRL|'C',		countwords,"ALT-^C"},
	{META|CNTRL|'F',		linefill,"ALT-^F"},
	{META|CNTRL|'H',		delbword,"ALT-^H"},
	{META|CNTRL|'I',		remove_tab,"ALT-^I"},	// shift tab
	{META|CNTRL|'K',		assign_function,"ALT-^K"},
	{META|CNTRL|'U',		unassign_key,"ALT-^U"},
	{META|CNTRL|'L',		toggle_sync_lock,"ALT-^L"},
	{META|CNTRL|'M',		toggle_move_lock,"ALT-^M"},
	{META|CNTRL|'N',		replace_next,"ALT-^N"}, 
	{META|CNTRL|'O',		toggle_over,"ALT-^O"},
	{META|CNTRL|'P',		hard_reposition,"ALT-^P"},
	{META|CNTRL|'R',		qreplace,"ALT-^R"},
	{META|CNTRL|'S',		set_option,"ALT-^S"},
	{META|CNTRL|'X',		macro_line,"ALT-^X"},
	{META|CNTRL|'Z',		set_bg,"Alt-^Z"},
	{META|CNTRL|CHR_LBRA,		abort_cmd,"Escape"},

#if	DARWIN0
   {0x7F,          del_prev_char,"DEL"},
#else
	{0x7F,			del_char,"DEL"},
#endif

#if	1
	{SPEC1|'A',		prev_line,"KEY-UP"},
	{SPEC1|'D',		prev_character,"KEY-LEFT"},
	{SPEC1|'C',		next_character,"KEY-RIGHT"},
	{SPEC1|'B',		next_line,"KEY-DOWN"},

	{SPEC1|5,		page_move_down,"PG-UP"},
	{SPEC1|6,		page_move_up,"PG-DOWN"},
#else
	{SPEC1|5,		prev_page,"PG-UP"},
	{SPEC1|6,		next_page,"PG-DOWN"},
#endif
	{SPEC1|2,		paste_region,"INSERT"},
	{SPEC1|0,		toggle_over,"INSERT"},
	{SPEC1|'P',		prev_word,"^KEY-LEFT"},
	{SPEC1|'N',		next_word,"^KEY-RIGHT"},
	{META|CNTRL|'Y' , system_paste,"SHIFT-INSERT"},

	{META|'S',		menu_command,"ALT-S"},
	{META|'F',		menu_command,"ALT-F"},
	{META|'E',		menu_command,"ALT-E"},
	{META|'H',		menu_command,"ALT-H"},
	{META|'M',		menu_command,"ALT-M"},
	{META|'T',		menu_command,"ALT-T"},
	{META|'W',		menu_command,"ALT-W"},

	{SPEC1|12,		set_arg,"F2"},
	{SPEC1|13,		quick_close,"F3"},
	{SPEC1|14,		describe_key,"F4"},
	{SPEC1|15,		find_next,"F5 or ^KEY-DOWN"},
	{SPEC1|17,		find_previous,"F6 or ^KEY-UP"},
	{SPEC1|18,		record_macro,"F7"},
	{SPEC1|19,		exec_macro,"F8"},
	{SPEC1|20,		next_window,"F9"},
	{SPEC1|21,		next_file,"F10"},
	{SPEC1|23,		toggle_sync_lock,"F11"},
	{SPEC1|'I',		copy_region,"ALT-[I"},
	{SPEC1|'W',		cut_region,"ALT-[W"},
#if	TNOTES
	{SPEC2|179,		show_tag_view,"shift F1"},
	{SPEC2|180,		show_calendar_list,"shift F2"},

	{SPEC2|181,		new_todo_note,"shift F3"},
	{SPEC2|182,		new_note,"shift F4"},
	{SPEC2|57,		new_cal_note,"shift F5"},
	{SPEC2|1,		new_todo_note,"shift F6"},
#endif
	{SPEC2|267,		goto_bof,"^HOME"},
	{SPEC2|265,		goto_eof,"^END"},
	{SPEC2|0x2B1,	next_window,"^PGUP"},

#if	!DARWIN
	{SPEC1|'z',		next_file,"^PG-DOWN"},
#endif
	{SPEC1|'w',		next_window,"^PG-UP"},
	{SPEC1|'c',		select_next_character,"SHIFT rigt"},
	{SPEC1|'b',		select_next_line,"SHIFT down"},
	{SPEC1|'d',		select_prev_character,"SHIFT left"},
	{SPEC1|'a',		select_prev_line,"SHIFT up"},
	{SPEC1|'n',		next_hmark,"ALT right arrow"},
	{SPEC1|'p',		prev_hmark,"ALT left arrow"},
	{SPEC1|'Z',		remove_tab,"Shift TAB"},

	{SPEC|'a',		toggle_move_lock,"F12"},
	{SPEC|'P',		show_info,"F1"},
	{0,			NULL,NULL}
};

KEYTAB keytab_dir[] = {
	{CNTRL|'M',	dir_exec,"^M"},
	{'e',		dir_edit,"e"},
	{'c',		dir_copy,"c"},
	{'l',		dir_link,"l"},
	{'m',		dir_move,"m"},
	{'n',		dir_file_rename,"n"},
	{'f',		dir_new_file,"f"},
	{'d',		dir_new_dir,"d"},
	{CNTRL|'D',	dir_del1,"^D"},	
	{'b',		dir_left,"b"},
	{'i',		dir_right,"i"},
	{'j',		dir_showfileinfo,"j"},
	{'r',		dir_reload,"r"},
	{'x',		dir_exec,"x"},
	{'v',		dir_view,"v"},
	{'s',		script_exec,"s"},

	{SPEC1|'C',	dir_right,"Arrow right"},
	{SPEC1|'D',	dir_left,"Arrow left"},
	{SPEC|'C',	dir_right,"spec c"},
	{SPEC|'D',	dir_left,"spec d"},
	{' ',		dir_tag,"space"},
	{CNTRL|'I',	next_window,"tab"},
	{CHR_LBRA,		prev_page,"["},
	{CHR_RBRA,		next_page,"]"},
	{0,		NULL,NULL}
};

KEYTAB keytab_view[] = {
/* simulate some vi keys in view mode */
	{'b',		prev_word,"b"},
	{'e',		goto_eow,"e"},
	{'0',		goto_bol,"0"},
  	{'$',		goto_eol,"$"},

	{'h',		prev_character,"h"},
	{'l',		next_character,"l"},
	{' ',		next_character,"l"},
	{'i',		dir_edit,"i"},
	{'k',		prev_line,"k"},
	{'j',		next_line,"j"},
	{'n',		find_next,"n"},
	{'N',		find_previous,"N"},
	{'w',		next_word,"b"},
	{'/',		forwsearch,"/"},
   	{'?',		backsearch,"?"},
	{'G',		goto_eof,"G"},
/* and some not so vi compatible  */
	{'1',		goto_bof,"1"},
	{'v',		quick_close,"v"},
	{SPEC1|'D', dir_left,"KEY-LEFT"},
	{'q',		close_file,"q"},
	{CHR_LBRA,		goto_bop,"["},
	{CHR_RBRA,		goto_eop,"]"},
	{',',		view_previous,","},
	{'.',		view_next,"."},
	{0,		NULL,NULL}
};


#if	TNOTES
KEYTAB keytab_notes[] = {
	{' ',		toggle_tag," "},
	{'+',		select_tag,"+"},
	{'=',		select_tag,"="},
	{'s',		select_tag,"s"},
	{'t',		toggle_tag,"t"},
	{'u',		unselect_tag,"u"},
	{'-',		remove_tags,"-"},
	{'*',		remove_tags,"*"},
	{'r',		remove_tags,"r"},
	{CNTRL|'I',	toggle_tagnote,"tab"},
	{CNTRL|'D',	delete_tagnote,"del"},
	{'e',		edit_note,"e"},
	{'v',		view_note,"v"},
	{SPEC1|'C', view_note,"KEY-RIGHT"},

	{'k',		prev_line,"k"},
	{'j',		next_line,"j"},

	{'n',		find_next,"n"},
	{'N',		find_previous,"N"},

	{'/',		forwsearch,"/"},
   	{'?',		backsearch,"?"},
	{'G',		goto_eof,"G"},

	{'1',		goto_bof,"1"},
	{'q',		close_file,"q"},
	{CHR_LBRA,		prev_page,"["},
	{CHR_RBRA,		next_page,"]"},
	{0,		NULL,NULL}
};
#endif
/* Not yet bounded keys
	^X(Q, E, Y, A, H, K, 456789, -, +)
	^X({,}, ;, :, ', ", ,, .,<,>,/, ?, \, |)


	ESC( Y, ], ;,  ", /, ?, ~) (do not allow shifted characters)
	ESC-^(Q,W,T, U, I, O, P, A, S, E, D, F, G, J, L,Z,V,B,N, M)
*/
/*
 Other keys
!!	^XG			update tagx file
!!	M-.			-> find_tag (jed, emacs M-.)
!!	M-'			-> add/delete c++ comment
!!	M-;			-> add/edit comment at end of line (jed)
!!	M-/			-> dabbrev (abbreviation jed)
!!				-> set bookmark
!!				-> goto bookmark
				-> mark function
*/

#if	COMMENT
// keymap editor compatibility !
# 		 ge2					brief					gedit				kate		emacs
 ALT-A = user menu file
 ALT-B = 						buffer list				??					??
 ALT-C = column mark			same					??					??
 ALT-D = delete next word		delete line				??					??
 ALT-E = edit menu				edit file				??					??
 ALT-I = insert tab				insert/overwrite mode	??					??
 ALT-K = assign key				kill to the end of line	??					??
 ALT-L = line mark 				same					??					??			region lower
 ALT-M = menu mode				mark/unmark				??					??
 ALT-G = goto line				same
 ALT-H = menu help				help					menu help			menu help
 ALT-N = next buffer			cannot  (M-N is used bu Unixware) can be done!
 ALT-P = previous hmark
 ALT-N = next mark				this is a special character on SCO
 ALT-Q = fill paragraph									quit				quit
 ALT-R = replace string
 ALT-T = tools menu				replace
 ALT-S = search menu
 ALT-- = sub test1				previous buffer
 ALT-U = word upper
 ALT-V = 
 ALT-W = window menu									quit				quit
 ALT-X = quit																			quit
 ALT-Y = 
 ALT-Z = redo
 
 F1 = show info		
 F2 = open file
 F3 = quick close
 F4 = describe key
 F5 = find next
 F6 = find previous
 F7 = macro on/off				YES
 F8 = play macro				YES
 F9 = next window
 F10 = menu
 F11 = toggle_sync_lock
 F12 = toggle move lock

 grey+ = copy to scrap			->yes? can be done!
 INS   = insert scrap				OK
 Alt-F5 = backward search		cannot 
 alt-f6 = backward replace		cannot

 ctrl-F5 = case sensitivity toggle
 ctrl-F6 = regular expression toggle

#       colibte                        	gedit               kate            	emacs
 ^A =   bol                            	select all          select all      	bol
 ^B =   select file                    	-                                   	back character
 ^C =   copy                           	copy                copy
 ^D =   delete line                    	delete line         -               	delete character
 ^E =   eol                            	-                   -               	eol
 ^F =   next char                      	find                find            	next char
 ^G =   abort                          	find next           goto line       	abort
S^G =									find previous
 ^H =   del_prev_char                  	replace             mark & find     	help or del_prev_char
 ^I =   tab                            	goto line           -               	tab
 ^J =   insert_ident                   	-                   -               	insert ident
 ^K =   erase_eol                      	incremental search  -               	erase_eol
 ^L =   redraw disp                    	open location       save all        	redraw disp
S^L =                                                      split vertical
 ^M =   new line                       	-                   -           
 ^N =   next line                      	new file            new file        	next line
 ^O =   open file                      	open file           open file       	open line
 ^P =   both_down                      	print               print           	previous line
 ^Q =   quote character                	quit                quit            	>>
 ^R =   search_reverse                 	-                   replace         	>>
S^R =                                  	                    close window
 ^S =   search                         	save                save            	i-search
 ^T =   twidle                         	-                   -               	twidle
S^T =                                                      split horizontal
 ^U =   both up                        	-                   -               	universal argument
 ^V =   past                           	paste               paste           	next page
 ^W =   cut_region                     	close               close           	cut_region
 ^X =   start ^X                       	cut_region          cut_region      	start ^X
 ^Y =                                  	-                   -               	yank
 ^Z =   undo                           	undo                undo				scroll up
S^Z = 									redo				redo
 ^Space	mark on/off						-					code completion		mark on/off
 ^_ = 																			undo
----------------------------------------------------------------------------------------------------
# 		colibte          		brief					gedit				kate		emacs
 ^X^@
 ^X^A =	save session*
 ^x^b = load session*
 ^x^c = quit																			save quit
 ^x^d = detab line*M																	dir list
 ^x^e = entab line*M																	eval-last-sexp
 ^x^f = select font*M																	find file
 ^x^g = 
 ^x^h =																					key help
 ^x^i = remove tab																		ident-rigidly
 ^x^j = 
 ^x^K = close file	*M																	kmacro-keymap
 ^x^l = region lower*M																	downcase-region
 ^X^M = toggle move lock*F12															prefix-command
 ^X^N = 																				set goal column
 ^X^O = del blank lines
 ^X^P = 																				mark page (bof?)
 ^X^Q = 																				toggle read only
 ^X^R = replace next																	find file read only
 ^X^S = save file																		save buffer
 ^X^T = trim line																		transpose lines
 ^X^U = region upper																	upcase region
 ^X^V = show session																	find alternate file
 ^X^W = save as																			write file
 ^X^X = swap mark																		swap mark
 ^X^Y = 
 ^X^Z = 																				iconify-deiconify-frame


# 		colibte         		brief					gedit				kate		emacs
 ^X$																					set selective display
 ^X''																					expand abbrev
 ^X(																					start macro
 ^X)																					end macro
 ^X*																					calc-dispatch
 ^X+																					balance windows
 ^X-																					shrink window
 ^X0	delete window																	delete window
 ^X1	one window																		one window
 ^X2	split window vertically															>>
 ^X3	split window horizontally														>>
 ^X;																					comment-set-column
 ^X<																					scroll left
 ^X=																					show cursor position
 ^X>																					scroll right
 ^X[																					backward page
 ^X]																					forward page
 ^X^																					enlarge window
 ^X`																					next error
 ^XA 	
 ^XB	toggle bold																		switch to buffer
 ^XC
 ^XD	 list dir																		dired
 ^XE																					kmacrot-end-and-call-macro
 ^XF																					set fill column
 ^XH																					mark-whole-buffer
 ^XG	update tags
 ^XH
 ^XI	toggle over																		insert file
 ^XK	close file																		kill buffer
 ^XL	toggle sync lock                       											count lines page
 ^XM																					compose mail
 ^XN	rename file																		rename file
 ^XO																					other window
 ^XP	previous window
 ^XQ																					kbd-macro-query
 ^XR	resize window(text mode)
 ^XS	set option																		save-some-buffers
 ^XT	set bmark
 ^XU																					advertised-undo
 ^XV
 ^XW
 ^XX
 ^XY
 ^XZ																					repeat
 ^X{																					shrink window horizontally
 ^X}																					enlarge window horizontally
 ^XDEL																					backward delete

 
#endif

/* -- */
