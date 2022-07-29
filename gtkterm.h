/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.
*/
/* gtk2 screen driver */


GtkWidget* create_parent (void);
void ge_close();
void ge_kopen();
void ge_kclose();
void ge_open();
int open_file_gtk(char *);

extern void on_list1_selection_changed (GtkList *list, gpointer user_data);
extern int execute(int c, int  n);
int getcmd();

gboolean
on_parent_key_press_event (GtkWidget *widget,
    GdkEventKey *event, gpointer  user_data);

gboolean
on_parent_configure (GtkWidget *widget,
  GdkEventConfigure *event, gpointer user_data);

gboolean
on_gs_entry_key_press_event (GtkEntry *widget,
	GdkEventKey *event, gpointer user_data);


gboolean
on_gs_entry_cancel_event (GtkWidget *widget,
	GdkEventKey *event, gpointer user_data);

gboolean
on_gs_entry_ok_event (GtkWidget *widget,
	GdkEventKey *event,gpointer user_data);

typedef struct color_element {
 int row;
 char *label;
 int fg;
 int bg;
 GtkWidget *sample_ctext;
 GtkStyle *sample_style;
} color_element;

typedef struct fcolor_t {
	float red,green,blue,a;
} fcolor_t;

#include "icons.h"

/* editors toolbar  */
T_ELEMENT main_toolbar[] = {
	{ ESTOCK,"fileopen.xpm","document-open",		fileopen_xpm,"File open", open_file, 1},
	{ ESTOCK,"filesave.xpm" ,"document-save",  		filesave_xpm,"File save", save_file,1},
	{ ESTOCK,"editcopy.xpm" , "edit-copy",  		editcopy_xpm,"Edit copy", copy_region,1},
	{ ESTOCK,"editpaste.xpm", "edit-paste", 		editpaste_xpm,"Edit paste", paste_region, 2},
	{ ESTOCK,"queue.xpm"    , "go-next", 		queue_xpm,  "Next File", next_file ,3},
	{ ESTOCK,"undo.xpm"     , "edit-undo",  		undo_xpm,   "Undo",undo,1},
	{ ESTOCK,"redo.xpm"     , "edit-redo",  		redo_xpm,   "Redo",redo,1},
//	{ ESTOCK,"rc1-20.xpm"	, "indent",		right_20_xpm, "Indent", insert_tab,1},
//	{ ESTOCK,"lc1-20.xpm"	, "unindent",	left_20_xpm, "Unindent", remove_tab,1},
	{ ESTOCK,"rec.xpm"      , "media-record",rec_xpm,    "Record macro", record_macro,3},
	{ ESTOCK,"run.xpm"      , "media-playback-start", 	run_xpm,    "Run macro", exec_macro,1},
	{ ESTOCK,"lc1-20.xpm"   , "media-seek-backward",  	left_20_xpm,"go back", prev_hmark, 1},
	{ ESTOCK,"rc1-20.xpm"   , "media-seek-forward",	right_20_xpm,"go forward", next_hmark, 1},
	{ ESTOCK,"up.xpm"       , "go-up",  		up_xpm,     "Find previous", find_previous, 1},
	{ ESTOCK,"down.xpm"     , "go-down", 	down_xpm,   "Find next", find_next, 1},
	{ 0,NULL, NULL , NULL, NULL,NULL,0 }
};

/* Input Line toolbar  */
T_ELEMENT input_toolbar[] = {
// object-select-symbolic, system-shutdown, view-fullscreen, view-refresh
// application-exit, emblem-default
	{ 2,"ok2.xpm"    , "emblem-default", ok2_xpm,"Ok!", on_gs_entry_ok_event, 1},
// edit-delete,
// edit-find, 
//	{ 2,"cancel2.xpm", "dialog-error", exit_xpm, "Cancel!", on_gs_entry_cancel_event, 1},
	{ 2,"cancel2.xpm", "window-close", exit_xpm, "Cancel!", on_gs_entry_cancel_event, 1},
//	{ 2,"ok2.xpm"    , "go-jump", ok2_xpm,"Ok!", on_gs_entry_ok_event, 1},
	{ 0,NULL, NULL, NULL, NULL,NULL,0 }
};

/* Input Line toolbar  */
T_ELEMENT search_tbar[] = {
// object-select-symbolic, system-shutdown, view-fullscreen, view-refresh
// application-exit, emblem-default
	{ 2,"ok2.xpm"    , "emblem-default", ok2_xpm,"Ok!", on_gs_entry_ok_event, 1},
//	{ 2,"cancel2.xpm", "window-close", exit_xpm, "Cancel!", on_gs_entry_cancel_event, 1},
// edit-delete,
// edit-find, 
	{ 2,"cancel2.xpm", "dialog-error", exit_xpm, "Cancel!", on_gs_entry_cancel_event, 1},
//	{ 2,"ok2.xpm"    , "go-jump", ok2_xpm,"Ok!", on_gs_entry_ok_event, 1},
//	{ 2,"cancel2.xpm", "window-close", exit_xpm, "Cancel!", on_gs_entry_cancel_event, 1},
	{ 0,NULL, NULL, NULL, NULL,NULL,0 }
};

