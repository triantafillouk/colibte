/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

*/

/* XLib terminal support. */

#define KEY_DEBUG 0
#define FONT_DEBUG 0
#define	MOUSE_DEBUG 0

#include    <X11/Xlib.h>
#include	<X11/Xutil.h>
#include	<X11/Xos.h>
#include	<X11/Xatom.h>
#include	<X11/keysym.h>
#include	<X11/Xresource.h>
#include	<X11/cursorfont.h>
#include	<sys/time.h>
#include	<signal.h>
#include	"xe.h"
#include	"xkeys.h"
#include	"xthemes.h"
#include	"menu.h"
#include	"xthemes.c"
#include	"panel_curses.h"

#define XF864		1	/* greek keyboard in XFree86 version 4 */

#define BEL     0x07
#define ESC     0x1B

#define	TLEFT	1
#define TRIGHT	0
#define TTOP	1
#define TBOTTOM	0


int color_menu_fg,color_menu_bg;
#if	1
char *font_names[] = { 
	"-*-fixed-*-*-*-*-*-*-*-*-*-*-ISO10646-1",
	"-misc-*-*-r-*-*-14-*-*-*-*-ISO10646-1",
	"-Misc-Fixed-Medium-R-Normal--14-130-75-75-C-70-ISO10646-1",
	"-Misc-Fixed-Bold-R-Normal--14-130-75-75-C-70-ISO10646-1", 
	"-*-courier-*-r-*-*-17-*-*-*-*-ISO10646-1",
	"-misc-*-*-*-*-*-15-*-*-*-*-ISO10646-1",
	"-misc-*-*-*-*-*-18-*-*-*-*-ISO10646-1",
	"-misc-*-*-*-*-*-20-*-*-*-*-ISO10646-1",
	"-*-courier-*-r-*-*-25-*-*-*-*-ISO10646-1",
	NULL };
#else
char *font_names[] = { 
	"-*-fixed-*-*-*-*-*-*-*-*-*-*-*-7",
	"-misc-*-*-r-*-*-14-*-*-*-*-*-7",
	"7x14",
	"7x14bold", 
	"-*-courier-*-r-*-*-17-*-*-*-*-*-*",
	"-misc-*-*-*-*-*-15-*-*-*-*-*-7",
	"-misc-*-*-*-*-*-18-*-*-*-*-*-7",
	"-misc-*-*-*-*-*-20-*-*-*-*-*-7",
	"-*-courier-*-r-*-*-25-*-*-*-*-*-*",
	NULL };
#endif

int drv_type=DRIVER_XLIB;
int mouse_col=0;	/* mouse position x */
int mouse_row=0;	/* mouse position y */
int mousex=0;
int mousey=0;
extern int e_pos;			// entry position

extern int drv_numrow;		// current driver screen rows
extern int drv_numcol;		// current driver screen columns
extern FILEBUF *cbfp;
WINDOW *mesg_window=NULL;

void init_wslide(WINDP *wp);
int is_in_sbar(int xw);
void drv_win_color(WINDP *wp);
WINDP *is_in_win(int xw);
WINDP *is_in_status(int xw);
void move_to_new_position(num new_column,num new_line);
void mesg_out(const char *fmt, ...);
int set_tag_view_position(int line,int column);

void allocate_virtual_window(WINDP *wp);
void update_screen_size(void);
int update_screen_height(int n);
int update_screen_width(int n);
void set_back_color();
void set_xposition(WINDP *wp,int);
void init_colors();
void init_downline();
void init_slide(WINDP *wp);
void show_slide(WINDP *wp);
void set_wmhints();
void reset_display(char *);
void set_plot_timer();
void show_cursor(char *);
void set_cursor_dl(int px);
void hide_cursor_dl();
void hide_cursor(char *from);
int X_process_events (char *buf, int *nread);
extern int wplot_event(XEvent *,int);
extern int wplotp_event(XEvent *,int);
extern int wplot_tools_event(XEvent *, int);
//WINDP *get_event_window(int event_window);
void set_xmark(WINDP *wp,int,int,int);
void update_selection();
extern void clear_message_line();
void show_subw1();
int x_insert ();

extern void update_box(int,int,int,int);
extern int update_screen(int);
int get_from_mouse(XEvent report,int);
extern int copy_region(int);
extern void update_status();
extern alist *window_list;
extern void set_wvs(WINDP *);

void drv_move(int y,int x);
int set_sposition(WINDP *wp,int *,int *);
int set_fontindex(int n);
int set_font(char *);
void drv_back_color();
void sinsert_nl(char *,int);
void Draw_up_button(GWINDP *gwp,int,int,int);
void Draw_dn_button(GWINDP *gwp,int,int,int);
void draw_middle(WINDP *wp);
void next_font();
int process_scrollbar(WINDP *wp,int,int,int);
int empty_char;

#define	HLINES	1	// display separation lines in menu and status line

WINDP	*new_wp;
extern MENUS	m_topn;
extern int in_menu,in_selection;
extern int update_all;

XSizeHints shints;
#define	SBWIDTH	12
#define	SHOW_SHADOW	1
int in_editor=-1;
int	selection_on=-1;
int color_scheme_ind;
XSizeHints bar_hints;
XSizeHints parent_hints = {
   PMinSize | PResizeInc | PBaseSize | PWinGravity,
   0, 0, 80, 24,	/* x, y, width, height */
   1, 1,		/* Min width, height */
   0, 0,		/* Max width, height - unused*/
   1, 1,		/* increments: width, height */
   {1, 1},		/* increments: x, y */
   {0, 0},		/* Aspect ratio - unused */
   SBWIDTH, 0,		/* base size: width, height */
   NorthWestGravity	/* gravity */
};

extern MENUS *c_menu;		/* current menu */

int px,py,ppx_dl;
int cursor_px=0;
int cursor_py=0;
int CLEN,CHEIGHT,CFONTBASE;
int CLEN_DL, CHEIGHT_DL,CFONTBASE_DL;
int CHEIGHTI=1;

Display *dis0;
int screen_num;
Screen *screen_ptr;

Drawable parent;
int parent_width, parent_height;

WINDP *sbar_window=NULL;

int d_x0,d_y0,d_xl,d_yl;
int box_x,box_y;

Drawable downline=0;
Drawable subw1,wmenu;
Pixmap xsubw1,xwmenu;
int w1_xl,w1_yl;
int dl_height = 17;
GC  dl_gc;
GC	status_gc;

#if	HLINES
GC dlbotom_gc, dltop_gc;
#endif
int in_w1;
int w1_len,w1_height;

int	mapped=0;
int cursor_showing=0;
GC gc;
GC cursor_gc;
static Drawable cursor_draw=0; 

XGCValues gcvalues;
int	press_position_y,press_line;
int press_position_x;
unsigned long valuemask=0;
XFontStruct *font_main;
int font_index =0;
#if	XPLOT
extern Drawable wplot;
extern Drawable wplotp;
extern Drawable wplot_tools;
extern Drawable wplot_sline;
extern Drawable wplot_out;
#endif

Colormap cmap;
XColor col_f, col_b;
int default_depth;
Visual *default_visual;
XColor exact_def;
int ncolors;

int colors[COLOR_SCHEMES][XCOLOR_TYPES];
XVisualInfo visual_info;
unsigned long background_pixel=0L, foreground_pixel=1L;

int is_updated;
int is_cnfupd;

typedef int (*function_int)(int);

function_int key_convert_from_mouse(function_int execf)
{
	return execf;
}

void do_timer(int i)
{

}

void start_interactive(char *prompt)
{
	// MESG("start_interactive:[%s]",prompt);
	entry_mode=KENTRY;
#if	0	// PROMPT_AS_BOX
	disp_box(prompt,2,4,5,6,60);
	// dspv(cbox->wnd,2,1,prompt);
	box_line_print(0,0," Y/N",58,0,0);
	hide_cursor_dl();
#else
	msg_line("%s ",prompt);
	show_cursor_dl(e_pos); 
#endif
}

void end_interactive()
{
	// MESG("end_interactive: entry_mode=%d");
#if	0	// PROMPt_AS_BOX
	remove_box();
#endif
	entry_mode=KNORMAL;
}

int confirm(char *title, char *prompt,int always)
{
	int c;
	int (*execf)();
	char message[256];
	if(!always && ((int)bt_dval("safe_ops")==0)) return true;
	snprintf(message,256,"%s, %s",title,prompt);
	start_interactive(message);
	entry_mode=KCONFIRM;
#if	1
	show_cursor_dl(strlen(message));
#endif
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
#if	1
				clear_message_line();
#endif
		if (c=='Y') return(TRUE);
		if (c=='N') return(FALSE);
	}
}

void drv_init(int argc, char **argv) 
{
}

char *drv_info()
{
  static char *info="xlib driver";
  return info;
}

void drv_post_init()
{
}

void drv_post_windows_update()
{
}

/* this is called from dublicate_window to allocate driver specifics */
GWINDP * drv_new_twinp()
{
 GWINDP *gwp;
 gwp = (GWINDP *) malloc(sizeof(struct GWINDP));
 if(gwp == NULL) exit(1);
 gwp->d_xpos=0;
 gwp->d_ypos=0;
 gwp->d_height=0;
 gwp->d_width=0;
 gwp->h_flags=0;
 gwp->draw=0;
 gwp->sbar=0;
 gwp->top_gc=0;
 gwp->botom_gc=0;
 gwp->sgc=0;
 gwp->sgm=0;
 return gwp;
}

/* Dummy function in this driver  */
int text_mouse_key(int *c)
{
	return FALSE;
}

void expose_window(WINDP *wp)
{
}

void drv_window_xwin(WINDP *wp)
{
 GWINDP *gwp=wp->gwp;
 if(gwp->draw!=0) return;	/* already created  */

 XSetWindowAttributes setwinattr;
 setwinattr.backing_store = Always;

 gwp->draw = XCreateSimpleWindow(dis0, parent,
	gwp->d_xpos,gwp->d_ypos,gwp->d_width,gwp->d_height-dl_height,0,
	foreground_pixel,
	colors[color_scheme_ind][7]
 );

 gwp->status = XCreateSimpleWindow(dis0, parent,
	gwp->d_xpos,gwp->d_ypos+gwp->d_height-dl_height,gwp->d_width,dl_height,0,
	foreground_pixel,
	colors[color_scheme_ind][7]
 );

 valuemask = CWBackingStore ;
 setwinattr.backing_store = Always;

 XChangeWindowAttributes(dis0,wp->gwp->draw,valuemask,&setwinattr); 
 XChangeWindowAttributes(dis0,wp->gwp->status,valuemask,&setwinattr); 


 XMapWindow(dis0, wp->gwp->draw);	/* Display the window */

 XSelectInput(dis0,wp->gwp->draw,
	ExposureMask|
//	StructureNotifyMask|
//	KeyPressMask|
	ButtonPressMask|ButtonReleaseMask|
//	EnterWindowMask | LeaveWindowMask|
	PointerMotionMask
	 );
 XRaiseWindow(dis0,wp->gwp->draw);

 XMapWindow(dis0, wp->gwp->status);	/* Display the window */
 XSelectInput(dis0,wp->gwp->status,
	ButtonPressMask|ButtonReleaseMask|
//	PointerMotionMask|
 	ExposureMask
 	);
 XRaiseWindow(dis0,wp->gwp->status);

 wp->gwp->sbar = XCreateSimpleWindow(dis0, parent,
	gwp->d_xpos+gwp->d_width,gwp->d_ypos, SBWIDTH,gwp->d_height ,0,
	colors[color_scheme_ind][INFOFORE],
	colors[color_scheme_ind][INFOBACK]
	); 

 XChangeWindowAttributes(dis0,wp->gwp->sbar,valuemask,&setwinattr); 

 XMapWindow(dis0, wp->gwp->sbar);	/* Display the window */

 XSelectInput(dis0,wp->gwp->sbar,
	ButtonPressMask|ButtonReleaseMask
	|PointerMotionMask
	);

	top_menu(1);
	/* init slide  */
	XFlush(dis0);
	init_slide(wp);
}

