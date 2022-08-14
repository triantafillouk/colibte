/*
	Curses,gtk editor,notes,directory browser
	Global variables, function declarations
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.
*/

int select_font_mono(int n);
int buffer_is_connected(FILEBUF *bf);
int noop(int n);
void msg_line(char *fmt, ...);
int init_drv_env();
void init_common();
int load_keys();
int save_keys();
int  get_cfg_int(char *label,int default_val);
int show_bt_table();
int show_bt_table_ordered();
int sh_outwindow(int i);
int show_key(int key);
void load_config();
void save_config();
num getcline();
int save_colors(int n);
int read_colors(int n);
int color_scheme_save();
int color_scheme_read();
int buffer_in_view(int n);
int prev_line(int);
int next_line(int);
int comment_line(int);
int comment_line2(int);
int scroll_up(int);
int scroll_down(int);
int get_hex_byte();
void toggle_hex_byte();
void reset_hex_byte();
int move_window(int);
void select_current_word();
int window_cursor_line(WINDP *wp);
void set_goal_column(int new_column,char *from);
void set_xmark(WINDP *smark_wp, int mouse_col,int mouse_row,int f);
void clear_names();
void clear_snames(char **,int);
int set_option_val(int vnum,char *value);
int goto_bol(int);
int goto_file(char *file_name);
int open_dir(char *dir_name,int type);
int goto_dir(char *dir_name,int dir_num);
int goto_line(num n);
int igotolinecol(num line,num column,int flag);
int igotooffset(num n,int ppline);
int docmd(char *cmd);
void hide_cursor(char *from);
void show_cursor(char *);
double get_val();
char *get_sval();
void set_sval(char *);
void set_dval(double val);

double next_value();
int set_arg(int n);
void status_line(WINDP *wp);
void show_slide(WINDP *wp);
void put_string_statusline(WINDP *wp, char *st, int position);
int show_position_info(int short_version);
void draw_window(int,WINDP *wp,char *from);
void expose_window(WINDP *wp);
void change_window(WINDP *wp);
int (*key_function(int c,int f))();
short int get_next_key(int mode);
char *date_string(int n);
int insert_date(int n);
void list_select(char *s,int dire);
void parse_command_line(int , char **);
int highlight_index(char *,int *);
int highlight_type(char *);

int select_highlight(int);
int check_update_highlight(int flag);
int file_type_is(char *extention,int check_type);
void open_log(char *program_name);
void set_cfg_int(char *label,int ival);


int load_session(int);
int show_session(int);
int save_session(int);
int execute_session(char *);
int parse_check_current_buffer(int n);
int parse_buffer_show_tokens(int n);
int set_color(int);
int color_scheme_read();

int set_sort_mode(int mode);

int select_scheme(int n);
void create_default_scheme();

// display.c
int utf_lerror();
int error_line(char *,...);
void vtinit();
int update_screen(int);
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
void getwquotes(WINDP *wp,int);
void setwquotes(WINDP *wp,int,num);
void fquote_state(offs till_o, offs from_o, WINDP *wp);
void top_menu(int init);
int draw_window_line(WINDP *wp, int row);
void put_wtext(WINDP *wp,int row,int maxcol);
void vtputc(WINDP *wp, unsigned int c);
void vt1eol(WINDP *wp,int c,int color);
void upd_part(WINDP *wp,char *from);

int set_view_mode(int n);
int unix2dos(int n);
int dos2unix(int n);
// from basic.c
int prev_character(int n);
int goto_eol(int n);
int next_character(int n);
int select_next_character(int n);
int select_prev_character(int n);
int select_next_line(int n);
int select_prev_line(int n);
int goto_bof(int);
int goto_eof(int n);
int emptyline(TextPoint *tp);
int valid_offset(offs bol,int curcol);
int goto_bop(int n);
int goto_eop(int n);
int next_page(int  n);
int prev_page(int n);
int set_mark(int n);
int set_line_mark(int n);
int set_column_mark(int n);
int setmark(int n);
int set_no_mark(int n);
int swap_mark(int n);
int toggle_move_lock(int n);
int toggle_sync_lock(int n);
int change_color_scheme(int  n);
int show_version(int n);
int do_nothing(int n);
unsigned char *str2out(char *);
void MoveLineCol(num l,num c);

