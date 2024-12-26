/*
	Curses,gtk editor,notes,directory browser
	Global variables, function declarations
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.
*/

int select_font_mono(num n);
int buffer_is_connected(FILEBUF *bf);
int noop(num n);
void msg_line(char *fmt, ...);
void MESG_time(const char *fmt, ...);
void MESG_time_start(const char *fmt, ...);
int init_drv_env();
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
int save_colors(num n);
int read_colors(num n);
int color_scheme_save();
int buffer_in_view(num n);
int prev_line(num);
int next_line(num);
int comment_line(num);
int comment_line2(num);
int scroll_up(num);
int scroll_down(num);
int get_hex_byte();
void toggle_hex_byte();
void reset_hex_byte();
int move_window(num);
int move_line_up(num);
int move_line_down(num);
void select_current_word();
int window_cursor_line(WINDP *wp);
void set_goal_column(int new_column,char *from);
void set_xmark(WINDP *smark_wp, int mouse_col,int mouse_row,int f);
void clear_names();
void clear_snames(char **,int);
int set_option_val(int vnum,char *value);
int goto_bol(num);
int goto_file(char *file_name);
int open_dir(char *dir_name,int type);
int goto_dir(char *dir_name,int dir_num);
int dir_size(num n);
int goto_line(num n);
int goto_offset(num n);
int igotolinecol(num line,num column,int flag);
int igotooffset(num n,int ppline);
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

double next_value();
int set_arg(num n);
void status_line(WINDP *wp);
void show_slide(WINDP *wp);
void put_string_statusline(WINDP *wp, char *st, int position);
int show_position_info(num short_version);
void draw_window(int,WINDP *wp,char *from);
void expose_window(WINDP *wp);
void change_window(WINDP *wp);
int (*key_function(int c,int f))();
short int get_next_key(num mode);
char *date_string(num n);
int insert_date(num n);
void list_select(char *s,int dire);
void parse_command_line(int , char **);
int highlight_index(char *,int *);
int highlight_type(char *);
int is_wrap_text(FILEBUF *fp);
void init_highlight_structure(MLQUOTES *hs);

int select_highlight(num);
int check_update_highlight(int flag);
int file_type_is(char *extention,int check_type);
void open_log(char *program_name);
#if	USE_GLIB0
void set_cfg_int(char *label,int ival);
#endif
#if	!USE_GLIB
char *lstr_to_upper(char *s);
char *lstr_to_lower(char *s);
#endif



int load_session(num);
int show_session(num);
int save_session(num);
int execute_session(char *);
int parse_check_current_buffer(num n);
int parse_buffer_show_tokens(num n);
int set_color(num);
int color_scheme_read();

int set_sort_mode(num mode);

int select_scheme(num n);
void init_default_schemes();

// display.c
int error_line(char *,...);
void vtinit(int argc, char **argp);
int update_screen(num);
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

int set_view_mode(num n);
int unix2dos(num n);
int dos2unix(num n);
// from basic.c
int prev_character(num n);
int goto_eol(num n);
int next_character(num n);
int select_next_character(num n);
int select_prev_character(num n);
int select_next_line(num n);
int select_prev_line(num n);
int goto_bof(num n);
int goto_eof(num n);
int emptyline(TextPoint *tp);
int valid_offset(offs bol,int curcol);
int goto_bop(num n);
int goto_eop(num n);
int next_page(num  n);
int prev_page(num n);
int set_mark(num n);
int set_line_mark(num n);
int set_column_mark(num n);
int setmark(num n);
int set_no_mark(num n);
int swap_mark(num n);
int toggle_move_lock(num n);
int toggle_sync_lock(num n);
int change_color_scheme(num  n);
int show_version(num n);
int do_nothing(num n);
unsigned char *str2out(char *);
void MoveLineCol(num l,num c);

