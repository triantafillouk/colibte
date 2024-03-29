/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

*/
/* window management.  */

#include	"xe.h"
#include	"color.h"

extern alist *window_list;
extern int *color;
extern FILEBUF *cbfp;
extern int color_scheme_ind;
extern int drv_initialized;

GWINDP * drv_new_twinp();
int DiffColumn(FILEBUF *fp, offs *dbo,offs col_offs,char *from);
int DiffColumns(FILEBUF *fp, offs start,offs col_offs,char *from);
offs FNext_wrap_line(WINDP *wp,offs current_offset,int lines,int top);
void set_top_hline(WINDP *wp,offs cof,char *from);

int window_num()
{
	return window_list->size;
}

/*
 * Position current line in mid window, redraw.
 * Bound to ^L.
 */
int reposition(int n)
{
 // MESG("reposition:");
	int sline=window_cursor_line(cwp);
	int midline=cwp->w_ntrows/2;
	int movelines=midline-sline;

	if(sline!=cwp->currow) { ERROR("reposition: sline=%d currow=%d",sline,cwp->currow);};

	if(sline==0) {movelines=cwp->w_ntrows-3;}
	else if(sline==midline) { movelines=-sline;}
	else if(sline==cwp->w_ntrows-1) { movelines = midline-cwp->w_ntrows;}

	move_window(movelines);
	change_color_scheme(color_scheme_ind+1);
    return (TRUE);
}

void set_window_width(WINDP *wp)
{
 // wp->w_width = wp->w_ntcols - wp->w_infocol -1 - (tabsize - (wp->w_ntcols-wp->w_infocol-1)%tabsize);
 wp->w_width = wp->w_ntcols - wp->w_infocol -1 - (wp->w_ntcols-wp->w_infocol-1)%tabsize;

 // MESG("set_window_width: max=%d set to %d",wp->w_ntcols-wp->w_infocol,wp->w_width);
}

/* hard reposition 
 * this is for error conditions only, remove for production 
 * bound to ^XL . use to show current status
 */
int hard_reposition(int n)
{
	n = 0;	/* default to 0 to center screen */
	num line,col;
	int ppline = cwp->w_ppline;
	show_info(1);
	line=tp_line(cwp->tp_current)+1;
	col=tp_col(cwp->tp_current);
	igotolinecol(line,1,ppline+1);	
	cwp->w_ppline=ppline+1;
	set_update(cwp,UPD_WINDOW);
    return (TRUE);
}

void change_window(WINDP *wp)
{
 WINDP *pwp;	// previous window
 pwp = cwp;
 // MESG("change_window:");

 reset_region_textpoints();	/* clear selection of previous window  */
 set_update(cwp,UPD_EDIT);
 hide_cursor("change_window");

 set_current_window(wp,"change_window");
 reset_region_textpoints();	/* clear selection of new window  */
 set_update(wp,UPD_MOVE);
 status_line(pwp);
 status_line(wp);
 if(!is_scratch_buffer(wp->w_fp)) set_working_dir(wp->w_fp->b_dname);
}

/* set current goal column  */
void set_goal_column(int new_column,char *from)
{
 FILEBUF *fp = cwp->w_fp;
#if	WRAPD
 int old_column=cwp->goal_column;
#endif
 if(new_column<0) {
	// if(GetCol()>cwp->goal_column) 
	{
		if(is_wrap_text(fp)) 
			cwp->goal_column=GetCol() % (cwp->w_width);
		else cwp->goal_column=GetCol();
	};
 } else {
	cwp->goal_column=new_column;
 };
 if(fp->view_mode & VMHEX) {
 	cwp->goal_column = new_column+cwp->w_lcol-HSTART-1;
	if(cwp->goal_column<0) cwp->goal_column=0;
	if(cwp->goal_column>HEX_LINE_LEN-1) cwp->goal_column=HEX_LINE_LEN-1;
 } else {
#if	TNOTES
	if(fp->b_flag == FSNOTES) cwp->goal_column = 0;
	else if(fp->b_flag & FSNOTESN) cwp->goal_column = NOTES_COLUMN+2;
#endif
	if(new_column>=0) cwp->goal_column += cwp->w_lcol ;
 };
#if	WRAPD
 MESG("set_goal_column: [%s] b_flag=%X old=%d new=%d ",from,fp->b_flag,old_column,cwp->goal_column);
#endif
}

