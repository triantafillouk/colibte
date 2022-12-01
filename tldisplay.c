/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

*/
/* This is the curses/xlib version of display functions */

#include	"xe.h"
#include	"support.h"
#include	"menus.h"
#include	"panel_curses.h"

char **getdir(char *dirname,char *s_find,int *num);
int utf_num_chars(char *);
void drv_restore_wdimensions(WINDP *wp);
int set_play_string1(char *st1);
void set_record_string1(char *st2);
char *str_tfile(struct stat *t,char *file_name,int maxsize);
void drv_window_delete(WINDP *wp);
int get_menu_index_from_mouse();
void utf_string_break(char *utf_string, int column);
int noop(int);

extern FILEBUF *cbfp;
extern int clen_error;
extern WINDOW *mesg_window;	/* message line curses window  */

int drv_numrow=0;		// current driver screen rows
int drv_numcol=0;		// current driver screen columns
int drv_colors=0;
char *time2a();

extern short *kbdptr;
extern int slang;
extern alist *window_list;
extern int *color;
extern char utfokey[10];
extern int utflen;
extern short	*kbdsptr;
extern int sort_mode;
extern int color_scheme_ind;

int e_pos=0;			// entry position

int repeat_arg=1;

int in_menu=0,in_selection=0;

 /* external function definitions  */
void refresh_menu();
void drv_set_wvs(WINDP *wp);

MENUS *c_menu;		/* current menu */

extern int color_menu_fg,color_menu_bg;
void drv_free_win(WINDP *w);


void update_selection()
{
	register WINDP *wp;
	if(cwp->selection==0) return;
	/* update any windows that need refreshing */
	hide_cursor("update_selection");
	upd_column_pos();
	cwp->currow = window_cursor_line(cwp);
	lbegin(window_list);
	while((wp=(WINDP *)lget(window_list))!=NULL)
	{
			if(wp==cwp) wp->currow = window_cursor_line(wp);

			upd_all_virtual_lines(wp,"update_selection");	/* update all lines */
			wp->w_flag = 0;
	}

	updgar();
	/* update the virtual screen to the physical screen */
	update_physical_windows();
	/* update the cursor and flush the buffers */
	movecursor(cwp->currow, WGetCol()); 
	show_cursor("update_selection");
	drv_flush();
}

/* Display the top menu  */
void top_menu(int init)
{
 int i;
 int fch,bch;
 MENUS *mm1;
 int pos_x=0;

 mm1 = &m_topn;
// clear the area
 if(init) clear_hmenu();
 bch=color_menu_bg;
 fch=color_menu_fg;

 for(i=0;;i++) {
	if(mm1->element[i].high==0) break;
	hdab(pos_x, mm1->element[i].high, mm1->element[i].txt,bch,fch);
	pos_x += HLABEL_WIDTH;
 };
}

/* initialize screen */
void vtinit(int argc, char **argp)
{
	drv_init(argc,argp);
	drv_open();		/* open the screen */
	drv_post_init();
	resize_screen(2);
	cwp = dublicate_window(0);	/* creates first window  */

	set_1window();
}

/*
 * Send a command to the terminal to move the hardware cursor to row "row"
 * and column "col" of the buffer??. The row and column arguments are origin 0.
 */ 

void movecursor(int row, int col)
{
    if(row<drv_numrow-1) {
		if(cwp->w_ntcols>40) show_position_info(0);
		else if(cwp->w_ntcols>25) show_position_info(1);
		else show_position_info(-1);
	};
	drv_win_move(cwp,row, col);
}

/* clear the common status line */
void clear_message_line()
{
 if(entry_mode) {
	show_cursor_dl(e_pos); 
	// we must write here the contents of getstring
	return;
 };

 if(app_error==0) {
	dspv(mesg_window,0,0,time2a());
 } else app_error--;
}

int window_row_resize(int n)
{
	char slines[80];

	/* must have a non-default argument, else ignore call */
	slines[0]=0;
	if(nextarg("Resize window by # lines: ", slines, 80,true)!=TRUE) return(FALSE);
	if(!macro_exec) n = get_val();
	return(hresize_wind(n));
}

int window_row_increase(int n)
{
	return(hresize_wind(5));
}

int window_column_increase(int n)
{
	return(vresize_wind(5));
}

int window_row_decrease(int n)
{
	return(hresize_wind(-5));
}

int window_column_decrease(int n)
{
	return(vresize_wind(-5));
}

int window_column_resize(int n)
{
	char slines[80];

	/* must have a non-default argument, else ignore call */
	slines[0]=0;
	if(nextarg("Resize window by # columns: ", slines, 80,true)!=TRUE) return(FALSE);
	if(!macro_exec) n = get_val();

	return(vresize_wind(n));
}

int describe_key(int n)	/* describe the command for a certain key */
{
	int c;		/* key to describe */
	if(macro_exec) return 0;
	/* prompt the user to type us a key to describe */
	msg_line("Describe-key : ");

	/* get the command sequence to describe */
	c=getcmd();
	/* and show it */
	return(show_key(c));
}

// dummy function for curses
int plot_on()
{
	return false;
}

// summy function for curses
void plot_redraw()
{
}

int mouse_move(int n)
{
 return true;
}



void show_menu_line(int line,M_element *element,int bcolor,int fcolor)
{
	char text_line[64];
	if(element->type==MTOGGLE) {
		int active=' ';
		if((int)bt_dval(element->help))
		{
			active='*';
		};
		// MESG("menu line: [%s]->[%c]",element->help,active);
		sprintf(text_line,"%c%s",active,element->txt);	
	} else {
		strcpy(text_line,element->txt);
	};
	xdab(line+1,element->high, text_line,bcolor,fcolor);
}

