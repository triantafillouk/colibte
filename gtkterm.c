/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.
*/
/* gtk2 screen driver */

#include "xe.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-prototypes"

#pragma GCC diagnostic ignored "-Wno-deprecated-declarations"
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <glib.h>
#include <gtk/gtkselection.h>

#include "geditdisplay.h"
#include "gtk_support.h"
#include "gtkterm.h"
#include "xkeys.h"
#include "xthemes.h"
#include <X11/Xlib.h>
#include "menus.h"
#include "icon.h"

#define	SHOW_CLINE	1
int addutfvchar1(char *str, vchar *vc, int pos,FILEBUF *w_fp);

// in gtk_common function definitions
GtkFileChooser *show_file_chooser(char *title, int action,char *name);
void cb_close_window(GtkWidget *w, WINDP * wp);
GtkWidget *new_combo_box(char **list,int active);
void gtk_entry_commit_cb(GtkIMContext *context, const gchar  *str, gpointer data);
void button_color_save(GtkWidget *wd, gpointer *data);
int put_wtext1(WINDP *wp, int row,int pos_x,int col);
void set_cursor(int val,char *from);
char **get_scheme_names();
void upd_all_wrap_lines(WINDP *wp,char *from);

/* local variables */
int color_scheme_ind;
COLOR_SCHEME *current_scheme=NULL;

int compact1 = 1;
MENUS *start_menu = &m_topn;

int drv_type=DRIVER_GTK2;
extern short  *kbdsptr;

int drv_colors=256;

int changed_color=0;
GIConv cd;

GtkWidget * create_top_menu(MENUS *menu_des , GtkWidget *parent,GtkAccelGroup  *menu_accels);
extern GtkWidget *popup_select_on, *popup_select_off, *popup_boxed_region, *popup_sort_menu,*popup_tag_menu,*popup_notes_menu;
extern FILEBUF *cbfp;
extern int clen_error;

void end_draw(GeEditDisplay *wd,char *message);

char **mono_fonts_list;
int mono_fonts_number;

GtkStyle *st1a,*st1i,*st3a,*st3i;
static GtkIMContext *im_context;
GtkWidget *table1,*colors_win=NULL;

extern int utflen;
extern char utfokey[10];

void show_color_sample(GtkWidget *table,int ypos,char *text);
void color_ok_select(GtkWidget *wd, gpointer *data);
void color_close_select(GtkWidget *wd, gpointer *data);
void put_to_table(GtkWidget *wdg, GtkWidget *table, int ypos, int xpos, int align);
void prepare_converter(int local_lang);

GtkSelectionData *seldata0=NULL;
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
void update_full(int i);
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

void file_ok_select(GtkFileSelection *fs, gpointer *data);
void font_ok_select(GtkFontSelectionDialog *fs, gpointer *data);
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
#if	XPLOT
extern void plot_flush();
#endif
extern alist *window_list;
void init_color();
int set_font(char *);
void sinsert_nl(char *dat,int nbytes);

WINDP * make_split(WINDP *oldwindow);
void expose_line(int i, WINDP *wp);
int set_fontindex(num);
void cb_tools(GtkWidget *w, T_ELEMENT *telem );

void allocate_virtual_window(WINDP *wp);

GtkWidget *ge_edit_display_new();

int slide_flag;
int in_slide=0;

BTWE rootbtwe;
int btindex=0;

GtkWidget *list1;
int index_value=0;

Display *dis0;
Screen *screen_ptr;
int screen_num;

GdkGC *fgc, *bgc ;

int cursor_showing=0;
int px,py,ppx,ppy;
int cposx=0;
int cposy=0;
int cpposx=-1;
int cpposy=-1;	/* in characters  */
float CLEN,CHEIGHT;
float SLBASE,CFONTBASEI;
int CLENI,CHEIGHTI;


int cursor_width;

int dra_width(GWINDP *);
int dra_height(GWINDP *);

int font_index=1;	// Set here the default font_index
int font_weight=CAIRO_FONT_WEIGHT_NORMAL;

char *current_font_name=NULL;

int ncolors;

GdkColor  *current_colors[COLOR_TYPES];

fcolor_t ccolorb,ccolorf;
fcolor_t ccolorb_default;

GdkColormap *cmap;
GtkWidget *parent;

GtkWidget *gs_label;
GtkWidget *toolbar1,*toolbar2;
GtkWidget *gs_entry;
char gs_entry_txt[512];

GtkWidget *entry_ok, *entry_cancel;
GtkWidget *statusbar1;
int gs_entry_return_flag;
GtkWidget *search_dialog;
GtkWidget *color_dialog;

extern int startup_exe;


GtkWidget*
create_parent (void)
{
  GtkWidget *parent;
  GtkWidget *vbox1;
  GtkWidget *hbox2;

  GtkAccelGroup *accel_group;
  int parent_width=800;
  int parent_height=500;
  int parent_x=10;
  int parent_y=10;
 /* Create the menubar in vbox */
  accel_group = gtk_accel_group_new ();

  parent = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  // MESG("create_parent: reset_position=$d",bt_dval("reset_position"));
  if(bt_dval("reset_position")==0) {
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
#if	DARWIN
  if(parent_y>=22) parent_y -= 22;
#endif
//  printf("parent width=%d height=%d\n",parent_width,parent_height);
  gtk_window_set_default_size(GTK_WINDOW(parent),parent_width,parent_height);
  gtk_window_set_policy(GTK_WINDOW(parent), TRUE, TRUE,FALSE);
  gtk_widget_set_uposition(parent,parent_x,parent_y);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (parent), vbox1);

  create_main_menu(vbox1, parent, &m_topn,accel_group);

  popup_select_on = create_top_menu( &m_select_on, parent, accel_group);
  popup_select_off = create_top_menu( &m_select_off, parent, accel_group);
  popup_boxed_region = create_top_menu( &m_boxed_region, parent, accel_group);
  popup_sort_menu = create_top_menu( &m_sort, parent, accel_group);
  popup_tag_menu = create_top_menu( &m_notes_tag, parent, accel_group);
  popup_notes_menu = create_top_menu( &m_note_popup, parent, accel_group);

	hbox2 = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox2);
	if(compact1) {
		gtk_box_pack_start (GTK_BOX (vbox1), hbox2, FALSE, FALSE, 0);
// 		toolbar1=new_toolbar(parent,hbox2,main_toolbar,0,GTK_ICON_SIZE_BUTTON,0);
		toolbar1=new_toolbar(parent,hbox2,main_toolbar,0,GTK_ICON_SIZE_LARGE_TOOLBAR,0);
		gtk_toolbar_set_show_arrow(GTK_TOOLBAR(toolbar1),FALSE);
	} else {
		toolbar1=new_toolbar(parent,vbox1,main_toolbar,0,GTK_ICON_SIZE_LARGE_TOOLBAR,0);
		gtk_toolbar_set_show_arrow(GTK_TOOLBAR(toolbar1),TRUE);
	};

	/* create the drawing window with scrollbar and statusline */
  if(cwp==NULL) { /* create a new one */
  	cwp = make_split(NULL);
  }; 
  
  curgwp = ge_cedit_new(vbox1, cwp,0);

  gs_label = gtk_label_new("");
  gtk_label_set_text((GtkLabel *)gs_label,"");
  gtk_widget_show(gs_label);
  gtk_box_pack_start (GTK_BOX (hbox2), gs_label, FALSE, TRUE, 0);

  gs_entry = gtk_entry_new();
  gtk_entry_set_editable(GTK_ENTRY(gs_entry), TRUE);
  gtk_entry_set_activates_default(GTK_ENTRY(gs_entry), TRUE);
  gtk_entry_set_has_frame(GTK_ENTRY(gs_entry), FALSE);

  gtk_box_pack_start (GTK_BOX (hbox2), gs_entry, TRUE, TRUE, 0);

  toolbar2=new_toolbar(parent,hbox2,input_toolbar,0,GTK_ICON_SIZE_MENU,0);

  gtk_toolbar_set_show_arrow(GTK_TOOLBAR(toolbar2),FALSE);
  gtk_widget_hide(toolbar2);

	/* This is the statusbar. All messages are displayed here ! */
  statusbar1 = gtk_statusbar_new ();

  gtk_widget_show (statusbar1);
  gtk_box_pack_start (GTK_BOX (hbox2), statusbar1, TRUE, TRUE, 0);

  if(compact1==0) {
	gtk_box_pack_start (GTK_BOX (vbox1), hbox2, FALSE, FALSE, 0);
  };
  /* the following is needed always so we can handle cursor keys correctly */	
  gtk_signal_connect (GTK_OBJECT (parent), "key_press_event",
                      GTK_SIGNAL_FUNC (on_parent_key_press_event),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (parent), "configure_event",
                      GTK_SIGNAL_FUNC (on_parent_configure),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (parent), "delete_event",
  						GTK_SIGNAL_FUNC(on_parent_destroy), NULL);
  
  gtk_signal_connect (GTK_OBJECT (gs_entry), "key_press_event",
                      GTK_SIGNAL_FUNC (on_gs_entry_key_press_event),
                      NULL);

  /* this is needed to send selections correctly! */
  gtk_signal_connect (GTK_OBJECT (parent), "selection_get",
						GTK_SIGNAL_FUNC (on_selection_get_event), NULL);

  gtk_signal_connect (GTK_OBJECT (parent), "selection_received",
						GTK_SIGNAL_FUNC (on_selection_received_event), NULL);


  gtk_widget_add_events(GTK_WIDGET(parent),GDK_SELECTION_NOTIFY);

	/* create default graphic context */

  fgc = GTK_EDIT_DISPLAY(curgwp->draw)->fgc;
  bgc = GTK_EDIT_DISPLAY(curgwp->draw)->bgc;

