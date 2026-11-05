/*
	Curses,gtk editor,notes,directory browser
	Global variables, function declarations
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.
*/

int buffer_is_connected(FILEBUF *bf);
int noop(num n);
void msg_line(char *fmt, ...);
void MESG_time(const char *fmt, ...);
void MESG_time_start(const char *fmt, ...);
// int init_drv_env();
void init_common();
int load_keys();
int save_keys();
int  get_cfg_int(char *label,int default_val);
int show_bt_table();
int show_bt_table_ordered();
int sh_outwindow(num n);
int show_key(int key);
void load_config();
void save_config();
num getcline();
int color_scheme_save();
int get_hex_byte();
void toggle_hex_byte();
void reset_hex_byte();
void select_current_word();
int window_cursor_line(WINDP *wp);
void set_goal_column(int new_column,char *from);
void set_xmark(WINDP *smark_wp, int mouse_col,int mouse_row,int f);
void clear_names();
void clear_snames(char **,int);
int set_option_val(int vnum,char *value);
// int goto_bol(num);
int goto_file(char *file_name);
int open_dir(char *dir_name,int type);
int goto_dir(char *dir_name,int dir_num);
int docmd(char *cmd);
void hide_cursor(char *from);
void show_cursor(char *);
double get_val();
char *get_sval();
void set_sval(char *);
void set_dval(double val);
void set_vdval(double val);
void set_vsval(char *s);
void set_window_width(WINDP *wp);

void initialize_call_stack(int initial_size);

// double next_value();
void status_line(WINDP *wp);
void show_slide(WINDP *wp);
void put_string_statusline(WINDP *wp, char *st, int position);
int show_position_info(num short_version);
void draw_window(int,WINDP *wp,char *from);
void expose_window(WINDP *wp);
void change_window(WINDP *wp);
int (*key_function(int c,int f))(num);
short int get_next_key(num mode);
char *date_string(num n);
void list_select(char *s,int dire);
void parse_command_line(int , char **);
int highlight_index(char *,int *);
int highlight_type(char *);
int is_wrap_text(FILEBUF *fp);
void init_highlight_structure(MLQUOTES *hs);
int check_update_highlight(int flag);
int file_type_is(char *extention,int check_type);
void open_log(char *program_name);
#if	!USE_GLIB
char *lstr_to_upper(char *s);
char *lstr_to_lower(char *s);
#endif



int execute_session(char *);
int parse_check_current_buffer(num n);
int parse_buffer_show_tokens(num n);

void init_default_schemes();

// display.c
int error_line(char *,...);
void vtinit(int argc, char **argp);
int check_cursor_position(WINDP *wp);
int get_utf_length(utfchar *utf_char_str);
void upd_all_virtual_lines(WINDP *wp,char *from);
void upd_column_pos();
void updgar();
void update_physical_windows();
void movecursor(int row, int col);
void clear_message_line();
void mlforce(char *s)	;
num vtline(WINDP *wp, offs tp_offs);
void vtmove(WINDP *wp,int row,int col);
void update_status();
offs highlight_restore_state(WINDP *wp,int);
void highlight_save_state(WINDP *wp,int,num);
void fquote_state(offs till_o, offs from_o, WINDP *wp);
void top_menu(num init);
int draw_window_line(WINDP *wp, int row);
void put_wtext(WINDP *wp,int row,int maxcol);
void vtputc(WINDP *wp, unsigned int c);
void vt1eol(WINDP *wp,int c,int color);
void upd_part(WINDP *wp,char *from);

// system.c
char *ext_system_paste_line();

// int start_edit(int b);
int dir_getfile(char *fname,int flag) ;
void view_file(char *fname);
void push_dir(FILEBUF *) ;
void pop_dir(FILEBUF *);
void reinit_dir(FILEBUF *);
// void show_textpoints(char *title,FILEBUF *);

// file.c


