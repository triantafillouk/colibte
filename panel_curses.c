/*	
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

	panel curses  video driver  
	uses the panel library
	needs wide character support for utf8 character set
*/

#include	"xe.h"
#include	<locale.h>
#define	_XOPEN_SOURCE_EXTENDED	1
#include	<locale.h>
#include	<curses.h>
#include	<stdlib.h>
#include	<panel.h>
#include	<term.h>
#include	<signal.h>
#include	<termios.h>
#include	<sys/ioctl.h>
#include	<fcntl.h>
#include	<glib.h>
#include	<signal.h>
#include "menu.h"

#include "rgb_colors.h"
#include "xthemes.h"

#define	TLEFT	1
#define TRIGHT	0
#define TTOP	1
#define TBOTTOM	0

#define COL_BASIC	8
#define COL_BOLD	A_BOLD
#define CLEAR_BG	0
#define	START_COLOR	17	// define our colors after the first basic 16 colors!

#include "panel_curses.h"

void save_original_colors();
void update_selection();
void restore_original_colors();
int set_tag_view_position(int line,int column);
int change_sort_mode(int mouse_col);
int listdir(int dtype);

BOX *cbox=NULL; // current box
BOX *msg_box=NULL;	// current message box

int drv_type=DRIVER_CURSES;
int vswidth=1;	/* vertical separator width  */
void show_debug_color_attr(COLOR_SCHEME *current_scheme);
num get_lines(FILEBUF *bp);
int imove_top_line(num new_top_line);
int set_sposition(WINDP *wp,int *st, int *l);
void utf_string_break(char *utf_string, int column);
int change_sort_mode(int mouse_col);
offs FNext_wrap_line(WINDP *wp,offs start,int num_lines,int top);

int	colorupdate;
int color_scheme_ind=0;
COLOR_SCHEME *current_scheme=NULL;
char **get_scheme_names();
COLOR_SCHEME *get_scheme_by_index(int scheme_num);

int cursor_showing=0;

extern int drv_numrow;		// current driver screen rows
extern int drv_numcol;		// current driver screen columns
extern int drv_colors;
extern int drv_max_colors;
int	drv_color_pairs=64;
int drv_basic_colors=8;
int drv_accent_size=1;
int is_cygwin=0;
extern FILEBUF *cbfp;

int mousex=0;	/* mouse position x */
int mousey=0;	/* mouse position y */
int mousepx=0;	/* mouse press position x */
int mousepy=0;	/* mouse press position y */
int mouserx=0;	/* mouse release position x */
int mousery=0;	/* mouse release position y */
int text_mouse_keys=0;	/* mouse keys recognised flagin curses */
num text_mouse_time=0;	/* time max for double mouse click  */
char *text_mouse_str=NULL;	/* text mouse terminfo kmous string */
int extended_mouse=0;	/* 1 = ansi mouse, 0 = M mouse  */
int mouse_button=0;
int mouse_active=0;
int drv_initialized=0;

float CLEN=1.0;
int CHEIGHTI=1;

WINDOW *hmenu_window;	/* horizontal menu curses window  */
WINDOW *mesg_window;	/* message line curses window  */
PANEL *hmenu_panel=NULL;
PANEL *mesg_panel=NULL;
// extern int color16_8[BG_COLORS+FG_COLORS];

int color_pair(int fg_color,int bg_color);
void free_virtual_window(WINDP *wp);
void allocate_virtual_window(WINDP *wp);
void drv_set_wvs(WINDP *wp);
void drv_free_win(WINDP *wp);
void drv_window_delete(WINDP *wp);
int check_w_sibling(WINDP *wp,int left,int top,int new_rows);
int check_v_sibling(WINDP *wp,int left,int top,int new_cols);
void move_to_new_position(num new_column,num new_line);

alist *box_list;

extern alist *window_list;
extern MENUS m_topn;
extern alist *color_scheme_list;

 /* from ICU utf8.h modified */
//extern int utf8_countBytes[256];
extern char utfokey[10];
extern int utflen;


void drv_window_delete(WINDP *wp)
{
}

int confirm(char *title, char *prompt,int always)
{
	int c;
	int (*execf)();
	char message[256];
	if(!always && ((int)bt_dval("safe_ops")==0)) {
		return true;
	};
	snprintf(message,256,"%s, %s",title,prompt);
	start_interactive(message);
	entry_mode=KCONFIRM;
	for (;;) {
		c = getcmd();
		execf = key_function(c,0);
		c = normalize(c);
		if(execf!=NULL) {
			if(execf==abort_cmd || c=='G' || c=='N' ) { c='N';}
			else continue;
		};
		if(c!='Y' && c!='N') continue;
		end_interactive();
		if (c=='Y') return(TRUE);
		if (c=='N') return(FALSE);
	}
}

int color_pair(int fg_color,int bg_color)
{
 int cpair;

	if(fg_color<BG_COLORS || fg_color>FG_COLORS+15) MESG("color pair fgcolor out of range %d",fg_color);
	if(bg_color<0 || bg_color>BG_COLORS) MESG("color pair bgcolor out of range %d",bg_color);
	if(drv_colors>8) {
		cpair = COLOR_PAIR((fg_color-BG_COLORS)*FG_COLORS+bg_color+1);
	} else {
		int fg=current_scheme->color_style[fg_color].color_index%8;
		int bg=current_scheme->color_style[bg_color].color_index%8;

		cpair = COLOR_PAIR((fg%8) *8+bg+1);
		if(fg>8) cpair |= COL_BOLD;
		// printf("cpair=%X (%d %d) a=%X\n",cpair,fg%8,bg,fg>8);
	};
	// if(fg_color==COLOR_FG) MESG("- cp: color_fg bg_color=%d pair=%d",bg_color,(fg_color-BG_COLORS)*FG_COLORS+bg_color+1);

 return(cpair);
}

void set_box_cline(int line)
{
	if(cbox==NULL) return;
	cbox->cline=line;
}

int get_box_cline()
{
	cbox->cline = mousey - cbox->y -1;
	return cbox->cline;
}

int select_font_mono(num n)
{
	return true;
}

sigset_t set, old_set;
struct sigaction signal_action, old_action;

void SigHandler(int sig)
{
 if(sig == SIGCONT) {
	sigaction(SIGTTIN, &signal_action,NULL); 
	sigaction(SIGTTOU, &signal_action,NULL); 
	sigaction(SIGTSTP, &signal_action,NULL); 
	reset_prog_mode();
 } else {
	sigaction(sig, &old_action,NULL); 
 	refresh();
	reset_shell_mode();
	kill(getpid(),sig);
 }
}

int set_bg(num n)
{
	kill(getpid(),SIGTSTP);	/* send pause signal!  */
	return(1);
}

extern int new_line(num n);

function_int key_convert_from_mouse(function_int execf)
{
 static int pressed_button=KMOUSE_NONE;
	// MESG("kcfm: pressed_button=%d",pressed_button);
	if( execf == text_mouse_left_press) {
		execf = do_nothing;
		pressed_button = mouse_button_in_box(pressed_button);
	};
	if( execf == text_mouse_right_press) {
		execf = do_nothing;
		pressed_button=KMOUSE_BUTTON3;
	};
	if(execf == text_mouse_pos) {
		// MESG(": mouse_move");
		execf=mouse_move;
	}
	if(execf == text_mouse_release) {
		if(pressed_button==KMOUSE_UPDATE) {
			execf=update_menu;
		} else
		if(pressed_button==KMOUSE_BUTTON1) execf=new_line;
		else if(pressed_button==KMOUSE_ENTER) execf=new_line;
		else if(pressed_button==KMOUSE_GOTO) {
			execf=(function_int)goto_line;
		} else if(pressed_button==KMOUSE_BUTTON3||pressed_button==KMOUSE_EXIT) execf=abort_cmd;
		else execf=do_nothing;
		pressed_button=KMOUSE_NONE;
	}
	if(execf == text_mouse_error) {
		execf=do_nothing;
	};
	return execf;
}

char *drv_info()
{
  static char info[256];
  snprintf(info,256,"ncurses colors=%d color pairs=%d basic colors=%d",drv_colors,drv_color_pairs,drv_basic_colors);
  return info;
}


GWINDP * drv_new_twinp()
{
 GWINDP *gwp;
 gwp = (GWINDP *) malloc(sizeof(struct GWINDP));
 if(gwp == NULL) exit(1);
 gwp->t_xpos=0;
 gwp->t_ypos=0;
 gwp->draw=NULL;
 gwp->panel=NULL;
 gwp->vline=NULL;		// right vertical line window
 gwp->vline_panel=NULL;	// right vertical line panel
 gwp->back_xpos=0;
 gwp->back_ypos=0;
 gwp->back_rows=0;
 gwp->back_cols=0;
 gwp->h_flags=0;
 return gwp;
}

WINDOW *drv_new_win(char *label,int h,int w,int y,int x)
{
 WINDOW *win;
	win=newwin(h,w,y,x);
	return(win);
}

/*
 * Delete the current window, rearange screen space
 * Bound to ^X0.
 */
int delete_window(num n)
{
	WINDP *wp=NULL;	/* loop window */ 
	WINDP *w1=NULL;	/* first window to go and receive deleted space */
	WINDP *to_delete = cwp;
	int is_first=1;
	float p_up=0.0;
	float p_down=0.0;
	float p_left=0.0;
	float p_right=0.0;
	float p;
	int cx1,cy1,cx2,cy2;
	int wx1,wy1,wx2,wy2;


	cx1=cwp->gwp->t_xpos;cx2=cx1+cwp->w_ntcols;
	cy1=cwp->gwp->t_ypos;cy2=cy1+cwp->w_ntrows;
	/* if there is only one window, don't delete it */
	if(window_list->head->next == NULL) return(FALSE);

	/* resize windows after delete  */
	lbegin(window_list);
	while((wp=lget(window_list))!=NULL) {
		if(wp==cwp) continue;
		wx1=wp->gwp->t_xpos;wx2=wx1+wp->w_ntcols;
		wy1=wp->gwp->t_ypos;wy2=wy1+wp->w_ntrows;
		if(wx1 == cx2+vswidth) {	/* right  */
			if(cy1 == wy1 && cwp->w_ntrows == wp->w_ntrows) {
				p_right=1.0;
				wp->gwp->t_xpos = cx1;	/* change xpos  */
				wp->w_ntcols += cwp->w_ntcols + vswidth;	/* add width of deleted to the right window  */
				if(is_first) { w1=wp; is_first=0;};
				drv_set_wvs(wp);
				set_window_width(wp);
			} else
			if(wy1>=cy1 && wy2<= cy2) {
				p = (float)(wy2-wy1)/(cy2-cy1);
				p_right += p;
				wp->gwp->t_xpos = cx1;	/* change xpos  */
				wp->w_ntcols += cwp->w_ntcols + vswidth;	/* add width of deleted to the right window  */
				if(is_first) { w1=wp; is_first=0;};
				drv_set_wvs(wp);
				set_window_width(wp);
			};
			if(p_right >= 1.0) break;
		}; 
		if(cx1 == wx2+vswidth) {	/* left  */
			if(cy1 == wy1 && cwp->w_ntrows == wp->w_ntrows) {
				p_left=1.0;
				wp->w_ntcols += cwp->w_ntcols + vswidth; 	/* add widht of deleted to the left window  */
				if(is_first) { w1=wp; is_first=0;};
				drv_set_wvs(wp);
				set_window_width(wp);
			}else
			if(wy1>=cy1 && wy2<= cy2) {
				p = (float)(wy2-wy1)/(cy2-cy1);
				p_left += p;
				wp->w_ntcols += cwp->w_ntcols + vswidth; 	/* add widht of deleted to the left window  */
				if(is_first) { w1=wp; is_first=0;};
				drv_set_wvs(wp);
				set_window_width(wp);
			};
			if(p_left >= 1.0) break;
		}; 
		if(wy1 == cy2) { 	/* bottom  */
			if(cx1 == wx1 && cwp->w_ntcols == wp->w_ntcols) {
				p_down = 1.0;
				wp->gwp->t_ypos = cy1;
				wp->w_ntrows += cwp->w_ntrows;
				if(is_first) { w1=wp; is_first=0;};
				drv_set_wvs(wp);
			}else
			if(wx1>=cx1 && wx2<= cx2) {
				p = (float)(wx2+vswidth-wx1)/(cx2-cx1);
				p_down += p;
				wp->gwp->t_ypos = cy1;
				wp->w_ntrows += cwp->w_ntrows;
				if(is_first) { w1=wp; is_first=0;};
				drv_set_wvs(wp);
			};
			if(p_down >= 1.0) break;
		};
		if(cy1 == wy2) { 	/* top  */
			if(cx1 == wx1 && cwp->w_ntcols == wp->w_ntcols) {
				p_up = 1.0;
				wp->w_ntrows += cwp->w_ntrows;
				if(is_first) { w1=wp; is_first=0;};
				drv_set_wvs(wp);
			} else
			if(wx1>=cx1 && wx2<= cx2) {
				p = (float)(wx2+vswidth-wx1)/(cx2-cx1);
				p_up += p;
				wp->w_ntrows += cwp->w_ntrows;
				if(is_first) { w1=wp; is_first=0;};
				drv_set_wvs(wp);
			};
			if(p_up >= 1.0) break;
		};
		set_window_width(wp);
	};

	if(w1==NULL) {
		drv_msg_line("cannot delete this window");
		return(false);
	};

	/* remove current window */
	unlink_filebuf(to_delete);
	lfind_data((void *)to_delete,window_list);

	remove_current_from_list(window_list);
	drv_free_win(to_delete);

	set_current_window(w1,"delete_window");
	free_window_data(to_delete);

	set_update(cwp,UPD_WINDOW|UPD_ALL);
	update_status();
	return(TRUE);
}