// system.c
char *ext_system_paste_line();
int ext_system_paste();
int ext_system_copy();
// 
int new_shell(num n);
int dir_left(num n);
int dir_right(num n);
int dir_new_file(num n);
int dir_new_dir(num n);
int dir_link(num n);
int dir_reload(num n);
int view_next(num n);
int view_previous(num n);
int shell_cmd(num n);
int bg_cmd(num n);
int grep_cmd(num n);
int rgrep_cmd(num n);
int filter_buffer(num n);
int grep_buffer(num n);
int sysexec(char *st);
int dir_exec(num n) ;
int dir_view(num n);
int script_exec(num n) ;
int dir_edit(num n) ;
int start_edit(int b);
int dir_copy(num n);
int dir_move(num n);
int dir_file_rename(num n);
int dir_del1(num  n) ;
int dir_tag(num n) ;
int dir_showfileinfo(num type) ;
int dir_getfile(char *fname,int flag) ;
void view_file(char *fname);
void push_dir(FILEBUF *) ;
void pop_dir(FILEBUF *);
void reinit_dir(FILEBUF *);
void show_textpoints(char *title,FILEBUF *);
void set_string_val(char **, char *);

// filebuf.c
FILEBUF *current_filebuf();
void set_current_filebuf(FILEBUF *fp);
int new_file(num n);
int next_file(num n);
int activate_file(FILEBUF *bp);
int FUtfCharLen(FILEBUF *bp,offs o);
int select_filebuf(FILEBUF *bp);
void link_window_buffer(WINDP *wp,FILEBUF *fp);
void unlink_filebuf(WINDP *);
int close_file(num n);
int delete_filebuf(FILEBUF *bp,int force);
int select_file(num n);
int empty_filebuf(FILEBUF *bp);
int select_file_internal(num n);
int list_filebuf(char *title, char *st,int f1,int *return_flag);
int changes_occured();
FILEBUF * new_filebuf(char *bname,int bflag);
FILEBUF * get_filebuf(char *bname,char *dname,int flag);
int DiffColumn(FILEBUF *fp, offs *dbo,offs col_offs,char *from);
int DiffColumns(FILEBUF *fp, offs start,offs col_offs,char *from);


int window_row_increase(num n);
int window_column_increase(num n);
int window_row_decrease(num n);
int window_column_decrease(num n);

void msg_log(int priority,const char *fmt, ...);
int toggle_mouse(num n);
int toggle_parameter(num type);
int twiddle(num n);
int quote(num n);
int tab(num n);
int next_tab(num i);
int detab_line(num n);
int entab_line(num n);
int trim_line(num n);
int new_line(num n);
int del_blank_lines(num n);
int insert_indent(num n);
int del_char(num n);
int del_prev_char(num n);
int delete_line(num n);
int erase_eol(num n);
int set_optionf(int option_flag,int global);
void toggle_optionf(int option_flag,int global);
int set_over(num n);
int toggle_over(num n);
int toggle_case(num n);
int toggle_regexp(num n);
int set_case(num n);
int set_regexp(num n);
#if	CRYPT
int set_crypt(num n);
#endif
int toggle_highlight(num n);
int toggle_crypt(num n);
int next_utf8_error(num n);
int set_just(num n);
int toggle_just(num n);
int match_fence(num n);
int istring(num n);
char *bname(char *fname) ;
char *getnw();
char *getnword();
char *getcursword();
char *getcurcword();
int insword(int c);
int execute_menu(num fixed);	/* execute menu */
int menu_command(num n);
int work_menu(num n);
int set_doc_lang(num n);
int set_default_local(num n);
void hide_cursor_dl();
void list_select(char *s,int dire);
int list_update(char *s);
void init_input();
int set_dir();
int insert_dir(FILEBUF *buf_dir,int retain);
int set_ddir(FILEBUF *file_buf);
int list_dir(char *d1,FILEBUF *file_buf);
void list_dir1(char *st);
int listdir1();
int listdir2();

void set_todirname(char *base_name,int dir_num);
int selectl(char *title,char *m[],int nu,int lmax,int sx,int sy,int width,int active);
int disp_list(char **m,int start,int nu,int lmax,int sx,int sy,int maxl,int active);
int find_tag(num n);
int view_functions(num n);
int return_from_tag(num n);

/* bookmark functions */
int set_bmark(num n);
int select_bmark(num n);
int prev_bmark(num n);
int next_bmark(num n);
int clear_bmarks(num n) ;
int set_hmark(int f,char *from);
int current_hmark(num n);
int prev_hmark(num n);
int next_hmark(num n);
void update_hmark();
int delete_hmark(num flag);
int select_hmark(num n);

