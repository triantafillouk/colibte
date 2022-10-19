/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.
*/
/* gtk3 screen driver */
	
#include "xe.h"
#define	GTK_SEARCH	0	/* use a different dialog for search operations. */

#include <glib.h>

#include "geditdisplay3.h"
#include "gtk_support.h"
#include "gtkterm3.h"
#include "xkeys.h"
#include "xthemes.h"
#include <X11/Xlib.h>
#include "menus.h"
#include "icon.h"

#include "xthemes.c"
#define	SHOW_CLINE	1
#define	LABEL_STATUS	0

// in gtk_common function definitions
GtkFileChooser *show_file_chooser(char *title, int action,char *name);
void cb_close_window(GtkWidget *w, WINDP * wp);
GtkWidget *new_combo_box(char **list,int active);
void gtk_entry_commit_cb(GtkIMContext *context, const gchar  *str, gpointer data);
void button_color_save(GtkWidget *wd, gpointer *data);
void set_cursor(int val,char *from);
void set_window_font(WINDP *wp);
void ge_edit_display_expose_area(GtkWidget *widget, GdkRectangle *area);

extern int update_all;
extern short  *kbdsptr;
extern FILEBUF *cbfp;
extern int clen_error;

void set_box_font_size(GtkWidget *widget,int font_size);
void set_box_color(GtkWidget *box,char *color,char *bgcolor);
void set_box_style(GtkWidget *widget,int font_size,char *fgcolor,char *bgcolor);
void set_edit_font(GeEditDisplay *wd);
GtkWidget *pixbuf_icon(const char **embeded_icon);
int check_cr(WINDP *wp,char *msg);

/* local variables */
int color_scheme_ind;
int compact1 = 1;

int drv_type=DRIVER_GTK3;
int drv_colors=256;

int changed_color=0;
GIConv cd;

GtkWidget * create_top_menu(MENUS *menu_des , GtkWidget *parent,GtkAccelGroup  *menu_accels);
extern GtkWidget *popup_select_on, *popup_select_off, *popup_boxed_region, *popup_sort_menu;
void new_gwp_draw(GWINDP *gwp,WINDP *wp,GtkWidget *parent,int ptype);

cairo_t *begin_draw(GeEditDisplay *wd,cairo_region_t *region,char *message);
void end_draw(GeEditDisplay *wd,char *message);

char **mono_fonts_list;
int mono_fonts_number;

GtkStyle *st1a,*st1i,*st3a,*st3i;
static GtkIMContext *im_context;
GtkWidget *table1,*colors_win=NULL;
void set_box_background_color(GtkWidget *box,char *color);

extern int utflen;
extern char utfokey[10];

void show_color_sample(GtkWidget *table,int ypos,char *text);
void color_close_select(GtkWidget *wd, gpointer *data);
void put_to_table(GtkWidget *wdg, GtkWidget *table, int ypos, int xpos, int align);
void prepare_converter(int local_lang);

GtkSelectionData *seldata0=NULL;
char *import_buffer=NULL;
int import_length=0;
int selection_received_flag=0;

int novalue;
int quote_flag=0;
int describe_flag;

void drv_update_styles();
//void record_session_key(short);
int set_play_string1(char *st1);
void set_record_string1(char *st2);
int set_sposition(WINDP *wp, int *st, int *l);

void drv_back_color_color();
int newxy(WINDP *wp);
GtkWidget * new_toolbar(GtkWidget *parent, GtkWidget *box, T_ELEMENT telem[], int orient, int icon_size,int side);
char *str2ctrl(char *s2);
extern GtkWidget *Interface_pixmap_new(GtkWidget *parent, gchar **data);
void ge_edit_display_expose_area    (GtkWidget *widget, GdkRectangle *area);
GtkWidget *ge_edit_display_new();
char *g2utf(char *str);

void on_selection_get_event(GtkWidget *widget, GtkSelectionData *seldata,
		   guint info, guint time_stamp, gpointer data);
void on_selection_received_event(GtkWidget *widget, GtkSelectionData *seldata,
		   gpointer data);
void font_ok_select(GtkFontChooser *fs, gchar *fontname, gpointer *data);
void cb_menu(GtkWidget *widget, M_element *element);
GWINDP *ge_cedit_new(GtkWidget *parent, WINDP *wp,int ptype);

void on_list1_change_row(GtkTreeSelection *selection, GtkTreeModel *mlist1);
void on_list1_activate_row(GtkTreeSelection *selection, GtkTreeModel *mlist1);
void on_select_hide(GtkWidget *wd,GtkListStore *mlist1);

int on_select_destroy(GtkWidget *wd,GtkListStore *mlist1);
int on_select_delete(GtkWidget *wd,GdkEventAny *event);
void cb_set_position(GtkAdjustment *adj, GtkWidget *draw);
void delete_gwindow(WINDP *wp);
void drv_free_win(WINDP *wp);
gboolean on_parent_destroy(GtkWidget *w, GdkEvent *ev, gpointer data);
void on_destroy(GtkWidget *widget);
GtkWidget * create_select_window();
gboolean on_wlist_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data);

void  set_dtlist(GtkWidget *list, char *glist[]);
int selectl(char *title,char *m[],int nu,int lmax,int sx,int sy,int width,int active);
extern int list_update(char *);
extern alist *window_list;
void init_color();
int set_font(char *);
void sinsert_nl(char *dat,int nbytes);

WINDP * make_split(WINDP *oldwindow);
void expose_line(int i, WINDP *wp);
int set_fontindex(int);
void cb_tools(GtkWidget *w, T_ELEMENT *telem );

void allocate_virtual_window(WINDP *wp);

int slide_flag;
int in_slide=0;

BTWE rootbtwe;
int btindex=0;

GtkWidget *list1;
int index_value=0;

Display *dis0;
Screen *screen_ptr;
int screen_num;

int cursor_showing=0;
int px,py,ppx,ppy;	/* in pixels  */
int cposx=0;
int cposy=0;
int cpposx=-1;
int cpposy=-1;	/* in characters  */
float CLEN,CHEIGHT;
float SLBASE;
int CLENI,CHEIGHTI,CFONTBASEI;

int dra_width(GWINDP *);
int dra_height(GWINDP *);

int font_index=1;	// Set here the default font_index
int font_weight=CAIRO_FONT_WEIGHT_NORMAL;

char *current_font_name=NULL;

int ncolors;

GdkRGBA *colors[COLOR_SCHEMES][XCOLOR_TYPES];

GdkRGBA ccolorb,ccolorf;
GdkRGBA ccolorb_default;

GtkWidget *parent;

GtkWidget *gs_label;
GtkWidget *toolbar1,*toolbar2;
GtkWidget *gs_entry;
GtkWidget *search_tbar;
//GtkWidget *replace_toolbar;
char gs_entry_txt[512];
GtkWidget *entry_ok, *entry_cancel;
GtkWidget *statusbar1;
int gs_entry_return_flag;
GtkWidget *color_dialog;

extern int startup_exe;

GtkWidget *parent_title_bar=NULL;

GtkWidget*
create_parent (void)
{
  GtkWidget *parent;
  GtkWidget *vbox1;
  GtkAccelGroup *accel_group;
  GtkWidget *upper_hbox;

  int parent_width=800;
  int parent_height=500;
  int parent_x=10;
  int parent_y=10;

 /* Create the menubar in vbox */
  accel_group = gtk_accel_group_new ();
  parent = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  if((int)bt_dval("use_titlebar")){
//	MESG("create_parent");
	parent_title_bar = gtk_header_bar_new();
	/* Use gtk titlebar  */
	gtk_header_bar_set_decoration_layout((GtkHeaderBar *)parent_title_bar,
		"menu:minimize,maximize,close");
	gtk_header_bar_set_title((GtkHeaderBar *)parent_title_bar,"Parent");
	gtk_window_set_titlebar((GtkWindow *)parent,parent_title_bar);
	gtk_widget_show(parent_title_bar);
	gtk_header_bar_set_show_close_button((GtkHeaderBar *)parent_title_bar,1);
  };
  // MESG("create_parent: reset_position=%d",(int)bt_dval("reset_position"));
  if(!(int)bt_dval("reset_position")) {
#if	USE_GLIB0
	  parent_width = get_cfg_int("x11_width",parent_width);
	  parent_height = get_cfg_int("x11_height",parent_height);
	  parent_x = get_cfg_int("x11_x",parent_x);
	  parent_y = get_cfg_int("x11_y",parent_y);
#else
	  parent_width  = (int)bt_dval("x11_width");
	  parent_height = (int)bt_dval("x11_height");
	  parent_x      = (int)bt_dval("x11_x");
	  parent_y      = (int)bt_dval("x11_y");
#endif
  };
//  MESG("create_parent: x=%d y=%d w=%d h=%d",parent_x,parent_y,parent_width,parent_height);
#if	DARWIN
  if(parent_y>=22) parent_y -= 22;
#endif
  gtk_widget_set_size_request(parent,400,300);	/* sets the minimum allowed size!  */
  gtk_window_set_default_size(GTK_WINDOW(parent),parent_width,parent_height);
  gtk_window_move(GTK_WINDOW(parent),parent_x+1,parent_y+1);

  vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);

  gtk_container_add (GTK_CONTAINER (parent), vbox1);
  gtk_widget_show (vbox1);

  popup_select_on = create_top_menu( &m_select_on, parent, accel_group);
  popup_select_off = create_top_menu( &m_select_off, parent, accel_group);
  popup_boxed_region = create_top_menu( &m_boxed_region, parent, accel_group);
  popup_sort_menu = create_top_menu( &m_sort, parent, accel_group);

  if((int)bt_dval("use_titlebar")){
	  create_main_menu(parent_title_bar, parent, &m_topn,accel_group);
//	  GtkWidget * tb = new_toolbar(parent,parent_title_bar,main_toolbar,0,GTK_ICON_SIZE_SMALL_TOOLBAR,2);
  } else {
	  create_main_menu(vbox1, parent, &m_topn,accel_group);
  };

	upper_hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);

//	MESG("create_parent:1 embed_icons=%f",bt_dval("embed_icons"));
	gtk_box_pack_start (GTK_BOX (vbox1), upper_hbox, FALSE, FALSE, 0);

	toolbar1=new_toolbar(parent,upper_hbox,main_toolbar,0,GTK_ICON_SIZE_SMALL_TOOLBAR,0);
//	gtk_toolbar_set_show_arrow(GTK_TOOLBAR(toolbar1),FALSE);
	gtk_widget_show (upper_hbox);
//		select size from GTK_ICON_SIZE_[SMALL_TOOLBAR,LARGE_TOOLBAR,MENU,BUTTON,DND,DIALOG]
	if((int)bt_dval("large_toolbar_icons")) {
		gtk_toolbar_set_icon_size((GtkToolbar *)toolbar1,GTK_ICON_SIZE_LARGE_TOOLBAR);
	} else {
		gtk_toolbar_set_icon_size((GtkToolbar *)toolbar1,GTK_ICON_SIZE_SMALL_TOOLBAR);
	};

	/* gs_label  */
//	MESG("create [search_toolbar]");
  search_tbar=new_toolbar(parent,upper_hbox,search_toolbar,0,GTK_ICON_SIZE_SMALL_TOOLBAR,0);
  gs_label = gtk_label_new("");
  gtk_label_set_text((GtkLabel *)gs_label,"gs_label");

  gtk_box_pack_start (GTK_BOX (upper_hbox), gs_label, FALSE, TRUE, 0);
  gtk_widget_hide(gs_label);
  gtk_widget_hide(search_tbar);
  gs_entry = gtk_entry_new();
  gtk_entry_set_activates_default(GTK_ENTRY(gs_entry), TRUE);
  
  gtk_entry_set_has_frame(GTK_ENTRY(gs_entry), FALSE);
  gtk_box_pack_start (GTK_BOX (upper_hbox), gs_entry, TRUE, TRUE, 0);
  gtk_widget_set_can_focus (gs_entry,TRUE);
  set_box_font_size(upper_hbox,status_font_size);