void drv_free_win(WINDP *wp)
{
	free_virtual_window(wp);
	if(wp->gwp == NULL) return;
	if(wp->gwp->panel)	del_panel(wp->gwp->panel);
	if(wp->gwp->draw) delwin(wp->gwp->draw);
	if(wp->gwp->vline_panel) del_panel(wp->gwp->vline_panel);
	if(wp->gwp->vline) delwin(wp->gwp->vline);
	wp->gwp->panel=NULL;wp->gwp->draw=NULL;
	wp->gwp->vline=NULL;wp->gwp->vline_panel=NULL;
}

// create vertical right line (show_position)
void create_rline(WINDP *wp)
{
	if(wp->gwp->vline_panel != NULL) del_panel(wp->gwp->vline_panel);
	if(wp->gwp->vline !=NULL) delwin(wp->gwp->vline);

	wp->gwp->vline=drv_new_win("vline",wp->w_ntrows,1,wp->gwp->t_ypos,wp->gwp->t_xpos+wp->w_ntcols);
	wp->gwp->vline_panel=new_panel(wp->gwp->vline);
#if	CLEAR_BG
	if(wp->gwp->vline!=NULL) {
		wbkgd(wp->gwp->vline,color_pair(COLOR_FG,COLOR_SELECT_BG));
	};
#endif
}

void set_1window()
{
 cwp->gwp->t_ypos = 1;
 cwp->w_ntrows = drv_numrow-1;

 cwp->gwp->t_xpos = 0;
 cwp->w_ntcols = drv_numcol-1;
 set_window_width(cwp);
 drv_set_wvs(cwp);
}


/*
 * Split the current window horizontally
 * Bound to "C-X 2". 
 */
int split_window(num n)
{
        register WINDP *wp;
        register int    ntru;
        register int    ntrl;
		int status=0;
 		if(!drv_initialized) return 0;
       if (cwp->w_ntrows < 16) {
                msg_line("Cannot split a %d line window", cwp->w_ntrows);
                return (true);
		}

		wp = dublicate_window(cwp);
        ntru = (cwp->w_ntrows) / 2;         /* Upper size           */
        ntrl = (cwp->w_ntrows-1) - ntru;      /* Lower size           */
		drv_free_win(cwp);
		cwp->w_ntrows = ntru;
		drv_set_wvs(cwp);
		wp->gwp->t_ypos = cwp->gwp->t_ypos+ntru;
		wp->gwp->t_xpos = cwp->gwp->t_xpos;
        wp->w_ntrows = ntrl+1;
		wp->w_ntcols = cwp->w_ntcols;
		set_window_width(wp);
		set_update(wp,UPD_ALL|UPD_STATUS);
		drv_set_wvs(wp);
		status=next_window(1);
		if(cwp->w_fp->dir_num == 1 && cbfp->b_flag & FSNLIST) {
			listdir(wp->w_fp->dir_num + 1);
		};

		return (status);
}


/*
 * Split the current window vertically
 * Bound to "C-X 3". 
 */
int vsplit_window(num n)
{
    WINDP *wp;
    int    ntl;
    int    ntr;
	int	status=0;	
 	if(!drv_initialized) return 0;
    if (cwp->w_ntcols < 40) {
            msg_line("Cannot split a %d column window", cwp->w_ntcols);
            return (true);
    }

		wp = dublicate_window(cwp);
        ntl = (cwp->w_ntcols) / 2;         /* left size           */
        ntr = (cwp->w_ntcols+1-vswidth) - ntl;      /* right  size           */
		drv_free_win(cwp);
		cwp->w_ntcols = ntl-1;
		set_window_width(cwp);
		drv_set_wvs(cwp);
		wp->gwp->t_ypos = cwp->gwp->t_ypos;
		wp->gwp->t_xpos = cwp->gwp->t_xpos+cwp->w_ntcols+vswidth;

		wp->w_ntcols = ntr;
		set_window_width(wp);
        wp->w_ntrows = cwp->w_ntrows;
		set_update(wp,UPD_ALL|UPD_STATUS);
		drv_set_wvs(wp);
		status=next_window(1);
		if((cwp->w_fp->dir_num == 1) && (cbfp->b_flag & FSNLIST)) {
			listdir(wp->w_fp->dir_num + 1);
		};
		return (status);
}

// allocate window virtual screen
void drv_set_wvs(WINDP *wp)
{
 //if(wp->vs !=NULL) free_virtual_window(wp);
 allocate_virtual_window(wp);

	if(wp->gwp->panel != NULL) del_panel(wp->gwp->panel);
	if(wp->gwp->draw != NULL) {	delwin(wp->gwp->draw);	};

	wp->gwp->draw = drv_new_win("vtinit:cwp",wp->w_ntrows,wp->w_ntcols,wp->gwp->t_ypos,wp->gwp->t_xpos);
	wp->gwp->panel = new_panel(wp->gwp->draw);

	create_rline(wp);
//	backup also dimensions
	wp->gwp->back_xpos=wp->gwp->t_xpos;
	wp->gwp->back_ypos=wp->gwp->t_ypos;
	wp->gwp->back_rows=wp->w_ntrows;
	wp->gwp->back_cols=wp->w_ntcols;
	wp->gwp->h_flags=0;
}

void drv_restore_wdimensions(WINDP *wp)
{
	wp->w_ntrows=wp->gwp->back_rows;
	wp->w_ntcols=wp->gwp->back_cols;
	set_window_width(wp);
	wp->gwp->h_flags=0;
}




int set_key_function(int (*kfunc)(int), int c,char *);
int getkcmd(char *k);

int init_sighandler();

void drv_bindkeys();
void putpad(char *str);



void set_wvs(WINDP *wp);

int color_menu_fg,color_menu_bg;

void set_current_scheme(int scheme);

/* set the screen/main window size */
void drv_size()
{
	getmaxyx(stdscr,drv_numrow,drv_numcol);
	drv_numrow--;
	refresh();
}

void enable_key_mouse()
{
 mouse_active=1;
#if	SOLARIS
	return;
#else
 mousemask(ALL_MOUSE_EVENTS|REPORT_MOUSE_POSITION,NULL);
 printf("\033[?1003h");	/* makes terminal report mouse mouvement  */
 if(extended_mouse) {
 	printf("\033[?1006h");	/* makes terminal extended report mouse mouvement  */
 }
 fflush(stdout);
#endif
}

void disable_key_mouse()
{
 mouse_active=0;
#if	SOLARIS
	return;
#else
 mousemask(0,NULL);
 printf("\033[?1003l");	/* makes terminal stop reporting mouse mouvement  */
 if(extended_mouse) {
 	printf("\033[?1006l");	/* makes terminal stop reporting mouse mouvement  */
 };
 fflush(stdout);
#endif
}

int toggle_mouse(num n)
{
	if(mcurflag) mcurflag=FALSE;else mcurflag=TRUE;
	if(mcurflag) {
		enable_key_mouse();
		msg_line("enable mouse control");
	} else {
		disable_key_mouse();
		msg_line("disable mouse control");
	};
	set_update(cwp,UPD_MOVE);
	return 1;
}

void print_colors(char *title)
{
 int i,j;
 fprintf(stderr," ----- %s -- colors = %d ---\n",title,drv_colors);
 for(i=0;i<8;i++) {
		short r,g,b;
		fprintf(stderr,"color %2d: ",i);
		for(j=0;j<2;j++){
			color_content(j*8+i,&r,&g,&b);
			fprintf(stderr,"%04d %04d %04d  ",r,g,b);
		};fprintf(stderr,"\n");
 };
}

/* Initialize real screen (curses)  */
void drv_open()
{
// setlocale(LC_ALL,"en_US.UTF-8"); 
 setlocale(LC_ALL,"");	// do not use specific locale, get it from the system !!
 init_sighandler();

 initscr();
 noecho();
 raw();
 nonl();

 if(mcurflag){
// keypad(stdscr,1);	/* no need for the moment, need a lot of changes!  */
#if	!SOLARIS
	mousemask(ALL_MOUSE_EVENTS|REPORT_MOUSE_POSITION,NULL);
#endif
	printf("\033[?1003h\n");	/* makes terminal report mouse mouvement  */
 };

 refresh();
 drv_size();
 // initialize colors
 color_scheme_ind=(int)bt_dval("color_scheme")-1;
 start_color();
 drv_colors=COLORS;
 drv_color_pairs=COLOR_PAIRS;
 if(drv_color_pairs>255) {
 	drv_basic_colors=16;
	drv_colors=16;
 };
 save_original_colors();
 color_scheme_read();

 if(drv_colors>drv_max_colors) drv_colors=drv_max_colors;
	// MESG("set drv_colors to %d color pairs are %d",drv_colors,drv_color_pairs);

	color_menu_fg=COLOR_MENU_FG;
	color_menu_bg=COLOR_MENU_BG;

// print_colors("Original ones");
   // MESG("drv_open: color_scheme_ind=%d",color_scheme_ind);
   set_current_scheme(color_scheme_ind+1);	/* set default midnight theme  */

 // driver specific keyboard bindings
 drv_bindkeys();
 vswidth=1;
 drv_initialized=1;
 // MESG("drv_open:end");
}

int drv_search_dialog(int f)
{
	return FALSE;
}

WINDP *get_mouse_window()
{
 static WINDP *wp;
 int y=mousey+1;

 lbegin(window_list);
 while((wp=(WINDP *)lget(window_list))!=NULL){
	GWINDP *gwp = wp->gwp;
	if(	(mousex >= gwp->t_xpos && mousex< gwp->t_xpos+wp->w_ntcols+1) &&
		(y > gwp->t_ypos && y< gwp->t_ypos+wp->w_ntrows)) {
		return wp;
	};
 };
 return NULL;
}

int is_in_top_menu()
{
 int in_top_menu;
	if(mousey==0) in_top_menu=TRUE;
	else in_top_menu=FALSE;
	return in_top_menu;
}

int get_menu_index_from_mouse()
{
 static int top_menu_size=0;
 int ind=0;
 ind = mousex/HLABEL_WIDTH;
 if(top_menu_size==0) {
 	MENUS *main_menu=&m_topn;
	int i=0;
	for(i=0;i<100;i++) {
		if(main_menu->element[i].high==0) {
			top_menu_size=i-1;break;
		};
	};
 };
 if(ind>top_menu_size) {
 	ind=-1;
 };
 return ind;
}