int activate_file(FILEBUF *bp);
// int FUtfCharLen(FILEBUF *bp,offs o);
int select_filebuf(FILEBUF *bp);
void link_window_buffer(WINDP *wp,FILEBUF *fp);
void unlink_filebuf(WINDP *);
int delete_filebuf(FILEBUF *bp,int force);
int empty_filebuf(FILEBUF *bp);
int list_filebuf(char *title, char *st,int f1,int *return_flag);
int changes_occured();
FILEBUF * new_filebuf(char *bname,int bflag);
FILEBUF * get_filebuf(char *bname,char *dname,int flag);

/* window.c  */

void msg_log(int priority,const char *fmt, ...);
char *getnword();
char *getcurcword();
void hide_cursor_dl();
void list_select(char *s,int dire);
int list_update(char *s);
void init_input();
int insert_dir(FILEBUF *buf_dir,int retain);
int set_ddir(FILEBUF *file_buf);
int list_dir(char *d1,FILEBUF *file_buf);
void list_dir1(char *st);

void set_todirname(char *base_name,int dir_num);
int selectl(char *title,char *m[],int nu,int lmax,int sx,int sy,int width,int active);
int disp_list(char **m,int start,int nu,int lmax,int sx,int sy,int maxl,int active);
int find_tag(num n);
int view_functions(num n);
int return_from_tag(num n);

/* bookmark functions */
int set_hmark(int f,char *from);
void update_hmark();

void freelist(char **namelist, num n);
/* ===================================================================*/
int scandir2(char *dirname, struct kdirent ***namelist_a);
void sort_dir2(int noStr, struct kdirent **dirlist);
void init_names();
void clear_names();

/* help.c */
char *xe_key_name(int);
int getckey();
int startup(char *sfname);
char *find_file(char *subdir,char *fname,int cflag,int create_if_not_found);
char *cmdstr(int c);
char *function_name(int (*func)(num), char **description);

// crypt.c
int set_buf_key(FILEBUF *curbp);
void crypt_string(char *ctr,num len);

// support.c
int read_list_array(char *file_name, alist *new_list);
int save_list_array(char *file_name, alist *array);
char *get_start_dir();
char *get_pfname(char *dir_name,char *file_name,int len);

// mlang.c
void set_break(char *from);

// eval.c
FILEBUF * cls_fout(char *bname);
int sfb(FILEBUF *bp);
int create_function_buffer(FILEBUF *pbuf,char *function_name,offs start_function_offset,offs foffset);
int newvar(char *vname);
double get_env(int vnum);
int set_option_val(int vnum,char *value);
int debug_flag();
void increase_debug_flag();
void decrease_debug_flag();

void set_env(int vnum,char *svalue,double value);

int sindex(char *source, char *pattern);
int nextarg(char *prompt,char *buffer, int size,int disinp);
char *time2a();
void getnc() ;
void getntok(char *var_name);

double expression(char *);
double lexpression();
double modulo(double value,double factor);
double lterm1();
double term1();
double term2();
void nword(char *var_name);
double getdouble();

// int refresh_current_line1(num n);
offs refresh_ddot(FILEBUF *buf,offs l,double value);
void init_exec_flags();
// int parse1(num n);
int get_expr_type(char *st);
double compute_string(char *s,char *new_string);
int setsval(int i,char *sval);
int setnewsvar(char *vname,char *sval);
void init_hash();
int docmd_error(char *cmd);
void fixstack();
// double rval(int iv);
double command();
double mcommand();
int check_rparenthesis();

// exec.c
double gosub(char *,VAR *vargs);

// int dofile(char *fname,array_dat *args_array);

// file.c

void save_scratch_files();
int load_scratch_files();
int is_scratch_buffer(FILEBUF *);
char *get_buf_full_name(FILEBUF *fp);
int open_file_named(char *fname);
int add_to_recent_list(char *full_file_name);
void set_working_dir(char *dir);
char * get_working_dir();