void set_current_window(WINDP *new_wp,char *title)
{
 if(new_wp) if(cwp!=new_wp){
	WINDP *previous_wp=cwp;
	cwp = new_wp;
	curgwp = cwp->gwp;
	if(/* wp->w_fp->b_dname!=NULL && */ new_wp->w_fp->b_dname[0]!=0)
		if(chdir(new_wp->w_fp->b_dname)!=0) msg_line("cannot set current window dir [%s]",new_wp->w_fp->b_dname);
	cbfp=new_wp->w_fp;
	cbfp->tp_current = cwp->tp_current;
	set_update(new_wp,UPD_STATUS);
	set_update(previous_wp,UPD_STATUS);
 };
}

/*
 * Go to next window.
	Bound to "C-X C-N".
 */
int next_window(int n)
{
	register WINDP *wp;
	if(!drv_initialized) return false;
	lfind_data((void *)cwp,window_list);
	lmove_to_next(window_list,1);
	wp=(WINDP *)lget_current(window_list);

	change_window(wp);
	return (TRUE);
}

/*
 * Go to previous window.
 */
int prev_window(int n)
{
	register WINDP *wp;
	if(!drv_initialized) return false;
	lfind_data((void *)cwp,window_list);
	lmove_to_previous(window_list,1);
	wp=(WINDP *)lget_current(window_list);
	change_window(wp);
	return (TRUE);
}

offs check_next_char(FILEBUF *fp,offs o,int *col) ;

offs FNext_wrap_line(WINDP *wp,offs start,int lines,int top)
{
 offs o=start;
 FILEBUF *fp=wp->w_fp;
 // from the start of the wrap line
 int col=0;
 // MESG_time(";FNext_wrap_line: from %ld lines=%d",start,lines);

 while(lines-- >0)
 {
 offs now_start=o;
 while(col < cwp->w_width) {
 	if(FEofAt(fp,o)) {
		MESG("on last line! %ld",now_start);
		return now_start;
	};
	if(FEolAt(fp,o)) {
		// if(col+col_now<wp->w_width) { o++;continue;};
		o+=fp->EolSize;
		col=0;
		// MESG("		: eol break! %ld new at %ld col=%d",start,o,col);
		break;
	};
	int c;
	utfchar uc;
	o = FUtfCharAt(fp,o,&uc);
 	c=uc.uval[0];
	if (c == CHR_TAB) {
		col=next_tab(col);
	} else {
		col += get_utf_length(&uc);
	};
  };
  col=0;
 };
 // MESG_time("!: > from %ld to %ld col=%d",start,o,col);
 return o;
}

/*
 col_position = current_column % w_width
 - if current column >= w_width
	set current_column to 
		current_column - w_width
 - else
	goto previous line
	if line_len > w_width
		full = (line_len/w_width) * w_width
		rest = line_len - full
		if rest<col_position
			goto end of line
		else goto column full+col_position
	else
		if line_len < col_position  
			goto end of line
		else
			goto column col_position
---
for col_position=0

 - if current column >= w_width
	set current_column to 
		current_column - w_width
 - else
	goto previous line
	if line_len > w_width
		full = (line_len/w_width) * w_width
		goto column full
	else
		goto column 0
*/

