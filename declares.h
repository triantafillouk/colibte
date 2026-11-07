/*
	Curses,gtk editor,notes,directory browser
	Global variables, function declarations
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.
*/
// file.c
void edinit(char *bname);
int init_ftype(FILEBUF *bf,char *file_name,int *temp_used,int from_note);
void update_file_status(FILEBUF *fp);
int menufile(num n);
int file_type(char *name, int *tc, char *oext);
int writeu1(char *fname, FILEBUF *fp);
int buffer_is_connected(FILEBUF *bf);
int goto_file(char *file_name);
int activate_file(FILEBUF *bp);
int select_filebuf(FILEBUF *bp);
void link_window_buffer(WINDP *wp,FILEBUF *fp);
void unlink_filebuf(WINDP *);
int delete_filebuf(FILEBUF *bp,int force);
int empty_filebuf(FILEBUF *bp);
int list_filebuf(char *title, char *st,int f1,int *return_flag);
int changes_occured();
FILEBUF * new_filebuf(char *bname,int bflag);
FILEBUF * get_filebuf(char *bname,char *dname,int flag);int set_buf_key(FILEBUF *curbp);
void crypt_string(char *ctr,num len);
FILEBUF * cls_fout(char *bname);
int sfb(FILEBUF *bp);
void save_scratch_files();
int load_scratch_files();
int is_scratch_buffer(FILEBUF *);
char *get_buf_full_name(FILEBUF *fp);
int open_file_named(char *fname);
int add_to_recent_list(char *full_file_name);
int resetkey(FILEBUF *bp);
int edit_file(char *fname);
int file_read(FILEBUF *bp, char *fname);
int file_read1(FILEBUF *bp, char *fname);
void set_bfname(char *bname, char *fname);

// edit.c
int is_wrap_text(FILEBUF *fp);
int dont_edit();
ClipBoard *clipboard_new();
void init_clipboard();
int insert_string(FILEBUF *fp,char *c,int size);
unsigned char *str2out(char *s);

// screen.c
void set_update(WINDP *wp_toset, int flag);
int check_cursor_position(WINDP *wp);
void msg_line(char *fmt, ...);
int show_key(int key);
num getcline();
int show_position_info(num short_version);
void draw_window(int,WINDP *wp,char *from);
#if	!USE_GLIB
char *lstr_to_upper(char *s);
char *lstr_to_lower(char *s);
#endif
int error_line(char *,...);
void upd_all_virtual_lines(WINDP *wp,char *from);
void upd_column_pos();
void updgar();
void update_physical_windows();
num vtline(WINDP *wp, offs tp_offs);
void vtmove(WINDP *wp,int row,int col);
void update_status();
int draw_window_line(WINDP *wp, int row);
void vtputc(WINDP *wp, unsigned int c);
void vt1eol(WINDP *wp,int c,int color);
void upd_part(WINDP *wp,char *from);
void update_buffer_windows(FILEBUF *bp,int flag);
void free_virtual_window(WINDP *wp);
num getccolumn();

// filebuf.c
offs   LineBegin(offs ptr);
offs   FNextLine(FILEBUF *fp,offs ptr);
void   MoveLineCol(num line,num col);
void textpoint_free(TextPoint *);
void *emalloc(long,char *);
void efree(void *,char *);
void textpoint_link(TextPoint *,FILEBUF *fp);
void textpoint_unlink(TextPoint *);
void  ResetTextPoints(FILEBUF *bp,int);
num GetCol();
num WGetCol();
num GetLine();
num LastLine();
num FGetLine(FILEBUF *fp);
num FColumn(FILEBUF *fp,offs o);
num Column(offs o);
void textpoint_move(TextPoint *tp,offs o);
void update_lines(FILEBUF *bp);
offs FSize(FILEBUF *);
bool test_b_flag(int flag);
void set_b_flag(int flag);
offs BufferSize();
int   ReadBlock(char *file_name,int fd,offs size,offs *act_read);
void  textpoint_delete(TextPoint *tp);
void set_Char(offs,byte);
int clipboard_copy(ClipBoard *clip);
int clipboard_paste(ClipBoard *clip);
void MESG_time(const char *fmt, ...);
void MESG_time_start(const char *fmt, ...);
int FUtfCharLen(FILEBUF *bp,offs o);
void show_textpoints(char *title,FILEBUF *);
num	tp_col(TextPoint *tp);
offs tp_offset(TextPoint *tp);
num tp_line(TextPoint *tp);
void tp_copy(TextPoint *tp1,TextPoint *tp2);
void textpoint_set(TextPoint *tp,offs o);
TextPoint *new_textpoint_at(FILEBUF *bf,int tp_type,offs o);
void textpoint_set_lc(TextPoint *tp,num, num c);
byte tp_char(TextPoint *);
byte FChar(FILEBUF *fp);
int FEofAt(FILEBUF *,offs);
int EofAt(offs o);
byte Char();
long int utf_value();
long int utf_value_len(int *len);
byte FCharAt(FILEBUF *,offs);
void set_Offset(offs);
void DeleteChar();
int InsertChar(FILEBUF *fp,char ch);
TextPoint *textpoint_o(FILEBUF *p,offs o,int tp_type,int);
void set_end_line(FILEBUF *fp,num line);