//	MESG("create [input_toolbar]");
  toolbar2=new_toolbar(parent,upper_hbox,input_toolbar,0,GTK_ICON_SIZE_SMALL_TOOLBAR,0);
  if((int)bt_dval("large_toolbar_icons")) {
  	gtk_toolbar_set_icon_size((GtkToolbar *)toolbar2,GTK_ICON_SIZE_LARGE_TOOLBAR);
  } else {
  	gtk_toolbar_set_icon_size((GtkToolbar *)toolbar2,GTK_ICON_SIZE_SMALL_TOOLBAR);
  };
  gtk_toolbar_set_show_arrow(GTK_TOOLBAR(toolbar2),FALSE);
  gtk_widget_hide(toolbar2);

	/* This is the statusbar. All messages are displayed here ! */
  statusbar1 = gtk_statusbar_new ();
  gtk_widget_set_margin_top(GTK_WIDGET(statusbar1), 0);
  gtk_widget_set_margin_bottom(GTK_WIDGET(statusbar1), 0);

  gtk_widget_show (statusbar1);
  gtk_box_pack_start (GTK_BOX (upper_hbox), statusbar1, TRUE, TRUE, 0);
	/* Set status bar style  */
//  set_box_color(gtk_statusbar_get_message_area ((GtkStatusbar *)statusbar1),"green","yellow");
  gtk_widget_set_size_request (statusbar1, 150,20);	

  /* the following is needed always so we can handle cursor keys correctly */	
  g_signal_connect (GTK_WIDGET (parent), "key_press_event",
                      (GCallback) on_parent_key_press_event,
                      NULL);
  g_signal_connect (parent, "configure_event",
                      (GCallback) on_parent_configure,
                      NULL);
  g_signal_connect (parent, "delete_event",
  						(GCallback) on_parent_destroy, NULL);
  
  g_signal_connect (gs_entry, "key_press_event",
                      (GCallback) on_gs_entry_key_press_event,
                      NULL);

  /* this is needed to send selections correctly! */
  g_signal_connect (parent, "selection_get",
						(GCallback) on_selection_get_event, NULL);

  g_signal_connect (parent, "selection_received",
						(GCallback) on_selection_received_event, NULL);


  gtk_widget_add_events(GTK_WIDGET(parent),GDK_SELECTION_NOTIFY);

	/* create default graphic context */
// set the XWindows icon
  {
	GdkPixbuf *gicon;
	if((gicon = gdk_pixbuf_new_from_xpm_data(icon_xpm)) != NULL)
	{
		gtk_window_set_icon(GTK_WINDOW(parent),gicon);
	}
  };
  /* create the drawing window with scrollbar and statusline */

	cwp = make_split(NULL);
	curgwp = ge_cedit_new(vbox1, cwp,0);
//  MESG("parent created!");
  return parent;
}

/* show current file name on title bar */
void titletext ()
{
    char buf[MAXFLEN];
	if(parent == NULL) return ;
	snprintf(buf,MAXFLEN,"%s",cbfp->b_fname);
	gtk_window_set_title(GTK_WINDOW(parent), buf);
	if(parent_title_bar){
		gtk_header_bar_set_subtitle((GtkHeaderBar *)parent_title_bar,cbfp->b_dname);
	};
}

int color_equal(GdkRGBA *color1,GdkRGBA  *color2)
{
 if(color1->red==color2->red &&
 	color1->green==color2->green &&
	color1->blue==color2->blue) return true;
	return false;
}

void drv_update_styles()
{
 // we have to destroy old styles if they exist!
}

void drv_flush() {	
//	events_flush();
}

void drv_win_flush(WINDP *wp)
{
//	events_flush();
}


void drv_set_default_bgcolor()
{
 GdkRGBA *gcolorb;
 gcolorb = colors[color_scheme_ind][BACKGROUND];
 ccolorb_default.red = (float)gcolorb->red;
 ccolorb_default.green = (float)gcolorb->green;
 ccolorb_default.blue = (float)gcolorb->blue;
}

/* clear the window to default color */
void window_bg_clear(WINDP *wp)
{
	drv_set_default_bgcolor();
//	MESG("window_bg_clear: id=%d",wp->id);
	if(wp!=NULL) {
		if(wp->gwp!=NULL) {
		if(GTK_EDIT_DISPLAY(wp->gwp->draw)!=NULL) {
			cairo_set_source_rgba (GTK_EDIT_DISPLAY(wp->gwp->draw)->cr, ccolorb_default.red, ccolorb_default.green, ccolorb_default.blue,1);
			cairo_rectangle(GTK_EDIT_DISPLAY(wp->gwp->draw)->cr, 0,0,dra_width(wp->gwp),dra_height(wp->gwp));
			cairo_fill(GTK_EDIT_DISPLAY(wp->gwp->draw)->cr);
			} 
		} 
	} ; 
//	MESG("window_bg_clear:end");
	cursor_showing=0;
//	set_cursor(0,"window_bg_clear");
}

void window_clear(WINDP *wp)
{
// dummy for gtk3
}

void drv_color(int fcol,int bcol) 
{
 GdkRGBA *gcolorf,*gcolorb;
#if	NUSE
 if(bcol>XCOLOR_TYPES) {
	MESG("drv_color: bcol=%d > %d",bcol,XCOLOR_TYPES);
 	bcol=0;
 };
 if(fcol>XCOLOR_TYPES) {
	MESG("drv_color: fcol=%d > %d",fcol,XCOLOR_TYPES);
 	fcol=1;
 };
#endif
	gcolorb = colors[color_scheme_ind][bcol];
	ccolorb.red = (float)gcolorb->red;
	ccolorb.green = (float)gcolorb->green;
	ccolorb.blue = (float)gcolorb->blue;
	gcolorf = colors[color_scheme_ind][fcol];
	ccolorf.red = (float)gcolorf->red;
	ccolorf.green = (float)gcolorf->green;
	ccolorf.blue = (float)gcolorf->blue;
}

void init_color()
{
 int i,j;
 GdkRGBA color;
	/* Read the colors from the conf files  */
  color_scheme_read();

 for(j=0;j<COLOR_SCHEMES;j++)
 {
	for(i=0;i<XCOLOR_TYPES;i++) 
	{
		if(!gdk_rgba_parse(&color,color_name[j][i] )) {
	  		ERROR("color %s is not in database",color_name[j][i]);
			exit(0);
		};
		colors[j][i]=(GdkRGBA *) malloc(sizeof(GdkRGBA));
		memcpy(colors[j][i],&color,sizeof(GdkRGBA));
		ncolors++;
 	}
 };
}


/* GTK3  */
void show_cursor (char *from)
{
	GdkRectangle area;
	cairo_region_t *region;
	GeEditDisplay *wd;
	static int ind=0;

	// MESG("show_cursor: start ind=%d showing=%d",ind,cursor_showing);
	if(!(cbfp->b_flag==FSNOTES || cbfp->b_flag==FSNOTESN || cbfp->b_flag & FSNLIST)) {
	wd = (GeEditDisplay *)(cwp->gwp->draw);
	if(ind==2) cursor_showing=0;
	if (cursor_showing) {
		// MESG("show_cursor: already shown! %d>",ind++);
		return;
	};
	// MESG("show_cursor: start from %s",from);
#if	SHOW_CLINE
	area.x = 0;
	area.height = 1;	/* current line height  */
	area.y = py+CHEIGHTI-area.height;
	area.width = cwp->gwp->width;
	// MESG("show_cursor: from %s <%d y=%d y=%d",from,ind,area.x,area.y);	
	if(area.height + area.width < 2 ) {
		return;
	};

	region = cairo_region_create_rectangle((cairo_rectangle_int_t *)&area);
	wd->cr = begin_draw(wd,region,"show_cursor");

	cairo_set_operator(wd->cr,CAIRO_OPERATOR_OVER);
	cairo_set_source_rgba(wd->cr,0.4,0.4,0.4,0.5);

	cairo_rectangle(wd->cr,	area.x, area.y,area.width,area.height);
	cairo_fill(wd->cr);
	cairo_region_destroy(region);
	end_draw(wd,"show_cursor:line end");
#endif
	/* get current cursor area */
	area.x = px+1;
	area.y = py;

	area.width = CLEN;
	area.height = CHEIGHTI;

	if(area.height + area.width < 2 ) {
		// MESG("show_cursor: wrong height %d>",ind++);
		return;
	};

	region = cairo_region_create_rectangle((cairo_rectangle_int_t *)&area);
	wd->cr = begin_draw(wd,region,"show_cursor");

	cairo_set_operator(wd->cr,CAIRO_OPERATOR_OVER);
	cairo_set_source_rgba(wd->cr,1,0.0,0.0,0.5);

	cairo_rectangle(wd->cr,	area.x, area.y,area.width,area.height);
	cairo_fill(wd->cr);
	cairo_region_destroy(region);

	end_draw(wd,"show_cursor:end");
	};
    ppx=px;
	ppy=py;
	cpposx=cposx;cpposy=cposy;
	cursor_showing = 1;
	// MESG("show_cursor [%s] id=%d x=%d y=%d %d>",from,cwp->id,cposx,cposy,ind++);
}

void set_cursor(int val,char *from)
{
	cursor_showing=val;
//	MESG("set_cursor: val=%d %s",val,from);
}

int cursor_status()
{
	return cursor_showing;
}

/* just refresh the current line */
void expose_line(int line, WINDP *wp)
{
#if	0
  GdkRectangle area;
  area.x = 0;
  area.y = line*CHEIGHTI;
  area.width = dra_width(wp->gwp);
  area.height = CHEIGHTI;
  ge_edit_display_expose_area(wp->gwp->draw,&area);
#endif
}

void hide_cursor (char *from)
{
	cairo_region_t *region;
	GdkRectangle area;
	static int ind=0;
	// MESG("hide_cursor: %d ind=%d",cursor_showing,ind);
	if (cursor_showing == 0) return;
#if	1
	if(cbfp->b_flag==FSNOTES || cbfp->b_flag==FSNOTESN || cbfp->b_flag & FSNLIST) {
	cursor_showing = 0;
	return;		
	};
#endif
	if(cpposy>cwp->w_ntrows) {
		// MESG("hide_cursor: skip %d > %d",cpposy,cwp->w_ntrows);
		return;
	};
	GeEditDisplay *wd = (GeEditDisplay *)(cwp->gwp->draw);
	cursor_showing=0;

	// MESG("hide_cursor: from %s x=%d y=%d num=%d<",from,cpposx,cpposy,ind);
	/* get current cursor area */
	area.x =0;
	area.y = ppy;

	area.width = cwp->gwp->width;
	area.height = CHEIGHTI;

	region = cairo_region_create_rectangle((cairo_rectangle_int_t *)&area);
	wd->cr = begin_draw(wd,region,"hide cursor");

	cairo_region_destroy(region);
	// MESG("hide_cursor: draw line %d",cpposy);
	draw_window_line(wd->wp,cpposy);
	end_draw(wd,"hide cursor");
	// MESG("hide_cursor: end");
	cursor_showing = 0;
	ind++;
	return;
}

char *get_font_string();