int (*get_menucmd(MENUS *m1,int first,int pos_x,int pos_y))()
{
 int i,j,c,iol=0;
 int sx,sy,nu,maxl;
 int ftime;
 int orient;
 int bch=color_menu_bg;
 int fch=color_menu_fg;
 int (*execf)();
 int border=1;

 sx=sy=maxl=nu=0;
 in_menu=1;
 c_menu=m1;
 ftime=1;
 orient=m1->orient;
 // MESG("get_menucmd:[%s] orient=%d xpos=%d ypos=%d pos_x=%d pos_y=%d",m1->title,m1->orient,m1->xpos,m1->ypos,pos_x,pos_y);
 if(kbdmode==PLAY||kbdmode==BPLAY){
	
	short line=first;
	if(line<0) { line=0;first=0;};
	
	if( m1->element[line].type == MMENU) {
		int line2;
		MENUS *m2 = m1->element[line].menu; 
		if(kbdmode==PLAY) first = *kbdptr++;
		else first = *kbdsptr++;

		for(line2=0;;line2++)
		{
			if(m2->element[line2].high==0) {
				execf=do_nothing;
				return(execf);
			};
			if(m2->element[line2].high==capital(first)) break;
		};

		if(m2->element[line2].type == MMENU){
			MENUS *submenu=m2->element[line2].menu;
			// MESG(" Call get_menucmd x=%d y=%d line2=%d",submenu->xpos,submenu->ypos,line2);
			execf = get_menucmd(submenu,line2,submenu->xpos,submenu->ypos) ;
		} else {
			execf = m2->element[line2].func;
			repeat_arg = (long int)m2->element[line2].menu;
		};
	} else {
		execf = m1->element[line].func ;
		repeat_arg = (long int)m1->element[line].menu;
	};
	return(execf);
 };

// find the box width
 for(nu=0;;nu++) {
	if(m1->element[nu].high==0) break;
 	if((j=strlen(m1->element[nu].txt)) >maxl) maxl=j;
 };
 if(orient==VERTICAL){
	sx = pos_x; 
	// sy=m1->ypos;
	sy = pos_y;
 } else {
	sx=0; sy=0;
#if	TARROWS
	nu-=4;
#endif
 };

 hide_cursor("get_menucmd");

 if(orient==VERTICAL) {
	if(first<0) {	/* mouse dependant position  */
		disp_box("",border,-1,-1,nu,maxl);
	} else {
		disp_box("",border,sy,sx,sy+nu+1,sx+maxl+1);
	};
  } else {
	maxl++;	/* separate horizontal menu elements  */
  };

  if(first<0) first=0;
 /* display menu */
 if(orient==VERTICAL) {
	for(i=0;i<nu;i++) {
//		xdab(i+1,m1->element[i].high,m1->element[i].txt,bch,fch);
		show_menu_line(i,&m1->element[i],bch,fch);
	};
 }
 drv_flush();

 i=iol=first;
 set_box_cline(iol);

 if(orient==VERTICAL){
	show_menu_line(i,&m1->element[i],COLOR_SELECT_BG,fch);
 } else	{
 	hdab(sx+i*HLABEL_WIDTH,m1->element[i].high,m1->element[i].txt,COLOR_SELECT_BG,fch);
 };
 drv_flush();
 /* get keyboard */
 while(1) {
	if(orient==HORIZONTAL && ftime==1) {
		i=first;
		ftime=0;
		execf=new_line;
	} else {
//		MESG("xupdate_box");
		xupdate_box();
		c=getcmd();
		execf = key_function(c,0);
		execf = key_convert_from_mouse(execf);
		if(execf == do_nothing) continue;
		if(execf==0) {
			c=normalize(c);
			for(j=0;j<nu;j++) {
			 if(c==m1->element[j].high) break;
			};
			if(j==nu) continue;	/* while loop */
			i=j;
			if( m1->element[i].type == MMENU) {
				execf = new_line;
				if(orient==VERTICAL) {
					show_menu_line(iol,&m1->element[iol],bch,fch);
				} else {
					hdab(sx+HLABEL_WIDTH*(iol),m1->element[iol].high,m1->element[iol].txt,bch,fch);
				};
				if(orient==VERTICAL){
					show_menu_line(i,&m1->element[i],COLOR_SELECT_BG,fch);
				}else {
					hdab(sx+HLABEL_WIDTH*(i),m1->element[i].high,m1->element[i].txt,COLOR_SELECT_BG,fch);
				};
				iol=i;
				set_box_cline(iol);
			} else { 
				execf = m1->element[i].func ;
				repeat_arg = (long int)m1->element[i].menu;
			};
	
			if(execf!=new_line) {
//				MESG("not new_line!");
			if(orient==VERTICAL) { 
//				MESG("remove box2");
				remove_box();
			} else refresh_menu(); 
	
			if(orient==HORIZONTAL) {
				if(execf==prev_character) {
					i--;if(i<0) i=nu-1;
					execf=new_line;
				};
				if(execf==next_character) {
					i++;if(i>nu-1) i=0;
					execf=new_line;
				};
			};
			 return(execf);
			};
		};
	};
	if(execf!=0) {
	 if( execf == next_line && orient == HORIZONTAL) execf=new_line;
	 if( execf == new_line) { 
	  while(execf == new_line) {
		if( m1->element[i].type == MMENU) {
			int op=orient;
			if(orient == HORIZONTAL) {
				execf = get_menucmd(m1->element[i].menu,0,iol*HLABEL_WIDTH,1) ;
			} else {
//				MESG("goto menu");
				MENUS *submenu=m1->element[i].menu;
				execf = get_menucmd(submenu,0,pos_x+maxl,submenu->ypos) ;
//				MESG("returned from submenu");
				remove_box();
				return execf;
			};
			orient=op;
		} else {
			execf = m1->element[i].func ;
			repeat_arg = (long int)m1->element[i].menu;
		};

		if ( m1->element[i].type == MFUNCTION) {
			if(orient==VERTICAL) {
				remove_box();
			} else refresh_menu();
			return(execf);
		};
		if ( execf==prev_line || execf==prev_character) {
//			MESG("prev_line or prev_character: orient=%d",orient);
			i--; if(i<0) i=nu-1;
			execf=new_line;
		};

		if ( execf==update_menu) {
			i = get_menu_index_from_mouse();
			if(i<0) execf=abort_cmd;
			else execf=new_line;
		};

		if ( execf==page_move_down) {
			i-=10; if(i<0) i=0;
		};
		
		if (execf==next_line || execf==next_character) {
			i++;if(i>nu-1) i=0;
//			MESG("next_line -> new_line:");
			execf=new_line;
		};
		if (execf==page_move_up) {
			i+=10; if(i>nu-1) i=nu-1;
		};
		if(execf==NULL) refresh_menu();

		if(execf != new_line) {
			if(orient==VERTICAL) {
				if(execf!=noop) {
					remove_box();
				} else {
					continue;				
				};
			} else refresh_menu();
			return(execf);
		} else {	/* highlight correct menu element */
//		MESG("highlight correct menu element iol=%d",iol);
		if(orient==VERTICAL) {
			show_menu_line(iol,&m1->element[iol],bch,fch);
		} else {
			hdab(sx+HLABEL_WIDTH*(iol),m1->element[iol].high,m1->element[iol].txt,bch,fch);
		};
		if(orient==VERTICAL){
			show_menu_line(i,&m1->element[i],COLOR_SELECT_BG,fch);
		} else {
			hdab(sx+HLABEL_WIDTH*(i),m1->element[i].high,m1->element[i].txt,COLOR_SELECT_BG,fch);
		};
		iol=i;
		set_box_cline(iol);
		};
	  };
	 };
	 if( execf == abort_cmd) { 
		if(orient==VERTICAL)remove_box();
		else refresh_menu();
		return(NULL);
	 };
	 if(orient==VERTICAL) {
	 	if(execf==goto_line) {
			i=get_box_cline();
		};
		if(execf==prev_character || execf==next_character || execf==update_menu) {
//			MESG("execf=prev_character");
			remove_box();
			return(execf) ;
		};
		if(execf==prev_line||execf==scroll_up) {
			i=iol-1;if(i<0) i=nu-1;
		};
		if(execf==next_line||execf==scroll_down) {
			i=iol+1;if(i>nu-1) i=0;
		};
		if ( execf==page_move_down) {
			i=iol-10; if(i<0) i=0;
		};
		if (execf==page_move_up) {
			i =iol+10; if(i>nu-1) i=nu-1;
		};

		if (execf==mouse_move) {
			int line = get_mouse_box_line();
			if(line) {
				i = line-1;
			} else  continue;
		};
	if(orient==VERTICAL) {
//		MESG("orient=vertical iol=%d i=%d",iol,i);
		if(iol!=i){
		show_menu_line(iol,&m1->element[iol],bch,fch);
		show_menu_line(i,&m1->element[i],COLOR_SELECT_BG,fch);
		};
	} else {
		hdab(sx+HLABEL_WIDTH*(iol),m1->element[iol].high,m1->element[iol].txt,bch,fch);
		hdab(sx+HLABEL_WIDTH*(i),m1->element[i].high,m1->element[i].txt,COLOR_SELECT_BG,fch);
	};
	}
	iol=i;
	set_box_cline(iol);
	};
// 	MESG("loop while 1");
 };
}