// eval.c
void init_message_loop();
void  main_execute(int c);
int set_play_string(char *);
void set_record_string(char *);
void record_key(short);
void init_common();
int set_option_val(int vnum,char *value);
int docmd(char *cmd);
short int get_next_key(num mode);
char *date_string(num n);
int execute_session(char *);
// input.c
char *get_last_in_list(int tlist);
void out_print(char *s,int nl);
void out_fprint(char *bname, char *s,int nl);
int load_keys();
int (*key_function(int c,int f))(num);
int save_keys();
void load_config();
void save_config();
void list_select(char *s,int dire);
void list_select(char *s,int dire);
int list_update(char *s);
void init_input();
char *xe_key_name(int);
int getckey();
int create_function_buffer(FILEBUF *pbuf,char *function_name,offs start_function_offset,offs foffset);
double get_env(int vnum);
int set_option_val(int vnum,char *value);
void set_env(int vnum,char *svalue,double value);
int execute(int c, int  n);
int sindex(char *source, char *pattern);
char *time2a();
double modulo(double value,double factor);
double compute_string(char *s,char *new_string);
// int dofile(char *fname,array_dat *args_array);
int writeout(char *fn,FILEBUF *fp);
int ifile(FILEBUF *bp,char *fname,int insert_flag);
void set_modified(FILEBUF *bp);
int complete(char *buf);
int get1key();
int getcmd();
int isspecialchar(char *st);
int normalize(int);
int capital(int);
int list_on();
void list_off();
void activate_list_mode();
char *function_name(int (*func)(num), char **description);

// window.c
WINDP *dublicate_window(WINDP *wp0);
int sh_outwindow(num n);
void set_window_width(WINDP *wp);
void change_window(WINDP *wp);
int getwline();
void set_windows_color();
void free_window_data(WINDP *wp);
WINDP *find_buffer_window(FILEBUF *fp);
int window_cursor_line(WINDP *wp);
void set_goal_column(int new_column,char *from);
void set_xmark(WINDP *smark_wp, int mouse_col,int mouse_row,int f);
void set_current_window(WINDP *wp,char *);

// xthemes.c
int color_scheme_save();
int color_edit(num n);

// word.c
void select_current_word();
char *getnword();
char *getcurcword();
int inword();
char *getcurfword();

// dir.c
void init_extensions();
void clear_snames(char **,int);
int open_dir(char *dir_name,int type);
int goto_dir(char *dir_name,int dir_num);
int dir_getfile(char *fname,int flag) ;
void view_file(char *fname);
void push_dir(FILEBUF *) ;
void pop_dir(FILEBUF *);
void reinit_dir(FILEBUF *);
int insert_dir(FILEBUF *buf_dir,int retain);
int list_dir(char *d1,FILEBUF *file_buf);
void set_todirname(char *base_name,int dir_num);
int scandir2(char *dirname, struct kdirent ***namelist_a);
void init_names();

// display driver
void drv_post_windows_update();
void update_cursor_position();
void start_interactive(char *);
void end_interactive();
int getstring(char *prompt, char *st1,int maxlen,int disinp);
void hide_cursor(char *from);
void show_cursor(char *);
void hide_cursor_dl();
void show_cursor_dl (int px);
void put_string_dl(char *st);
void status_line(WINDP *wp);
void show_slide(WINDP *wp);
void put_string_statusline(WINDP *wp, char *st, int position);
void expose_window(WINDP *wp);
void vtinit(int argc, char **argp);
int get_utf_length(utfchar *utf_char_str);
void movecursor(int row, int col);
void clear_message_line();
void top_menu(num init);
void put_wtext(WINDP *wp,int row,int maxcol);
void list_dir1(char *st);
int selectl(char *title,char *m[],int nu,int lmax,int sx,int sy,int width,int active);
int disp_list(char **m,int start,int nu,int lmax,int sx,int sy,int maxl,int active);
int confirm(char *title,char *prompt,int always);
void titletext();
void xupdate_box();
void xdab(int y,int b,char *s,int bcolor,int fcolor);
void box_line_print(int y,int start, char *st,int width, int selected,int active);
void remove_box();
void set_1window();
void window_clear(WINDP *wp);
void prepare_converter(int);
int text_mouse_key(int *c);
int mouse_button_in_box(int mouse_button);

