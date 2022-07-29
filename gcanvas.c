/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

	Plot canvas routines in gtk/gdk
*/


#include "xe.h"
#include <gtk/gtk.h>
#include	<gdk/gdkx.h>
#include <stdio.h>
#if	IMLIB2
#include	<X11/Xlib.h>
#include	<Imlib2.h>
#endif
#include "gcanvas.h"

extern int plot_sync;
extern int redrawing_on;
extern int plot_update;
void plot_reset(int,int);
GdkCursor *pcursor;

static GtkWidgetClass *parent_class = NULL;

static void ge_plot_area_class_init     (GePlotAreaClass *klass);
static void ge_plot_area_init           (GePlotArea *wd);
static void ge_plot_area_realize        (GtkWidget *widget);
static void ge_plot_area_unrealize      (GtkWidget *widget);
void ge_plot_area_size_allocate  (GtkWidget *widget, GtkAllocation *allocation);

static gint ge_plot_area_expose         (GtkWidget *widget, GdkEventExpose *event);
void ge_plot_area_expose_area    (GtkWidget *widget, GdkRectangle *area);
static void ge_plot_area_style_set      (GtkWidget *widget, GtkStyle *previous_style);

GtkWidget*
ge_plot_area_new (void)
{
    return GTK_WIDGET (gtk_type_new(ge_plot_area_get_type()));
}

GtkType
ge_plot_area_get_type (void)
{
    static GtkType plot_area_type = 0;
    if (!plot_area_type)
    {
	static const GtkTypeInfo plot_area_info =
	{
	    "GePlotArea",
	    sizeof(GePlotArea),
	    sizeof(GePlotAreaClass),
	    (GtkClassInitFunc) ge_plot_area_class_init,
	    (GtkObjectInitFunc) ge_plot_area_init,
		NULL,NULL,NULL,
	};

	plot_area_type = gtk_type_unique(gtk_widget_get_type(), &plot_area_info);
	
    }
    return plot_area_type;
}


static void
ge_plot_area_class_init (GePlotAreaClass *klass)
{
    GtkObjectClass *object_class;
    GtkWidgetClass *widget_class;

    object_class = (GtkObjectClass*) klass;
    widget_class = (GtkWidgetClass*) klass;
    parent_class = gtk_type_class (gtk_widget_get_type());

    widget_class->expose_event = ge_plot_area_expose;
    widget_class->realize = ge_plot_area_realize;
    widget_class->unrealize = ge_plot_area_unrealize;
    widget_class->size_allocate = ge_plot_area_size_allocate;
    widget_class->style_set = ge_plot_area_style_set;
}



static void
ge_plot_area_realize (GtkWidget *widget)
{
    GdkWindowAttr		attributes;
    gint			attributes_mask;
    GePlotArea		*wd;

    g_return_if_fail(widget != NULL);
    g_return_if_fail(KT_IS_PLOT_AREA(widget));

    wd = KT_PLOT_AREA(widget);

    GTK_WIDGET_SET_FLAGS(widget, GTK_REALIZED);

    attributes.window_type = GDK_WINDOW_CHILD;
    attributes.x = widget->allocation.x;
    attributes.y = widget->allocation.y;
    attributes.width = widget->allocation.width;
    attributes.height = widget->allocation.height;

    attributes.wclass = GDK_INPUT_OUTPUT;
    attributes.event_mask = gtk_widget_get_events(widget)
			| GDK_EXPOSURE_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_POINTER_MOTION_MASK
//			| GDK_KEY_PRESS_MASK
			| GDK_BUTTON_RELEASE_MASK;
    attributes.visual = gtk_widget_get_visual (widget);
    attributes.colormap = gtk_widget_get_colormap (widget);
	
    attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

    widget->window = gdk_window_new (gtk_widget_get_parent_window(widget),
				     &attributes, attributes_mask);

	gdk_window_set_user_data (widget->window, widget);
	pcursor = gdk_cursor_new(GDK_CROSSHAIR);
	gdk_window_set_cursor(widget->window, pcursor );
	/* Edit window */

	wd->ffgc = gdk_gc_new(widget->window);
  	wd->bpgc = gdk_gc_new(widget->window);
  	wd->pgc = gdk_gc_new(widget->window);
	
#if	IMLIB2
	wd->up = NULL;
#endif
    widget->style = gtk_style_attach(widget->style, widget->window);
}