offs   FPrev_wrap_line(WINDP *wp,offs ptr,int num_lines)
{
 FILEBUF *fp=wp->w_fp;
 TextPoint *pwl = new_textpoint_at(fp,1,ptr);
#if	WRAPD
 MESG("FPrev_wrap_line: now at o=%ld %ld l=%ld c=%ld",tp_offset(pwl),ptr,tp_line(pwl),tp_col(pwl));
#endif
 int col_position = tp_col(pwl) % wp->w_width;
 // if(FEolAt(fp,ptr) && col_position>0)
 if(col_position < cwp->goal_column) col_position=cwp->goal_column;
 num line=tp_line(pwl);
 num prev_col=tp_col(pwl);
#if	WRAPD
 MESG(";FPrev_wrap_line:num_lines=%d wcol_pos=%d line=%ld o=%ld tp_col=%ld >= %d",num_lines,col_position,line,ptr,tp_col(pwl),num_lines*wp->w_width);
 MESG("12");
#endif
 num o1=ptr;
 if(tp_col(pwl) >= num_lines*wp->w_width) 
 {
#if	WRAPD
	MESG("13 tp_col=%ld n=%d w=%d",tp_col(pwl),num_lines,wp->w_width,num_lines*wp->w_width);
#endif
#if	1
	textpoint_set_lc(pwl,tp_line(pwl),tp_col(pwl)-num_lines*wp->w_width);
	o1=tp_offset(pwl);
#if	WRAPD
	MESG("	1: l=%ld c=%ld o=%ld",tp_line(pwl),tp_col(pwl),o1);
	textpoint_set(pwl,o1);
	MESG("	2: l=%ld c=%ld o=%ld",tp_line(pwl),tp_col(pwl),o1);
#endif
#else
 	num col=tp_col(pwl) - num_lines*wp->w_width;
	/// if(tp_col(pwl)%wp->w_width < cwp->goal_column) col += cwp->goal_column - tp_col(pwl)%wp->w_width;
	// if(col%wp->w_width < wp->goal_column) col += (wp->goal_column - col%wp->w_width);
	strcpy(mesgs,"init");

	textpoint_set_lc(pwl,line,col);
	o1=tp_offset(pwl);
#if	WRAPD
	num diffcol = DiffColumns(fp,FLineBegin(fp,o1),o1,"FPrev_wrap_line:BEG");
	// if(col!=diffcol)
	{
		MESG("!FPrev_wrap_line: backtrack %ld cols o1=%ld diffcol %ld prev col=%ld pos=%ld new col=%ld pos=%ld",
			num_lines*wp->w_width,o1,diffcol,prev_col,prev_col%wp->w_width,col,col%wp->w_width);
		MESG(" from %s pos=%d goal=%d %d %d",mesgs,wp->goal_column,col_position,col%wp->w_width,diffcol%wp->w_width);
	};
#endif
 	// MESG(";FPre_line:num_lines=%d pline=%ld -> line=%ld  col %ld -> %ld o1=%ld >= width %d rows=%d",num_lines,pline,line,prev_col,tp_col(pwl),o1,wp->w_width,wp->w_ntrows);
#endif
 } else {
	// MESG("11");
 	if(line>0) {
		line -= num_lines;
		if(line<0) line=0;
#if	WRAPD
		MESG("set pwl to start of previous line (%ld)!",line);
#endif
		textpoint_set_lc(pwl,line,0);
		// MESG("1");
		num goal_column = wp->goal_column % wp->w_width;
		if(col_position==0) set_goal_column(0,"FPrev_wrap_line");
		num o0 = tp_offset(pwl);
		    o1 = FLineEnd(fp,o0);
#if	WRAPD
		MESG("	goto prev line: %ld o0=%ld end=%ld",line,o0,o1);
#endif
		num linecols = DiffColumns(fp,o0,o1,"Fprev_wrap_line:2:BEG");
#if	WRAPD
		MESG("	goto prev line: %ld o0=%ld end=%ld linecols=%ld col=%ld",line,o0,o1,linecols,tp_col(pwl));
#endif
		num rest = linecols % wp->w_width;
		num full_lines = linecols / wp->w_width;
		if(rest <= goal_column) {
			// goto eol o1
		} else {
			textpoint_set_lc(pwl,line,full_lines*wp->w_width+goal_column);
			o1 = tp_offset(pwl);
		};
		// textpoint_set(pwl,o1);
	} else {
		// MESG("	line 0");
		textpoint_set_lc(pwl,0,prev_col%wp->w_width);
		o1=tp_offset(pwl);
	}
 };
 textpoint_delete(pwl);
 // MESG("		o1=%ld",o1);
 return o1;
}

