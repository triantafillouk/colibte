/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

	Menu stuctures
*/
#ifndef _MENUS_
#define _MENUS_

 /* Menu type definitions */
#define MSIMPLE		0
#define MMENU		1
#define MSTOCK		2	// stock item
#define	MTOGGLE		3	// toggle item
#define MRADIO		4	// menu with radio item elements
#define	ERADIO		5	// radio item element
#define	DMENU		6	// dynamic menu


#define HORIZONTAL	0
#define VERTICAL	1
#define HLABEL_WIDTH 7	/* horizontal menu lebel width  */

#define MFUNCTION	0
#define MMENU		1
#define MSTOCK		2

#define	ESFILE	0	/* external file */
#define ESEMBED	1	/* embed icon */
#define ESTOCK 	2	/* stock icon */

typedef long long num;

 /* Menu definitions by KT */
typedef struct M_element {
	int type;	/* function, menu, line */
	char high;	/* highlight character */
	char *txt;
	char *help;
	int (*func)(num);
	struct MENUS *menu;
	char *macro;
} M_element;

typedef struct MENUS {
	char *title;
	int xpos;
	int ypos;
	int orient; /* horizontal, vertical */
	struct M_element *element;
} MENUS;


#endif
