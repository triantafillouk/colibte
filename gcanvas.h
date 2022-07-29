/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

	Plot canvas routines in gtk/gdk
*/

#include <gdk/gdk.h>
#include <gtk/gtk.h>

#define KT_TYPE_PLOT_AREA            (ge_plot_area_get_type ())
#define KT_PLOT_AREA(obj)            (GTK_CHECK_CAST ((obj), KT_TYPE_PLOT_AREA, GePlotArea))
#define KT_PLOT_AREA_CLASS(klass)    (GTK_CHECK_CLASS_CAST ((klass), KT_TYPE_PLOT_AREA, GePlotAreaClass))
#define KT_IS_PLOT_AREA(obj)         (GTK_CHECK_TYPE ((obj), KT_TYPE_PLOT_AREA))
#define KT_IS_PLOT_AREA_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass), KT_TYPE_PLOT_AREA))


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

    /* display */
    GdkPixmap	*pixmap;
    GdkGC	*ffgc;	/* font gc */
	GdkGC	*bpgc;	/* backgound color */
	GdkGC	*pgc;	/* plot gc */
	GdkColor	fgcolor, bgcolor;
#if	IMLIB2
	Imlib_Updates up;
	Imlib_Image im_canvas;
#endif
	guint width;
	guint height;
};

GtkWidget	*ge_plot_area_new(void);
GtkType		ge_plot_area_get_type(void);

