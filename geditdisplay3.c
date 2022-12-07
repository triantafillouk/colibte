/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

	Gtk3 display routines.
*/

#include "xe.h"
#include "geditdisplay3.h"

int x_insert ();

extern int font_weight;
extern char *current_font_name;
extern char **mono_fonts_list;
extern int mono_fonts_number;
extern int cursor_showing;

int selection_on=-1;

int newxy(WINDP *wp);
void update_from_mouse(WINDP *wp,int x,int y,int button,int reset);
void set_window_font(WINDP *wp);
void ge_set_initial_font(GtkWidget *widget);
void set_edit_font(GeEditDisplay *wd);
void upd_some_virtual_lines(WINDP *wp,char *from);

extern GtkWidget *popup_select_on, *popup_select_off, *popup_boxed_region, *popup_sort_menu,*popup_tag_menu,*popup_notes_menu;
void end_draw(GeEditDisplay *wd,char *message);

extern float CHEIGHT,CLEN,CFONTBASE,SLHEIGHT,SLBASE;
extern int CHEIGHTI;
extern int update_all;

void set_cursor(int val,char *from);
extern int pfont_index,font_size,status_font_size;

static GtkWidgetClass *parent_class = NULL;

static void ge_edit_display_class_init     (GeEditDisplayClass *klass);
static void ge_edit_display_init           (GeEditDisplay *wd);
static void ge_edit_display_realize        (GtkWidget *widget);
static void ge_edit_display_unrealize      (GtkWidget *widget);

static gint ge_edit_display_event (GtkWidget *widget, GdkEvent *event);
static gint ge_edit_draw_event (GtkWidget *widget, cairo_t *cr);

/* local definitions and uses */
void set_xmark(WINDP *smark_wp, int x,int y,int f);
int arrange_window(WINDP *wp,int mouse_y);
void move_to_new_position(WINDP *new_wp, offs, num new_line, int new_column);

void
ge_edit_display_expose_area(GtkWidget *widget, GdkRectangle *area)
{
#if	!WSL
	gtk_widget_queue_draw_area(widget,area->x,area->y,area->width,area->height);
#endif
}

GtkWidget*
ge_edit_display_new (void)
{
    return (GtkWidget *) (g_object_new(EDIT_DISPLAY_CONTAINER,NULL));
}

G_DEFINE_TYPE( GeEditDisplay, ge_edit_display, GTK_TYPE_DRAWING_AREA )

static void
ge_edit_display_class_init (GeEditDisplayClass *klass)
{
    GtkWidgetClass *widget_class;
    widget_class = (GtkWidgetClass*) klass;
    widget_class->realize = ge_edit_display_realize;
    widget_class->unrealize = ge_edit_display_unrealize;
#if	NUSE
	widget_class->focus_in_event = ge_edit_focus_in;
	widget_class->focus_out_event = ge_edit_focus_out;
    widget_class->style_set = ge_edit_display_style_set;
#endif
	widget_class->event = ge_edit_display_event;
	widget_class->draw = ge_edit_draw_event;
}


int check_cr(WINDP *wp,char *msg)
{
 GeEditDisplay *wd;
 if(wp==NULL) { 
 	// ERROR("check_cr: wp is NULL in %s!",msg);
 	return 0;
 };
 if(wp->gwp == NULL) { ERROR("check_cr: wp->gwp is NULL %s!",msg);return 0;};
 if(wp->gwp->draw == NULL) { ERROR("check_cr: wp->gwp->draw is NULL %s!",msg);return 0;};
 wd = (GeEditDisplay *)(wp->gwp->draw);
 if(wd->cr == NULL) {
	return 1;
 } else {
	return 2;
 }
}

extern int slide_flag;
extern int in_slide;

extern GdkRGBA *current_colors[COLOR_TYPES];

extern int color_scheme_ind;

extern int ppy;
extern int py;
extern int cursor_showing;

