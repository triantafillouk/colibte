/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

	Link list (double linked lists) support functions
	Geditdisplay Common code
*/

void drv_show_menu_popup(GtkWidget *popup_menu);
void move_to_new_position(WINDP *new_wp, offs new_offs, num new_line, int new_column);
num physical_column(num vcol);
int set_tag_view_position(int line,int column);
extern FILEBUF *cbfp;
int get_current_line();
int change_sort_mode(int mouse_col);
offs FNext_wrap_line(WINDP *wp,offs start,int num_lines,int top);

void update_full(int f);

// This is used in scrolling!
void move_window_lines(WINDP *wp,int lines)
{

	num current_line = get_current_line();

	// MESG("move_window_lines: id=%d f=%d current=%d lines=%d",wp->id,wp->w_flag,current_line,lines);
	if(current_line==0 && lines<1) return;
	
	hide_cursor("move_window_lines");
	move_window(-lines);
	if(next_line(lines)){
	upd_column_pos();
	check_cursor_position(wp);
	if(is_wrap_text(wp->w_fp)) 
	upd_all_wrap_lines(wp,"cb_set_position 3");
	else
	upd_all_virtual_lines(wp,"move_window_lines");
	draw_window(1,wp,"move_upate_window");

	// textpoint_set(cwp->tp_current,LineBegin(Offset()));
	tp_copy(wp->w_emark,wp->tp_current);
	wp->currow = window_cursor_line(wp);
	set_goal_column(0,"move_window_lines");
	wp->curcol=wp->goal_column;
	};
	// MESG("move_window_lines: %d %d",wp->currow, wp->curcol);
	movecursor(wp->currow, physical_column(wp->curcol)); 
	show_cursor("move_window_lines");
}