// system.c
char *ext_system_paste_line();
int ext_system_paste();
int ext_system_copy();
// 
int new_shell(int n);
int dir_left(int n);
int dir_right(int n);
int dir_new_file(int n);
int dir_new_dir(int n);
int dir_link(int n);
int dir_reload(int n);
int view_next(int n);
int view_previous(int n);
int shell_cmd(int n);
int bg_cmd(int n);
int grep_cmd(int n);
int rgrep_cmd(int n);
int filter_buffer(int n);
int grep_buffer(int n);
int sysexec(char *st);
int dir_exec(int n) ;
int dir_view(int n);
int script_exec(int n) ;
int dir_edit(int n) ;
int start_edit(int b);
int dir_copy(int n);
int dir_move(int n);
int dir_file_rename(int n);
int dir_del1(int  n) ;
int dir_tag(int n) ;
int dir_showfileinfo(int type) ;
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
int new_file(int n);
int next_file(int n);
int activate_file(FILEBUF *bp);
int FUtfCharLen(FILEBUF *bp,offs o);
int select_filebuf(FILEBUF *bp);
void link_window_buffer(WINDP *wp,FILEBUF *fp);
void unlink_filebuf(WINDP *);
int close_file(int n);
int delete_filebuf(FILEBUF *bp,int force);
int select_file(int n);
int empty_filebuf(FILEBUF *bp);
int select_file_internal(int n);
int list_filebuf(char *title, char *st,int f1,int *return_flag);
int changes_occured();
FILEBUF * new_filebuf(char *bname,int bflag);
FILEBUF * get_filebuf(char *bname,char *dname,int flag);
int DiffColumn(FILEBUF *fp, offs *dbo,offs col_offs);


int window_row_increase(int n);
int window_column_increase(int n);
int window_row_decrease(int n);
int window_column_decrease(int n);

void msg_log(int priority,const char *fmt, ...);
int toggle_mouse(int n);
int toggle_parameter(int type);
int twiddle(int n);
int quote(int n);
int tab(int n);
int next_tab(int i);
int detab_line(int n);
int entab_line(int n);
int trim_line(int n);
int new_line(int n);
int del_blank_lines(int n);
int insert_indent(int n);
int del_char(int n);
int del_prev_char(int n);
int delete_line(int n);
int erase_eol(int n);
int set_optionf(int option_flag,int global);
void toggle_optionf(int option_flag,int global);
int set_over(int n);
int toggle_over(int n);
int toggle_case(int n);
int toggle_regexp(int n);
int set_case(int n);
int set_regexp(int n);
#if	CRYPT
int set_crypt(int n);
#endif
int toggle_highlight(int n);
int toggle_crypt(int n);
int set_just(int n);
int toggle_just(int n);
int match_fence(int n);
int istring(int n);
char *bname(char *fname) ;
char *getnw();
char *getnword();
char *getcursword();
char *getcurcword();
int insword(int c);
int execute_menu(int fixed);	/* execute menu */
int menu_command(int n);
int work_menu(int n);
int set_doc_lang(int n);
int set_default_local(int n);
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
int find_tag(int n);
int view_functions(int n);
int return_from_tag(int n);

/* bookmark functions */
int set_bmark(int n);
int select_bmark(int n);
int prev_bmark(int n);
int next_bmark(int n);
int clear_bmarks(int n) ;
int set_hmark(int f,char *from);
int current_hmark(int n);
int prev_hmark(int n);
int next_hmark(int n);
void update_hmark();
int delete_hmark(int flag);
int select_hmark(int n);