WINDP *is_in_status()
{
 static WINDP *wp;
 int y=mousey+1;

 lbegin(window_list);
 while((wp=(WINDP *)lget(window_list))!=NULL){
	GWINDP *gwp = wp->gwp;
	if(	(mousex >= gwp->t_xpos && mousex< gwp->t_xpos+wp->w_ntcols) &&
		(y == gwp->t_ypos+wp->w_ntrows)) {
		return wp;
	};
 };
 return NULL;
}

extern MENUS m_select_on;

/* get various elements from mouse position */
 /* window , buffer, line, character offset */ 
int text_mouse_function(int move) 
{
 offs new_offset;
 WINDP *wp;
 int i;
 int mouse_window_row,mouse_window_col; 
 static num start_line=0;
 static num start_col=0;
 static int mouse_started_in_rline=0;
 num new_line;

	if(mouse_button==KMOUSE_NONE) {
		return 0;
	};
 // MESG("text_mouse_function: button=%d move=%d",mouse_button,move);

 if(is_in_top_menu()) {
	if(move==KMOUSE_RELEASE+KMOUSE_BUTTON1){
		int f1;
		MENUS *main_menu=&m_topn;
		f1 = get_menu_index_from_mouse();
		if(f1<0) return (FALSE);
		lastkey= main_menu->element[f1].high;
		menu_command(1);
	}
 	return TRUE;
 };
 // MESG("text_mouse_function:1");
	wp = get_mouse_window();
	if(wp==NULL) {
		wp=is_in_status();

		if(wp) { 
			if(wp!=cwp) {
				// MESG("mouse changed window!");
				change_window(wp);
				return -1;
			} else {
			// MESG("text_mouse_function: move=%X",move);
				if(move==KMOUSE_RELEASE+KMOUSE_BUTTON1){
					mouse_started_in_rline=0;
//					MESG("reset mouse_started: release 1");
				};
				if(move==KMOUSE_RELEASE+KMOUSE_BUTTON1){
					mouse_window_col = mousex - wp->gwp->t_xpos - wp->w_infocol;
					// MESG("text_mouse_function: col=%d",mouse_window_col);
					if((cbfp->b_flag & FSNLIST) && (mouse_window_col<5)) {
						// dir_left(0);
						// update_screen(FALSE);
						return(0);
					} else {
//						MESG("mouse: x=%d",mouse_window_col);
						select_file(1);
						return -1;
					};
				
				};
			};
		} else {
			// MESG("text_mouse_function: return -1");
			return -1;
		};
	} else {
	if(wp!=cwp) {
		// MESG("change window with mouse!");
		// reset_region_textpoints();
		set_mark(0);
		change_window(wp);
	};
	/* We are in an editors window */
	mouse_window_row = mousey - wp->gwp->t_ypos;
	mouse_window_col = mousex - wp->gwp->t_xpos - wp->w_infocol;
	// MESG("	r=%d c=%d",mouse_window_row,mouse_window_col);
	if(move>KMOUSE_RELEASE) {
		mouse_started_in_rline=0;
		start_line=0;
		start_col=0;
		// MESG("reset mouse_started, button released!");
	} else {
		// MESG("mouse pressed mb=%d move=%d row=%d col=%d",mouse_button,move,mouse_window_row,mouse_window_col);
	};
	if(mouse_button==KMOUSE_BUTTON1 && move<KMOUSE_RELEASE){
		// MESG("	1: mouse button1 press! move=%d < %d",move,KMOUSE_RELEASE);
		if((mouse_window_col+wp->w_infocol)==wp->w_ntcols) { // on rline (position status line)
			int start,len;
			int lines_to_move;
			int top_line=tp_line(cwp->tp_hline);
			set_sposition(wp,&start,&len);
			mouse_started_in_rline=1;
			if(get_lines(wp->w_fp)>10000) lines_to_move=get_lines(wp->w_fp)/(wp->w_ntrows-1);
			else lines_to_move=wp->w_ntrows-1;	// one page move

			if(	cbfp->b_flag & FSNLIST 
#if	TNOTES
				|| cbfp->b_flag == FSNOTES || cbfp->b_flag & FSNOTESN
#endif
				) {
				if(mouse_window_row<start){
					return(prev_page(1));
				} else if(mouse_window_row>start+len) {
					return(next_page(1));
				};							
			} else {

//			MESG("set mouse_started");
			if(mouse_window_row < start) {
//				MESG("move up: to %d-%d = %d",top_line,lines_to_move,top_line-lines_to_move);
				return imove_top_line(top_line-lines_to_move);
			} else if(mouse_window_row > start+len) {
//				MESG("move down: to %d+%d = %d",top_line,lines_to_move,top_line+lines_to_move);
				return imove_top_line(top_line+lines_to_move);
			} else return(0);
			};
		};
		
//		if cursor pressed on info column and if in dir/view mode the go left
		if((cbfp->b_flag & (FSNLIST))) {
			if (mouse_window_col<0) {
				dir_left(0);
				update_screen(FALSE);
				return(0);
			};
			if(mouse_window_row<1) {
				if(change_sort_mode(mouse_window_col)){
					update_screen(TRUE);
					// return(0);
				};
				return(0);
			};
#if	NUSE
			else {
				// MESG("mouse dir mode");
				if(cbfp->b_flag & FSNLIST) {
				if(cwp->top_note_line+mouse_window_row > cbfp->dir_list_str->size) return FALSE;
				if(cwp->current_note_line == cwp->top_note_line + mouse_window_row-1) {
					// MESG("dir _right 2 line=%d %d",mouse_window_row,cwp->current_note_line);
					if(cbfp->b_flag & FSDIRED){
					dir_right(0);
					} else {
						edit_note(0);
					};
					update_screen(FALSE);
					return(0);
				} else 
					cwp->current_note_line=cwp->top_note_line + mouse_window_row-1;
					set_update(cwp,UPD_EDIT);
					update_screen(FALSE);
					return(0);
				};
			}
#endif
		} 
	};
	if(mouse_window_col<0) mouse_window_col=0;	/* inside info left column  */
	if(is_wrap_text(cbfp)) new_offset=tp_offset(cwp->tp_hline);
	else new_offset=LineBegin(tp_offset(cwp->tp_hline));

	// MESG("tp_hline:1 new_offset=%ld row=%d",new_offset,mouse_window_row);
	int head_line=(cbfp->b_header!=NULL);
	for(i=head_line;i<mouse_window_row;i++) 
	{
		if(is_wrap_text(cbfp)) 
		new_offset = FNext_wrap_line(cwp,new_offset,1,1);
		else 
  		new_offset = FNextLine(cbfp,new_offset);
	};
	// MESG("text_mouse_function: new_offset=%ld mouse_row=%d ",new_offset,mouse_window_row);
	if(mouse_button==KMOUSE_BUTTON1 && move<KMOUSE_RELEASE) {
	// MESG("	mouse release!");
		if(mouse_window_col==wp->w_width) {
			// MESG("move<KMOUSE_RELEASE");
			return 0;
		};
		if(mouse_started_in_rline) {
//			MESG("in rline !");
			return 0;
		};
		// MESG("tp_current:beginning: new_offset=%ld b_flag=%X",new_offset,cbfp->b_flag);
		if(cbfp->b_flag & FSNLIST) {
				// MESG("mouse dir mode");
				if(cwp->top_note_line+mouse_window_row > cbfp->dir_list_str->size) return FALSE;
				if(cwp->current_note_line == cwp->top_note_line + mouse_window_row-1) {
					// MESG("dir _right 2 line=%d %d",mouse_window_row,cwp->current_note_line);
					if(cbfp->b_flag & FSDIRED){
					dir_right(0);
					}
#if	TNOTES
					 else {
						edit_note(0);
					};
#endif
					update_screen(FALSE);
					return(0);
				} else {
					cwp->current_note_line=cwp->top_note_line + mouse_window_row-1;
					set_update(cwp,UPD_EDIT);
					update_screen(FALSE);
					return(0);
				};
		} else {
			textpoint_set(cwp->tp_current,new_offset);
			new_line=tp_line(cwp->tp_current);
			int wrap_column=tp_col(cwp->tp_current);

			// MESG("	new_offset:1 %ld %ld set line %ld col %ld mouse_col=%d",
				// new_offset,tp_offset(cwp->tp_current),tp_line(cwp->tp_current),tp_col(cwp->tp_current),mouse_window_col);
			 if(move==KMOUSE_DBLCLICK)
			 {
				move_to_new_position(mouse_window_col+wrap_column,new_line);
				select_current_word();
				return 0;
			 };
			if(move==0) {
//				MESG("move_to_new_position mouse_window_col=%ld new_line=%ld",mouse_window_col,new_line);
				if(
#if	TNOTES
					cwp->w_fp->b_flag & FSNOTES || cwp->w_fp->b_flag & FSNOTESN ||
#endif
					cwp->w_fp->b_flag & FSNLIST ) {
					new_line=mouse_window_row-1;
					mouse_window_col=set_tag_view_position(new_line,mouse_window_col);
					set_update(cwp,UPD_EDIT);
					// MESG("mouse_window_col=%d line=%d",mouse_window_col,new_line);
				} else {
				start_line=new_line;
				start_col = wrap_column+mouse_window_col;
				// MESG("	move_to_new_pos: mouse_col=%d wrap_column=%d new_line=%d",mouse_window_col,wrap_column,new_line);
				move_to_new_position(wrap_column+mouse_window_col,new_line);
				};
			} else {
				move_to_new_position(wrap_column+mouse_window_col,start_line);
				if(cwp->selection==0) {
					/* start selection  */
//					MESG("mouse move: start selection: mouse_col=%d o=%lld charlen=%d start_col=%lld",
//						mouse_window_col,Offset(),charlen(cwp->w_fp,Offset()),start_col);
					cwp->selection=1;
					set_xmark(cwp,start_col,start_line-tp_line(cwp->tp_hline),0);
				} else {
//					MESG("mouse move: move  selection: mouse_col=%d o=%lld charlen=%d start_col=%lld",
//						mouse_window_col,Offset(),charlen(cwp->w_fp,Offset()),start_col);
					set_xmark(cwp,wrap_column+mouse_window_col,new_line - tp_line(cwp->tp_hline),1);
				};
				set_update(cwp,UPD_EDIT);
			}
		};
	};
	// MESG("	text_mouse_function:1");
	if(mouse_button==KMOUSE_BUTTON3 && move<KMOUSE_RELEASE) {
		if(cwp->selection){
			int status;
			status = work_menu(1);
			mouse_button = KMOUSE_NONE;

			set_selection(false);
	   		set_xmark(cwp,mouse_window_col,mouse_window_row,2); /* remove marks */
			set_update(cwp,UPD_EDIT);
			return status;
		} else {
			mouse_button = KMOUSE_NONE;
			if(cbfp->b_flag & FSDIRED) {
				dir_left(0);
				return 1;
			} else {
				int status = work_menu(1);
				mouse_button = KMOUSE_NONE;
				return status;
			};
		}
	};
	}
	return(-1);
}

void drv_post_init()
{
 hmenu_window = drv_new_win("drv_post_init: hmenu",1,drv_numcol,0,0);
 if(hmenu_panel==NULL) hmenu_panel = new_panel(hmenu_window);
 else replace_panel(hmenu_panel,hmenu_window);
 mesg_window = drv_new_win("drv_post_init: mesg",1,drv_numcol,drv_numrow,0);

 if(mesg_panel==NULL) mesg_panel = new_panel(mesg_window);
 else replace_panel(mesg_panel,mesg_window);
// test_accent();
 top_menu(1);
}

int getstring(char *prompt, char *st1,int maxlen,int disinp)
{
 return win_getstring(mesg_window,prompt, st1,maxlen,disinp);
}

void drv_post_windows_update()
{
	top_menu(0);
	doupdate();
}

void drv_init(int argc, char **argp)
{
  box_list=new_list(0,"drv_init: box_list");
}