int convert_char(num n);
void freelist(char **namelist, num n);
/* ===================================================================*/
int scandir2(char *dirname, struct kdirent ***namelist_a);
void sort_dir2(int noStr, struct kdirent **dirlist);
void init_names();
void clear_names();

// bind.c
int help(num n);
char *xe_key_name(int);
int show_keys(num n);
int assign_function(num n);
int assign_sub(num n);
int unassign_key(num n);
int describe_key(num n);
int getckey();
int startup(char *sfname);
char *find_file(char *subdir,char *fname,int cflag,int create_if_not_found);
char *cmdstr(int c);
char *function_name(int (*func)(), char **description);

// crypt.c
int set_key( num n);
int set_buf_key(FILEBUF *curbp);
void crypt_string(char *ctr,num len);

// support.c
int read_list_array(char *file_name, alist *new_list);
int save_list_array(char *file_name, alist *array);
char *get_start_dir();
char *get_pfname(char *dir_name,char *file_name,int len);

// mlang.c
void initialize_vars();
void set_break();

// eval.c
FILEBUF * cls_fout(char *bname);
int sfb(FILEBUF *bp);
int create_function_buffer(FILEBUF *pbuf,char *function_name,offs start_function_offset,offs foffset);
double eval_fun(num fnum);
int newvar(char *vname);
double get_env(int vnum);
int set_option(num n);
int set_option_val(int vnum,char *value);
int set_debug(num n);
int debug_flag();
void increase_debug_flag();
void decrease_debug_flag();

void set_env(int vnum,char *svalue,double value);

int sindex(char *source, char *pattern);
int nextarg(char *prompt,char *buffer, int size,int disinp);
#if	DATE
char *time2a();
#endif
void getnc() ;
void getntok(char *var_name);

double num_expression();
double lexpression();
double modulo(double value,double factor);
double lterm1();
double term1();
double term2();
double set_string_value();
void nword(char *var_name);
double getdouble();
int refresh_current_line(num n);
int refresh_current_line1(num n);
offs refresh_ddot(FILEBUF *buf,offs l,double value);
int refresh_current_buffer(num n);
void init_exec_flags();
int show_parse_buffer(num n);
int parse1(num n);
int get_expr_type(char *st);
double compute_string(char *s,char *new_string);
int setsval(int i,char *sval);
int setnewsvar(char *vname,char *sval);
void init_hash();
int docmd_error(char *cmd);
void fixstack();
double rval(int iv);
double command();
double mcommand();
int set_locale(num n);
int check_rparenthesis();

// exec.c
int macro_line(num n);
double gosub(char *,VAR *vargs);
int execsub(num n);

int exec_file(num n);
// int dofile(char *fname,array_dat *args_array);
#if	CTAGS
int update_tags(num n);
#endif

// file.c
int reload_file(num n);
int insert_file(num  n);
int open_file(num n);
int clear_buffer(num n);
int umark_buffer(num n);
int open_recent_file(num n);
int read_file_history(num n);
int save_file_history(num n);
void save_scratch_files();
int load_scratch_files();
int is_scratch_buffer(FILEBUF *);
char *get_buf_full_name(FILEBUF *fp);
int open_file_named(char *fname);
int add_to_recent_list(char *full_file_name);
void set_working_dir(char *dir);
char * get_working_dir();

void init_extensions();
#if	CRYPT
int resetkey(FILEBUF *bp);
#endif
int edit_file(char *fname);
int file_read(FILEBUF *bp, char *fname);
int file_read1(FILEBUF *bp, char *fname);
void set_bfname(char *bname, char *fname);
void create_base_name(char *base_name,char *full_name);

int writeu1(char *fname, FILEBUF *fp);
int saveas_file(num n);
int save_file(num n);
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
int mouse_move(num n);
#if	1
int text_mouse_pos(num n);
int text_mouse_left_press(num n);
int text_mouse_right_press(num n);
int text_mouse_release(num n);
int text_mouse_error(num n);
#endif
WINDP *get_mouse_window();
int list_on();
void list_off();
void activate_list_mode();


