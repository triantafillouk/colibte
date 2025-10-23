
/*
	Display driver needed functions
*/
#if !defined _DISPLAY_DRIVER_FUNCTIONS
#define _DISPLAY_DRIVER_FUNCTIONS

void drv_init(int, char **);
void drv_post_init();
void drv_open();
void drv_close();
int drv_getc(int quote);
void drv_flush();
void drv_move(int,int);
void drv_color(int,int);
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

char *drv_info();
GWINDP * drv_new_twinp();
void events_flush();

GWINDP *new_twinp();

void set_curgwp();
void drv_set_default_bgcolor();
void window_clear(WINDP *wp);
void drv_clear_line(WINDP *wp,int row);
void set_window_font(WINDP *wp);

void window_bg_clear(WINDP *wp);
void drv_color(int fcol,int bcol);
void drv_back_color();
int drv_search_dialog(int n);
int init_drv_env();

#endif