/* Close screen driver  */
void drv_close()
{
	restore_original_colors();
#if	!SOLARIS
	use_default_colors();
	disable_key_mouse();
#endif
	endwin();
}


/* Flush/resync */
void drv_flush()
{
	refresh();
}

void drv_win_flush(WINDP *wp)
{
	wnoutrefresh(wp->gwp->draw);
}

/*
 * Read a character from the terminal, performing no editing and doing no echo
 * at all.
 */
int drv_getc(int quote)
{
 int key;
	key=getch();
	if(quote){
		return(key);
	};
	lastkey=key;
#if	DOUBLE_ESC
	return lastkey;
#endif
	if(lastkey==27) {
		// the next key should have a timeout
		timeout(50);
		lastkey=0;
		return 27;
	} else {
		if(lastkey<0) {	
			lastkey=27;	/* timedout send abort (esc)  */
		};
		timeout(-1);	/* wait for ever  */
		return lastkey;
	};
}

static int checking_break_key=0;

void drv_start_checking_break()
{
#if	!SOLARIS
	nodelay(stdscr,TRUE);
	qiflush();
#endif
	if(drv_initialized) checking_break_key=1;
}

void drv_stop_checking_break()
{
#if	!SOLARIS
	nodelay(stdscr,FALSE);
	qiflush();
#endif
	checking_break_key=0;
	utflen=0;
	utfokey[0]=0;
}

int drv_check_break_key()
{
 int key=0;
 static int count=0;
 if(checking_break_key) {
 count++;
 // MESG("drv_check_break_key: %d",count);
 if(count>10000) {
 	key=getch();
	count=0;
 };
 if(key==3) { set_break();return 1;}
 };
 return 0;
}

// Full screen clear
void drv_back_color()
{
 bkgdset(color_pair(COLOR_FG,COLOR_BG));
 clear();
// MESG("drv_back_color:");
#if	CLEAR_BG
 WINDP *wp;
 lbegin(window_list);
 while((wp=(WINDP *)lget(window_list))!=NULL)
 {
	wbkgd(wp->gwp->draw,color_pair(COLOR_FG,COLOR_BG));
 };
#endif
}

extern int 	(*get_function(char *))();

struct  BFK {
	char *func_name;
	char *fkey;	// tinfo variable
	char *description;	
	};

struct BFK bk[] = {
	{"bol",				"khome","Home key"},
	{"bol",				"home","Home1 key"},
	{"bol",				"kf57",	"Home2 key"},
	{"eol",				"kend", "End key"}, /* key end */
	{"eol",				"kf51", "End1 key"},

	{"next_character",	"kcuf1","Arrow right"},
	{"next_character",	"kf56","Arrow1 right"},
	{"prev_character",	"kcub1","Arrow left"},
	{"prev_character",	"kf54","Arrow1 left"},
	{"find_previous",	"kf8","key F8" },
//	{"bof",				"kHOM"}, /* shifted home */
#if	DARWIN
	{"set_mark",		"kc3", "^Space"},
#else
	{"next_file",		"kc3", "^Page DN"}, /* keypad 3 = bottom right */
#endif
	{"next_window",		"kb2", "Center key"}, /* keypad 5 or center key */
	{"prev_word",		"kcmd", "key cmd"},
	{"next_word",		"kcrt", "key crt"},
	{"next_line",		"kcud1", "Arrow down"},
	{"next_line",		"kf52", "Arrow1 down"},

	{"page_move_up",		"knp", "Pg Dn"},
	{"page_move_up",		"kf53", "Pg1 Dn"},
	{"page_move_down",		"kpp", "Pg Up"},
	{"page_move_down",		"kf59", "Pg1 Up"},

	{"prev_line",		"kcuu1", "Arrow up"},
//	{"prev_line",		"kf58", "Arrow1 up"},
	{"toggle_over",		"kich1", "Insert key"},
	{"toggle_over",		"kf50", "Inser1 key"}, 
	{"find_next",	"kfnd","Find key"}, /* key find */
	{"find_next",	"kcan", "^Down arrow"},

	{"exec",			"kf9", "key F9"},
	{"show_info",		"kf1", "key F1"},
	{"set_arg",			"kf2","key F2"},
	{"quick_close",		"kf3", "key F3"},
	{"describe_key",	"kf4", "key F4"},
#if	DARWIN
    {"describe_key",    "kf13", "key F13"},
    {"record_macro",    "kf14", "key f14"},
    {"exec_macro",      "kf15", "key F15"},
#endif
	{"find_next",		"kf5", "key F5"},
	{"find_previous",	"kf6", "key F6"},
	{"record_macro",	"kf7", "key F7"},
	{"find_previous",	"kFND", "^Up arrow"}, /* cltr + key8 */
	{"find_next",		"kfnd", "^Down arrow"}, /* cltr + key2 */
	{"exec_macro",		"kf8", "key F8"},
	{"next_window",		"kf39", "^numeric 9"},
	{"next_file",		"kf10", "key F10"},
	{"toggle_sync_lock",	"kf11", "key F11"},
	{"toggle_move_lock", "kf12", "key F12"},
//	Shift function keys (shift F1 is F13)

	// {"show_tag_view",	"kf13", "key shift F1"},
	// {"show_calendar_listw",	"kf14", "key shift F2"},
	{"toggle_sync_lock",	"kf23", "key shift F11"},

	{"del_char",	"kdch1", "delete key"},
	{"del_prev_char",	"kbs", "backspace"},
//	{"find_next", "knxt", "key Next" },	// key next	- knxt
//	{"find_previous", "kprv", "key Previous" },	// key previus	- kprv
	{"next_hmark","knxt","Next mark"},
	{"prev_hmark","kprv","Previous mark"},
	{"insert_indent","kent","Enter key"},
	{"bof","kf37","^numeric 7"},
	{"eof","kf31","^numeric 1"},
	{"","",""}
};

/* bind keys and get init keyboard codes */
void drv_bindkeys()
{
 int i;
 char *k1;
 int (*kfunc)(int);
 const char *termvalue;
 char default_mouse_str[] = { 27,'[','M',0 };
// MESG("drv_bindkeys:");
 termvalue = getenv("TERM");
 if(!strcmp(termvalue,"cygwin")) is_cygwin=1;
	text_mouse_str = tigetstr("kmous");	/* ncurses mouse  */
	if(text_mouse_str !=NULL) {
		text_mouse_keys=1;
		mouse_active=1;
		if(text_mouse_str[2]=='<') extended_mouse=1;
		if(mcurflag) enable_key_mouse();
	} else {
		text_mouse_str = tigetstr("Km");
		if(is_cygwin) {
			text_mouse_keys=1;
			text_mouse_str = default_mouse_str;
		};
	};
	/* bind function keys */	
	for(i=0;bk[i].func_name[0] !=0;i++) {
		k1 = tigetstr(bk[i].fkey);
		if(k1 > 0) { 
		kfunc = get_function(bk[i].func_name);
		set_key_function(kfunc,getkcmd(k1),bk[i].description);
		}
	};
}

/* return 0 if not inside the box  */
int get_mouse_box_line()
{
	if(cbox) {
		if(
			(cbox->x < mousex && mousex < cbox->x2) &&
			(cbox->y < mousey && mousey < cbox->y2)
		) {
			return mousey - cbox->y;
		} else {
			return 0;
		}
	};
	return 0;
}

int mouse_button_in_box(int mouse_button)
{
	if(cbox) {
		if(
			(cbox->x < mousex && mousex < cbox->x2) &&
			(cbox->y < mousey && mousey < cbox->y2)
		)
		{
			if((cbox->cline+cbox->y+1) == mousey) {
				return KMOUSE_ENTER;
			} else {
				return KMOUSE_GOTO;
			}
		} else {
			if(is_in_top_menu()) return KMOUSE_UPDATE;
			else return KMOUSE_EXIT;
		}
	} else return 1;
	return mouse_button;
}

int getanum(int *end_key)
{
 int num=0;
 int c;
 while((c=getch()) >= '0' && c<='9'){
 	num = num*10 + (c-'0');
 };
 *end_key = c;
 return num;
}


int get_mouse_key()
{
 int c;
	int mouse_key='C';
	int button=0;
	int last_key=' ';
	if(extended_mouse==0) {
		c = get1key();
		mousex=get1key()-33;
		mousey=get1key()-33;
	if(c==' ') mouse_key=c;
	if(c=='#') mouse_key=c;
	if(c=='`') mouse_key=c;
	if(c==CHR_DQUOTE) mouse_key=c;
	if(c=='a') mouse_key=c;
	if(c=='@') mouse_key='C';	/* left move  */
	if(c=='B') mouse_key='C';	/* right move  */
	if(c=='S') mouse_key='C';	/* control move  */
	if(c=='0') mouse_key=32;	/* control left press  */
	if(c=='P') mouse_key='C';	/* control left move  */
	if(c=='R') mouse_key='C';	/* control right move  */
	if(c=='2') mouse_key=34;	/* control right press  */
	if(c=='#') mouse_key=35;	/* control release */
	if(c=='3') mouse_key=35;
	if(c=='K') mouse_key='C';	/* alt move  */
	if(c=='H') mouse_key='C';	/* alt left move  */
	if(c=='J') mouse_key='C';	/* alt right move  */
	if(c=='+') mouse_key=35;	/* alt release  */
	if(c== 40) mouse_key=32;	/* alt left press  */
	if(c== 42) mouse_key=34;	/* alt right press  */
	if(c=='[') mouse_key='C';	/* contol alt move  */
	if(c=='8') mouse_key=32;	/* contol alt left press  */
	if(c=='X') mouse_key='C';	/* contol alt left move  */
	if(c==':') mouse_key=34;	/* contol alt right press  */
	if(c==';') mouse_key=35;	/* contol alt release  */
	if(c=='Z') mouse_key='C';	/* contol alt right move  */				
	if(c=='G') mouse_key='C';	/* shift move  */
	if(c=='$') mouse_key=32;	/* shift left press */
	if(c=='D') mouse_key='C';	/* shift left move  */
	if(c=='\'') mouse_key=35;	/* shift release  */
	if(c=='&') mouse_key=34;	/* shift right press  */
	if(c=='F') mouse_key='C';	/* shift right move  */
	if(c=='e') mouse_key='`';	/* shift scroll up  */
	if(c=='f') mouse_key='a';	/* shift scroll down  */
	if(c=='q') mouse_key='`';	/* control scroll up  */
	if(c=='p') mouse_key='a';	/* control scroll down  */
	if(c=='i') mouse_key='`';	/* alt scroll up  */
	if(c=='h') mouse_key='a';	/* alt scroll down  */
	
	} else {	/* extended mouse support  */
		c='C';	/* set to move  */
		button = getanum(&last_key);
		mousex=getanum(&last_key)-1;
 		mousey=getanum(&last_key)-1;
		if(button==64||button==65){
			if(button==64) mouse_key=96;	/* down  */
			if(button==65) mouse_key=97;	/* up  */
		} else {
			if(last_key=='M'){	/* press  */
				mouse_key=32+button%8;
				if(button==32) mouse_key='C';
			} else {	/* release  */
				mouse_key=35;
				if(button==35) mouse_key='C';
			};
		};
	}

//	MESG("gmkey:cbutton=%2d mk=%d last=%c x=%d y=%d",button,mouse_key,last_key,mousex,mousey);
	if(mouse_key==0) mouse_key='C';

	if(mouse_key==32 || mouse_key==34) {	/* left,right press  */
	 mousepx=mousex;
	 mousepy=mousey;
	};
	if(mouse_key==35) {
	 mouserx=mousex;
	 mousery=mousey;
	};
	if(mouse_key==96 || mouse_key==97) {
		WINDP *wp;
		wp = get_mouse_window();
		set_current_window(wp,"mouse scroll");
	};
// button 0 mouse_key=32 press left
// button 0 mouse_key=35 release left
// button 2 mouse_key=67 right press/release ???
// button 35 mouse_key=67 mouse move

//	MESG("mkey: %d %c [%c] x=%d y=%d",mouse_key,mouse_key,c,mousex,mousey);
	return mouse_key;
}

