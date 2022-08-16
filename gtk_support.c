/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.
*/

/* Gtk util functions */
#include <gtk/gtk.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "config.h"
#include "gtk_support.h"


static GList *pixmaps_directories = NULL;

double bt_dval(char *name);

GtkWidget *kt_new_listview(GtkListStore *store, GtkWidget *parent_box, gchar **ctitles,int show_titles)
{
 GtkWidget *ct;
 GtkWidget *w_scroll1;
 int i;
 GtkCellRenderer *renderer;
 GtkTreeViewColumn   *col;

 w_scroll1 = gtk_scrolled_window_new(NULL, NULL);
 gtk_widget_show(w_scroll1);

 
 ct = gtk_tree_view_new_with_model(GTK_TREE_MODEL (store));

 if(!show_titles) gtk_tree_view_set_headers_visible((GtkTreeView *)ct,0);
 else  gtk_tree_view_set_headers_clickable((GtkTreeView *)ct,1);

 for(i=0;ctitles[i]!=NULL;i++) {
 
 col = gtk_tree_view_column_new();
 if(show_titles){ 
	gtk_tree_view_column_set_title(col, ctitles[i]);
 };

 gtk_tree_view_append_column(GTK_TREE_VIEW(ct), col);
 renderer = gtk_cell_renderer_text_new();
 gtk_tree_view_column_pack_start(col, renderer, TRUE);
 gtk_tree_view_column_add_attribute(col, renderer, "text", i);
 
 };

 gtk_widget_show(ct);
 gtk_container_add(GTK_CONTAINER(w_scroll1),GTK_WIDGET(ct));
 gtk_box_pack_start(GTK_BOX(parent_box), w_scroll1, TRUE,TRUE,0);
 g_object_unref(store);

 return ct;
}

GtkWidget *Interface_pixmap_new(GtkWidget *parent, char **data)
{
   GtkWidget *pixmapwid;
   GdkColormap *colormap;
   GdkPixmap *pixmap;
   GdkBitmap *mask;
   GtkStyle *style;

   style = gtk_widget_get_style(parent);

   colormap = gtk_widget_get_colormap (parent);

   pixmap = gdk_pixmap_colormap_create_from_xpm_d(NULL,colormap,
				&mask, &style->bg[GTK_STATE_NORMAL],data);

   pixmapwid = gtk_pixmap_new(pixmap, mask);
   gdk_pixmap_unref(pixmap);
   gdk_bitmap_unref(mask);
   return (pixmapwid);
}


/* This is a dummy pixmap we use when a pixmap can't be found. */
char *dummy_pixmap_xpm[] = {
/* columns rows colors chars-per-pixel */
"6 6 4 1",
"  c None",
"r c #FF0000",
"b c #000000",
"g c #00FF00",
/* pixels */
"b    b",
" rrrr ",
" r  r ",
" r  r ",
" rrrr ",
"b    b "
};


/* This is an internally used function to create pixmaps. */
GtkWidget*
create_dummy_pixmap                    (GtkWidget       *widget)
{
  GdkColormap *colormap;
  GdkPixmap *gdkpixmap;
  GdkBitmap *mask;
  GtkWidget *pixmap;

  colormap = gtk_widget_get_colormap (widget);
  gdkpixmap = gdk_pixmap_colormap_create_from_xpm_d (NULL, colormap, &mask,
                                                     NULL, dummy_pixmap_xpm);
  if (gdkpixmap == NULL)
    g_error ("Couldn't create replacement pixmap.");
  pixmap = gtk_pixmap_new (gdkpixmap, mask);
  gdk_pixmap_unref (gdkpixmap);
  gdk_bitmap_unref (mask);
  return pixmap;
}

