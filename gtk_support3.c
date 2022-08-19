/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.
*/

/* Gtk3 utility functions */
#include <gtk/gtk.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "gtk_support.h"
#include "bt_func.h"

void set_box_background_color(GtkWidget *box,char *bgcolor);
void set_box_color(GtkWidget *box,char *color,char *bgcolor);
char *find_file(char *subdir, char *fname, int check_start_dir, int create_if_not_found);

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
 gtk_tree_view_set_grid_lines ((GtkTreeView  *)ct,0);	/* No grid lines  */
 gtk_tree_view_set_hover_selection ((GtkTreeView  *)ct,1);		/* not bad!  */
 gtk_tree_view_set_enable_search ((GtkTreeView  *)ct,1);
 // the following just change the font!
 set_box_color(parent_box,"white","black");
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

/* This is a dummy pixmap we use when a pixmap can't be found. */
const char *dummy_pixmap_xpm[] = {
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

#if	NUSE
/* This is an internally used function to create dummy icon  */
GtkWidget*
create_dummy_icon ()
{
  GdkPixbuf *dummy_pixbuf;
  GtkWidget *icon;
  dummy_pixbuf = gdk_pixbuf_new_from_xpm_data(dummy_pixmap_xpm);
  if (dummy_pixbuf == NULL)
    g_error ("Couldn't create replacement pixmap.");
  icon = gtk_image_new_from_pixbuf(dummy_pixbuf);
  return icon;
}
#endif

GtkWidget*
dummy_icon ()
{
  static GtkWidget *icon=NULL;
  if(icon) return icon;
  else {
	  GdkPixbuf *dummy_pixbuf;
	  dummy_pixbuf = gdk_pixbuf_new_from_xpm_data(dummy_pixmap_xpm);
	  if (dummy_pixbuf == NULL)
	    g_error ("Couldn't create replacement pixmap.");
	  icon = gtk_image_new_from_pixbuf(dummy_pixbuf);
	  return icon;
  }
}

GtkWidget *
pixbuf_icon(const char **embeded_icon)
{
  GdkPixbuf *pixbuf_icon;
  GtkWidget *icon;
  if(embeded_icon==NULL) {
  	MESG("pixbuf_icon: embeded_icon is NULL, return dummy!");
	return dummy_icon();
  };
  pixbuf_icon = gdk_pixbuf_new_from_xpm_data(embeded_icon);
  icon = gtk_image_new_from_pixbuf(pixbuf_icon);
  return icon;
}

// Create menus in the start of a gtk widget (vbox1), in a parent window
void create_main_menu(GtkWidget *vbox, GtkWidget *parent, MENUS *menu,GtkAccelGroup *accel_group)
{
 GtkWidget *menubar;
 int i;
 /* Create the menubar in vbox */

  menubar = gtk_menu_bar_new ();
  gtk_widget_show (menubar);

  if((int)bt_dval("use_titlebar")){
	gtk_header_bar_pack_start ((GtkHeaderBar *) (vbox), menubar);
  } else {
	gtk_box_pack_start (GTK_BOX (vbox), menubar, FALSE, FALSE, 0);
  };
 /* add the menu elements */
 for(i=0;menu->element[i].high!=0; i++) {
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
		txt = underline_element(element);
		if(element->type == MTOGGLE) {
			gelement = gtk_check_menu_item_new_with_mnemonic(txt);
			if((int)bt_dval(element->help)) 
				gtk_menu_item_activate((GtkMenuItem *)gelement); 
		} else {
			gelement = gtk_menu_item_new_with_mnemonic(txt);
		};
	if(strlen(element->help)>1 && element->type!=MTOGGLE) {
		int modifier=0;
		GtkWidget *child = gtk_bin_get_child(GTK_BIN(gelement));
		if(element->help[0]=='_') modifier=GDK_MOD1_MASK;
		if(element->help[0]=='^') modifier=GDK_CONTROL_MASK;
		if(element->help[0]=='F') modifier=GDK_MOD3_MASK;
		if(element->help[0]=='*') modifier=GDK_MOD1_MASK|GDK_CONTROL_MASK;
//			gtk_label_set_markup(GTK_LABEL(child),element->help+2);
		if(modifier==GDK_MOD3_MASK) {
			gtk_accel_label_set_accel(GTK_ACCEL_LABEL(child), GDK_KEY_F1+element->help[1]-'1',0);
		} else gtk_accel_label_set_accel(GTK_ACCEL_LABEL(child), element->help[1],modifier);
	};
	gtk_widget_show (gelement);
	gtk_container_add (GTK_CONTAINER (menu), gelement);
	
	g_signal_connect (gelement, "activate",
	                  (GCallback)cb_menu,
	                  element);
 }
 return (gelement);
}

char *underline_element(M_element *element)
{
 static char text[100];
 unsigned int i,i1;
 char *element_text;
 if(strlen(element->help)>2 && element->type!=MTOGGLE) element_text=(element->help)+2;
 else element_text=element->txt;
 for(i=0,i1=0;i<strlen(element->txt);i++,i1++) {
 	if(element_text[i] == element->high) text[i1++]='_';
 	text[i1] = element_text[i];
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

// Do not allow space in front of menu elements!
// gtk_menu_set_reserve_toggle_size((GtkMenu *)n_menu_menu,0);

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

/* create pixmaps from icon files. */
GtkWidget* create_pixmap(GtkWidget *widget, char *filename)
{
  char *found_filename = NULL;
  GtkWidget *pixmap;
  GdkPixbuf *pixbuf;
  GError *error=NULL;
  
  /* We first try any pixmaps directories set by the application. */
	if((found_filename = find_file("pixmaps",filename,1,0))==NULL) return NULL;

	pixbuf = gdk_pixbuf_new_from_file(found_filename,&error);
	if(pixbuf) {
//		MESG("create image from pixbuf");
		pixmap = gtk_image_new_from_pixbuf(pixbuf);
	  	return pixmap;
  } else {
  	MESG("cannot create pixbuf from [%s]",found_filename);
	return NULL;
  };
}

GtkWidget * new_toolbar(GtkWidget *parent, GtkWidget *box, T_ELEMENT telem[], int orient,int icon_size,int side)
{
  GtkWidget *toolbar1;
  GtkIconTheme *icontheme = gtk_icon_theme_get_default();

  int i;
  int embed_icons = (int) bt_dval("embed_icons");

	toolbar1 = gtk_toolbar_new();

	if(orient==0) gtk_orientable_set_orientation((GtkOrientable *)toolbar1,GTK_ORIENTATION_HORIZONTAL);
	else gtk_orientable_set_orientation((GtkOrientable *)toolbar1,GTK_ORIENTATION_VERTICAL);
//	MESG("new_toolbar:");
	gtk_widget_show (toolbar1);
	if(side==1) gtk_box_pack_end (GTK_BOX (box), toolbar1, FALSE, TRUE, 0);
	else if(side==2) gtk_header_bar_pack_end((GtkHeaderBar *)box,toolbar1);
	else gtk_box_pack_start (GTK_BOX (box), toolbar1, FALSE, TRUE, 0);

	gtk_toolbar_set_icon_size((GtkToolbar *)toolbar1,icon_size);
//	MESG("new_toolbar:1");
  for(i=0;telem[i].fname !=NULL;i++) {
	GtkImage *img;
	GtkWidget *button;
	GtkWidget *icon=NULL;
//	MESG(">> %d",i);
	if(embed_icons){ /* For umbeded icons  */
		icon = pixbuf_icon((const char **)telem[i].embedded_icon);
	} else {	/* from stock  */
//	MESG("- %d type=%d [%s]",i,telem[i].type,telem[i].icon_name);
	if(telem[i].icon_type==ESTOCK) {
//		MESG("	estock: icon_name=[%s]",telem[i].icon_name);
		if(gtk_icon_theme_has_icon(icontheme,telem[i].icon_name)) {
//			MESG("	-----");
			icon = gtk_image_new_from_icon_name(telem[i].icon_name,icon_size);
		};
//		MESG("	-----");
	} else {	/* from file  */
//		MESG("	fname=[%s]",telem[i].fname);
		icon = create_pixmap(parent, telem[i].fname);
		if(icon==NULL) {	/* if file not found try embeded  */
			icon = pixbuf_icon((const char **)telem[i].embedded_icon);
		};
	};
	};
	if(icon==NULL) {
		MESG("Icon not found,use embedded icon for [%s]",telem[i].fname);
		icon = pixbuf_icon((const char **)telem[i].embedded_icon);
	};
    if(telem[i].type==1) {
		button = (GtkWidget *)gtk_tool_button_new(icon,telem[i].help);
		gtk_tool_item_set_tooltip_markup(GTK_TOOL_ITEM(button),telem[i].help);
		gtk_container_add(GTK_CONTAINER (toolbar1),button);
//		MESG("toolbar add: button %s", telem[i].fname); 
		g_signal_connect (button, "clicked", (GCallback) cb_tools, &telem[i]);
	} else
	if(telem[i].type>=100) {
		button = (GtkWidget *)gtk_toggle_tool_button_new();
		gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON (button), icon);
		gtk_container_add(GTK_CONTAINER (toolbar1),button);
		gtk_tool_item_set_tooltip_markup(GTK_TOOL_ITEM(button),telem[i].help);
//		MESG("toolbar add: button >100 %s", telem[i].fname); 
		g_signal_connect (button, "toggled", (GCallback) cb_tools, &telem[i]);
	} else {
		button = (GtkWidget *)gtk_tool_button_new(icon,"");
		gtk_tool_item_set_tooltip_markup(GTK_TOOL_ITEM(button),telem[i].help);
		gtk_container_add(GTK_CONTAINER (toolbar1),button);
//		MESG("toolbar add: button %s help=%s", telem[i].fname,telem[i].help); 
		g_signal_connect (button, "clicked", (GCallback) cb_tools, &telem[i]);
	};
//	MESG("	---- end widget");
	gtk_widget_show((GtkWidget *)icon);
//	MESG("  ----- end2");
	gtk_widget_show(button);
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
