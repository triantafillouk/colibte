/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.
*/

/* 
	Gtk utility functions 
*/
#ifndef _GSUPPORT
#define _GSUPPORT

#include <gtk/gtk.h>
#include "support.h"

#include	"menu.h"

#define	BGW	8

typedef struct T_ELEMENT  {
	int icon_type;			/* icon prefered type ESTOCK,0=use file  */
	char *fname;		/* icon file name  */
	char *icon_name;	/* stock icon nane  */
	char **embedded_icon;		/* embeded icon  */
	char *help;			/* showing help  */
	int (*func)();		/* bind function  */
	int type;
} T_ELEMENT;

/*
 * This function returns a widget in a component created by Glade.
 * Call it with the toplevel widget in the component (i.e. a window/dialog),
 * or alternatively any widget in the component, and the name of the widget
 * you want returned.
 */
GtkWidget*  lookup_widget              (GtkWidget       *widget,
                                        const gchar     *widget_name);

GtkWidget* create_pixmap(GtkWidget *widget, char *filename);

GtkWidget* create_dummy_pixmap(GtkWidget *widget);

/* get_widget() is deprecated. Use lookup_widget instead. */
#define get_widget lookup_widget

/* Use this function to set the directory containing installed pixmaps. */
void  add_pixmap_directory  (const gchar     *directory);

void create_main_menu(GtkWidget *vbox, GtkWidget *parent, MENUS *menu,GtkAccelGroup *accel_group);

GtkWidget *  create_menu(M_element *element, GtkWidget *pelement, GtkWidget *parent,GtkAccelGroup  *menu_accels, guint alt_flag);

GtkWidget *  create_menu_element(M_element *element, GtkWidget *menu, GtkWidget *parent, GtkAccelGroup  *menu_accels, guint alt_flag);

char *underline_element(M_element *element);

void cb_menu(GtkWidget *widget, M_element *element);

GtkWidget * new_popup_menu(MENUS *popup,GtkAccelGroup  *menu_accels);

GtkWidget *Interface_pixmap_new(GtkWidget *parent, gchar **data);

void cb_tools(GtkWidget *w, T_ELEMENT *telem );

GtkWidget *kt_new_listview(GtkListStore *store, GtkWidget *parent_box, gchar **ctitles,int titles);

int kt_remove_liststore_row(GtkTreeModel *store, int row);

GtkWidget * new_toolbar(GtkWidget *parent, GtkWidget *box, T_ELEMENT telem[], int orient,int icon_size,int side);

#endif