/* create pixmaps from icon files. */
GtkWidget* create_pixmap(GtkWidget *widget, char *filename)
{
  char *found_filename = NULL;
  GdkColormap *colormap;
  GdkPixmap *gdkpixmap;
  GdkBitmap *mask;
  GtkWidget *pixmap;

  if (!filename || !filename[0])
      return create_dummy_pixmap (widget);

  /* We first try any pixmaps directories set by the application. */
	if((found_filename = find_file("pixmap",filename,1,0))==NULL) 
		return create_dummy_pixmap (widget);

  colormap = gtk_widget_get_colormap (widget);
  gdkpixmap = gdk_pixmap_colormap_create_from_xpm (NULL, colormap, &mask,
                                                   NULL, found_filename);
  if (gdkpixmap == NULL) {
      return create_dummy_pixmap (widget);
  }

  pixmap = gtk_pixmap_new (gdkpixmap, mask);
  gdk_pixmap_unref (gdkpixmap);
  gdk_bitmap_unref (mask);

  return pixmap;
}

// Create menus in the start of a gtk widget (vbox1), in a parent window
void create_main_menu(GtkWidget *vbox, GtkWidget *parent, MENUS *menu,GtkAccelGroup *accel_group)
{
 GtkWidget *menubar;
 int i;
 /* Create the menubar in vbox */
  menubar = gtk_menu_bar_new ();

  gtk_widget_show (menubar);
  gtk_box_pack_start (GTK_BOX (vbox), menubar, FALSE, FALSE, 0);

 /* add the menu elements */
 for(i=0;i< 100; i++) {
	if(menu->element[i].high==0) break;
   
   create_menu_element( &menu->element[i], menubar, parent, accel_group, 1);
 };
}

GtkWidget *  create_menu_element(M_element *element, GtkWidget *menu, GtkWidget *parent, GtkAccelGroup  *menu_accels, guint alt_flag)
{
 GtkWidget *gelement;
 char *txt;

 if(element->type==MMENU) {
	return ( create_menu( element, menu,  parent, menu_accels, alt_flag));
 } else { 
	if(element->type==MSTOCK) {
	 	gelement = gtk_image_menu_item_new_from_stock(element->txt,menu_accels);
	} else {
		txt = underline_element(element);
		if(element->type == MTOGGLE) {
			gelement = gtk_check_menu_item_new_with_mnemonic(txt);
			if((int)bt_dval(element->help)) 
			gtk_menu_item_activate((GtkMenuItem *)gelement); 
		} else {
			gelement = gtk_menu_item_new_with_mnemonic(txt);
		};
  }
  gtk_widget_show (gelement);
  gtk_container_add (GTK_CONTAINER (menu), gelement);

  gtk_signal_connect (GTK_OBJECT (gelement), "activate",
                      GTK_SIGNAL_FUNC (cb_menu),
                      element);
 }
 return (gelement);
}

char *underline_element(M_element *element)
{
 static char text[100];
 unsigned int i,i1;

 for(i=0,i1=0;i<strlen(element->txt);i++,i1++) {
 	if(element->txt[i] == element->high) text[i1++]='_';
 	text[i1] = element->txt[i];
 };
 text[i1]=0;
 return(text);
}

GtkWidget * create_menu(M_element *element, GtkWidget *pelement, GtkWidget *parent,GtkAccelGroup  *menu_accels, guint alt_flag)
{
 GtkWidget *n_menu, *n_menu_menu;
 char *txt;
 int i;
 txt = underline_element(element);
 n_menu = gtk_menu_item_new_with_mnemonic(txt);
 gtk_widget_show(n_menu);

 if(pelement!=NULL) 
 gtk_container_add( GTK_CONTAINER (pelement), n_menu);

 n_menu_menu = gtk_menu_new();
// if(!strncmp(element->txt,"SELECT",6)) menu_selected=n_menu_menu;
// if(!strncmp(element->txt,"NOSELECT",8)) menu_not_selected=n_menu_menu;

 gtk_menu_item_set_submenu (GTK_MENU_ITEM (n_menu), n_menu_menu);
 for(i=0;;i++) {
	if(element->menu->element[i].high==0) break;
 	create_menu_element( &element->menu->element[i], n_menu_menu, parent, menu_accels,0); 
 };

 return (n_menu_menu);
}

