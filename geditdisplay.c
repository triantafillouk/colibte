/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

	Gtk display routines.
*/

#include "xe.h"
#include "geditdisplay.h"

int x_insert ();

extern int font_weight;
extern char *current_font_name;
extern char **mono_fonts_list;
extern int mono_fonts_number;
extern int color_scheme_ind;

int selection_on=-1;

int newxy(WINDP *wp);
void update_from_mouse(WINDP *wp,int x,int y,int button,int reset);
void set_window_font(WINDP *wp);
void upd_all_wrap_lines(WINDP *wp,char *from);

extern GtkWidget *popup_select_on, *popup_select_off, *popup_boxed_region, *popup_sort_menu,*popup_tag_menu,*popup_notes_menu;

extern float CHEIGHT,CLEN,CFONTBASE,SLBASE;
extern int CHEIGHTI;
extern int update_all;

extern int font_index,font_size;
void set_cursor(int val,char *from);

static GtkWidgetClass *parent_class = NULL;

static void ge_edit_display_class_init     (GeEditDisplayClass *klass);
static void ge_edit_display_init           (GeEditDisplay *wd);
static void ge_edit_display_realize        (GtkWidget *widget);
static void ge_edit_display_unrealize      (GtkWidget *widget);
void ge_edit_display_size_allocate  (GtkWidget *widget, GtkAllocation *allocation);

static gint ge_edit_display_expose         (GtkWidget *widget, GdkEventExpose *event);
void ge_edit_display_expose_area    (GtkWidget *widget, GdkRectangle *area);

static int ge_edit_focus_in		(GtkWidget *widget, GdkEventFocus *event);
static int ge_edit_focus_out	(GtkWidget *widget, GdkEventFocus *event);
static void ge_edit_display_draw_focus		(GtkWidget *widget);
static gint ge_edit_display_event (GtkWidget *widget, GdkEvent *event);

/* local definitions and uses */
void set_xmark(WINDP *smark_wp, int x,int y,int f);
int arrange_window(WINDP *wp,int mouse_y);
void move_to_new_position(WINDP *new_wp, offs, num new_line, int new_column);

GtkWidget*
ge_edit_display_new (void)
{
    return GTK_WIDGET (gtk_type_new(ge_edit_display_get_type()));
}

GtkType
ge_edit_display_get_type (void)
{
    static GtkType edit_display_type = 0;
    if (!edit_display_type)
    {
	static const GtkTypeInfo edit_display_info =
	{
	    "GeEditDisplay",
	    sizeof(GeEditDisplay),
	    sizeof(GeEditDisplayClass),
	    (GtkClassInitFunc) ge_edit_display_class_init,
	    (GtkObjectInitFunc) ge_edit_display_init,
		(GtkClassInitFunc) NULL,
	};

	edit_display_type = gtk_type_unique(gtk_widget_get_type(), &edit_display_info);
	
    }
    return edit_display_type;
}

static void
ge_edit_display_class_init (GeEditDisplayClass *klass)
{
    GtkWidgetClass *widget_class;
    widget_class = (GtkWidgetClass*) klass;
    parent_class = gtk_type_class (gtk_widget_get_type());

    widget_class->expose_event = ge_edit_display_expose;
    widget_class->realize = ge_edit_display_realize;
    widget_class->unrealize = ge_edit_display_unrealize;
    widget_class->size_allocate = ge_edit_display_size_allocate;
//    widget_class->style_set = ge_edit_display_style_set;
	widget_class->event = ge_edit_display_event;
	widget_class->focus_in_event = ge_edit_focus_in;
	widget_class->focus_out_event = ge_edit_focus_out;
}