void set_edit_font(GeEditDisplay *wd)
{
 
 if(current_font_name == NULL) return;
 if(wd->width==1) return;
 if(wd->cr == NULL) { 
	ERROR("set_edit_font: end cr is NULL!!!!");
	return;
 };

	/* set the pango font description  */
	char *font_str=get_font_string();
	if(wd->ge_font_desc!=NULL) pango_font_description_free (wd->ge_font_desc);
	wd->ge_font_desc = pango_font_description_from_string (font_str);

	/* use pango metrics  */
	if(wd->layout==NULL) {
		wd->layout=pango_cairo_create_layout (wd->cr);
		// MESG("set_edit_font: new layout!");
	};
	PangoContext *context = pango_layout_get_context(wd->layout);
	PangoLanguage *language = pango_language_get_default();
	// PangoLanguage *language = pango_language_from_string("ja_jp");
	PangoFontMetrics* metrics = pango_context_get_metrics(context, wd->ge_font_desc, language);;
	double pango_w = ceil((double)pango_font_metrics_get_approximate_digit_width (metrics)/PANGO_SCALE);
	double pango_ascent = (double)pango_font_metrics_get_ascent(metrics) ;
	double pango_descent = (double)pango_font_metrics_get_descent(metrics) ;
	double pango_h = ceil((double)(pango_ascent+pango_descent+1)/PANGO_SCALE);
	CLEN=pango_w;
	CHEIGHT = pango_h;
	SLBASE = pango_h;
	CHEIGHTI= (int)(CHEIGHT);
	// MESG("set_edit_font:%s pango width = %f->%d h=%f CLEN=%f CHEIGHT=%f %d",font_str,pango_w,(int)pango_w,pango_h,CLEN,CHEIGHT,CHEIGHTI);
	CLENI = (int)CLEN;
	// g_object_unref(layout);
}

void update_full(int f)
{
/* dummy for gtk3 */
}

void set_window_font(WINDP *wp)
{
 GeEditDisplay *wd = (GeEditDisplay *)(wp->gwp->draw);
 // MESG("set_window_font:");
 set_edit_font(wd);
}

int select_font(int n)
{
 char font_name[MAXFLEN];
 int status;
 gchar *st;

// prepare_converter(default_local_codepage);
// st = g_convert_with_iconv(sst,-1,cd,&r,&w,NULL);
 st = strdup("ΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠΡΣΤΥΦΧΨΩ");	/* utf string  */

 if(!macro_exec) {
	GtkFontChooser *fs;
 	// get font_name from font dialog
	fs = (GtkFontChooser *)gtk_font_chooser_dialog_new("main font",GTK_WINDOW(parent));
#if	DARWIN
	gtk_font_chooser_set_font(fs,"menlo 14");
#else
	gtk_font_chooser_set_font(fs,"Monospace 12");
#endif
	gtk_font_chooser_set_preview_text(fs,st);
	g_signal_connect(fs,"destroy",(GCallback) on_destroy,fs);

	 g_signal_connect(fs,"font-activated",(GCallback) font_ok_select,fs);
//	 g_signal_connect(GTK_OBJECT((GTK_FONT_CHOOSER(fs)))->cancel_button),"clicked",(GCallback) gtk_widget_destroy,GTK_OBJECT(fs));

//	gtk_window_set_transient_for(window,parent)
//	gtk_window_set_modal(fs,1);
	gtk_widget_show((GtkWidget *)fs);
	g_free(st);
	return(FALSE);
 } else {
 	g_free(st);
	font_name[0]=0;
	if(nextarg("New font: ",font_name, MAXFLEN,true) != TRUE) return(FALSE);
 };
 status = set_font(font_name);
 g_free(st);
 if(status) {
	return(TRUE);
 } else return(FALSE);
}

void show_slide(WINDP *wp)
{
 int start, len;
 gfloat value;
 if(in_slide) return;
 set_sposition(wp,&start,&len);
 // MESG("show_slide: slide_flag=%d",slide_flag);
 value = start;
 if(value==0) value=1;
 if(slide_flag!=2) 
 {
 slide_flag=1;

 if(wp->w_fp == NULL) {
	gtk_adjustment_set_page_size(wp->gwp->draw_adj,0.0);
 } else {
	num lines;

	if(wp->w_fp->b_flag & FSNLIST) lines=wp->w_fp->dir_list_str->size;
#if	TNOTES
	else if(wp->w_fp->b_flag & FSNOTESN) lines=wp->w_fp->dir_list_str->size;
	else if(wp->w_fp->b_flag & FSNOTES) lines=wp->w_fp->b_tag_list->size;
#endif
	else lines=wp->w_fp->lines;

	len = wp->w_ntrows;

	gtk_adjustment_configure(wp->gwp->draw_adj,
		value,	/* the new value  */
		0.0,	/* The new minimum value  */
		lines,	/* the new maximum value  */
	1,
	len/2,
	len
	);
 };

 slide_flag=1;
 gtk_adjustment_set_value(wp->gwp->draw_adj, value);
 slide_flag=0;
 };
 // MESG("show_slide: end");
}

void drv_start_window_update(WINDP *wp)
{
 cairo_region_t *region;
 GeEditDisplay *wd = (GeEditDisplay *)(wp->gwp->draw);

 region = gdk_window_get_visible_region (wd->edit_window);
 wd->cr = begin_draw(wd,region,"drv_start_window_update");
 // window_bg_clear(wp);
}

GtkWidget * create_select_window()
{
  GtkWidget *select_window;
  GtkWidget *vbox2;
  GtkListStore *mlist1;	// model
  GtkTreeSelection *list1_select=NULL;
  char *stitles[] = { "value",NULL };
//  MESG("create_select_window:");
  select_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_window_set_modal(GTK_WINDOW(select_window),1); // allow input only from this window
  gtk_window_set_title (GTK_WINDOW (select_window), "select");
  gtk_window_set_transient_for (GTK_WINDOW(select_window),GTK_WINDOW(parent));

  gtk_widget_set_size_request (select_window, 200,200);

  vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (select_window), vbox2);

  mlist1 = gtk_list_store_new(1,G_TYPE_STRING);
  list1 = kt_new_listview(mlist1,vbox2,stitles,0);
  list1_select = gtk_tree_view_get_selection (GTK_TREE_VIEW (list1));
  gtk_tree_selection_set_mode (list1_select, GTK_SELECTION_BROWSE);
	/* Use gtk header bar  */
	GtkWidget *list_title_bar = gtk_header_bar_new();
	gtk_header_bar_set_title((GtkHeaderBar *)list_title_bar,"List");
	gtk_window_set_titlebar((GtkWindow *)select_window,list_title_bar);
	gtk_header_bar_set_show_close_button((GtkHeaderBar *)list_title_bar,1);
	gtk_widget_show(list_title_bar);

  gtk_widget_realize(select_window);

  g_signal_connect (G_OBJECT (select_window), "hide",
  						(GCallback)on_select_hide,mlist1);

  g_signal_connect (G_OBJECT (select_window), "key_press_event",
                      (GCallback) on_wlist_key_press,
                      list1);
  g_signal_connect (G_OBJECT (select_window), "destroy_event",
  						(GCallback) on_select_destroy, mlist1);

  g_signal_connect (G_OBJECT (select_window), "delete-event",
  						(GCallback) on_select_delete, mlist1);

  g_signal_connect (G_OBJECT (list1_select), "changed",
                      (GCallback) on_list1_change_row,
                      list1);

  g_signal_connect (G_OBJECT (list1), "row-activated",
                      (GCallback) on_list1_activate_row,
                      mlist1);
 return(select_window);
}

void put_string_dl(char *string) 
{
#if	1
	gtk_statusbar_pop((GtkStatusbar *)statusbar1,1);
	gtk_statusbar_push((GtkStatusbar *)statusbar1,1,string);
#else
 char *utf;
 gsize r,w;

 utf = utf_check(string,&r);
 if(utf==NULL) {
	prepare_converter(default_local_codepage);
	utf = g_convert_with_iconv(string,-1,cd,&r,&w,NULL);
	gtk_statusbar_push((GtkStatusbar *)statusbar1,1,utf);
	g_free(utf);
 } else {
	gtk_statusbar_push((GtkStatusbar *)statusbar1,1,utf);
 }
#endif
}

/* refresh the current window, expose it  */
void expose_window(WINDP *wp)
{
 if(!check_cr(wp,"expose window")) return;
 GeEditDisplay *wd = (GeEditDisplay *)(wp->gwp->draw); 
#if	1
	end_draw(wd,"expose_window");
#else
	if(wd->in_draw) return;
	if(wd->cr && wp->gwp) {
	gdk_window_end_draw_frame(wd->edit_window,wd->edit_gdk_context);
	wd->cr=NULL;
	wd->region=NULL;
	};
#endif
}


void delete_gwindow(WINDP *wp)
{
 GWINDP *gwp;
 BTWE *btep,*btes,*btec; /* parent, simbling, current */

 gwp = wp->gwp;
 /* find the parent in btree */
 if(gwp->bte->type==0) return; /* this is the only window */
 btec = gwp->bte;
 btep = gwp->bte->parent;

 if(btep==NULL) return; 	/* this is the only window. no parent */
 /* find the simbling */

	if(btec->type==0) return;
	if(btec->type==1) btes=btep->right;
	else btes=btep->left;
	gtk_widget_destroy((GtkWidget *)gwp->bte->pbox);

 /* reparent his simbling */
	if(btes->pan==NULL) { // only a single window underneeth !!
		// we must hide the child windows before reparent the father! and show them again after! (in GTK2 only, GTK1 works ok!)
		GeEditDisplay *wd;
		WINDP *wp;	/* the window to reparent  */

		wd = btes->gw->draw;	/* the old one  */
		wp = wd->wp;
		new_gwp_draw(btes->gw,wp,(GtkWidget *)btep->pbox,btes->type);

		btep->gw=btes->gw; 
		btes->gw->bte=btep;
		if(btep->pan){
			gtk_widget_hide_on_delete((GtkWidget *)(btep->pan));
			btep->pan=NULL;
			free(btep->left);
			free(btep->right);
			btep->left=btep->right=NULL;
		};
		gtk_widget_unrealize((GtkWidget *)wd);
		gtk_container_remove((GtkContainer *)btes->gw->box,(GtkWidget *)btes->pbox);
	}
}

void scheme_names_button_change(GtkWidget *wdg, void* data)
{
 int active,i;
 active=gtk_combo_box_get_active(GTK_COMBO_BOX (wdg));
 color_scheme_ind = active;
 change_color_scheme(color_scheme_ind+1);
 update_screen(1);
  for(i=0;i<CTYPE_ROWS;i++) {
	// update fg,bg color button colors
	gtk_color_chooser_set_rgba(
		(GtkColorChooser *)ctype[i].button_fg,
		colors[color_scheme_ind][ctype[i].fg]
		);
		if(ctype[i].button_bg!=NULL){
	gtk_color_chooser_set_rgba(
		(GtkColorChooser *)ctype[i].button_bg,
		colors[color_scheme_ind][ctype[i].bg]
		);
		};
	// show color sample
	show_color_sample(table1,i+1,"text");
  }; 
}