/* move text position in window by n lines forward/backward */
int move_window(int n)
{
	offs curoffs;
	if(!drv_initialized) return false;
	if(n==0) return FALSE;
	if(is_wrap_text(cwp->w_fp)) 
		curoffs = tp_offset(cwp->tp_hline);
	else
		curoffs = LineBegin(tp_offset(cwp->tp_hline));
	// MESG("move_window: current=%ld n=%d",curoffs,n);
    if (n < 0) {
			if(is_wrap_text(cwp->w_fp)) {
			curoffs=FNext_wrap_line(cwp,curoffs,-n,1);
			} else {	
        while (n++ < 0) {
				curoffs=FNextLine(cbfp,curoffs);
		}
			};
    } else  {
		if(is_wrap_text(cwp->w_fp)) {
			while((n-- > 0) &&  (curoffs>0)) {
				// go to prev window line
				curoffs = FPrev_wrap_line(cwp,curoffs,1);
			};
		} else {
	        while ((n-- >0) && (curoffs>0)) curoffs=FPrevLine(cbfp,curoffs);
		};
    }
	set_update(cwp,UPD_MOVE|UPD_WINDOW);
	tp_copy(cwp->prev_hline,cwp->tp_hline);
	set_top_hline(cwp,curoffs,"move_window");
	// textpoint_set(cwp->tp_hline,curoffs);
	// MESG("move_window: by %d top from (o=%ld l=%ld) to (o=%ld l=%ld) col=%ld",lines_to_move,from_offset,from_line
		// ,curoffs,tp_line(cwp->tp_hline),tp_col(cwp->tp_hline));

    return (TRUE);
}

void free_window_data(WINDP *wp)
{
	if(wp==NULL) return;
	drv_window_delete(wp);
// free window data
	drv_free_win(wp);

	textpoint_delete(wp->tp_current);
	textpoint_delete(wp->w_smark);
	textpoint_delete(wp->w_emark);
	textpoint_delete(wp->prev_hline);
	textpoint_delete(wp->tp_hline);
	textpoint_delete(wp->tp_hsknown);
	textpoint_delete(wp->tp_hmknown);

	// free the real window
	free(wp->gwp);
	free ((void *)wp);
}


/*
 * Remove all other windows from screen
 * Bound to "^X 1". 
 */
int one_window(int n)
{
 WINDP *wp;

 if(!drv_initialized) return false;
 drv_flush();
 if(window_list->head->next == NULL) return(TRUE);	/* this is the only window */

 /* delete all other windows */

 lbegin(window_list);
 while((wp=(WINDP *)lget_current(window_list))!=NULL) {
	if(wp==cwp) { 
		lmove_to_nextnc(window_list);
		continue;
 	};
	remove_current_from_list(window_list);
	unlink_filebuf(wp);
	free_window_data(wp);
 };

 events_flush();	/* needed for gtk2 and cairo!  */
 set_1window();
 set_update(cwp,(UPD_FULL));
 return (TRUE);
}

int buffer_in_view(int n)
{
 FILEBUF *view_buffer;
 WINDP *view_window;
 char bname[MAXFLEN];
 int s1;
 if(!drv_initialized) return false;
 if((s1 = nextarg("buffer name: ",bname,MAXFLEN,true)) !=TRUE) return FALSE;
 view_buffer=get_filebuf(bname,NULL,0);
 if(view_buffer) {
 	view_window=find_buffer_window(view_buffer);
	if(view_window) return true;
 };
 return false;
}