int on_parent_configure              (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{
  int p_width,p_height;

  gtk_window_get_size((GtkWindow *)widget,&p_width,&p_height);
#if	USE_GLIB0
	set_cfg_int("x11_width",p_width);
	set_cfg_int("x11_height",p_height);
	set_cfg_int("x11_x",event->x);
	set_cfg_int("x11_y",event->y);
#else
	set_bt_num_val("x11_x",event->x);
	set_bt_num_val("x11_y",event->y);
	set_bt_num_val("x11_width",p_width);
	set_bt_num_val("x11_height",p_height);
#endif
//	MESG("on_parent_configure: [%d,%d]",event->width,event->height);
  return(FALSE);
}

static void
ge_edit_display_realize (GtkWidget *widget)
{
    GdkWindowAttr		attributes;
    gint			attributes_mask;
    GeEditDisplay		*wd;

    g_return_if_fail(widget != NULL);
    g_return_if_fail(GTK_IS_EDIT_DISPLAY(widget));

    wd = GTK_EDIT_DISPLAY(widget);

    GTK_WIDGET_SET_FLAGS(widget, GTK_REALIZED);

    attributes.window_type = GDK_WINDOW_CHILD;
    attributes.x = widget->allocation.x;
    attributes.y = widget->allocation.y;
    attributes.width = widget->allocation.width;
    attributes.height = widget->allocation.height;
//	MESG("realize: w=%d",attributes.width);
    attributes.wclass = GDK_INPUT_OUTPUT;
    attributes.event_mask = gtk_widget_get_events(widget)
			| GDK_EXPOSURE_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_POINTER_MOTION_MASK
			| GDK_BUTTON_RELEASE_MASK
			| GDK_KEY_PRESS_MASK;
    attributes.visual = gtk_widget_get_visual (widget);
    attributes.colormap = gtk_widget_get_colormap (widget);
	
    attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

    widget->window = gdk_window_new (gtk_widget_get_parent_window(widget),
				     &attributes, attributes_mask);

	gdk_window_set_user_data (widget->window, widget);

	/* Edit window */
	
    attributes.window_type = GDK_WINDOW_CHILD;
    attributes.x = wd->edit_rect.x;
    attributes.y = wd->edit_rect.y;
    attributes.width = wd->edit_rect.width;
    attributes.height = wd->edit_rect.height;
    attributes.wclass = GDK_INPUT_OUTPUT;

	/* the following does not set all events ! */
	/* More precisely the motion notify is not handled well! */
    attributes.event_mask = gtk_widget_get_events(widget)
			| GDK_EXPOSURE_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_POINTER_MOTION_MASK
			| GDK_BUTTON_RELEASE_MASK;

    attributes.visual = gtk_widget_get_visual (widget);
    attributes.colormap = gtk_widget_get_colormap (widget);
    attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

	wd->edit_window = gdk_window_new(widget->window, &attributes, attributes_mask);
	gdk_window_set_user_data(wd->edit_window,widget);

	gdk_window_show(wd->edit_window);

	wd->fgc = gdk_gc_new(wd->edit_window);
  	wd->bgc = gdk_gc_new(wd->edit_window);

    widget->style = gtk_style_attach(widget->style, widget->window);
	wd->ge_font_desc=NULL;
	wd->layout=NULL;
}

static void
ge_edit_display_unrealize (GtkWidget *widget)
{
    GeEditDisplay *wd;

    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_EDIT_DISPLAY (widget));

    wd = GTK_EDIT_DISPLAY(widget);

	if(GTK_WIDGET_MAPPED(widget)) gtk_widget_unmap(widget);
	GTK_WIDGET_UNSET_FLAGS (widget, GTK_MAPPED);


	if(wd->ge_font_desc !=NULL ) pango_font_description_free (wd->ge_font_desc);
	if(wd->layout != NULL) g_object_unref(wd->layout);

	/* destroy child window */
    if (wd->edit_window)
    {
	gdk_window_set_user_data(wd->edit_window, NULL);

	cairo_destroy(wd->cr);
	gdk_window_destroy (wd->edit_window);
	wd->edit_window = NULL;
    }
    if (GTK_WIDGET_CLASS (parent_class)->unrealize)
	(* GTK_WIDGET_CLASS (parent_class)->unrealize) (widget);
}