// set the XWindows icon
  {
    GdkPixmap   *pmap; 
    GdkBitmap   *mask; 
  
	if((pmap = gdk_pixmap_create_from_xpm_d(parent->window, &mask, NULL, icon_xpm)) != NULL)
	{
  		gdk_window_set_icon(parent->window,NULL,pmap,mask);
  	};
  };

  return parent;
}


int color_equal(fcolor_t *color1,fcolor_t *color2)
{
 if(color1->red==color2->red &&
 	color1->green==color2->green &&
	color1->blue==color2->blue) return true;
	return false;
}

/* show current file name on title bar */
void titletext ()
{
    char buf[MAXFLEN];
	if(parent == NULL) return ;
	snprintf(buf,sizeof(buf),"%s",cbfp->b_fname);
	gtk_window_set_title(GTK_WINDOW(parent), buf);
}

void drv_update_styles()
{
 // we have to destroy old styles if they exist!
 // MESG("drv_update_styles: color_scheme_ind=%d",color_scheme_ind);
 st1a = gtk_style_new();
 st1i = gtk_style_new();
 st3a = gtk_style_new();
 st3i = gtk_style_new();

 gdk_color_parse(current_scheme->color_style[COLOR_MENU_FG].color_value,&st1a->text[GTK_STATE_NORMAL]);
 gdk_color_parse(current_scheme->color_style[COLOR_MENU_FG].color_value,&st1a->fg[GTK_STATE_NORMAL]);
 gdk_color_parse(current_scheme->color_style[COLOR_MENU_BG].color_value,&st1a->base[GTK_STATE_NORMAL]);
 gdk_color_parse(current_scheme->color_style[COLOR_MENU_BG].color_value,&st1a->bg[GTK_STATE_NORMAL]);

 gdk_color_parse("green",&st1a->base[GTK_STATE_SELECTED]);

 gdk_color_parse(current_scheme->color_style[COLOR_INACTIVE_FG].color_value,&st1i->text[GTK_STATE_NORMAL]);
 gdk_color_parse(current_scheme->color_style[COLOR_INACTIVE_FG].color_value,&st1i->fg[GTK_STATE_NORMAL]);
 gdk_color_parse(current_scheme->color_style[COLOR_INACTIVE_BG].color_value,&st1i->base[GTK_STATE_NORMAL]);
 gdk_color_parse("green",&st1i->base[GTK_STATE_SELECTED]);

 gdk_color_parse(current_scheme->color_style[COLOR_ROWCOL_FG].color_value,&st3a->text[GTK_STATE_NORMAL]);
 gdk_color_parse(current_scheme->color_style[COLOR_ROWCOL_FG].color_value,&st3a->fg[GTK_STATE_NORMAL]);
 gdk_color_parse(current_scheme->color_style[COLOR_MENU_BG].color_value,&st3a->base[GTK_STATE_NORMAL]);
 gdk_color_parse(current_scheme->color_style[COLOR_MENU_BG].color_value,&st3a->bg[GTK_STATE_NORMAL]);
 gdk_color_parse("green",&st3a->base[GTK_STATE_SELECTED]);

 gdk_color_parse(current_scheme->color_style[COLOR_ROWCOL_FG].color_value,&st3i->text[GTK_STATE_NORMAL]);
 gdk_color_parse(current_scheme->color_style[COLOR_ROWCOL_FG].color_value,&st3i->fg[GTK_STATE_NORMAL]);
 gdk_color_parse(current_scheme->color_style[COLOR_INACTIVE_BG].color_value,&st3i->base[GTK_STATE_NORMAL]);
 gdk_color_parse(current_scheme->color_style[COLOR_INACTIVE_BG].color_value,&st3i->bg[GTK_STATE_NORMAL]);
 gdk_color_parse("green",&st3i->base[GTK_STATE_SELECTED]);
 // MESG("drv_update_styles: end");
}

int drv_search_dialog(int f)
{
	return FALSE;
}

void drv_flush() {	
	gdk_flush();
}

void drv_win_flush(WINDP *wp)
{
	gdk_flush();
}


void drv_set_default_bgcolor()
{
 GdkColor *gcolorb;
 gcolorb = current_colors[COLOR_BG];
 ccolorb_default.red = (float)gcolorb->red/65535;
 ccolorb_default.green = (float)gcolorb->green/65535;
 ccolorb_default.blue = (float)gcolorb->blue/65535;
}

/* clear the window to default color */
void window_clear(WINDP *wp)
{
 return;
	drv_set_default_bgcolor();
	// MESG("window_clear:%d",wp->id);
	if(wp!=NULL) {
		if(wp->gwp!=NULL) {
		if(GTK_EDIT_DISPLAY(wp->gwp->draw)->cr!=NULL) {
			cairo_set_source_rgba (GTK_EDIT_DISPLAY(wp->gwp->draw)->cr, ccolorb_default.red, ccolorb_default.green, ccolorb_default.blue,1);
			cairo_rectangle(GTK_EDIT_DISPLAY(wp->gwp->draw)->cr, 0,0,dra_width(wp->gwp),dra_height(wp->gwp));
			cairo_fill(GTK_EDIT_DISPLAY(wp->gwp->draw)->cr);
			} 
		} 
	} ; 
//	MESG("window_clear");
	cursor_showing=0;
//	drv_flush();
}


