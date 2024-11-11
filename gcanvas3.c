/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

	Plot canvas routines in gtk3
*/

#include "xe.h"
#include "gcanvas3.h"

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
static gint ge_plot_area_draw(GtkWidget *widget,cairo_t *cr);
static void ge_plot_area_style_set      (GtkWidget *widget, GtkStyle *previous_style);

GtkWidget*
ge_plot_area_new (void)
{
    return (GtkWidget *) (g_object_new(KT_PLOT_AREA_CONTAINER,NULL));
}


G_DEFINE_TYPE( GePlotArea, ge_plot_area, GTK_TYPE_DRAWING_AREA );

static void
ge_plot_area_class_init (GePlotAreaClass *klass)
{
    GtkWidgetClass *widget_class;

    widget_class = (GtkWidgetClass*) klass;
    widget_class->realize = ge_plot_area_realize;
    widget_class->unrealize = ge_plot_area_unrealize;
    widget_class->draw = ge_plot_area_draw;
    widget_class->style_set = ge_plot_area_style_set;
}



static void
ge_plot_area_realize (GtkWidget *widget)
{
    GdkWindowAttr		attributes;
    gint			attributes_mask;
    GePlotArea		*wd;
	GdkWindow *parent_window = NULL;
	GtkWidget *parent = gtk_widget_get_parent(widget);

	GdkDisplay	*default_display = gdk_display_get_default();

    g_return_if_fail(widget != NULL);
    g_return_if_fail(KT_IS_PLOT_AREA(widget));

    wd = KT_PLOT_AREA(widget);

	gtk_widget_set_realized(widget,TRUE);
    attributes.window_type = GDK_WINDOW_CHILD;
	parent_window = gtk_widget_get_parent_window(widget);

    attributes.x = 0;
    attributes.y = 0;

    attributes.width = gtk_widget_get_allocated_width(widget);
    attributes.height = gtk_widget_get_allocated_height(widget);

    attributes.wclass = GDK_INPUT_OUTPUT;
    attributes.event_mask = gtk_widget_get_events(widget)
			| GDK_EXPOSURE_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_POINTER_MOTION_MASK
			| GDK_KEY_PRESS_MASK
			| GDK_BUTTON_RELEASE_MASK;
    attributes.visual = gtk_widget_get_visual (parent);
    attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL;

	wd->plot_window = gdk_window_new(parent_window, &attributes, attributes_mask);
	gtk_widget_set_window(widget,wd->plot_window);

	pcursor = gdk_cursor_new_for_display(default_display,GDK_CROSSHAIR);
	gdk_window_set_cursor(wd->plot_window, pcursor );
	/* Edit window */
	gtk_widget_set_has_window (widget,TRUE);
	gdk_window_set_user_data(wd->plot_window,widget);
	gtk_widget_set_app_paintable(widget,TRUE);

}

static void
ge_plot_area_unrealize (GtkWidget *widget)
{
    GePlotArea *wd;

    g_return_if_fail (widget != NULL);
    g_return_if_fail (KT_IS_PLOT_AREA (widget));

    wd = KT_PLOT_AREA(widget);
	if(gtk_widget_get_mapped(widget)) gtk_widget_unmap(widget);

}

static void
ge_plot_area_init (GePlotArea *wd)
{
 gtk_widget_set_can_focus ((GtkWidget *)wd,TRUE);

 wd->width=0;
 wd->height=0;
 wd->rect.x = wd->rect.y =0;
 wd->rect.width = wd->rect.height =0;
}

static gint
ge_plot_area_draw(GtkWidget *widget,cairo_t *cr)
{
	static int first_time=0;
    GtkAllocation allocation;
	GdkRectangle clip_rect;
	GePlotArea *wd;

	wd = (GePlotArea *)widget;	
	gdk_cairo_get_clip_rectangle(cr,&clip_rect);

    gtk_widget_get_allocation(widget, &allocation);
	plot_reset(allocation.width,allocation.height);

	pl_clear();
	plot_redraw();

	return TRUE;
}

// set style while realize the widget!
static void
ge_plot_area_style_set (GtkWidget *widget, GtkStyle *previous_style)
{
//	MESG("ge_plot_area_style_set: dymmy! ");
}
