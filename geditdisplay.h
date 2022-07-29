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

#define GTK_TYPE_EDIT_DISPLAY            (ge_edit_display_get_type ())
#define GTK_EDIT_DISPLAY(obj)            (GTK_CHECK_CAST ((obj), GTK_TYPE_EDIT_DISPLAY, GeEditDisplay))
#define GTK_EDIT_DISPLAY_CLASS(klass)    (GTK_CHECK_CLASS_CAST ((klass), GTK_TYPE_EDIT_DISPLAY, GeEditDisplayClass))
#define GTK_IS_EDIT_DISPLAY(obj)         (GTK_CHECK_TYPE ((obj), GTK_TYPE_EDIT_DISPLAY))
#define GTK_IS_EDIT_DISPLAY_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass), GTK_TYPE_EDIT_DISPLAY))

typedef struct _GeEditDisplay GeEditDisplay;
typedef struct _GeEditDisplayClass GeEditDisplayClass;

struct _GeEditDisplayClass
{
	GtkVBoxClass parent_class;
	GdkColor	bgcolor;
	GdkColor	fgcolor;
//	guint color_cnt;
//	void (* ge_edit_display) (GeEditDisplay *ge); 
};

struct _GeEditDisplay
{
    GtkWidget	widget;
	GdkWindow	*edit_window;
	GdkRectangle edit_rect;
    /* display */
    GdkPixmap	*edit_pixmap;
//    gboolean	backing_store;
    GdkGC	*fgc;
	GdkGC	*bgc;

	GdkColor	fgcolor, bgcolor;
//    GdkCursor	*cursor;
	WINDP	*wp;	/* window associated */
	guint start_line;
	guint nrows;
	guint ncols;
	guint last_complete;
	guint width;
	guint height;
//	GdkFont *fn1;
	cairo_t *cr;
	cairo_font_extents_t wc_extents;
	cairo_font_extents_t sc_extents;
	guint sl_base;	/* status line font base */
	PangoFontDescription *ge_font_desc;
	PangoLayout *layout;
};

GtkWidget	*ge_edit_display_new(void);
GtkType		ge_edit_display_get_type(void);
void	    ge_edit_display_scroll	(GeEditDisplay *ed, gint lines);