WINDP *find_buffer_window(FILEBUF *fp)
{
 WINDP *wp;
 lbegin(window_list);
 while((wp=(WINDP *)lget(window_list))!=NULL) {
	if(wp->w_fp == fp) return wp;
 };
 return NULL; // buffer not in view
}

// evaluate current window line.
int chardline(WINDP *wp)
{
 int screen_row=0;
 num ptr=tp_offset(wp->tp_hline);
 num size=FSize(wp->w_fp);
 num current_offset = FOffset(wp->w_fp);

 if(size==0) return screen_row;

 do
 {
	if(is_wrap_text(wp->w_fp))
	 	ptr=FNext_wrap_line(wp,ptr,1,1);
	else
	 	ptr=FNextLine(wp->w_fp,ptr);
	if(ptr>current_offset) break;
	
	if(is_wrap_text(wp->w_fp) || FBolAt(wp->w_fp,ptr)) screen_row++;
 } while(ptr<size);

 return(screen_row);
}

int window_cursor_line_wrap(WINDP *wp)
{
  int cline=0;
  num top_line=tp_line(wp->tp_hline);
  num top_offset=tp_offset(wp->tp_hline);
  num current_line=tp_line(wp->tp_current);
  num current_offset=tp_offset(wp->tp_current);

  // MESG(";window_cursor_line_wrap: top (o=%ld l=%ld) current (o=%ld l=%ld)",top_offset,top_line,current_offset,current_line);
  if(top_line==current_line) {
	int col=DiffColumns(wp->w_fp,top_offset,current_offset,"window_cursor_line_wrap:OK:TOP");
	int line_rows=col/cwp->w_width;
	cline=line_rows;
	// MESG(";     wclw: current line on top, cline=%d",cline);
	wp->currow=cline;
	return cline;
  };  

  if(top_line<current_line) {
  	// till the end of hline
	num o=LineEnd(top_offset);
	int col=DiffColumns(wp->w_fp,top_offset,o,"window_cursor_line_wrap:OK:TOP:2");
	int line_rows=col/cwp->w_width;
	cline=line_rows;
	if(o>top_offset && ((col%cwp->w_width)!=0)) cline++;

	if(col%cwp->w_width !=0) 
		o += wp->w_fp->EolSize;
	// MESG("#	wclw: lr=%d cline=%d col=%d w=%d o=%ld bol=%d eol=%d",line_rows,cline,col,cwp->w_width,o,FBolAt(cwp->w_fp,o),FBolAt(cwp->w_fp,o));
	while(1) {
	  num line_start=o;
	  
  	  o=LineEnd(line_start);
	  if(o>current_offset) {
		// MESG("	- o=%ld co=%ld",o,current_offset);
        int col=DiffColumns(wp->w_fp,line_start,current_offset,"window_cursor_line_wrap:OK:3");
	    int line_rows=col/wp->w_width;
	    cline+=line_rows;
		// MESG(";     wclw: end of current line > offset!, cline=%d lr=%d ntrows=%d",cline,line_rows,wp->w_ntrows);
		// MESG("	wclw: %ld > %ld cline=%d ntrows=%d",o,current_offset,cline,wp->w_ntrows);
		wp->currow=cline;
	    return cline;
	  } else if(o==current_offset) {
        int col=DiffColumns(wp->w_fp,line_start,o,"window_cursor_line_wrap:OK:4");
	    int line_rows=col/wp->w_width;
		cline+=line_rows;
		// MESG("	wclw:  %ld = %ld  cline=%d ntrows=%d, lr=%d ",o,current_offset,cline,wp->w_ntrows,line_rows);
		wp->currow=cline;
		return cline;	  
	  } else {
        int col=DiffColumns(wp->w_fp,line_start,o,"window_cursor_line_wrap:OK:5");
	    int line_rows=col/wp->w_width;
	    cline+=line_rows;
		o += wp->w_fp->EolSize;
		cline++;
		// MESG("	wclw: %ld < %ld add %d lines -> cline=%ld",o,current_offset,line_rows+1,cline);
	  };
  	};
  };
  // MESG("; !!!!!!!!!! ---- current offset is before hline !!!!");
  return 0;
}

