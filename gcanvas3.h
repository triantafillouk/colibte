/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

	Plot canvas routines in gtk3
*/
#include <gtk/gtk.h>

#define KT_PLOT_AREA_CONTAINER       (ge_plot_area_get_type ())
#define KT_PLOT_AREA(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), KT_PLOT_AREA_CONTAINER, GePlotArea))
#define KT_PLOT_AREA_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), KT_PLOT_AREA_CONTAINER, GePlotAreaClass))
#define KT_IS_PLOT_AREA(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KT_PLOT_AREA_CONTAINER))
#define KT_IS_PLOT_AREA_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), KT_PLOT_AREA_CONTAINER))
#define KT_PLOT_AREA_GET_CLASS(obj)	 (G_TYPE_INSTANCE_GET_CLASS ((obj), KT_PLOT_AREA_CONTAINER, GePlotAreaClass))

typedef struct _GePlotArea GePlotArea;
typedef struct _GePlotAreaClass GePlotAreaClass;

struct _GePlotAreaClass
{
	GtkVBoxClass parent_class;
	GdkColor	bgcolor;
	GdkColor	fgcolor;
};

struct _GePlotArea
{
    GtkWidget	widget;
	GdkRectangle rect;
	cairo_t *cr;
	guint in_draw;
	
    /* display */
	GdkWindow 	*plot_window;
	GdkDrawingContext *plot_gdk_context;
	GdkRGBA fgcolor,bgcolor;

	guint width;
	guint height;
	guint id;
};

GtkWidget	*ge_plot_area_new(void);
GType		ge_plot_area_get_type(void);

