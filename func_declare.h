// edit.c
int quick_close(num n);
int quit(num n);
int save_colors(num n);
int read_colors(num n);
int move_line_up(num);
int move_line_down(num);

// xthemss.h
int color_scheme_read(num);
int change_color_scheme(num  n);

// convert.c
int unix2dos(num n);
int dos2unix(num n);


// file.c
int new_file(num n);
int next_file(num n);
int close_file(num n);
int select_file(num n);
int select_file_internal(num n);

/* window.c  */
int window_row_increase(num n);
int window_column_increase(num n);
int window_row_decrease(num n);
int window_column_decrease(num n);
int toggle_mouse(num n);
int toggle_parameter(num type);
int quote(num n);
int istring(num n);
int execute_menu(num fixed);	/* execute menu */
int menu_command(num n);
int work_menu(num n);

/* bookmark functions */
int set_bmark(num n);
int select_bmark(num n);
int prev_bmark(num n);
int next_bmark(num n);
int clear_bmarks(num n) ;
int current_hmark(num n);
int prev_hmark(num n);
int next_hmark(num n);
int delete_hmark(num flag);
int select_hmark(num n);
int convert_char(num n);

/* help.c */
int help(num n);
int show_keys(num n);
int assign_function(num n);
int assign_sub(num n);
int unassign_key(num n);
int describe_key(num n);
int set_key( num n);

// input.c
int set_key_emulation(num n);
int insert_money_symbol(num); 

// highlight.c
int select_highlight(num n);

// eval.c
int record_macro(num n);
int exec_macro(num n);
int set_arg(num n);
int load_session(num);
int show_session(num);
int save_session(num);
int set_option(num n);
int set_debug(num n);
int refresh_current_line(num n);
int set_locale(num n);
int macro_line(num n);
int execsub(num n);
int exec_file(num n);

// mlang.c
int refresh_current_buffer(num n);
int show_parse_buffer(num n);

// system.c
int update_tags(num n);
int ext_system_paste(num n);
int ext_system_copy(num n);
int shell_cmd(num n);
int bg_cmd(num n);
int grep_cmd(num n);
int rgrep_cmd(num n);
int filter_buffer(num n);
int grep_buffer(num n);
int sysexec(char *st);

// file.c
void edinit(char *bname);
int reload_file(num n);
int insert_file(num  n);
int open_file(num n);
int clear_buffer(num n);
int umark_buffer(num n);
int open_recent_file(num n);
int read_file_history(num n);
int save_file_history(num n);
int saveas_file(num n);
int save_file(num n);

int mouse_move(num n);
#if	1
int text_mouse_pos(num n);
int text_mouse_left_press(num n);
int text_mouse_right_press(num n);
int text_mouse_release(num n);
int text_mouse_error(num n);
#endif
int set_bg(num n);
// window.c
int buffer_in_view(num n);
int move_window(num);
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

// word.c
int prev_word(num n);
int next_word(num n);
int goto_eow(num n);
int delete_word(num n);
int delbword(num n);
int insert_tab(num n);
int remove_tab(num n);
int fillpara(num n);
int killpara(num n);
int countwords(num n);
int linefill(num ident);

// screen functions
int vsplit_window(num n);
int select_font(num n);
int select_font_mono(num n);
int set_fontindex(num n);
int increase_font_size(num n);
int decrease_font_size(num n);
int toggle_bold(num n);
int select_font_mono(num n);
int set_color(num);
int select_scheme(num n);
int update_screen(num);
int show_version(num n);
int new_shell(num n);

// search.c
int forwsearch(num n);
int find_next(num n);
int backsearch(num n);
int find_previous(num n);
int sreplace(num n);
int qreplace(num n);
int replace_next(num n);
int forw_stat(num n);
int back_stat(num n);

// dir.c
int dir_size(num n);
int dir_count_files(num n);
int dir_compare(num n);
int dir_select_reverse(num dummy);
int dir_select_all(num dummy);
int dir_select_none(num dummy);
int set_sort_mode(num mode);
int dir_left(num n);
int dir_right(num n);
int dir_touch_file(num n);
int dir_show_diffs(num n);
int dir_new_dir(num n);
int dir_link(num n);
int dir_reload(num n);
int view_next(num n);
int view_previous(num n);
int dir_exec(num n) ;
int dir_view(num n);
int script_exec(num n) ;
int dir_edit(num n) ;
int dir_copy(num n);
int dir_clone(num n);
int dir_move(num n);
int dir_file_rename(num n);
int dir_del1(num  n) ;
int dir_tag(num n) ;
int dir_showfileinfo(num type) ;
int listdir1(num);
int listdir2(num);