void resize_draw_win(WINDP *wp)
{

 GWINDP *gwp=wp->gwp;

 XMoveResizeWindow(dis0, gwp->draw, gwp->d_xpos, gwp->d_ypos, gwp->d_width, gwp->d_height);
 XMoveResizeWindow(dis0, gwp->sbar, gwp->d_xpos+gwp->d_width, gwp->d_ypos, SBWIDTH, gwp->d_height);
 XMoveResizeWindow(dis0, gwp->draw, gwp->d_xpos, gwp->d_ypos, gwp->d_width, gwp->d_height-dl_height);
 XMoveResizeWindow(dis0, gwp->status, gwp->d_xpos, gwp->d_ypos+gwp->d_height-dl_height, gwp->d_width, dl_height);

 XFlush(dis0);	/* it is needed!!!  */
}

void drv_open() 
{
 static int opened=0;
 char *display_name = NULL; 
 int lenx,leny;
 if(opened) return;
 opened=1;

 XSetWindowAttributes setwinattr;
 dis0 = XOpenDisplay(display_name);
 if(dis0==NULL) { ERROR("cant open X display");exit(-1);};
 screen_num = DefaultScreen(dis0);
 screen_ptr = DefaultScreenOfDisplay(dis0);
 lenx = 70;
 leny = 25;
 init_colors();

 CLEN = 10; CHEIGHT=20;
 CHEIGHTI=CHEIGHT;

 parent_width = CLEN*lenx+SBWIDTH;
 parent_height = CHEIGHT*leny+2*dl_height;

 shints.flags = PSize | PResizeInc | PMinSize | PBaseSize ;
 shints.max_height = DisplayHeight(dis0,screen_num) ;
 shints.max_width  = DisplayWidth(dis0,screen_num) ;

 parent_hints.max_height = shints.max_height;
 parent_hints.max_width =  shints.max_width;

 parent_hints.base_height=2*dl_height;
 bar_hints.max_width  = SBWIDTH ;

 parent = XCreateSimpleWindow(dis0, RootWindow(dis0,screen_num),
 	0,0, parent_width,parent_height,0,
	foreground_pixel,
	background_pixel
	);

 mapped=1;
 wmenu = XCreateSimpleWindow(dis0, parent,
	0,0 , parent_width, dl_height,0,
	BlackPixel(dis0,screen_num),
	colors[color_scheme_ind][MODEBACK]
	);

 downline = XCreateSimpleWindow(dis0, parent,
	0,1,1,1,0,
	BlackPixel(dis0,screen_num),
	colors[color_scheme_ind][MODEBACK]
	);

 init_downline();

 background_pixel = colors[color_scheme_ind][BACKGROUND];
 valuemask= CWBackPixel;

	xsubw1=(Pixmap)NULL;

	subw1 = XCreateSimpleWindow(dis0,parent,
	30,20, 50, 100,0,
	BlackPixel(dis0,screen_num),
	background_pixel
	);


 valuemask = CWBackingStore ;
 /* a lot of redisplay is done by backing_store */
 setwinattr.backing_store = Always;

 XChangeWindowAttributes(dis0,downline,valuemask,&setwinattr); 
 XChangeWindowAttributes(dis0,subw1,valuemask,&setwinattr); 
 if(DoesSaveUnders(screen_ptr)) {
 setwinattr.save_under = True;
 valuemask = CWSaveUnder;
 XChangeWindowAttributes(dis0,subw1,valuemask,&setwinattr); 
 };

 XStoreName(dis0,parent,"xe");

 gc = XCreateGC(dis0, parent, valuemask,&gcvalues);

 cursor_gc = XCreateGC(dis0,parent, valuemask,&gcvalues);
 XSetFunction(dis0,cursor_gc,GXxor);

 XSetLineAttributes(dis0, gc, 4, LineSolid, CapRound, JoinRound);

 set_font(font_names[font_index]); 

 font_main = XLoadQueryFont(dis0,font_names[font_index]);

/* if font not found load fixed font */
 if(font_main == NULL) {
 	mesg_out("font %s not found",font_names[font_index]);
	font_index=0;
	font_main = XLoadQueryFont(dis0,"fixed");
	if(!font_main) {
		ERROR("cannot find any usable font");
		exit(0);
	};
 };
 XSetFont(dis0, gc,font_main->fid);
 CLEN = XTextWidth(font_main,"m",1);
 CHEIGHT = font_main->ascent + font_main->descent;
 CFONTBASE = font_main->descent;
 CHEIGHTI=CHEIGHT;

 lenx = (parent_width-SBWIDTH)/CLEN;
 leny = (parent_height-2*dl_height)/CHEIGHT;

 // this is the new len,height after the selected font is used

 parent_width = lenx*CLEN+SBWIDTH;
 parent_height = leny*CHEIGHT+2*dl_height;

 set_wmhints();

 XResizeWindow(dis0, parent, parent_width,parent_height);

 // show the windows
 XMapWindow(dis0, parent);	/* Display the window */
 XMapWindow(dis0, downline);
 XMapWindow(dis0, wmenu);

#if	FONT_DEBUG
 MESG("font ascent = %d", font_main->ascent);
 MESG("font descent = %d", font_main->descent);
 MESG("char len = %d", XTextWidth(font_main,"m",1));
#endif

 reset_display("drv_open:2"); 	/* re evaluate total rows,cols, resize windows!!  */
 XSetWindowColormap(dis0,parent,cmap);

 XSelectInput(dis0,parent,
 	(ExposureMask|StructureNotifyMask|KeyPressMask
		|PointerMotionMask
	)
 );


 XSelectInput(dis0, downline,
 	ExposureMask
		|ButtonPressMask
    	|ButtonReleaseMask
 	);
 XSelectInput(dis0, subw1,
 	ExposureMask
 		|ButtonPressMask
    	|ButtonReleaseMask
	);

 XSelectInput(dis0, wmenu,
 	ExposureMask
 		|ButtonPressMask
    	|ButtonReleaseMask
	);


#if	XPLOT
	set_plot_timer();
#endif
}

void drv_close() { }

void drv_free_win(WINDP *wp)
{
}

int drv_search_dialog(int f)
{
	return FALSE;
}

/* set window virtual screen  */
void drv_set_wvs(WINDP *wp)
{
 if(wp->vs !=NULL) {
 	free_virtual_window(wp);
 } else {
	drv_window_xwin(wp);
	drv_win_color(wp);

 };

 wp->w_ntcols=wp->gwp->d_width/CLEN;
 wp->w_ntrows=(wp->gwp->d_height-dl_height)/CHEIGHT+1;
 wp->gwp->back_width = wp->gwp->d_width;
 wp->gwp->back_height = wp->gwp->d_height;

 allocate_virtual_window(wp);
 
 resize_draw_win(wp); 
}

// dummy one
void drv_restore_wdimensions(WINDP *wp)
{
	wp->gwp->d_height=wp->gwp->back_height;
	wp->gwp->d_width=wp->gwp->back_width;
	wp->gwp->h_flags=0;
}

int checking_break_key=0;

void drv_start_checking_break()
{
	checking_break_key=1;
}


void drv_stop_checking_break()
{
	checking_break_key=0;
}

int drv_check_break_key()
{
 int key=0;
 static int count=0;
 char key_buf[100];
 int key_index=0;

 count++;

 if(checking_break_key && count>100000) {
	X_process_events(key_buf, &key_index);
	key_index--;
	key=key_buf[key_index];
	count=0;
	if(key==3) {
		set_break();
		return 1;
	};
 };
 return 0;
}


int drv_getc(int quote) {
   static int nread=0;
   static int index=0;
   int ret0;
   static char buf[300];

   if(nread>0) { 
		index++;nread--;
		ret0=buf[index];
		buf[index]=0;
   } else {
		while(1) {
			if(X_process_events (buf, &nread) != 0 ) {
				if(nread) 
				break;
			};
			usleep(10);
		};
		if(nread>0) {
			ret0 = *buf;
			index=0;nread--;
		} else {
			ret0=0;
			index=0;
		}
	};
#if	KEY_DEBUG
	if(ret0<32) MESG("    [%d] = %d",index,ret0);
	else	MESG("    [%d] = %d = %c",index,ret0,ret0);
#endif
	lastkey=ret0;
	return(ret0);
}


int put_wstring(Drawable dw,int ypos,char *st)
{
 int len;
 len=strlen(st);
 XDrawImageString(dis0, dw, gc, px, ypos,st,len);
 return len*CLEN;
}

// Display string in box
// this is mainly used in selectl
void box_line_print(int y,int x, char *st,int width, int f, int active)
{
 char show_string[256];
 int len;
 int x1,y1;
 x1=4;
 y1=4;
 if(active) snprintf(show_string,256,"*%-*s",width,st);
 else snprintf(show_string,256," %-*s",width,st);
 if(xsubw1==(Pixmap)NULL) return;
 len=strlen(show_string);
 XSetFunction(dis0,dl_gc,GXcopy);
 if(!f) {
	XSetBackground(dis0, dl_gc, colors[color_scheme_ind][MODEBACK]);
	XSetForeground(dis0, dl_gc, colors[color_scheme_ind][MODEFORE]);
 } else {
	XSetBackground(dis0, dl_gc, colors[color_scheme_ind][MODEFORE]);
	XSetForeground(dis0, dl_gc, colors[color_scheme_ind][MODEBACK]);
 };
 XFillRectangle (dis0, xsubw1 , dl_gc, x*CLEN_DL+x1, (y)*CHEIGHT_DL+y1+1,CLEN_DL*len,CHEIGHT_DL-1); 
 XDrawImageString(dis0, xsubw1, dl_gc, x*CLEN_DL+x1, (y+1)*CHEIGHT_DL+y1-CFONTBASE_DL,show_string,len);
}

void put_string_statusline(WINDP *wp,char *st,int position)
{
 int xpos=0;
 int len;
 int rpos=25;

 if(position){
	if(position>1) rpos=8;
 	xpos=wp->gwp->d_width-rpos*CLEN_DL;
 };

 if(position>0){
  XSetForeground(dis0, status_gc, colors[color_scheme_ind][DROWCOL]);
 } else {
  XSetForeground(dis0, status_gc, colors[color_scheme_ind][MODEFORE]);
 };

  if(cwp==wp) {
	  XSetBackground(dis0, status_gc, colors[color_scheme_ind][MODEBACK]);
  } else {
	  XSetBackground(dis0, status_gc, colors[color_scheme_ind][MODEBACKI]);
  };

  if(position==0) {
	XSetWindowAttributes xwatt;
	if(wp==cwp) 
		xwatt.background_pixel = colors[color_scheme_ind][MODEBACK];
	else 
		xwatt.background_pixel = colors[color_scheme_ind][MODEBACKI];

 	XChangeWindowAttributes(dis0, wp->gwp->status, CWBackPixel, &xwatt);
	
  	 XClearWindow(dis0,wp->gwp->status);
  };

 len=strlen(st);
 XDrawImageString(dis0, wp->gwp->status, status_gc, xpos, dl_height-CFONTBASE_DL-1,st,len);

 drv_flush();
}

/* put string in message line!  */
void put_string_dl(char *st)
{
 int len;

 len=strlen(st);
 XDrawImageString(dis0, downline, dl_gc, 0, dl_height-CFONTBASE_DL-1,st,len);
 ppx_dl+=CLEN_DL*len;
 XClearArea(dis0, downline, ppx_dl,1,parent_width-ppx_dl,CHEIGHT_DL-1,0);
}

/* display on message window */
int dspv(WINDOW *mesg_window,int x,int y,char *st)
{
 int c;
 int len;
 int x1,xi;
 int px;
 char dstr[128];
 x1=x;
 xi=0;
 px = x1*CLEN_DL;
 len=0;

 hide_cursor_dl();
 while((c=*st++)) {
 	if(c<32) {
		dstr[xi++]='^';
		(c)+=64;
		len++;
	};
	dstr[xi++]=c;
	len++;
 };

 dstr[xi++]=' ';
 dstr[xi++]=' ';
 dstr[xi++]=0;

 XSetFunction(dis0,dl_gc,GXcopy);
#if	HLINES
 XFillRectangle (dis0, downline , dl_gc, px, 1,2*CLEN_DL,CHEIGHT_DL-1); 
#else
 XFillRectangle (dis0, downline , dl_gc, px, 0,2*CLEN_DL,CHEIGHT_DL-1); 
#endif

 XClearArea(dis0, downline, px,1,parent_width-ppx_dl,CHEIGHT_DL-1,0);
 XDrawImageString(dis0, downline, dl_gc, px, dl_height-CFONTBASE_DL-1,dstr,len+2);
 XFlush(dis0);

 return(len);
}

