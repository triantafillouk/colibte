
int delete_region();
void toggle_global_flag(int option_flag);
void set_global_flag(int option_flag,int value);
int get_hex_byte ();
void toggle_hex_byte();
void reset_hex_byte();
int next_tab(num i);
int do_nothing(num n);
int is_wrap_text(FILEBUF *fp);

void toggle_val(char *name);
int toggle_parameter(num type);

int goto_bol(num n);
int prev_character(num n);
int goto_eol(num n);
int goto_eol(num n);
int next_character(num n);
int select_next_character(num n);
int select_prev_character(num n);
int select_next_line(num n);
int select_prev_line(num n);
int imove_top_line(num new_top_line);
int igotolinecol(num line,num column,int line_flag);
int igotooffset(num n,int ppline);
int goto_line(num n);
int goto_offset(num n);
int goto_bof(num n);
int dont_edit();
int goto_eof(num n);
int scroll_up(num n);
int scroll_down(num n);

int get_current_line();
int next_wrap_line(num n);
int next_line(num n);
int prev_wrap_line(num n);
int prev_line(num n);
int emptyline(TextPoint *tp);
int comment_line(num n);
int comment_line2(num n);
int goto_bop(num n);
int goto_eop(num n);

int valid_offset(offs o_in_line,int column_goal);

int next_page(num n);
int prev_page(num num_pages);

int set_mark(num n);
int set_no_mark(num n);
int set_line_mark(num n);
int set_column_mark(num n);
int setmark(num n);
int swap_mark(num n);
int match_fence(num flag);

int toggle_move_lock(num n);
int toggle_sync_lock(num n);
int page_move_down(num n);
int page_move_up(num n);
int quit(num n);
int quick_close(num n);
int abort_cmd(num n);
int update_menu(num n);
int noop(num n);
int twiddle(num n);
int ed_tab(num n);
int detab_line(num n);
int entab_line(num n);
int del_blank_lines(num n);
int new_line(num n);
int insert_date(num n);
int del_char(num n);
int del_prev_char(num n);
int erase_eol(num n);
int insert_chr(int n,int c);
int insert_string(FILEBUF *fp,char *c,int size);
int insert_string_nl(FILEBUF *fp,char *str);
int insert_newline(FILEBUF *fp);
int forw_delete(num n);
int back_delete(num n);
int trim_line(num n);
int insert_indent(num n);
int ed_delete_line(num n);
int cut_region(num flag);
int copy_region(num n);
int move_line_up(num n);
int move_line_down(num n);
int paste_region(num n);
int lowerregion(num  n);
int upperregion(num  n);
int delete_box(num n);
int set_over(num n);
int set_case(num n);
int toggle_highlight(num n);
int set_regexp(num n);
int toggle_case(num n);
int toggle_regexp(num n);
int toggle_over(num n);
int set_crypt(num n);
int toggle_crypt(num n);
int set_just(num n);
int toggle_just(num n);
int read_colors(num n);
int save_colors(num n);

void set_global_flag(int option_flag,int value);
void toggle_global_flag(int option_flag);
int set_optionf(int option_flag,int global);
void toggleoptionf(int option_flag, int global);

int set_linetext(char *txt);
int delete_region();
void normalize_region(FILEBUF *bp, offs *offs1, offs *offs2);
unsigned char *str2out(char *s);

int main_clipboard_copy();
void init_clipboard();
ClipBoard * clipboard_new();