void drv_color(int fcol,int bcol) 
{
 GdkColor *gcolorf,*gcolorb;
 if(bcol>COLOR_TYPES) bcol=0;
 if(fcol>COLOR_TYPES) fcol=1;
	gcolorb = current_colors[bcol];
	ccolorb.red = (float)gcolorb->red/65535;
	ccolorb.green = (float)gcolorb->green/65535;
	ccolorb.blue = (float)gcolorb->blue/65535;
	gcolorf = current_colors[fcol];
	ccolorf.red = (float)gcolorf->red/65535;
	ccolorf.green = (float)gcolorf->green/65535;
	ccolorf.blue = (float)gcolorf->blue/65535;
}

void init_color()
{
 int i;
 /* Read the colors from the conf files  */
  color_scheme_read();
  cmap = gdk_colormap_get_system ();	/* use the system colormap */

 for(i=0;i<COLOR_TYPES;i++) 
 	current_colors[i]=(GdkColor *)malloc(sizeof(GdkColor));
}

void set_current_colors()
{
 int i=0;
 GdkColor color;
 for(i=0;i<COLOR_TYPES;i++) {
  if(!gdk_color_parse(current_scheme->color_style[i].color_value, &color  )) {
  	ERROR("color %s is not in database",current_scheme->color_style[i].color_value);
	exit(0);
  };
	memcpy(current_colors[i],&color,sizeof(GdkColor));
 };
}

int cursor_status()
{
	return cursor_showing;
}

void show_cursor (char *from)
{
   GdkRectangle area;
   double CH1 = (int)CHEIGHTI;
	GeEditDisplay *wd;

	if(!(cbfp->b_flag==FSNOTES || cbfp->b_flag==FSNOTESN || cbfp->b_flag & FSNLIST)) {

//	MESG("	show_cursor:%d",cursor_showing);
   if (cursor_showing) return; 

	wd = GTK_EDIT_DISPLAY(cwp->gwp->draw);

#if	SHOW_CLINE
	area.x = 0;
	area.height = 2;	/* current line height  */
	area.y = py+CHEIGHTI-area.height;
	area.width = cwp->w_ntcols*CH1;
//	area.width = 100;

	cairo_set_operator(wd->cr,CAIRO_OPERATOR_OVER);
	cairo_set_source_rgba(wd->cr,0.4,0.4,0.4,0.5);
	// MESG("show_cursor: x=%d y=%d h=%d w=%d",area.x,area.y,area.height,area.width);
	cairo_rectangle(wd->cr,	area.x, area.y,area.width,area.height);
	cairo_fill(wd->cr);
	cairo_set_operator(wd->cr,CAIRO_OPERATOR_OVER);
	ge_edit_display_expose_area((GtkWidget *)wd,&area);

//	cairo_region_destroy(region);
//	end_draw(wd,"show_cursor:line end");
#endif

//	MESG("show_cursor:[%s] wid=%d",from,cwp->id);
	cairo_set_operator(wd->cr,CAIRO_OPERATOR_OVER);
	cairo_set_source_rgba(wd->cr,1,0,0.1,0.6);	// this must be a color different from background FIXEME!
	cairo_rectangle(wd->cr,
			px+1, py ,
		       CLEN,
		       CH1 );
	cairo_fill(wd->cr);
	cairo_set_operator(wd->cr,CAIRO_OPERATOR_OVER);

	area.x = px;
	area.y = py;
	area.width = CLEN;
	area.height = CH1;
	ge_edit_display_expose_area(cwp->gwp->draw,&area);
	};

	ppx=px;ppy=py;
	cpposx=cposx;
	cpposy=cposy;
	cursor_showing=1;
}

// draw window character on screen
int put_wtext1(WINDP *wp, int row,int col, int pos_x)
{
	vchar *v1, *vtext;
	VIDEO *vp1;
	int ccolor,cattr;
	char vstr[8];
	GeEditDisplay *wd = GTK_EDIT_DISPLAY(wp->gwp->draw);
	// MESG("put_wtext1: col=%d pos_x=%d",col,pos_x);
#if	0
	if(pos_x<0 || row<0) {
		MESG("put_wtext1: wrong cursor position!!!! row=%d col=%d",row,col);
		return pos_x;
	};
#endif
#if	NUSE
	if(wp->w_ntcols*CLEN<pos_x || wp->w_ntrows<row) {
			MESG("too small window %d at(%d %d) max(%d %d)!!",wp->id,
			row,pos_x,wp->w_ntrows,wp->w_ntcols
		);
		row=0;
		return pos_x;
	};	
#endif
	vp1 = wp->vs[row];
	v1=vtext=vp1->v_text;
	
	ccolor=v1[col].fcolor;
	cattr=v1[col].bcolor;

	memset(vstr,0,8);
	drv_color(ccolor,cattr);

	px=pos_x;
	py=CHEIGHTI*row;
	cposx=col;
	cposy=row;

	ccolor=v1[col].fcolor;
	cattr=v1[col].bcolor;

	addutfvchar1(vstr,&v1[col],0,wp->w_fp);

	int len = utf_num_chars(vstr);

	// MESG("put_wtext1:row=%d col=%d [%s] len=%d px=%d",row,col,vstr,len,px);

	// background
	cairo_set_source_rgb(GTK_EDIT_DISPLAY(wp->gwp->draw)->cr,ccolorb.red,ccolorb.green,ccolorb.blue);
	cairo_rectangle(GTK_EDIT_DISPLAY(wp->gwp->draw)->cr, px, py, CLEN*len+10,CHEIGHTI);
	cairo_fill(GTK_EDIT_DISPLAY(wp->gwp->draw)->cr);

	// show text line
	cairo_set_source_rgb(GTK_EDIT_DISPLAY(wp->gwp->draw)->cr,ccolorf.red,ccolorf.green,ccolorf.blue);

	cairo_move_to(GTK_EDIT_DISPLAY(wp->gwp->draw)->cr,px+1,py+1);
	if(vstr[0]!=0xFF && vstr[1]!=0xFF) {
	 	// PangoLayout *layout;
		int width=0,height=0;
		if(wd->layout == NULL) wd->layout = pango_cairo_create_layout (GTK_EDIT_DISPLAY(wp->gwp->draw)->cr);
		// MESG("pw1: [%s]",vstr);
		pango_layout_set_text (wd->layout, vstr, -1);
		// MESG("row=%2d col=%3d [%s][%2X %2X %2X %2X]",row,col,vstr,vstr[0],vstr[1],vstr[2],vstr[3]);
		 // MESG("!");
		pango_layout_set_font_description (wd->layout, wd->ge_font_desc);
		// pango_font_description_free (desc);
		pango_cairo_show_layout (wd->cr, wd->layout);
		pango_layout_get_size (wd->layout, &width, &height);
		px+=width/PANGO_SCALE;
		// g_object_unref(layout);
	} else {
		// MESG("col=%d dummy char!",col);
	 	px+=CLEN;
	}
	return px;
}

/* just refresh the current line */
void expose_line(int line, WINDP *wp)
{
#if	1
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
//	MESG("	hide_cursor:from %s %d cols=%d",from,cursor_showing,cwp->w_ntcols);
	if (cursor_showing == 0) return;
	cursor_showing = 0;
#if	1
	if(cbfp->b_flag==FSNOTES || cbfp->b_flag==FSNOTESN || cbfp->b_flag & FSNLIST) {
	cursor_showing = 0;
	return;		
	};
#endif
#if	SHOW_CLINE
	draw_window_line(cwp,cpposy);
#else
   GdkRectangle area;
	int pos_x=0;
	area.y = ppy;
	area.height = CHEIGHTI;
	area.x = ppx;
	area.width = CLEN;
	put_wtext1(cwp,cpposy,cpposx,ppx);
	ge_edit_display_expose_area(cwp->gwp->draw,&area);
#endif
// 	gdk_flush();
}

