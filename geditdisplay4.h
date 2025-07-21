/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.
*/

#include <gdk/gdk.h>
#include <gtk/gtk.h>

gboolean
on_dra0_key_press_event        (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data);

#define EDIT_DISPLAY_CONTAINER            (ge_edit_display_get_type ())
#define GTK_EDIT_DISPLAY(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EDIT_DISPLAY_CONTAINER, GeEditDisplay))
#define GTK_EDIT_DISPLAY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST  ((klass), EDIT_DISPLAY_CONTAINER, GeEditDisplayClass))
#define GTK_IS_EDIT_DISPLAY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE  ((obj), EDIT_DISPLAY_CONTAINER))
#define GTK_IS_EDIT_DISPLAY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EDIT_DISPLAY_CONTAINER))
#define EDIT_DISPLAY_CONTAINER_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS  ((obj), EDIT_DISPLAY_CONTAINER, GeEditDisplayClass))


typedef struct _GeEditDisplay GeEditDisplay;
typedef struct _GeEditDisplayClass GeEditDisplayClass;

struct _GeEditDisplayClass
{
	GtkBoxClass parent_class;
	GdkRGBA fgcolor,bgcolor;
};

struct _GeEditDisplay
{
    GtkWidget	widget;
	GdkWindow	*edit_window;
	GdkRectangle edit_rect;

    /* display */

	GdkDrawingContext *edit_gdk_context;
	GdkRGBA fgcolor,bgcolor;

	WINDP	*wp;	/* window associated */
	guint start_line;
	guint nrows;
	guint ncols;
	guint last_complete;
	guint width;
	guint height;
	cairo_region_t *region;
	cairo_t *cr;
	guint in_draw;
	cairo_font_extents_t wc_extents;
	cairo_font_extents_t sc_extents;
	guint id;
	guint act;
	PangoFontDescription *ge_font_desc;
	PangoLayout *layout;
};

GtkWidget	*ge_edit_display_new(void);
GType		ge_edit_display_get_type(void);

/* --- */