MENUS *start_menu = &m_topn;
extern int mousey,mousex;

int execute_menu(int fixed)	/* execute menu */
{
 register int status=FALSE;
 register int (*execfunc)();		/* ptr to function to execute */
 int line=1;
 short key;
 long int repeat = repeat_arg;
 MENUS *active_menu;
 int pos_x=0;
 int pos_y=0;

 active_menu = start_menu;

 if(kbdmode==PLAY||kbdmode==BPLAY){

	key = get_next_key(kbdmode);

	int line2;
	MENUS *m1;

 	key=normalize(key);
	for(line=0;;line++) 
	{
	if(active_menu->element[line].high==0) {line=0;break;};
	if(active_menu->element[line].high==key) break;
	};
	m1 = active_menu->element[line].menu;

	key = get_next_key(kbdmode);

	for(line2=0;;line2++) 
	{
	if(m1->element[line2].high==0) {line2=0;break;};
	if(m1->element[line2].high==capital(key)) break;
	};
	
	execfunc = get_menucmd(m1,line2,0,line2);
	if(execfunc != NULL) status=(*execfunc)(repeat_arg); // menu_command
	repeat_arg=repeat;

	in_menu=0;
	return(status);
 } else {
 	key=normalize(lastkey);

	for(line=0;;line++) 
	{
	if(active_menu->element[line].high==0) {line=0;break;};
	if(active_menu->element[line].high==capital(key)) break;
	};
	record_key(key);
//	MESG("menu key: %c",key);
 }
 if(macro_exec) return 1;
 if(fixed==0) line=-1;
 if(active_menu->orient==HORIZONTAL) {
 	pos_x = line*HLABEL_WIDTH;
	pos_y = 1;
	 } else {
	// MESG(" fixed = %d mouse x=%d y=%d",fixed,mousex,mousey);
	if(fixed<0) {
		pos_x=mousex;
		pos_y=mousey;
	} else {
	 	pos_x = active_menu->xpos;
		pos_y = active_menu->ypos;
	};
 };
 execfunc = get_menucmd(active_menu,line,pos_x,pos_y);
 in_menu=0;
 clear_message_line();

 if(execfunc != NULL) status=(*execfunc)(repeat_arg); // menu_command

 repeat_arg=repeat;
 return(status);
}

int menu_command(int n)
{
	start_menu = &m_topn;
	return execute_menu(1);
}

 /* new_wp, new_line, new_column have been set */
void move_to_new_position(num new_column,num new_line)
{
//	MESG("move_to_new_position: col=%ld line=%ld",new_column,new_line);
	set_goal_column(new_column,"move_to_new_position");
	MoveLineCol(new_line,cwp->goal_column);
	update_status();
}

extern int drv_type;

/* m = array of elements
 * nu = number of elements
 * max_height = number of elements to show (height)
 * sx,sy = position
 * width = width 
 */