static gint
ge_edit_draw_event(GtkWidget *widget,cairo_t *cr)
{
	static int first_time=0;
	static int ind=0;
    GeEditDisplay *wd;
    GtkAllocation alloc;
	GdkRectangle clip_rect;
	WINDP *wp;
	gint RVAL=TRUE;
	int line_from=0;
	int line_to=1;
	// static int in_this=1;
	if(in_slide) return TRUE;
	wd = GTK_EDIT_DISPLAY(widget);
	
	if(wd->in_draw) {
		// MESG(" ge_edit_draw_event: already in draw, return! %d",ind++);
		return RVAL;
	};
	// MESG("ge_edit_draw_event:");
	if(wd->cr!=NULL) {
		// MESG("ge_edit_draw_event: wd->cr !NULL, end_draw and return! %d",ind++);
		gdk_window_end_draw_frame(wd->edit_window,wd->edit_gdk_context);
		wd->cr=NULL;
		return RVAL;
	};
	gdk_cairo_get_clip_rectangle(cr,&clip_rect);

	if(wd->cr!=NULL) MESG("!!!!!!!! cr not null!");

	if(clip_rect.width<3 || clip_rect.height<3) 
	{
		GdkRGBA *lcolor;
		lcolor = current_colors[COLOR_BG];
		// redraw cliped area near scroll slide!
		cairo_set_operator(cr,CAIRO_OPERATOR_OVER);
		// redraw with background color!!
		cairo_set_source_rgba(cr,lcolor->red,lcolor->green,lcolor->blue,1.0);
		cairo_rectangle(cr,	clip_rect.x, clip_rect.y,clip_rect.width,clip_rect.height);
		cairo_fill(cr);

		wd->in_draw=0;
		expose_window(wd->wp);
		// MESG(" draw_event: fill (w=%d h=%d) end small return !!, %d",clip_rect.width,clip_rect.height,ind++);
		return RVAL;
	}

	first_time++;
	if(in_slide || slide_flag){
		// MESG("in slide, no draw! in_slide=%d flag=%d return! %d",in_slide,slide_flag,ind++);
		wd->in_draw=0;
		if(wd->wp == cwp) {
			show_cursor("ge_edit_draw_event,in slide");
			show_position_info(0);
		};

		return RVAL;
	};

	wd->cr = cr;
	wd->in_draw=1;
	wp = wd->wp;

    gtk_widget_get_allocation(widget, &alloc);
	wd->edit_rect.width = alloc.width;
	wd->edit_rect.height = alloc.height +1;
	wd->edit_rect.x = 0;
	wd->edit_rect.y = 0;
	
	if(first_time==1) /* for the first time only */
	{
		ge_set_initial_font(widget);
	};
	wd->ncols = alloc.width/CLEN+1;

	wd->height = alloc.height;
	wd->nrows = wd->height/CHEIGHTI;
	wd->width = alloc.width;
	line_from=clip_rect.y/CHEIGHTI;
	line_to=(clip_rect.y+clip_rect.height)/CHEIGHTI;
	if(line_to < 4) line_to--;
	// MESG("<- ge_edit_draw_event: w=%d h=%d window %d  ind=%d l=%d - %d",clip_rect.width,clip_rect.height,wp->id,ind++,line_from,line_to);

	/* we must set the new height,width of this window and redraw */
	update_all=RVAL;

	/* find the window and refresh it */
	if(newxy(wd->wp)!=0) 
	{	/* prepare the window */
		// MESG("ge_edit_draw_event: id=%d f=%d draw_f=%d",wp->id,wp->w_flag,wp->draw_flag);

		upd_column_pos();
		// check_cursor_position(wd->wp);
		wd->wp->w_fp->line_from=line_from;
		wd->wp->w_fp->line_to=line_to;
		// MESG("update some from %d to %d",line_from,line_to);
		upd_some_virtual_lines(wd->wp,"ge_edit_draw_event:");
		// MESG("	ge_edit_draw_event: newxy, before_drawing window!");
		wd->wp->w_flag=UPD_FORCE;
		draw_window(1,wd->wp,"ge_edit_draw_event");
		wd->wp->w_flag=0;
		update_all=0;
	} else {
		wd->in_draw=0;
		// MESG("-> ge_edit_draw_event: end too small %d",ind++); 
		return RVAL;
	};

	if(wd->wp == cwp) {
		// set_update(cwp,UPD_MOVE);
		drv_move(cwp->currow, WGetCol()); 
		// MESG("ge_draw_event: move to row=%d col=%d",cwp->currow,WGetCol());
		// update_status();
		status_line(cwp);
		cursor_showing=0;	/* force show cursor  */
		// wd->cr=NULL;
		movecursor(window_cursor_line(wd->wp), WGetCol());
		// gdk_window_end_draw_frame(wd->edit_window,wd->edit_gdk_context);
		show_cursor("ge_edit_event");
			// show_position_info(0);
	} else {;
		// MESG("status line!");
		status_line(wd->wp);
	};
	wd->cr=NULL;
	wd->in_draw=0;
	// MESG("-> ge_edit_draw_event: ended window %d %d",wp->id,ind++); 
	return RVAL;
}

