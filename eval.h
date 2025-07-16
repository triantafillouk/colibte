/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

	macro language option definitions, variable names, operator names, function names
	directive names
*/

int set_arg(num n);
void init_message_loop();
BTNODE *find_bt_element(char *name);
double get_env(int vnum);
int set_option(num n);
short int get_next_key(num mode);
int set_option_val(int vnum,char *svalue);
void set_env(int vnum,char *svalue,double value);
/* find pattern within source */
int sindex(char *source, char *pattern);
char *date_string(num n);
char *time2a();
double modulo(double value,double factor);
void init_common();
double compute_string(char *s,char *new_string);
int docmd(char *cmd);
int macro_line(num nused);
int exec_named_function(char *name);
int execsub(num n);
int exec_file(num n);
int dofile(char *fname);
int execute(int c, int  n);
void main_execute(int c);
int set_play_string(char *st1);

#if	RSESSION
int set_play_string1(char *st1);
void set_record_string1(char *st2);
void record_session_key(short ks);
int execute_session(char *fname);
int load_session(num n);
int show_session(num n);
int save_session(num n);
int save_trace(num n);
#endif

void record_key(short ks);
void set_record_string(char *st2);
int record_macro(num n);
int replay(int mode,int ntimes);
int exec_macro(num ntimes);

int set_locale(num local_codepage);
int create_function_buffer(FILEBUF *pbuf,char *function_name,offs start_function_offset,offs foffset);
int refresh_current_line(num nused);

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
	{"custom_cell_width",0,NULL},
	{"x11_x",10,NULL},
	{"x11_y",10,NULL},
	{"x11_width",500,NULL},
	{"x11_height",300,NULL},
	{NULL,0},
};


/* -- */