void drv_flush() {
	XFlush(dis0);
}

void drv_win_flush(WINDP *wp)
{
	drv_flush();
}

void drv_win_move(WINDP *wp,int y,int x)
{
	px=CLEN*(x);
	py=CHEIGHT*(y) - CFONTBASE;
}

// move to window position
void drv_move(int y,int x) {
	px=CLEN*(x);
	py=CHEIGHT*(y) - CFONTBASE;
}

void drv_color(int fcol,int bcol) { 
  static int colorf = 255, colorb=255;
  if(fcol != colorf || bcol!=colorb) {
	colorf=fcol;
	colorb = bcol;
	XSetBackground(dis0, gc, colors[color_scheme_ind][bcol]);
   	XSetForeground(dis0, gc, colors[color_scheme_ind][fcol]);
  };
}

void init_colors()
{
 int i,j;
 color_menu_fg=MODEFORE;
 color_menu_bg=MODEBACK;

 color_scheme_read();
 default_depth = DefaultDepth(dis0,screen_num);
 default_visual = DefaultVisual(dis0,screen_num);
 cmap = DefaultColormap (dis0,screen_num);

 i=5;
 while(!XMatchVisualInfo(dis0,screen_num,default_depth,i--,&visual_info));
 
 for(j=0;j<COLOR_SCHEMES;j++) {
	for(i=0;i<XCOLOR_TYPES;i++) 
	{
	  if(!XParseColor(dis0, cmap, color_name[j][i], &exact_def)) {
		exit(0);
	  };
	  if(!XAllocColor(dis0,cmap,&exact_def)) {
		exit(0);
	  };
	  colors[j][i]=exact_def.pixel;
	  ncolors++;
	};
 };
	foreground_pixel=colors[color_scheme_ind][MODEFORE];
	background_pixel=colors[color_scheme_ind][MODEBACK];
}

int get_menu_index_from_mouse()
{
 static int top_menu_size=0;
 int ind=0;
 ind = press_position_x/CLEN_DL/HLABEL_WIDTH;
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
 	ind=top_menu_size;
//	ind=-1;
 };
 return ind;
}

/* -------------------------------------------------------------------------------- */
/* Convert X events to keyboard events */
int X_process_events ( char *buf, int *n_chars)
{
   XEvent report;
   int ch1;
   static int pch1=0;	// previous character
   char *bufp;
   KeySym ks = 0;
   Window root, child;
   int posx, posy, rootx, rooty;
   unsigned int keys_buttons;
   static int last_event, motion_button;
   static unsigned int motion_state;
   static int in_sbar=0;
   int width, height;
   int n1=0;
   int i;
   static int n2=0;

   while(1) {
   if(XPending(dis0)) {
   while (XPending(dis0))
   {
	XNextEvent(dis0, &report);
#if	XPLOT
	if(report.xmotion.window==wplot) {
		n1=wplot_event(&report,n1);
		break;
	};
	if(report.xmotion.window==wplotp) {
		n1=wplotp_event(&report,n1);
		break;
	};
	if(report.xmotion.window==wplot_tools) {
		n1=wplot_tools_event(&report,n1);
		break;
	};
#endif
	n1=0;
	if(report.xmotion.window==(Drawable)NULL) {
		 break;
	};

	switch  (report.type) 
	{
	   case ClientMessage:
#if	NUSE
		MESG("Client Message format = %d",report.xclient.format);
	     if ((report.xclient.format == 32) &&
		 (report.xclient.data.l[0] == dra0))
	       MESG("Quick exit");
#endif
	    break; 


	   case MotionNotify:{
			mousex = report.xmotion.x;
	   		mousey = report.xmotion.y;
			mouse_row=mousey/CHEIGHT;
			mouse_col=mousex/CLEN;
	     while (XCheckMaskEvent (dis0, ButtonMotionMask, &report));
	     if (!XQueryPointer(dis0, report.xmotion.window,
				&root, &child, &rootx, &rooty, &posx, &posy,
				&keys_buttons)) break;
		 in_sbar=is_in_sbar(report.xmotion.window);

	     /* This will ensure that modifier keys are not pressed while
	        we are in motion. */

	     if ((last_event == MotionNotify) && (motion_state != keys_buttons)) {
		 	last_event=0;	
			break;
		 };
	     motion_state = keys_buttons;

		 if(last_event==ButtonPress && !is_in_sbar(report.xmotion.window)) {
		    if(motion_button == 1) 
				{ /* set position or mark, start selection */
				set_xmark(cwp,mouse_col,mouse_row,0);
				selection_on=1;
				set_selection(false);
				update_selection();
	     	};
		 };

	     last_event = MotionNotify;
	     if(motion_button) {
			if(motion_button==1) { 
				if(is_in_sbar(report.xmotion.window)) {
					set_xposition(sbar_window,posy);
				/* the following line enforce a window reframe also */
					set_update(sbar_window,UPD_WINDOW);
				} else
				{
					cwp->selection=REGION_CHAR;
					set_xmark(cwp,mouse_col,mouse_row,1);
					update_selection();
				};
			};
	     };
	     break;
		}

	   case Expose:{
	    WINDP *draw_win;
		if(report.xexpose.window==downline) {
			if(report.xexpose.count==0) {
				clear_message_line();
			};
			break;
		};
		if(report.xexpose.window==subw1) {
			break;
		};

		if((draw_win=is_in_win(report.xexpose.window))!=NULL)
		{
			if(!entry_mode) hide_cursor("expose");
			d_x0 = report.xexpose.x ;
			d_y0 = report.xexpose.y ;
			d_xl = report.xexpose.width ;
			d_yl = report.xexpose.height;

			is_updated=1;
			draw_win->w_flag |= UPD_STATUS;
			if(draw_win==cwp) hide_cursor("expose window");
			upd_all_virtual_lines(draw_win,"expose window");
			status_line(draw_win);
			draw_window(1,draw_win,"expose");
			if(draw_win==cwp) {
				cwp->currow = window_cursor_line(cwp);
				update_cursor_position();
				cwp->w_ppline = window_cursor_line(cwp);
			};
			drv_flush();
			break;
		};

		if(report.xexpose.window==wmenu) {
			top_menu(1);
			break;
		};
		if(report.xexpose.window==parent) {
			d_x0 = report.xexpose.x ;
			d_y0 = report.xexpose.y ;
			d_xl = report.xexpose.width ;
			d_yl = report.xexpose.height ;
			break;
		};
		break;
		}	
#if	NUSE
		/* it seames that we have an exit and enter each time we press a key ??? */
	   case EnterNotify:
		in_editor=1;in_sbar=0;
		break;

	   case LeaveNotify:
		in_editor=0;
		break;
#endif

	   case ConfigureNotify:
	     width = report.xconfigure.width;
	     height = report.xconfigure.height;
		 if ((width != parent_width) || (height != (parent_height)))
	     {
		  is_cnfupd=0;
	      parent_width=width;
		  parent_height = height;

		  last_event = ConfigureNotify;
		  is_updated=0;
	     }
	     break;

      case VisibilityNotify: 
		return 0;
		break;
	     
	   case ButtonPress:
		if(entry_mode) break;
	     /* Prohibit dragging more than one button at a time. */
		motion_button = report.xbutton.button;
		press_position_y = report.xbutton.y;
		press_position_x = report.xbutton.x;
		last_event=ButtonPress;

		if(is_in_sbar(report.xbutton.window))
		{
			in_sbar=1;
			if(sbar_window!=cwp) set_current_window(sbar_window,"button_press:in sbar");
			press_line= tp_line(cwp->tp_hline)+cwp->currow-1;
			break;
		};
		in_sbar=0;
		sbar_window=is_in_win(report.xbutton.window);
		if(sbar_window!=NULL){
			if(sbar_window!=cwp) set_current_window(sbar_window,"button_press:2");
		} else {
			if(in_menu) return 0;
			else 		return 1;
		};
	    if(report.xbutton.button == 2) { 
		/* paste area at current position */ 
			if(x_insert()) {
				update_screen(FALSE);
				drv_flush();
				break;
			};
	    };
		if(report.xbutton.button == 4) { // scroll up
			get_from_mouse(report,0);
			if(sbar_window!=cwp) set_current_window(sbar_window,"scroll up");
			move_window(5);
			prev_line(5);
			update_screen(1);
			break;
		};
		if(report.xbutton.button == 5) { // scroll down
			get_from_mouse(report,0);
			if(sbar_window!=cwp) set_current_window(sbar_window,"scroll down");
			move_window(-5);
			next_line(5);
			update_screen(1);
		};
#if	1
		if(report.xbutton.button == 3) {	/* right mouse key  */
			int status;
			get_from_mouse(report,0);
			status = work_menu(1);
			if(status) update_screen(1);
		}
#endif
		// We could also check for scroll left/right (7,8)		
	     break;

	   case ButtonRelease:
		{
			mousex = report.xmotion.x;
	   		mousey = report.xmotion.y;
			mouse_row=mousey/CHEIGHT;
			mouse_col=mousex/CLEN;

		 if(entry_mode) break;
	     motion_button = 0;
		 selection_on = 0;

		if(report.xbutton.window==wmenu) {
//			i=report.xbutton.x/CLEN_DL/HLABEL_WIDTH;
			i=get_menu_index_from_mouse();
			
			ch1 = m_topn.element[i].high;
			in_menu=1;
			if(report.xbutton.button==2 || report.xbutton.button>3) return 0;
			if(ch1==pch1) break; // do nothing, we are on the same menu
			pch1=ch1;

			bufp = buf;
			if(report.xbutton.button==3) { // get out of menu
				*n_chars = 1;
				*bufp++ = 7;
				in_menu=0;
				pch1=0;
				return 1;			
			};
			if(in_menu) { // change to new menu
				*n_chars = 3;
				*bufp++ = 7;
				*bufp++ = 27;
				*bufp++ = ch1;
				pch1=0;
				return 1;
			};
			// select menu item
			in_menu=1;
			*n_chars= 2;
			*bufp++ = 27;
			*bufp++ = ch1;
			return 1;
		};
#if	0
		if(report.xbutton.window==downline) {
			if(report.xbutton.button==3) next_font();
			break;
		};
#endif
		if(report.xbutton.window==subw1) {
			i = (report.xbutton.y-4)/CHEIGHT_DL;
			if(report.xbutton.button==2 || report.xbutton.button>3) return 0;
			bufp=buf;
			if(report.xbutton.button>1) {	/* send abord  */
				ch1='G';
				*bufp++=27;
				*bufp++=27;
				*n_chars=2;
				pch1=0;
				return 1;			
			};
			if(in_selection) {
				/* send index and enter key  */
				ch1=0x80+i;
				*bufp++=ch1;
				*bufp++=13;
				*n_chars=2;
			} else {	/* we are in menu, send menu key  */
			 	ch1 = c_menu->element[i].high; 
				*n_chars= 1;
				*bufp++ = ch1;
			};
			return 1;
		};

	     if(is_in_sbar(report.xbutton.window)) {
	     	if(last_event == MotionNotify) {
	     		update_screen(1);
				return 0;
			} else {
				if(n2==0) { /* process scrolling at list once */
					ch1 = process_scrollbar(sbar_window,report.xbutton.x,report.xbutton.y,report.xbutton.button);
					last_event=ButtonRelease;
					n2=0;
					return 0;
				};
				n2=0;
			};
			last_event=ButtonRelease;
			break;
	    };
	     if(last_event == MotionNotify) {
			if(in_menu) return(0);
			ch1=get_from_mouse(report,report.xbutton.button);

		   if(report.xbutton.y >CHEIGHT && ch1>-2 ) {
		   		set_xmark(cwp,mouse_col,mouse_row,1);
				cwp->selection=REGION_CHAR;
				set_selection(false);
				copy_region(0);
				/* remove the selection after the copy */
	   			set_xmark(cwp,mouse_col,mouse_row,2); /* remove marks */
				update_selection();
		   }
	     } else {
			sbar_window=is_in_win(report.xbutton.window);
			if(sbar_window!=NULL){
				if(sbar_window!=cwp) set_current_window(sbar_window,"sbar 3");
			};
			if(in_menu) {
					bufp=buf;
					*bufp++=27;
					*bufp++=27;
					*n_chars=2;
					pch1=0;
				return(1);
			};
			ch1=get_from_mouse(report,report.xbutton.button);
			
	       if(ch1<1) { 
			/* mouse returns no char */
			return 0;
			break;
	       };


	     };
	     last_event = 0;
	     break;
		}

	/* ----------------------------------------------------------------------------*/	     
	   case KeyPress:
	     bufp = buf;
	     *n_chars = XLookupString(&report.xkey, buf, 20, &ks, NULL);
		 
	     ks = ks & 0xFFFF;

		 if(checking_break_key) {

			*n_chars=1;
			if(report.xkey.state & ControlMask)
			 	buf[0]=ks%32;
			else 
			 	buf[0]=ks;
			return 1;
		 };


#if	XF864
		 // This is for the new keyboard for XFree86 X11(v4)
		 // Works ok for version 4.02 and higher
		 // XLookupString should return 1 character but returns 0
		 // 
		 if(ks/256==7) { *n_chars=1; buf[0]=ks%256;};
#endif
	     if (ks >= 0xFF00) {
		  ks = ks & 0xFF;
		  if(ks==27) {
		    *bufp = 7;*n_chars=1;
		  } else {
		  if (report.xkey.state & ShiftMask) bufp = Shift_KeySym_Mapping [ks];
		  else if (report.xkey.state & ControlMask) bufp = Control_KeySym_Mapping [ks];
		  else bufp = KeySym_Mapping [ks];
		  
		  if (0 != (*n_chars = (int) *bufp++)) memcpy(buf, bufp, *n_chars);
		  };
	    }
	     else if (*n_chars == 1) 
	       {
		  if (report.xkey.state & Mod1Mask)
		    {
		       ch1 = *bufp;
				 *bufp++ = (unsigned char) 27;
				 *bufp = (unsigned char) ch1;
				 *n_chars = 2;
		    }
		  else if (report.xkey.state & ControlMask)
		    {
		       if (*buf == ' ') *buf = 0;
		       else if (*buf == '-') *buf = 0x1F;
		    }
		  else if (*bufp == 27) { 
		  		bufp++; 
				*bufp=27; 
				*n_chars=2;
				};
	       }
	     
	     if (*n_chars == 0) break;
	     return 1;
		case MapNotify:
			return 0;
		case ReparentNotify:
			return 0;
	   default:
		return 0;
	  }   
	} 
	} else {
		mousex = report.xmotion.x;
   		mousey = report.xmotion.y;
		mouse_row=mousey/CHEIGHT;
		mouse_col=mousex/CLEN;
		if(checking_break_key) return (0);
		/* idle state , no pending events */
		/* if there is something to be done, do it else sleep for a while */
		ch1 = get_from_mouse(report,0);
		if(report.xbutton.button<6)
		if(selection_on==1) {
			if(ch1==-4) { /* on menu */
			/* scroll window one line down to select new end of selection */
			move_window(1);
//			set_xmark(cwp,report.xbutton.x,report.xbutton.y+CHEIGHT+1,1);
			set_xmark(cwp,mouse_col,mouse_row+1,1);
			update_selection();
			usleep(8000);
			continue;
			};
			if(ch1==-2) { /* on status line */
			/* scroll window one line down to select new end of selection */
			move_window(-1);
//			set_xmark(cwp,report.xbutton.x,report.xbutton.y-CHEIGHT,1);
			set_xmark(cwp,mouse_col,mouse_row-1,1);
			update_selection();
			usleep(8000);
			continue;
			};
		};
		if(in_sbar) {
			/* timer values may change? something to test */
			if((report.xbutton.y< SBWIDTH || report.xbutton.y> sbar_window->gwp->d_height-SBWIDTH) && n1>1) { n1=10;n2=1;};

			if((last_event==ButtonPress) && ((n1>10 || n1>3) && (n2==1))  ) 
			{
				ch1 = process_scrollbar(sbar_window,report.xbutton.x,report.xbutton.y,report.xbutton.button);
				n2=1; /* dont process release button and speedup scrolling */

				if((report.xbutton.y< SBWIDTH || report.xbutton.y> sbar_window->gwp->d_height-SBWIDTH)) usleep(50);
				return 0;
			};
		};
		if(last_event==ConfigureNotify && n1>1) {
		// wait a while before updating so we will not be forced to update multiple times
		  reset_display ("Last is ConfigureNotify");
   		  update_screen_size();

		  update_all = TRUE;
		  top_menu(1);
		  update_screen(TRUE);
		  last_event=0;
		  is_cnfupd=1;
		  clear_message_line();

		  n1=0;
		  last_event=0;
		  continue;
		};
		usleep(5000);n1++;
	};
	usleep(100);
   }; /* full loop */
  return 0;
}

