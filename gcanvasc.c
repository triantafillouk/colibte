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

#include "gcanvasc.h"

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
    GtkWidgetClass *widget_class;

    widget_class = (GtkWidgetClass*) klass;
    parent_class = gtk_type_class (gtk_widget_get_type());

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

    g_return_if_fail(widget != NULL);
    g_return_if_fail(KT_IS_PLOT_AREA(widget));

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
			| GDK_POINTER_MOTION_MASK;
//			| GDK_KEY_PRESS_MASK
//			| GDK_BUTTON_RELEASE_MASK;
    attributes.visual = gtk_widget_get_visual (widget);
    attributes.colormap = gtk_widget_get_colormap (widget);
	
    attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

    widget->window = gdk_window_new (gtk_widget_get_parent_window(widget),
				     &attributes, attributes_mask);

	gdk_window_set_user_data (widget->window, widget);
	pcursor = gdk_cursor_new(GDK_CROSSHAIR);
	gdk_window_set_cursor(widget->window, pcursor );
	/* Edit window */

    widget->style = gtk_style_attach(widget->style, widget->window);
}

static void
ge_plot_area_unrealize (GtkWidget *widget)
{
    g_return_if_fail (widget != NULL);
    g_return_if_fail (KT_IS_PLOT_AREA (widget));

	if(GTK_WIDGET_MAPPED(widget)) gtk_widget_unmap(widget);
	GTK_WIDGET_UNSET_FLAGS (widget, GTK_MAPPED);

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
	};
	plot_reset(allocation->width,allocation->height);

	/* we must set the new height,width of this window and redraw */
}

static void
ge_plot_area_init (GePlotArea *wd)
{
 GTK_WIDGET_SET_FLAGS (GTK_WIDGET(wd), GTK_CAN_FOCUS);

 wd->rect.x = wd->rect.y =0;
 wd->rect.width = wd->rect.height =0;
}


static void
ge_plot_area_style_set (GtkWidget *widget, GtkStyle *previous_style)
{
//	MESG("ge_plot_area_style_set ");
}

/* -- */