void init_extensions();
int resetkey(FILEBUF *bp);

int edit_file(char *fname);
int file_read(FILEBUF *bp, char *fname);
int file_read1(FILEBUF *bp, char *fname);
void set_bfname(char *bname, char *fname);
void create_base_name(char *base_name,char *full_name);

int writeu1(char *fname, FILEBUF *fp);
int writeout(char *fn,FILEBUF *fp);
int rename_file(num n);
int ifile(FILEBUF *bp,char *fname,int insert_flag);
void update_file_status(FILEBUF *fp);
void set_modified(FILEBUF *bp);
int menufile(num n);
int file_type(char *name, int *tc, char *oext);
int getindex(char *st);
void update_buffer_windows(FILEBUF *bp,int flag);

int get_highlight(FILEBUF *fp);

VIDEO **new_screen();
void reallocate_windows();

// input.c
int confirm(char *title,char *prompt,int always);
int ectoc(int c);
int (*getname(char *prompt))();
int complete(char *buf);
int tgetc();
int get1key();
int getcmd();
int getstring(char *prompt, char *st1,int maxlen,int disinp);
int delchar(char *st,int pos);
int inspace(char *st,int pos);
int isspecialchar(char *st);
int normalize(int);
int xlate(int,int);
int capital(int);
WINDP *get_mouse_window();
int list_on();
void list_off();
void activate_list_mode();



// window.c
int getwline();
void set_windows_color();
void save_point_mark(WINDP *wp);
void move_window_data(WINDP *wp1, WINDP *wp2);
void free_window_data(WINDP *wp);
WINDP *find_buffer_window(FILEBUF *fp);
void free_virtual_window(WINDP *wp);

// word.c
int inword();

// eval.c
int execute(int c, int  n);
// int nullproc(num n);
// int wrap();


// search.c
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

int select1_font(num n);

// plot.c
void set_plot_timer();
void do_timer_plot();
int transx(double x);
int transy(double y);
int plot_init(num n);
int plot_close(num n) ;
int path_start(num n);
int path_end(num n);
int path_move(num n);
int path_line(num n);
int path_curve(num n);
int set_relative(num n);
int set_absolute(num n);
int plot_frames(num n);
int plot_curve_to(num n);
int plot_clear(num n);
void pl_clear();
int plot_move_to(num n);
int plot_point(num n);
int plot_line(num n) ;
int plot_lineto(num n) ;
int plot_color(num n);
int plot_set_color(num n);
int plot_lwidth(num n);
int plot_resize(num n);
void resize_plot_subwins();
void plot_status(int f,char *st);
int plot_title(num n);
int plot_coord(num n) ;
int plot_rotate(num n);
int plot_translate(num n);
int plot_scale(num n);
int plot_text(num n);
void set_ratio();
void set_coords();
// void show_coords();
int plot_fill(num n);
int plot_circle(num n) ;
int plot_rcircle(num n) ;
int plot_elipsep(num n);
int plot_fcolor(num n);
int plot_rectangle(num n);
int plot_triangle(num n);
int plot_arc(num n) ;
int plot_image(num n) ;
int plot_polyline(num n);
int plot_alpha(num n);
int plot_stat(num n) ;
int plot_close_path(num n);
int plot_poly_nostroke(num n);
void toggle_isomode();
void toggle_gridmode();
void toggle_nummode();
void toggle_zoompriority();
void toggle_zoom();
double dmean(double x,double y);
int plot_zoom(num n) ;
int plot_move(num direction) ;
void plot_redraw();
char *kt_getstr(char *prompt);
void show_values();

void new_plot_list();
double *add_plot_element(int type, int size);
void free_plot_list();
void titletext();
void put_string(char *);


void show_cursor_dl (int px);
void put_string_dl(char *st);
void xupdate_box();
void xdab(int y,int b,char *s,int bcolor,int fcolor);
void box_line_print(int y,int start, char *st,int width, int selected,int active);
void remove_box();