// edit.c
void set_update(WINDP *wp, int flag);
int insert_chr(int n,int c);
int insert_string(FILEBUF *,char *c,int size);
int insert_string_nl(FILEBUF *,char *c);
int insert_newline(FILEBUF *);
int forw_delete(long n);
int back_delete(long n);
char *getctext();
int set_linetext(char *iline);
int ldelnewline();
int	system_paste(num n);

// edit.c
int cut_region(num n);
int copy_region(num n);
int paste_region(num n);
int lowerregion(num n);
int upperregion(num n);
int delete_box(num n);
int get_kill(int init);
void init_kill();
void reset_region_textpoints();
int delete_region();
int abort_cmd(num n);
int update_menu(num n);


int undo(num n);
int redo(num n);
int set_bg(num n);

// window.c
int reposition(num n);
int hard_reposition(num n);
int next_window(num n);
int prev_window(num n);
int one_window(num n);
int delete_window(num n);
int split_window(num n);
int window_row_resize(num n);
int window_column_resize(num n);
int vresize_wind(num n);
int hresize_wind(num n);
int both_down(num n);
int both_up(num n);
int page_move_up(num n);
int page_move_down(num n);
int getwline();
void set_windows_color();
void save_point_mark(WINDP *wp);
void move_window_data(WINDP *wp1, WINDP *wp2);
void free_window_data(WINDP *wp);
WINDP *find_buffer_window(FILEBUF *fp);
void free_virtual_window(WINDP *wp);

// word.c
int prev_word(num n);
int next_word(num n);
int goto_eow(num n);
int delete_word(num n);
int delbword(num n);
int insert_tab(num n);
int remove_tab(num n);
int inword();
int fillpara(num n);
int killpara(num n);
int countwords(num n);
int linefill(num ident);

// main.c
void edinit(char *bname);
int execute(int c, int  n);
int quick_close(num n);
int quit(num n);
int record_macro(num n);
int exec_macro(num n);
int nullproc(num n);
int wrap();
int set_key_emulation(num n);

// gui driver

int vsplit_window(num n);

// search.c
int eq(int x,int y);
int forwsearch(num n);
int find_next(num n);
int backsearch(num n);
int find_previous(num n);
int	scanner(char *patrn, int direct, int beg_or_end);
int	readpattern(char *prompt, char *str, int srch);
int	setpattern(char apat[], char *st,int srch);
void savematch(int direction);
void rvstrcpy(char *rvstr, char *str,int maxlen);
int sreplace(num n);
int qreplace(num n);
int replace_next(num n);
int	replaces(int kind, int f,num n);
int delins(int dlength, char *instr);
int forw_stat(num n);
int back_stat(num n);
int lfound(int status,int direction);
offs find_string_inline(char *patrn);
//void set_global(int beg_or_end,int direct);

int select_font(num n);
int select1_font(num n);
int select_font_mono(num n);

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

int set_fontindex(num n);
int increase_font_size(num n);
int decrease_font_size(num n);

int dont_edit();
void set_current_window(WINDP *wp,char *);
void set_1window();
void window_clear(WINDP *wp);

/* low level terminal driver functions */
void drv_init(int, char **);
void drv_post_init();
void drv_open();
void drv_close();
int drv_getc(int quote);
void drv_flush();
void drv_move(int,int);
void drv_color(int,int);
void prepare_converter(int);
void drv_update_styles();
void drv_msg_line(char *);
void drv_win_move(WINDP *wp,int row,int col);
void drv_win_flush(WINDP *wp);
void drv_start_checking_break();
void drv_stop_checking_break();
int drv_check_break_key();
void drv_window_delete(WINDP *wp);
void drv_free_win(WINDP *wp);
void drv_start_window_update(WINDP *wp);
int text_mouse_key(int *c);
int mouse_button_in_box(int mouse_button);
char *drv_info();
GWINDP * drv_new_twinp();
void events_flush();

GWINDP *new_twinp();