// mlang.c
double get_val();
char *get_sval();
void set_sval(char *);
void set_dval(double val);
void set_vdval(double val);
void initialize_call_stack(int initial_size);
int parse_check_current_buffer(num n);
int parse_buffer_show_tokens(num n);
void set_break(char *from);
int nextarg(char *prompt,char *buffer, int size,int disinp);
void init_hash();
int check_rparenthesis();

double expression(char *);
double lexpression();
void init_exec_flags();

// main.c
void parse_command_line(int , char **);
// highlight.c
int highlight_index(char *,int *);
int highlight_type(char *);
void init_highlight_structure(MLQUOTES *hs);
int check_update_highlight(int flag);
int file_type_is(char *extention,int check_type);
offs highlight_restore_state(WINDP *wp,int);
void highlight_save_state(WINDP *wp,int,num);
void fquote_state(offs till_o, offs from_o, WINDP *wp);
int get_highlight(FILEBUF *fp);
void init_highlight();
void set_highlight(FILEBUF *bf,int type);
void update_highlight(WINDP *wp);
void init_line_highlight(WINDP *wp);
void set_selection(int);
int get_selection();

// xthemes.c
void init_default_schemes();
// system.c
char *ext_system_paste_line();
int init_system_clipboard();
int mkdirRecursive(const char *path, mode_t mode);
int sysexec(char *st);

// marks.c
int set_hmark(int f,char *from);
void update_hmark();

// convert.c
int xlate(int,int);

// search.c
int find_tag(num n);
int eq(int x,int y);
int	scanner(char *patrn, int direct, int beg_or_end);
int	readpattern(char *prompt, char *str, int srch);
int	setpattern(char apat[], char *st,int srch);
void savematch(int direction);
void rvstrcpy(char *rvstr, char *str,int maxlen);
int	replaces(int kind, int f,num n);
int delins(int dlength, char *instr);
int lfound(int status,int direction);
offs find_string_inline(char *patrn);
//void set_global(int beg_or_end,int direct);



// panel_curses.c
int select1_font(num n);

#if	XPLOT
// plot.c
void set_plot_timer();
void do_timer_plot();
int transx(double x);
int transy(double y);
void pl_clear();
void resize_plot_subwins();
void plot_status(int f,char *st);
void set_ratio();
void set_coords();
// void show_coords();
void toggle_isomode();
void toggle_gridmode();
void toggle_nummode();
void toggle_zoompriority();
void toggle_zoom();
double dmean(double x,double y);
void plot_redraw();
char *kt_getstr(char *prompt);
void show_values();
void new_plot_list();
double *add_plot_element(int type, int size);
void free_plot_list();
#endif

/* support.c  */
void open_log(char *program_name);
void msg_log(int priority,const char *fmt, ...);
char *find_file(char *subdir,char *fname,int cflag,int create_if_not_found);
int read_list_array(char *file_name, alist *new_list);
int save_list_array(char *file_name, alist *array);
char *get_start_dir();
char *get_pfname(char *dir_name,char *file_name,int len);
int debug_flag();
void increase_debug_flag();
void decrease_debug_flag();
void set_working_dir(char *dir);
char * get_working_dir();
void create_base_name(char *base_name,char *full_name);
char **read_sarray(char *file_name, int *size);
void free_sarray(char **sarray);


#if	!DARWIN
/*
 * '_cups_strlcat()' - Safely concatenate two strings.
 */

size_t  /* O - Length of string */
strlcat(char *dst, 			/* O - Destination string */
        const char *src,	/* I - Source string */
        size_t size);   	/* I - Size of destination string buffer */

/*
 * '_cups_strlcpy()' - Safely copy two strings.
 */

size_t /* O - Length of string */
strlcpy(char *dst,        /* O - Destination string */
        const char *src,  /* I - Source string */
        size_t size);     /* I - Size of destination string buffer */
#endif

#include "utils.h"

/* ---  */