int convert_char(int n);
void freelist(char **namelist, int n);
/* ===================================================================*/
int scandir2(char *dirname, struct kdirent ***namelist_a);
void sort_dir2(int noStr, struct kdirent **dirlist);
void init_names();
void clear_names();

// bind.c
int help(int n);
char *xe_key_name(int);
int show_keys(int n);
int assign_function(int n);
int assign_sub(int n);
int unassign_key(int n);
int describe_key(int n);
int getckey();
int startup(char *sfname);
char *find_file(char *subdir,char *fname,int cflag,int create_if_not_found);
char *cmdstr(int c);
char *function_name(int (*func)(), char **description);
int execsub(int i);

// crypt.c
int set_key( int n);
int set_buf_key(FILEBUF *curbp);
void crypt_string(char *ctr,long len);

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
double eval_fun(int fnum);
int newvar(char *vname);
double get_env(int vnum);
int set_option(int n);
int set_option_val(int vnum,char *value);
int set_debug(int n);
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
int refresh_current_line(int n);
int refresh_current_line1(int n);
offs refresh_ddot(FILEBUF *buf,offs l,double value);
int refresh_current_buffer(int n);
void init_exec_flags();
int show_parse_buffer(int n);
int parse1(int n);
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
int set_locale(int i);
int check_rparenthesis();

// exec.c
int macro_line(int n);
double gosub(char *,VAR *vargs);
int execsub(int n);

int exec_file(int n);
int dofile(char *fname);
#if	CTAGS
int update_tags(int n);
#endif

// file.c
int reload_file(int n);
int insert_file(int  n);
int open_file(int n);
int clear_buffer(int n);
int umark_buffer(int n);
int open_recent_file(int n);
int read_file_history(int n);
int save_file_history(int n);
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
int saveas_file(int n);
int save_file(int n);
int writeout(char *fn,FILEBUF *fp);
int rename_file(int n);
int ifile(FILEBUF *bp,char *fname,int insert_flag);
void update_file_status(FILEBUF *fp);
void set_modified(FILEBUF *bp);
int menufile(int n);
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
// int dspv(int x,int y,char *st);
int isspecialchar(char *st);
int normalize(int);
int xlate(int,int);
int capital(int);
int mouse_move(int n);
#if	1
int text_mouse_pos(int n);
int text_mouse_left_press(int n);
int text_mouse_right_press(int n);
int text_mouse_release(int n);
int text_mouse_error(int n);
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
int	system_paste(int n);

// edit.c
int cut_region(int n);
int copy_region(int n);
int paste_region(int n);
int lowerregion(int n);
int upperregion(int n);
int delete_box(int n);
int get_kill(int init);
void init_kill();
void reset_region_textpoints();
int delete_region();
int abort_cmd(int n);
int update_menu(int n);
#if	NUSE
int insert_string_col(FILEBUF *fp, char *str,int col);
#endif


int undo(int n);
int redo(int n);
int set_bg(int n);

// window.c
int reposition(int n);
int hard_reposition(int n);
int next_window(int n);
int prev_window(int n);
int one_window(int n);
int delete_window(int n);
int split_window(int n);
int window_row_resize(int n);
int window_column_resize(int n);
int both_down(int n);
int both_up(int n);
int page_move_up(int n);
int page_move_down(int n);
int getwline();
void set_windows_color();
void save_point_mark(WINDP *wp);
void move_window_data(WINDP *wp1, WINDP *wp2);
void free_window_data(WINDP *wp);
WINDP *find_buffer_window(FILEBUF *fp);
void free_virtual_window(WINDP *wp);

// word.c
int prev_word(int n);
int next_word(int n);
int goto_eow(int n);
int delete_word(int  n);
int delbword(int n);
int insert_tab(int n);
int remove_tab(int n);
int inword();
int fillpara(int n);
int killpara(int n);
int countwords(int n);
int linefill(int ident);

// main.c
void edinit(char *bname);
int execute(int c, int  n);
int quick_close(int n);
int quit(int n);
int record_macro(int n);
int exec_macro(int n);
int nullproc(int n);
int wrap();
int set_key_emulation(int n);