void out_print(char *s,int nl);
void out_fprint(char *bname, char *s,int nl);
num	tp_col(TextPoint *tp);
offs tp_offset(TextPoint *tp);
num tp_line(TextPoint *tp);
void tp_copy(TextPoint *tp1,TextPoint *tp2);
void textpoint_set(TextPoint *tp,offs o);
TextPoint *new_textpoint_at(FILEBUF *bf,int tp_type,offs o);
void move_at(offs o);
offs   LineBegin(offs ptr);
offs   FLineBegin(FILEBUF *fp,offs ptr);
offs   LineEnd(offs ptr);
offs   FLineEnd(FILEBUF *fp,offs ptr);
offs   Size();
void   MoveRightChar(FILEBUF *fp);
void   MoveLeftChar(FILEBUF *fp);
void	ToLineEnd();
num FLineLen(FILEBUF *,offs);
int		Eol();	// if at end of line
int		Bol();	// if at beginning of line
void textpoint_set_lc(TextPoint *tp,num, num c);
int	BolAt(offs);
int	FBolAt(FILEBUF *,offs);
int EolAt(offs);
int FEolAt(FILEBUF *bf,offs o);
byte tp_char(TextPoint *);

int FEofAt(FILEBUF *,offs);
int EofAt(offs o);
int tp_llength(TextPoint *);
int offs_llength(offs o);
byte Char();
long int utf_value();
long int utf_value_len(int *len);
byte FCharAt(FILEBUF *,offs);
byte CharAt(offs);
offs FUtfCharAt(FILEBUF *,offs,utfchar *);
offs Offset();
offs FOffset(FILEBUF *);
offs CSize();
void set_Offset(offs);
int FEof(FILEBUF *);
int FBof(FILEBUF *);
void DeleteChar();
int InsertChar(char ch);
TextPoint *tp_dup(TextPoint *,int);
TextPoint *textpoint_init(int tp_type,int,FILEBUF *fp);
TextPoint *textpoint_new(FILEBUF *b,int tp_type,int);
TextPoint *textpoint_o(FILEBUF *p,offs o,int tp_type,int);
void set_end_line(FILEBUF *fp,num line);
void  EmptyText(FILEBUF *);
UNDOS *undo_new();
void tp_move(TextPoint *tp, offs o, num line, int col);
offs FSize(FILEBUF *);
bool test_b_flag(int flag);
void set_b_flag(int flag);
offs BufferSize();
offs lforw(offs);
offs lback(offs);
offs FNextLine(FILEBUF *,offs);
offs FPrevLine(FILEBUF *,offs);
int   ReadBlock(char *file_name,int fd,offs size,offs *act_read);
int   DeleteBlock(offs left,offs right);
int   InsertBlock(FILEBUF *fp,char *block_left,offs size_left, char *block_right,offs size_right);
#if	NUSE
int   ReplaceBlock(char *block,offs size);
#endif
void  textpoint_delete(TextPoint *tp);
offs   get_text_offs(FILEBUF *,char *,offs,offs);
void set_Char(offs,byte);
void ToStartColumn();
int StartColumn(offs o);
void ToLineBegin();
void ToLineEnd();
int clipboard_copy(ClipBoard *clip);
int clipboard_paste(ClipBoard *clip);
ClipBoard *clipboard_new();
void init_clipboard();
void checkwb(char *);
void syncwb();
void textpoint_move(TextPoint *tp,offs o);
void update_lines(FILEBUF *bp);
int init_ftype(FILEBUF *bf,char *file_name,int *temp_used,int from_note);
void beginundogroup(UNDOS *u);
void EndUndoGroup(UNDOS *u);
num GetCol();
num WGetCol();
num GetLine();
num GetColumn();
num getccolumn();
num LastLine();
num FGetLine(FILEBUF *fp);
num FColumn(FILEBUF *fp,offs o);
num Column(offs o);
int show_info(num);
void undo_set_glue();
void undo_set_noglue();
offs NextNLines(offs,num);
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
int toggle_bold(num n);
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

#if	TNOTES
int new_note(num n);
int new_cal_note(num n);
int new_todo_note(num n);
int save_note();
int open_notes_dir(num n);
int search_notes_dir(num n);
int search_notes_tagged(num n);
int view_notes_by_date(num n);
int create_notes_db(num n);
int show_calendar_list(num n);
int show_todo_list(num n);
char *get_notes_status();
int reload_tag_view();
#endif

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