int window_cursor_line(WINDP *wp)
{
 int cline=0;
#if	TNOTES
	if(cwp->w_fp->b_flag == FSNOTES) cline=cwp->current_tag_line-cwp->top_tag_line;
	else if(cwp->w_fp->b_flag & FSNOTESN) cline=cwp->current_note_line-cwp->top_note_line;
	else 
#endif
	if(cwp->w_fp->b_flag & FSNLIST) cline=cwp->current_note_line-cwp->top_note_line;
	else if(is_wrap_text(cwp->w_fp)) {	// WRAP mode!
		return window_cursor_line_wrap(wp);
	} 
	else cline = tp_line(wp->tp_current)-tp_line(wp->tp_hline);
 // MESG("window_cursor_line: %d",cline);
 return (cline);
}

void next_column(int cols)
{
 TextPoint *tp=cbfp->tp_current;
 // offs from = tp_offse(tp);
 // offs o=from;
 // int col=0;
 textpoint_set_lc(tp,tp_line(tp),tp_col(tp)+cols);
 return;
#if	0
#if	0
 // MESG_time("next_column: start");
 int num_chars=0;
	while (col<cols) {
		if(FEof(cbfp)) return;
		o = check_next_char(cbfp,o,&col);
		num_chars++;
	};
	// MESG_time("next_column: from=%ld to %ld",from,o);
	textpoint_set(cbfp->tp_current,o);

#else
	while (col<cols) {
		if(FEof(cbfp)) return;
		o = check_next_char(cbfp,o,&col);
		MoveRightChar(cbfp);
	};
#endif
 // MESG("next_column: cols=%d from=%ld to %ld line %ld col %ld",cols,from,tp_offset(cbfp->tp_current),tp_line(cbfp->tp_current),tp_col(cbfp->tp_current));
#endif
}

/*
	Set selection
	if f==0 then start point
	if f==1 then end point
	if f==2 then clear selection marks
*/ 
void set_xmark(WINDP *smark_wp, int mouse_col,int mouse_row,int f)
{
	int i;
	offs new_offset;
	int goal=0;

	if(smark_wp==NULL) return;

	if(smark_wp->w_fp->view_mode & VMHEX) mouse_col-=HSTART+1;
		/* find line (mouse_row) lines down from top window line */
		new_offset=LineBegin(tp_offset(smark_wp->tp_hline));
		for(i=0;i<mouse_row;i++) {
	  		new_offset = FNextLine(cbfp,new_offset);
		};
		if(f==0) { /* set start mark */
			goal=valid_offset(new_offset,mouse_col+smark_wp->w_lcol);
			textpoint_set(smark_wp->w_smark,new_offset+goal);
			tp_copy(smark_wp->w_emark,smark_wp->w_smark);
			smark_wp->selection=REGION_CHAR;
		};
		if(f==1) { /* set end mark */
			if(mouse_col==0) goal=0;
			else {
				goal=valid_offset(new_offset,mouse_col+smark_wp->w_lcol);
			};
			textpoint_set(smark_wp->tp_current,new_offset+goal);
			textpoint_set(smark_wp->w_emark,new_offset+goal);
		};
		if(f==2) { /* reset marks */
			goal=valid_offset(new_offset,mouse_col+smark_wp->w_lcol);
			textpoint_set(smark_wp->tp_current,new_offset+goal);
			textpoint_set(smark_wp->w_smark,new_offset+goal);
			textpoint_set(smark_wp->w_emark,new_offset+goal);

			smark_wp->selection=REGION_NONE;
		};

		return ;
}