int text_mouse_key(int *c)
{
	if(text_mouse_keys && mouse_active)
	{
		if(*c==text_mouse_str[2]) 
		{
			*c = KMOUS+get_mouse_key();
			return TRUE;
		};
		if(extended_mouse && *c=='M') {
			// has been falled-back to default, (wsl console sometimes do that!)
			extended_mouse=0;
			*c = KMOUS+get_mouse_key();
			// put back to extended mode
			disable_key_mouse();
			enable_key_mouse();
			extended_mouse=1;
			msg_line("mouse has been reset!!");
			return TRUE;
		};
	};
	return FALSE;
}

void drv_win_move(WINDP *wp,int row,int col)
{
	 wmove(stdscr,wp->gwp->t_ypos+row,wp->gwp->t_xpos+col);
}

void drv_move(int row, int col)
{
	wmove(stdscr,row,col);
}

void drv_wcolor(WINDOW *wnd, int afcol, int abcol)
{
 int fcolor=afcol % 0x100;
 int attrib=0;
 if(afcol & FONT_STYLE_UNDERLINE) {
 	attrib |=A_UNDERLINE;
 } else {
 	int a = current_scheme->color_style[fcolor].color_attr;
#if	!SOLARIS
	if(a & FONT_STYLE_ITALIC) attrib |= A_ITALIC;
#endif
	if(a & FONT_STYLE_BOLD) attrib |= A_BOLD;
	if(a & FONT_STYLE_DIM) attrib |= A_DIM;
	if(a & FONT_STYLE_REVERSE) attrib |= A_REVERSE;
 };
#if	0
 if(afcol & FONT_STYLE_ITALIC) attrib |= A_ITALIC;
 if(afcol & FONT_STYLE_BOLD) attrib |= A_BOLD;
 if(afcol & FONT_STYLE_DIM) attrib |= A_DIM;
 if(afcol & FONT_STYLE_REVERSE) attrib |= A_REVERSE;
#endif
  // if(attrib>0) MESG("- attrib %X fcolor=%X afcol=%X",attrib,fcolor,afcol);
  wattrset(wnd,color_pair(fcolor,abcol)|attrib);
}

void drv_update_styles()
{
 WINDP *wp1;
 lbegin(window_list);
 while((wp1=(WINDP *)lget(window_list))!=NULL){
	drv_set_wvs(wp1);
 };
 top_menu(1);
}

void putpad(char *str)
{
	while(*str) putchar(*str++);	
}

int getkcmd(char *k)
{
 int c,c1;
 unsigned char c0,c2,c3;
 int n1=0;
 int n2=0;
 c=c1=0;
 if(k==NULL) { 
	/* ERROR("Null key");*/
	return(0);};
 if(*k == 27) {
  c=META;k++;
  c2=*k;
  if(c2 == 'O' || c2=='N' || c2 == CHR_LBRA) 
  { 
	switch(c2) {
	case 'O': c1=SPEC;break;
	case CHR_LBRA: { c1=SPEC1;k++;
	  c0 = *k++;
	  if(c0==CHR_LBRA) { c1=SPEC; c0 = *k++; c1 |= c0; return(c1); };
	  if(c0>='0' && c0<='9') {
	    c0 -= '0';
		n1 = c0;
		// fprintf(stderr,"lbra: n1=%X\n",n1);
	    c3 = *k++;
	    if(c3>='0' && c3<='9') { // second digit of first numeric
		c0 *=10; c0 += c3-'0';
		n1=c0;
		c3=*k++;
	    };
	    c1 |= c0;
	    if(c3=='^') { c1 |= 128;}
		else {
			if(c3==';'){
				c=*k++;
				c-='0';
				n2=c;
				c0=*k;
				if(c0 >= 'a') c0 -= 'a';
				else c0 -= 'A';
				c1 = SPEC2 + 100*n1+32*n2+c0;
				// fprintf(stderr,"spec2: SPEC2=%X n2=%X c0=%X c1=%X\n",SPEC2,n2,c0,c1);
			}
		}
	    return(c1);
	  };
	  if(c0==0) c0='@';
	  c1 |= c0;
	  return(c1);  
	break;
	};
	};
	k++;
	c=c1 | *k;
  }  else

   { c |= *k; };
 } 

 else 
 if (*k == 1) {
    c1 = SPEC;
    k++;
    c = c1 | *k;
 } 

 else 
  if(*k < 32 ) c=CNTRL | (*k + 64);
 
 else if(*k == 127) { c = 127;};
 return(c);
}


void hide_cursor(char *from) 
{
	// MESG("hide_cursor: from %s",from);
	curs_set(0);
}

void show_cursor(char *from) 
{
	if(!entry_mode) {
		if(
#if	TNOTES
			cbfp->b_flag==FSNOTES || cbfp->b_flag==FSNOTESN || 
#endif
			cbfp->b_flag & FSNLIST) curs_set(0);
		else curs_set(1);
		update_panels();
		// MESG("show_cursor:!");
		doupdate();
	};
}

void show_cursor_dl(int pos) 
{
 if(entry_mode){
	wmove(mesg_window,0,pos);
	curs_set(1);
	wrefresh(mesg_window);
 };
}

void hide_cursor_dl() { curs_set(0); }

void disp_box(char *box_title,int border,int y1,int x1,int y2,int x2)
{
 // push box -------------------------
 cbox=(BOX *)malloc(sizeof(struct BOX));
 // MESG("disp_box: drv_numrow=%d mousey=%d y1=%d y2=%d size=%d",drv_numrow,mousey,y1,y2,sizeof(cbox->title));
 strlcpy(cbox->title,box_title,sizeof(cbox->title));
 // MESG("disp_box:[%s] width=%d",cbox->title,x2-x1);
 if(y1<0) {
 	y1=mousey;
	if( (y1+y2)> drv_numrow) {
		y1 = drv_numrow-y2-1;
	};
	y2 = y1+y2+1;
 };

 if(x1<0) {
 	x1=mousex;
	if( (x1+x2)> drv_numcol) {
		x1 = drv_numcol-x2-1;
	};
	x2 = x1+x2+1;
 };
 cbox->wnd=drv_new_win("box",y2-y1+1,x2-x1+1,y1,x1);
 cbox->panel=new_panel(cbox->wnd);
 cbox->border=border;

 drv_wcolor(cbox->wnd,COLOR_MENU_FG,COLOR_BOX_BG);	/* in case we want it a different color!  */

 cbox->y=y1;
 cbox->x=x1;
 cbox->x2=x2;
 cbox->y2=y2;
 cbox->cline=0;
 box(cbox->wnd,0,0);
 if(strlen(cbox->title)>0){
 	wmove(cbox->wnd,0,1);
	utf_string_break(cbox->title,cbox->x2-cbox->x-2);
 	wprintw(cbox->wnd,"%s",cbox->title);
 };

 wrefresh(cbox->wnd);
 top_panel(cbox->panel);
 update_panels();
 lpush(cbox,box_list);
}

void start_interactive(char *prompt)
{
	// MESG("start_interactive:[%s]",prompt);
	entry_mode=KENTRY;
	disp_box(prompt,1,4,5,6,60);
	box_line_print(0,0," Y/N",58,0,-1);
	hide_cursor_dl();
}

void end_interactive()
{
	// MESG("end_interactive: entry_mode=%d");
	remove_box();
	entry_mode=KNORMAL;
	set_update(cwp,UPD_STATUS);
}

// remove box and show info behind it
void remove_box()
{
 cbox=(BOX *) lpop(box_list);
 if(cbox==NULL) return;
 hide_panel(cbox->panel);
 update_panels();
 del_panel(cbox->panel);
 if(cbox->wnd!=NULL) delwin(cbox->wnd);

 cbox->wnd=NULL;
 cbox->panel=NULL;
 free(cbox);
 cbox=NULL;
 update_panels();
}

// dummy function for xlib compatibility
void xupdate_box()
{
	wrefresh(cbox->wnd);
}

#if	USE_GLIB
/* Convert a current local string to utf for output */
char *str_local_to_utf(WINDP *wp,char *in)
{
 char *slocal;
 gsize r,w;
 static char *space=" ";
 int lcp=default_local_codepage;
 if(wp->w_fp->b_lang!=0) lcp=wp->w_fp->b_lang;
 slocal = g_convert(in,-1,"UTF-8",codepage_str[lcp],&r,&w,NULL);

 if(slocal!=NULL) {
 	return slocal;
 }
 else return space;
}
#endif

int check_v_sibling(WINDP *wp,int left,int top,int new_cols)
{
 WINDP *wp1;

 lbegin(window_list);
 
 while((wp1=(WINDP *)lget(window_list))!=NULL){
	if(wp1==cwp) continue;
	if(wp1->gwp->h_flags) continue;	/* already updated  */
	if(
		(!top && wp->gwp->t_ypos+wp->w_ntrows == wp1->gwp->t_ypos) ||	/* if on the bottom  */
		(top && wp1->gwp->t_ypos+wp1->w_ntrows == wp->gwp->t_ypos) 		/* or on the top  */
	){
		if(
		 (wp->gwp->t_xpos+wp->w_ntcols == wp1->gwp->t_xpos+wp1->w_ntcols && left) ||	/* they have the same right boundary */
		 (wp->gwp->t_xpos == wp1->gwp->t_xpos && !left)	/* they have the same left boundary  */
		){
			if(wp1->w_ntcols+new_cols<10) { wp1->gwp->h_flags=-1;return false;};
			wp1->gwp->h_flags = 2;
			if(
				(top && wp1->gwp->t_ypos>0) ||		/* not a top window  */
				(!top && wp1->gwp->t_ypos+wp->w_ntrows<drv_numrow-2) 	
			){
				if(!check_v_sibling(wp1,left,top,new_cols)) return false;	/* check it  */
			};
			if(!left) wp1->gwp->t_xpos -= new_cols;
			wp1->w_ntcols += new_cols;
			set_window_width(wp1);
			return true;
		};
	};
 };

 return true;
}

int hresize_wind(num n)
{
/*
 horizontal resize window 
 it means resize the rows of it.
*/
 
 WINDP *wp=NULL;
 WINDP *wp1=NULL;
 int error=0;

 if(cwp->w_ntrows+n<3) return false; 
 lbegin(window_list);
 while((wp1=(WINDP *)lget(window_list))!=NULL){
	wp1->gwp->h_flags=0;	/* set update flag */
 };
 if(cwp->gwp->t_ypos>1) {	/* current window not on the top of screen  */

	lbegin(window_list);
	while((wp1=(WINDP *)lget(window_list))!=NULL){
		if(wp1->gwp->h_flags) continue;	/* already checked  */
		if(wp1->gwp->t_ypos+wp1->w_ntrows == cwp->gwp->t_ypos){	/* common top boundary with current  */
			_el *cwin=window_list->current;
			if(cwp->gwp->t_xpos+cwp->w_ntcols-1 <wp1->gwp->t_xpos ||
				cwp->gwp->t_xpos > wp1->gwp->t_xpos+wp1->w_ntcols) {
					continue;
				};
			wp=wp1;

			// check siblings left and right with common boundary with wp1
			if(wp1->w_ntrows-n<3) { wp1->gwp->h_flags=-1;error=701;break;};
			wp1->gwp->h_flags =2;

			if(!check_w_sibling(wp1,0,TBOTTOM,-n)) { error=702;break;};
			if(!check_w_sibling(wp1,1,TBOTTOM,-n)) { error=703;break;};
			window_list->current=cwin;

			wp1->w_ntrows -=n;
		}; 
 	};
	if(wp==NULL) return false;	/* no window found to give/take space  */

	if(!check_w_sibling(cwp,0,TTOP,n)) { error=704;}
	else {
	if(!check_w_sibling(cwp,1,TTOP,n)) { error=705;} 
	if(!error)	{
		cwp->gwp->h_flags=2;
		cwp->w_ntrows +=n;	
		cwp->gwp->t_ypos-=n;
	};
	};
 } else {	/* current window on top of screen  */

 	lbegin(window_list);
	while((wp1=(WINDP *)lget(window_list))!=NULL){
		if(wp1->gwp->h_flags) continue;	/* already checked  */

		if(cwp->gwp->t_ypos+cwp->w_ntrows == wp1->gwp->t_ypos){
			_el *cwin=window_list->current;
			if(cwp->gwp->t_xpos+cwp->w_ntcols-1 <wp1->gwp->t_xpos ||
				cwp->gwp->t_xpos > wp1->gwp->t_xpos+wp1->w_ntcols) continue;
			wp=wp1;
			// check siblings left and right with common boundary with wp1
			if(wp1->w_ntrows-n<3 || wp1->gwp->t_ypos+n> drv_numrow-3 ) { error=706;wp1->gwp->h_flags=-1;break;};
			wp1->gwp->h_flags=2;

			if(!check_w_sibling(wp1,0,1,-n)) { error=707;break;}
			if(!check_w_sibling(wp1,1,1,-n)) { error=708;break;} 
			window_list->current=cwin;

			wp1->gwp->t_ypos+=n;
			wp1->w_ntrows -=n;
		}; 
	}; 
	if(wp==NULL) return false;	/* no window found to give/take space  */

	if(!check_w_sibling(cwp,0,0,n)) { error=709;}
	else { 
	if(!check_w_sibling(cwp,1,0,n)) { error=710;}
	if(!error) {
		cwp->gwp->h_flags=2;
		cwp->w_ntrows +=n;	/* set  */

	};
	};
 };
 lbegin(window_list);
 if(error) {
	 while((wp=lget(window_list))!=NULL) {
 		drv_restore_wdimensions(wp);
	 }
 } else {
 	while((wp=lget(window_list))!=NULL) {
		if(wp->gwp->h_flags!=0)	drv_set_wvs(wp);
 	};
 }
 set_update(cwp,UPD_ALL);
 return true;
}