GtkWidget * new_popup_menu(MENUS *popup,GtkAccelGroup  *menu_accels)
{
 int i;
 GtkWidget *n_menu_menu = gtk_menu_new();

 for(i=0;;i++) {
	if(popup->element[i].high==0) break;
 	create_menu_element( &popup->element[i], n_menu_menu, NULL, menu_accels,0); 
 };
 
 return(n_menu_menu);
}

GtkWidget * new_toolbar(GtkWidget *parent, GtkWidget *box, T_ELEMENT telem[], int orient,int icon_size,int side)
{
  GtkWidget *button;
  GtkWidget *toolbar1;
  GtkWidget *icon;
  int i;
  int embed_icons = (int) bt_dval("embed_icons");
	embed_icons=0;
//	MESG("New toolbar:");
	toolbar1 = gtk_toolbar_new();
	if(orient==0) gtk_toolbar_set_orientation(GTK_TOOLBAR (toolbar1), GTK_ORIENTATION_HORIZONTAL);
	else gtk_toolbar_set_orientation(GTK_TOOLBAR (toolbar1), GTK_ORIENTATION_VERTICAL);
	gtk_widget_show (toolbar1);
	if(side) gtk_box_pack_end (GTK_BOX (box), toolbar1, FALSE, TRUE, 0);
	else gtk_box_pack_start (GTK_BOX (box), toolbar1, FALSE, TRUE, 0);

//	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar1),GTK_RELIEF_NONE);
	gtk_toolbar_set_style (GTK_TOOLBAR (toolbar1), GTK_TOOLBAR_ICONS);
	gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar1),icon_size);
//	gtk_widget_set_size_request (toolbar1, 100, -1);

	GtkTooltips *tooltips1 = gtk_tooltips_new();
	gtk_tooltips_enable(tooltips1);
//  gtk_tooltips_set_delay(tooltips1, 1000);
	gtk_toolbar_set_tooltips(GTK_TOOLBAR(toolbar1),TRUE);
	
  for(i=0;telem[i].fname !=NULL;i++) {
//	MESG("	%d: embed=%d type=%d [%s]",i,embed_icons,telem[i].type,telem[i].fname);
	if(embed_icons){
		button =  gtk_toolbar_append_item (GTK_TOOLBAR (toolbar1),
								NULL,
                                telem[i].help, NULL,
								Interface_pixmap_new(parent, telem[i].embedded_icon),
								NULL, NULL);
	} else {
	if(telem[i].icon_type==ESTOCK) {
//		MESG("use stock icon named [%s]",telem[i].icon_name);
		icon = gtk_image_new_from_icon_name(telem[i].icon_name,0);
		button = (GtkWidget *)gtk_tool_button_new(icon,telem[i].help);
		gtk_widget_show(icon);
		gtk_widget_show(button);
		gtk_container_add(GTK_CONTAINER (toolbar1),button);
		gtk_tool_item_set_tooltip(GTK_TOOL_ITEM(button),tooltips1,telem[i].help,NULL);
	} else {
//		MESG("use icon from file");
		icon = create_pixmap(parent, telem[i].fname);
		button = (GtkWidget *)gtk_tool_button_new(icon,telem[i].help);
		gtk_container_add(GTK_CONTAINER (toolbar1),button);
		gtk_widget_show(icon);
		gtk_widget_show(button);
		gtk_tool_item_set_tooltip(GTK_TOOL_ITEM(button),tooltips1,telem[i].help,NULL);
	}
	}
  gtk_widget_show(button);
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (cb_tools),
                      &telem[i]);
  };
  return toolbar1;
}

int kt_remove_liststore_row(GtkTreeModel *store, int row)
{
 GString *spath=g_string_new("");
 g_string_printf(spath,"%d",row);
 GtkTreeIter iter;
 GtkTreePath *path = gtk_tree_path_new_from_string(spath->str);
 g_string_free(spath, TRUE);
 if ( gtk_tree_model_get_iter (store, &iter, path) ) { // get iter from specified path
	gtk_list_store_remove ( (GtkListStore *)store, &iter ); // remove item
	return 1;
 };
 return 0;
}