/* after a resize of parent window  */
void reset_display(char *message) {
 static int pw=0;
 static int ph=0;
 if(parent_width==pw && parent_height==ph) return;
 pw=parent_width;
 ph=parent_height;

// drv_numcol=((parent_width-SBWIDTH) / CLEN)+1;
 drv_numcol=((parent_width-SBWIDTH) / CLEN_DL)+1;
 drv_numrow=(parent_height-2*dl_height) / CHEIGHT  ;

	/* resize menu and message windows  */
 XMoveResizeWindow(dis0, wmenu,0,0, parent_width,dl_height);
 XMoveResizeWindow(dis0, downline, 0, parent_height-dl_height, parent_width, dl_height);
}

void update_rows(WINDP *wp,int old_start_row,int new_start_row,float ratio_h,int prev_h)
{
 int old_ypos;
 GWINDP *gwp=wp->gwp;
 	if((gwp->h_flags & 1) ==0) return; 	/* already updated  */
	if(gwp->d_ypos != old_start_row) return;

	old_ypos=gwp->d_ypos + gwp->d_height;
	if(gwp->d_ypos+gwp->d_height == prev_h) {
		gwp->d_ypos = new_start_row;
		gwp->d_height = parent_height - gwp->d_ypos-dl_height;
		gwp->h_flags &= ~1;
	} else {
		WINDP *wp1;
		_el *cwin=window_list->current;
		gwp->d_ypos = new_start_row;
		gwp->d_height = round(gwp->d_height * ratio_h);
		gwp->h_flags &= ~1;

		lbegin(window_list);
		while((wp1=(WINDP *)lget(window_list))!=NULL){
			update_rows(wp1,old_ypos,gwp->d_ypos + gwp->d_height,ratio_h,prev_h);
		};
		window_list->current=cwin;
		// pop wp position in window list
	};
}

void update_cols(WINDP *wp,int old_start_col,int new_start_col,float ratio_w,int prev_w)
{
 int old_xpos;
 GWINDP *gwp=wp->gwp;
 	if((gwp->h_flags & 2) ==0) {
		return; 	/* already updated  */
	};
	if(gwp->d_xpos==0) { 
		if(old_start_col!=0) return;
	} else if(gwp->d_xpos != old_start_col) return;

	old_xpos=gwp->d_xpos + gwp->d_width+SBWIDTH;
	if(gwp->d_xpos+gwp->d_width+SBWIDTH == prev_w) 
	{
		gwp->d_xpos = new_start_col;
		gwp->d_width = parent_width - gwp->d_xpos-SBWIDTH;
		wp->gwp->h_flags &= ~2;
	} else {
		WINDP *wp1;
		_el *cwin=window_list->current;
		gwp->d_xpos = new_start_col;
		gwp->d_width = round(gwp->d_width * ratio_w);
		gwp->h_flags &= ~2;
		lbegin(window_list);
		while((wp1=(WINDP *)lget(window_list))!=NULL){
			update_cols(wp1,old_xpos,gwp->d_xpos + gwp->d_width+SBWIDTH,ratio_w,prev_w);
		};
		window_list->current=cwin;
		// pop wp position in window list
	};
}

int resize_screen(int init)
{
	WINDP *wp;	/* current window being examined */
	static int prev_h=0;
	static int prev_w=0;
	double ratio_h=1,ratio_w=1;

//	MESG("resize_screen: init=%d",init);
	if(init==2) {
		prev_h=parent_height;
		prev_w=parent_width;
		return(false);
	};

	if(prev_h==parent_height && prev_w==parent_width) return false;
	/* We need a windows resize ! */
	ratio_h=(double)(parent_height)/(double)prev_h; 
	ratio_w=(double)(parent_width)/(double)prev_w; 

	/* set every window to be resized  */
	lbegin(window_list);
	while((wp=(WINDP *)lget(window_list))!=NULL){
		wp->gwp->h_flags=3;	/* update both vertical and horizontal dimensions  */
	};
	lbegin(window_list);
	while((wp=(WINDP *)lget(window_list))!=NULL){
		update_rows(wp,dl_height,dl_height,ratio_h,prev_h-dl_height);
	};
	lbegin(window_list);
	while((wp=(WINDP *)lget(window_list))!=NULL){
		update_cols(wp,0,0,ratio_w,prev_w);
	};

 prev_h=parent_height;
 prev_w=parent_width;
 return true;
}


void drv_window_delete(WINDP *wp)
{
    if(cursor_draw==wp->gwp->draw) cursor_draw=0;
	XDestroyWindow(dis0,wp->gwp->draw);
	XDestroyWindow(dis0,wp->gwp->sbar);
	XDestroyWindow(dis0,wp->gwp->status);
}
/*
 * Delete the current window, rearange screen space
 * Bound to ^X0.
 */
int delete_window(int n)
{
	WINDP *wp=NULL;	/* loop window */ 
	WINDP *w1=NULL;	/* first window to go and receive deleted space */
	WINDP *to_delete=cwp;
	int is_first=1;

	float p_up=0.0;
	float p_down=0.0;
	float p_left=0.0;
	float p_right=0.0;
	float p;
	int cx1,cy1,cx2,cy2;
	int wx1,wy1,wx2,wy2;


	cx1=cwp->gwp->d_xpos;cx2=cx1+cwp->gwp->d_width+SBWIDTH;
	cy1=cwp->gwp->d_ypos;cy2=cy1+cwp->gwp->d_height;
	/* if there is only one window, don't delete it */
	if(window_list->head->next == NULL) return(FALSE);

	lbegin(window_list);
	/* resize windows after delete  */
	while((wp=lget(window_list))!=NULL) {
		if(wp==cwp) continue;
		wx1=wp->gwp->d_xpos;wx2=wx1+wp->gwp->d_width+SBWIDTH;
		wy1=wp->gwp->d_ypos;wy2=wy1+wp->gwp->d_height;
		if(wx1 == cx2) {	/* right  */
			if(cy1 == wy1 && cwp->gwp->d_height == wp->gwp->d_height) {
				p_right=1.0;
				wp->gwp->d_xpos = cx1;	/* change xpos  */
				wp->gwp->d_width += cwp->gwp->d_width + SBWIDTH;	/* add width of deleted to the right window  */
				if(is_first) { w1=wp; is_first=0;};
				drv_set_wvs(wp);
			} else
			if(wy1>=cy1 && wy2<= cy2) {
				p = (float)(wy2-wy1)/(cy2-cy1);
				p_right += p;
				wp->gwp->d_xpos = cx1;	/* change xpos  */
				wp->gwp->d_width += cwp->gwp->d_width + SBWIDTH;	/* add width of deleted to the right window  */
				if(is_first) { w1=wp; is_first=0;};
				drv_set_wvs(wp);
			};
			if(p_right >= 1.0) break;
		}; 
		if(cx1 == wx2) {	/* left  */
			if(cy1 == wy1 && cwp->gwp->d_height == wp->gwp->d_height) {
				p_left=1.0;
				wp->gwp->d_width += cwp->gwp->d_width + SBWIDTH; 	/* add widht of deleted to the left window  */
				if(is_first) { w1=wp; is_first=0;};
				drv_set_wvs(wp);
			}else
			if(wy1>=cy1 && wy2<= cy2) {
				p = (float)(wy2-wy1)/(cy2-cy1);
				p_left += p;
				wp->gwp->d_width += cwp->gwp->d_width + SBWIDTH; 	/* add widht of deleted to the left window  */
				if(is_first) { w1=wp; is_first=0;};
				drv_set_wvs(wp);
			};
			if(p_left >= 1.0) break;
		}; 
		if(wy1 == cy2) { 	/* bottom  */
			if(cx1 == wx1 && cwp->gwp->d_width == wp->gwp->d_width) {
				p_down = 1.0;
				wp->gwp->d_ypos = cy1;
				wp->gwp->d_height += cwp->gwp->d_height;
				if(is_first) { w1=wp; is_first=0;};
				drv_set_wvs(wp);
			}else
			if(wx1>=cx1 && wx2<= cx2) {
				p = (float)(wx2+SBWIDTH-wx1)/(cx2-cx1);
				p_down += p;
				wp->gwp->d_ypos = cy1;
				wp->gwp->d_height += cwp->gwp->d_height;
				if(is_first) { w1=wp; is_first=0;};
				drv_set_wvs(wp);
			};
			if(p_down >= 1.0) break;
		};
		if(cy1 == wy2) { 	/* top  */
			if(cx1 == wx1 && cwp->gwp->d_width == wp->gwp->d_width) {
				p_up = 1.0;
				wp->gwp->d_height += cwp->gwp->d_height;
				if(is_first) { w1=wp; is_first=0;};
				drv_set_wvs(wp);
			} else
			if(wx1>=cx1 && wx2<= cx2) {
				p = (float)(wx2-wx1)/(cx2-cx1);
				p_up += p;
				wp->gwp->d_height += cwp->gwp->d_height;
				if(is_first) { w1=wp; is_first=0;};
				drv_set_wvs(wp);
			};
			if(p_up >= 1.0) break;
		};
	};

	if(w1==NULL) {
		drv_msg_line("cannot delete this window");
		return(false);
	};

	/* remove current window */
	unlink_filebuf(to_delete);
	lfind_data((void *)to_delete,window_list);
	remove_current_from_list(window_list);

	set_current_window(w1,"delete_window");
	free_window_data(to_delete);

	set_update(cwp,UPD_WINDOW|UPD_ALL);
	update_status();
	return(TRUE);
}