void set_box_style(GtkWidget *widget,int font_size,char *fgcolor,char *bgcolor)
{
 char style[256];
// opacity removed, snprintf float type is not consistent and always compatible with simple css
 snprintf(style,256,"box { font-size: %dpx; color: %s; background-color: %s; }",font_size,fgcolor,bgcolor);
 GtkCssProvider  *css_provider  = gtk_css_provider_new();
 GtkStyleContext *style_context = gtk_widget_get_style_context(widget);
 
 gtk_css_provider_load_from_data(css_provider,style, -1, NULL);

 gtk_style_context_add_provider(style_context,
	GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}

void set_box_font_size(GtkWidget *widget,int font_size)
{
 char style[256];
 snprintf(style,256,"box { font-size: %dpx; color: blue }",font_size);
 GtkCssProvider  *css_provider  = gtk_css_provider_new();
 GtkStyleContext *style_context = gtk_widget_get_style_context(widget);
 
 gtk_css_provider_load_from_data(css_provider,style, -1, NULL);

 gtk_style_context_add_provider(style_context,
	GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}

void set_box_color(GtkWidget *box,char *color,char *bgcolor)
{
 set_box_style(box,status_font_size,color,bgcolor);
}

void set_box_background_color(GtkWidget *box,char *bgcolor)
{
 char style[64];
 snprintf(style,64,"box { background-color: %s }",bgcolor);
 GtkCssProvider  *css_provider  = gtk_css_provider_new();
 GtkStyleContext *style_context = gtk_widget_get_style_context(box);

 style_context = gtk_widget_get_style_context(box);
 gtk_css_provider_load_from_data(css_provider,(const char *)style, -1, NULL);

 gtk_style_context_add_provider(style_context,
                                   GTK_STYLE_PROVIDER(css_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);
}

void  connect_exit_button(GtkBox *parent_box, GCallback callback_function, void *data)
{
 GtkWidget *exit_button;
 GtkWidget *icon;
 exit_button = gtk_button_new();

 gtk_button_set_relief(GTK_BUTTON(exit_button), GTK_RELIEF_NONE);
 gtk_widget_set_focus_on_click(exit_button, FALSE);
 if(bt_dval("embed_icons")){
	icon = pixbuf_icon((const char **) quit_icon);	/* from embeded icon  */
 } else {
	icon = gtk_image_new_from_icon_name("window-close",GTK_ICON_SIZE_MENU);
 };
 gtk_container_add(GTK_CONTAINER(exit_button), icon);
 gtk_widget_show(exit_button);
 gtk_widget_show(icon);
 
 gtk_box_pack_end (parent_box, (GtkWidget *)exit_button, FALSE, FALSE, 0);
 g_signal_connect(exit_button,"clicked",callback_function,data);
}


gboolean select_file_x(GtkWidget *widget, GdkEventButton *event,gpointer data)
{
 WINDP *wp = (WINDP *)data;
// MESG("select file! wp id=%d",wp->id);
 change_window(wp);
 select_file(1);
 set_update(cwp,UPD_MOVE);
 update_screen(FALSE);
 return FALSE;
}

#if	NUSE
static gint ge_edit_display_scroll(GtkWidget *widget,GdkEvent *event,gpointer user_data)
{
 MESG("scroll!!");
 return TRUE;
}
#endif

void new_gwp_draw(GWINDP *gwp,WINDP *wp,GtkWidget *parent,int ptype)
{
 GtkBox *sb2;
 GeEditDisplay *wd;

 gwp->draw = ge_edit_display_new();
 wd = (GeEditDisplay *)gwp->draw;
 gtk_widget_add_events((GtkWidget *)wd, GDK_SCROLL_MASK);
 gwp->ptype = ptype;

 wd->wp = wp;
 wp->gwp = gwp;
 wd->id = wp->id;
 GTK_EDIT_DISPLAY(gwp->draw)->wp = wp;

 	gwp->box = (void *)gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);	
	gtk_box_pack_start(GTK_BOX(parent), (GtkWidget *)gwp->box, TRUE,TRUE,0);

	gwp->hbox=(void *)gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);	// here goes the new draw

	gtk_box_pack_start(GTK_BOX(gwp->box),(GtkWidget *)gwp->hbox,TRUE,TRUE,0);
	gtk_widget_show((GtkWidget *)gwp->hbox);
	gtk_widget_show((GtkWidget *)gwp->box);

	gtk_widget_realize((GtkWidget *)gwp->hbox);
	gtk_box_pack_start(GTK_BOX(gwp->hbox),gwp->draw,TRUE,TRUE,0);
	
 /* scroll bar */
 gwp->draw_adj = (GtkAdjustment *)gtk_adjustment_new(50.0,0.0,100.0,1.0,5.0,10.0);

 gwp->scroll_bar = (GtkVScrollbar *)gtk_scrollbar_new ( GTK_ORIENTATION_VERTICAL,gwp->draw_adj );
 gtk_box_pack_start (GTK_BOX (gwp->hbox), (GtkWidget *)gwp->scroll_bar, FALSE, FALSE, 0);

 gtk_widget_show ((GtkWidget *)gwp->scroll_bar);
 g_signal_connect(G_OBJECT(gwp->draw_adj), "value_changed", (GCallback) cb_set_position,gwp->draw);

 im_context = gtk_im_context_simple_new ();
 g_signal_connect (im_context, "commit", (GCallback) gtk_entry_commit_cb, NULL);

 /* create status as a triple entry */

 gwp->hstatus = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
 gtk_widget_set_size_request((GtkWidget *)gwp->hstatus,200,1);

 set_box_background_color((GtkWidget *)gwp->hstatus,"yellow");
// set_box_color(gtk_statusbar_get_message_area ((GtkStatusbar *)(gwp->hstatus)),"yellow","red");
 gtk_box_set_spacing ((GtkBox *)gwp->hstatus,0);

 sb2 = (GtkBox *)gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
 gtk_widget_show((GtkWidget *)sb2);

 gwp->status2 = gtk_label_new("");

 gwp->status1 = gtk_statusbar_new();

 GtkEventBox *box_status1=(GtkEventBox *)gtk_event_box_new();
 gtk_container_add((GtkContainer *)box_status1,gwp->status1);
// gtk_event_box_set_visible_window(box_status1,1);
 gtk_widget_show((GtkWidget *)box_status1);

// remove spaces above and below statubar widgets
 gtk_widget_set_margin_top(GTK_WIDGET(gwp->status1), 0);
 gtk_widget_set_margin_bottom(GTK_WIDGET(gwp->status1), 0);

#if	LABEL_STATUS
 gwp->status3 = gtk_label_new("rowcol");
 gtk_widget_set_size_request (gwp->status3, 50,1);
#else
 gwp->status3 = gtk_statusbar_new();
 gtk_widget_set_margin_top(GTK_WIDGET(gwp->status3), 0);
 gtk_widget_set_margin_bottom(GTK_WIDGET(gwp->status3), 0);
 gtk_widget_set_size_request (gwp->status3, 50,1);
 set_box_color(gtk_statusbar_get_message_area ((GtkStatusbar *)(gwp->status3)),"yellow",color_name[color_scheme_ind][MODEBACK]);
#endif

 
 g_signal_connect(G_OBJECT((box_status1)),"button_press_event", (GCallback) select_file_x ,wp);


 gtk_box_pack_start (sb2, (GtkWidget *)gwp->status2, FALSE, FALSE, 0);

 set_box_background_color(gwp->hstatus,color_name[color_scheme_ind][MODEBACK]);

 gtk_box_pack_start (GTK_BOX (gwp->hstatus), (GtkWidget *)sb2, FALSE, FALSE, 0);
 gtk_box_pack_start (GTK_BOX (gwp->hstatus), (GtkWidget *)box_status1, TRUE, TRUE, 0);
 gtk_box_pack_start (GTK_BOX (gwp->hstatus), (GtkWidget *)gwp->status3, FALSE, FALSE, 0);
 gtk_widget_set_size_request (gwp->status3, 50,1);

 gtk_widget_set_size_request((GtkWidget *)gwp->status2,50,1);

 gtk_box_pack_start (GTK_BOX (gwp->box), (GtkWidget *)gwp->hstatus, FALSE, TRUE, 1);

 gtk_widget_show(gwp->status2);
 gtk_widget_show(gwp->status1);
 gtk_widget_show(gwp->status3);
 gtk_widget_show(gwp->hstatus);

 if(window_list->size>1){
	connect_exit_button(GTK_BOX (gwp->hstatus),(GCallback) cb_close_window,(void *)wp);
 };
#if	NUSE
 g_signal_connect(gwp->draw,"scroll_event",G_CALLBACK(ge_edit_display_scroll),NULL);
#endif
 gtk_widget_show(gwp->draw);
}

/* combine edit display creation */
GWINDP * ge_cedit_new(GtkWidget *parent, WINDP *wp,int ptype)
{
 GWINDP  *new_gwp;
 GtkBox *pbox1=NULL;
 GtkBox *pbox2=NULL;
 GtkBox *pan=NULL;

 BTWE *btep,*btel,*bter; /* parent,left,right */
 gint orientation=0;

 if(ptype==1) orientation=GTK_ORIENTATION_VERTICAL;
 if(ptype==2) orientation=GTK_ORIENTATION_HORIZONTAL;
 new_gwp = ( GWINDP *) malloc(sizeof( GWINDP));

	if(wp!=NULL) wp->gwp = new_gwp;	
	else ERROR("windp wp is NULL!");

	pbox2=(void *)gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	gtk_widget_show(GTK_WIDGET(pbox2));
	gtk_widget_set_size_request((GtkWidget *)pbox2,200,0);

 if(ptype==0) { /* this is for the first window */

	rootbtwe.parent=NULL;
	rootbtwe.left=NULL;
	rootbtwe.right=NULL;
	rootbtwe.type=ptype;
	rootbtwe.pan=NULL;
	rootbtwe.gw=new_gwp;
	rootbtwe.id=btindex++;
	new_gwp->bte=(struct BTWE *)&rootbtwe;
	rootbtwe.pbox=pbox2;

	gtk_box_pack_start(GTK_BOX(parent), (GtkWidget *)pbox2, TRUE,TRUE,0);
	new_gwp_draw(new_gwp,wp,(GtkWidget *)pbox2,ptype);

 } else { /* this is for any additional window */

	btep = curgwp->bte;

	gtk_container_remove((GtkContainer *)btep->pbox,(GtkWidget *)curgwp->box);

	pbox1 = (void *)gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);	// here goes the old draw
	gtk_widget_set_size_request((GtkWidget *)pbox1,200,0);
	gtk_widget_show(GTK_WIDGET(pbox1));

	pan = (void *)gtk_paned_new(orientation);
	gtk_box_pack_start(GTK_BOX(btep->pbox), (GtkWidget *)pan, TRUE,TRUE,0);
//	The following sets what window will be the active one
    gtk_paned_pack1(GTK_PANED(pan),  (GtkWidget *)pbox2, TRUE, TRUE);
    gtk_paned_pack2(GTK_PANED(pan),  (GtkWidget *)pbox1, TRUE, TRUE);

	gtk_paned_set_wide_handle (GTK_PANED(pan),TRUE);
	gtk_widget_show(GTK_WIDGET(pan));

	new_gwp_draw(new_gwp,wp,(GtkWidget *)pbox2,ptype);
	new_gwp_draw(curgwp,cwp,(GtkWidget *)pbox1,curgwp->ptype);

	/* set the btree values */
	btel = (BTWE *)malloc(sizeof(BTWE));
	bter = (BTWE *)malloc(sizeof(BTWE));
	btel->id=btindex++;
	bter->id=btindex++;
	btep->left = btel;
	btep->right = bter;
	btep->gw = NULL;
	btep->pan = (GtkVBox *)pan;
	btel->pan=bter->pan=NULL;
	btel->left=btel->right=bter->left=bter->right=NULL;
	btel->parent=bter->parent=btep;
	btel->gw = curgwp; curgwp->bte = btel;
	bter->gw = new_gwp; new_gwp->bte=bter;
	btel->type=1;
	bter->type=2;
	btel->pbox=pbox1;
	bter->pbox=pbox2;
 };

 return (new_gwp);
}

