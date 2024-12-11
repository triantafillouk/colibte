/*
	curses,gtk editor,directory browser
	Copyright Kostas Triantafillou 
	GPL2 License
*/
/* gtk3 screen driver */

GtkWidget* create_parent (void);
void ge_close();
void ge_kopen();
void ge_kclose();
void ge_open();
int open_file_gtk(char *);

extern int execute(int c, int  n);
int getcmd();

gboolean
on_parent_key_press_event (GtkWidget *widget,
    GdkEventKey *event, gpointer  user_data);

gboolean
on_parent_configure (GtkWidget *widget,
  GdkEventConfigure *event, gpointer user_data);

int input_line_ok_event(num n);
int input_line_cancel_event(num n);
gboolean
on_gs_entry_key_press_event (GtkEntry *widget,
	GdkEventKey *event, gpointer user_data);

gboolean
on_gs_entry_cancel_event (GtkWidget *widget,
	GdkEventKey *event, gpointer user_data);

gboolean
on_gs_entry_ok_event (GtkWidget *widget,
	GdkEventKey *event,gpointer user_data);

gboolean
on_search_entry_ok_event (GtkWidget *widget,
	GdkEventKey *event,gpointer user_data);

gboolean
toggle_button (GtkWidget *widget,int type);

gboolean
on_search_entry_cancel_event (GtkWidget *widget,
	GdkEventKey *event,gpointer user_data);

typedef struct color_element {
 int row;
 char *label;
 int fg;
 int bg;
 GtkWidget *sample_ctext;
 GtkWidget *sample_box;
 GtkWidget *button_fg;
 GtkWidget *button_bg;
} color_element;

#include "icons.h"
/* editors toolbar  */
T_ELEMENT main_toolbar[] = {
	{ ESTOCK,"fileopen.xpm","document-open", fileopen_xpm, "File open", open_file, 1},
	{ ESTOCK,"filesave.xpm","document-save", filesave_xpm, "File save", save_file,1},
	{ ESTOCK,"editcopy.xpm","edit-copy",  editcopy_xpm,  "Edit copy", copy_region,1},
	{ ESTOCK,"editpaste.xpm","edit-paste",  editpaste_xpm, "Edit paste", paste_region, 2},

	{ ESTOCK,"queue.xpm","edit-paste",  queue_xpm, "Next File", next_file,1},

	{ ESTOCK, "undo.xpm","edit-undo",  undo_xpm, "Undo",undo,1},
	{ ESTOCK, "redo.xpm","edit-redo",  redo_xpm, "Redo",redo,1},

	{ ESTOCK,"rec.xpm","media-record", rec_xpm , "Record macro", record_macro,1},
	{ ESTOCK, "run.xpm","media-playback-start", run_xpm, "Run macro", exec_macro,1},
	{ ESTOCK,"lc1-20.xpm","media-seek-backward", left_20_xpm, "go back", prev_hmark, 1},
	{ ESTOCK,"rc1-20.xpm","media-seek-forward", right_20_xpm, "go next", next_hmark, 1},
	{ ESTOCK,"up.xpm","go-up",  up_xpm, "Find previous", find_previous, 1},
	{ ESTOCK,"down.xpm","go-down", down_xpm, "Find next", find_next, 1},
	{ 0,NULL, NULL,NULL,NULL,NULL,0 }
};

/* Input Line toolbar  */
T_ELEMENT input_toolbar[] = {
#if	1
	{ ESTOCK,"ok2.xpm","emblem-default", ok2_xpm, "Ok!", input_line_ok_event, 1},
	{ ESTOCK,"cancel2.xpm","window-close", exit_xpm, "Cancel!", input_line_cancel_event, 1},
#else
	{ ESTOCK,"ok2.xpm","emblem-default", ok2_xpm, "Ok!", on_gs_entry_ok_event, 1},
	{ ESTOCK,"cancel2.xpm","window-close", exit_xpm, "Cancel!", on_gs_entry_cancel_event, 1},
#endif
	{ 0,NULL, NULL,NULL,NULL,NULL,0 }
};


T_ELEMENT search_toolbar[] = {
	{ ESTOCK,"mode_regexp","xapp-use-regex-symbolic",regexp_xpm, "Regexp",toggle_regexp,100+EMCREGEXP},
	{ ESTOCK,"exact_match","xapp-text-case-symbolic",exact_xpm, "Exact match",toggle_case,100+EMCCASE},
	{ 0,NULL, NULL,NULL,NULL,NULL,0 }
};