// gui driver

int vsplit_window(int n);

// search.c
int eq(int x,int y);
int forwsearch(int n);
int find_next(int  n);
int backsearch(int n);
int find_previous(int n);
int	scanner(char *patrn, int direct, int beg_or_end);
int	readpattern(char *prompt, char *str, int srch);
int	setpattern(char apat[], char *st,int srch);
void savematch();
void rvstrcpy(char *rvstr, char *str,int maxlen);
int sreplace(int n);
int qreplace(int n);
int replace_next(int n);
int	replaces(int kind, int f,int n);
int delins(int dlength, char *instr);
int forw_stat(int n);
int back_stat(int n);
int lfound(int status,int direction);
offs find_string_inline(char *patrn);
//void set_global(int beg_or_end,int direct);

int select_font(int n);
int select1_font(int n);
int select_font_mono(int n);

// plot.c
void set_plot_timer();
void do_timer_plot();
int transx(double x);
int transy(double y);
int plot_init(int n);
int plot_close(int n) ;
int path_start(int n);
int path_end(int n);
int path_move(int n);
int path_line(int n);
int path_curve(int n);
int set_relative(int n);
int set_absolute(int n);
int plot_frames(int n);
int plot_curve_to(int n);
int plot_clear(int n);
void pl_clear();
int plot_move_to(int n);
int plot_point(int n);
int plot_line(int n) ;
int plot_lineto(int n) ;
int plot_color(int n);
int plot_set_color(int n);
int plot_lwidth(int n);
int plot_resize(int n);
void resize_plot_subwins();
void plot_status(int f,char *st);
int plot_title(int n);
int plot_coord(int n) ;
int plot_rotate(int n);
int plot_translate(int n);
int plot_scale(int n);
int plot_text(int n);
void set_ratio();
void set_coords();
void show_coords();
int plot_fill(int n);
int plot_circle(int n) ;
int plot_rcircle(int n) ;
int plot_elipsep(int n);
int plot_fcolor(int n);
int plot_rectangle(int n);
int plot_triangle(int n);
int plot_arc(int n) ;
int plot_image(int n) ;
int plot_polyline(int n);
int plot_alpha(int n);
int plot_stat(int n) ;
int plot_close_path(int n);
int plot_poly_nostroke(int n);
void toggle_isomode();
void toggle_gridmode();
void toggle_nummode();
void toggle_zoompriority();
void toggle_zoom();
double dmean(double x,double y);
int plot_zoom(int n) ;
int plot_move(int direction) ;
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

int set_fontindex();
int increase_font_size(int n);
int decrease_font_size(int n);

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
int   ReplaceBlock(char *block,offs size);
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
void update_lines();
int init_ftype(FILEBUF *bf,char *file_name,int *temp_used);
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
int show_info(int);
void undo_set_glue();
void undo_set_noglue();
offs NextNLines(offs,num);
void textpoint_link(TextPoint *,FILEBUF *fp);
void textpoint_unlink(TextPoint *);
void set_change(FILEBUF *);
void  ResetTextPoints(FILEBUF *bp,int);

/* highlight.c */
void init_highlight();
void set_highlight();
void init_line_txt(WINDP *wp,offs tp_offs);
void init_line_cobol(WINDP *wp,offs tp_offs);

void update_highlight(WINDP *wp);
void init_line_highlight(WINDP *wp);

void set_selection(int);
int get_selection();
void *emalloc(long,char *);
void efree(void *,char *);
char *getcurfword();
void start_interactive();
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
int toggle_bold(int n);
int file_exists(char *fname);

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
int new_note(int n);
int new_cal_note(int n);
int new_todo_note(int n);
int save_note();
int open_notes_dir(int n);
int search_notes_dir(int n);
int search_notes_tagged(int n);
int view_notes_by_date(int n);
int create_notes_db(int n);
int show_calendar_list(int n);
int show_todo_list(int n);
char *get_notes_status();
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