int selectl(char *title,char *m_array[],int nu,int max_height,int sx,int sy,int width,int active)
{
 int i,c,iol,start;
 int found=0;
 int border=0;

 char name[MAXFLEN];
 int (*execf)();
 char *title_shown=strdup(title);
 int slen=0; 


 if(nu==0) { return(-1);};
 if(nu==1) return(0);
 hide_cursor("selectl");
 border = (strlen(title)==0);

 activate_list_mode();
// MESG("selectl:[%s] nu=%d max_height=%d",title,nu,max_height);

 if(!macro_exec) {
 start=0;
 in_menu=1;
 in_selection=1;
 if(nu<max_height) max_height=nu;
 if(sy+max_height > drv_numrow) max_height=drv_numrow - sy;

 disp_box(title_shown,border,sy,sx,sy+max_height+1,sx+width+1);
 free(title_shown);

 start=disp_list(m_array,start,nu,max_height,sx,sy,width,active);
 i=0;
 box_line_print(i,0,m_array[i],width,1,active);
 iol=0;
 set_box_cline(0);
 for(;;) {
	xupdate_box();

	c=getcmd();
	execf = key_function(c,0);

	execf = key_convert_from_mouse(execf);

	if(execf==do_nothing) continue;
	if(execf==new_line) {
		break;
	};
	if(execf==abort_cmd||execf==paste_region||execf==menu_command ) {
		break;
	};
	if(execf==mouse_move) {
			int line = get_mouse_box_line();
			if(line) {
				i=start+line-1;
			} else  continue;
	} else
	if(execf == goto_line) {  // get next character as number in list (xwin)
		i=get_box_cline()+start;
		if(i==iol) {
			execf=new_line;
			break;
		};
	} else
	if(execf==goto_bol) {
			start=0;
			start=disp_list(m_array,start,nu,max_height,sx,sy,width,active);
			i=0;
		} else
	if(execf==prev_line || execf==scroll_down) {
		if(i<1) continue;
		i=iol-1;
		// MESG(":	prev_line i=%d",i);
		if(i<start) {
			start=i;
			start=disp_list(m_array,start,nu,max_height,sx,sy,width,active);
		};
	} else
	if(execf==prev_page || execf==page_move_down) {
			if(i<max_height/2) i=0;
			else i = iol-max_height/2;

			if(i<start) {
				start=start-max_height/2;
				if(start<0) start=0;
				start=disp_list(m_array,start,nu,max_height,sx,sy,width,active);
				i=start;
				box_line_print(i,start,m_array[i],width,1,-1);
				iol=i;
			} else {
				box_line_print(iol,start,m_array[iol],width,0,active);
				box_line_print(i,start,m_array[i],width,1,active);
				iol=i;
			};
			set_box_cline(iol);
			continue;
		} else
	if(execf==next_page || execf==page_move_up) {
			if(i>=nu) continue;
			i=iol+max_height/2;
			// MESG(":	next_page: iol=%d start=%d i=%d nu=%d",iol,start,i,nu);
			if(i>=nu) i=nu-1;
			if(i-start>=max_height) {
				if(start+max_height/2+max_height>nu) {
					start=nu-max_height;
					i=nu-1;
				};
			start = disp_list(m_array,start+max_height/2,nu,max_height,sx,sy,width,active);
				box_line_print(i,start,m_array[i],width,1,active);
				iol=i;
				set_box_cline(iol);
				continue;
			} else {
				box_line_print(iol,start,m_array[iol],width,0,active);
				box_line_print(i,start,m_array[i],width,1,active);
				iol=i;
				set_box_cline(iol);
				continue;
			};
		} else
	if(execf==next_line || execf==scroll_up) {
			i=iol+1;
			if(i>=nu) continue;
			// MESG(": next_line iol=%d i=%d max=%d",iol,i,max_height);
			if(i-start>=max_height) {
			start = disp_list(m_array,start+1,nu,max_height,sx,sy,width,active);
			};

			// set_box_cline(i);
	} else
	if(execf==del_prev_char) {
			if(slen>0) {
//				MESG("delete previous char [%s]",search_string);
//				search_string[slen-1]=0;
			};
	}	else /* continue; */
	{
	 int j;
		if(c == (CNTRL | CHR_LBRA)) { continue;};
#if	1
		if(c>127 && c<160) {	/* this is input from xlib mouse selection  */
			i=c-128;continue;
		};
#endif
		if(c>='a' && c<='z') c-=32; 
		c=capital(c);

		i=iol+1;	/* start searching from the next line!  */
		if(i>=nu) { continue;};
		while(1) {
			for(j=i;j<nu;j++) {
//				MESG("check start=%d i=%d j=%d c=%c s=%c",start,i,j,c,m_array[j][0]);
				if(c==m_array[j][0] || c==m_array[j][0]-32) 
				{
					i=j;
//					MESG("	found at j=%d i=%d",j,i);
					found=1;
					break;
				};
			};
			if(found) break;
			if(i==0) break;
			i=0;
		};
		if(j>=nu) continue;

		// MESG(": found c=%c start=%d i=%d iol=%d",c,start,i,iol);
		if(i>start+max_height-1) {
			start=disp_list(m_array,i,nu,max_height,sx,sy,width,active);
			box_line_print(0,0,m_array[i],width,1,active);
		} else {
			if(i<start) start=i;
			start=disp_list(m_array,start,nu,max_height,sx,sy,width,active);
			box_line_print(i,start,m_array[i],width,1,active);
		};
		iol=i;

	};
#if	NUSE
	if(iol<start) {
		start=disp_list(m_array,iol,nu,max_height,sx,sy,width,active);
	};
#endif
	if(!found) { 
		// MESG(">> start=%d current y=%d new y=%d",start,iol,i);
		box_line_print(iol,start,m_array[iol],width,0,active);
		box_line_print(i,start,m_array[i],width,1,active);
		iol=i;
	} else found=0;
	set_box_cline(iol);
 };
 remove_box();
 msg_line("");
 drv_flush();
 in_menu=0;
 in_selection=0;

 if(execf == abort_cmd ) { list_off();return(-1);};
 if(execf == paste_region ) { list_off(); return(-10);};
 if(execf == new_line ) { 
	// MESG(": new_line: i=%d iol=%d",i,iol);
 	list_off();return(i);
 };
 if(execf == menu_command) {
 	list_off(); return menu_command(1);
 }
 } else {
	name[0]=0;
	nextarg("",name,MAXFLEN,true);
 };
 list_off();
 return(-1);
}

int disp_list(char **m_array,int start,int nu,int max_lines,int sx,int sy,int width,int active)
{
 register int i;
 // MESG("disp_list: < start=%d lines=%d",start,max_lines);
 if(start+max_lines>nu) start=nu-max_lines;
 for(i=start;i<start+max_lines;i++) {
	box_line_print(i,start,m_array[i],width,0,active);
 };
 // MESG("---->");
 drv_flush();
 return(start);
}

/*
 * Quote the next character, and insert it into the buffer. All the characters
 * are taken literally. Bound to "^Q"
 */
int quote(int n)
{
	int    c;
	if(dont_edit()) return FALSE;
	if(kbdmode==BPLAY||kbdmode==PLAY) {
		char c=get_next_key(kbdmode);
		insert_chr(n,c);
		return TRUE;
	};

    c = drv_getc(1);
    return (insert_chr(n, c));
}

void events_flush() {}	

extern BOX *cbox;

void update_cursor_position()
{
	// MESG("update_cursor_position: entry_mode=%d row=%d col=%d",entry_mode,cwp->currow,WGetCol());
	if(!entry_mode)
	{
		if(!in_menu) {
#if	TNOTES
			if(cwp->w_fp->b_flag == FSNOTES) movecursor(cwp->current_tag_line-cwp->top_tag_line+(cwp->w_fp->b_header!=NULL), WGetCol());
			else if(cwp->w_fp->b_flag & FSNOTESN) movecursor(cwp->current_note_line-cwp->top_note_line+(cwp->w_fp->b_header!=NULL), WGetCol());
			else 
#endif
			if(cwp->w_fp->b_flag & FSNLIST) movecursor(cwp->current_note_line-cwp->top_note_line+(cwp->w_fp->b_header!=NULL), 0);
			else movecursor(cwp->currow+(cwp->w_fp->b_header!=NULL), WGetCol());
			show_cursor("update_cursor_position");
		};
	};
}

void clear_string(char *string,int max)
{
 char *s = string;
 while(s<string+max) *s++ = 0;
}