/* a general event handler  */
static gint
ge_edit_display_event(GtkWidget	*widget, GdkEvent *event)
{
 GeEditDisplay *wd;
 static int last_event=0;
 static int motion_button=0;
 static int position_x=0, position_y=0;
 static int mouse_x=0, mouse_y=0, prev_x=0, prev_y=0;
 static int event_sec=0;
 static int event_usec=0;
 static int double_click=0;
 
 // MESG("ge_edit_display_event:");
 g_return_val_if_fail(widget !=NULL,FALSE);
 g_return_val_if_fail(GTK_IS_EDIT_DISPLAY(widget),FALSE);
 wd = GTK_EDIT_DISPLAY(widget);
	if(event->type == GDK_KEY_PRESS) {
		on_dra0_key_press_event(widget, (GdkEventKey *)event, NULL );
		last_event = GDK_KEY_PRESS;
		return TRUE;
	};

 if(event->type == GDK_SCROLL) {
	int unow,now;
 	GdkEventScroll *ev = (GdkEventScroll *)event;
	struct timeval timev;
	// MESG("ge_edit_display_event: scroll");
	gettimeofday(&timev,NULL);
	unow=(int) timev.tv_usec;
	now=(int) timev.tv_sec;

	switch(ev->direction) {
		case GDK_SCROLL_UP: {
			int lines=-4;
			// MESG(" SCROLL_UP:");
			if(wd->wp != cwp) change_window(wd->wp);
			// if(tp_line(cwp->tp_current)<1) return FALSE;
			if(now==event_sec && unow-event_usec < 50000) return(FALSE);
			event_sec=now;event_usec=unow;
			last_event=GDK_SCROLL;

			move_window_lines(wd->wp,lines);
			return TRUE;
		};break;
		case GDK_SCROLL_DOWN: {
			int lines=4;
			// MESG(" SCROLL_DOWN:");
			if(wd->wp != cwp) change_window(wd->wp);
			// if(tp_line(cwp->tp_current)==cwp->w_fp->lines-1) return(FALSE); 
			if(now==event_sec && unow-event_usec < 50000) return(FALSE);
			event_sec=now;event_usec=unow;
			last_event=GDK_SCROLL+1;

			move_window_lines(wd->wp,lines);
			return TRUE;
		};
#if	GTK3
		case GDK_SCROLL_SMOOTH: {
			int lines=ev->delta_y*100;
			// MESG("gedit_display smooth: dire=%d dy=%f lines=%d",ev->direction,ev->delta_y,lines);

			if(ev->delta_y>0) {
				if(lines<1) return FALSE;
				if(wd->wp != cwp) change_window(wd->wp);
				if(now==event_sec && unow-event_usec < 500000) return(FALSE);
				event_sec=now;event_usec=unow;
				last_event=GDK_SCROLL+1;
				if(wd->cr!=NULL) {
					MESG("	drawing!");
				}; 
				if(lines<1) {
					return (TRUE);
				};
				// MESG("move up %d lines",lines);
				move_window_lines(wd->wp,lines);
				return TRUE;
			} else if(ev->delta_y<0) {
				if(lines>-1) return FALSE;
				if(wd->wp != cwp) change_window(wd->wp);
				if(now==event_sec && unow-event_usec < 500000) return(FALSE);
				event_sec=now;event_usec=unow;
				last_event=GDK_SCROLL;
				// MESG("move down %d lines",lines);
				move_window_lines(wd->wp,lines);
				return TRUE;
			} else {
				// MESG("no move");
			};
			return TRUE;
		};
#endif
	 	default:
		return TRUE;		
	}; 
 };

 if(event->any.window == wd->edit_window) {
 	// MESG("event: edit_window");
	if(event->type == GDK_2BUTTON_PRESS) {
//		GdkEventButton *ev = (GdkEventButton *)event;
//		MESG("Double click! button=%d",ev->button);
		double_click=KMOUSE_DBLCLICK;
	};
	if(event->type == GDK_BUTTON_PRESS) {
		GdkEventButton *ev = (GdkEventButton *)event;
		double_click=0;
//		MESG("button press button=%d",ev->button);
		motion_button = ev->button;
		last_event = GDK_BUTTON_PRESS;
		position_x=ev->x;
		position_y=ev->y;
		mouse_x=position_x/CLEN;
		mouse_y=position_y/CHEIGHTI;
		update_from_mouse(wd->wp,ev->x,ev->y,ev->button,1);

		if(ev->button==1) {
//			MESG("button 1 ->hc");
			prev_x=mouse_x;
			prev_y=mouse_y;

			if(wd->wp != cwp) 
			{
//			hide_cursor("button press 1");
			prev_x=mouse_x;
			prev_y=mouse_y;
//			MESG("button 1 ->sc");
//			show_cursor("button_press 1");
			};
//			MESG("return from press button");
			return TRUE;
		};
#if	0	// disable it because sometimes second button is pressed during smooth scrolling!
		if(ev->button==2) {
			x_insert();
			return TRUE;
		}
#endif
		if(ev->button==3) {
			return FALSE;
		};
		return TRUE;
	};
	if(event->type == GDK_BUTTON_RELEASE) {
		GdkEventButton *ev = (GdkEventButton *)event;
//		MESG("button release: button=%d double=%d",ev->button,double_click);
		if(ev->button==4 || ev->button==5) return TRUE;

		motion_button=0;
		selection_on=0;
		position_x = ev->x;
		position_y = ev->y;
		mouse_x=position_x/CLEN;
		mouse_y=position_y/CHEIGHTI;
//		MESG("button release:2");
		if(last_event==GDK_MOTION_NOTIFY) {
//		   MESG("last was motion!");
		   update_from_mouse(wd->wp,position_x,position_y,ev->button,0);
			show_cursor("button release 2");
			last_event = GDK_BUTTON_RELEASE;
			return TRUE;
		};
//		MESG("button release:3");
		update_from_mouse(wd->wp,ev->x,ev->y,double_click+ev->button,0);
//		MESG("button release:4");
		cwp->w_flag |= UPD_MOVE;	// Check in GDK3 ??
		last_event = GDK_BUTTON_RELEASE;
//		MESG("button release: set");
		return TRUE;
	};
	if(event->type == GDK_MOTION_NOTIFY) {
		GdkEventMotion *ev = (GdkEventMotion *)event;
	    mouse_x = ev->x / CLEN; mouse_y = ev->y / CHEIGHTI;
		position_x = ev->x;
		position_y = ev->y;
		// MESG("ge_edit_display_event:motion");
		if(prev_x==mouse_x && prev_y==mouse_y) {
			return(true);
		};
		if(last_event==GDK_BUTTON_PRESS) {
			// check if in the same window.
			if(motion_button == 1) 
			{ /* set position or mark, start selection */
				update_from_mouse(wd->wp,position_x,position_y,motion_button,1);
				set_xmark(cwp,prev_x-cwp->w_infocol,prev_y,0);
//				MESG("last is press");
				hide_cursor("motion button1");
				selection_on=1;
				set_selection(true);
	     	};
		};
		if(last_event==GDK_MOTION_NOTIFY) {
			if(selection_on && cwp->selection && motion_button==1) {
				// if different position then redraw selection
				if(mouse_y<0 || mouse_y > cwp->w_ntrows-1 ) {
					arrange_window(cwp,mouse_y);
				};
				if(prev_x!=mouse_x || prev_y!=mouse_y) {
			    update_from_mouse(wd->wp,position_x,position_y,motion_button,0);
				prev_x=mouse_x;
				prev_y=mouse_y;
				set_xmark(cwp,mouse_x-cwp->w_infocol,mouse_y,1);
				};
			};
		}
		last_event = GDK_MOTION_NOTIFY;
		return TRUE;
	};
	// MESG("event: end!");
	return FALSE;
 } else {
	return TRUE;
 } 
//  return FALSE;
}