/* dublicate or create new window */
WINDP *dublicate_window(WINDP *wp0)
{
 WINDP *wp;
 static int id=1;
 int ind;
 if ((wp = (WINDP *) malloc(sizeof(WINDP))) == NULL) {
	msg_line("[OUT OF MEMORY]");
	return (FALSE);
 };
 wp->id=id++;
 wp->gwp = drv_new_twinp();
 wp->vs = NULL;
 wp->w_ntrows=0;
 wp->w_ntcols=0;
 wp->curcol=0;
 wp->currow=0;
 wp->goal_column=0;
 if(wp0!=NULL) {
		wp0->w_fp->b_nwnd++;	/* one more window for this file  */
		wp->w_fp  = wp0->w_fp;
		wp->vtrow=wp0->vtrow;
		wp->vtcol=wp0->vtcol;		
 		wp->top_note_line=wp0->top_note_line;
		wp->current_note_line=wp0->current_note_line;
 		wp->top_tag_line=wp0->top_tag_line;
		wp->current_tag_line=wp0->current_tag_line;
		wp->tp_current = tp_dup(wp0->tp_current,wp->id);
		wp->w_smark = tp_dup(wp0->w_smark,wp->id);
		wp->w_emark = tp_dup(wp0->w_emark,wp->id);

		wp->prev_hline = textpoint_new(wp->w_fp,TP_PREV_HLINE,wp->id);
		tp_copy(wp->prev_hline,wp0->tp_hline);

		wp->tp_hline = tp_dup(wp0->tp_hline,wp->id);
		wp->tp_hsknown = tp_dup(wp0->tp_hsknown,wp->id);
		wp->tp_hmknown = tp_dup(wp0->tp_hmknown,wp->id);

		wp->w_infocol = cwp->w_infocol;

		wp->w_lcol = wp0->w_lcol;
		wp->w_plcol = wp0->w_plcol;
		wp->w_ppline = wp0->w_ppline;
		wp->w_prev_line = wp0->w_prev_line;
		

		for(ind=0;ind<2;ind++) {
			wp->hs[ind].w_hquotem = wp0->hs[ind].w_hquotem;
			wp->hs[ind].w_hselection = 0;
			wp->hs[ind].w_slang = wp0->hs[ind].w_slang;
			wp->hs[ind].w_notes = wp0->hs[ind].w_notes;
			wp->hs[ind].w_hstate = wp0->hs[ind].w_hstate;
			wp->hs[ind].w_first = wp0->hs[ind].w_first;
			wp->hs[ind].w_in_array = wp0->hs[ind].w_in_array;
			wp->hs[ind].known_offset = wp0->hs[ind].known_offset;
		};

		lfind_data(wp0,window_list);
		linsert_after(window_list,wp);
 		set_update(cwp,UPD_FULL);
 		set_update(wp,UPD_FULL);
 } else {
		if(cbfp==NULL) ERROR("current file pointer is NULL!!");
		wp->w_fp = cbfp;
		wp->vtcol=0;
		wp->vtrow=0;
 		wp->top_note_line=0;
		wp->current_note_line=0;
 		wp->top_tag_line=0;
		wp->current_tag_line=0;
		// initialize window textpoints
		wp->w_emark=textpoint_init(TP_W_EMARK,wp->id,wp->w_fp);
		wp->tp_current=textpoint_init(TP_CURRENT,wp->id,wp->w_fp);
		wp->tp_hline=textpoint_init(TP_HLINE,wp->id,wp->w_fp);
		wp->prev_hline=textpoint_init(TP_PREV_HLINE,wp->id,wp->w_fp);
		wp->w_smark=textpoint_init(TP_W_SMARK,wp->id,wp->w_fp);
		wp->tp_hsknown=textpoint_init(TP_HSKNOWN,wp->id,wp->w_fp);
		wp->tp_hmknown=textpoint_init(TP_HMKNOWN,wp->id,wp->w_fp);

		wp->w_lcol=0;
		wp->w_plcol=0;

		if(is_wrap_text(wp->w_fp))
		{
			wp->w_infocol = wp->w_fp->b_infocol;
			wp->w_fp->view_mode = VMWRAP|VMINFO;
		} else 
		if(wp->w_fp->view_mode & VMINFO)
		{
			wp->w_infocol = VMICOLS;
			wp->w_fp->view_mode |= VMINFO;
			set_update(wp,UPD_ALL);
		} else {
				wp->w_infocol = 0;
		};
		wp->w_flag	= UPD_FULL;
		wp->w_ppline = 0;
		wp->w_prev_line=0;
		wp->draw_flag = 0;		
		for(ind=0;ind<2;ind++){
			wp->hs[ind].w_hquotem = 0;
			wp->hs[ind].w_hselection =0;
			wp->hs[ind].w_slang=0;
			wp->hs[ind].w_notes=0;
			wp->hs[ind].known_offset=0;
		};

		add_element_to_list((void *)wp,window_list);
		link_window_buffer(wp,wp->w_fp);
		set_update(cwp,UPD_FULL);
 };

 wp->selection=0;
 wp->w_fcolor = COLOR_FG; 
 wp->w_bcolor = COLOR_BG;

 set_selection(false);
 return wp;
}

