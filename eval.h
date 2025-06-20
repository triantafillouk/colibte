/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

	macro language option definitions, variable names, operator names, function names
	directive names
*/

void set_global_flag(int option_flag,int value);

VAR option_names[] = {
	{"fillcol", 72, NULL},
	{"lmargin", 0, NULL},
	{"tabsize", 4, NULL},
	{"print_precision",6,NULL},
	{"color_scheme",2,NULL},
	{"xcolor_scheme",2,NULL},
	{"show_hex",0,NULL},
	{"keyboard_emulation",1,NULL},
	{"make_backup",0,NULL},
	{"embed_icons",0,NULL},
	{"show_position",1,NULL},
	{"show_vinfo",0,NULL},
	{"show_vmlines",0,NULL},
	{"show_coffset",1,NULL},
	{"show_cdata",1,NULL},
	{"save_history",0,NULL},
	{"large_toolbar_icons",0,NULL},
	{"safe_ops",0,NULL},
	{"use_titlebar",1,NULL},
	{"exact_match",0,NULL},
	{"mode_over",0,NULL},
	{"mode_regexp",0,NULL},
	{"wrap_mode",0,NULL},
	{"slow_display",0,NULL},
	{"x11_x",10,NULL},
	{"x11_y",10,NULL},
	{"x11_width",500,NULL},
	{"x11_height",300,NULL},
	{NULL,0},
};


/* -- */