/* Put a string on a window's statusline at position */
void put_string_statusline(WINDP *wp, char *st, int position)
{
 char msg[512];
 char *bg_color;
 char *fg_color;
 if(cwp->gwp == NULL) return;

 if(wp==cwp) {
	bg_color = color_name[color_scheme_ind][MODEBACK];
	fg_color = color_name[color_scheme_ind][MODEFORE];
 } else {
	bg_color = color_name[color_scheme_ind][MODEBACKI];
	fg_color = color_name[color_scheme_ind][MODEFOREI];
	// clear status3 on all windows except current window
#if	LABEL_STATUS
	// GtkWidget *label = gtk_label_new (NULL);
	// gtk_label_set_markup (GTK_LABEL (label), "<small>Small text</small>");
	// gtk_label_set_mnemonic_widget (GTK_LABEL (label),wp->gwp->status3);
// 	gtk_entry_set_text(wp->gwp->status3,"");
	// gtk_button_set_label(GTK_BUTTON(wp->gwp->status3),"");
	gtk_label_set_label(GTK_LABEL(wp->gwp->status3),"");
#else
	set_box_color(gtk_statusbar_get_message_area ((GtkStatusbar *)(wp->gwp->status3)),fg_color,bg_color);
	gtk_statusbar_pop((GtkStatusbar *)(wp->gwp->status3),1);
	gtk_statusbar_push((GtkStatusbar *)(wp->gwp->status3),1,"");
#endif
 };
 set_box_color(wp->gwp->hstatus,fg_color,bg_color);

 if(position>0) {
#if	LABEL_STATUS
	const char *format = "<span color=\"yellow\">%s  </span>";
	char *markup = g_markup_printf_escaped(format,st);
	gtk_label_set_markup(GTK_LABEL(wp->gwp->status3),markup);
	g_free(markup);
#else
	set_box_color(gtk_statusbar_get_message_area ((GtkStatusbar *)(wp->gwp->status3)),"yellow",bg_color);
	gtk_statusbar_pop((GtkStatusbar *)(wp->gwp->status3),1);
	gtk_statusbar_push((GtkStatusbar *)(wp->gwp->status3),1,st);
#endif
 }
 if(position==0) {
#if	0
	/* just for label reference  */
 	set_box_color(wp->gwp->sb1,fg_color,bg_color);
	gtk_label_set_text((GtkLabel *)wp->gwp->status1,st);
	gtk_label_set_xalign((GtkLabel *)wp->gwp->status1,0.0);
	gtk_widget_set_halign(wp->gwp->status1,GTK_ALIGN_START); 
	gtk_label_set_ellipsize((GtkLabel *)wp->gwp->status1,PANGO_ELLIPSIZE_END);
#endif

//	set_box_color(gtk_statusbar_get_message_area ((GtkStatusbar *)(wp->gwp->status1)),fg_color,bg_color);
	gtk_statusbar_pop((GtkStatusbar *)(wp->gwp->status1),2);
	gtk_statusbar_push((GtkStatusbar *)(wp->gwp->status1),2,st);
 }
 if(position<0) {
	snprintf(msg,512,"<span font=\"12\" background=\"%s\" color=\"yellow\"><b>%s</b></span>",bg_color,st);
	gtk_label_set_markup ((GtkLabel *)((GtkStatusbar *)(wp->gwp->status2)),msg);
 }
}


int newxy(WINDP *wp)
{
 int new_cols;
 int new_rows;
 guint new_width;
 guint new_height;
 GWINDP *gwp;
	if(wp==NULL) {
		MESG("newxy: error null window!");
		return FALSE;
	};
	gwp = wp->gwp;
	if(CLEN>0.1) {
		new_width = dra_width(gwp);
		new_height = dra_height(gwp);
		new_cols = new_width/CLEN+1;
		new_rows = new_height/CHEIGHTI+2;
	if(gwp->width != new_width || gwp->height != new_height)
		{
		free_virtual_window(wp);
		wp->w_ntcols = new_cols;
		wp->w_ntrows = new_rows;
		gwp->width = new_width;
		gwp->height = new_height;
		allocate_virtual_window(wp);
		};
		return(TRUE);

	} else { 
		MESG("newxy: CLEN too small!");
	};
	return(FALSE);
}

/* this is called when we move the slider */
void cb_set_position(GtkAdjustment *adj, GtkWidget *widget)
{
 int prev_line; // previous line
 int gline;
 num lines;

 WINDP *pwp;	// previous window if changed
 GeEditDisplay *wd = GTK_EDIT_DISPLAY(widget);
 // MESG("cb_set_position: slide_flag=%d in_draw=%d in_slide=%d cursor=%d",slide_flag,wd->in_draw,in_slide,cursor_showing);
 if(wd->in_draw) { 
 	// MESG("cb_set_position: in_draw return!");
	return;
 };
 in_slide=1;
// MESG("cb_set_position: id=%d",wd->wp->id);
 if(slide_flag==0) 
  {
	// check_cr(wd->wp,"cb_set_position");
	slide_flag=2;

  	// make this window the current window
	if(cwp != wd->wp) {
		hide_cursor("cb_set_position: cwp!=wp");
		pwp = cwp;
		set_current_window(wd->wp,"cb_set_position");
		status_line(pwp); status_line(cwp);
	};
	// scroll up/down corresponding difference lines.
	prev_line=getcline();

	if(cbfp->b_flag & FSNLIST) lines=cbfp->dir_list_str->size;
#if	TNOTES
	else if(cbfp->b_flag & FSNOTESN) lines=cbfp->dir_list_str->size;
	else if(cbfp->b_flag & FSNOTES) lines=cbfp->b_tag_list->size;
#endif
	else lines=cbfp->lines;

	gline = lines / (gtk_adjustment_get_upper(adj) - gtk_adjustment_get_page_size(adj)) * gtk_adjustment_get_value(adj);
	if(gline>lines-1) gline=lines-1;

	// MESG("cb_set_position: prev_line=%d gline=%d",prev_line,gline);
	wd->wp->w_flag=28;
	if(prev_line==gline) { in_slide=0; slide_flag=0; return;};
	// MESG("cb_set_position: hide cursor %d",cursor_showing);
	hide_cursor("cb_set_position");
	// MESG("cb_set_position: after hiding cursor");
	if(cbfp->b_flag & FSNLIST) {
		cwp->current_note_line=gline;
	} else textpoint_set_lc(cwp->tp_current,gline-1,0);
	upd_column_pos();

	check_cursor_position(wd->wp);
#if	1	// this is not needed for wayland!, it sends a draw event in any case !!!!
//	update_all=true;
	// MESG("cb_set_position: update virtual lines!");
	upd_all_virtual_lines(wd->wp,"cb_set_position 3");
	// MESG("cb_set_position: draw_window!");
	draw_window(0,wd->wp,"cb_set_position");
	// MESG("cb_set_position: move cursor");
	movecursor(window_cursor_line(wd->wp), WGetCol());
	show_cursor("cb_set_position");
#endif
  } else {
//  	MESG("cb_set_position: update slide position!");
  };
  slide_flag=0;
  in_slide=0;
  // MESG("cb_set_position: end!");
}

void font_ok_select(GtkFontChooser *fs, gchar *fontname,gpointer *data)
{
 static char st[1024];
// gchar *fname;
 if(fontname!=NULL) {
	char *st1=st;
	strlcpy(st,fontname,1020);
	while(st1!=0) { 
		if(*st1 == ' ') { *st1=0;break;};
		st1++;
	};
 };
 gtk_widget_destroy((GtkWidget *)fs);
 if(fontname!=NULL) {

 set_font(st);
 update_screen(1);
 show_cursor("font_ok_select");
 };
}

void drv_back_color()
{
// Dummy for gtk3
}

#if	COMMENTS

/*
 vbox1=top -> pp.1
	pbox2.1 -> p.1
	 box2.1
	  draw+adj.1 

#first split
 vbox1	-> ppp.2
	pbox2.1	-> pp.2
		pan.2 {
		  pbox1.2
			box2.1
				draw+adj.1
		  pbox2.2
			box2.2
				draw+adj.2
		}
 vbox1=top->parent
 	pbox2.1 == btwe.0
		pan.2 {
		 pbox1.2 == btwe.0l
		   pan.3 {
		     pbox1.3 == btwe.0ll
			   box2.1
			   	draw+adj.1
			 pbox2.3 == btwe.0lr
			   box2.3
			    draw+adj.3
			}
		pbox2.2 == btwe.0r
			box2.2
				draw+adj.2
	   }
		
*/
#endif



#if	GTK_SEARCH

GtkWidget *search_dialog=NULL;
GtkWidget *search_input,*replace_input;
int search_type=0;
extern char search_pattern[MAXLLEN];
extern char replace_pattern[MAXLLEN];

gboolean on_gs_entry_key_press_event(GtkEntry *widget,
	GdkEventKey     *event, gpointer  user_data);

void search_close(GtkWidget *widget)
{
// possibly get position/dimensions
 gtk_window_close((GtkWindow *)widget);
 search_dialog=NULL;
 search_input=NULL;
 replace_input=NULL;
 entry_mode=KNORMAL;
// MESG("search_closed");
}

gboolean on_search_dialog_key_event(GtkWidget *widget,
	GdkEventKey *event,gpointer user_data)
{
  guint ks;
  ks = event->keyval;
  if(ks==GDK_KEY_Escape) {
  	search_close(widget);
	return TRUE;
  };
  if(ks==GDK_KEY_Return) {
  	on_search_entry_ok_event(widget,NULL,NULL);
  };
  return FALSE;
}	

gboolean
on_search_entry_ok_event (GtkWidget *widget,
	GdkEventKey *event,gpointer user_data)
{
// get button values

// get entry values
	MESG("search type is %d",search_type);
//	gtk_widget_grab_focus(cwp->gwp->draw);
	MESG("search input is [%s]",gtk_entry_get_text((GtkEntry *)search_input));
	strcpy(search_pattern,gtk_entry_get_text((GtkEntry *)search_input));
	MESG("search pattern [%s]",search_pattern);
	if(search_type==2) {
		strcpy(replace_pattern,gtk_entry_get_text((GtkEntry *)replace_input));
	};
	MESG("close window");
	search_close(search_dialog);
	gs_entry_return_flag=1;
	MESG("search ok!");
// go search or replace!
	if(search_type==1) {
		MESG("search for [%s]",search_pattern);
	};
	if(search_type==2) {
		MESG("search for   [%s]",search_pattern);
		MESG("replace with [%s]",replace_pattern);
	};
 return true;
}

gboolean
on_search_entry_cancel_event (GtkWidget *widget,
	GdkEventKey *event,gpointer user_data)
{
	MESG("on_search_entry_cancel_event:");
	search_close(search_dialog);
	gs_entry_return_flag=0;
	return false;
}

gboolean on_search_dialog_close_event (GtkWidget *widget,
	GdkEventKey *event,gpointer user_data)
{
	MESG("on_search_dialog_close_event:");
	
	search_close(widget);
	gs_entry_return_flag=0;
	return false;
}