int on_parent_configure              (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{
	int p_width,p_height;

	gtk_window_get_size((GtkWindow *)widget,&p_width,&p_height);

	set_btval("x11_x",-1,NULL,event->x);
	set_btval("x11_y",-1,NULL,event->y);
	set_btval("x11_width",-1,NULL,p_width);
	set_btval("x11_height",-1,NULL,p_height);

//	MESG("on_parent_configure: x=%d y=%d [%d,%d]",event->x,event->y,event->width,event->height);
//	MESG("window width=%d height=%d",p_width,p_height);

  return(FALSE);
}

static gboolean mouse_wscroll (GtkWidget *widget,
       GdkEvent  *event,
       gpointer   user_data)
{
    printf("window scroll! \n");
    return TRUE;
}

static void
ge_edit_display_realize (GtkWidget *widget)
{
    GdkWindowAttr		attributes;
    gint			attributes_mask;
    GeEditDisplay		*wd;
	GdkWindow *parent_window = NULL;
	GtkWidget *parent = gtk_widget_get_parent(widget);

    g_return_if_fail(widget != NULL);
    g_return_if_fail(GTK_IS_EDIT_DISPLAY(widget));

	wd = (GeEditDisplay *)widget;

    attributes.window_type = GDK_WINDOW_CHILD;

	parent_window = gtk_widget_get_parent_window(widget);
	/* Edit window */	
    attributes.width = gtk_widget_get_allocated_width(widget);
    attributes.height = gtk_widget_get_allocated_height(widget);
	attributes.x = 0;
	attributes.y = 0;

    attributes.wclass = GDK_INPUT_OUTPUT;
    attributes.event_mask = gtk_widget_get_events(widget)
			| GDK_EXPOSURE_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_POINTER_MOTION_MASK
			| GDK_BUTTON_RELEASE_MASK
			| GDK_KEY_PRESS_MASK
			| GDK_SCROLL_MASK
#if	!DARWIN
			| GDK_SMOOTH_SCROLL_MASK
#endif
			;
    attributes.visual = gtk_widget_get_visual (parent);
    attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL |GDK_WA_TYPE_HINT;

	wd->edit_window = gdk_window_new(parent_window, &attributes, attributes_mask);
	gtk_widget_set_window(widget,wd->edit_window);

	gtk_widget_set_has_window (widget,TRUE);
	gdk_window_set_user_data(wd->edit_window,widget);
	gtk_widget_set_app_paintable(widget,TRUE);
	gtk_widget_set_can_focus (widget,TRUE);
	gtk_widget_set_realized(widget,TRUE);
	wd->ge_font_desc=NULL;

	// MESG("ge_edit_display_realize: realized!!");
	// gtk_widget_add_events(GTK_WIDGET(widget),GDK_SCROLL_MASK);
	// g_signal_connect(widget, "scroll-event", G_CALLBACK(mouse_wscroll), NULL);
}

static void
ge_edit_display_unrealize (GtkWidget *widget)
{
    GeEditDisplay *wd;
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_EDIT_DISPLAY (widget));

    wd = GTK_EDIT_DISPLAY(widget);
	// MESG("unrealize!!");
	if(gtk_widget_get_mapped(widget)) gtk_widget_unmap(widget);

	if(wd->ge_font_desc !=NULL ) pango_font_description_free (wd->ge_font_desc);
	if(wd->layout != NULL) g_object_unref(wd->layout);
	// MESG("ge_edit_display_unrealize: free layout!");

	/* destroy child window */
    if (wd->edit_window)
    {
		gdk_window_set_user_data(wd->edit_window, NULL);
	
		cairo_destroy(wd->cr);
		gdk_window_destroy (wd->edit_window);
		wd->edit_window = NULL;
    }
}