void
ge_edit_display_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
{
	static int first_time=1;
    GeEditDisplay *wd;

    g_return_if_fail(widget != NULL);
    g_return_if_fail(GTK_IS_EDIT_DISPLAY (widget));
    g_return_if_fail(allocation != NULL);
//	MESG("ge_edit_display_size_allocate: w=%d h=%d f=%d",allocation->width,allocation->height,first_time);
//	if(allocation->width>2000) allocation->width=1200;
	set_cursor(0,"ge_edit_display_size_allocate");
	
	if (allocation->height< 2*CHEIGHTI) allocation->height=CHEIGHTI;
    widget->allocation = *allocation;
	wd = GTK_EDIT_DISPLAY(widget);


	if(first_time) // for the first time only
	{ 
		/* find status line height */
		/* find fonts monospaced for the editor */
		GdkPixmap	*test_pixmap;
		cairo_t *cr;
		int i,j;
#if	DARWIN
		current_font_name="Menlo";
#else
		current_font_name="Monospace";
#endif	
		font_size=18;
		first_time=0;
		test_pixmap = gdk_pixmap_new(wd->edit_window, 10,10, -1);
		cr = gdk_cairo_create(test_pixmap);

		wd->sl_base = SLBASE;

		/* create list of monospace fonts  */
		PangoContext *pc;
		PangoFontFamily **families;
		int mono=0,n_families=0;
		pc = gtk_widget_get_pango_context (widget);
		pango_context_list_families(pc, &families, &n_families);
		mono_fonts_list = (char **)malloc(sizeof(char *)*(n_families+1));
		mono_fonts_number = n_families;
		for(i=0,j=0;i<n_families;i++) {
			mono = pango_font_family_is_monospace(families[i]);
			if(mono) {
				mono_fonts_list[j] = strdup(pango_font_family_get_name(families[i]));
				// MESG("		mono font [%s]",mono_fonts_list[j]);
				j++;
			};
		};
		mono_fonts_list[j]=NULL;
		mono_fonts_number=j;
		gdk_pixmap_unref(test_pixmap);
		cairo_destroy(cr);
	};
	wd->edit_rect.width = allocation->width;
	wd->edit_rect.height = allocation->height;
	wd->edit_rect.x = 0;
	wd->edit_rect.y = 0;

    if (GTK_WIDGET_REALIZED(widget))
	{
	gdk_window_move_resize (widget->window,
  				allocation->x, allocation->y,
				allocation->width, allocation->height);

	gdk_window_move_resize (wd->edit_window, // wd->edit->window
 				wd->edit_rect.x,
				wd->edit_rect.y,
				wd->edit_rect.width,
				wd->edit_rect.height);

	/* free backing pixmap we already had */
	/* we should check dimensions and reallocate if we have a bigger window */
	if (wd->edit_pixmap) {
		cairo_destroy(wd->cr);
		gdk_pixmap_unref(wd->edit_pixmap);
	};
	change_color_scheme(color_scheme_ind+1);
	drv_update_styles();
	/* get backing pixmap to draw into */
	wd->edit_pixmap = gdk_pixmap_new(wd->edit_window, allocation->width,
				    allocation->height, -1);

	wd->cr = gdk_cairo_create(wd->edit_pixmap);

	cairo_set_source_rgb(wd->cr,1,0.5,0);	// find default color and clear!!!
	cairo_rectangle(wd->cr,0,0,allocation->width,allocation->height);
	cairo_clip(wd->cr);
	cairo_fill(wd->cr);

	/* find status line height  */
	// we must set here also the default font !!!!
	set_window_font(wd->wp);

	wd->ncols = allocation->width/CLEN+1;
	wd->height = allocation->height;
	wd->nrows = wd->height/CHEIGHTI;
	wd->width = allocation->width;
	} else { 
	};
	/* we must set the new height,width of this window and redraw */
	// if(CHEIGHTI>0) 
	{
		if(allocation->width==0 || allocation->height==0) return;
	
	/* find the window and refresh it */

	if(newxy(wd->wp)!=0) 
	{	/* prepare the window */
//		MESG("newxy prepare window:");
			update_all=true;

			if(cwp==NULL) {
				return;
			};
			if(cwp->w_fp==NULL) {
				return; /* no window yet! */
			}

			upd_column_pos();
			check_cursor_position(wd->wp);
			if(is_wrap_text(wd->wp->w_fp)) 
			upd_all_wrap_lines(wd->wp,"cb_set_position 3");
			else
			upd_all_virtual_lines(wd->wp,"gedit_display_size_allocate");
			draw_window(1,wd->wp,"gedit_display_size_allocate");
			update_all=0;
			set_cursor(1,"end size allocate!");	// ????? it works !!
	};
	};
}