// dummy function
void window_clear(WINDP *wp)
{
}


void show_cursor (char *from)
{
	if(cursor_draw) hide_cursor ("show_cursor");
   	XSetForeground(dis0, cursor_gc, colors[color_scheme_ind][MODEFORE]);
	cursor_draw=cwp->gwp->draw;
	XFillRectangle( dis0, cursor_draw,
				cursor_gc,
		       px,
		       py + CFONTBASE + 1,
		       CLEN ,
		       CHEIGHT - 1);
   cursor_px=px;cursor_py=py;
   XFlush(dis0);
}

void hide_cursor (char *from)
{

   if (cursor_draw == 0) return;

	XFillRectangle( dis0, cursor_draw,
				cursor_gc,
		       cursor_px, cursor_py + CFONTBASE + 1,
		       CLEN, CHEIGHT - 1);
  cursor_draw=0;
  cursor_px=0;
  cursor_py=0;
}

void show_cursor_dl (int px)
{
   if (cursor_showing) hide_cursor_dl ();
   cursor_showing = 1;
   ppx_dl=px*CLEN_DL;

	XFillRectangle( dis0, downline, cursor_gc,
		       ppx_dl,
#if	HLINES
		       2,
#else
			   1,
#endif
		       CLEN_DL , CHEIGHT_DL - 1);
   XFlush(dis0);
}

void set_cursor_dl(int px)
{
	ppx_dl = px*CLEN_DL;
}

void hide_cursor_dl (void)
{
   if (cursor_showing == 0) return;
   cursor_showing = 0;
	XFillRectangle( dis0, downline, cursor_gc,
		       ppx_dl,
#if	HLINES
		       2,
#else
			   1,
#endif
		       CLEN_DL, CHEIGHT_DL-1);
}


void disp_rectangle(int x,int y,int x1,int y1)
{
 int x0,y0,xl,yl;
        x0 = (x<x1) ? x:x1;
        y0 = (y<y1) ? y:y1;
        xl = (x<x1) ? x1-x:x-x1;
        yl = (y<y1) ? y1-y:y-y1 ;
//	MESG("disp_rectangle: (%d:%d) - (%d:%d)",x,y,x1,y1);
	xl--;
	x0 *= CLEN_DL; 
	x0+=2;
	xl *= CLEN_DL;
	yl--;

	y0 *= CHEIGHT_DL; yl *= CHEIGHT_DL;
	y0++;
#if	HLINES
	y0+=2;
#endif
	yl+=8; x0-=2 ;xl+=8;

	XMoveResizeWindow(dis0, subw1, x0,y0,xl+5,yl);
	XMapWindow(dis0,subw1);
	xsubw1 = XCreatePixmap(dis0, subw1, xl,yl,default_depth);
	XSetFunction(dis0,dl_gc,GXcopy);
	
	XSetLineAttributes(dis0, gc, 4, LineSolid, CapRound, JoinRound); 

	drv_color(MODEFORE,0);
	XDrawLine(dis0, xsubw1, gc, 2, 2, xl-2, 2);
	XDrawLine(dis0, xsubw1, gc, 2, 2, 2, yl-2);
	drv_color(MODEFORE,0);
	XDrawLine(dis0, xsubw1, gc, 2, yl-2, xl-2, yl-2);
	XDrawLine(dis0, xsubw1, gc, xl-2, 2, xl-2, yl-2);
	// create area for redisplay window in case of exposure events
	w1_xl=xl;
	w1_yl=yl;
	XFlush(dis0);
	show_subw1();
}

void show_subw1()
{	
	if(xsubw1==(Pixmap)NULL) return;
	XRaiseWindow(dis0,subw1);
	XFlush(dis0);
}


int process_scrollbar(WINDP *wp,int x,int y,int button) 
{ 
 /* in slider */
 int start, len;
 set_sposition(sbar_window,&start,&len);
 if(cwp!=sbar_window) set_current_window(sbar_window,"process_scrollbar");
 if(y<SBWIDTH+1) { /* start of file */
	if(button==3) {
		goto_bof(0);
	} else
	{
		move_window(1);
	};
	update_screen(1);
	return 0;
 };
 if(y> wp->gwp->d_height-SBWIDTH-1) { /* end of file */
 	if(button==3) {
		goto_eof(0);
	} else
	{
		move_window(-1);
	};
	update_screen(1);
	return 0;
 };
 if(y<start) { /* page up */
	if(button==1) {
		prev_page(1);
	} else
	{
		/* go backward */
		find_previous(1);
	};
	update_screen(1);
	return 0;
 };
 if(y>2+len+start-1) { /* page down */
	if(button==1) {
		next_page(1);
	} else 
	{
		/* go forward */
		find_next(1);
	};
 };
 	update_screen(1);
	return(0);
}

void set_1window()
{
 cwp->gwp->d_xpos=0;
 cwp->gwp->d_ypos=dl_height;
 cwp->gwp->d_width=parent_width-SBWIDTH;
 cwp->gwp->d_height=parent_height-2*dl_height;

 drv_set_wvs(cwp);
 show_slide(cwp);
}

/*
 * Split the current window horizontally
 * Bound to "C-X 2". 
 */
int split_window(int n)
{
        register WINDP *wp;

        if (cwp->w_ntrows < 3) {
                msg_line("Cannot split a %d line window", cwp->w_ntrows);
                return (FALSE);
		}

		wp = dublicate_window(cwp);

		wp->gwp->d_width=cwp->gwp->d_width;
		wp->gwp->d_height=cwp->gwp->d_height/2;
		wp->gwp->d_xpos=cwp->gwp->d_xpos;

		cwp->gwp->d_height -= wp->gwp->d_height;
		wp->gwp->d_ypos=cwp->gwp->d_ypos+cwp->gwp->d_height;

		drv_free_win(cwp);
		drv_set_wvs(cwp);

		drv_set_wvs(wp);
		return (next_window(1));
}

/*
 * Split the current window vertically
 * Bound to "C-X 3". 
 */
int vsplit_window(int n)
{
    WINDP *wp;
	
    if (cwp->w_ntcols < 20) {
            msg_line("Cannot split a %d column window", cwp->w_ntcols);
            return (FALSE);
    }

		wp = dublicate_window(cwp);

		wp->gwp->d_width=(cwp->gwp->d_width-SBWIDTH)/2-SBWIDTH;
		wp->gwp->d_height=cwp->gwp->d_height;
		wp->gwp->d_ypos = cwp->gwp->d_ypos;

		cwp->gwp->d_width -= wp->gwp->d_width+SBWIDTH;
		wp->gwp->d_xpos = cwp->gwp->d_xpos+SBWIDTH+cwp->gwp->d_width;

		drv_free_win(cwp);

		drv_set_wvs(cwp);
		drv_set_wvs(wp);
		return (next_window(1));
}

/* get various elements from mouse position */
 /* window , buffer, line, character offset */ 
int get_from_mouse(XEvent report,int button) 
{ 
 offs new_offset;
 int x=report.xbutton.x;
 int y=report.xbutton.y;
 WINDP *wp;
 num new_line;
 int new_column;
 int i;

	mousex = report.xmotion.x;
	mousey = report.xmotion.y;
	mouse_row=mousey/CHEIGHT;
	mouse_col=mousex/CLEN;

	new_column = mouse_col; 

	wp = is_in_win(report.xbutton.window);
	if(button==0) {
		int s=0;
		if(wp) { 
			if(cwp==wp) {
				if(y<0) s = -4;
				else 
				if(cwp->gwp->d_ypos+cwp->gwp->d_height-2*dl_height < y) s = -2;
				else s = 0;
			};
		}
		return s;
	};

	if(wp==NULL) {
		wp=is_in_status(report.xbutton.window);

		if(wp) { 
			if(wp!=cwp) {
				set_current_window(wp,"get_from_mouse");
				update_status();
				update_screen(FALSE);
				return -1;
			} else {
			};
		} else	return -1;
	};

	if(cwp->gwp->status==report.xbutton.window) 
	{
		new_column = x/CLEN_DL;
		if(new_column<30) next_file(1);
		else
		if(new_column<70) {
			if(button==2) split_window(1);
			if(button==3) delete_window(1);
		}
		update_screen(FALSE);
		return(-2);
	};
	if(wp!=cwp) {
		set_current_window(wp,"get_from_mouse");
	};

	/* We are in an editors window */
	new_offset=LineBegin(tp_offset(cwp->tp_hline));
	for(i=0;i<mouse_row;i++) {
  		new_offset = FNextLine(cbfp,new_offset);
	};
	textpoint_set(cwp->tp_current,new_offset);

	new_line=tp_line(cwp->tp_current);
	if(button==1) {	/* move to position */
		move_to_new_position(new_column,new_line);
		if(cbfp->b_flag & FSDIRED) {
			dir_right(0);
		};
#if	TNOTES
		if(cwp->w_fp->b_flag & FSNOTES) {
			new_column=set_tag_view_position(new_line,new_column);
		};
#endif
		update_screen(FALSE);
	};

	if(button==3) {
		if(cbfp->b_flag & FSDIRED) {
			dir_left(0);
			update_screen(FALSE);
		} else {
			int status;
			status = work_menu(1);
			return status;
		};
	};
	return(-1);
}

int set_fontindex(int n)
{
 font_index=n-1;
 if(font_index>9 || font_index<0) font_index=0;
 set_font(font_names[font_index]);
 return(TRUE);
}

int set_font(char *font_name)
{
 XFontStruct *fn1;
 WINDP *wp;

 if(!discmd) return true;
 fn1 = XLoadQueryFont(dis0,font_name);
 if(fn1) {
   XSetFont(dis0, gc, fn1->fid);
   CLEN = XTextWidth(fn1,"m",1);
   CHEIGHT = fn1->ascent + fn1->descent;
   CFONTBASE = fn1->descent;
   set_wmhints();

	lbegin(window_list);
	while((wp=lget(window_list))!=NULL) {
		drv_set_wvs(wp);
	};

   update_all = TRUE;
   update_screen(TRUE);
   return(TRUE);
 };
 msg_line("font %s not found",font_name);
 return(FALSE);
}

