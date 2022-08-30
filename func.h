/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

	Editor function table
*/

//	macro name, args,function, description
FUNCS	ftable[] = {
	{"menu",0,			menu_command,"Menu command"},
	{"work",0,			work_menu,"Work menu"},
	{"show_version",0,	show_version,"Show the current version on status line"},
	{"abort",	0,		abort_cmd,"Abort key"},
	{"color_scheme",1,		change_color_scheme,"Change the color scheme currently used"},
	{"xcolor_scheme",1,		change_color_scheme,"Change the xcolor scheme currently used"},
	{"prev_character",1,	prev_character,"move left"},
	{"record_macro",0,		record_macro,"start and stop a key macro recording"},
	{"bof",			0,		goto_bof,"Go to the start of the file"},
	{"bol",			0,		goto_bol,"Go to the start of the line"},
	{"assign_function",2,	assign_function,"Assign a function to a key"},
	{"assign_sub",	2,		assign_sub,"Assign a macro subroutine to a key"},
	{"execsub",	1,		execsub,"execute a subroutine"},
	{"region_lower",0,		lowerregion,"Lower case the characters of a selected region"},
	{"region_upper",0,		upperregion,"Upper case the characters of a selected region"},
	{"convert_char",1,		convert_char,"Character set conversion"},

	{"insert_tab",	1,		insert_tab,"Insert a tab"},
	{"remove_tab",	1,		remove_tab,"Remove a tab"},
	{"new_file",	0,		new_file,"new file"},
	{"rename_file",	1,		rename_file,"rename current file"},
	{"copy_region",	0,		copy_region,"Copy region to clipboard"},
	{"count_words",	0,		countwords,"Count words of a selected region"},
	{"del_blank_lines",0,		del_blank_lines,"Delete blanc lines before and after cursor, alsways left one blank line" },
	{"close_file",	0,		close_file,"close current file"},
	{"set_bg",0,		set_bg,"suspend"},
	{"del_char",1,			del_char,"delete the character under the cursor"},
	{"del_next_word",1,		delete_word, "delete current word"},
	{"one_window",	0,		one_window,"remove all other windows"},
	{"del_prev_word",1,		delbword,"delete previous word"},
	{"del_prev_char",1,		del_prev_char,"delete previous character"},
	{"delete_window",	0,	delete_window,"delete current window"},
	{"dos2unix",		0,	dos2unix,"convert dos file to unix"},
	{"unix2dos",		0,	unix2dos,"convert unix file to dos"},
	{"set_color",	1,		set_color,"set the background color"},
	{"show_keys",	0,		show_keys,"show the current key assignements"},
	{"color_scheme_read",	1,	color_scheme_read, "color scheme read from file"},
	{"describe_key",0,		describe_key,"Show the function or subroutine assigned to a key"},
	{"macro_line",1,	macro_line, "execute a command line"},
	{"set_arg",			1,	set_arg,"set a numeric argument for next function to execute"},
	{"detab_line", 0,			detab_line,"Convert tabs to spaces"},
	{"line_fill", 0,			linefill,"fill the line, in right align mode insert spaces bettween characters"},
	{"eof",		0,		goto_eof,"Go to the end of file"},
	{"eol",		0,		goto_eol,"Go to the end of line"},
	{"entab_line",0,	entab_line, "Convert spaces to tabs in a line"},
	{"comment_line",0,	comment_line, "Comment line"},
	{"comment_line2",0,	comment_line2, "Comment line end"},
	{"swap_mark", 0,	swap_mark,"swap mark and cursor position"},

	{"exec_file",	1,		exec_file,"execute a macro file"},
	{"exec_macro",	0,		exec_macro,"execute a saved macro (F8)"},
	{"parse_check", 0, parse_check_current_buffer,"parse_check_current_buffer"},
	{"show_tokens",0, parse_buffer_show_tokens,"show token list of a parsed buffer"},
	{"undo",	0,			undo,"Undo changes"},
	{"redo",	0,			redo,"Redo changes"},
	{"refresh_cline",0,		refresh_current_line,"refresh current line (macro) and show results after :"},
	{"refresh_buffer",0,	refresh_current_buffer,"execute current macro file"},
	{"quit",		0,		quit,"quit the editor"},
	{"fill_paragraph",	0,	fillpara,"fill current paragraph filling lines"},
	{"open_file",		1,	open_file,"Open a file for editing"},
	{"clear_buffer",	1,	clear_buffer,"Clear buffer named"},
	{"unmark_buffer",	0,	umark_buffer,"unmark current buffer"},
	{"next_character",	1,	next_character,"move right"},
	{"select_next_character",	1,	select_next_character,"select and move right"},
	{"select_prev_character",	1,	select_prev_character,"select and move left"},
	{"select_next_line",	1,	select_next_line,"select and move down"},
	{"select_prev_line",	1,	select_prev_line,"select and move up"},
	{"goto_line",		1,	goto_line,"Go to a specific line"},
	{"match_fence",		0,	match_fence,"Find a correspondig fence and go there (C fences)"},
	{"tab",				1,	tab, "insert a tab"},
	{"find_previous",	1,	find_previous,"Find previous occurense of a search"},
	{"find_next",	1,	find_next,"Find next occurense of a search"},
	{"select_font",		1,	select_font,"Select a font"},
	{"set_fontindex",	1,	set_fontindex,"select font from default list"},
	{"set_locale",		1,	set_locale,"select localcodepage"},
	{"set_debug",		1,	set_debug,"set debug flag"},
/* The following are dummy for curses driver */
	{"toggle_bold",		0,  toggle_bold,"togfle bold"},
	{"select_font_mono",0,	select_font_mono,"Select a monospace font"},
	{"increase_font_size",	0,	increase_font_size,"increase font size"},
	{"decrease_font_size",	0,	decrease_font_size,"decrease font size"},
/* The following are dummy for GTK */
	{"window_column_increase",	0,	window_column_increase,"window_column_increase"},
	{"window_column_decrease",	0,	window_column_decrease,"window_column_decrease"},
	{"window_row_increase",		0,	window_row_increase,"window_row_increase"},
	{"window_row_decrease",		0,	window_row_decrease,"window_row_decrease"},
/* --- */
	{"toggle_mouse",	0,	toggle_mouse,"toggle mouse integration"},
	{"help",	0,		help,"Show help text"},
	{"exec",	1,		shell_cmd,"Execute a shell command, show output in [command] buffer"},
	{"bg_cmd",	1,		bg_cmd,"Execute a background command, output in /tmp/cmd### file, error in /tmp/err### file"},
	{"new_shell",	0,	new_shell,"start interactive shell (xterm like)"},
	{"rgrep_command",	1,	rgrep_cmd,"Recursive find insize current directory"},
	{"grep_command",	1,	grep_cmd,"Gr a word and show results in a new window"},
	{"script_exec",		1,	script_exec,"script exec buffer"},
	{"filter_buffer",	1,	filter_buffer,"filter current file thought an external command"},
	{"insert_file",1,	insert_file,"insert a file at current position"},
	{"insert",		1,	istring,"insert a string to current position"},
	{"erase_eol",	0,		erase_eol,"remove characters till the end of line"},
	{"select_file",	1,		select_file, "Select an edited file to display. If interactive show a list first"},	
	{"move_window",	1,	move_window,"Move window"},
	{"newline",		1,	new_line,"Insert a new line"},
	{"insert_indent",	1,	insert_indent,"insert indent"},
	{"next_file",		0,	next_file,"show next file in current window"},
	{"next_line",		1,	next_line,"goto next line"},
	{"scroll_up",		1,	scroll_up,"move text up"},
	{"scroll_down",		1,	scroll_down,"move_text_down"},
	{"delete_line",		1,	delete_line,"Delete current line"},
	{"next_paragraph",	1,	goto_eop,"Go to end of paragraph"},
	{"next_page",		1,	next_page,"Goto next page"},
	{"next_word",		1,	next_word,"move to next word"},
	{"next_window",		0,	next_window,"Go to next window"},
	{"goto_eow",		1,	goto_eow,"move to end of word"},
	{"prev_page",		1,	prev_page,"Move on page backwards"},
	{"prev_line",		1,	prev_line,"move up"},
	{"prev_paragraph",	1,	goto_bop,"Move to the begin of previous paragraph"},
	{"prev_word",		1,	prev_word,"Move to previous word"},
	{"prev_window",		0,	prev_window,"Move to previous window"},
	{"query_replace",	2,	qreplace,"Interacive replace"},	/* only in interactive mode  */
	{"quick_close",		0,	quick_close,"Save file and close it"},
	{"quote_character",	0,	quote,"Insert control character at current position"},
	{"redraw_display",	0,	reposition,"Redrawy display"},
	{"hard_reposition",	0,	hard_reposition,"Redraw display reseting textpoints"},
	{"window_row_resize",	1,	window_row_resize,"Resize window rows"},
	{"window_column_resize",	1,	window_column_resize,"Resize window cols"},
	{"replace_string",	2,	sreplace,"Replace a string"},
	{"replace_next",	1,	replace_next,"replace next ??"},
	{"save_file",		0,	save_file,"Save the file"},
	{"both_down",	1,		both_down,"Scroll files in both windows down"},
	{"both_up",		1,	both_up,"Scroll files in both windows up"},
	{"search_forward",1,		forwsearch,"Ask for a word and search forward"},
	{"search_reverse",1,		backsearch,"Ask for a word and search backward"},

	{"save_colors",0		, save_colors,"Save colors"},
	{"read_colors",0		, read_colors,"read colors"},

	{"set_key",	1,	set_key,"Set the encryption key"},
#if	TNOTES
	{"set_notes_key",	1,	set_notes_key,"Set the encryption key"},
#endif
	{"set_mark",0,			set_mark,"set normal mark at current position"},
	{"set_no_mark",0,			set_no_mark,"remove selection marks"},
	{"set_line_mark",0,			set_line_mark,"set line mark at current position"},
	{"set_column_mark",0,			set_column_mark,"set column mark at current position"},
	{"split_window", 0,		split_window,"Split current window horizontally"},
	{"vsplit_window",0,		vsplit_window,"Split current window vertically"},
	{"system_paste",		0,	system_paste,"Past the system clipboard contents in current position"},
	{"twiddle",	0,			twiddle,"Replace the two characters left of cursor"},
	{"trim_line",	0,		trim_line,"Remove trailing spaces of a line"},
	{"unassign_key",1,		unassign_key,"Unassign a function or subroutine from a key"},	/* only in interactive mode  */
	{"saveas_file",	1,		saveas_file,"Save the file as another name"},
	{"paste_region",	0,	paste_region,"Past the clipboard contents in current position"},
	{"cut_region",		0,	cut_region,"Cut the region and put it in clipboard"},
	{"update_screen",	1,  update_screen,"update_screen"},

	{"update_tags",	0,		update_tags,"Update C-Tags"},
	{"find_tag",	0,		find_tag,"Find the function under cursor in other files of current dir"},
	{"view_functions",0,	view_functions,"view functions"},
	/* bookmark functions */
	{"prev_bmark",	0,		prev_bmark,"Go to previous bookmark"},
	{"next_bmark",	0,		next_bmark,"Go to next bookmark"},
	{"select_bmark",1,		select_bmark,"Select a bookark to go to"},
	{"set_bmark",	0,		set_bmark,"Bookmark current position"},
	{"clear_bmark",	0,		clear_bmarks,"Clear all bookmarks"},
	{"prev_hmark",	0,		prev_hmark,"Go to previous histoty mark"},
	{"next_hmark",	0,		next_hmark,"Goto next history mark"},
	{"select_hmark",	0,		select_hmark,"Select history mark"},
#if	XPLOT
	{"plot_init",	1,		plot_init,"Initialize a plot window"},

	{"path",	1,		path_start,"init path"},
	{"pmove",	2,		plot_move_to,"path move to"},
	{"pline",	2,		plot_lineto,"path line"},
	{"pcurve",	4,		plot_curve_to,"path curve"},
	{"pend",	0,		path_end,"end path"},
	{"close_path",	0,	plot_close_path,"set close path flag"},

	{"plot_close",	0,		plot_close,"Close the plot window"},

	{"title",	1,		plot_title,"Set a title for the plot window"},
	{"point",	2,		plot_point,"draw a point"},
	{"move_to",	2,		plot_move_to,"move to"},
	{"line_to",	2,		plot_lineto,"draw a line to a point"},
	{"line",	4,		plot_line,"draw a line"},
	{"circle",	3,		plot_circle,"draw a circle"},
	{"curve_to",6,		plot_curve_to,"draw curve"},
	{"rcircle",1,		plot_rcircle,"draw relative cirle"},
	{"elipsep",3,		plot_elipsep,"draw elipse path"},
	{"arc",	3,		plot_arc,"draw relative arc"},
	{"rectangle",	2,	plot_rectangle,"draw relative rectangle"},
	{"triangle",	4,	plot_triangle,"draw relative triangle"},
	{"polyline",	-1,	plot_polyline,"draw polyline"},
	{"poly_nostroke",	0,	plot_poly_nostroke,"poly is only filled!"},
	{"make_solid",	0,	plot_poly_nostroke,"poly is only filled!"},
	{"plot_stat",	0,		plot_stat,"show info??"},
	{"plot_zoom",	1,		plot_zoom,"zoom plot"},

	{"plot_set_color",	4,		plot_set_color,"set color type"},
	{"stroke_color",	3,		plot_color,"set current drawing color"},
	{"fill_color",	4,		plot_fcolor,"set current drawing color"},
	{"plot_coord",	4,		plot_coord,"set ploting coordinates"},
	{"line_width",	1,		plot_lwidth,"set width of line"},
	{"stroke_width",	1,		plot_lwidth,"set width stroke"},
	{"plot_resize",	2,		plot_resize,"resize plot window"},
	{"image",	3,		plot_image,"plot image, scale , rotate"},
	{"text",	1,		plot_text,"plot text"},
	{"relative",0,		set_relative,"relative"},
	{"absolute",0,		set_absolute,"absolute"},
	{"set_alpha",1,		plot_alpha,"set plot transparency"},
	{"alpha",1,		plot_alpha,"set plot transparency"},
	{"set_rotate",1,		plot_rotate,"cairo rotate"},
	{"rotate",1,		plot_rotate,"cairo rotate"},
	{"set_translate",2,	plot_translate,"cairo_translate"},
	{"translate",2,	plot_translate,"cairo_translate"},
	{"show_frames",1, plot_frames,"show frames"},
	{"scale",2,		plot_scale,"cairo scale"},
#endif
	{"set_option",	2,		set_option,"set option. If interactive select option from a list"},
	{"toggle_move_lock",0,	toggle_move_lock,"toggle movement lock"},
	{"toggle_sync_lock",0,	toggle_sync_lock,"toggle page movement lock"},
	{"page_move_up",1,		page_move_up,"page move up"},
	{"page_move_down",1,	page_move_down,"page move down"},
	{"set_over",	0,		set_over,"set/unset overwrite"},
	{"set_case",	0,		set_case,"set case sensitive search flag"},
	{"set_regexp",	0,		set_regexp,"set regular expression search flag"},
	{"toggle_over",	0,		toggle_over,"toggle overwrite"},
#if	TNOTES
	{"show_tag_view",0,		show_tag_view,"show tag view"},
	{"show_calendar_list",0,show_calendar_list,"show calendar list"},
	{"show_todo_list",0,show_todo_list,"show todo list"},
	{"new_note",1		,	new_note,"new note"},
	{"new_cal_note",0	,	new_cal_note,"new calendar note"},
	{"new_todo_note",0	,	new_todo_note,"new todo note"},
#endif
#if	0
	{"toggle_case",	0,		toggle_case,"change case sensitive search flag"},
	{"toggle_regexp",0,		toggle_regexp,"change regular expression search flag"},
#endif
	{"set_crypt",	0,		set_crypt,"set encryption mode"},
	{"set_justify",	0,		set_just,"set right justify mode"},
	{"toggle_crypt",	0,		toggle_crypt,"change encryption mode"},
	{"toggle_just",	0,		toggle_just,"change right justify mode"},
	{"menufile",	1,		menufile,"Show a menu from file and select a function to execute, also execute n'th element in macro mode"},
	{"listdir1",	0,		listdir1,"Show current dir"},
	{"listdir2",	0,		listdir2,"Show new dir"},
	{"dir_exec",	0,		dir_exec,"Execute current file (dir mode)"},
	{"dir_edit",	0,		dir_edit,"Edit current file (dir mode)"},
	{"dir_copy",	1,		dir_copy,"Copy current file (dir mode)"},
	{"dir_move",	1,		dir_move,"Move current file (dir mode)"},
	{"dir_rename",	1,		dir_file_rename,"Rename current file (dir mode)"},
	{"dir_del1",	0,		dir_del1,"Delete current file (dir mode)"},
	{"dir_view",	0,		dir_view,"View current file (dir mode)"},
	{"mkdir",		1,		dir_new_dir,"Create new dir (dir mode)"},
	{"touch",		1,		dir_new_file,"create new file (dir mode)"},
	{"link",		1,		dir_link,"symbolic link (dir mode)"},
	{"dir_right",	0,		dir_right,"Dir right: view file on enter directory"},
	{"dir_left",	0,		dir_left,"Dir left: exit viewing file or goto to parent directory"},
	{"view_next",	0,		view_next,"view next file"},
	{"view_previous",	0,		view_previous,"view previous file"},
	{"dir_tag",		0,		dir_tag,"Tag current file (dir mode)"},
	{"dir_reload",	0,		dir_reload,"refresh dir (dir mode)"},
	{"in_view",		1,		buffer_in_view,"buffer in view"},
	{"dir_showfileinfo",0,	dir_showfileinfo,"Show file info (dir mode)"},
	{"show_info",	0,		show_info,"Show info"},
#if	RSESSION
	{"load_session",1,		load_session,"load, replay session keys"},
	{"save_session",0,		save_session,"save session keys"},
	{"show_session",0,		show_session,"show session keys"},
#endif
//	{"show_tree",0,		show_bt_table,""},
#if	TNOTES
	{"select_tag",0,	select_tag,""},
	{"toggle_tagnote",0,	toggle_tagnote,""},
	{"edit_note",0,		edit_note,""},
	{"view_note",0,		view_note,""},
	{"toggle_tag",0,		toggle_tag,""},
	{"unselect_tag",0,	unselect_tag,""},
	{"remove_tags",0,	remove_tags,""},
	{"delete_tagnote",0,delete_tagnote,""},
	{"save_as_note",0, save_as_note,""},
	{"recreate_notes_db",1,recreate_notes_db,""},
#endif
	{"do_nothing",0,	do_nothing,"do nothing"},
	{NULL,	0,		NULL}
};