/* Display prompt, edit start string */
int win_getstring(WINDOW *disp_window,char *prompt, char *st1,int maxlen,int disinp)
{
 int prompt_len;
 int ys=0;	// y current position
 int cursor_x;	// x current position
 unsigned int ce=0;	/* position in string  */
 unsigned int ce1=0;	/* position in shown string  */
 int quotef;
 int ci;	/* character to insert  */
 int cw=0;	/* character width  */
 int (*execf)();
 utfchar uc;
 char st[MAXLLEN];	// this is the string to display (asterix for encrypted)
 char st2[MAXLLEN];	// this is the real string
 int saved_kbdmode=kbdmode;
 if(maxlen>MAXLLEN-1) maxlen=MAXLLEN-1;
 if(execmd) {
	printf("%s",prompt);
	if(fgets(st1,maxlen,stdin)==NULL) return false;
	return(true);
 };

 if(kbdmode==PLAY) { // play string
		return(set_play_string(st1));
 };
#if	RSESSION
 if(kbdmode==BPLAY) return(set_play_string1(st1));
#endif
 hide_cursor("getstring");
 quotef=FALSE;
 entry_mode=KENTRY;
 init_input();
 msg_line("");
 
 strlcpy(st2,st1,MAXLLEN);

 /* get current position prompt_len,ys */
#if	PCURSES
 ys=0;
#else
 ys=drv_numrow;
#endif
 cursor_x = dspv(disp_window,0,ys,prompt);
 prompt_len=cursor_x;
 /* display start string */
 ce=0;
 ce1=0;

 strlcpy(st,st1,511);
 cursor_x += dspv(disp_window,cursor_x,ys,st);

 ce=strlen(st2);
 ce1=ce;

 quotef=FALSE;	/* we start with no escape for special chars  */
 kbdmode=0;
 for(;;) {
   e_pos = cursor_x;
   show_cursor_dl(cursor_x);
   if( quotef ) {
       /* handle control insert */
	ci = get1key();

    if( !(ci & CNTRL) ) { quotef=0;continue;};
	if(ci & CNTRL) ci -= CNTRL + '@';
	inspace(st2,ce);
	if(disinp) inspace(st,ce1);
	st2[ce]=ci;
	if(disinp)	st[ce]=ci;else st[ce1]='*';
	ce++;
	dspv(disp_window,cursor_x,ys,st+ce1);ce1++;
	
    cursor_x += 2;
	quotef=0;
    continue;
   } else {
	usleep(1000);
	ci=getcmd();
   };
   execf = key_function(ci,0);
	execf = key_convert_from_mouse(execf);
 	if(execf==do_nothing) continue;
	if(execf==mouse_move) continue;
	if(execf !=NULL) {
    if(execf == quote) { quotef=1;continue;};
    if(execf == prev_character ) {
//		MESG("prev_character:1");
		if(ce>0) {
		  ce=utf8_str_left(st2,ce);

		  SUtfCharAt(st2,ce,&uc);
		  if(disinp) {
		  	ce1=ce;
			cursor_x -= get_utf_length(&uc);
		  } else {
		  	ce1--;
			cursor_x--;
		  };
		  if(st2[ce]<32 && disinp) cursor_x--;
		};continue;
     };
     if(execf == next_character ) {
		if(ce<strlen(st2)) {
		  if(st2[ce]<32 && disinp) cursor_x++;	/* this is a quoted control character  */

		  ce = SUtfCharAt(st2,ce,&uc);
		  if(disinp) {
		  	ce1=ce;
			cursor_x+=get_utf_length(&uc);
		  } else {
		  	ce1++;
			cursor_x++;
		  };
		};continue;
	  };
      if(execf == goto_bol ) {
		ce=0;ce1=0;
		cursor_x=prompt_len;
		continue;
      };
      if(execf == goto_eol ) {
		ce=strlen(st2);
		ce1=strlen(st);
		cursor_x=prompt_len+dspv(disp_window,prompt_len,ys,st);
		continue;
      };

      if(execf == del_char ) {
	  	if(ce<strlen(st2)) {
		cw=utf8_countbytes((int)st2[ce]);
		while(cw>0){
			delchar(st2,ce);cw--;
			if(disinp) delchar(st,ce1);
		};
		dspv(disp_window,cursor_x,ys,st+ce1);
		};
		continue;
      };
	  if(execf == erase_eol) {
	  	st2[ce]=0;
		st[ce1]=0;
		dspv(disp_window,cursor_x,ys,st+ce1);
		continue;
	  };
	  if(execf == cut_region) {	/* remove all chars  */
		if(ce>0) {		
			cw=0;
			st2[0]=0;
			st[0]=0;

		    ce=0;ce1=0;
			cursor_x=prompt_len;
			cursor_x = dspv(disp_window,0,ys,prompt);
		};
		continue;
	  };
      if (execf == new_line) {
		if (check_list_type(LDIR) && isspecialchar(st2)) execf = find_next;
		else break;
      };

	  if (execf == next_line || execf == prev_line || execf == tab) {
	  	if(execf == prev_line) list_select(st2,-1);
		if(execf == next_line) list_select(st2,+1);
		if(execf == tab) {
//			MESG("tab: [%s]",st2);
			if(check_list_type(LDIR2)) strlcpy(st2,st1,MAXLLEN);
			else {
				list_select(st2,0);
			};
		};
		ce=strlen(st2);
		if(disinp) {strlcpy(st,st2,MAXLLEN);ce1=ce;}
		else {
			ce1=utf_num_chars(st2);
			memset(st,'*',ce1);st[ce1]=0;
		};
		cursor_x=prompt_len+dspv(disp_window,prompt_len,ys,st);
		continue;
	  };

     if(execf == abort_cmd ) {
		st1[0]=0;
		hide_cursor_dl();
		entry_mode=KNORMAL;
		clear_message_line();
		list_off();
		lastkey=0;
		return(FALSE);
     };
     
     if( execf== del_prev_char ) { /* delete previous character */
	 if(ce<1) continue;
//		Go back one character
		ce=utf8_str_left(st2,ce);

		SUtfCharAt(st2,ce,&uc);
		if(disinp) {
			cursor_x -= get_utf_length(&uc);
		} else {
			cursor_x--;
		}
		ci=st2[ce];
		if(disinp) ce1=ce;else ce1--;
		if(ci<32) {
			cursor_x--;
			if(disinp) ce1--;
		};
		cw=utf8_countbytes(ci);

		if(disinp) {
			if(ci<32){
				delchar(st2,ce);
				delchar(st,ce1);
			} else {
				while((cw--)>0){
					delchar(st,ce1);
					delchar(st2,ce);
				};
			};
		} else {
			while((cw--)>0){
				delchar(st2,ce);
			};
			delchar(st,ce1);
		};
		dspv(disp_window,cursor_x,ys,st+ce1);
		continue;
     };
	 if(execf == paste_region) {
		strlcpy(st,getcurcword(),511);
		strlcpy(st2,st,511);	 
		ce=strlen(st2);
		ce1=strlen(st);
		cursor_x=prompt_len+dspv(disp_window,prompt_len,ys,st);
	 };
	 if(execf == system_paste) {
	 	strlcpy(st,ext_system_paste_line(),511);
		strlcpy(st2,st,511);	 
		ce=strlen(st2);
		ce1=strlen(st);
		cursor_x=prompt_len+dspv(disp_window,prompt_len,ys,st);
	 };
     } else
     {
		int w,wlen;
    	if(ci==' ' && check_list_type(LCOM)) {
     		list_select(st,1); strlcpy(st2,st,MAXLLEN);
			ce=strlen(st);
			cursor_x=prompt_len+dspv(disp_window,prompt_len,ys,st);
			continue;
     	};
    if(ce>(unsigned)(maxlen-1)) continue;

     /* handle normal insert */
	if(utflen>1) {
		wlen=utflen;
		if(wlen>1) for(w=0;w<wlen;w++) uc.uval[w]=utfokey[w];	/* move it to tmp buffer  */
		
		for(w=0;w<wlen;w++) {
			inspace(st2,ce);
			if(disinp) inspace(st,ce1);
		};
		if(disinp) {
			for(w=0;w<utflen;w++) {
				st2[ce+w]=uc.uval[w];
				st[ce1+w]=uc.uval[w];
			};
		} else {
			for(w=0;w<utflen;w++) {
				st2[ce+w]=uc.uval[w];
			};
			st[ce1]='*';
		};
		w=utflen;
	} else {
		inspace(st2,ce);
		inspace(st,ce1);
		uc.uval[0]=ci;
		uc.uval[1]=0;
		st2[ce]=ci;
		if(disinp) {
			st[ce1]=ci;
		} else {
			st[ce1]='*';
		};
		w=1;
	};
		dspv(disp_window,cursor_x,ys,st+ce1);
		utflen=0; utfokey[0]=0;
	 ce+=w;
	 if(disinp) ce1+=w;else ce1++;
	 if(disinp) {
		cursor_x+=get_utf_length(&uc);
	 } else {
     	cursor_x++;
	 }
     if(ci<32 && disinp) cursor_x++;
   };
 };
#if	USE_GLIB
 if(cbfp->b_lang == 0) {	// default lang is UTF8, no convertion
	strlcpy(st1,st2,MAXLLEN);
 } else {	// convert to local buffer character set
 	char *ustr;
 	gsize r,w;
 	ustr = g_convert(st2,-1,codepage_str[cbfp->b_lang],"UTF-8",&r,&w,NULL);
 	strlcpy(st1, ustr ,MAXLLEN);
 	g_free(ustr);
 };
#else
	strlcpy(st1,st2,MAXLLEN);
#endif
 
 list_update(st2);
 kbdmode=saved_kbdmode;
 // record string
 set_record_string(st2);

 clear_string(st2,strlen(st2)); // for security reasons on password input
 hide_cursor_dl(); 

 end_interactive();
// clear_message_line();
 return(TRUE);
}