cairo_t *begin_draw(GeEditDisplay *wd,cairo_region_t *region,char *from)
{
 unsigned long lwd;
 lwd = (unsigned long) (wd->edit_window);
 	// wd->act=0;
#if	NUSE
	if(wd->in_draw) { 
//		MESG("	in draw, return cr! from %s",from);
		return wd->cr;
	}
#endif
	if(wd->cr !=NULL) {
//		MESG("	cr not null, return cr! from %s",from);
		return wd->cr;
	};
	
	// MESG("begin_draw: from %s",from);
	wd->edit_gdk_context = gdk_window_begin_draw_frame (wd->edit_window,region);
	wd->cr = gdk_drawing_context_get_cairo_context (wd->edit_gdk_context);
	wd->region=region;

	if(wd->layout==NULL) {
		MESG("begin_draw: new layout!");
		wd->layout = pango_cairo_create_layout (wd->cr);
	};
	return wd->cr;
}

void end_draw(GeEditDisplay *wd,char *from)
{
 unsigned long lwd;
 lwd = (unsigned long) (wd->edit_window);
	if(wd->in_draw) { 
		// MESG("end_draw: from[%s], still in draw!",from);
		return;
	};
	// MESG("end_draw: finish from %s",from);

	if(wd->region) {
		// MESG("end_draw: call queue_draw_region");
		gtk_widget_queue_draw_region ((GtkWidget *)wd->wp->gwp->draw,wd->region);
		// MESG("end_draw: after queue_draw_region");
	};

	if(wd->cr!=NULL){
		gdk_window_end_draw_frame(wd->edit_window,wd->edit_gdk_context);
	};
	wd->cr=NULL;
	wd->region=NULL;
	wd->act=1;
}

void ge_set_initial_font(GtkWidget *widget)
{
	/* find status line height */
	/* find fonts monospaced for the editor */
	GeEditDisplay *wd = GTK_EDIT_DISPLAY(widget);
	cairo_region_t *region;
	cairo_text_extents_t te;
	int i,j;
	char *st="0123456789";
#ifdef	DARWIN
		current_font_name="Menlo";
#else
		current_font_name="Monospace";
#endif	
		font_size=14;
		status_font_size=14;

		region = gdk_window_get_clip_region (wd->edit_window);
		// MESG("ge_set_initial_font:");
		set_edit_font(wd);

		/* create list of monospace fonts  */
		PangoContext *pc;
		PangoFontFamily **families;
		int mono=0,n_families=0;
		pc = gtk_widget_get_pango_context (widget);
		pango_context_list_families(pc, &families, &n_families);
		mono_fonts_list = (char **)malloc(sizeof(char *)*(n_families+1));
		mono_fonts_number = 0;
		for(i=0,j=0;i<n_families;i++) {
			mono = pango_font_family_is_monospace(families[i]);
			if(mono) {
				mono_fonts_list[j] = strdup(pango_font_family_get_name(families[i]));
				j++;
			};
		};
		mono_fonts_list[j]=NULL;
		mono_fonts_number=j;
		/* create list of monospace fonts end */
}

static void
ge_edit_display_init (GeEditDisplay *wd)
{
 gtk_widget_set_can_focus ((GtkWidget *)wd,TRUE);

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
 wd->in_draw=0;
// wd->configured=0;
 wd->id=0;
}

#include "gedit_common.c"

/* -- */