int vresize_wind(num n)
{
/*
 vertical resize window 
 it means resize the columns of it.
*/
 
 WINDP *wp=NULL;
 WINDP *wp1=NULL;
 int error=0;

 if(cwp->w_ntcols+n<10) return false; 
 lbegin(window_list);
 while((wp1=(WINDP *)lget(window_list))!=NULL){
	wp1->gwp->h_flags=0;	/* set update flag */
 };

 if(cwp->gwp->t_xpos>0) {	/* current window not on the left of the screen  */
	lbegin(window_list);
	while((wp1=(WINDP *)lget(window_list))!=NULL){
		if(wp1->gwp->h_flags) continue;	/* already checked  */
		if(wp1->gwp->t_xpos+wp1->w_ntcols+1 == cwp->gwp->t_xpos){	/* common left boundary with current  */
			_el *cwin=window_list->current;
			if(cwp->gwp->t_ypos+cwp->w_ntrows < wp1->gwp->t_ypos ||
				cwp->gwp->t_ypos > wp1->gwp->t_ypos+wp1->w_ntrows) {
					continue;
				};
			wp=wp1;

			if(wp1->w_ntcols-n<10) { wp1->gwp->h_flags=-1;error=712;break;};
			wp1->gwp->h_flags =2;

			if(!check_v_sibling(wp1,TLEFT, TBOTTOM,-n)) { error=713;break;};
			if(!check_v_sibling(wp1,TLEFT ,TTOP,-n)) { error=714;break;};
			window_list->current=cwin;

			wp1->w_ntcols -=n;
			set_window_width(wp1);
		}; 
 	};
	if(wp==NULL) return false;	/* no window found to give/take space  */
//	check siblings top and bottom with common boundary with wp1
	if(!check_v_sibling(cwp,TRIGHT,TTOP,n)) { error=715;}
	else {
	if(!check_v_sibling(cwp,TRIGHT,TBOTTOM,n)) { error=716;} 
	if(!error)	{
		cwp->gwp->h_flags=2;
		cwp->w_ntcols +=n;	
		cwp->gwp->t_xpos-=n;
		set_window_width(cwp);
	};
	};
 } else {	/* current window on left of the screen  */
 	lbegin(window_list);
	while((wp1=(WINDP *)lget(window_list))!=NULL){
		if(wp1->gwp->h_flags) continue;	/* already checked  */
		if(cwp->gwp->t_xpos+cwp->w_ntcols+1 == wp1->gwp->t_xpos){
			_el *cwin=window_list->current;
			if(cwp->gwp->t_ypos+cwp->w_ntrows < wp1->gwp->t_ypos ||
				cwp->gwp->t_ypos > wp1->gwp->t_ypos+wp1->w_ntrows) continue;
			wp=wp1;
			// check siblings left and right with common boundary with wp1
			if(wp1->w_ntcols-n<10 || wp1->gwp->t_xpos+n> drv_numcol-10 ) { error=717;wp1->gwp->h_flags=-1;break;};
			wp1->gwp->h_flags=2;

			if(!check_v_sibling(wp1,TRIGHT,0,-n)) { error=718;break;}
			if(!check_v_sibling(wp1,TRIGHT,1,-n)) { error=719;break;} 
			window_list->current=cwin;

			wp1->gwp->t_xpos+=n;
			wp1->w_ntcols -=n;
			set_window_width(wp1);
		}; 
	}; 
	if(wp==NULL) return false;	/* no window found to give/take space  */
	// check siblings left and right with common boundary with wp1
	if(!check_v_sibling(cwp,TLEFT,0,n)) { error=720;}
	else { 
	if(!check_v_sibling(cwp,TLEFT,1,n)) { error=721;}
	if(!error) {
		cwp->gwp->h_flags=2;
		cwp->w_ntcols +=n;	/* set  */
		set_window_width(cwp);
	};
	};
 };
 lbegin(window_list);
 if(error) {
	 while((wp=lget(window_list))!=NULL) {
 		drv_restore_wdimensions(wp);
	 }
 } else {
 	while((wp=lget(window_list))!=NULL) {
		if(wp->gwp->h_flags!=0)	drv_set_wvs(wp);
 	};
 }
 set_update(cwp,UPD_ALL);
 return true;
}

/* Check window sibling position */
int check_w_sibling(WINDP *wp,int left,int top,int new_rows)
{
 WINDP *wp1;

 lbegin(window_list);
 
 while((wp1=(WINDP *)lget(window_list))!=NULL){
	if(wp1==cwp) continue;
	if(wp1->gwp->h_flags) continue;	/* already updated  */
	if(
		(!left && wp->gwp->t_xpos+wp->w_ntcols+1 == wp1->gwp->t_xpos) ||	/* if on the right  */
		(left && wp1->gwp->t_xpos+wp1->w_ntcols+1 == wp->gwp->t_xpos) 		/* or on the left  */
	){
		if(
		 (wp->gwp->t_ypos+wp->w_ntrows == wp1->gwp->t_ypos+wp1->w_ntrows && !top) || 	/* same bottom boundary  */
		 (wp->gwp->t_ypos == wp1->gwp->t_ypos && top)									/* same top boundary  */
		){
			if(wp1->w_ntrows+new_rows<3) { wp1->gwp->h_flags=-1;return false;};
			wp1->gwp->h_flags = 2;
			if(
				(left && wp1->gwp->t_xpos>1) ||							/* if not the leftiest window  */
				(!left && wp1->gwp->t_xpos+wp->w_ntcols<drv_numcol-2) 	/* or?? not the rightest */
			){
				if(!check_w_sibling(wp1,left,top,new_rows)) return false;
			};
			if(top) wp1->gwp->t_ypos -= new_rows;
			wp1->w_ntrows += new_rows;
			return true;
		};
	};
 };

 return true;
}


// dummy function
void expose_window(WINDP *wp)
{
	wrefresh(wp->gwp->draw);
	doupdate();
}

void drv_clear_line(WINDP *wp,int row)
{
	wmove(wp->gwp->draw,row,0);
	int i;
	for(i=0;i<wp->w_ntcols;i++) {
		waddstr(wp->gwp->draw,"-");
	};
	wrefresh(wp->gwp->draw);
	doupdate();
	wmove(wp->gwp->draw,row,0);
	// MESG("drv_clear_line: %2d",row);
}

/* Put virtual screen text on physical */
void put_wtext(WINDP *wp ,int row,int maxcol)
{
 int i;
 int imax=0;
 int xcol=0;
 vchar *v1, *vtext;
 VIDEO *vp1;
 int fcolor,bcolor;
 char vstr[8];
 vp1 = wp->vs[row];
 v1=vtext=vp1->v_text;
 // char *unknown2="🔠"; 
 char *unknown1="¤"; 

 fcolor=255;
 bcolor=0;

 maxcol -= 2;	/* ??  */
 memset(vstr,0,8);

 	/* we have a difference */
	v1=vtext;
	imax=0;
	// int ccor=0;

	v1=vtext;

	wmove(wp->gwp->draw,row,xcol);
	imax=maxcol+1;
#if	USE_SLOW_DISPLAY
	if(wp->w_fp->slow_display) 
	{ /* a little bit slower but clears shadow text!  */
		wclrtoeol(wp->gwp->draw);
		wrefresh(wp->gwp->draw);
	 	update_panels();
		doupdate();
	};
#endif
	for(i=0;i<=imax;i++) {
	 uint32_t ch;
	 	if(v1->fcolor < 256) fcolor = v1->fcolor+v1->attr;
		else fcolor = v1->fcolor;
		bcolor = v1->bcolor;

		drv_wcolor(wp->gwp->draw,fcolor,bcolor);
		ch=v1->uval[0];
#if	USE_SLOW_DISPLAY
		if(ch==0xF0) wp->w_fp->slow_display=1;
#endif
		if(ch==0xFF) { 	/* skip in case of char len > 1  */
			if(v1->uval[1]==0xFF) 
			{ 
				// wprintw(wp->gwp->draw,"%c",' ');
				v1++;
				continue;
			};
		};
#if	USE_GLIB
		if(ch>128 && v1->uval[1]==0) {	/* this is a local character, convert from local to utf  */
			 strlcpy(vstr,str_local_to_utf(wp,(char *)v1->uval),6);
			 if(wp->w_fp->b_lang==0)
			 	drv_wcolor(wp->gwp->draw,COLOR_CTRL_FG,bcolor);	/* show local chars with different color  */
			 else
			 	drv_wcolor(wp->gwp->draw,fcolor,bcolor);
		} else {
			memcpy(vstr,v1->uval,6);
		};
#else
		memcpy(vstr,v1->uval,6);
#endif
#if	DARWIN
		// if(vstr[0]>0x20) {MESG("  %2d %2d [%s] [%X %X %X]",row,i,vstr,vstr[0],vstr[1],vstr[2]);};
		if(vstr[0]==0x20) {vstr[0]=0xC2;vstr[1]=0x80;};
#endif
		if	(vstr[0]==0xF0 && vstr[1]==0x9F && vstr[2]!=0x8F && vstr[2]!=0x91 && vstr[2]!=0x92 && vstr[2]!=0x94 && vstr[2]!=0x96 && vstr[2]!=0x98 && vstr[2]!=0xA4 && vstr[2]!=0xA7 ) { 
			wprintw(wp->gwp->draw,"%c",'?');
			// ccor++;
		}
		// else if	(vstr[0]==0xF0 && vstr[1]==0x9F && vstr[2]!=0x94) wprintw(wp->gwp->draw,"%c",'?');
		else if	(vstr[0]==0xF0 && vstr[1]==0x9D) { wprintw(wp->gwp->draw,"%s",unknown1);}
		else if	(vstr[0]==0xF0 && vstr[1]==0x90 && vstr[2]==0x90) wprintw(wp->gwp->draw,"%c",'#');
		else if	(vstr[0]==0xF0 && vstr[2]==0x84) wprintw(wp->gwp->draw,"%c",'~');
		else if	(vstr[0]==0xF3 && vstr[1]==0xA0) wprintw(wp->gwp->draw,"%s",unknown1);
#if	DARWIN
		else if (vstr[0]==0xE2) { 
			if ((vstr[1]==0x9D||vstr[1]==0x9C)) wprintw(wp->gwp->draw,"%s","🠓");
			else 
				waddstr(wp->gwp->draw,vstr);
		}
#endif
		else 
			waddstr(wp->gwp->draw,vstr);
			// wprintw(wp->gwp->draw,"%s",vstr);
		v1++;
	};
	// MESG("row %d eol %d",row,i);

#if	USE_SLOW_DISPLAY
	if(wp->w_fp->slow_display) {
		wrefresh(wp->gwp->draw);
		 update_panels();
		 doupdate();
	} else
#endif
		wnoutrefresh(wp->gwp->draw);
}