char *get_font_string();

void set_edit_font(GeEditDisplay *wd)
{
 if(wd->cr==NULL) return;
	char *font_str=get_font_string();
	// PangoLayout *layout;
	if(wd->ge_font_desc!=NULL) pango_font_description_free (wd->ge_font_desc);
	wd->ge_font_desc = pango_font_description_from_string (font_str);
	if(wd->layout==NULL) wd->layout = pango_cairo_create_layout (wd->cr);

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
	CHEIGHTI= (int)(CHEIGHT)+1;
	// MESG("set_edit_font: pango width = %f->%d h=%f CLEN=%f CHEIGHT=%f %d",pango_w,(int)pango_w,pango_h,CLEN,CHEIGHT,CHEIGHTI);
	CLENI = (int)CLEN;
	// g_object_unref(wd->layout);
}

void set_window_font(WINDP *wp)
{
 GeEditDisplay *wd = (GeEditDisplay *)(wp->gwp->draw);
 set_edit_font(wd);
}

int select_font(num n)
{
 char font_name[MAXFLEN];
 int status;
 gchar *st;

 st = strdup("ΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠΡΣΤΥΦΧΨΩ");

 if(!macro_exec) {
	GtkWidget *fs;
 	// get font_name from font dialog
	fs = gtk_font_selection_dialog_new("main font");
	gtk_font_selection_set_font_name((GtkFontSelection *)GTK_FONT_SELECTION_DIALOG(fs)->fontsel,"menlo 14");
	gtk_font_selection_set_preview_text((GtkFontSelection *)GTK_FONT_SELECTION_DIALOG(fs)->fontsel,st);

	g_signal_connect(fs,"destroy",(GCallback) on_destroy,fs);
	g_signal_connect(fs,"font-activated",(GCallback) font_ok_select,fs);

	gtk_widget_show(fs);
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

 value = start;
 if(value==0) value=1;
// if(slide_flag!=2) 
 {
 slide_flag=1;

 if(wp->w_fp == NULL) {
	wp->gwp->draw_adj->page_size=0.0;
 } else {
	num lines;

	if(wp->w_fp->b_flag & FSNLIST) lines=wp->w_fp->dir_list_str->size;
#if	TNOTES
	else if(wp->w_fp->b_flag & FSNOTESN) lines=wp->w_fp->dir_list_str->size;
	else if(wp->w_fp->b_flag & FSNOTES) lines=wp->w_fp->b_tag_list->size;
#endif
	else lines=wp->w_fp->lines;

 len = wp->w_ntrows;
 wp->gwp->draw_adj->page_size = len;
 wp->gwp->draw_adj->upper = lines+len;
 wp->gwp->draw_adj->page_increment = len/2;

 };

 gtk_adjustment_changed(wp->gwp->draw_adj);
 slide_flag=1;
 gtk_adjustment_set_value(wp->gwp->draw_adj, value);
 slide_flag=0;
 };
}

void drv_start_window_update(WINDP *wp)
{
	window_clear(wp);
}

GtkWidget * create_select_window()
{
  GtkWidget *wlist;
  GtkWidget *vbox2;
  GtkListStore *mlist1;	// model
  GtkTreeSelection *list1_select=NULL;
  char *stitles[] = { "value",NULL };

  wlist = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_window_set_modal(GTK_WINDOW(wlist),1); // allow input only from this window
  gtk_window_set_title (GTK_WINDOW (wlist), "select");
  gtk_window_set_transient_for (GTK_WINDOW(wlist),GTK_WINDOW(parent));
  gtk_widget_set_usize(wlist, 200,200);
  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (wlist), vbox2);

  mlist1 = gtk_list_store_new(1,G_TYPE_STRING);
  list1 = kt_new_listview(mlist1,vbox2,stitles,0);
  list1_select = gtk_tree_view_get_selection (GTK_TREE_VIEW (list1));
  gtk_tree_selection_set_mode (list1_select, GTK_SELECTION_BROWSE);

  gtk_widget_realize(wlist);

  g_signal_connect (G_OBJECT (wlist), "hide",
  						GTK_SIGNAL_FUNC(on_select_hide),mlist1);
  gtk_signal_connect (GTK_OBJECT (wlist), "key_press_event",
                      GTK_SIGNAL_FUNC (on_wlist_key_press),
                      list1);
  gtk_signal_connect (GTK_OBJECT (wlist), "destroy_event",
  						GTK_SIGNAL_FUNC(on_select_destroy), NULL);

  gtk_signal_connect (GTK_OBJECT (wlist), "delete_event",
  						GTK_SIGNAL_FUNC(on_select_destroy), NULL);

  g_signal_connect (G_OBJECT (list1_select), "changed",
                      GTK_SIGNAL_FUNC (on_list1_change_row),
                      list1);

  g_signal_connect (G_OBJECT (list1), "row-activated",
                      GTK_SIGNAL_FUNC (on_list1_activate_row),
                      mlist1);
 return(wlist);
}

void put_string_dl(char *string) 
{
// MESG("put_str_dl:[%s]",string);
#if	1
 gtk_label_set_text((GtkLabel *)((GtkStatusbar *)statusbar1)->label,string);
#else
 char *utf;
 gsize r,w;
 utf = utf_check(string,&r);
 if(utf==NULL) {
	prepare_converter(default_local_codepage);
	utf = g_convert_with_iconv(string,-1,cd,&r,&w,NULL);
	
	gtk_label_set_text((GtkLabel *)((GtkStatusbar *)statusbar1)->label,utf);
	g_free(utf);
 } else {
	gtk_label_set_text((GtkLabel *)((GtkStatusbar *)statusbar1)->label,utf);
 };
#endif
}

/* refresh the current window, expose it  */
void expose_window(WINDP *wp)
{
  GdkRectangle area;
  GWINDP *gwp = wp->gwp;

  if(gwp==NULL) return;
  area.x = 0;
  area.y = 0;
  area.width = dra_width(gwp); // (curgwp->draw)->allocation.width;
  area.height = dra_height(gwp); // (curgwp->draw)->allocation.height;

  ge_edit_display_expose_area(gwp->draw,&area);
  if(GTK_EDIT_DISPLAY(gwp->draw)->wp == cwp) {
	movecursor(wp->currow, WGetCol());
	// MESG("expose_window ->sc");
	show_cursor("expose_window");
  };
}