/* delete a character in a string at position */
int delchar(char *st,int pos)
{
 if(pos<0) return(-1);
 memmove(st+pos,st+pos+1,strlen(st)-pos);	/* UTFSAFE ?? */
 return(1);
}

/* insert space in a string at position */
int inspace(char *st,int pos)	/* UTFSAFE ?? */
{
 if(pos<0 || (unsigned)pos>strlen(st)) return(-1);
 st[strlen(st)+1]=0;
 memmove(st+pos+1,st+pos,strlen(st)-pos);

 st[pos]=' ';
 return(1);
}


void main_loop()
{
 int c;
 init_message_loop(); 
 change_color_scheme(color_scheme_ind+1);
 while(1) { /* main keyboard loop */
	/* Fix up the screen    */
//	MESG("main_loop:0 buffer=%d [%s][%s] -----",cbfp->b_index,cbfp->b_dname,cbfp->b_fname);
    update_screen(FALSE);
	/* get the next command from the keyboard */
	app_error=0;
	c = getcmd();

	/* execute the keyboard sequence */
	main_execute(c);
 };
}


int show_version(int n)
{
 // MESG("show_version:");
 msg_line("This is %s version %s !",APPLICATION_NAME,VERSION);
 return(FALSE);
}

// info size according to show_position_info in screen.c
int  show_position_size(WINDP *wp,int short_version)
{
	int size=0;

//	MESG("show_position_info: o=%ld loff=%ld",Offset(),loffs);
	if(wp->w_fp->view_mode & VMHEX) {
		if(short_version==0) 
			size=17;
		else size=6;
	} else {
	  if(wp->w_fp->b_flag & (FSDIRED|FSNLIST)) {
	  	if(!short_version) {
			// sstat=dir_getfile(finfo,2);
			// sstat=snprintf(str,MAXSLEN,"%4lld|%s",getcline()+1,finfo);
			size=24;
		} else {
			// sstat=snprintf(str,MAXSLEN,"%4lld",getcline()+1);
			size=4;
		};
	  } else {
		if(short_version) {
			// sstat=snprintf(str,MAXSLEN,"%6lld",getcline()+1);
			size=6;
		} else {
			if(Eol()) {
				if(bt_dval("show_coffset")) {
					// sstat=snprintf(str,MAXSLEN,"%5lld %5lld %3d ",getcline()+1,Offset(),col);
					size=16;
				} else {
					// sstat=snprintf(str,MAXSLEN,"%5lld %3lld %3d ",getcline()+1,loffs,col);
					size=14;
				};
				if((int)bt_dval("show_cdata")) { 
					// if(cwp->w_fp->b_mode & EMDOS) strlcat(str,"0D0A",MAXSLEN);
					// else if(cwp->w_fp->b_mode & EMUNIX) strlcat(str,"000A",MAXSLEN);
					// else if(cwp->w_fp->b_mode & EMMAC) strlcat(str,"000D",MAXSLEN);
					// else strlcat(str,"    ",MAXSLEN);
					size += 4;
				};
			} else {
				// sstat=snprintf(str,MAXSLEN,"%5lld ",getcline()+1);
				size=6;
				if(bt_dval("show_coffset")) {
					// sstat=snprintf(str+strlen(str),MAXSLEN-strlen(str),"%5lld ",Offset());
					size+=6;
				} else {
					// sstat=snprintf(str+strlen(str),MAXSLEN-strlen(str),"%3lld ",loffs);
					size+=4;
				};
				// sstat=snprintf(str+strlen(str),MAXSLEN-strlen(str),"%3d ",col);
				size+=4;
				if((int)bt_dval("show_cdata")) {
					// sstat=snprintf(str+strlen(str),MAXSLEN-strlen(str),"%04lX",utf_value());
					size+=6;
				};
			};
		};
	  };
	};
	// MESG("pos_size=%d",size);
	return size;
}