/* display with attribute bold in current box */
void xdab(int y,int b,char *st,int bcolor,int fcolor)
{
 int x=1;
 curs_set(0);
 char string_to_show[256];
 char *s=string_to_show;
 int width;
 if(cbox==NULL)  return;
 if(cbox->wnd==NULL) return;
 width = cbox->x2 - cbox->x-2 < 256 ? cbox->x2-cbox->x-2: 256;
// MESG("xdab[%s]",st);
 wmove(cbox->wnd,y,x);
 drv_wcolor(cbox->wnd,fcolor,bcolor);
 strlcpy(string_to_show,st,256);
 utf_string_break(string_to_show,width);
 wprintw(cbox->wnd,"%s",string_to_show);

 for(;*s;x++,s++) {
	if(b==*s) {
		wmove(cbox->wnd,y,x);
		drv_wcolor(cbox->wnd,COLOR_CTRL_FG,bcolor);
		waddch(cbox->wnd,b);
		break;
	};
 };
 wrefresh(cbox->wnd);
}

/*
 line: line of array to print that corresponds to st
 start: top line, so the printed line in the box is line-start.
 st: the string to print
 w: width of the box
 selected: if true the reverse colors
 active_line: or -1
*/
void box_line_print(int line,int start,char *st, int w, int selected,int active_line)
{
 char string_to_show[256];
 int y=line-start;
 int active=(line==active_line);
 int width = cbox->x2 - cbox->x-2 < 256 ? cbox->x2-cbox->x-2: 256;
 int real_width;
 if(active>=0) width--;
 real_width = width + strlen(st)-utf_num_chars(st)+1;
 if(selected)  drv_wcolor(cbox->wnd,COLOR_MENU_FG,COLOR_SELECT_BG);
 else drv_wcolor(cbox->wnd,COLOR_MENU_FG,COLOR_MENU_BG);
 // MESG("box_line_print: y=%d h=%d [%s]",y,cbox->y2 - cbox->y -1,st);
#if	USE_GLIB
 char *normal_st = g_utf8_normalize(st,-1,G_NORMALIZE_ALL_COMPOSE);
 snprintf(string_to_show,256,"%-*s",real_width,normal_st);
 // MESG("[%s] -> [%s]",st,normal_st);
 g_free(normal_st);
#else
 snprintf(string_to_show,256,"%-*s",real_width,st);
#endif
 utf_string_break(string_to_show,width);
 if(y+1 > (cbox->y2 - cbox->y -1)) return;
 wmove(cbox->wnd,y+1,1);

// MESG("box_line_print [%s] [%s] %d",st,string_to_show,f);
 if(active)
#if	USE_UTF8
	wprintw(cbox->wnd,"▶%s",string_to_show);
#else
	wprintw(cbox->wnd,"~%s",string_to_show);
#endif
 else wprintw(cbox->wnd," %s",string_to_show);
 // drv_wcolor(cbox->wnd,COLOR_MENU_BG,COLOR_MENU_FG);

 wrefresh(cbox->wnd);
}

void export_region(ClipBoard *clip)
{
	ext_system_copy();
}

int set_fontindex(num n) {
// dummy function
 return TRUE;
}

int select_font(num n)
{ // dummy function
 return TRUE;
}

int init_drv_env()
{
//	MESG("init_drv_env:");
	half_last_line=0;

	color_scheme_ind=(int)bt_dval("color_scheme")-1;
	// MESG("init_drv_env: color_scheme=%d",color_scheme_ind);
	default_lang=0;	// default is UTF-8

	return DRIVER_CURSES;	/* returns type of driver  */
}

void titletext() {}

void clear_hmenu()
{
 curs_set(0);

 drv_wcolor(hmenu_window,COLOR_MENU_FG,COLOR_MENU_BG);
 wmove(hmenu_window,0,0);
 wprintw(hmenu_window,"%*s",drv_numcol,"  ");
 wnoutrefresh(hmenu_window);

 msg_line(time2a());
}

void window_clear(WINDP *wp)
{
	curs_set(0);
	wclear(wp->gwp->draw);
}

/*
 * Write a message into the status line.
 */
void drv_msg_line(char *arg)
{
	if (discmd == FALSE || macro_exec) return;
	curs_set(0);
	wmove(mesg_window,0,0);	
#if	1
	if(app_error) 
		wbkgd(mesg_window,color_pair(COLOR_FG,COLOR_SEARCH_BG));
	else
		wbkgd(mesg_window,color_pair(COLOR_FG,COLOR_BG));
#endif

	utf_string_break(arg,drv_numcol-1);
	wprintw(mesg_window,"%s",(char *)str2out(arg));
	if(strlen(arg)>0) wclrtoeol(mesg_window);
	wrefresh(mesg_window);
}

/* displays a string at xy and return the real on screen length */
/* put pad at the end */
/* take care of control characters 
   This is used in message line
*/
int dspv(WINDOW *disp_window,int x,int y,char *st)
{
 int c;
 // int len;
 int x_pos,y_pos;
 // len=0;
 // MESG("dspv: x=%d y=%d [%s]",x,y,st);
 hide_cursor("dpsv");
 wmove(disp_window,y,x);

 while((c=*st++)!=0) {
 	if(c<32) {
		drv_wcolor(disp_window,COLOR_CTRL_FG,COLOR_BG);
		waddch(disp_window,'^');
		(c)+=64;
		// len++;
	};
	waddch(disp_window,c);
	drv_wcolor(disp_window,COLOR_FG,COLOR_BG);
 };
 getyx(disp_window,y_pos,x_pos);
 wclrtoeol(disp_window);
 wnoutrefresh(disp_window);
 return(x_pos - x);
}

RGB_DEF original_color[COLOR_TYPES];

void restore_original_colors()
{
 int i;
 // MESG("restore_original_colors:");
 for(i=0;i<COLOR_TYPES;i++) 
 {
 	init_color(i,original_color[i].r,original_color[i].g,original_color[i].b);
 };
}

void save_original_colors()
{
 int i;
 // MESG("save_original_colors:");
 for(i=0;i<COLOR_TYPES;i++) {
 	color_content(i,(short int *)(&original_color[i].r),(short int *)(&original_color[i].g),(short int *)(&original_color[i].b));
 };
}

void show_debug_color_attr(COLOR_SCHEME *current_cscheme)
{
 int i;
 fprintf(stderr,"show_debug_color_attr: scheme=%d\n",color_scheme_ind);
 for(i=0;i<COLOR_TYPES;i++) {
 	fprintf(stderr," color %d attrib %d\n",current_scheme->color_style[i].color_index,current_scheme->color_style[i].color_attr);
 };
}

void set_current_scheme(int scheme)
{
 int i,j;
 // MESG("set_current_scheme: scheme=%d drv_colors=%d",scheme,drv_colors);
 if(scheme<1 || scheme> color_scheme_list->size) scheme=1;

 color_scheme_ind=scheme-1;
 set_bt_num_val("color_scheme",color_scheme_ind+1); 

 current_scheme = get_scheme_by_index(color_scheme_ind);

 // MESG("init scheme2 %s colors=%d",current_scheme->scheme_name,drv_colors);
 // show_debug_color_attr(current_scheme);
 
 if(drv_colors==0) return;
	// MESG("init scheme3 %s colors=%d",current_scheme->scheme_name,drv_colors);

	if(drv_colors>8) {
		for(i=0;i<FG_COLORS+BG_COLORS;i++) {
			refresh();
			// MESG("	set color %d: %s",i,current_scheme->color_style[i]);
			RGB_DEF *rv = get_rgb_values(current_scheme->color_style[i].color_value);
			init_color(i+START_COLOR,rv->r,rv->g,rv->b);
		};
		for(i=0;i<FG_COLORS;i++) 
			for(j=0;j<BG_COLORS;j++) {
				// MESG(" - pair %3d: f=%d b=%d",i*FG_COLORS+j,i+BG_COLORS,j);
				int pair=i*FG_COLORS+j+1;
				init_pair(pair,i+BG_COLORS+START_COLOR,j+START_COLOR);
				// MESG("init_pair : %d as (%d %d)",pair,i,j);
			};
	} else 
	{
#if	1
		if(can_change_color()) {
		int inited[8] = {0,0,0,0,0,0,0,0};
		for(i=0;i<BG_COLORS+FG_COLORS;i++) {
			int basic = current_scheme->color_style[i].color_index % 8;
			if(inited[basic]==0 ) {
				refresh();
				RGB_DEF *rv = get_rgb_values(current_scheme->color_style[i].color_value);
				init_color(basic,rv->r,rv->g,rv->b);
				inited[basic]=1;
				// MESG("init color: %d (%d %d %d)",basic,rv->r,rv->g,rv->b);
			};
		};
		}
#endif
		// 8 color terminals is supposed to not be able to change color values
		// so we simple define all color pairs
		for(i=0;i<drv_basic_colors;i++) 
			for(j=0;j<drv_basic_colors;j++) {
				int pair=i*drv_basic_colors+j+1;
				init_pair(pair,i,j);
				// MESG("init_pair : %d as (%d %d)",pair,i,j);
			};
	};
// MESG("set_current_scheme:end");
}

void set_cursor(int val,char *from)
{
	cursor_showing=val;
//	MESG("set_cursor: val=%d %s",val,from);
}

/* status of each window  */
void put_string_statusline(WINDP *wp,char *show_string,int position)
{
 int status_row=wp->w_ntrows-1;;
 int maxlen;
 int fg_color=COLOR_MENU_FG;
 int bg_color=COLOR_MENU_BG;
 char *status_string = show_string;
 int rpos=utf_num_chars(status_string)+2;
#if	!CLASSIC_STATUS
 if((drv_color_pairs>63 && drv_colors!=8) && cwp!=wp) {	/* if enough color pairs, use them ! */
 	fg_color=COLOR_INACTIVE_FG;
	bg_color=COLOR_INACTIVE_BG;
 };
#endif
// change foreground color if buffer changed
 if(wp->w_fp->b_state & FS_CHG) {
 	fg_color=COLOR_CHANGE_FG;
 }

 curs_set(0);
 if(position>0)
 {
	if(position>1) {	/* short version  */
		rpos = utf_num_chars(status_string)+2;
	};
	if(wp->w_ntcols-rpos<10) return;
	drv_wcolor(wp->gwp->draw,COLOR_ROWCOL_FG,bg_color);
	wmove(wp->gwp->draw,status_row,wp->w_ntcols-rpos);
	maxlen=wp->w_ntcols-rpos;
 } else {
	drv_wcolor(wp->gwp->draw,fg_color,bg_color);
	wmove(wp->gwp->draw,status_row,0);
	maxlen=wp->w_ntcols;
 };

  utf_string_break(status_string,maxlen);
  wprintw(wp->gwp->draw,"%s",status_string);
}

void refresh_menu()
{
	top_menu(1);
	set_update(cwp,UPD_MOVE);
}

void hdab(int x,int b,char *s,int bcolor,int fcolor)
{
 curs_set(0);
 wmove(hmenu_window,0,x);
 drv_wcolor(hmenu_window,fcolor,bcolor);
 wprintw(hmenu_window,"%s",s);
 for(;*s;x++,s++) {
	if(b==*s) {
		wmove(hmenu_window,0,x);
		drv_wcolor(hmenu_window,COLOR_CTRL_FG,bcolor);
		waddch(hmenu_window,b);
		break;
	};
 };
 wrefresh(hmenu_window);
}

/* Mechanism to resize windows after a screen resize */
#include <signal.h>