int select_font(int n)
{
 char font_name[MAXFLEN];
 int status;
 font_name[0]=0;
 if(nextarg("New font: ",font_name, MAXFLEN,true) != TRUE) return(FALSE);
 status = set_font(font_name);

 clear_message_line();
 if(status) {
	return(TRUE);
 } else return(FALSE);
}

void set_wmhints()
{
    parent_hints.base_height = 2*dl_height ;
    parent_hints.base_width  = CLEN ;
    parent_hints.height_inc  = CHEIGHT ;
    parent_hints.width_inc = CLEN;
	parent_hints.min_width = CLEN*15+SBWIDTH;
	parent_hints.min_height = 2*dl_height+(CHEIGHT)*6;
    XSetWMNormalHints(dis0, parent, &parent_hints);

	drv_back_color();
}

void drv_back_color()
{
 XSetWindowAttributes xwatt;
 unsigned long valuemask;
 background_pixel = colors[color_scheme_ind][BACKGROUND];
//	valuemask= CWBackingPixel | CWBackPixel;
//	xwatt.backing_pixel = background_pixel;
	valuemask= CWBackPixel;
	xwatt.background_pixel = background_pixel;
 	XChangeWindowAttributes(dis0, parent, valuemask, &xwatt);
	XChangeWindowAttributes(dis0, downline, valuemask, &xwatt);

	xwatt.background_pixel = colors[color_scheme_ind][MODEBACK];
	XChangeWindowAttributes(dis0, wmenu, valuemask, &xwatt);
}

void drv_win_color(WINDP *wp)
{
 XSetWindowAttributes xwatt;
 unsigned long valuemask;
 background_pixel = colors[color_scheme_ind][BACKGROUND];
	valuemask= CWBackingPixel | CWBackPixel;
	xwatt.backing_pixel = background_pixel;
	valuemask= CWBackPixel;
	xwatt.background_pixel = background_pixel;
 	XChangeWindowAttributes(dis0, wp->gwp->draw, valuemask, &xwatt);
	XClearWindow(dis0,wp->gwp->draw);
}

void drv_update_styles()
{
 WINDP *wp;

 XSetWindowAttributes xwatt;
 unsigned long valuemask;
 background_pixel = colors[color_scheme_ind][BACKGROUND];
//	valuemask= CWBackingPixel | CWBackPixel;
//	xwatt.backing_pixel = background_pixel;
	valuemask= CWBackPixel;
	xwatt.background_pixel = background_pixel;
 	XChangeWindowAttributes(dis0, parent, valuemask, &xwatt);

	init_downline();
	top_menu(1);
	lbegin(window_list);
	while((wp=lget(window_list))!=NULL){
		drv_win_color(wp);
		init_slide(wp);
	};
}

int system_paste(int n)
{
 if(dont_edit() || cbfp->b_flag & FSDIRED )return false;
#if	1
	if(ext_system_paste()){
		update_screen(1);
		return TRUE;
	};
	return FALSE;
#else
 int nbytes;
	nbytes=x_insert();
	if(nbytes==0) return FALSE;
	update_screen(1);
	return TRUE;
#endif
}

int x_insert ()
{
   int nbytes;
   char *dat;

   dat = XFetchBytes (dis0, &nbytes);
   if (dat != NULL) {
	insert_string(cbfp,dat,nbytes);
	XFree (dat);
   };
   return nbytes;
}


void disp_box(char *title,int atr,int y1,int x1,int y2,int x2)
{
 if(y1<0) {
 	y1=mousey/CHEIGHT_DL;
	if( (y1+y2+3)*CHEIGHT_DL > parent_height-20) {
		y1 = (parent_height-(y2+3)*CHEIGHT_DL)/CHEIGHT_DL;
	};
	y2 = y1+y2+1;
	
 };
 if(x1<0) {
 	x1=mousex/CLEN_DL;
	if( (x1+x2+2)*CLEN_DL > (parent_width-SBWIDTH)) {
		x1 = (parent_width-SBWIDTH-(x2-1)*CLEN_DL)/CLEN_DL;
	};
	x2 = x1+x2+1;
 };
	box_x=x1;
	box_y=y1;
	disp_rectangle(x1,y1,x2,y2);
}

int getstring(char *prompt, char *st1,int maxlen,int disinp)
{
 return win_getstring(mesg_window,prompt, st1,maxlen,disinp);
}

// dummy function in xlib!
void set_box_cline(int line)
{
//	if(cbox==NULL) return;
//	cbox->cline=line;
}

// dummy function in xlib!
int get_box_cline()
{
//	cbox->cline = mouse_col - cbox->y -1;
//	return cbox->cline;
	return 1;
}

// dummy function in xlib!
int get_mouse_box_line()
{
	return 0;
}

 /* Put virtual screen text on physical */
void put_wtext(WINDP *wp,int row,int maxcol)
{
 int i,imax,imin,i1;
 vchar *v1,*vtext;
 unsigned char ch;
 char ccolor,cattr;
 char st[MAXBLEN];
 int prev_fore,prev_back;
 VIDEO *vp1;
 vp1 = wp->vs[row];

 v1=vtext=vp1->v_text;
 ccolor=prev_fore=255;
 cattr=prev_back=255;
 
// find the first different character (imin)
 imin=0;
 drv_color(5,4);
 if(imin<=maxcol) {
 	/* we have a difference */
	v1=vtext;imax=imin;
	if(row==0) imax=maxcol;
	else
	for(i=maxcol;i>=imin;i--) {
		if(v1[i].uval[0] != empty_char /*  && v1[i]%256 !=155 */) { imax=i;break;};
	};
	/* count of trailing spaces is maxcol-imax */
	v1=vtext+maxcol;
	v1=vtext+imin;

	drv_move(row,imin);

	for(i=imin,i1=0;i<=imax;i++) {
		ch=v1->uval[0];
		ccolor=v1->fcolor;
		cattr=v1->bcolor;
		v1++;
		if(ccolor!=prev_fore || cattr!=prev_back) {
			if(i1>0) { 
				st[i1]=0;
				px+=put_wstring(wp->gwp->draw,py+CHEIGHT,st);
			};
			drv_color(ccolor,cattr); 
			i1=0;
			prev_fore=ccolor;prev_back=cattr;
			};
		st[i1++]=ch;
	};
	if(i1>0) {
		st[i1]=0; 
		px+=put_wstring(wp->gwp->draw,py+CHEIGHT,st);
	};
 }; 
}

int export_region(ClipBoard *clip)
{
#if	1
	return ext_system_copy();
#else
 long size;
 int nbytes;

 if(clip->height>1) size=(clip->width+1)*clip->height;
 else size=clip->width;
	
	XStoreBuffer(dis0, clip->text,size,0);
	/* we need to get it back, otherwise it gets much complicated to paste from onother instance */
	XFetchBytes(dis0, &nbytes);
 return(1);
#endif
}

void init_downline()
{
 XFontStruct *font_status;
 font_status = XLoadQueryFont(dis0,font_names[DL_FONT]);
 if(font_status==NULL) {
 	mesg_out("cannot load font [%s]",font_names[DL_FONT]);
	font_status = XLoadQueryFont(dis0,"-*-fixed-medium-r-normal-*-20-*-*-*-*-*-*-7");
  };
  dl_gc = XCreateGC(dis0, parent, valuemask,&gcvalues);
#if	HLINES
  dltop_gc = XCreateGC(dis0, parent, valuemask,&gcvalues);
  dlbotom_gc = XCreateGC(dis0, parent, valuemask,&gcvalues);
  XSetLineAttributes(dis0,dl_gc,1,LineSolid,CapRound, JoinRound);
  XSetForeground(dis0, dltop_gc, colors[color_scheme_ind][MODEFOREI]);
  XSetForeground(dis0, dlbotom_gc, colors[color_scheme_ind][MODEBACKI]);

//  xwatt.background_pixel = colors[color_scheme_ind][BAR_FOREGROUND];
//  XChangeWindowAttributes(dis0, downline, CWBackPixel, &xwatt);
#endif
  XSetForeground(dis0, dl_gc, colors[color_scheme_ind][BACKGROUND]);
  XSetBackground(dis0, dl_gc, colors[color_scheme_ind][FOREGROUND]);
  XSetFont(dis0, dl_gc,font_status->fid);

  status_gc = XCreateGC(dis0, parent, valuemask,&gcvalues);
  XSetLineAttributes(dis0,status_gc,1,LineSolid,CapRound, JoinRound);
  XSetForeground(dis0, status_gc, colors[color_scheme_ind][MODEFORE]);
  XSetBackground(dis0, status_gc, colors[color_scheme_ind][MODEBACK]);
  XSetFont(dis0, status_gc,font_status->fid);

  CLEN_DL = XTextWidth(font_status,"m",1);
  CHEIGHT_DL = font_status->ascent + font_status->descent;
  CFONTBASE_DL = font_status->descent;
#if	HLINES
  dl_height = CHEIGHT_DL+2;
#else
  dl_height = CHEIGHT_DL;
#endif
}

void show_downline()
{
 XSetWindowAttributes xwatt;

  xwatt.background_pixel = colors[color_scheme_ind][MODEBACK];
  XChangeWindowAttributes(dis0, downline, CWBackPixel, &xwatt);
  
  XClearWindow(dis0,downline);
  XSetBackground(dis0, dl_gc, colors[color_scheme_ind][MODEBACK]);
  XSetForeground(dis0, dl_gc, colors[color_scheme_ind][MODEFOREI]);

#if	HLINES
  XDrawLine(dis0, downline, dlbotom_gc, 0, 0, parent_width, 0);
  XDrawLine(dis0, downline, dltop_gc, 0, dl_height-1, parent_width, dl_height-1);
#endif
}

void init_slide(WINDP *wp)
{
 GWINDP *gwp=wp->gwp;
 int color_ind = color_scheme_ind;
 valuemask=0;

  wp->gwp->sgc = XCreateGC(dis0, gwp->sbar, valuemask,&gcvalues);
  wp->gwp->sgm = XCreateGC(dis0, gwp->sbar, valuemask,&gcvalues);
  wp->gwp->top_gc = XCreateGC(dis0, gwp->sbar, valuemask,&gcvalues);
  wp->gwp->botom_gc = XCreateGC(dis0, gwp->sbar, valuemask,&gcvalues);
  XSetLineAttributes(dis0,wp->gwp->sgc,1,LineSolid,CapRound, JoinRound);

  XSetForeground(dis0, wp->gwp->sgc, colors[color_ind][INFOBACK]);
  XSetForeground(dis0, wp->gwp->sgm, colors[color_ind][INFOFORE]);
  XSetForeground(dis0, wp->gwp->top_gc, colors[color_ind][CBOXTFORE]);
  XSetForeground(dis0, wp->gwp->botom_gc, colors[color_ind][CBOXTBACK]);
}


	/* return window if inside of a window  */
WINDP *is_in_win(int xw)
{
 static WINDP *wp;
 lbegin(window_list);
 while((wp=(WINDP *)lget(window_list))!=NULL){
 	if(xw==wp->gwp->draw) {
		return wp;
	};
 };
 return NULL;
}

	/* return window is inside of a window status line  */
WINDP *is_in_status(int xw)
{
 static WINDP *wp;
 lbegin(window_list);
 while((wp=(WINDP *)lget(window_list))!=NULL){
 	if(xw==wp->gwp->status) {
		return wp;
	};
 };
 return NULL;
}

int is_in_sbar(int xw)
{
 WINDP *wp;
 lbegin(window_list);
 while((wp=(WINDP *)lget(window_list))!=NULL){
 	if(xw==wp->gwp->sbar) {
		sbar_window=wp;
		return 1;
	};
 };
 sbar_window=NULL;
 return 0;
}

void show_slide(WINDP *wp)
{
  XClearWindow(dis0,wp->gwp->sbar);
  XDrawLine(dis0, wp->gwp->sbar, wp->gwp->botom_gc, 0, 0, 0, wp->gwp->d_height);
  XDrawLine(dis0, wp->gwp->sbar, wp->gwp->top_gc, SBWIDTH-1, 0, SBWIDTH-1, wp->gwp->d_height);

  Draw_up_button(wp->gwp,1,0,1);
  Draw_dn_button(wp->gwp,1,wp->gwp->d_height-SBWIDTH,1 );

  draw_middle(wp);
  XFlush(dis0);  
}