int dont_edit();
void set_current_window(WINDP *wp,char *);
void set_1window();
void window_clear(WINDP *wp);

/* low level terminal driver functions */
void prepare_converter(int);
int text_mouse_key(int *c);
int mouse_button_in_box(int mouse_button);


void out_print(char *s,int nl);
void out_fprint(char *bname, char *s,int nl);
num	tp_col(TextPoint *tp);
offs tp_offset(TextPoint *tp);
num tp_line(TextPoint *tp);
void tp_copy(TextPoint *tp1,TextPoint *tp2);
void textpoint_set(TextPoint *tp,offs o);
TextPoint *new_textpoint_at(FILEBUF *bf,int tp_type,offs o);
void move_at(offs o);
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
void tp_move(TextPoint *tp, offs o, num line, int col);
offs FSize(FILEBUF *);
bool test_b_flag(int flag);
void set_b_flag(int flag);
offs BufferSize();
offs lforw(offs);
offs lback(offs);
int   ReadBlock(char *file_name,int fd,offs size,offs *act_read);
#if	NUSE
int   ReplaceBlock(char *block,offs size);
#endif
void  textpoint_delete(TextPoint *tp);
void set_Char(offs,byte);
int clipboard_copy(ClipBoard *clip);
int clipboard_paste(ClipBoard *clip);
ClipBoard *clipboard_new();
void init_clipboard();
void checkwb(char *);
void syncwb();
void textpoint_move(TextPoint *tp,offs o);
void update_lines(FILEBUF *bp);
int init_ftype(FILEBUF *bf,char *file_name,int *temp_used,int from_note);
// void beginundogroup(UNDOS *u);
// void EndUndoGroup(UNDOS *u);
num GetCol();
num WGetCol();
num GetLine();
num getccolumn();
num LastLine();
num FGetLine(FILEBUF *fp);
num FColumn(FILEBUF *fp,offs o);
num Column(offs o);
int show_info(num);
// offs NextNLines(offs,num);
void textpoint_link(TextPoint *,FILEBUF *fp);
void textpoint_unlink(TextPoint *);
void set_change(FILEBUF *);
void  ResetTextPoints(FILEBUF *bp,int);

/* highlight.c */
void init_highlight();
void set_highlight(FILEBUF *bf,int type);
void init_line_txt(WINDP *wp,offs tp_offs);
void init_line_cobol(WINDP *wp,offs tp_offs);

void update_highlight(WINDP *wp);
void init_line_highlight(WINDP *wp);

void set_selection(int);
int get_selection();
void *emalloc(long,char *);
void efree(void *,char *);
char *getcurfword();
void start_interactive(char *);
void end_interactive();
void init_message_loop();
void  main_execute(int c);
int set_play_string(char *);
void set_record_string(char *);
void record_key(short);
void drv_post_windows_update();
void update_cursor_position();
void textpoint_free(TextPoint *);
WINDP *dublicate_window(WINDP *wp0);
char *get_last_in_list(int tlist);
int file_exists(char *fname);
int color_edit(num n);

/* system.c  */
int init_system_clipboard();
int mkdirRecursive(const char *path, mode_t mode);

/* support.c  */
char **read_sarray(char *file_name, int *size);
void free_sarray(char **sarray);
void print_sarray(char **sarray,char *title);

#include "utils.h"

void skip_token();

#if	!DARWIN
/*
 * '_cups_strlcat()' - Safely concatenate two strings.
 */

size_t                  /* O - Length of string */
strlcat(char       *dst,        /* O - Destination string */
              const char *src,      /* I - Source string */
          size_t     size);      /* I - Size of destination string buffer */

/*
 * '_cups_strlcpy()' - Safely copy two strings.
 */

size_t                  /* O - Length of string */
strlcpy(char       *dst,        /* O - Destination string */
              const char *src,      /* I - Source string */
         size_t      size);     /* I - Size of destination string buffer */
#endif