/* refresh all the windows */
void update_full(int f)
{
  WINDP *wp;
  WINDP *tmpwp;
  tmpwp = cwp;
//	MESG("update_full:");
	update_screen(f);
	lbegin(window_list);
	while((wp=(WINDP *)lget(window_list))!=NULL)
	{
		set_current_window(wp,"update_full");
			if(f) {
				expose_window(wp);
				// we should send an allocation signal for each window instead
			};
	};
	set_current_window(tmpwp,"update_full:current");
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

 	gtk_widget_destroy(gwp->draw);
 	gtk_widget_destroy((GtkWidget *)gwp->scroll_bar);

	gtk_widget_destroy((GtkWidget *)gwp->hbox);
	gtk_widget_destroy((GtkWidget *)gwp->status2);
	gtk_widget_destroy((GtkWidget *)gwp->status1);
	gtk_widget_destroy((GtkWidget *)gwp->status3);
	gtk_widget_destroy((GtkWidget *)gwp->hstatus);

	gtk_widget_destroy((GtkWidget *)gwp->box);
	gtk_widget_destroy((GtkWidget *)gwp->bte->pbox); // pbox

 /* reparent his simbling */
	if(btes->pan!=NULL) { // there is a tree underneeth !!
		gtk_widget_reparent((GtkWidget *)btes->pan, (GtkWidget *)btep->pbox);
		gtk_widget_hide_on_delete((GtkWidget *)(btep->pan));
		btep->pan = btes->pan;
		gtk_widget_destroy((GtkWidget *)btes->pbox);
		btep->gw=NULL;
		btep->left = btes->left;
		btes->left->parent=btep;
		btep->right = btes->right;
		btes->right->parent=btep;
		free(btes);
		free(btec);
	} else { // only a single window underneeth !!
		// we must hide the child windows before reparent the father! and show them again after! (in GTK2 only, GTK1 works ok!)
		gtk_widget_hide(btes->gw->status2);
		gtk_widget_hide(btes->gw->status1);
		gtk_widget_hide(btes->gw->status3);
		gtk_widget_hide(btes->gw->hstatus);
		gtk_widget_reparent((GtkWidget *)btes->gw->box, (GtkWidget *)btep->pbox);
		gtk_widget_show(btes->gw->status2);
		gtk_widget_show(btes->gw->status1);
		gtk_widget_show(btes->gw->status3);
		gtk_widget_show(btes->gw->hstatus);

		btep->gw=btes->gw; 
		btes->gw->bte=btep;
		gtk_widget_hide_on_delete((GtkWidget *)(btep->pan));
		btep->pan=NULL;
		free(btep->left);
		free(btep->right);
		btep->left=btep->right=NULL;
	}
}

void scheme_names_button_change(GtkWidget *wdg, void* data)
{
 int active,i;

 active=gtk_combo_box_get_active(GTK_COMBO_BOX (wdg));
 color_scheme_ind = active;
 change_color_scheme(color_scheme_ind+1);
 update_screen(1);
 // MESG("scheme_names_button_change: 1"); 
 
  for(i=0;i<CTYPE_ROWS;i++) {
	show_color_sample(table1,i+1,"text");
  };
 // MESG("scheme_names_button_change: ok!"); 
}

void connect_exit_button(GtkBox *parent_box, GCallback callback_function, void *data)
{
//GTK_BOX (gwp->hstatus),(GCallback) cb_close_window,(void *)wp)
 GtkWidget *exit_button;
 GtkWidget *icon;
 exit_button = gtk_button_new();

 gtk_button_set_relief(GTK_BUTTON(exit_button), GTK_RELIEF_NONE);
 gtk_button_set_focus_on_click(GTK_BUTTON(exit_button), FALSE);

 icon = gtk_image_new_from_icon_name("window-close",GTK_ICON_SIZE_MENU);

 gtk_container_set_border_width(GTK_CONTAINER(exit_button),0);
 gtk_container_add(GTK_CONTAINER(exit_button), icon);

 gtk_widget_show(exit_button);
 gtk_widget_show(icon);

 gtk_button_set_focus_on_click(GTK_BUTTON(exit_button), FALSE);
 
 gtk_signal_connect (GTK_OBJECT (exit_button), "clicked",
                      GTK_SIGNAL_FUNC (callback_function),
                      data);

 gtk_box_pack_end (parent_box, (GtkWidget *)exit_button, FALSE, FALSE, 0);
}