// Old style statusline
void status_line(WINDP *wp)
{
    char *cp;
    int c;
    int n;		/* cursor position count */
    FILEBUF *bp;
    int i;			/* loop index */
    char lchar;		/* character to draw line in buffer with */
	int max_t;
	char status_string[512], *stp;
	char fdname[MAXLLEN];
	int hline1 = '-';	/* or 18  */
	int hline2 = '=';	/* or 15  */
	char modecode[]=MODECODE;

	if(!discmd) return;

    bp = wp->w_fp;
	if(bp==NULL) return;
	if(wp->w_ntcols>NUM_COLS_SHORT) strlcpy(fdname,bp->b_dname,MAXLLEN);
	else strlcpy(fdname,bp->b_fname,MAXLLEN);

	if(wp->w_ntcols > NUM_COLS_FULL) max_t = wp->w_ntcols - NUM_COLS_FINFO - show_position_size(wp,0);
	else if(wp->w_ntcols > NUM_COLS_SHORT) max_t = wp->w_ntcols - NUM_COLS_FINFO - show_position_size(wp,1);
	else if(wp->w_ntcols > NUM_COLS_NOINFO) max_t = wp->w_ntcols - show_position_size(wp,-1);
	else max_t = wp->w_ntcols-2;

#if	!CLASSIC_STATUS
	if(drv_colors>8) 
		lchar = ' ';
	else
#endif
		lchar = (wp == cwp) ? hline2 : hline1;	/* mark the current buffer */

	stp = status_string;

//	in debug mode print window number at bottom left
	if(debug_flag()){
		char snum[10];
		snprintf(snum,10,"%d",wp->id);
		*stp++ = snum[0];
		if(snum[1]!=0) *stp++ = snum[1];
	};

    *stp++ = lchar;

	n=1;
    /* display the modes */
	*stp=0;

	if(wp->w_ntcols-2 > NUM_COLS_SHORT){	/* not in the very short mode!  */
#if	TNOTES
		if(bp->b_type & NOTE_TYPE 
			|| bp->b_type & NOTE_CAL_TYPE
			|| bp->b_type & NOTE_TODO_TYPE
		) {
			*stp++='N';n++;
		};
#endif
		if(wp->w_fp->b_flag & FSDIRED){
			if(wp->w_fp->b_state & FS_VIEW) { *stp++ = 'V'; n++;}
			else {*stp++ = 'D'; n++;};
		} else {
			if(wp->w_fp->b_state & FS_VIEW) {*stp++ = 'V'; n++;}
		};
		if(wp->w_fp->b_flag & FSMMAP) {*stp++ = 'M'; n++;};
		if(wp->w_fp->b_flag & FSNLIST) {*stp++ = 'C'; n++;};

		if(wp->w_fp->b_state & FS_CHG) { *stp++ = '*';}
		else { *stp++ = lchar;}; n++;

		if(gmode_over) { *stp++ = 'O';} else {*stp++ = lchar;};
		n++;

		for (i = 0; i < 8 ; i++)	/* add in the mode flags */
			if (wp->w_fp->b_mode & (1 << i)) {
				*stp++ = modecode[i];n++;
			};
		if(wp->w_fp->bom_type == FTYPE_UTF8BOM) { *stp++ = '8';n++;};
		if(wp->w_fp->bom_type == FTYPE_UTF16BOM) { 
			*stp++ = '1';n++;*stp++ = '6';n++;
		};
	    *stp++ = lchar;
	    // *stp++ = lchar;
	    *stp++ = ' ';
	    n += 2;
		*stp = 0;
	};

	/* directory name,file name */
#if	USE_GLIB
	char *composite;
	char cp_data[MAXLLEN];
	char *full_name;
	cp=cp_data;
	// MESG("status_line: fdname [%s] l=%d",fdname,strlen(fdname));
	composite = g_utf8_normalize(fdname,-1,G_NORMALIZE_ALL_COMPOSE);
	// MESG("           : composite [%s]",composite);
	if(wp->w_ntcols>NUM_COLS_SHORT) {
	full_name = get_pfname(composite,bp->b_fname,max_t-1);
	// MESG("           : full_name [%s] l=%d max_t=%d",full_name,strlen(full_name),max_t);
	strlcpy(cp,full_name,MAXLLEN);
	} else {
	
	strlcpy(cp,bp->b_fname,MAXLLEN);
	};
	// MESG("           : cp        [%s]",cp);
	g_free(composite);
#else
	char cp_data[MAXLLEN];
	char *full_name;
	full_name = get_pfname(fdname,bp->b_fname,max_t-1);
	cp=cp_data;
	strlcpy(cp,full_name,MAXLLEN);
#endif
	// MESG("           : freed!");
	int utf_ntcols = wp->w_ntcols + strlen(cp) - utf_num_chars(cp);
	for(;(c= *cp);n++) {
// 		if((stp-status_string)>(wp->w_ntcols-1)) break;
		*stp++ = c;
		cp++;
	};
	*stp++ = ' ';

    while (n++ < utf_ntcols-21)  /* Pad to full width. */
    {
		*stp++ = lchar;
    }
    while (n++ <= utf_ntcols)  /* Pad to full width. */
    {
		*stp++ = ' ';
    }
	*stp=0;

// 	MESG("   status   : [%s] %d",status_string,max_t);
	put_string_statusline(wp,status_string,0);
	wp->w_flag &= ~UPD_STATUS;
	if(wp==cwp) titletext();
	show_slide(wp);
}

// dummy commands
int increase_font_size(int n)
{
	return true;
}

int decrease_font_size(int n)
{
	return true;
}

int toggle_bold(int n)
{
	return true;
}

void freelist(char **namelist, int n)
{
 int i;
 for(i=0;i<n;i++) free(namelist[i]);
 free(namelist);
}

// show a dir list in a selection window
void list_dir1(char *st)
{
 int i;
 int nam1;
 int type;
 char **file_names;
 char **ddnames;
 char dirname[MAXLLEN];
 alist *dir_list;
 char *dir_name;
 int window_width=72;
 int i1;
 int n=0;	/* number of files in the directory */
 int lheight;
 struct stat t;
 int max_cols = drv_numcol;
 // MESG("list_dir1:[%s] window_width=%d max_cols=%d",st,window_width,max_cols);
 if(window_width+10>max_cols) window_width = max_cols-10;
// set_list_type(LMAC);
 lheight = drv_numrow - 7;

 file_names=(char **)getdir(".",st,&n);

 dir_list = new_list(0,"list_dir1");

 for(i=0;i<n;i++)
 {
	i1=stat(file_names[i],&t);
	if(i1<0) continue;
	dir_name=strdup(str_tfile(&t,file_names[i],window_width+1));

	add_element_to_list((void *)dir_name,dir_list);
 };
 ddnames = (char **) array_data(dir_list);
 /* free the file name list */
 freelist(file_names,n);
 nam1=selectl("file name",ddnames,i,lheight,5,1,window_width,-1); /* 60 */
 if(nam1<0) {
  strcpy(st,"");type=0;
 } else {
  type=ddnames[nam1][0];

  if(type!='d'){
		char current_dir[MAXFLEN],*cwd;
		cwd=getcwd(current_dir,MAXFLEN);
		snprintf(st,MAXFLEN,"%s/%s",cwd,ddnames[nam1]+DIR_NAME_POSITION);
	} else strlcpy(dirname,ddnames[nam1]+DIR_NAME_POSITION,MAXFLEN);
 };

 free_list(dir_list,"dir_list1: dir_list");

 if(type=='d')
 {
  int s;
	s=chdir(dirname);
	if(s==0)
	{
		strcpy(st,"");
		list_dir1(st);
	} else {
		MESG("cannot change to dir [%s]",dirname);
	};
 };
 list_off();
}

