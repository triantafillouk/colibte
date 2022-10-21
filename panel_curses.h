
#if	XLIB
typedef struct WINDOW {
	int dummy;
} WINDOW;
#endif

// BOX definition
typedef struct BOX {
#if	PCURSES
	WINDOW *wnd;
	PANEL *panel;
	int border;
#endif
	char title[256];
	int x,y;
	int x2,y2;
	int cline;
} BOX;

// box-mouse interaction (dummy for xlib)
int get_box_cline();
int get_mouse_box_line();
void disp_box(char *title,int atr,int y1,int x1,int y2,int x2);
int dspv(WINDOW *disp_window,int x,int y,char *st);
void set_box_cline(int line);

void hdab(int x,int b,char *s,int bcolor,int fcolor);
void clear_hmenu();

int resize_screen(int init);
int vresize_wind(int diff);
int hresize_wind(int diff);

typedef int (*function_int)(int);
function_int key_convert_from_mouse(function_int execf);

int win_getstring(WINDOW *disp_window,char *prompt, char *st1,int maxlen,int disinp);