void end_draw(GeEditDisplay *wd,char *message)
{
/* Dummy function for GTK2 driver */
}

static void
ge_edit_display_init (GeEditDisplay *wd)
{
 GTK_WIDGET_SET_FLAGS (GTK_WIDGET(wd), GTK_CAN_FOCUS);

 wd->edit_window = NULL;
 wd->wp	= NULL;
 wd->start_line=0;
 wd->nrows=0;
 wd->ncols=0;
 wd->last_complete=0;
 wd->width=0;
 wd->height=0;
 wd->edit_rect.x = wd->edit_rect.y =0;
 wd->edit_rect.width = wd->edit_rect.height =0;
 wd->wp = cwp;
}

static gint
ge_edit_display_expose(GtkWidget *widget, GdkEventExpose *event)
{
    GeEditDisplay *wd = GTK_EDIT_DISPLAY(widget);
    if (event->window == wd->edit_window)
    {
		if(wd->edit_pixmap == NULL) return FALSE;
		if(event->area.width==0 || event->area.height==0) return FALSE;

	    gdk_draw_pixmap(wd->edit_window,
			    widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
			    wd->edit_pixmap, event->area.x, event->area.y,
			    event->area.x, event->area.y,
			    event->area.width, event->area.height);
    }
    return FALSE;
}


void
ge_edit_display_expose_area(GtkWidget *widget, GdkRectangle *area)
{
    GdkEventExpose event;
    GeEditDisplay *wd = GTK_EDIT_DISPLAY(widget);
    event.type = GDK_EXPOSE;
    event.window = wd->edit_window;
    event.send_event = TRUE;
    event.area = *area;
    event.count = 0;
	
    ge_edit_display_expose(widget, &event);
}

static void
ge_edit_display_draw_focus (GtkWidget *widget)
{
#if	NUSE
 GdkRectangle rect;
 GeEditDisplay *wd;
 // MESG("ge_edit_display_draw_focus:");
 g_return_if_fail(widget !=NULL);
 g_return_if_fail(GTK_IS_EDIT_DISPLAY(widget));

 wd = GTK_EDIT_DISPLAY(widget);
 rect.x =0;
 rect.y =0;
 rect.width = widget->allocation.width;
 rect.height = widget->allocation.height;
 if(GTK_WIDGET_DRAWABLE(wd)) {
 	ge_edit_display_expose_area(widget,&rect);
 }
#endif
}

static int
ge_edit_focus_in (GtkWidget *widget, GdkEventFocus *event)
{
 g_return_val_if_fail(widget !=NULL,FALSE);
 g_return_val_if_fail(GTK_IS_EDIT_DISPLAY(widget),FALSE);

 GTK_WIDGET_SET_FLAGS(widget, GTK_HAS_FOCUS);
 ge_edit_display_draw_focus(widget);
 return(FALSE);
}

static int
ge_edit_focus_out (GtkWidget *widget, GdkEventFocus *event)
{
 g_return_val_if_fail(widget !=NULL,FALSE);
 g_return_val_if_fail(GTK_IS_EDIT_DISPLAY(widget),FALSE);

 GTK_WIDGET_UNSET_FLAGS(widget, GTK_HAS_FOCUS);
 ge_edit_display_draw_focus(widget);
 return(FALSE);
}

#include "gedit_common.c"

/* -- */