// approximately find length of a character. this is almost black magic..
// Different for each platform, screen driver
int get_utf_length(utfchar *utf_char_str)
{
 int b0;
 if(clen_error) { return 1;};

 b0=utf_char_str->uval[0];
 if(b0==0) return 0;
 if(b0<128) return 1;
#if	DARWIN | 1
 // accents do not take space in mac 
 if((b0==0xCC || b0==0xCD)) {
	 if(utf_char_str->uval[1]< 0xB0) return 0;
	 return 1;
 };
#else
 // accents do take space in WSL exept when converted to composed.
 if((b0==0xCC || b0==0xCD) && utf_char_str->uval[1]< 0xB0) return 1;
#endif
 if(b0<0xE1) return 1;
 if(b0==0xE2) {
	int b1=utf_char_str->uval[1];
	if(b1 == 0x80) {
#if	DARWIN
		return 1;
#else
		int b2=utf_char_str->uval[2];
		if(b2==0x80) return 1; 	/* 1/4 en  */
		if(b2==0x8B) return 0;	/* zero space  */
		if(b2==0x8C) return 0;	/* zero space  */
		if(b2==0x8D) return 1;	/* zero space  */
		if(b2==0xA6) return 1;
		return 1;
#endif
	};
	if(b1==0x81 || b1==0x82) return 1; 	/* diacriticals, subscripts, currency symbols  */
	if(b1 == 0x83) {
		int b2=utf_char_str->uval[2];
		if(b2 > 0x8F && b2< 0xb1) return 0;	/* combining characters */
		return 1;
	};
	if(b1 == 0x84) return 1;
	if(b1 == 0x92) return 1;
#if	DARWIN
	if(b1==0x9C||b1==0x9D||b1==0x9E) return 1;
#endif
	if(b1==0x99) {
		int b2=utf_char_str->uval[2];
		if(b2==0x80||b2==0x82) return 1;
		return 2;
	};
	if(b1==0x9A) {
		int b2=utf_char_str->uval[2];
		if(b2==0xA1||b2==0xAA||b2==0xAB) return 2;
		return 1;
	};
	if(b1==0x8F||b1==0x9B||b1==0x9C||b1==0x9D) return 2;
	if(b1 > 0x83 && b1< 0xBA) {
		return 1; 	/* should be 2 in most cases ...  */
	};
 	return 2;
 };
#if	1
 if(b0==0xEA) {
 	int b1=utf_char_str->uval[1];
	if(b1==0x99) {
		return 1;
	};
	if(b1==0x9A) {
		return 1;
	};
	if(b1==0x9C) {
		return 1;
	}	
	if(b1==0x9D) {
		return 1;
	};
	if(b1==0x9E) {
		return 1;
	};
 };
#else
 if(b0==0xEA) {
 	int b1=utf_char_str->uval[1];
	if(b1==0x9A) {
		int b2=utf_char_str->uval[2];
		if(b2==0xAC) return 2;
		if(b2==0x84) return 2;
		if(b2==0x85) return 2;
		if(b2==0x86) return 2;
		if(b2==0x87) return 2;
		if(b2==0x92) return 2;
		if(b2==0x96) return 2;
		if(b2==0x97) return 2;
	};
	if(b1==0x9C) {
		int b2=utf_char_str->uval[2];
		if(b2==0xA8) return 2;
		if(b2==0xB2) return 2;
		if(b2==0xb4) return 2;
		if(b2==0xb6) return 2;
		if(b2==0xB8) return 2;
		if(b2==0xBA) return 2;
		if(b2==0xA6) return 2;
		if(b2==0xBC) return 2;
	};
	if(b1==0x99) {
		int b2=utf_char_str->uval[2];
		if(b2==0x8C) return 2;
		if(b2==0x90) return 2;
		if(b2==0x8D) return 2;
		if(b2==0x91) return 2;
		if(b2==0x92) return 2;
		if(b2==0x94) return 2;
		if(b2==0x96) return 2;
		if(b2==0x9A) return 2;
		if(b2==0xA2) return 2;
		if(b2==0xA4) return 2;
		if(b2==0xA6) return 2;
		if(b2==0xAC) return 2;
		if(b2==0xAD) return 2;
	};
	if(b1==0x9D) {
		int b2=utf_char_str->uval[2];
		if(b2==0x8E) return 1;
	};
	return 1;
 };
#endif

 if(b0>0xEA && b0<0xED) return 2;	/* Korean  */
 if(b0==0xEF) {
	int b1=utf_char_str->uval[1];
	if(b1==0xBC) {
		int b2=utf_char_str->uval[2];
		// if(b2==0x84) return 1;
		if(b2>0x80 && b2<0xC0)
		return 2;	/* comma with added space etc  */
	};
	if(b1==0xBD) {
		int b2=utf_char_str->uval[2];
		// if(b2==0x84) return 1;
		if(b2>=0x80 && b2<0xA1) return 2;
	};
 	return 1;
 };
 if(b0==0xF0) {
	int b1=utf_char_str->uval[1];
	if(b1==0x90) {
		int b2=utf_char_str->uval[2];
		if(b2==0x90) return 1;
		if(b2==0x91) return 1;
		return 2;
	};
	if(b1==0x9D) {
		int b2=utf_char_str->uval[2];
		if(b2==0x90) return 1;
		if(b2==0x91) return 1;
		if(b2==0x92) return 1;
		if(b2==0x93) return 1;
		if(b2==0x94) return 1;
		if(b2==0x9D) return 1;
		return 1;
	};
	if(b1==0x9F) {
		int b2=utf_char_str->uval[2];
#if	WSL | DARWIN
		if (b2==0x84) return 1;
		if (b2==0xA6) return 1;	/* not shown in Dawrin  */
#else
		if (b2==0x84) return 1;
#endif
		return 2;
	};
 	return 1;
 };
 if(b0==0xF1 || b0==0xF2) return 1;	/* do not exist in apple terminal  */
 if(b0==0xF3) return 1;
 if(b0==0xE1) {
	int b1=utf_char_str->uval[1];

	return 1;
	if(b1>0x83) return 2;
	
	return 1;
 };
 if(b0==0xE3) {
 	int b1=utf_char_str->uval[1];
	if(b1==0x82) {
		int b2=utf_char_str->uval[2];
		if(b2==0x99) return 0;/* dakuten */
		if(b2==0x9A) return 0;/* handakuten */
	}
 }
 return 2;
}

void set_window_font(WINDP *WP)
{
}

void drv_start_window_update(WINDP *wp)
{
}

#include "menu_common.c"

/* --- */