void set_windows_color()
{
 WINDP *wp;
 lbegin(window_list);
 while((wp=(WINDP *)lget(window_list))!=NULL)
 {
 wp->w_fcolor = COLOR_FG;
 wp->w_bcolor = COLOR_BG;
 wp->w_flag=UPD_FULL;
 };
}

/* move both windows down  */
int both_down(int n)
{
	next_page(n);
	prev_window(1);
	next_page(n);
	cwp->draw_flag=1;
	upd_column_pos();
	check_cursor_position(cwp);
	set_update(cwp,UPD_EDIT);
	next_window(1);
	set_update(cwp,UPD_EDIT);

	cwp->draw_flag=1;
	return(TRUE);
}

/* move both windows up  */
int both_up(int n)
{
	prev_page(n);
	next_window(1);
	prev_page(n);
	cwp->draw_flag=1;
	upd_column_pos();
	check_cursor_position(cwp);
	set_update(cwp,UPD_EDIT);
	prev_window(1);
	set_update(cwp,UPD_EDIT);

	cwp->draw_flag=1;
	return(TRUE);
}

int getwline()	/* get screen offset of current line in current window */
{
	int screen_row=0;	/* screen line from top of window */
	offs ptr;
	/* search down the line we want */
	ptr=LineBegin(tp_offset(cwp->tp_hline));
	while (ptr <= Offset() && ptr<CSize()) {
		ptr=FNextLine(cbfp,ptr);
		screen_row++;
	}
	if(Offset()==FSize(cbfp)) {
		if(Bol()) screen_row++;
	};
	/* and return the value */
	return(screen_row);
}

int sh_outwindow(int n)
{
 FILEBUF *bp;
 WINDP *current_window = cwp;
 if((bp=new_filebuf("[out]",0)) !=FALSE) {
		if(!activate_file(bp)) return 0;
		if(bp->b_nwnd >0) {
			WINDP *out_window = find_buffer_window(bp);
			// find output window and close it!
			if(out_window && cbfp != bp) {
				cwp=out_window;
				delete_window(1);
				set_current_window(current_window,"sh_outwindow");
				set_update(cwp,UPD_ALL);
				return (OK_CLRSL);
			};			
			msg_line("cannot delete output window while inside it");
			return 0;
		};
		split_window(1);
 		next_window(1);	/* got to the oter window already created  */
		sfb(bp);		/* select out buffer for this window  */
	};
	set_current_window(current_window,"sh_outwindow:go back");	/* go back where we started!  */
	set_update(cwp,UPD_ALL);
	return (OK_CLRSL);
}

/* -- */