/* get various elements from mouse position */
 /* window , buffer, line, character offset */ 
void update_from_mouse(WINDP *wp,int x,int y,int button, int reset) 
{ 
 static int prow=-1,pcol=-1;
 int i,mouse_row,mouse_col;
 static int update_ok=0;
 static int count=0;
 static int prev_button=0;
 int update_all=0;

 count ++;
 num new_line;
 offs new_offset;
 // MESG("update_from_mouse: x=%d y=%d button=%d reset=%d",x,y,button,reset);
 if(reset) { prow=-1;pcol=-1; 
	// MESG("update_from_mouse: reset");
	hide_cursor("update_from_mouse"); 
	if(cwp !=wp /* && selection_on==0 */) { // we must change current window
//		MESG("update_from_mouse: change_window");
//		change_window(wp);
	};
	return;
 };

 mouse_row=y/CHEIGHTI-(wp->w_fp->b_header!=NULL);
 mouse_col=x/CLEN;
 if(cwp!=wp) change_window(wp);
 if(mouse_row<0) {
	if(!change_sort_mode(mouse_col)) {
		mouse_row=prow;
		mouse_col=pcol;
	};
	update_screen(TRUE);
	return;
 };
 // MESG("update_from_mouse: prow=%d col=%d row=%d",prow,mouse_col,mouse_row);
 if(prow<0) update_all=1;
 if(mouse_row==prow && mouse_col==pcol) { 
	update_ok=false;
 } else {
	prow=mouse_row;
	pcol=mouse_col;
	update_ok=true;
 };

	if(cwp !=wp /* && selection_on==0 */) { // we must change current window
		change_window(wp);
	};
	/* We are always in an editors window */
	if(cbfp->b_flag & FSNLIST) {
		new_line = wp->top_note_line+mouse_row;
		// MESG("mouse at new_line=%d",new_line);
		if(new_line==wp->current_note_line) {
			// MESG("dir: in same line=%d",new_line);
			if(button==1) {
			if(cbfp->b_flag & FSDIRED) {
				if(button==1) dir_right(0);
				// if(button==3) dir_left(0);
			} 
#if	TNOTES
			else {
				if(button==1) {
					view_note(0);
				};
			};
#endif
			update_screen(FALSE);
			return;
			};
		} {
			// MESG("change note line to %d",new_line);
			// update_screen(FALSE);
			// return ;
		};
	} else {
		new_line=tp_line(wp->tp_hline)+mouse_row;
		if(is_wrap_text(wp->w_fp)) new_offset=tp_offset(wp->tp_hline);
		else new_offset=LineBegin(tp_offset(wp->tp_hline));
		// MESG("top line is: %ld",new_offset);
		for(i=0;i<mouse_row;i++) {
			if(is_wrap_text(wp->w_fp)) 
				new_offset = FNext_wrap_line(wp,new_offset,1,1);
	  		else new_offset = FNextLine(cbfp,new_offset);
		};
		if(button==KMOUSE_DBLCLICK+1) {
			select_current_word();
			update_screen(FALSE);
			return;
		};
	};
	// MESG("button=%d",button);
		if(button==1) {	/* move to position */
			// MESG("mouse button 1 prev=%d",prev_button);
			if(prev_button==3) { prev_button=1;set_mark(0);set_update(cwp,UPD_EDIT);update_screen(1);update_full(1);return ;};
			prev_button=1;
			set_update(cwp,UPD_MOVE);
			{
				if(update_ok) {
					if(
#if	TNOTES
						cwp->w_fp->b_flag & FSNOTES || cwp->w_fp->b_flag & FSNOTESN ||
#endif
						cwp->w_fp->b_flag & FSNLIST ) {
						mouse_col=set_tag_view_position(new_line,mouse_col);
						update_screen(TRUE);
					} else {
					if(is_wrap_text(wp->w_fp)) {
						// MESG("	new_offset: %ld line=%ld col=%d",new_offset,new_line, mouse_col);
						textpoint_set(wp->tp_current,new_offset);
						num line = tp_line(wp->tp_current);
						num col0 = tp_col (wp->tp_current);
						move_to_new_position(wp,new_offset,line, col0+mouse_col);
					} else
					move_to_new_position(wp,new_offset,new_line, mouse_col);
					if(update_all) update_screen(TRUE);
					else update_screen(FALSE);
					};
				};
			};
		};
		if(button==2) {
			if(update_ok) {
				update_screen(FALSE);
			};
			prev_button=2;
		};
		if(button==3) {
			// MESG("button:3");
			prev_button=3;
			if(cbfp->b_flag & FSNOTES) {
				drv_show_menu_popup(popup_tag_menu);
				update_screen(TRUE);
			} else 
			if(cbfp->b_flag & FSNOTESN) {
				drv_show_menu_popup(popup_notes_menu);
				update_screen(TRUE);
			} else
			if(!(cbfp->b_flag & FSNLIST) && !cwp->selection) {
				// MESG("button:3 no selection or FSNLIST!");
				if(cbfp->b_flag & FSDIRED) {
					// MESG("FSDIRED!");
					move_to_new_position(wp,new_offset,new_line, mouse_col);
					dir_left(0);
				} else {
					drv_show_menu_popup(popup_select_off);
				};
				update_screen(TRUE);
			} else {
				if(cwp->selection)
					/* check for no region in buffer or box in buffer */
				{
					// MESG("selection menu!");
					if(cwp->selection == REGION_COLM) {
						drv_show_menu_popup(popup_boxed_region);
					} else {
						drv_show_menu_popup(popup_select_on);
					};
					// MESG("show selection popup menu!");
				};
				if(cbfp->b_flag & FSNLIST) {
					// MESG("FSNLIST!");
					// move_to_new_position(wp,new_offset,new_line, mouse_col);
					dir_left(0);
					update_screen(TRUE);
				};
				update_screen(FALSE);
			};
		};
}

 /* new_wp, new_line, new_column have been set */
void move_to_new_position(WINDP *new_wp, offs new_offs, num new_line, int new_column)
{
 if(new_wp!=cwp) {
//	MESG("move_to_new_position: new window!");
 	hide_cursor("move_to_new_position, new window");
	set_current_window(new_wp,"move_to_new_position");
 };
//	MESG("move_to_new_position: col=%ld line=%ld",new_column,new_line);
	set_goal_column(new_column-cwp->w_infocol,"move_to_new_position");
	MoveLineCol(new_line,cwp->goal_column);
}

int arrange_window(WINDP *wp,int mouse_y)
{
 if(mouse_y<0) {
	move_window(1);
	set_xmark(wp,0,0,1);
 };
 if(mouse_y>0) {
	move_window(-1);
	set_xmark(wp,0,wp->w_ntrows,1);
 };
 return(1); 
}


/* -- */