void update_rows(WINDP *wp,int old_start_row,int new_start_row,float ratio_h,int prev_h)
{
 int old_ypos;
 	if((wp->gwp->h_flags & 1) ==0) return; 	/* already updated  */
	if(wp->gwp->t_ypos != old_start_row) return;

	old_ypos=wp->gwp->t_ypos + wp->w_ntrows;
	if(wp->gwp->t_ypos+wp->w_ntrows == prev_h) {
		wp->gwp->t_ypos = new_start_row;
		wp->w_ntrows = drv_numrow - wp->gwp->t_ypos;
		wp->gwp->h_flags &= ~1;
	} else {
		WINDP *wp1;
		_el *cwin=window_list->current;
		wp->gwp->t_ypos = new_start_row;
		wp->w_ntrows = round(wp->w_ntrows * ratio_h);
		wp->gwp->h_flags &= ~1;

		lbegin(window_list);
		while((wp1=(WINDP *)lget(window_list))!=NULL){
			update_rows(wp1,old_ypos,wp->gwp->t_ypos + wp->w_ntrows,ratio_h,prev_h);
		};
		window_list->current=cwin;
		// pop wp position in window list
	};
}

void update_cols(WINDP *wp,int old_start_col,int new_start_col,float ratio_w,int prev_w)
{
 int old_xpos;
 	if((wp->gwp->h_flags & 2) ==0) {
		return; 	/* already updated  */
	};
	if(wp->gwp->t_xpos==0) { 
		if(old_start_col!=0) return;
	} else if(wp->gwp->t_xpos != old_start_col+vswidth) return;

	old_xpos=wp->gwp->t_xpos + wp->w_ntcols;
	if(wp->gwp->t_xpos+wp->w_ntcols == prev_w) 
	{
		wp->gwp->t_xpos = new_start_col;
		wp->w_ntcols = drv_numcol - wp->gwp->t_xpos;
		set_window_width(wp);
		wp->gwp->h_flags &= ~2;
	} else {
		WINDP *wp1;
		_el *cwin=window_list->current;
		wp->gwp->t_xpos = new_start_col;
		wp->w_ntcols = round(wp->w_ntcols * ratio_w);
		set_window_width(wp);
		wp->gwp->h_flags &= ~2;

		lbegin(window_list);
		while((wp1=(WINDP *)lget(window_list))!=NULL){
			update_cols(wp1,old_xpos,wp->gwp->t_xpos + wp->w_ntcols+vswidth,ratio_w,prev_w);
		};
		window_list->current=cwin;
		// pop wp position in window list
	};
}

int resize_screen(int init)	/* resize the screen, re-writing the screen */
{
	WINDP *wp;	/* current window being examined */

	static int prev_h=0;
	static int prev_w=0;
	double ratio_h=1,ratio_w=1;

	if(init==2) {
		prev_h=drv_numrow;
		prev_w=drv_numcol;
		return(false);
	};

	ratio_h=(double)(drv_numrow)/(double)prev_h; 
	ratio_w=(double)(drv_numcol)/(double)prev_w; 

	if(init){
	/* delete menu,message windows, they will be created in drv_post_init()  */
		del_panel(hmenu_panel);
		del_panel(mesg_panel);
		hmenu_panel=mesg_panel=NULL;
	
		delwin(hmenu_window);
		delwin(mesg_window);
		hmenu_window=mesg_window=NULL;
	};

/*
	for every top window
		update recursively its botom windows rows and y position.
		if this is one of the bottom windows then it get the remain rows.
	for every left window
		update recursively its right windows columns and x position.
		if this is one of the right windows the it gets the remain columns.
*/
	/* set every window to be resized  */
	lbegin(window_list);
	while((wp=(WINDP *)lget(window_list))!=NULL){
		wp->gwp->h_flags=3;	/* update both vertical and horizontal dimensions  */
	};
	lbegin(window_list);
	while((wp=(WINDP *)lget(window_list))!=NULL){
		update_rows(wp,1,1,ratio_h,prev_h);
	};

	lbegin(window_list);
	while((wp=(WINDP *)lget(window_list))!=NULL){
		update_cols(wp,0,0,ratio_w,prev_w);
	};
	lbegin(window_list);
	while((wp=(WINDP *)lget(window_list))!=NULL){
		drv_set_wvs(wp);
	};

	set_update(cwp,UPD_ALL);
	prev_h=drv_numrow;
	prev_w=drv_numcol;
	return(TRUE);
}

/* update parent size */
static void update_screen_size(void)
{
	static int updating=0;
	if(updating) { 
		return;
	};
	updating=1;

	endwin();
	initscr();
	refresh();
	drv_size();

	resize_screen(1);

	drv_post_init();
	update_screen(TRUE);
	updating=0;
}

static void winch_sig_handler(int signo)
{
 if(signo==SIGTSTP) {
	sigaction(signo, &old_action,NULL); 
 	refresh();
	reset_shell_mode();
	kill(getpid(),signo);
 } else {
 	update_screen_size();
 };
}

/* for catching screen resizing events! */
int init_sighandler()
{
	struct sigaction winch_sig;
	winch_sig.sa_handler = winch_sig_handler;
	sigemptyset(&winch_sig.sa_mask);
	winch_sig.sa_flags = SA_RESTART;
	sigaction(SIGWINCH, &winch_sig, NULL);
	return 0;
}

int system_paste(num n)
{
	return ext_system_paste();
}

int set_color(num n)
{
	return(TRUE);
}

int new_shell(num n)
{
	int status;
	char *cp;
	if ((cp = getenv("DISPLAY")) != NULL && *cp != '\0') {
		status=system("gnome-terminal 2>/dev/null >/dev/null &");
		return(TRUE);
	};
	drv_move(drv_numrow, 0);			 /* Seek to last line.	 */
	drv_wcolor(stdscr,COLOR_FG,COLOR_BG);
	drv_flush();
	drv_close();							  /* stty to old settings */
	if ((cp = getenv("SHELL")) != NULL && *cp != '\0')
		status=system(cp);
	else
		status=system("exec /bin/sh");
	set_update(cwp,UPD_ALL|UPD_STATUS);

	drv_open();
	top_menu(1);
	if(status==0) return(TRUE);
	else return(FALSE);
}

int text_mouse_function(int);

int text_mouse_error(num n)
{
	msg_line("text_mouse error");
	return 1;
}

int text_mouse_pos(num n)
{
	text_mouse_function(1);
	return(FALSE);
}

WINDP *get_mouse_window();

int text_mouse_left_press(num n)
{
 struct timeval timev;
 int double_click=0;
 static num ppress=0;
 num press_time;
 num diff_time=0;

 // MESG("mouse_left_press:");
 mouse_button=KMOUSE_BUTTON1;
 gettimeofday(&timev,NULL);
 press_time=(num) timev.tv_sec*1000000 + (num) timev.tv_usec;
 diff_time = press_time-ppress;
 if(ppress>0) {
	if(diff_time < 200000) { double_click = KMOUSE_DBLCLICK;};
 };
 ppress = press_time;
 text_mouse_function(double_click);
 double_click=0;
 return FALSE;
}

int text_mouse_right_press(num n)
{
 // MESG("mouse_right_press:");
 mouse_button=KMOUSE_BUTTON3;
 text_mouse_function(KMOUSE_BUTTON3);	/* remove selections  */
  return FALSE;
}

int text_mouse_release(num n)
{

	if(mousepx==mouserx && mousepy==mousery) {	/* this is just a click!  */
		// MESG("text_mouse_release: %X",KMOUSE_RELEASE+mouse_button);
		text_mouse_function(KMOUSE_RELEASE+mouse_button);
	} else {
		text_mouse_function(KMOUSE_RELEASE+mouse_button);
	};
	mouse_button=KMOUSE_NONE;
	return FALSE;
}

int open_file_dialog(char *fname,num n)
{
	return(TRUE);
}

void prepare_converter(int local_lang)
{

}

int select1_font(num n)
{
	return(TRUE);
}

int select_scheme(num n)
{
 int nv;
 char **scheme_names;

 scheme_names = get_scheme_names();
 nv = selectl("select scheme",scheme_names,color_scheme_list->size,20,30,1,20,color_scheme_ind);
 sarray_clear(scheme_names);
 if(nv>=0) {
// 	MESG("selected %d scheme",nv);
	change_color_scheme(nv+1);
 };
 return 1;
}

int set_sposition(WINDP *wp,int *st, int *l)
{
 int view_rows;
 int cline;
 num buf_lines=get_lines(wp->w_fp);

 if(wp==NULL) {
 	*st=0;*l=0;
	return 0;
 };
 view_rows = wp->w_ntrows-1;
 if(buf_lines==0) buf_lines=1;

	if(wp->w_fp->b_flag & FSNLIST) cline=wp->top_note_line;
#if	TNOTES
	else if(wp->w_fp->b_flag & FSNOTESN) cline=wp->top_note_line;
	else if(wp->w_fp->b_flag & FSNOTES) cline=wp->top_tag_line;
#endif
	else cline = tp_line(wp->tp_hline);

	*l = (view_rows * view_rows) / (double)buf_lines;
	if(*l<1) *l=1;

//	start/rows = cline/buf_lines => start = cline * rows/buf_lines
	*st = (view_rows)* (double)(cline) / (double)(buf_lines);
	if(*st < 0) *st=0;
	if(*st>view_rows-*l-1) *st=view_rows-*l-1;

 return(cline);
}

void show_slide(WINDP *wp)
{
 int start=0;
 int len, end;
 int row;
// char single_line[5] = { 0xE2,0x94,0x82,0,0};
// █ ▓ │ ┃   ║ ▌ ░ ▒ ▓ ╳
 char double_vline[5] = { 0xE2,0x95,0x91,0,0};
// char full_line[5] = { 0xE2,0x96,0x88,0,0};
// char hatch_line[5] = { 0xE2,0x96,0x93,0,0};
// char left_half[5] = { 0xE2,0x96,0x8C,0,0};
// char xblock[5] = { 0xE2,0x95,0xB3,0,0};
 int fg_color=COLOR_MENU_FG;
 int bg_color=COLOR_MENU_BG;
 curs_set(0);

#if	!CLASSIC_STATUS
 if(cwp!=wp) {
	if(drv_colors>8) {
	 	fg_color=COLOR_INACTIVE_FG;
		bg_color=COLOR_INACTIVE_BG;
	} else {
	 	fg_color=COLOR_MENU_FG;
		bg_color=COLOR_MENU_BG;
	};
 };
#endif

 if(bt_dval("show_position")==0) {
 	return;
 };
 set_sposition(wp,&start,&len);
 end=start+len;
// MESG("show_slide: window=%d start=%d end=%d len=%d",wp->id,start,end,len);

 drv_wcolor(wp->gwp->vline,fg_color,bg_color);
#if	USE_SLOW_DISPLAY
 if(wp->w_fp->slow_display){
 	wbkgd(wp->gwp->vline,color_pair(fg_color,bg_color));
	wrefresh(wp->gwp->vline);
 };
#endif
 for(row=0;row<wp->w_ntrows-1;row++){
	wmove(wp->gwp->vline,row,0);
	// wrefresh(wp->gwp->vline);
	if(row<start || row> end) {
		wprintw(wp->gwp->vline,"%s"," ");
	} else {
		wprintw(wp->gwp->vline,"%s",double_vline);
	};
 };
 wmove(wp->gwp->vline,wp->w_ntrows,0);
 /* show change flag at bottom right corner!  */
 if(wp->w_fp->b_flag & (1 << 1)) {
 	drv_wcolor(wp->gwp->vline,COLOR_CTRL_FG,bg_color);
 	wprintw(wp->gwp->vline,"%s","*");
 } else wprintw(wp->gwp->vline,"%s"," ");
#if	USE_SLOW_DISPLAY
 if(wp->w_fp->slow_display){
 	wnoutrefresh(wp->gwp->vline);
	update_panels();
	doupdate();
 } else
#endif
	wnoutrefresh(wp->gwp->vline);
}

#include "xthemes.c"

/* -- */