/* draw triangular up button with a shadow of 2 pixels */
void Draw_up_button (GWINDP *gwp,int x, int y, int state)
{
 Drawable dw=gwp->sbar;
   const unsigned int sz2 = (SBWIDTH / 2);
   XPoint pt [3];
   GC top=gwp->sgm, bot=gwp->sgm;

   switch (state) {
    case +1: top = gwp->top_gc; bot = gwp->botom_gc; break;
    case -1: top = gwp->botom_gc; bot = gwp->top_gc; break;
    case 0: top = bot = gwp->sgm; break;
   }

   /* fill triangle */
   pt[0].x = x;			pt[0].y = y + SBWIDTH;
   pt[1].x = x + SBWIDTH - 3;	pt[1].y = y + SBWIDTH;
   pt[2].x = x + SBWIDTH/2-1;		pt[2].y = y;
   XFillPolygon (dis0, dw, gwp->sgm, pt, 3, Convex, CoordModeOrigin);

   /* draw base */
   XDrawLine (dis0, dw, bot, pt[0].x, pt[0].y, pt[1].x, pt[1].y);

   /* draw shadow */
   pt[1].x = x + sz2-1 ;	pt[1].y = y;
   XDrawLine (dis0, dw, top, pt[0].x, pt[0].y, pt[1].x, pt[1].y);
# if (SHOW_SHADOW > 1)
   /* doubled */
   pt[0].x++; 
   pt[1].y++;
   XDrawLine (dis0, dw, top, pt[0].x, pt[0].y, pt[1].x, pt[1].y);
# endif

   /* draw shadow */
   pt[0].x = x + sz2-1;		pt[0].y = y;
   pt[1].x = x + SBWIDTH - 3;	pt[1].y = y + SBWIDTH-1 ;
   XDrawLine (dis0, dw, bot, pt[0].x, pt[0].y, pt[1].x, pt[1].y);
# if (SHOW_SHADOW > 1)
   /* doubled */
   pt[0].y++; pt[1].x--; 
   XDrawLine (dis0, dw, bot,  pt[0].x, pt[0].y, pt[1].x, pt[1].y);
# endif
}

/* draw triangular down button with a shadow of 2 pixels */
void Draw_dn_button (GWINDP *gwp,int x, int y, int state)
{
 Drawable dw=gwp->sbar;
   XPoint pt [3];
   GC top=gwp->sgm, bot=gwp->sgm;

   switch (state) {
    case +1: top = gwp->top_gc; bot = gwp->botom_gc; break;
    case -1: top = gwp->botom_gc; bot = gwp->top_gc; break;
    case 0: top = bot = gwp->sgm; break;
   }

   /* fill triangle */
   pt[0].x = x;			pt[0].y = y;
   pt[1].x = x + SBWIDTH-2;	pt[1].y = y;
   pt[2].x = x + SBWIDTH/2-1;		pt[2].y = y + SBWIDTH;
   XFillPolygon (dis0, dw, gwp->sgm, pt, 3, Convex, CoordModeOrigin);

   /* draw base */
   XDrawLine (dis0, dw, top, pt[0].x, pt[0].y, pt[1].x, pt[1].y);

   /* draw shadow */
   pt[1].x = x + SBWIDTH/2-1;		pt[1].y = y + SBWIDTH;
   XDrawLine (dis0, dw, top, pt[0].x, pt[0].y, pt[1].x, pt[1].y);
# if (SHOW_SHADOW > 1)
   /* doubled */
   pt[0].x++; pt[0].y++; 
   pt[1].y-=2;
   XDrawLine (dis0, dw, top, pt[0].x, pt[0].y, pt[1].x, pt[1].y);
# endif

   /* draw shadow */
   pt[0].x = x + SBWIDTH/2-1;			pt[0].y = y + SBWIDTH;
   pt[1].x = x + SBWIDTH-3;		pt[1].y = y+1;
   XDrawLine (dis0, dw, bot, pt[0].x, pt[0].y, pt[1].x, pt[1].y);
# if (SHOW_SHADOW > 1)
   /* doubled */
   pt[0].y-=2; 
   pt[1].x--; 
   pt[1].y+=1;
   XDrawLine (dis0, dw, bot, pt[0].x, pt[0].y, pt[1].x, pt[1].y);
# endif
}

void draw_middle(WINDP *wp)
{
 int start, len;
 GWINDP *gwp=wp->gwp;
 set_sposition(wp,&start,&len);
 if(start+len+SBWIDTH>gwp->d_height) len=gwp->d_height-start-2*SBWIDTH-1;
 if(len<3) return;
 if(start>0)
 {
 XDrawLine 	(dis0, gwp->sbar, gwp->top_gc, 1, start, SBWIDTH-2, start);
 XDrawLine 	(dis0, gwp->sbar, gwp->top_gc, 1, start, 1, start+len); 
 XDrawLine 	(dis0, gwp->sbar, gwp->botom_gc, SBWIDTH-2, start+1, SBWIDTH-2, start+len);
 XDrawLine 	(dis0, gwp->sbar, gwp->botom_gc, 1, start+len, SBWIDTH-2, start+len);
 XFillRectangle	(dis0, gwp->sbar, gwp->sgm, 2,start+1,SBWIDTH-4,len-1);
 };
}

void set_xposition(WINDP *wp,int y)
{
 int gline=0;	/* goto line */
 int start, len;
 int cline;
 int h;
 FILEBUF *fp = wp->w_fp;
 FILEBUF *back_fp=cbfp;
 cline = set_sposition(wp,&start,&len);

 if(y<SBWIDTH+2 || y > wp->gwp->d_height-SBWIDTH) return;
 h = wp->gwp->d_height-2*SBWIDTH;

 {
 gline = press_line+fp->lines*(double)(y-(press_position_y)) / (double)(h-len-1);

 if(gline<1) gline=1;
 if(gline> fp->lines) gline=fp->lines;

 if(gline!=cline) {
 	cbfp=fp;
 	igotolinecol(gline,1,1);
	cbfp=back_fp;
	update_screen(1);
 };
 };
}

/* set scroll bar position variables */
int set_sposition(WINDP *wp,int *st, int *l)
{
 int h;
 int cline;
 FILEBUF *fp=wp->w_fp;;

 h = wp->gwp->d_height-2*SBWIDTH;

 if(wp != NULL) {
 cline=tp_line(wp->tp_current);
 if(fp->lines< wp->w_ntrows ) {
	*l=(h-3);
	*st=SBWIDTH+1;
 } else {
	 *l = (h-2) * (double)wp->w_ntrows/ (double)fp->lines  -1;
	 if(*l<4) *l=4;
	 *st = (h-*l)* (double)(cline) / (double)(fp->lines)+SBWIDTH;
	 if(*st<SBWIDTH+2) *st=SBWIDTH+1;
 };
 } else { cline=0;*st=0;*l=0; };
 return(cline);
}

int select_font_mono(int n)
{
 int s;
 s=selectl("Select Font",font_list,10,20,30,1,30,-1);
 if(s<0) return 0;
 if(s==9) return select_font(1);
 return(set_fontindex(s+1));
}

/* init environment */
int init_drv_env()
{
 half_last_line=0;
 xwin=1;
 {
	empty_char = 160;	/* for Linux console asci(127) is empty */
   
	color_scheme_ind=4;
 };
 default_lang=7;	// default is greek iso
 return DRIVER_XLIB;
}

void titletext ()
{
    char buf[MAXFLEN];
	int sstat=0;

	if(cbfp->b_fname[0]=='/') 
	sstat=snprintf(buf,MAXFLEN,"xe %s",cbfp->b_fname);
	else
	sstat=snprintf(buf,MAXFLEN,"xe %s/%s",cbfp->b_dname,cbfp->b_fname);
	if(sstat>MAXFLEN) MESG("truncated 14");
	if(mapped>0) XStoreName(dis0,parent,buf);
}

void xdab(int y,int b,char *s,int bcolor,int fcolor)
{
 int x1,y1,i;
 char sattr[2];
 x1=4;
 y1=CHEIGHT_DL+4;
 y-=2;
 if(xsubw1==(Pixmap)NULL) { 
 	return;
 };
 XSetFunction(dis0,dl_gc,GXcopy);

 XSetBackground(dis0, dl_gc, colors[color_scheme_ind][bcolor]);
 XSetForeground(dis0, dl_gc, colors[color_scheme_ind][fcolor]);

 XFillRectangle (dis0, xsubw1 , dl_gc, x1, y*CHEIGHT_DL+y1,strlen(s)*CLEN_DL,CHEIGHT_DL); 
 XDrawImageString(dis0, xsubw1, dl_gc, x1,(y+1)*CHEIGHT_DL+y1-CFONTBASE_DL ,s,strlen(s));

 for(i=x1;*s;i+=CLEN_DL,s++) {
	if(b==*s) {
		XSetForeground(dis0,dl_gc, colors[color_scheme_ind][CTRLFORE]);
		sattr[0]=b;
		sattr[1]=0;
		XDrawImageString(dis0,xsubw1,dl_gc,i,(y+1)*CHEIGHT_DL+y1-CFONTBASE_DL,sattr,1);
		XSetForeground(dis0,dl_gc, colors[color_scheme_ind][fcolor]);
		break;
	};
 };
 XFlush(dis0);
}

// removes the window box so the underground text will 
// be shown again!
void remove_box()
{
 XUnmapWindow(dis0, subw1);
 if(xsubw1!=(Pixmap)NULL) {
 	XFreePixmap(dis0,xsubw1);
 	xsubw1=(Pixmap)NULL;
 	drv_color(BACKGROUND,FOREGROUND);
 };
 XFlush(dis0);
}

void xupdate_box()
{
	XCopyArea(dis0,xsubw1,subw1, dl_gc, 0,0, w1_xl,w1_yl, 0,0);
}

void clear_hmenu()
{
  XClearWindow(dis0,wmenu);
#if	HLINES
  XDrawLine(dis0, wmenu, dlbotom_gc, 0, 0, parent_width, 0);
  XDrawLine(dis0, wmenu, dltop_gc, 0, dl_height-1, parent_width, dl_height-1);
#endif
}

void show_wmenu()
{
  XClearWindow(dis0,wmenu);
#if	HLINES
  XDrawLine(dis0, wmenu, dlbotom_gc, 0, 0, parent_width, 0);
  XDrawLine(dis0, wmenu, dltop_gc, 0, dl_height-1, parent_width, dl_height-1);
#endif
 top_menu(1);  
}

void hdab(int x,int b,char *s,int bcolor, int fcolor)
{
 int x1,y1,i;
 int h;
 char sattr[2];
 x1=CLEN_DL*x;
 y1=1;
 h=CHEIGHT_DL;
 XSetFunction(dis0,dl_gc,GXcopy);

 XSetBackground(dis0, dl_gc, colors[color_scheme_ind][bcolor]);
 XSetForeground(dis0, dl_gc, colors[color_scheme_ind][fcolor]);
 XFillRectangle (dis0, wmenu , dl_gc, x1, y1,strlen(s)*CLEN_DL,h-CFONTBASE_DL); 
 
 XDrawImageString(dis0, wmenu, dl_gc, x1, h+y1-CFONTBASE_DL ,s,strlen(s));
// return;
 for(i=x1;*s;i+=CLEN_DL,s++) {
	if(b==*s) {
		XSetForeground(dis0,dl_gc, colors[color_scheme_ind][CTRLFORE]);
		sattr[0]=b;
		sattr[1]=0;
		XDrawImageString(dis0,wmenu,dl_gc,i,h+y1-CFONTBASE_DL,sattr,1);
		XSetForeground(dis0,dl_gc, colors[color_scheme_ind][fcolor]);
		break;
	};
 };
 XFlush(dis0);
}


/*
 * Write a message into message line
 */
