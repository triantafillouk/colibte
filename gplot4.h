/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

*/
#include "gtk_support.h"

extern char *rec_xpm[];
extern char *run_xpm[];

// type,fname,icon_name,icon,help,func,i1
T_ELEMENT tbar_plot[] = {
	{ ESTOCK,"rec.xpm","media-record", rec_xpm, "dot input", record_macro,3},
	{ ESTOCK,"run.xpm","media-playback-start", run_xpm,"animate", exec_macro,3},
	{ 0,NULL, NULL,NULL,NULL,NULL,0 }
};