/* GTK: dialog for search, replace */
int drv_search_dialog(int n)
{
 
 GtkWidget *vb1,*hb1,*hb2,*hb3,*label;
 GtkWidget *toolbar_search;
 int parent_x,parent_y,parent_width,parent_height;
 int dialog_width = 250;
 search_type=n;
 entry_mode=KENTRY;

 gtk_window_get_position((GtkWindow *)parent,&parent_x,&parent_y);
 gtk_window_get_size((GtkWindow *)parent,&parent_width,&parent_height);

 search_dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
// gtk_window_set_title(GTK_WINDOW(search_dialog),"Search");
 gtk_window_set_transient_for(GTK_WINDOW(search_dialog),GTK_WINDOW(parent));
 gtk_window_set_modal(GTK_WINDOW(search_dialog),1);
// gtk_window_set_title (GTK_WINDOW (search_dialog), "Search");
#if	0
	gtk_window_set_decorated(GTK_WINDOW(search_dialog),1);
	/* Use gtk header bar  */
	GtkWidget *dialog_title_bar = gtk_header_bar_new();
	gtk_header_bar_set_title((GtkHeaderBar *)dialog_title_bar,"Search");
	gtk_window_set_titlebar((GtkWindow *)search_dialog,dialog_title_bar);
	gtk_header_bar_set_show_close_button((GtkHeaderBar *)dialog_title_bar,1);
	gtk_widget_show(dialog_title_bar);
#else
	gtk_window_set_decorated(GTK_WINDOW(search_dialog),0);
#endif

 gtk_window_move((GtkWindow *)search_dialog,parent_x+parent_width-dialog_width,parent_y);
 
 if(n==1) {	// search
	gtk_widget_set_size_request (search_dialog, dialog_width, 70);
	gtk_window_set_title((GtkWindow *)search_dialog,"search dialog");
 } else {	// replace
	gtk_widget_set_size_request (search_dialog, dialog_width, 100);
	gtk_window_set_title((GtkWindow *)search_dialog,"replace dialog");
 }
//
 
 gtk_widget_show(search_dialog);

 vb1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
 gtk_widget_show(vb1);
 gtk_container_add (GTK_CONTAINER (search_dialog), vb1);

// search area
 hb1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
 gtk_widget_show(hb1);
 gtk_box_pack_start((GtkBox *)vb1,hb1,FALSE,FALSE,0);
#if	1
 label = gtk_image_new_from_icon_name("edit-find",GTK_ICON_SIZE_MENU);
 gtk_box_pack_start(GTK_BOX(hb1),label, FALSE,FALSE,10);
#else
 label = gtk_label_new("s");
 gtk_widget_set_size_request (label,70,35);
 gtk_label_set_text((GtkLabel *)label,"search "); 
 gtk_box_pack_start(GTK_BOX(hb1),label, FALSE,FALSE,0);
#endif
 gtk_widget_show(label);

 g_signal_connect (GTK_WIDGET (search_dialog), "key_press_event",
                      (GCallback) on_search_dialog_key_event,
                      NULL);
  g_signal_connect (G_OBJECT (search_dialog), "delete_event",
  						(GCallback) on_search_dialog_close_event, NULL);

 search_input = gtk_entry_new();
 gtk_widget_set_size_request (search_input,170,35);
 gtk_box_pack_start(GTK_BOX(hb1),search_input, TRUE,TRUE,0);
 gtk_widget_show(search_input);

 g_signal_connect (GTK_WIDGET (search_input), "key_press_event",
                      (GCallback) on_gs_entry_key_press_event,
                      NULL);

 if(n==2) { // replace area
	hb2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	gtk_widget_show(hb2);
	gtk_box_pack_start(GTK_BOX(vb1),hb2,FALSE,FALSE,0);
#if	1
	label = gtk_image_new_from_icon_name("edit-find-replace",GTK_ICON_SIZE_MENU);
	gtk_box_pack_start(GTK_BOX(hb2),label, FALSE,FALSE,10);
#else
	label = gtk_label_new("s");
	gtk_widget_set_size_request (label,70,35);
	gtk_label_set_text((GtkLabel *)label,"replace "); 
	gtk_box_pack_start(GTK_BOX(hb2),label, FALSE,FALSE,0);
#endif
	gtk_widget_show(label);
	
	replace_input = gtk_entry_new();
	gtk_widget_set_size_request (replace_input,170,35);
	gtk_box_pack_start(GTK_BOX(hb2),replace_input, TRUE,TRUE,0);
	gtk_widget_show(replace_input);
 };

// option area
 hb3 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
 gtk_widget_show(hb3);
 gtk_box_pack_start(GTK_BOX(vb1),hb3, FALSE,TRUE,0);

 if(n==1) {
	toolbar_search = new_toolbar(search_dialog,hb3,search_tbar,0,GTK_ICON_SIZE_SMALL_TOOLBAR,1);
 } else {
	toolbar_search = new_toolbar(search_dialog,hb3,replace_tbar,0,GTK_ICON_SIZE_SMALL_TOOLBAR,1);
 };
 gtk_toolbar_set_icon_size((GtkToolbar *)toolbar_search,GTK_ICON_SIZE_SMALL_TOOLBAR);

 gtk_widget_show(toolbar_search);
 gtk_widget_grab_focus(search_input);
 gtk_widget_set_sensitive(search_input,TRUE);
 return true;
}

#else
int drv_search_dialog(int f)
{
	gtk_widget_show(search_tbar);
	return FALSE;
}
#endif

color_element *current_color_element=NULL; 
GtkWidget *csd=NULL;
int foreground_changed=0;

void color_button_update_color_fg(GtkWidget *color_button, gpointer data)
{
  color_element *ce = (color_element *)data;
  GdkRGBA color;
  char cname[16];
  char text_sample[64];
  int bg_index;
  if(ce->bg>0) bg_index=ce->bg;
  else bg_index = -ce->bg;
  current_color_element=ce;
  foreground_changed=1;
  gtk_color_chooser_get_rgba((GtkColorChooser *)color_button,&color);
  sprintf(cname,"#%02X%02X%02X",(int)(color.red*255),(int)(color.green*255),(int)(color.blue*255));
  color_name[color_scheme_ind][ce->fg]=strdup(cname);
  set_box_color(ce->sample_box,
  	color_name[color_scheme_ind][ce->fg],
  	color_name[color_scheme_ind][bg_index]);
  sprintf(text_sample,"changed fg: bg=%s fg=%s",
   color_name[color_scheme_ind][bg_index],color_name[color_scheme_ind][ce->fg]);
//  MESG("update_color_fg: bg_i=%d bg=[%s] fg=[%s]", ce->bg,color_name[color_scheme_ind][bg_index],color_name[color_scheme_ind][ce->fg]);
  gtk_statusbar_pop((GtkStatusbar *)ce->sample_ctext,1);
  gtk_statusbar_push((GtkStatusbar *)ce->sample_ctext,1,text_sample);
	set_box_color(gtk_statusbar_get_message_area ((GtkStatusbar *)ce->sample_ctext),
  	color_name[color_scheme_ind][ce->fg],
  	color_name[color_scheme_ind][bg_index]);

  colors[color_scheme_ind][ce->fg]->red=color.red;
  colors[color_scheme_ind][ce->fg]->green=color.green;
  colors[color_scheme_ind][ce->fg]->blue=color.blue;
  update_screen(1);
}

void color_button_update_color_bg(GtkWidget *color_button, gpointer data)
{
  color_element *ce = (color_element *)data;
  GdkRGBA color;
  char cname[16];
  char text_sample[64];

  current_color_element=ce;
  foreground_changed=0;
  gtk_color_chooser_get_rgba((GtkColorChooser *)color_button,&color);
  sprintf(cname,"#%02X%02X%02X",(int)(color.red*255),(int)(color.green*255),(int)(color.blue*255));
  color_name[color_scheme_ind][ce->bg]=strdup(cname);
  sprintf(text_sample,"changed bg: bg=%s fg=%s",
   color_name[color_scheme_ind][ce->bg],color_name[color_scheme_ind][ce->fg]);

//  MESG("update_color_bg: bg=[%s] fg=[%s]", color_name[color_scheme_ind][ce->bg],color_name[color_scheme_ind][ce->fg]);

  set_box_color(ce->sample_box,
  	color_name[color_scheme_ind][ce->fg],
  	color_name[color_scheme_ind][ce->bg]);
  gtk_statusbar_pop((GtkStatusbar *)ce->sample_ctext,1);
  gtk_statusbar_push((GtkStatusbar *)ce->sample_ctext,1,text_sample);
	set_box_color(gtk_statusbar_get_message_area ((GtkStatusbar *)ce->sample_ctext),
  	color_name[color_scheme_ind][ce->fg],
  	color_name[color_scheme_ind][ce->bg]);

  colors[color_scheme_ind][ce->bg]->red=color.red;
  colors[color_scheme_ind][ce->bg]->green=color.green;
  colors[color_scheme_ind][ce->bg]->blue=color.blue;
  update_screen(1);
}

/* show color selection and set background color */
void color_close_select(GtkWidget *wd, gpointer *data)
{
	gtk_widget_destroy(wd);
	csd=NULL;
}

// show current colors
void print_colors()
{
 int i;
 GdkRGBA *color;
 MESG("color scheme %d",color_scheme_ind);
 for(i=0;i<CTYPE_ROWS;i++){
  int fr,fg,fb;
  int br,bg,bb;
  int bgindex;
  color = (GdkRGBA *)colors[color_scheme_ind][ctype[i].fg];
  fr=(int)(color->red*255);
  fg=(int)(color->green*255);
  fb=(int)(color->blue*255);
  bgindex=ctype[i].bg>0?ctype[i].bg:-ctype[i].bg;
  color = (GdkRGBA *)colors[color_scheme_ind][bgindex];
  br=(int)(color->red*255);
  bg=(int)(color->green*255);
  bb=(int)(color->blue*255);
  MESG("color %d f=(%2X %2X %2x) b=(%2x %2x %2x)",i,fr,fg,fb,br,bg,bb);
 };
}


void button_color_close(GtkWidget *wd, gpointer *data)
{
 GtkWidget *to_close=(GtkWidget *)data;
 gtk_widget_hide(to_close);
}

void colors_win_destroy_cb(GtkWidget *wd, gpointer *data)
{
// do not delete, just hide it
 gtk_widget_hide(wd);
}

void button_color_apply(GtkWidget *wd, gpointer *data)
{
// MESG("button_color_apply:");
 print_colors();
}

void put_to_table(GtkWidget *wdg, GtkWidget *table, int ypos, int xpos, int align)
{
  gtk_grid_attach ((GtkGrid *) table, wdg, xpos, ypos,1,1);
//  gtk_grid_set_row_homogeneous ((GtkGrid *) table,1);
  // this is for labels only
  if(align) { 
	/* set padding with pango attributes in gtk3!!  */
  };
	gtk_widget_set_margin_top(wdg,0);
	gtk_widget_set_margin_bottom(wdg,0);
	gtk_widget_set_margin_start(wdg,2);
	gtk_widget_set_margin_end(wdg,2);

  gtk_widget_set_size_request (wdg,5,5);
  gtk_widget_show(wdg);
}


GtkWidget * griv_table_new(int rows, int cols, int border)
{
 GtkWidget *table;
  table = gtk_grid_new ();
  gtk_widget_show(table);
  gtk_container_set_border_width (GTK_CONTAINER (table), border);
  return table;
}

#define TNEW	1