static void
ge_plot_area_unrealize (GtkWidget *widget)
{
    GePlotArea *wd;
    g_return_if_fail (widget != NULL);
    g_return_if_fail (KT_IS_PLOT_AREA (widget));

    wd = KT_PLOT_AREA(widget);

	if(GTK_WIDGET_MAPPED(widget)) gtk_widget_unmap(widget);
	GTK_WIDGET_UNSET_FLAGS (widget, GTK_MAPPED);
	gdk_gc_destroy(wd->ffgc);
	gdk_gc_destroy(wd->bpgc);
	gdk_gc_destroy(wd->pgc);
	if (wd->pixmap) gdk_pixmap_unref(wd->pixmap);
#if	IMLIB2
	if(wd->im_canvas) {
		imlib_context_set_image(wd->im_canvas);
		imlib_free_image();
	};
#endif	
    if (GTK_WIDGET_CLASS (parent_class)->unrealize)
	(* GTK_WIDGET_CLASS (parent_class)->unrealize) (widget);
}

void
ge_plot_area_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
{
    GePlotArea *wd;
	plot_sync=0;
    g_return_if_fail(widget != NULL);
    g_return_if_fail(KT_IS_PLOT_AREA (widget));
    g_return_if_fail(allocation != NULL);

	if(allocation->height< 10)  allocation->height=10;
	if(allocation->width< 10)  allocation->width=10;
    widget->allocation = *allocation;
	wd = KT_PLOT_AREA(widget);

	wd->rect.width = allocation->width;
	wd->rect.height = allocation->height;
	wd->rect.x = 0;
	wd->rect.y = 0;

    if (GTK_WIDGET_REALIZED(widget))
	{
	gdk_window_move_resize (widget->window,
  				allocation->x, allocation->y,
				allocation->width, allocation->height);

	gdk_window_clear(widget->window);

	/* free backing pixmap we already had */
	/* we should check dimensions and reallocate if we have a bigger window */
	if(plot_sync==2) {
		usleep(10);
	};
	if (wd->pixmap) gdk_pixmap_unref(wd->pixmap);
#if	IMLIB2
	if(wd->im_canvas) {
		imlib_context_set_image(wd->im_canvas);
		imlib_free_image();
	};
	wd->im_canvas = imlib_create_image(allocation->width,allocation->height);
#endif	

	/* get backing pixmap to draw into */
	wd->pixmap = gdk_pixmap_new(widget->window, allocation->width,
				    allocation->height, -1);

#if	IMLIB2
	imlib_context_set_drawable(GDK_WINDOW_XWINDOW(KT_PLOT_AREA(wd)->pixmap));
#endif
	};
	plot_reset(allocation->width,allocation->height);
	pl_clear();
	plot_redraw();

	/* we must set the new height,width of this window and redraw */
}

static void
ge_plot_area_init (GePlotArea *wd)
{
 GTK_WIDGET_SET_FLAGS (GTK_WIDGET(wd), GTK_CAN_FOCUS);

 wd->width=0;
 wd->height=0;
 wd->rect.x = wd->rect.y =0;
 wd->rect.width = wd->rect.height =0;
}

static gint
ge_plot_area_expose(GtkWidget *widget, GdkEventExpose *event)
{
    GePlotArea *wd = KT_PLOT_AREA(widget);
	if(redrawing_on) return FALSE;
	if(plot_update>0) return FALSE;	// no need to expose

	plot_sync=0;
    if (event->window == widget->window)
    {
		if(wd->pixmap == NULL) {
			ERROR("ge_plot_area_expose: pixmap is null!");
			return FALSE;
		};
		if(event->area.width==0 || event->area.height==0) return FALSE;

	    gdk_draw_pixmap(widget->window,
			    widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
			    wd->pixmap, event->area.x, event->area.y,
			    event->area.x, event->area.y,
			    event->area.width, event->area.height);
    }
    return FALSE;
}

void
ge_plot_area_expose_area(GtkWidget *widget, GdkRectangle *area)
{
    GdkEventExpose event;

    event.type = GDK_EXPOSE;
    event.window = widget->window;
    event.send_event = TRUE;
    event.area = *area;
    event.count = 0;

    ge_plot_area_expose(widget, &event);
}

static void
ge_plot_area_style_set (GtkWidget *widget, GtkStyle *previous_style)
{
//	MESG("ge_plot_area_style_set ");
}