/* combine edit display creation */
/* the new one */
GWINDP * ge_cedit_new(GtkWidget *parent, WINDP *wp,int ptype)
{
 GWINDP  *new_gwp;
 GtkBox *pbox1=NULL,*pbox2=NULL,*pan=NULL;
 BTWE *btep,*btel,*bter; /* parent,left,right */
// MESG("ge_cedit_new:");
 new_gwp = ( GWINDP *) malloc(sizeof( GWINDP));

	if(wp!=NULL) wp->gwp = new_gwp;	
	else ERROR("windp wp is NULL!");
	new_gwp->ptype = ptype; /* 0=main, 1=hsplit, 2=vsplit */

	new_gwp->hbox=(void *)gtk_hbox_new(FALSE,0);
 	new_gwp->box = (void *)gtk_vbox_new(FALSE,0);	// here goes the new draw
	gtk_box_pack_start(GTK_BOX(new_gwp->box),(GtkWidget *)new_gwp->hbox,TRUE,TRUE,0);
	gtk_widget_show((GtkWidget *)new_gwp->box);
	gtk_widget_show((GtkWidget *)new_gwp->hbox);

	pbox2=(void *)gtk_hbox_new(FALSE,0);
	gtk_widget_show(GTK_WIDGET(pbox2));

 if(ptype==0) { /* this is for the first window */
	rootbtwe.parent=NULL;
	rootbtwe.left=NULL;
	rootbtwe.right=NULL;
	rootbtwe.type=0;
	rootbtwe.pan=NULL;
	rootbtwe.gw=new_gwp;
	rootbtwe.id=btindex++;
	new_gwp->bte=(struct BTWE *)&rootbtwe;
	rootbtwe.pbox=pbox2;
	gtk_box_pack_start(GTK_BOX(parent), (GtkWidget *)pbox2, TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(pbox2), (GtkWidget *)new_gwp->box, TRUE,TRUE,0);

	gtk_widget_show(GTK_WIDGET(new_gwp->box));
 } else { /* this is for any additional window */
	btep = curgwp->bte;
	if(ptype==1)pan = (void *)gtk_vpaned_new();
	if(ptype==2)pan = (void *)gtk_hpaned_new();

	pbox1 = (void *)gtk_hbox_new(FALSE,0);	// here goes the old draw
	gtk_widget_show(GTK_WIDGET(pbox1));
	gtk_widget_show(GTK_WIDGET(new_gwp->box));
	gtk_box_pack_start(GTK_BOX(pbox2), (GtkWidget *)new_gwp->box, TRUE,TRUE,0);

	gtk_box_pack_start(GTK_BOX(btep->pbox), (GtkWidget *)pan, TRUE,TRUE,0);
//	The following sets what window will be the active one
    gtk_paned_pack1(GTK_PANED(pan),  (GtkWidget *)pbox1, TRUE, TRUE);
    gtk_paned_pack2(GTK_PANED(pan),  (GtkWidget *)pbox2, TRUE, TRUE);

	gtk_widget_hide(curgwp->status2);
	gtk_widget_hide(curgwp->status1);
	gtk_widget_hide(curgwp->status3);
	gtk_widget_hide(curgwp->hstatus);

	gtk_widget_reparent((GtkWidget *)curgwp->box, (GtkWidget *)pbox1);
	gtk_widget_show(curgwp->hstatus);
	gtk_widget_show(curgwp->status2);
	gtk_widget_show(curgwp->status1);
	gtk_widget_show(curgwp->status3);

	gtk_widget_show(GTK_WIDGET(pan));
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
 new_gwp->draw = GTK_EDIT_DISPLAY(ge_edit_display_new());

 GTK_WIDGET_SET_FLAGS (new_gwp->draw, GTK_DOUBLE_BUFFERED);

 GTK_EDIT_DISPLAY(new_gwp->draw)->wp = wp;

 gtk_box_pack_start(GTK_BOX(new_gwp->hbox),new_gwp->draw,TRUE,TRUE,0);
 new_gwp->draw_adj = (GtkAdjustment *)gtk_adjustment_new(50.0,0.0,100.0,1.0,5.0,10.0);

 /* scroll bar */
 new_gwp->scroll_bar = (GtkVScrollbar *)gtk_vscrollbar_new ( new_gwp->draw_adj );
 gtk_signal_connect(GTK_OBJECT(new_gwp->draw_adj), "value_changed", GTK_SIGNAL_FUNC(cb_set_position),new_gwp->draw);

 im_context = gtk_im_context_simple_new ();
 g_signal_connect (im_context, "commit", G_CALLBACK (gtk_entry_commit_cb), NULL);

 /* create status as a triple entry */
 new_gwp->hstatus = gtk_hbox_new(FALSE,1);
 new_gwp->evb_hstatus = gtk_event_box_new();
 gtk_container_set_border_width(GTK_CONTAINER(new_gwp->evb_hstatus),0);
 gtk_container_set_border_width(GTK_CONTAINER(new_gwp->hstatus),0);
 gtk_container_add (GTK_CONTAINER(new_gwp->evb_hstatus), new_gwp->hstatus);
 gtk_event_box_set_above_child ((GtkEventBox *)new_gwp->evb_hstatus,0);
 new_gwp->status2 = gtk_entry_new();
 new_gwp->status1 = gtk_entry_new();
 new_gwp->status3 = gtk_entry_new();
 gtk_entry_set_has_frame((GtkEntry *)new_gwp->status2,0);

 gtk_entry_set_editable((GtkEntry *)new_gwp->status1,FALSE);
 gtk_entry_set_inner_border((GtkEntry *)new_gwp->status1,NULL);
 gtk_entry_set_editable((GtkEntry *)new_gwp->status2,FALSE);
 gtk_entry_set_inner_border((GtkEntry *)new_gwp->status2,NULL);
 gtk_entry_set_editable((GtkEntry *)new_gwp->status3,FALSE);
 gtk_entry_set_inner_border((GtkEntry *)new_gwp->status3,NULL);

 gtk_entry_set_width_chars (GTK_ENTRY (new_gwp->status3), 20);
 gtk_entry_set_width_chars (GTK_ENTRY (new_gwp->status2), 5);
 if(wp->w_fp!=NULL) gtk_entry_set_text((GtkEntry *)new_gwp->status1,wp->w_fp->b_fname);
 gtk_entry_set_alignment((GtkEntry *)new_gwp->status3,1);

 gtk_entry_set_has_frame((GtkEntry *)new_gwp->status2,FALSE);
 gtk_entry_set_has_frame((GtkEntry *)new_gwp->status1,FALSE);
 gtk_entry_set_has_frame((GtkEntry *)new_gwp->status3,FALSE);

 // clear the default style
// gtk_widget_set_style(new_gwp->evb_hstatus,st1i); 
 gtk_widget_set_style(new_gwp->status1,st1a); 
 gtk_widget_set_style(new_gwp->status3,st3a);
 gtk_widget_set_style(new_gwp->status2,st3a); 
 gtk_box_pack_start (GTK_BOX (new_gwp->hbox), (GtkWidget *)new_gwp->scroll_bar, FALSE, FALSE, 0);
 
 gtk_box_pack_start (GTK_BOX (new_gwp->box), (GtkWidget *)new_gwp->evb_hstatus, FALSE, TRUE, 0);

 gtk_box_pack_start (GTK_BOX (new_gwp->hstatus), (GtkWidget *)new_gwp->status2, FALSE, FALSE, 1);
 gtk_box_pack_start (GTK_BOX (new_gwp->hstatus), (GtkWidget *)new_gwp->status1, TRUE, TRUE, 1);
// gtk_widget_set_usize((GtkWidget *)new_gwp->status1,100,30);
 gtk_box_pack_start (GTK_BOX (new_gwp->hstatus), (GtkWidget *)new_gwp->status3, FALSE, FALSE, 1);
 

 gtk_widget_realize(new_gwp->hstatus);
 gtk_widget_show(new_gwp->evb_hstatus);
 gtk_event_box_set_above_child ((GtkEventBox *)new_gwp->evb_hstatus,0);
 
 gtk_widget_show(new_gwp->hstatus);
 gtk_widget_show(new_gwp->evb_hstatus);
 
 gtk_widget_show(new_gwp->status2);
 gtk_widget_show(new_gwp->status1);
 gtk_widget_set_size_request((GtkWidget *)new_gwp->hstatus,600,-1);

 gtk_widget_show(new_gwp->status3);
// if(window_list->size>1) 
 {
 	connect_exit_button(GTK_BOX (new_gwp->hstatus),(GCallback) cb_close_window,(void *)wp);
 }
 gtk_widget_show ((GtkWidget *)new_gwp->scroll_bar);
 gtk_widget_realize(new_gwp->draw);
 gtk_widget_show(new_gwp->draw);

 return (new_gwp);
}

/* Put a string on a window's statusline at position */
void put_string_statusline(WINDP *wp, char *st, int position)
{
 if(cwp->gwp == NULL) return;
 if(wp==cwp) {
	gtk_widget_set_style(GTK_WIDGET(wp->gwp->evb_hstatus),st3a);
	gtk_widget_set_style(GTK_WIDGET(wp->gwp->status1),st1a);
	gtk_widget_set_style(GTK_WIDGET(wp->gwp->status2),st3a);
	gtk_widget_set_style(GTK_WIDGET(wp->gwp->status3),st3a);

 } else {
	gtk_widget_set_style(GTK_WIDGET(wp->gwp->evb_hstatus),st3i);
	gtk_widget_set_style(GTK_WIDGET(wp->gwp->status1),st1i);
	gtk_widget_set_style(GTK_WIDGET(wp->gwp->status2),st3i);
	gtk_widget_set_style(GTK_WIDGET(wp->gwp->status3),st3i);
 };
 if(position>0) gtk_entry_set_text(GTK_ENTRY(wp->gwp->status3),st);
 if(position==0) gtk_entry_set_text(GTK_ENTRY(wp->gwp->status1),st);
 if(position<0) gtk_entry_set_text(GTK_ENTRY(wp->gwp->status2),st);
}


int newxy(WINDP *wp)
{
	if(wp==NULL) {
		return FALSE;
	};
	free_virtual_window(wp);

	wp->w_ntcols = dra_width(wp->gwp)/CLEN+1;
	wp->w_ntrows = dra_height(wp->gwp)/CHEIGHTI+2;
	set_window_width(wp);
	allocate_virtual_window(wp);
	return(TRUE);
}

/* this is called when we move the slider */
void cb_set_position(GtkAdjustment *adj, GtkWidget *widget)
{
 int prev_line; // previous line
 int gline;
 num lines;

 WINDP *pwp;	// previous window if changed
 GeEditDisplay *wd = GTK_EDIT_DISPLAY(widget);
 in_slide=1;
// MESG("cb_set_position: id=%d",wd->wp->id);
  if(slide_flag==0) 
  {
  slide_flag=2;

  	// make this window the current window
	if(cwp != wd->wp) {
		cursor_showing=1;
		hide_cursor("cb_set_position cwp!=wp");
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

	gline = lines / (adj->upper-adj->page_size ) * adj->value;
	if(gline>lines-1) gline=lines-1;

	if(prev_line==gline) { 
		in_slide=0; 
		slide_flag=0;
		return;
	};

	hide_cursor("cb_set_position");

	if(cbfp->b_flag & FSNLIST) {
		cwp->current_note_line=gline;
	} else textpoint_set_lc(cwp->tp_current,gline-1,0);

	upd_column_pos();
	check_cursor_position(wd->wp);
	if(is_wrap_text(wd->wp->w_fp)) 
	upd_all_wrap_lines(wd->wp,"cb_set_position 3");
	else
	upd_all_virtual_lines(wd->wp,"cb_set_posision");

	update_physical_windows();
	movecursor(window_cursor_line(wd->wp), WGetCol());

	expose_window(wd->wp);
	show_cursor("cb_set_position");
  };
  slide_flag=0;
  in_slide=0;
}

void font_ok_select(GtkFontSelectionDialog *fs, gpointer *data)
{
 char st[1024];
 gchar *fname;
 GtkFontSelection *fsel = GTK_FONT_SELECTION(fs->fontsel);

 fname=gtk_font_selection_get_font_name(fsel);
 if(fname!=NULL) {
 strlcpy(st,gtk_font_selection_get_font_name(fsel),1020);
 };
 gtk_widget_destroy((GtkWidget *)fs);
 if(fname!=NULL) {
// hide_cursor("font_ok_select");
 set_font(st);
 set_cursor(0,"font_ok_select");
 update_screen(1);
// MESG("font_ok_select ->sc");
 show_cursor("font_ok_select");
 };
}

void drv_back_color()
{
 if(parent==NULL) { 
	return;
 };
 if(cwp==NULL) { 
	return;
 };

 if(GTK_EDIT_DISPLAY(curgwp->draw)->edit_window == NULL) { 
	return;
 };	
 gdk_window_set_background(GTK_WIDGET(parent)->window, current_colors[COLOR_BG]);
 gdk_window_set_background(GTK_EDIT_DISPLAY(curgwp->draw)->edit_window, current_colors[COLOR_BG]);
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



color_element *current_color_element=NULL; 
GtkWidget *csd=NULL;
int foreground_changed=0;

void color_selection(char *wtitle, int icolor)
{
 GdkColor *gcolor;
 GtkWidget *cs;
 
 changed_color = icolor;
 gcolor = (GdkColor *)current_colors[changed_color];

 if(csd==NULL) {
 	csd = gtk_color_selection_dialog_new(wtitle);

 gtk_signal_connect_object(GTK_OBJECT(GTK_COLOR_SELECTION_DIALOG(csd)->ok_button),
	"clicked",GTK_SIGNAL_FUNC(color_ok_select),GTK_OBJECT(csd));
 gtk_button_set_label(GTK_BUTTON(GTK_COLOR_SELECTION_DIALOG(csd)->ok_button),"apply");
 gtk_signal_connect_object(GTK_OBJECT(GTK_COLOR_SELECTION_DIALOG(csd)->cancel_button),
 	"clicked",(GtkSignalFunc) color_close_select,GTK_OBJECT(csd));
 gtk_button_set_label(GTK_BUTTON(GTK_COLOR_SELECTION_DIALOG(csd)->cancel_button),"close");
 gtk_signal_connect_object(GTK_OBJECT(csd),
 	"destroy",(GtkSignalFunc) on_destroy,GTK_OBJECT(csd));
	gtk_widget_show(csd);

  gtk_signal_connect (GTK_OBJECT (csd), "key_press_event",
                      GTK_SIGNAL_FUNC (on_wlist_key_press),
                      NULL);

 } else {
	gtk_window_set_title(GTK_WINDOW(csd),wtitle);
	gtk_widget_show(csd);
 };

 cs = GTK_COLOR_SELECTION_DIALOG(csd)->colorsel;
 gtk_color_selection_set_previous_color( GTK_COLOR_SELECTION(cs), gcolor);
 gtk_color_selection_set_current_color( GTK_COLOR_SELECTION(cs), gcolor);
 
}


void color_button_select_fg(GtkWidget *wd, gpointer data)
{
  color_element *ce = (color_element *)data;
  char label[MAXSLEN];
  snprintf(label,sizeof(label),"%s foreground",ce->label);
  current_color_element=ce;
  foreground_changed=1;
  color_selection(label,ce->fg);
}

void color_button_select_bg(GtkWidget *wd, gpointer data)
{
  color_element *ce = (color_element *)data;
  char label[MAXSLEN];
  snprintf(label,sizeof(label),"%s background",ce->label);
  current_color_element=ce;
  foreground_changed=0;
  color_selection(label,ce->bg);
}

/* show color selection and set background color */
void color_close_select(GtkWidget *wd, gpointer *data)
{
	gtk_widget_destroy(wd);
	csd=NULL;
}

void color_ok_select(GtkWidget *wd, gpointer *data)
{
 GdkColor *gcolor,*fcolor;
 int i;
 GtkWidget *ctext;
 GtkStyle *box_style;

 GtkColorSelectionDialog *csd = GTK_COLOR_SELECTION_DIALOG(wd);
 GtkColorSelection *cs = GTK_COLOR_SELECTION(csd->colorsel);
 char new_color_name[20];

 gcolor = (GdkColor *)current_colors[changed_color];
 gtk_color_selection_get_current_color(GTK_COLOR_SELECTION(cs), gcolor);
 gdk_colormap_alloc_color(cmap,gcolor,0,1);

 change_color_scheme(color_scheme_ind+1);
 update_screen(TRUE);
 // MESG("color_ok_select ->sc");
 show_cursor("color_ok_select");

 ctext=current_color_element->sample_ctext;

   box_style = current_color_element->sample_style;

	/* must remove widget to change background color  */
	gtk_widget_destroy(ctext);
	current_color_element->sample_ctext=gtk_entry_new();
	ctext=current_color_element->sample_ctext;
	put_to_table(ctext,table1,current_color_element->row,4,0);

 if(foreground_changed) {
	box_style->text[GTK_STATE_NORMAL].red=gcolor->red;
	box_style->text[GTK_STATE_NORMAL].green=gcolor->green;
	box_style->text[GTK_STATE_NORMAL].blue=gcolor->blue;
	gtk_entry_set_text(GTK_ENTRY(ctext),"fg updated");
	gtk_widget_set_style(ctext,box_style); 
 } else {

	fcolor = (GdkColor *)current_colors[current_color_element->fg];
	box_style->text[GTK_STATE_NORMAL].red=fcolor->red;
	box_style->text[GTK_STATE_NORMAL].green=fcolor->green;
	box_style->text[GTK_STATE_NORMAL].blue=fcolor->blue;

	box_style->base[GTK_STATE_NORMAL].red=gcolor->red;
	box_style->base[GTK_STATE_NORMAL].green=gcolor->green;
	box_style->base[GTK_STATE_NORMAL].blue=gcolor->blue;
	gtk_entry_set_text(GTK_ENTRY(ctext),"bg updated");

	gtk_widget_set_style(ctext,box_style); 

	for(i=0;i<CTYPE_ROWS;i++){
		/* find same background to other color types and change accordingly  */
		if(ctype[i].bg == -current_color_element->bg){
			/* must remove widget to change background color  */
			box_style=ctype[i].sample_style;
			gtk_widget_destroy(ctype[i].sample_ctext);
			ctype[i].sample_ctext = gtk_entry_new ();
			ctext=ctype[i].sample_ctext;
			put_to_table(ctext,table1,ctype[i].row,4,0);

			box_style->base[GTK_STATE_NORMAL].red=gcolor->red;
			box_style->base[GTK_STATE_NORMAL].green=gcolor->green;
			box_style->base[GTK_STATE_NORMAL].blue=gcolor->blue;
						
			gtk_widget_set_style(ctext,box_style); 
			gtk_entry_set_text(GTK_ENTRY(ctext),"obg updated");
		};
	};	
 };
 sprintf(new_color_name,"#%02X%02X%02X",gcolor->red/256,gcolor->green/256,gcolor->blue/256);
 current_scheme->color_style[changed_color].color_value=strdup(new_color_name);

 gtk_widget_show(ctext);
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
}

void put_to_table(GtkWidget *wdg, GtkWidget *table, int ypos, int xpos, int align)
{
  int xoptions;
  if(xpos==2 && !align) xoptions = GTK_FILL|GTK_EXPAND;
  else xoptions = GTK_FILL;
  gtk_table_attach (GTK_TABLE (table), wdg, xpos-1, xpos, ypos-1, ypos,
                    (GtkAttachOptions) (xoptions),
                    (GtkAttachOptions) (0), 0, 0);
  // this is for labels only
  if(align) { 
	gtk_misc_set_alignment (GTK_MISC (wdg), 0, 0.5);
	gtk_misc_set_padding (GTK_MISC (wdg), 2, 0);
  };
  gtk_widget_set_usize(wdg,0,25);
  gtk_widget_show(wdg);
}


GtkWidget * griv_table_new(int rows, int cols, int border)
{
 GtkWidget *table;
  table = gtk_table_new (rows, cols, FALSE);
  gtk_widget_show(table);
  gtk_container_set_border_width (GTK_CONTAINER (table), border);
  gtk_table_set_row_spacings (GTK_TABLE (table), 0);
  gtk_table_set_col_spacings (GTK_TABLE (table), 4);
  return table;
}

void show_color_sample(GtkWidget *table,int ypos,char *text)
{
 GtkWidget *ctext;
 int i=ypos-1;
 GtkStyle *box_style;
 int fgindex,bgindex;
 GdkColor *bcolor,*fcolor;
	if(ctype[i].bg>0) {
		bgindex=ctype[i].bg;
	} else {
		bgindex=-ctype[i].bg;
	};
	if(ctype[i].fg>0) fgindex=ctype[i].fg;
	else 	fgindex= -ctype[i].fg;

 	if(ctype[i].sample_ctext != NULL) gtk_widget_destroy(ctype[i].sample_ctext);
	ctype[i].sample_ctext = gtk_entry_new ();
	ctext=ctype[i].sample_ctext;
	gtk_entry_set_editable((GtkEntry *)ctext,false);
	gtk_entry_set_has_frame((GtkEntry *)ctext,true);
	gtk_entry_set_text(GTK_ENTRY(ctext),text);
	if(ctype[i].sample_style==NULL) {
		ctype[i].sample_style=gtk_style_new();
		box_style = ctype[i].sample_style;
		gdk_color_parse(current_scheme->color_style[bgindex].color_value,&box_style->base[GTK_STATE_NORMAL]);
		gdk_color_parse(current_scheme->color_style[fgindex].color_value,&box_style->text[GTK_STATE_NORMAL]);
	} else {
		bcolor = (GdkColor *)current_colors[bgindex];
		fcolor = (GdkColor *)current_colors[fgindex];
		box_style = ctype[i].sample_style;
		box_style->text[GTK_STATE_NORMAL].red=fcolor->red;
		box_style->text[GTK_STATE_NORMAL].green=fcolor->green;
		box_style->text[GTK_STATE_NORMAL].blue=fcolor->blue;

		box_style->base[GTK_STATE_NORMAL].red=bcolor->red;
		box_style->base[GTK_STATE_NORMAL].green=bcolor->green;
		box_style->base[GTK_STATE_NORMAL].blue=bcolor->blue;
		
	};

	gtk_widget_set_style(ctext,box_style); 

	put_to_table(ctext,table,ypos,4,0);
}

extern alist *color_schemes;

int set_color(num n)
{
 GtkWidget *vbox;
 GtkWidget *hbox;
 GtkWidget *hbox1;
  GtkWidget *scheme_label;
  GtkWidget *scheme_names_button;
 GtkWidget *hbox2;
  GtkWidget *sbutton;
  GtkWidget *apply_button;
  GtkWidget *close_button;
  char **scheme_names = get_scheme_names();

 int i;
 char wtitle[256];

 if(colors_win!=NULL) {
 	gtk_widget_show(colors_win);
	return(1);
 }; 

 snprintf(wtitle,sizeof(wtitle),"set color %lld",n);
 changed_color=n;

 colors_win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
 gtk_window_set_title(GTK_WINDOW(colors_win),"color");
 gtk_widget_show(colors_win);

  GtkWidget *label,*fgb,*bgb;
  int ypos;

  table1 = griv_table_new (CTYPE_ROWS, 4,10);

  vbox = gtk_vbox_new(FALSE,0);gtk_widget_show(vbox);
  hbox = gtk_hbox_new(FALSE,0);gtk_widget_show(hbox);

  hbox1 = gtk_hbox_new(FALSE,0);gtk_widget_show(hbox1);
  scheme_label=gtk_label_new("select color scheme");gtk_widget_show(scheme_label);

  scheme_names_button=new_combo_box(scheme_names,color_scheme_ind);
  gtk_signal_connect(GTK_OBJECT(scheme_names_button),"changed", (GtkSignalFunc) scheme_names_button_change ,NULL);

  hbox2 = gtk_hbox_new(FALSE,0);gtk_widget_show(hbox2);
  sbutton = gtk_button_new_with_label("save");
  gtk_widget_show(sbutton);
  apply_button = gtk_button_new_with_label("apply");
  gtk_widget_show(apply_button);
  close_button = gtk_button_new_with_label("close");
  gtk_widget_show(close_button);
  
  gtk_container_add (GTK_CONTAINER (colors_win), vbox);
  gtk_container_set_border_width (GTK_CONTAINER (colors_win), 10);
  gtk_box_pack_start(GTK_BOX(vbox),hbox1, FALSE,TRUE,0);
  gtk_box_pack_start(GTK_BOX(vbox),table1, FALSE,TRUE,0);

  gtk_box_pack_start(GTK_BOX(hbox1),scheme_label, TRUE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(hbox1),scheme_names_button, TRUE,FALSE,0);

  gtk_box_pack_start(GTK_BOX(vbox),hbox2, FALSE,TRUE,0);

  gtk_box_pack_start(GTK_BOX(hbox2),sbutton, TRUE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(hbox2),apply_button, TRUE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(hbox2),close_button, TRUE,FALSE,0);	/*   */

  gtk_signal_connect(GTK_OBJECT(sbutton),"clicked", (GtkSignalFunc) button_color_save ,NULL);
  gtk_signal_connect(GTK_OBJECT(apply_button),"clicked", (GtkSignalFunc) button_color_apply ,NULL);
  gtk_signal_connect(GTK_OBJECT(close_button),"clicked", (GtkSignalFunc) button_color_close ,(void *)colors_win);
  gtk_signal_connect(GTK_OBJECT(colors_win),"delete_event", (GtkSignalFunc) colors_win_destroy_cb ,(void *)colors_win);

  for(i=0;i<CTYPE_ROWS;i++) {
	ypos=i+1;  
	label = gtk_label_new(ctype[i].label);
	put_to_table(label,table1,ypos,1,1);
	if(ctype[i].fg>0) 
	{
		fgb = gtk_button_new_with_label ("FG");
		put_to_table(fgb,table1,ypos,2,0);
		gtk_signal_connect(GTK_OBJECT(fgb),	"clicked", (GtkSignalFunc) color_button_select_fg ,(gpointer)&ctype[i]);
	};
	if(ctype[i].bg>0 || (ctype[i].bg==0 && i==0)) {
		bgb = gtk_button_new_with_label ("BG");
		put_to_table(bgb,table1,ypos,3,0);
		gtk_signal_connect(GTK_OBJECT(bgb),	"clicked", (GtkSignalFunc) color_button_select_bg ,(gpointer)&ctype[i]);
	} else {
//		bgindex=-ctype[i].bg;
	};
	show_color_sample(table1,ypos,"text");
  };

  gtk_signal_connect (GTK_OBJECT (colors_win), "key_press_event",
                      GTK_SIGNAL_FUNC (on_wlist_key_press),
                      NULL);
 return(0); 
}

void drv_show_menu_popup(GtkWidget *popup_menu)
{
	gtk_menu_popup(GTK_MENU(popup_menu),NULL,NULL,NULL,NULL,1,0);
}


#include "gtk_common.c"
#include "menu_common.c"

/* -- */