void show_color_sample(GtkWidget *table,int ypos,char *text)
{
 GtkWidget *ctext=NULL;
 GtkWidget *cbox=NULL;
 int i=ypos-1;
 int fgindex,bgindex;
 char msg[100];
#if	!TNEW
 int is_new=0;
#endif
	if(ctype[i].bg>0) {
		bgindex=ctype[i].bg;
	} else {
		bgindex=-ctype[i].bg;
	};
	fgindex=ctype[i].fg;

#if	TNEW
 	if(ctype[i].sample_ctext != NULL) {
		gtk_widget_destroy(ctype[i].sample_ctext);
		gtk_widget_destroy(ctype[i].sample_box);
	};
#else
 	if(ctype[i].sample_ctext == NULL) {
		ctype[i].sample_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
		ctype[i].sample_ctext = gtk_statusbar_new ();
		gtk_box_pack_start(GTK_BOX(ctype[i].sample_box),ctype[i].sample_ctext,TRUE,TRUE,2);
		is_new=true;
	};
#endif
#if	TNEW
	ctype[i].sample_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_margin_top(GTK_WIDGET(ctype[i].sample_box), 0);
	gtk_widget_set_margin_bottom(GTK_WIDGET(ctype[i].sample_box), 0);

	ctype[i].sample_ctext = gtk_statusbar_new ();

	gtk_widget_set_margin_top(GTK_WIDGET(ctype[i].sample_ctext), 0);
	gtk_widget_set_margin_bottom(GTK_WIDGET(ctype[i].sample_ctext), 0);
#endif
	ctext=ctype[i].sample_ctext;
	cbox=ctype[i].sample_box;
//	set_box_color(gtk_statusbar_get_message_area ((GtkStatusbar *)(ctext)),"white","black");

	gtk_widget_show(ctext);

	gtk_widget_set_size_request(ctext,300,5);
#if	TNEW
	gtk_box_pack_start(GTK_BOX(cbox),ctext,TRUE,TRUE,2);
#endif
	snprintf(msg,100,"%s fg=%s bg=%s",text,color_name[color_scheme_ind][fgindex],color_name[color_scheme_ind][bgindex]);
	gtk_statusbar_pop((GtkStatusbar *)ctext,1);
	gtk_statusbar_push((GtkStatusbar *)ctext,1,msg);

	set_box_color(cbox,
		color_name[color_scheme_ind][fgindex],
		color_name[color_scheme_ind][bgindex]);
	set_box_color(gtk_statusbar_get_message_area ((GtkStatusbar *)ctext),
		color_name[color_scheme_ind][fgindex],
		color_name[color_scheme_ind][bgindex]);

//	MESG("	- %d bg=[%s] fg=[%s]",ypos,
//		color_name[color_scheme_ind][fgindex],
//		color_name[color_scheme_ind][bgindex]);
#if	TNEW
	put_to_table(cbox,table,ypos,4,0);
#else
	if(is_new)	put_to_table(cbox,table,ypos,4,0);
#endif
}

// create/show color dialog
int set_color(int n)
{
 GtkWidget *vbox;
 GtkWidget *hbox;
 GtkWidget *hbox1;
  GtkWidget *scheme_label;
  GtkWidget *scheme_names_button;
 GtkWidget *hbox2;
  GtkWidget *sbutton;
//  GtkWidget *apply_button;
  GtkWidget *close_button;
  char label_txt[100];
 int bsize=10;

 int i;
 char wtitle[32];
 
 if(colors_win!=NULL) {
 	gtk_widget_show(colors_win);
	return(1);
 }; 
 snprintf(wtitle,32,"set color %d",n);
 changed_color=n;

 colors_win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
 gtk_window_set_title(GTK_WINDOW(colors_win),"color");
 gtk_window_set_transient_for(GTK_WINDOW(colors_win),GTK_WINDOW(parent));
 gtk_widget_show(colors_win);

  GtkWidget *label,*fgb,*bgb;
  int ypos;
/*
gtk_box_pack_start(GTK_BOX(vbox), scwin, TRUE, TRUE, 1);
*/
#if	0
  GtkWidget *scwin = gtk_scrolled_window_new(NULL, NULL);
  gtk_container_add(GTK_CONTAINER(scwin), colors_win);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scwin), GTK_POLICY_AUTOMATIC,
                               GTK_POLICY_ALWAYS);
#endif
  table1 = griv_table_new (CTYPE_ROWS, 4,4);

  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show(vbox);
  hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);gtk_widget_show(hbox);

  hbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);gtk_widget_show(hbox1);
  scheme_label=gtk_label_new("select color scheme");gtk_widget_show(scheme_label);

	gtk_widget_set_halign (scheme_label, GTK_ALIGN_START);
	snprintf(label_txt,100,"<span font=\"14\"  >select color scheme</span>");
	gtk_label_set_markup ((GtkLabel *)scheme_label,label_txt);

  scheme_names_button=new_combo_box(scheme_names,color_scheme_ind);
  g_signal_connect(G_OBJECT(scheme_names_button),"changed", (GCallback) scheme_names_button_change ,NULL);

  hbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);gtk_widget_show(hbox2);
  sbutton = gtk_button_new_with_label("save");
  gtk_widget_show(sbutton);
//  apply_button = gtk_button_new_with_label("apply");
//  gtk_widget_show(apply_button);
  close_button = gtk_button_new_with_label("close");
  gtk_widget_show(close_button);

  gtk_container_add (GTK_CONTAINER (colors_win), vbox);
  gtk_container_set_border_width (GTK_CONTAINER (colors_win), 5);

  gtk_box_pack_start(GTK_BOX(vbox),hbox1, FALSE,TRUE,0);
#if	1
  GtkWidget *scwin = gtk_scrolled_window_new(NULL, NULL);
  gtk_container_add(GTK_CONTAINER(scwin), table1);
  gtk_widget_set_size_request(scwin,570,600);

  gtk_widget_show(scwin);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scwin), GTK_POLICY_AUTOMATIC,
                               GTK_POLICY_ALWAYS);
  gtk_box_pack_start(GTK_BOX(vbox), scwin, TRUE, TRUE, 1);
#else
  gtk_box_pack_start(GTK_BOX(vbox),table1, FALSE,TRUE,0);
#endif

	gtk_widget_set_margin_top(GTK_WIDGET(table1), 0);
	gtk_widget_set_margin_bottom(GTK_WIDGET(table1), 0);

  gtk_box_pack_start(GTK_BOX(hbox1),scheme_label, TRUE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(hbox1),scheme_names_button, TRUE,FALSE,0);

  gtk_box_pack_start(GTK_BOX(vbox),hbox2, FALSE,TRUE,0);

  gtk_box_pack_start(GTK_BOX(hbox2),sbutton, TRUE,FALSE,0);
//  gtk_box_pack_start(GTK_BOX(hbox2),apply_button, TRUE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(hbox2),close_button, TRUE,FALSE,0);	/*   */

  g_signal_connect(G_OBJECT(sbutton),"clicked", (GCallback) button_color_save ,NULL);
//  g_signal_connect(G_OBJECT(apply_button),"clicked", (GCallback) button_color_apply ,NULL);
  g_signal_connect(G_OBJECT(close_button),"clicked", (GCallback) button_color_close ,(void *)colors_win);
  g_signal_connect(G_OBJECT(colors_win),"delete_event", (GCallback) colors_win_destroy_cb ,(void *)colors_win);
//  MESG("set color, create boxes %d rows",CTYPE_ROWS);
  for(i=0;i<CTYPE_ROWS;i++) {
  	char color_title[100];
	ypos=i+1;  
	ctype[i].button_fg=NULL;
	ctype[i].button_bg=NULL;
	// show label left aligned, with specific font size
	label = gtk_label_new(ctype[i].label);
	gtk_widget_set_halign (label, GTK_ALIGN_START);
//	gtk_widget_set_margin_top(GTK_WIDGET(label), 0);
//	gtk_widget_set_margin_bottom(GTK_WIDGET(label), 0);


	snprintf(label_txt,100,"<span font=\"10\">%s</span>",ctype[i].label);
	gtk_label_set_markup ((GtkLabel *)label,label_txt);
	gtk_widget_set_size_request(label,150,bsize);
	put_to_table(label,table1,ypos,1,1);

	// show color buttons foreground,background if needed
	fgb = gtk_color_button_new_with_rgba (colors[color_scheme_ind][ctype[i].fg]);

	ctype[i].button_fg=fgb;
	gtk_container_set_border_width((GtkContainer *)fgb,0);

	snprintf(color_title,100,"Foreground %s",ctype[i].label);
	gtk_color_button_set_title((GtkColorButton *)fgb,color_title);
	put_to_table(fgb,table1,ypos,2,0);
//	gtk_widget_set_tooltip_text (GTK_WIDGET(fgb),color_title);
	g_signal_connect(G_OBJECT(fgb),	"color-set", (GCallback) color_button_update_color_fg ,(gpointer)&ctype[i]);
	if(ctype[i].bg>0) {
		bgb = gtk_color_button_new_with_rgba (colors[color_scheme_ind][ctype[i].bg]);
		gtk_widget_set_size_request(GTK_WIDGET(bgb), bsize,bsize);
		ctype[i].button_bg=bgb;
		snprintf(color_title,100,"Background %s",ctype[i].label);
//		gtk_color_button_set_title((GtkColorButton *)bgb,color_title);
		put_to_table(bgb,table1,ypos,3,0);
		g_signal_connect(G_OBJECT(bgb),	"color-set", (GCallback) color_button_update_color_bg ,(gpointer)&ctype[i]);
	} else {
//		bgindex=-ctype[i].bg;
	};

	// show color sample
	show_color_sample(table1,ypos,"text");
  };

  g_signal_connect (G_OBJECT (colors_win), "key_press_event",
                      (GCallback) on_wlist_key_press,
                      NULL);
 return(0); 
}

void drv_show_menu_popup(GtkWidget *popup_menu)
{
	MESG("drv_show_menu_popup:");
	gtk_menu_popup_at_pointer(GTK_MENU(popup_menu),NULL);
}

// approximately find length of a character. this is almost black magic..
int get_pango_length(char *st)
{
 int width,height;
 if(st[0]<0x80) return CLEN;	/* suppose that all asci have CLEN display size  */
 if(!check_cr(cwp,"gl")) {
 	// MESG("get_pango_length: check_cr problem!");
	return 0;
 };
	GeEditDisplay *wd = GTK_EDIT_DISPLAY(cwp->gwp->draw);
	if(wd->layout==NULL) {
		// MESG("layout is null!!!");
		return 0;
	};
	pango_layout_set_font_description (wd->layout, wd->ge_font_desc);
	pango_layout_set_text (wd->layout, st, -1);
	pango_layout_get_size (wd->layout, &width, &height);
	// if(st[0]>0x80)
// 	MESG("pango_len:[%2X %2X %2X %2x] w=%d CLEN=%f",st[0],st[1],st[2],st[3],width/PANGO_SCALE,CLEN);
	return width/PANGO_SCALE;
}