void drv_msg_line(char *arg)
{
	if (discmd == FALSE || macro_exec) return;
	
	if(entry_mode) { 
		hide_cursor_dl();	
	};
	set_cursor_dl(0);	// set x-coordinates
	show_downline();
	put_string_dl((char *)str2out(arg));

	drv_flush();
}



/* change color scheme */
int change_color_scheme(int  n)
{
 if(n<1 || n>COLOR_SCHEMES)	{
	color_scheme_ind=0;
 } else {
	color_scheme_ind=n-1;
 };
 update_all = TRUE;
 if(!discmd) return (TRUE);

 drv_back_color();
 set_windows_color();
 drv_update_styles();
 return(TRUE);
}

void refresh_menu()
{
	top_menu(0);
}


void update_screen_size()
{
 WINDP *wp;
 int stat=0;

 stat=resize_screen(1); 

 if(stat)
 {
	lbegin(window_list);
	while((wp=(WINDP *)lget(window_list))!=NULL)
	{
		drv_set_wvs(wp);
	}
 }
}

/* Check horizonttal window sibling position */
int check_w_sibling(WINDP *wp,int left,int top,int new_height)
{
 WINDP *wp1;

 lbegin(window_list); 
 while((wp1=(WINDP *)lget(window_list))!=NULL){
	if(wp1==cwp) continue;
	if(wp1->gwp->h_flags) continue;	/* already updated  */
	if(
		(!left && wp->gwp->d_xpos+wp->gwp->d_width+SBWIDTH == wp1->gwp->d_xpos) ||
		(left && wp1->gwp->d_xpos+wp1->gwp->d_width+SBWIDTH == wp->gwp->d_xpos) 
	){
		if(
		 (wp->gwp->d_ypos+wp->gwp->d_height == wp1->gwp->d_ypos+wp1->gwp->d_height && !top) ||
		 (wp->gwp->d_ypos == wp1->gwp->d_ypos && top)
		){
			if(wp1->gwp->d_height+new_height<30) { wp1->gwp->h_flags=-1;return false;};
			wp1->gwp->h_flags = 2;
			if(
				(left && wp1->gwp->d_xpos>1) ||
				(!left && wp1->gwp->d_xpos+wp->gwp->d_width < parent_width-SBWIDTH-2)
			){
				if(!check_w_sibling(wp1,left,top,new_height)) return false;
			};
			if(top) wp1->gwp->d_ypos -= new_height;
			wp1->gwp->d_height += new_height;
			return true;
		};
	};
 };

 return true;
}


int check_v_sibling(WINDP *wp,int left,int top,int new_width)
{
 WINDP *wp1;

 lbegin(window_list);
 while((wp1=(WINDP *)lget(window_list))!=NULL){
	if(wp1==cwp) continue;
	if(wp1->gwp->h_flags) continue;	/* already updated  */
	if(
		(!top && wp->gwp->d_ypos+wp->gwp->d_height == wp1->gwp->d_ypos) ||	/* if on the bottom  */
		(top && wp1->gwp->d_ypos+wp1->gwp->d_height == wp->gwp->d_ypos) 		/* or on the top  */
	){
		if(
		 (wp->gwp->d_xpos+wp->gwp->d_width == wp1->gwp->d_xpos+wp1->gwp->d_width && left) ||	/* they have the same right boundary */
		 (wp->gwp->d_xpos == wp1->gwp->d_xpos && !left)	/* they have the same left boundary  */
		){
			if(wp1->gwp->d_width+new_width<10) { wp1->gwp->h_flags=-1;return false;};
			wp1->gwp->h_flags = 2;
			if(
				(top && wp1->gwp->d_ypos>0) ||		/* not a top window  */
				(!top && wp1->gwp->d_ypos+wp->gwp->d_height < parent_height-2*dl_height-2)
			){
				if(!check_v_sibling(wp1,left,top,new_width)) return false;
			};
			if(!left) wp1->gwp->d_xpos -= new_width;
			wp1->gwp->d_width += new_width;
			return true;
		};
	};
 };

 return true;
}


int hresize_wind(int diff)
{
/*
 horizontal resize window 
 it means resize the rows of it.
*/
 
 WINDP *wp=NULL;
 WINDP *wp1=NULL;
 int error=0;
 int n=diff*CHEIGHT;

 if(cwp->gwp->d_height+n<40) return false; 
 lbegin(window_list);
 while((wp1=(WINDP *)lget(window_list))!=NULL){
	wp1->gwp->h_flags=0;	/* set update flag */
 };
 if(cwp->gwp->d_ypos>dl_height) {	/* current window not on the top of screen  */
	lbegin(window_list);
	while((wp1=(WINDP *)lget(window_list))!=NULL){
		if(wp1->gwp->h_flags) continue;	/* already checked  */
		if(wp1->gwp->d_ypos+wp1->gwp->d_height == cwp->gwp->d_ypos){	/* common top boundary with current  */
			_el *cwin=window_list->current;
			if(cwp->gwp->d_xpos+cwp->gwp->d_width-SBWIDTH <wp1->gwp->d_xpos ||
				cwp->gwp->d_xpos > wp1->gwp->d_xpos+wp1->gwp->d_width) {
					continue;
				};
			wp=wp1;
			// check siblings left and right with common boundary with wp1
			if(wp1->gwp->d_height-n<40) { wp1->gwp->h_flags=-1;error=1;break;};
			wp1->gwp->h_flags =2;

			if(!check_w_sibling(wp1,0,TBOTTOM,-n)) { error=2;break;};
			if(!check_w_sibling(wp1,1,TBOTTOM,-n)) { error=3;break;};
			window_list->current=cwin;

			wp1->gwp->d_height -=n;
		}; 
 	};
	if(wp==NULL) return false;	/* no window found to give/take space  */
//	check siblings left and right with common boundary with wp1
	if(!check_w_sibling(cwp,0,TTOP,n)) { error=4;}
	else {
	if(!check_w_sibling(cwp,1,TTOP,n)) { error=5;} 
	if(!error)	{
		cwp->gwp->h_flags=2;
		cwp->gwp->d_height +=n;	
		cwp->gwp->d_ypos-=n;
	};
	};
 } else {	/* current window on top of screen  */
 	lbegin(window_list);
	while((wp1=(WINDP *)lget(window_list))!=NULL){
		if(wp1->gwp->h_flags) continue;	/* already checked  */

		if(cwp->gwp->d_ypos+cwp->gwp->d_height == wp1->gwp->d_ypos){
			_el *cwin=window_list->current;
			if(cwp->gwp->d_xpos+cwp->gwp->d_width-SBWIDTH < wp1->gwp->d_xpos ||
				cwp->gwp->d_xpos > wp1->gwp->d_xpos+wp1->gwp->d_width) continue;	/* check sbwidth  */
			wp=wp1;
			// check siblings left and right with common boundary with wp1
			if(wp1->gwp->d_height-n<40 || wp1->gwp->d_ypos+n > parent_height-40 ) { error=6;wp1->gwp->h_flags=-1;break;};
			wp1->gwp->h_flags=2;

			if(!check_w_sibling(wp1,0,1,-n)) { error=7;break;}
			if(!check_w_sibling(wp1,1,1,-n)) { error=8;break;} 
			window_list->current=cwin;

			wp1->gwp->d_ypos+=n;
			wp1->gwp->d_height -=n;
		}; 
	}; 
	if(wp==NULL) return false;	/* no window found to give/take space  */
	// check siblings left and right with common boundary with wp1
	if(!check_w_sibling(cwp,0,0,n)) { error=9;}
	else { 
	if(!check_w_sibling(cwp,1,0,n)) { error=10;}
	if(!error) {
		cwp->gwp->h_flags=2;
		cwp->gwp->d_height +=n;	/* set  */

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

int vresize_wind(int diff)
{
/*
 vertical resize window 
 it means resize the columns of it.
*/
 
 WINDP *wp=NULL;
 WINDP *wp1=NULL;
 int error=0;
 int n=diff*CLEN;

 if(cwp->gwp->d_width+n<100) return false; 
 lbegin(window_list);
 while((wp1=(WINDP *)lget(window_list))!=NULL){
	wp1->gwp->h_flags=0;	/* set update flag */
 };

 if(cwp->gwp->d_xpos>0) {	/* current window not on the left of the screen  */
	lbegin(window_list);
	while((wp1=(WINDP *)lget(window_list))!=NULL){
		if(wp1->gwp->h_flags) continue;	/* already checked  */
		if(wp1->gwp->d_xpos+wp1->gwp->d_width+SBWIDTH == cwp->gwp->d_xpos){	/* common left boundary with current  */
			_el *cwin=window_list->current;
			if(cwp->gwp->d_ypos+cwp->gwp->d_height < wp1->gwp->d_ypos ||
				cwp->gwp->d_ypos > wp1->gwp->d_ypos+wp1->gwp->d_height) {
					continue;
				};
			wp=wp1;
			// check siblings top and bottom with common boundary with wp1
			if(wp1->gwp->d_width-n<100) { wp1->gwp->h_flags=-1;error=1;break;};
			wp1->gwp->h_flags =2;

			if(!check_v_sibling(wp1,TLEFT, TBOTTOM,-n)) { error=2;break;};
			if(!check_v_sibling(wp1,TLEFT ,TTOP,-n)) { error=3;break;};
			window_list->current=cwin;

			wp1->gwp->d_width -=n;
		}; 
 	};
	if(wp==NULL) return false;	/* no window found to give/take space  */
//	check siblings top and bottom with common boundary with wp1
	if(!check_v_sibling(cwp,TRIGHT,TTOP,n)) { error=4;}
	else {
	if(!check_v_sibling(cwp,TRIGHT,TBOTTOM,n)) { error=5;} 
	if(!error)	{
		cwp->gwp->h_flags=2;
		cwp->gwp->d_width +=n;	
		cwp->gwp->d_xpos-=n;
	};
	};
 } else {	/* current window on left of the screen  */
 	lbegin(window_list);
	while((wp1=(WINDP *)lget(window_list))!=NULL){
		if(wp1->gwp->h_flags) continue;	/* already checked  */
		if(cwp->gwp->d_xpos+cwp->gwp->d_width+SBWIDTH == wp1->gwp->d_xpos){
			_el *cwin=window_list->current;
			if(cwp->gwp->d_ypos+cwp->gwp->d_height < wp1->gwp->d_ypos ||
				cwp->gwp->d_ypos > wp1->gwp->d_ypos+wp1->gwp->d_height) continue;
			wp=wp1;
			// check siblings left and right with common boundary with wp1
			if(wp1->gwp->d_width -n<100 || wp1->gwp->d_xpos+n> parent_width-150 ) { error=6;wp1->gwp->h_flags=-1;break;};
			wp1->gwp->h_flags=2;

			if(!check_v_sibling(wp1,TRIGHT,0,-n)) { error=7;break;}
			if(!check_v_sibling(wp1,TRIGHT,1,-n)) { error=8;break;} 
			window_list->current=cwin;

			wp1->gwp->d_xpos+=n;
			wp1->gwp->d_width -=n;
		}; 
	}; 
	if(wp==NULL) return false;	/* no window found to give/take space  */
	// check siblings left and right with common boundary with wp1
	if(!check_v_sibling(cwp,TLEFT,0,n)) { error=9;}
	else { 
	if(!check_v_sibling(cwp,TLEFT,1,n)) { error=10;}
	if(!error) {
		cwp->gwp->h_flags=2;
		cwp->gwp->d_width +=n;	/* set  */
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


int new_shell(int n)
{
	// start terminal in background!, no return code!, so return always true.
	if(system("xterm&")==0) return(TRUE);
	else return(FALSE);
}

// dummy commands

int set_color(int n)
{
	return(TRUE);
}

int open_file_dialog(char *fname,int n)
{
	return(TRUE);
}

void prepare_converter(int local_lang)
{

}

int set_bg(int n)
{
	return(TRUE);
}

int toggle_mouse(int n)
{
	return FALSE;
}

int text_mouse_pos(int n)
{
	return FALSE;
}

int text_mouse_left_press(int n)
{
	return FALSE;
}

int text_mouse_release(int n)
{
	return FALSE;
}

int text_mouse_right_press(int n)
{
	return FALSE;
}

int text_mouse_error(int n)
{
	return FALSE;
}

void create_default_scheme()
{
}

/* -- */