// Different for each platform, screen driver
int get_utf_length(utfchar *utf_char_str)
{
 int b0;
 if(clen_error) { return 1;};
 int plen=get_pango_length((char *)utf_char_str->uval);
 if(plen!=0) {
	// MESG("utf_length from pango:[%s] %d",(char *)utf_char_str->uval,plen);
 	if(plen<=(int)CLEN) return 1;
	else return 2;
 };

 // Use manual evaluation of display length !
 b0=utf_char_str->uval[0];
 if(b0==0) return 0;
 if(b0<128) return 1;
 // MESG("c=%X %X",b0,utf_char_str->uval[1]);
#if	DARWIN | 1
 // accents do not take space in mac 
 if((b0==0xCC || b0==0xCD) && utf_char_str->uval[1]< 0xB0) return 0;
#else
 // accents do take space in WSL exept when converted to composed.
 if((b0==0xCC || b0==0xCD) && utf_char_str->uval[1]< 0xB0) return 1;
#endif
 if(b0<0xE1) return 1;
 if(b0==0xE2) {
	int b1=utf_char_str->uval[1];
	if(b1 == 0x80) {
		int b2=utf_char_str->uval[2];
		if(b2==0x80) return 1; 	/* 1/4 en  */
		if(b2==0x8B) return 0;	/* zero space  */
		if(b2==0x8C) return 0;	/* zero space  */
		if(b2==0x8D) return 0;	/* zero space  */
		return 1;
	};
	if(b1==0x81 || b1==0x82) return 1; 	/* diacriticals, subscripts, currency symbols  */
	if(b1 == 0x83) {
		int b2=utf_char_str->uval[2];
		if(b2 > 0x8F && b2< 0xb1) return 0;	/* combining characters */
		return 1;
	};
	if(b1 == 0x92) return 2; // circled
	if(b1 == 0x93) return 2;
	if(b1 == 0xB0) {
		int b2=utf_char_str->uval[2];
		if(b2==0x80) return 1;
		if(b2==0x85) return 1;
		if(b2==0x8D) return 1;
		if(b2==0x91) return 1;
		if(b2==0x93) return 1;
		if(b2==0x98) return 1;
		if(b2==0x99) return 1;
		if(b2==0xA0) return 1;
		if(b2>=0xB0 && b2<0xBF) return 1;
		return 2;
	};
	if(b1 == 0xB1) {
		int b2=utf_char_str->uval[2];
		if(b2==0x86) return 2;
		if(b2==0x92) return 2;
		if(b2==0xA7) return 2;
		if(b2==0xA9) return 2;
		if(b2==0xB2) return 2;
		if(b2==0xB3) return 2;

		if(b2<=0xBF) return 1;
		return 2;
	}
	if(b1 > 0x83 && b1< 0xBA) {
		return 1; 	/* should be 2 in most cases ...  */
	};
 	return 2;
 };
 if(b0==0xEA) {
 	int b1=utf_char_str->uval[1];
	if(b1==0x99) {
		int b2=utf_char_str->uval[2];
		if(b2==0x88) return 2;
		if(b2<0x8C) return 1;
		if(b2==0x8F) return 1;
		if(b2==0x97) return 1;
		if(b2==0x99) return 1;
		if(b2==0x9F) return 1;
		if(b2==0xA1) return 1;
		if(b2==0xA3) return 1;
		if(b2==0xA5) return 1;
		if(b2==0xA9) return 1;
		if(b2==0xAB) return 1;
		return 2;
	};
	if(b1==0x9A) {
		int b2=utf_char_str->uval[2];
		if(b2<0x84) return 1;
		if(b2==0x89) return 1;
		if(b2==0x8B) return 1;
		if(b2==0x8C) return 1;
		if(b2==0x8D) return 1;
		if(b2==0x8F) return 1;
		if(b2==0x90) return 1;
		if(b2==0x91) return 1;
		if(b2==0x93) return 1;
		if(b2==0x95) return 1;
		return 2;
	};
	if(b1==0x9C) {
		int b2=utf_char_str->uval[2];
		if(b2==0xA8) return 2;
		if(b2==0xA9) return 2;
		if(b2<0xB2) return 1;
		if(b2==0xB9) return 1;
		if(b2==0xBB) return 1;
		if(b2==0xBD) return 1;
		if(b2==0xBF) return 1;
		return 2;
	}	
	if(b1==0x9D) {
		int b2=utf_char_str->uval[2];
		if(b2==0x88) return 1;
		if(b2==0x8F) return 2;
		if(b2==0x90) return 1;
		if(b2==0x9A) return 1;
		if(b2==0xA4) return 1;
		if(b2==0xA6) return 1;
		if(b2==0xAE) return 1;
		if(b2==0xBA) return 1;
		if(b2==0xBB) return 1;
		if(b2==0xBC) return 1;
		if(b2==0xBE) return 1;
		if(b2==0xAE) return 1;
		if((b2%2)==1) return 1;
		return 2;
	};
	if(b1==0x9E) {
		int b2=utf_char_str->uval[2];
		// if(b2==0x81) return 2;
		if(b2==0x82) return 2;
		if(b2==0xA0) return 2;
		if(b2==0xA2) return 2;
		if(b2==0xA4) return 2;
		if(b2==0xA6) return 2;
		return 1;
	};
	return 2;
 };
 if(b0>=0xEA && b0<0xED) return 2;	/* Korean  */
 if(b0==0xEF) {
	int b1=utf_char_str->uval[1];
	if(b1==0xBC ) {
		return 2;	/* comma with added space etc  */
	};
	if(b1==0xBD) {
		int b2=utf_char_str->uval[2];
		if(b2>=0x80 && b2<0xA1) return 2;
	};
 	return 1;
 };
 if(b0==0xF0) {
	int b1=utf_char_str->uval[1];
	if(b1==0x90) {
		int b2=utf_char_str->uval[2];
		if(b2==0x90) {
			int b3=utf_char_str->uval[3];
			if(b3==0x80) return 1;
			if(b3==0xA8) return 1;
		};
		if(b2==0x91) {
			int b3=utf_char_str->uval[3];
			if(b3==0x8D) return 1;
			if(b3==0x8E) return 1;
			if(b3==0x8F) return 1;
		};
		return 2;
	};
	if(b1==0x9F) {
// 		int b2=utf_char_str->uval[2];
		// if (b2==0x84) return 1;
		return 2;
	};
	if(b1==0x9D) {
		int b2=utf_char_str->uval[2];
		if(b2==0x90) {
			int b3=utf_char_str->uval[3];
			if(b3==0x85) return 1;
			if(b3==0x88) return 1;
			if(b3==0x89) return 1;
			if(b3==0x8F) return 1;
			if(b3==0x92) return 1;

			if(b3< 0x99) return 2;
			if(b3==0xA6) return 2;
			if(b3==0xB0) return 2;
			if(b3==0xB6) return 2;
			if(b3==0xB7) return 2;
			if(b3==0xB8) return 2;
			if(b3==0xBA) return 2;
		return 1;
		};
		if(b2==0x91) {
			int b3=utf_char_str->uval[3];
			if(b3==0x80) return 2;
			if(b3==0x81) return 2;
			if(b3==0x82) return 2;
			if(b3==0x84) return 2;
			// if(b3==0x87) return 2;
			if(b3==0x88) return 2;
			// if(b3==0x89) return 2;
			if(b3==0x8A) return 2;
			// if(b3==0x8B) return 2;
			// if(b3==0x8C) return 2;
			if(b3==0xAB) return 2;
			if(b3==0xAC) return 2;
			if(b3==0xAD) return 2;
			if(b3==0xAE) return 2;
			if(b3==0xAF) return 2;
			if(b3==0xB2) return 2;
			if(b3==0xB4) return 2;
			if(b3==0xB5) return 2;
			if(b3==0xB6) return 2;
			// if(b3==0xB7) return 2;
			if(b3==0xB8) return 2;
			// if(b3==0xBB) return 2;
			if(b3==0xBC) return 2;
			if(b3==0xBD) return 2;
			if(b3==0xBE) return 2;
			if(b3==0xBF) return 2;
			return 1;
		};
		if(b2==0x9A) {
			int b3=utf_char_str->uval[3];
			if(b3<=0xA3) return 1;

			if(b3==0xAB) return 2;
			if(b3==0xAE) return 2;
			if(b3==0xAF) return 2;
			if(b3==0xB1) return 2;
			if(b3==0xB2) return 2;
			if(b3==0xB3) return 2;
			if(b3==0xB4) return 2;
			if(b3==0xB5) return 2;
			if(b3==0xB6) return 2;
			if(b3==0xB7) return 2;
			if(b3==0xBB) return 2;
			if(b3==0xBC) return 2;
			if(b3==0xBD) return 2;
			if(b3==0xBE) return 2;
			if(b3==0xBF) return 2;
			return 1;
		};
		if(b2==0x9B) {
			int b3=utf_char_str->uval[3];
			if(b3==0x80) return 2;
			if(b3==0x97) return 2;
			if(b3==0x99) return 2;
			if(b3==0x9A) return 2;
			if(b3==0xA8) return 2;
			if(b3==0xA9) return 2;
			if(b3==0xAB) return 2;
			// if(b3==0xAC) return 2;
			if(b3==0xAD) return 2;
			if(b3==0xAE) return 2;
			if(b3==0xAF) return 2;
			if(b3==0xB0) return 2;
			if(b3==0xB1) return 2;
			// if(b3==0xB2) return 2;
			if(b3==0xB4) return 2;
			// if(b3==0xB5) return 2;
			if(b3==0xB6) return 2;
			if(b3==0xB7) return 2;
			// if(b3==0xB8) return 2;
			if(b3==0xB9) return 2;
			if(b3==0xBA) return 2;
			return 1;
		};
		if(b2==0x9C) {
			int b3=utf_char_str->uval[3];
			// if(b3==0x91) return 2;
			if(b3==0x93) return 2;
			if(b3==0x94) return 2;

			if(b3==0xA2) return 2;
			if(b3==0xA3) return 2;
			if(b3==0xA7) return 2;
			if(b3==0xA8) return 2;
			// if(b3==0xA9) return 2;
			if(b3==0xAA) return 2;
			if(b3==0xAB) return 2;
			// if(b3==0xAC) return 2;
			// if(b3==0xAF) return 2;
			if(b3==0xAE) return 2;
			if(b3==0xB0) return 2;
			if(b3==0xB1) return 2;
			if(b3==0xB2) return 2;
			if(b3==0xB3) return 2;
			if(b3==0xB4) return 2;
			if(b3==0xB2) return 2;
			return 1;
		};
		if(b2==0x9D) {
			int b3=utf_char_str->uval[3];
			// if(b3==0x8B) return 2;
			if(b3==0x8D) return 2;
			if(b3==0x8E) return 2;
			if(b3==0x9C) return 2;
			if(b3==0x9D) return 2;
			if(b3==0xA4) return 2;
			if(b3==0xAB) return 2;
			if(b3==0xAD) return 2;
			if(b3==0xAE) return 2;
			return 1;
		};
		if(b2==0x9E) {
			int b3=utf_char_str->uval[3];
			if(b3==0x85) return 2;
			if(b3==0x88) return 2;
			if(b3==0x96) return 2;
			if(b3==0x97) return 2;
			if(b3==0x9B) return 2;
			if(b3==0x9E) return 2;
			if(b3==0x9F) return 2;
			if(b3==0xA4) return 2;
			if(b3==0xA5) return 2;
			if(b3==0xA6) return 2;
			if(b3==0xA7) return 2;
			if(b3==0xA8) return 2;
			if(b3==0xBF) return 2;
			if(b3==0xA4) return 2;
			if(b3==0xA4) return 2;
			return 1;
		}
		if(b2==0x9F) {
			int b3=utf_char_str->uval[3];
			if(b3==0x81) return 2;
			if(b3==0x82) return 2;
		};
		return 1;
	}
 	return 2;
 };
 if(b0==0xF1 || b0==0xF2) return 1;	/* do not exist in apple terminal  */
 if(b0==0xF3) return 1;
 if(b0==0xE1) {
	int b1=utf_char_str->uval[1];
	return 1;
 	if(b1==0) return 1;
	if(b1>0x83) return 2;
	return 1;
 };
 if(b0==0xE3) {
 	int b1=utf_char_str->uval[1];
	if(b1==0x82) {
		int b2=utf_char_str->uval[2];
		if(b2==0x99) return 0;/* dakuten */
		if(b2==0x9A) return 0;/* handakuten */
	}
 }
 return 2;
}

#include "gtk_common.c"
#include "menu_common.c"

/* -- */
