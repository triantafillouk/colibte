/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.
	
	basic editor commands  
*/

#include	"xe.h"

int delete_region();

int is_utf_accent(FILEBUF *fp, offs o);
offs MoveToColumn(int go_col);
int export_region(ClipBoard *);	/* this is platform dependant!  */
void toggle_global_flag(int option_flag);
void set_global_flag(int option_flag,int value);
void get_lowercase_string(char *lower, char *string);
void get_uppercase_string(char *lower, char *string);
void update_lines(FILEBUF *bp);

offs FNext_wrap_line(WINDP *wp,offs current_offset,int num_lines,int top);
offs FPrev_wrap_line(WINDP *wp,offs ptr,int num_lines);
void set_top_hline(WINDP *wp,offs cof,char *from);

void next_column(int cols);

int GetBlock(FILEBUF *fp,char *copy,offs from,offs size);
int delins(int dlength, char *instr);

ClipBoard *MainClipBoard=NULL;
extern FILEBUF *cbfp;
extern alist *window_list;

int lock_move=0;	/* scroll lock movements */
int lock_sync=0;	/* lock page movement  */
int hex_input=0;
int hex_byte=0;

extern char utfokey[10];
extern int utflen;
extern int color_scheme_ind;
extern VAR option_names[];

int get_hex_byte ()
{
	return hex_byte;
}

void toggle_hex_byte()
{
	hex_byte ^= 1;
}

void reset_hex_byte()
{
	hex_byte = 0;
}

int next_tab(num i)
{
 return (i+tabsize-(i)%(tabsize));
}

// a function to do nothing
int do_nothing(num n)
{
	return TRUE;
}

int is_wrap_text(FILEBUF *fp)
{
 if(fp->view_mode & VMWRAP &&
    fp->b_flag < FSNOTES &&
	!(fp->view_mode & VMHEX)
 ) return 1;
 return 0;
}

int main_clipboard_copy()
{
	return  clipboard_copy(MainClipBoard); 
}

void toggle_val(char *name){
//	if(name==NULL) return;
	if((int)bt_dval(name)) {
		set_bt_num_val(name,0);
	} else set_bt_num_val(name,1);
	// MESG("new value of [%s] is %d",name,(int)bt_dval(name));
}

int toggle_parameter(num type)
{
 // MESG("toggle_parameter: type[%d]=[%s] gmode=%d ",type,option_names[type].name,gmode);
  toggle_val(option_names[type].name);
  switch(type) {
  	case EMKEYEMUL:	{
		int emulation;
		emulation = (int)bt_dval(option_names[type].name);
		set_key_emulation(emulation);
		msg_line("set key amulation to %s",(emulation==0) ? "native":"emacs");
		}; break;
	case EMBEDICONS: // toggle_val("embed_icons");
//		redraw toolbar (now needs restart)
		msg_line("restart to change icons");
		break;
	case EMVINFOCOL: 
		if(!(cwp->w_fp->view_mode & VMHEX)) {
		set_update(cwp,UPD_MOVE|UPD_FULL);
//		MESG("toggle_parameter [%d] val=%d",EMVINFOCOL,(int)bt_dval("show_vinfo"));
		if((int)bt_dval("show_vinfo")) {
			cbfp->view_mode |= VMINFO;
			cwp->w_infocol = VMICOLS;
			cbfp->b_infocol = VMICOLS;
		} else {
			cbfp->view_mode = 0;
			cwp->w_infocol = 0;
			cbfp->b_infocol = 0;
		};
		update_buffer_windows(cbfp,UPD_ALL|UPD_MOVE);
		update_screen(1);
		};
		break;
	case EMCDATA: 
		set_update(cwp,UPD_MOVE);
		break;
	case EMCCASE:
		if(gmode_exact_case) gmode_exact_case=0;else gmode_exact_case=1;
		break;
	case EMCOVER:
		if(gmode_over) gmode_over=0;else gmode_over=1;
		break;
	case EMCREGEXP:
		if(gmode_reg_exp) gmode_reg_exp=0;else gmode_reg_exp=1;
		break;
	case EMLARGEICONS: 
//		redraw toolbar (now needs restart)
		msg_line("restart to change icons");
		break;
	case EMUSETITLEBAR:
//		toggle gtk3 ttilebar (now needs restart)
		msg_line("restart to change");
		break;
	case EMWRAP: {
	   int infocols=0;
	   int view_mode=0;
	   int v1=(int)bt_dval("wrap_mode");
	   // MESG("EMWRAP %f",(int)v1);
		if((int)bt_dval("show_vinfo")) {
			infocols=1;
			view_mode=VMINFO;
		};
	   
		if(v1>0) {
			view_mode |=VMWRAP;
			infocols=1;
		};
		lbegin(window_list);
		WINDP *wp;
		while((wp=lget(window_list))!=NULL) {
			if(wp->w_fp->b_flag < FSNOTES) {
			wp->w_infocol = infocols;
			wp->w_fp->view_mode |= view_mode;
			// MESG("set w_fp view_mode to 0x%X",wp->w_fp->view_mode);
			set_update(wp,UPD_ALL);
			};
		};
	   break;
	  };
  };  
  return true;
}

/* Move the cursor to the beginning of the current line. */
int goto_bol(num n)
{
	// MESG("goto_bol: b_flag=%X",cbfp->b_flag);
	if(cbfp->b_flag==FSNOTES || cbfp->b_flag & FSNOTESN || cbfp->b_flag& FSNLIST) {
		return goto_bof(n);
	};
	/* in interactive mode first go to the beginning of the window then at bof */
	if(BolAt(Offset()) && macro_exec==FALSE && kbdmode==STOP) 
	{
		if(Offset() == tp_offset(cwp->tp_hline)) { 
			goto_bof(0);
		} else {
			tp_copy(cbfp->tp_current,cwp->tp_hline);
		};
	} else
	ToLineBegin();
	set_goal_column(0,"goto_bol");
	if(cwp) set_update(cwp,UPD_MOVE);
	return (OK_RSTGOAL);
}

/* Move the cursor back by "n" characters.  */
int prev_character(num n)
{
 if (n < 0) return (next_character(-n));

 while (n--) {
 	if(Offset()<=0) return FALSE;
	MoveLeftChar(cbfp);
	set_update(cwp,UPD_MOVE);
 };
 // MESG_time("prev_char:end");
 set_goal_column(-1,"prev_character");
 return (OK_RSTGOAL);
}

/* Move the cursor to the end of the current line. */
int goto_eol(num n)
{
	// MESG("goto_eol: b_flag=%X",cbfp->b_flag);
	if(cbfp->b_flag==FSNOTES || cbfp->b_flag & FSNOTESN || cbfp->b_flag& FSNLIST) {
		return goto_eof(n);
	};
	if(Eol() && macro_exec==FALSE && kbdmode==STOP) {
		if(getcline() == (tp_line(cwp->tp_hline)+cwp->w_ntrows-2-half_last_line)) 
		{
			goto_eof(0);
		}else {
		// Goto end of screen
			textpoint_set_lc(cbfp->tp_current,tp_line(cwp->tp_hline)+cwp->w_ntrows-2-half_last_line,0);
		};
	} else {
		ToLineEnd();
	};
	set_goal_column(-1,"goto_eol");
	set_update(cwp,UPD_MOVE);
	return (OK_RSTGOAL);
}

/*
 * Move the cursor forwards by "n" characters. 
 */
int next_character(num n)
{
	if (n < 0) return (prev_character(-n));
	while (n--) {
		if(FEof(cbfp)) return FALSE;
		MoveRightChar(cbfp);
	};
	set_goal_column(-1,"next_character");
	set_update(cwp,UPD_MOVE);
    return (OK_RSTGOAL);
}

int select_next_character(num n)
{
	if(cwp->selection==0) setmark(REGION_CHAR);
	set_update(cwp,UPD_MOVE);
	return next_character(n);
}

int select_prev_character(num n)
{
	if(cwp->selection==0) setmark(REGION_CHAR);
	set_update(cwp,UPD_MOVE);
	return prev_character(n);
}

int select_next_line(num n)
{
	if(cwp->selection==0) setmark(REGION_CHAR);
	set_update(cwp,UPD_MOVE);
	return next_line(n);
}

int select_prev_line(num n)
{
	if(cwp->selection==0) setmark(REGION_CHAR);
	set_update(cwp,UPD_MOVE);
	return prev_line(n);
}

// Set new top line, keep current row
int imove_top_line(num new_top_line)
{
 int current_row=tp_line(cwp->tp_current)-tp_line(cwp->tp_hline);
 num new_current_line;
 num last_line= tp_line(cwp->w_fp->tp_text_end);
 if(new_top_line<0) new_top_line=0;
 if(new_top_line>last_line) new_top_line=last_line;
 new_current_line=new_top_line+current_row;

 if(new_current_line>last_line) new_current_line=last_line;
 MESG("imove_top_line:to %ld",new_top_line);
 textpoint_set_lc(cwp->tp_hline, new_top_line,0);
 textpoint_set_lc(cwp->tp_current, new_current_line,0);
 set_update(cwp,UPD_MOVE|UPD_WINDOW);
 set_goal_column(0,"imove_top_line");
 
 return 1;
}

/*  internal gotolinecol command */
int igotolinecol(num line,num column,int line_flag)
{
	if(line<1) return FALSE;

	init_highlight();
	cwp->w_ppline=line_flag;	// set physical position at mid window
	MoveLineCol(line-1,column-1);
	set_goal_column(column,"iogotolinecol:");
	set_update(cwp,UPD_WINDOW);
	undo_set_noglue();

	return TRUE;
}

int igotooffset(num n,int ppline)
{
	if(n<1) return FALSE;
//	MESG("igotooffset:");

	init_highlight();
	set_Offset(n);
	set_goal_column(0,"iogotooffset");
	
	cwp->w_ppline=ppline;	/* set physical position at mid window */
	set_update(cwp,UPD_WINDOW|UPD_MOVE);
	undo_set_noglue();

	return TRUE;
}

/* move to a specific line. */
/* Assigned to ALT-G */
int goto_line(num n)		
{
	char arg[MAXSLEN];
	arg[0]=0;
	if ((nextarg("Go to line : ", arg, MAXSLEN,true)) != TRUE) 	return(FALSE);
	if(macro_exec!=MACRO_MODE2) n = get_val();
	if(cbfp->b_flag & FSNLIST) {
		cwp->current_note_line=n;
		if(n>cbfp->b_notes) cwp->current_note_line=cbfp->b_notes-1;
		set_update(cwp,UPD_MOVE);
		return(OK_CLRSL);
	}
	return(igotolinecol(n,1,0));
}

int goto_offset(num n)		
{
	char arg[MAXSLEN];
	arg[0]=0;
	if ((nextarg("Go to offset : ", arg, MAXSLEN,true)) != TRUE) 	return(FALSE);
	if(macro_exec!=MACRO_MODE2) n = get_val();
	if(cbfp->b_flag & FSNLIST) {
		return(OK_CLRSL);
	};
	textpoint_set(cbfp->tp_current,n);
	return OK_CLRSL;
}

/* Go to the beginning of the file */
/* Assigned to ALT-< */
int goto_bof(num n)
{
 // MESG("goto_bof:");
#if	TNOTES
	if(cbfp->b_flag==FSNOTES) {
		if(cwp->current_tag_line==0) return FALSE;
		cwp->current_tag_line=0;
		// MoveLineCol(0,0);
		set_update(cwp,UPD_WINDOW);
		return (OK_RSTGOAL);
	} else 
#endif
	if(cbfp->b_flag & FSNOTESN || cbfp->b_flag & FSNLIST) {
		if(cwp->current_note_line==0) return FALSE;
		cwp->current_note_line=0;
		// MoveLineCol(0,cwp->goal_column);
		set_update(cwp,UPD_WINDOW);
		msg_line(time2a());
		return (OK_RSTGOAL);
	};
	set_Offset(0);
	undo_set_noglue();
	if(execmd) return (OK_CLRSL);
	set_update(cwp,UPD_WINDOW);
	msg_line(time2a());
	// MESG("goto_bof");
	return (OK_RSTGOAL);
}

/* return true if edit is not allowed */
int dont_edit()
{
	//* do not insert characters in view mode */
	if(cbfp->b_state & FS_VIEW) return(TRUE);
	return FALSE;
}

/* Move to the end of the file. */
/*  Assigned to "ALT->".*/
int goto_eof(num n)
{
	int headline=(cbfp->b_header!=NULL);
	int toline=0;
// MESG("goto_eof: b_flag=%X",cbfp->b_flag);
#if	TNOTES

	if(cbfp->b_flag==FSNOTES) {
		set_goal_column(0,"goto_eof");
		toline=cbfp->b_tags-headline;
		cwp->current_tag_line = toline;
		MoveLineCol(toline,cwp->goal_column);
		set_update(cwp,UPD_WINDOW);
		return (OK_CLRSL);
	} else
	if(cbfp->b_flag & FSNOTESN) {
		set_goal_column(NOTES_COLUMN+2,"goto_eof:2");
		toline=cbfp->b_notes-headline;
		cwp->current_note_line = toline;
		MoveLineCol(toline,cwp->goal_column);
		set_update(cwp,UPD_WINDOW);
		return (OK_CLRSL);
	} else 
	// MESG("next_page: toline=%d col=%d",toline,cwp->goal_column);
#endif
	if(cbfp->b_flag& FSNLIST) {
		set_goal_column(0,"goto_eof:3");
		toline=cbfp->b_notes-headline;
		cwp->current_note_line = toline;
		MoveLineCol(toline,cwp->goal_column);
		set_update(cwp,UPD_WINDOW);
		msg_line(time2a());
		return (OK_CLRSL);
	};
	set_Offset(FSize(cbfp));
	if(execmd) return (OK_CLRSL);
	undo_set_noglue();
	set_hmark(0,"goto_eof");
	set_update(cwp,UPD_WINDOW);
	if(cwp) cwp->w_ppline=0;
	msg_line(time2a());
	return (OK_RSTGOAL);
}

int scroll_up(num n)
{
 int lines=n;
 lock_move=1;
 lines=next_line(lines);
 lock_move=0;
 return lines;
}

int scroll_down(num n)
{
 int status;
 // if(tp_offset(cwp->tp_hline)<1) return FALSE;
 lock_move=1;
 status=prev_line(n);
 lock_move=0;

 return status;
}

int get_current_line()
{
 int current_line;
 FILEBUF *fp=cwp->w_fp;
#if	TNOTES
	if(fp->b_flag==FSNOTES) current_line=cwp->current_tag_line;
	else 
#endif
	if(fp->b_flag & FSNOTESN || fp->b_flag & FSNLIST) current_line=cwp->current_note_line;
	else current_line=GetLine();
	return current_line;
}

int next_wrap_line(num n)
{
	num current_line;
	int headline=(cbfp->b_header!=NULL);

	// get the current line
	current_line=get_current_line();
	cwp->w_prev_line=current_line;

#if	WRAPD
	MESG_time("next_wrap_line: current line=%ld %ld c=%ld o=%ld",
		tp_line(cwp->tp_current),current_line,tp_col(cwp->tp_current),tp_offset(cwp->tp_current));
#endif
    if(lock_move) {
		if(current_line==tp_line(cwp->tp_hline))
		{
            lock_move=0;
            int status=next_wrap_line(n);
            lock_move=1;
			if(status==FALSE) return status;
        };
        return(move_window(-n));
    };

	{	
		if(cbfp->lines  < GetLine()+n+headline) n=cbfp->lines-GetLine()-headline;
#if	WRAPD
		MESG_time("next_wrap_line: l=%d lines=%ld n=%ld",GetLine(),cbfp->lines,n);
#endif
	};

	{
		offs o_now=tp_offset(cwp->tp_current);
#if	1
		offs o=o_now;
		while(!EolAt(o) && o< o_now+(n+2)*cwp->w_width) {
			if(EofAt(o)) {
#if	WRAPD
				MESG("next_wrap_line: at end return false!");
#endif
				return(FALSE);
			}
			o++;
		};
		offs o_end=o;
#else
		offs o_end=LineEnd(o_now);
#endif
		int remains=DiffColumns(cwp->w_fp,o_now,o_end,"next_wrap_line:OK:known");
		if(remains<cwp->w_width && current_line >= tp_line(cwp->w_fp->tp_text_end)) return FALSE;
#if	WRAPD
		MESG_time("next_wrap_line:1 wrap mode! o=%ld end=%ld remain columns=%d",o_now,o_end,remains);
#endif
		if(remains>= n*cwp->w_width) {
#if	WRAPD
			MESG("next_wrap_line:2 remains %d > w=%d",remains,cwp->w_width);
#endif
			next_column(n*cwp->w_width);
		} else {
			int now_col=tp_col(cwp->tp_current) % cwp->w_width;
			if(now_col+remains >= cwp->w_width) {
#if	WRAPD
				MESG("next_wrap_line:3 now_col %d + remains %d >= w=%d",now_col,remains,cwp->w_width);
#endif
				next_column(remains);
			} else {
#if	WRAPD
				MESG("next_wrap_line:4 now_col %d + remains %d < w=%d",now_col,remains,cwp->w_width);
#endif
				MoveLineCol(current_line+n,cwp->goal_column);
			}
		};

		if(cwp->currow == cwp->w_ntrows-2-half_last_line) 
		{
			move_window(-1);
		};
	};
#if	WRAPD
	MESG_time("next_wrap_line:end new pos l=%ld c=%ld",tp_line(cwp->tp_current),tp_col(cwp->tp_current));
#endif
	set_hmark(0,"next_line");
	// MESG("next_wrap_line:	end >!");
	/* reseting the current position */
	set_update(cwp,UPD_MOVE);
	return(TRUE);
}

/* Move forward by n lines. */
/*  emacs key is "^N". */
int next_line(num n)
{
	int w_row=0;
	num current_line;
	int status=0;
	int b_flag = cbfp->b_flag;
	int headline=(cbfp->b_header!=NULL);

	// MESG("next_line: ------------- n=%d lock=%d",n,lock_move);
    if (n < 0)  return (prev_line(-n));
	if(n==0) return(FALSE);

	if(is_wrap_text(cbfp) & !execmd) return next_wrap_line(n);

	// get the current line
	current_line=get_current_line();
	// MESG("# next_line: n=%d current is %ld b_flag=%d",n,current_line,cbfp->b_flag);
	// MESG("next_line: < current=%ld col=%ld",current_line,GetCol());
	cwp->w_prev_line=current_line;
	// MESG("next_line: current_line=%d b_flag=0x%X",current_line,b_flag);
#if	TNOTES
	if(b_flag==FSNOTES) {
		if(cbfp->b_tags  < current_line+n+headline) n=cbfp->b_tags-current_line-headline;
		if(n==0) return FALSE;
		cwp->current_tag_line+=n;
		// MESG("next_line: FSNOTES, new tag_line=%d flag=%d",cwp->current_tag_line,cbfp->b_flag);
		set_update(cwp,UPD_MOVE);
		return(TRUE);
	} else
#endif
	if(b_flag & FSNOTESN || b_flag & FSNLIST) {
		// MESG("next_line: FSNOTES, l=%d notes=%d",current_line,cbfp->b_notes);
		if(cbfp->b_notes  < current_line+n+headline) n=cbfp->b_notes-current_line-headline;
		if(n==0) return FALSE;
		cwp->current_note_line+=n;		
		if(cwp->current_note_line >= cbfp->b_notes) cwp->current_note_line=cbfp->b_notes-1;
		// MESG("next_line: FSNOTESN, new note_line=%d flag=%d",cwp->current_note_line,cbfp->b_flag);
		msg_line(time2a());
		set_update(cwp,UPD_MOVE);
		return(TRUE);
	};

    if(lock_move) {
		if(current_line==tp_line(cwp->tp_hline))
		{
            lock_move=0;
            status=next_line(n);
            lock_move=1;
			if(status==FALSE) return status;
        };
		if(tp_line(cbfp->tp_text_end)-4 < tp_line(cwp->tp_hline)) return FALSE;
        return(move_window(-n));
    };

 	if(b_flag & FSNLIST)
	{
		// MESG("next_line: FSNLIST, l=%d notes=%d n+l=%d",GetLine(),cbfp->b_notes,GetLine()+n);
		if(cbfp->lines  < GetLine()+n+1+headline) n=0;
	} else 
	{	
		if(cbfp->lines  < GetLine()+n+headline) n=cbfp->lines-GetLine()-headline;
		// MESG("next_line: l=%d lines=%ld n=%ld",GetLine(),cbfp->lines,n);
	};
	if(n<1) return (false);

	if(!execmd){
	// w_row = current_line-tp_line(cwp->tp_hline);
	w_row = cwp->currow;
	// if at the end move up
	if(w_row == cwp->w_ntrows-2-half_last_line) {
		// if(!is_wrap_text(cwp->w_fp))
			move_window(-1);
	};
	};
	if(current_line==tp_line(cbfp->tp_text_end)) return FALSE;
	/* and move the point down */
	if(cbfp->view_mode & VMHEX){
		textpoint_set_lc(cbfp->tp_current,current_line+n,Offset()%16);
	} else 
	{
		MoveLineCol(current_line+n,cwp->goal_column);
	};
	set_hmark(0,"next_line");
	// MESG("next_line:	end >!");
	/* reseting the current position */
	if(!execmd) set_update(cwp,UPD_MOVE);
	return(TRUE);
}

void update_top_position_wrap();
offs   FPrevUtfCharAt(FILEBUF *fp,offs o, utfchar *uc);

int prev_wrap_line(num n)
{
 num current_line=get_current_line();

	cwp->w_prev_line=current_line;
#if	WRAPD
	MESG("!prev_wrap_line: current=%d row=%d lock=%d",current_line,cwp->currow,lock_move);
#endif
	// if(current_line-n < 0) n=current_line;

    if(lock_move) {
        // If at the last line of the window
		// MESG("prev_line: row=%d line=%ld col=%ld",cwp->currow,tp_line(cwp->tp_current),tp_col(cwp->tp_current));
		if(cwp->currow >= cwp->w_ntrows-3-half_last_line
			|| tp_offset(cwp->tp_current)>cwp->w_width
		) {
			lock_move=0;
			int status=prev_wrap_line(n);
			lock_move=1;
			if(status==FALSE) return status;
			// return TRUE;
		} else {
			lock_move=0;
			int status=prev_wrap_line(n-1);
			lock_move=1;
			if(status==FALSE) return status;
		};
		move_window(n);
        return(n);
    };

	offs o_now=tp_offset(cwp->tp_current);
	// MESG("prev_line:1");
	offs new_offset = FPrev_wrap_line(cwp,o_now,n);
	textpoint_set(cwp->tp_current,new_offset);
#if	WRAPD
	MESG("prev_wrap_line: o_now=%ld to (o=%ld col=%ld %d)",
		o_now,new_offset,tp_col(cwp->tp_current),tp_col(cwp->tp_current)%cwp->w_width);
#endif
	set_hmark(0,"prev_line");
	set_update(cwp,UPD_MOVE);
	undo_set_noglue();
	return (TRUE);
}

/* Move backward by n lines. */
int prev_line(num n)
{
 num current_line;
 MESG("prev_line: ------------- n=%d lock=%d",n,lock_move);
 int status=0;
	if (n==0) return FALSE;
   	if (n < 0) return (next_line(-n));
	if(is_wrap_text(cbfp) & !execmd) return prev_wrap_line(n);

	current_line=get_current_line();
	cwp->w_prev_line=current_line;
	// MESG("!prev_line: current=%d row=%d lock=%d",current_line,cwp->currow,lock_move);

	if(current_line<1) return(0);
	if(current_line-n < 0) n=current_line;
#if	TNOTES
	if(cwp->w_fp->b_flag==FSNOTES) {
		cwp->current_tag_line-=n;
		if(cwp->current_tag_line<0) cwp->current_tag_line=0;
		// MESG("prev_line: FSNOTES, tag_line=%d flag=%d",cwp->current_tag_line,cbfp->b_flag);
		set_update(cwp,UPD_MOVE);
		// return(true);
	} else
#endif
	if(cwp->w_fp->b_flag & FSNOTESN || cbfp->b_flag & FSNLIST) {
		cwp->current_note_line -= n;
		if(cwp->current_note_line<0) cwp->current_note_line=0;
		// MESG("prev_line: FSNOTESN, note_line=%d flag=%d",cwp->current_note_line,cbfp->b_flag);
		set_update(cwp,UPD_MOVE);
		// return(true);
		msg_line(time2a());
	};


    if(lock_move) {
		MESG("lock_move:");
#if	0
        // If at the last line of the window
		if( current_line - tp_line(cwp->tp_hline) == cwp->w_ntrows-2-half_last_line
		 || tp_line(cwp->tp_current)>0
		)
		{
            lock_move=0;
            status=prev_line(n);
            lock_move=1;
			if(status==FALSE) return status;
        };
#else
		if( current_line - tp_line(cwp->tp_hline) == cwp->w_ntrows-2-half_last_line
		 // || tp_line(cwp->tp_current)>0
		)
		{
            lock_move=0;
			MESG("prev_line lock end 1");
            status=prev_line(n);
            lock_move=1;
			if(status==FALSE) return status;
        } else if(tp_line(cwp->tp_current)>0 && tp_line(cwp->tp_hline)==0) {
            lock_move=0;
			MESG("prev_line lock start 2");
            status=prev_line(n);
            lock_move=1;
			return status;		
		} else {
            lock_move=0;
			MESG("prev_line lock mid 3");
            status=prev_line(n-1);
            lock_move=1;
		};
#endif
		move_window(n);
        return(n);
    };

	if(current_line-n < tp_line(cwp->tp_hline))
	{ // move up window
		// move_window(n+tp_line(cwp->tp_hline)-current_line);
		move_window(n+current_line-tp_line(cwp->tp_hline));
	};
	if(cbfp->view_mode & VMHEX){
		textpoint_set_lc(cbfp->tp_current,current_line-n,Offset()%16);
	} else {
		MoveLineCol(current_line-n,cwp->goal_column);
	};
	set_hmark(0,"prev_line");
	set_update(cwp,UPD_MOVE);
	undo_set_noglue();
	return (TRUE);
}

/* find if a line is empty */
int emptyline(TextPoint *tp)
{
 offs o;
 FILEBUF *fp=tp->fp;
 o=tp_offset(tp);
 if(FBolAt(fp,o) && FEolAt(fp,o)) return true;
 return false;
}


// comment/uncomment at start of line
int comment_line(num n)
{
	FILEBUF *fp = cbfp;
	int comment_type=C_COLSTART;
	// MESG("comment_line: n=%d type=%d buffer_type=%d = %s",n,comment_type,fp->b_type,fp->hl->description);
	if(cwp->selection) {
		offs start,end,o,end0;
		int size=0;
		start = tp_offset(cwp->w_smark);
		end = tp_offset(cwp->w_emark);
		end0=end;
		if(end<start) {
			o=end;end=start;start=o;
		};
		start=LineBegin(start);
		end=LineBegin(end);
		int startcol=StartColumn(start);
		
		for(o=start;o<=end;o=LineEnd(o)+fp->EolSize){
			if(StartColumn(o)!=startcol) { comment_type=C_LINEBEG;break;};
		};
		// MESG("comment_line: type=%d",comment_type);
		for(o=start;o<=end;o=LineEnd(o)+fp->EolSize) {
			set_Offset(o);
			size = fp->hl->comment(comment_type);
			end += size;
		};
		setmark(0);
		if(end0!=end) return true;
		else return false;
		return true;
	} else {
		offs coffs=Offset();
		if(coffs==LineBegin(coffs)) comment_type=C_LINEBEG;
		// MESG("comment_line: single type=%d",comment_type);
		set_update(cwp,UPD_EDIT);
		if(fp->hl->comment(comment_type)!=0) return true;
		else return false;
	};
}

// comment at end of line or at start/end of selection
int comment_line2(num n)
{
	FILEBUF *fp = cbfp;
	// MESG("comment_line2:");
	set_update(cwp,UPD_EDIT);
	if(fp->hl->comment(C_STARTEND)!=0) return true;
	else return false;
}

/* go to the beginning  of paragraph. 
   If at the beginning  then goto the
   beginning of previous paragraph 
*/
int goto_bop(num n)
{
 register TextPoint *tp;
 num toline;
// MESG("goto_bop:");
 tp = tp_dup(cbfp->tp_current,cwp->id);

 while((toline=tp_line(tp)) != 0) {
	textpoint_set_lc(tp,tp_line(tp)-1,0);
	if(!(emptyline(tp)||tp_char(tp)==12)) break;
 };

 for(toline=tp_line(tp);toline>0;toline--) {
 	textpoint_set_lc(tp,toline-1,0);
	if(emptyline(tp) || tp_char(tp)==12) break;
 };

 set_goal_column(0,"goto_bop");
 MoveLineCol(toline,0);

 set_update(cwp,UPD_MOVE);
 textpoint_delete(tp);
 undo_set_noglue();
 return (OK_RSTGOAL);
}

/* goto end of paragraph */
int goto_eop(num n)
{
 TextPoint *tp;
 num toline;

 tp = tp_dup(cbfp->tp_current,cwp->id);
 toline = tp_line(tp);

 while(toline < cbfp->lines) {
 	if(!(emptyline(tp)||tp_char(tp)==12)) break;
	textpoint_set_lc(tp,toline+1,0);
	toline++;
 };

 while(toline < cbfp->lines) {
	textpoint_set_lc(tp,toline,0);
	if(emptyline(tp) || tp_char(tp)==12	) break;
	toline++; 
 };
 MoveLineCol(toline,0);

 set_update(cwp,UPD_MOVE);
 textpoint_delete(tp);
 undo_set_noglue();

 return (OK_RSTGOAL);
}

/* Get the best choise of an offset for a specific column. */
int valid_offset(offs o_in_line,int column_goal)
{
 int    col;
 offs	lstart, coffset;
 utfchar uc;

 col = 0;
 if(column_goal==0) return 0;

 coffset=LineBegin(o_in_line);
 lstart=coffset;

 if(cbfp->view_mode & VMHEX) {
 	if(coffset+column_goal>FSize(cbfp)) {
		return(FSize(cbfp)-coffset);
	} else return column_goal;
 };
 if(cbfp->b_lang==0 && !utf8_error()){
 while(!EolAt(coffset))
 {
	coffset = FUtfCharAt(cbfp,coffset,&uc);

	if (uc.uval[0] == CHR_TAB) {
		col=next_tab(col);
	} else {
		col += get_utf_length(&uc);
	};
	if (col >= column_goal) {
//		MESG("valid_offset: col=%d s=%ld o=%ld lo=%ld",col,lstart,coffset,coffset-lstart);
		break;
	};
 };
 } else {
 	while(!EolAt(coffset))
 	{
		int c=CharAt(coffset);
	
		if (c == CHR_TAB) {
			col=next_tab(col);
		} else {
			col ++;
		};
		coffset++;
		if (col >= column_goal) break;
 	};
 };

 return (coffset-lstart);
}


/* Scroll forward by a specified number of pages */
int next_page(num n)
{
 int headline=(cbfp->b_header!=NULL);
 num toline;
 num topline;
 if(!discmd) {
	// no page defined yet without window!
	// we must define a default page lenght
 	return false; 	
 };
	if (n<0) return (prev_page(-n));
	if (n==0) return FALSE;
	n *= cwp->w_ntrows-1;
	// MESG("next_page: n=%d w=%d",n,cwp->w_width);
#if	TNOTES
	if(cbfp->b_flag==FSNOTES) {
		set_goal_column(0,"next_page:1");
		if(cbfp->b_tags < cwp->current_tag_line+n+headline) n=cbfp->b_tags-cwp->current_tag_line-headline;
		if(n==0) return FALSE;
		// MESG("next_page: FSNOTES from %d + %d = %d max=%d",cwp->current_tag_line,n,cwp->current_tag_line+n,cbfp->b_tags);
		cwp->current_tag_line+=n;
		set_update(cwp,UPD_MOVE|UPD_WINDOW);
		return (OK_CLRSL);
	} else
#endif
	if(cbfp->b_flag & FSNOTESN || cbfp->b_flag & FSNLIST) {
		set_goal_column(NOTES_COLUMN+2,"next_page:2");
		if(cbfp->b_notes < cwp->current_note_line+n+headline) n=cbfp->b_notes-cwp->current_note_line-headline;
		if(n==0) return FALSE;
		cwp->current_note_line+=n;
		set_update(cwp,UPD_MOVE|UPD_WINDOW);
		return (OK_CLRSL);
	};

#if	1
	if(is_wrap_text(cbfp)) {
		if(FSize(cbfp) - FOffset(cbfp)<cwp->w_width) return FALSE; 
	} else {
		if(tp_line(cbfp->tp_current)+2>cbfp->lines) return FALSE;
		if (FEof(cbfp)) return FALSE;
	};
#else
	if (FEof(cbfp)) return FALSE;
#endif

	toline=tp_line(cwp->tp_current);
	if(is_wrap_text(cbfp)) {
		// set new topline
		offs o=tp_offset(cwp->tp_hline);
		// offs co=tp_offset(cwp->tp_current);
		int num_lines=n;
		// MESG("next_page: topo=%ld lines=%d --------",o,num_lines);
		o=FNext_wrap_line(cwp,o,num_lines,1);
		// MESG("         : new topo=%ld",o);
		set_top_hline(cwp,o,"next_page");
		// MESG("		start current from %ld n=%d",o,n);
		o=tp_offset(cwp->tp_current);

		o=FNext_wrap_line(cwp,o,num_lines,0);
		// MESG("         : new current=%ld",o);
		textpoint_set(cwp->tp_current,o);
		textpoint_set(cwp->w_fp->tp_current,o);
		// MESG("		new current o=%ld line=%ld",tp_line(cwp->tp_current),tp_line(cwp->tp_current));
		
		set_update(cwp,UPD_MOVE|UPD_WINDOW);
		undo_set_noglue();

    	return (OK_CLRSL);
	} else {
	cwp->w_ppline = tp_line(cwp->tp_current)-tp_line(cwp->tp_hline)+1;
	topline=tp_line(cwp->tp_hline);
	if(toline+n > cbfp->lines) {
		toline=cbfp->lines;
		topline=toline-cwp->w_ntrows/2;
		if(topline<0) topline=0;
	} else {
		toline += n;
		topline +=n;
	};
	};
	
	// MESG("next_page: toline=%d col=%d",toline,cwp->goal_column);
	MoveLineCol(toline,cwp->goal_column);

	set_hmark(0,"next_page");
	set_update(cwp,UPD_MOVE|UPD_WINDOW);
	undo_set_noglue();

    return (OK_CLRSL);
}


/* Scroll backward by a specified number of pages. */
int prev_page(num num_pages)
{
 num toline;
 int headline=(cbfp->b_header!=NULL);
 int num_lines = num_pages*(cwp->w_ntrows-1)+1;
 if(is_wrap_text(cbfp)) num_lines = num_pages*(cwp->w_ntrows-1);
 if(!discmd) {
	// no page defined yet without window!
	// we must define a default page lenght
 	return false; 	
 };
 // MESG("# prev_page:");
 if (num_pages<0) return (next_page(-num_pages));

#if	TNOTES
	if(cbfp->b_flag==FSNOTES) {
		set_goal_column(0,"prev_page:1");
		if(num_lines > cwp->current_tag_line+headline) num_lines=cwp->current_tag_line;
		if(num_lines==0) return FALSE;

		// MESG("prev_page: FSNOTES from %d - %d = %d",cwp->current_tag_line,n,cwp->current_tag_line-n);
		cwp->current_tag_line-=num_lines;
		set_update(cwp,UPD_MOVE|UPD_WINDOW);
		return (OK_CLRSL);
	} else
#endif
	// MESG("prev_page: b_flag=%X num_lines=%d",cbfp->b_flag,num_lines);
	if(cbfp->b_flag & FSNOTESN || cbfp->b_flag & FSNLIST) {
		set_goal_column(NOTES_COLUMN+2,"prev_page:1");
		if(num_lines > cwp->current_note_line+headline) num_lines=cwp->current_note_line;

		if(num_lines<1) return FALSE;

		cwp->current_note_line-=num_lines;
		if(cwp->current_note_line<0) cwp->current_note_line=0;
		// MESG("	: current_note_line=%d",cwp->current_note_line);
		set_update(cwp,UPD_MOVE|UPD_WINDOW);
		return (OK_CLRSL);
	};

 if(FBof(cbfp)) return (false);
 toline=tp_line(cwp->tp_current);
 if(is_wrap_text(cbfp)) {
 	offs o=tp_offset(cwp->tp_hline);
	// int num_lines=n;
	int nline=num_lines;
	// MESG("prev_page: toline=%d num_lines=%d w=%d rows=%d",toline,num_lines,cwp->w_width,cwp->w_ntrows);
		if(toline==0) 
		o=FPrev_wrap_line(cwp,o,num_lines);
		else
		while(--nline >0) {
			o=FPrev_wrap_line(cwp,o,1);
		};
		set_top_hline(cwp,o,"prev_page");
		// MESG("		start current from %ld n=%d",o,n);
		o=tp_offset(cwp->tp_current);
		nline=num_lines;

		if(toline==0) 
		o=FPrev_wrap_line(cwp,o,num_lines);
		else
		while(nline-- >0) {
			o=FPrev_wrap_line(cwp,o,1);
			// MESG("		next line o=%ld",o);
		};

		textpoint_set(cwp->tp_current,o);
		textpoint_set(cwp->w_fp->tp_current,o);
		// MESG("		new current o=%ld line=%ld",tp_line(cwp->tp_current),tp_line(cwp->tp_current));
		
		set_update(cwp,UPD_MOVE|UPD_WINDOW);
		undo_set_noglue();

    	return (OK_CLRSL);
 };
 cwp->w_ppline = tp_line(cwp->tp_current)-tp_line(cwp->tp_hline)+1;
 if(toline-num_lines<0 ) {
	toline=0;
	num_lines=toline;
 } else {toline-=num_lines;};

 MoveLineCol(toline,cwp->goal_column);

 set_hmark(0,"prev_page");
 set_update(cwp,UPD_MOVE|UPD_WINDOW);
 undo_set_noglue();
 return (OK_CLRSL);
}

/* set normal (character) marked region */
int set_mark(num n)	
{
	return(setmark(REGION_CHAR));
}

int set_no_mark(num n)
{
	return(setmark(0));
}

/* set line region */
int set_line_mark(num n)	
{
	return(setmark(REGION_LINE));
}

/* set column region */
/* assigned to alt-c */
int set_column_mark(num n)	
{
	return(setmark(REGION_COLUMN));
}

/* set/clear mark region */
/* assigned to ^space */
int setmark(num n)
{
 if(!discmd) {
	// no selection without a window!
	return false; 	
 };
// MESG("setmark:");
 if(n) {
 	if(cwp->selection) {
		reset_region_textpoints();
	} else cwp->selection=n;
 } else {
	reset_region_textpoints();
 };

 if(cwp->selection) {
	 if(cwp->selection == REGION_LINE) { // line mark
		textpoint_set(cwp->w_smark,LineBegin(Offset()));
		textpoint_set(cwp->w_emark,LineEnd(Offset()));
 	} else {
		tp_copy(cwp->w_smark,cwp->tp_current);
 		tp_copy(cwp->w_emark,cwp->tp_current);
		cwp->hs[0].w_hselection=1;
 	};
 } else { // clear region selection and set region mode to normal
		set_update(cwp,UPD_FULL);
		tp_copy(cwp->w_smark,cwp->tp_current);
		tp_copy(cwp->w_emark,cwp->tp_current);
		cwp->hs[0].w_hselection=0;
	};
	set_update(cwp,UPD_MOVE);

    return (TRUE);
}

/* Swap the values of "." and "mark" in the current window. */
/* emacs key  "^X^X".  */
/* every window has its own marks!! */
int swap_mark(num n)
{
 TextPoint *p;
 if(!discmd) return false;
 if(!cwp->selection) return(FALSE);

 p=tp_dup(cwp->tp_current,cwp->id);
 tp_copy(cwp->tp_current,cwp->w_smark);
 tp_copy(cwp->w_smark,p);
 textpoint_delete(p);
 set_update(cwp,UPD_WINDOW);
 return (OK_RSTGOAL);
}

/*	the cursor is moved to a matching fence	*/
/*	default assigned to ^] */
int match_fence(num flag)
{
	int sdir;	/* direction of search (1/-1) */
	int count;	/* current fence level count */
	char ch;	/* fence type to match against */
	char ofence;	/* open fence */
	char c;	/* current character in scan */
	offs o;

	/* get the current character */
	o=Offset();
	ch = CharAt(o);

	/* setup proper matching fence */
	switch (ch) {
		case '(': ofence = ')'; sdir = FORWARD; break;
		case '{': ofence = '}'; sdir = FORWARD; break;
		case '[': ofence = ']'; sdir = FORWARD; break;
		case ')': ofence = '('; sdir = REVERSE; break;
		case '}': ofence = '{'; sdir = REVERSE; break;
		case ']': ofence = '['; sdir = REVERSE; break;
		case '>': ofence = '<'; sdir = REVERSE; break;
		case '<': ofence = '>'; sdir = FORWARD; break;
		default: ch=0; ofence= '{';sdir=REVERSE;
	};

	if(ch==0) { // we are not on a special character, find the first one!
		for(o--;o>0;o--) {
			ch=CharAt(o);
			if(ch=='(' || ch=='{' ) 
			{
				set_Offset(o);
				return(TRUE);
			};			
		};	
		return FALSE;
	};
	/* set up for scan */
	count = 1;
	if (sdir == REVERSE) o--;
	else o++;
	if(o<0 || o > FSize(cbfp)) return (FALSE);
 
	// reset highlighting

	/* scan until we find it, or reach the end of file */
	while (count > 0) {
		c = CharAt(o);
		if (c == ch) ++count;
		if (c == ofence) --count;
		if (sdir == FORWARD) {
			o++;
			if(o == FSize(cbfp)) break;
		} else {
			o--;
			if(o < 0) break;
		};
	}

	/* if count is zero, we have a match, move cursor */
	if (count == 0) {
		if (sdir == FORWARD) o--;
		else o++;
		set_update(cwp,UPD_MOVE);
		set_Offset(o);
		undo_set_noglue();
		return(OK_RSTGOAL);
	}

	msg_line("No match!");
	return(FALSE);
}

/* toggle lock movement */
/* bind to ^X^M or F12 */
int toggle_move_lock(num n)
{
 if(lock_move==0) lock_move=1;else lock_move=0;
 return TRUE;
}

/* toggle page movement synchronization  */
/* bind to F11 */
int toggle_sync_lock(num n)
{
 if(lock_sync==0) lock_sync=1;else lock_sync=0;
 return TRUE;
}

int page_move_down(num n)
{
 if(lock_sync) return both_up(n);
 else return prev_page(n);
}

int page_move_up(num n)
{
 if(lock_sync) return both_down(n);
 else return next_page(n);
}

/* common functions from display.c */

/* convert a string to printable format under current locale conditions */
/* convert binary control characters to something printable */
unsigned char *str2out(char *s)
{
    register unsigned char c;
    static unsigned char st[256];
	unsigned char *s1;

    s1=&st[0];
    while ((c = *s++) != 0 && (s1-st)<255 ) {
    	if( c<32 || c==127) { *s1++ ='^';c+=0x40;};

	/* find if a char is printable from a table */
	if( c>127 && c<160 && cbfp->b_lang) {
		*s1++ ='@';c-=0x40;};
		if(c==127) c=0x3F;

		*s1++ = c;
	};
	*s1=0;
	return(st);
}


/* Quit command. */
/*   emacs key  ^X^C, assigned also to ALT-X  */
int quit(num n)
{
	// MESG("quit:n=%d discmd=%d,macro_exec=%d,changed=%d",n,discmd,macro_exec,changes_occured());
        if (
		!discmd					/* no display forces it */
		|| macro_exec				/* in a macro just quit no questions */
		|| n>1
        || changes_occured() == FALSE         /* All buffers clean.   */
						/* User says it's OK.   */
        || (confirm("Modified files exist","Leave anyway?",1)) == TRUE) {
		// MESG("quit!!!");
		msg_line("");
		save_scratch_files();
		save_config();
		save_file_history(1);
		save_keys();
		drv_close();
        exit(0);
        };
		clear_message_line();
		set_update(cwp,UPD_STATUS);
        return (FALSE);
}

/* Save the current file and closes it. If the last one then quits. */
/* Assigned to F3 keybutton. */
int quick_close(num n)
{
    register FILEBUF *oldcb; /* original current buffer */
	register int status;
	if(buffer_is_connected(cbfp)) {
		msg_line("cannot close, buffer is connected");
		return FALSE;
	};
	oldcb = cbfp;                          /* save in case we fail */
	// MESG("quick_close:");
	// if(oldcb->connect_buffer!=NULL) MESG("	is connected!");
	if(cbfp->b_fname[0]!=CHR_LBRA)
	if(!dont_edit()) {
		// save the buffer
		msg_line("[Saving %s]",oldcb->b_fname);
		if ((status = save_file( n)) != TRUE) {

			if(cbfp->connect_buffer!=NULL) {
				// MESG("buffer is connected!");
				FILEBUF *connect_fp=cbfp->connect_buffer;
				select_filebuf(connect_fp);
				delete_filebuf(oldcb,1);
				return(OK_CLRSL);
			};

      		cbfp= oldcb;	/* restore fp */
       		return(status);
        }
	};
	if(cbfp->b_flag == FSDIRED && (cbfp->b_state & FS_VIEW) &&
		cbfp->b_fname[0]!=CHR_LBRA){ 
			delete_hmark(1);
			return dir_left(0);
	}else {
	// MESG("quick_close: close_file");
		if(cbfp->connect_buffer!=NULL) {
		// MESG("buffer is connected!");
			FILEBUF *connect_fp=cbfp->connect_buffer;
			select_filebuf(connect_fp);
			delete_filebuf(oldcb,1);
			return(OK_CLRSL);
		} else {;
			// MESG("no connected!!!");
			close_file(1);
			return(TRUE);
		};
	};

	if(cbfp == oldcb) 
	{ /* exit editor! this is the only file edited */
		quit(n);	
	} else {
//		MESG("quick_close: delete_filebuf");
		delete_filebuf(oldcb,1);
	}
	return(TRUE);
}

/* Abort command.  */
	/* emacs key ^G  */
int abort_cmd(num n)
{
	kbdmode = STOP;
//	MESG("abort_cmd:");
	if(!execmd) {
	clear_message_line();
	set_update(cwp,UPD_STATUS);
	drv_flush();
	};
    return(false);
}

int update_menu(num n)
{
//	MESG("update menu:");
	drv_flush();
	return true;
}

int noop(num n){
	MESG("No op!");
	return true;
};

/* editing functions */

/* Twiddle the two previous characters */
/* emacs key "^T". */
int twiddle(num n)
{
	char ts[16];
	int i;

	check_update_highlight(0);
	if(cbfp->b_flag & FSDIRED) return(0);
	if(dont_edit()) return FALSE;
	// if(FColumn(cbfp,Offset())<2) return FALSE;
	if(tp_col(cbfp->tp_current)<2) return FALSE;
	offs p1,p2;
	
	MoveLeftChar(cbfp);
	cbfp->line_from = GetLine();
	p2=Offset();
	if(FBolAt(cbfp,p2)) {
		MoveRightChar(cbfp);
		return(FALSE);
	};
	// while is not accent
	if(is_utf_accent(cbfp,p2)) {
		MoveLeftChar(cbfp);
		p2=Offset();
	};
	MoveLeftChar(cbfp);
	p1=Offset();
	if(is_utf_accent(cbfp,p1)) {
		MoveLeftChar(cbfp);
		p1=Offset();
	};

//	copy the block
	for(i=0;i<p2-p1;i++){
		ts[i]=CharAt(p1+i);
	};ts[i]=0;
// delete it (from p1-p2)
	DeleteBlock(0,p2-p1);
	MoveRightChar(cbfp);
	if(is_utf_accent(cbfp,Offset())) MoveRightChar(cbfp);
// insert deleted
	if(!insert_string(cbfp,ts,p2-p1)) return(false);
	set_end_line(cbfp,GetLine());
	set_modified(cbfp);
	check_update_highlight(2);
    return (TRUE);
}

/* Insert a tab  */
/* Assigned to ^I */
int tab(num n)
{
	// MESG("tab: b_flag=0x%X",cbfp->b_flag);
	if(cbfp->b_flag & FSDIRED) return(0);
	if(dont_edit()) return FALSE;
	if(cbfp->view_mode & VMHEX) {
		if(hex_input) hex_input=0;else hex_input=1;
		cbfp->view_mode ^= VMINP;
		set_update(cwp,UPD_MOVE);
		return true;
	}
	if(discmd) {
		if(cwp->selection) return insert_tab(n);
	};
	set_update(cwp,UPD_EDIT);
	return(insert_tab(n));
}

/* change tabs to spaces for a number of lines */
int detab_line(num n)
{
	num oldline;
//	MESG("detab_line:");
	if(cbfp->b_flag & FSDIRED) return(0);
	if(dont_edit()) return FALSE;
	if(n<1) return FALSE;
	
	// keep old value
	oldline = tp_line(cwp->tp_current);
	/* loop thru detabbing n lines */
    while (n) {
		if(cwp->selection) { 
		 	set_Offset(tp_offset(cwp->w_smark));
		};
		// ToLineBegin
		ToLineBegin();
		while(Offset() < tp_offset(cwp->w_emark)){
			/* detab the entire current line */
			while (Offset() < LineEnd(Offset())) 
			{
				/* if we have a tab */
				if (Char() == CHR_TAB) {
					forw_delete(1);
					insert_chr(tabsize - (GetCol() % tabsize),' ');
				} else
				next_character(1);
			};
			if(cwp->selection) {
				next_character(1);
				FNextLine(cwp->w_fp,Offset());
			};
//			MESG("detab: po=%lld o=%lld e=%lld",po,Offset(),tp_offset(cwp->w_emark));
		};
		if(cwp->selection) break;
		/* advance/or back to the next line */
		if(n>1) next_line(1);
		n --;
	};

	// restore old line,column
	MoveLineCol(oldline,cwp->goal_column);

	set_update(cwp,UPD_EDIT);
	return(TRUE);
}

/* change spaces to tabs for a number of lines !!*/
int entab_line(num n)
{
 register int cc;	/* current column */
 register int c;	/* current character */
 register int ns;	/* spaces to delete */
 num line1;
 utfchar uc;
 offs position;
 offs newpos;
// MESG("entab_line:");
 if(cbfp->b_flag & FSDIRED) return FALSE;
 if(dont_edit()) return FALSE;
 if(n<1) return FALSE;
 if(cbfp->hl->notab) return FALSE;	/* do not entab where not needed!  */

 line1=GetLine();

 if(cwp->selection) { 
 	position = LineBegin(tp_offset(cwp->w_smark));
 } else {
	position=LineBegin(Offset());
 };
 do {
  if(cwp->selection==0) 
  	textpoint_set(cwp->w_emark,LineEnd(position));

  for (cc=0,ns=0;position < tp_offset(cwp->w_emark);) 
  {
	newpos = FUtfCharAt(cbfp,position,&uc);
	c=uc.uval[0];
    if(cc%(tabsize)==0) {
	if(ns>1) { /* we must compress here */
		set_Offset(position-ns);
		forw_delete(ns);
		insert_chr(1,CHR_TAB);
		cc += tabsize;
		ns=0;
		position=Offset();
		continue;
	};ns=0;
    }
    if(c==' ') ns++; else ns=0;
	position=newpos;
	set_Offset(position);
	if(c==9) cc=next_tab(cc);
	else cc += get_utf_length(&uc);
	if(BolAt(position)) {ns=0;cc=0;};
  };
  if(cwp->selection) break; 
  if(n>1) next_line(1);
  n--;
 } while(n>0) ;

 MoveLineCol(line1,cwp->goal_column);
 set_update(cwp,UPD_EDIT);
 return(TRUE);
}

/* Delete blank lines around current position */
/* emacs key "^X^O". */
int del_blank_lines(num n)
{
 offs o1,o2;
 if(cbfp->b_flag & FSDIRED) return(0);
 if(dont_edit()) return FALSE;
 
 if(!emptyline(cbfp->tp_current)) return FALSE;

 o1 = Offset();
 // find next lines to delete
 while(next_line(1)) {
	if(!emptyline(cbfp->tp_current)) break;
 };
 o2=LineBegin(Offset());
 set_Offset(o1);
 // find previous lines to delete
 while(prev_line(1)) {
	if(!emptyline(cbfp->tp_current)) {
		next_line(1);
		break;
	};
 };
 o1=LineBegin(Offset());

 textpoint_set(cwp->w_smark,o1);
 textpoint_set(cwp->w_emark,o2);
 set_update(cwp,UPD_MOVE);
 return(delete_region());
}

/* Insert a newline. */
/* Assigned to "^M" or <enter>. */
int new_line(num n)
{
	register int    s;
	if(cbfp->b_flag & FSDIRED) return(0);
	if(dont_edit()) return FALSE;
	if (n < 1) return (FALSE);
	// MESG("new_line: lines %d",n);
	cbfp->line_from=GetLine();
	/* insert some lines */
	while (n--) {
		if ((s=insert_newline(cbfp)) != TRUE)
			return (s);
	};
	// MESG("new_line: new lines=%ld",cbfp->lines);
	cbfp->line_to=LastLine();
	undo_set_noglue();
	set_update(cwp,UPD_EDIT);
	return (TRUE);
}


int insert_date(num n)
{
 char *date=date_string(n);
	insert_string(cbfp,date,strlen(date));
	set_goal_column(cwp->tp_current->col,"date");
	// cwp->goal_column=cwp->tp_current->col;
	update_buffer_windows(cbfp,UPD_WINDOW);
	return TRUE;
}

/* Delete character(s), or selected region */
/* Assigned to DEL . */
int del_char(num n)
{ 
 int s;
 if(cbfp->b_flag & FSDIRED) return(dir_del1(n));
 if(dont_edit()) return FALSE;
 if(cbfp->view_mode & VMINP) {
 	// get the character
 	byte ch=CharAt(Offset());
	// change the corresponding byte to zero
	if(get_hex_byte()) {
		ch &= 0xF0;
	} else {
		ch &= 0x0F;
	};

	// put back the character
	set_Char(Offset(),ch);
	update_buffer_windows(cbfp,UPD_WINDOW);
	return TRUE;
 };
 if (n < 0) return (del_prev_char( -n));
 if(cwp->selection) { 
	s=cut_region(1); 
	return(s);
 };
 s=forw_delete((long)n);
 return (s);
}

/* Delete backwards. */
/* Assigned to both "BACKSPACE" and "^H". */
int del_prev_char(num n)
{
    register int    s;
	if(cbfp->b_flag & FSDIRED) return(0);
	// MESG("del_prev_char: lines=%ld",cbfp->lines);
	if(dont_edit()) return FALSE;
    if (n < 0) return (del_char(-n));
	s=back_delete(n);
	// MESG("del_prev_char: f=%X l1=%d l2=%d",cwp->w_flag,cbfp->line_from,cbfp->line_to);
	return(s);
}

/* Erase text till end of line., 
   move the deleted chars to clipboard */
/*  emacs key ^K */
int erase_eol(num n)
{
	if(cbfp->b_flag & FSDIRED) return(0);
	if(dont_edit()) return FALSE;
	
	textpoint_set(cwp->w_smark,Offset());
	textpoint_set(cwp->w_emark,LineEnd(Offset()));
	return(delete_region());
}

/* insert same characters at current position */
int insert_chr(int n,int c)
{
 char *s;
 int l=strlen(utfokey);
 num reverse=0;
 if(l==0) l=1;
 if(n==0) return(TRUE);

 check_update_highlight(0);
 cbfp->line_from = GetLine();
 if(cbfp->view_mode & VMINP){	/* Hex mode input  */
 	byte ch=CharAt(Offset());
	// convert c to hex!
	if(c>=0x30 && c<0x3A) c -= 0x30;
	else if(c>='A' && c<='F') { c-='A';c+=10;}
	else if(c>='a' && c<='f') { c-='a';c+=10;}
	else return FALSE;
	if(get_hex_byte()) {
		ch &= 0xF0;
		ch += c;
	} else {
		ch &= 0x0F;
		ch += c*16;
	};
	forw_delete(1);
	c=ch;
 };
 s=(char *)malloc(n*l+1);
 if(s==NULL) return (FALSE);
 
 if(l>1) {	/* it is a utf character!  */
 	int n1=n;
	char *ss=s;

	while(n1-->0){
		int l1=0;
		for(l1=0;l1<l;l1++) *ss++ = utfokey[l1];
	};*ss=0;

	utfokey[0]=0;
	utflen=0;
 } else
 { 
	l=1;
	memset(s,c,n);
	s[n]=0;
 };
 if(cwp->selection == REGION_COLUMN){
	// from first line of selection to last line of selection
	num s1,s2,s_temp;
	int ccol0=GetCol();
	int start_line,end_line;
	s1 = LineBegin(tp_offset(cwp->w_smark));
	s2 = tp_offset(cwp->tp_current);

	start_line = tp_line(cwp->w_smark);
	cbfp->line_from = start_line;

	end_line = tp_line(cwp->tp_current);

	if(start_line>end_line) {
		reverse=tp_offset(cwp->w_emark)+1;
			s2=s1;
			s1=LineBegin(tp_offset(cwp->w_emark));
			s_temp=start_line;start_line=end_line;end_line=s_temp;
	} else {
		set_end_line(cbfp,GetLine());
	};

	while(s1<=s2 && start_line <= end_line) {
		int ccol;

		ccol = valid_offset(s1,ccol0);
		s1+=ccol;
		set_Offset(s1);
		if(gmode_over && !Eol()) {
			DeleteBlock(n*l,0);
		};
	 	InsertBlock(cbfp,s,n*l,0,0);

		s1 = LineEnd(s1)+1;
		s2 += n*l;
		start_line++;
	};
	if(gmode_over) MoveRightChar(cbfp);
 } else {
 	InsertBlock(cbfp,s,n*l,0,0);
 }
 free(s);
 if(cbfp->view_mode & VMINP){
 	MoveRightChar(cbfp);
 } else {
	// move at end of insert
	if(reverse) {
		set_Offset(reverse);
	} else {
		textpoint_move(cbfp->tp_current,n*l);
	};
 };
 set_goal_column(cwp->tp_current->col,"inschr");
 // cwp->goal_column=cwp->tp_current->col;
 check_update_highlight(2);
 set_end_line(cbfp,GetLine());
 set_modified(cbfp);
 return(TRUE);
}


/* insert string */
int insert_string(FILEBUF *fp,char *c,int size)
{
 // check_update_highlight(0);
 if(!InsertBlock(fp,c,size,0,0)) return false;
 textpoint_move(fp->tp_current,size);
 // check_update_highlight(2);
 // set_update(cwp,UPD_EDIT);
 set_modified(fp);
 return TRUE;
}

/* insert a string with a new line */
int insert_string_nl(FILEBUF *fp,char *str)
{
	int len=strlen(str);
	if(!insert_string(fp,str,len)) return(FALSE);;
	if(!insert_newline(fp)) return (FALSE);
	return TRUE;
}

/* Insert a newline  at the current location  */
int insert_newline(FILEBUF *fp)
{
	if(!InsertBlock(fp,fp->EolStr,fp->EolSize,0,0)) return FALSE;
	textpoint_move(fp->tp_current,fp->EolSize);
	set_goal_column(fp->tp_current->col,"newline");
	update_buffer_windows(fp,UPD_EDIT);
	set_modified(fp);
	update_lines(fp);
	return TRUE;
}

/* delete a number of real chars (first find bytes of  utf chars ) */
int forw_delete(long n)
{
	int clen,i;
	offs o;
	check_update_highlight(0);
	cbfp->line_from = GetLine();
	if(cbfp->b_lang == 0) {
	/* find bytes to delete  */
	if(!(cbfp->view_mode & VMHEX)) {
		o=Offset();
		for(i=0;i<n;i++) {
			clen=FUtfCharLen(cbfp,o);
			o+=clen;
		};
		n = o-Offset();
	};
	}
	if(!DeleteBlock(0,n)) return FALSE;
	check_update_highlight(2);
	set_update(cwp,UPD_EDIT);	/* ????  */
	set_modified(cbfp);
	return TRUE;
}

/* delete previous character(s) */
int back_delete(long n)
{
 num reverse=0;

 check_update_highlight(0);
 // MESG("back_delete: lines=%ld",oldlines);
 cbfp->line_from = GetLine();
 if(cwp->selection == REGION_COLUMN)
 {
	// from first line of selection to last line of selection
	num s1,s2,s_temp;
	int ccol0=GetCol();
	int ncol0;
	int start_line,end_line;

	if(ccol0<1) return 0;

	s1 = LineBegin(tp_offset(cwp->w_smark));
	s2 = tp_offset(cwp->tp_current);
	start_line = tp_line(cwp->w_smark);
	end_line = tp_line(cwp->tp_current);

	if(start_line>end_line) {
		s2=s1;
		s1=LineBegin(tp_offset(cwp->w_emark));
		s_temp=start_line;start_line=end_line;end_line=s_temp;
		reverse=tp_offset(cwp->w_emark)-(valid_offset(s1,ccol0)-valid_offset(s1,ccol0-n));	
	} else {
		set_end_line(cbfp,GetLine());
	};

	if(ccol0<n) n=ccol0;
	ncol0=ccol0-n;
	while(s1<=s2 && start_line <= end_line) {
		int ccol;
		int ncol=valid_offset(s1,ncol0);
		int chars_to_delete=n;
		ccol = valid_offset(s1,ccol0);

		chars_to_delete=ccol-ncol;
		s1+=ncol;
		set_Offset(s1);
		// MESG("	delete: chars=%d ccol=%d ncol=%d ncol0=%d start=%d end=%d",chars_to_delete,ccol,ncol,ncol0,s1-ncol,s1);
		if(chars_to_delete==0) {
			chars_to_delete=n;
			DeleteBlock(chars_to_delete,0);
		} else {
			DeleteBlock(0,chars_to_delete);
		};
		s1 = LineEnd(s1)+1;
		s2 += n;
		start_line++;
	};
	 if(reverse) {
	 	set_Offset(reverse);
	 };
 	set_update(cwp,UPD_EDIT);
 } else {
 while(n--){
 	if(prev_character(1)) {
 		forw_delete(1);
 	} else return false;
 };
 };
 check_update_highlight(2);
 set_modified(cbfp);
 set_end_line(cbfp,GetLine());
 return TRUE;
}

/* set_linetext:	replace the current line with the passed in 0 terminated string*/
int set_linetext(char *txt)
{
	num len;
	int stat;
	/* delete the current line */
	ToLineBegin();
	DeleteBlock(0,LineEnd(Offset())-LineBegin(Offset()));
	len=strlen(txt);
	stat=InsertBlock(cbfp,txt,len,0,0);
	set_update(cwp,UPD_EDIT);
	set_modified(cbfp);
	set_modified(cbfp);
	return(stat);
}

// trim spaces at the end of current line
int trim_line(num n)
{
   offs   ptr,todelete,oldcur;
   int trimmed_at_start=0;
   int trimmed_at_end=0;
	if(cbfp->b_flag & FSDIRED) return(0);
   if(dont_edit()) return FALSE;

   oldcur=Offset();
	/* trim the end of the line  */
   todelete=LineEnd(Offset());
   ptr=todelete;
   while(!BolAt(ptr) && ((CharAt(ptr-1)==' ' || CharAt(ptr-1)==CHR_TAB)))
         ptr--;
	textpoint_set(cwp->tp_current,ptr);
	trimmed_at_end = todelete-ptr;
	if(trimmed_at_end>0)
	{
	  DeleteBlock(0,todelete-ptr);
	  cbfp->oldptr2=cbfp->ptr2;
	  set_update(cwp,UPD_EDIT);
	  set_modified(cwp->w_fp);
	};
	/* goto previous position if possible */
	if(oldcur<ptr)	textpoint_set(cwp->tp_current,oldcur);
	else textpoint_set(cwp->tp_current,ptr);

	/* trim the begin of the line  */
	oldcur=Offset();
	todelete=LineBegin(Offset());
	ptr=todelete;
	while(!EolAt(ptr) && ((CharAt(ptr)==' ' || CharAt(ptr)==CHR_TAB)))
	     ptr++;
	textpoint_set(cwp->tp_current,ptr);
	trimmed_at_start=ptr-todelete;
	if(trimmed_at_start>0)
	{
	  DeleteBlock(ptr-todelete,0);
	  cbfp->oldptr2=cbfp->ptr2;
	  set_modified(cwp->w_fp);
	  set_update(cwp,UPD_EDIT);
	}

	/* goto previous position if possible */
	if(oldcur>ptr)	textpoint_set(cwp->tp_current,oldcur-trimmed_at_start);
	else textpoint_set(cwp->tp_current,ptr);

	if(!macro_exec)
		msg_line("Line trimed by %d + %d chars!",trimmed_at_start,trimmed_at_end);
	return(TRUE);
}

/* Indent with tabs and spaces */
/* emacs key "^J". */
int insert_indent(num n)
{
    int    c,itabs,ispaces;
	offs curoffs;
	TextPoint *tp_indent;

	if(cbfp->b_flag & FSDIRED) return(0);
	if(dont_edit()) return FALSE;

	tp_indent = tp_dup(cwp->tp_current,cwp->id);
	undo_set_noglue();
	
	for(curoffs=LineBegin(Offset());!EolAt(curoffs);curoffs++) {
		c = CharAt(curoffs);
		if(c!=CHR_SPACE && c!=CHR_TAB) break;
		if(Offset()==curoffs) break;
	};

	textpoint_set(tp_indent,curoffs);
	cwp->curcol=tp_col(tp_indent);
	itabs=tp_col(tp_indent)/(tabsize);
	ispaces=tp_col(tp_indent)%(tabsize);

	textpoint_delete(tp_indent);
	while (n-- > 0) {
		if(cbfp->hl->notab) {
		if (insert_newline(cbfp) == FALSE
			|| (insert_chr(tp_col(tp_indent), CHR_SPACE)==FALSE))
				return (FALSE);
		} else {
		if (insert_newline(cbfp) == FALSE
			|| (insert_chr(itabs, CHR_TAB)==FALSE)
			|| (insert_chr(ispaces, CHR_SPACE)==FALSE))
				return (FALSE);
		}
	};
	set_update(cwp,UPD_EDIT);
	return (TRUE);
}

/* Delete one whole line , put it in local clipboard */ 
/* assigned to ^D  */
int delete_line(num n)
{
 int status;
#if	TNOTES
	if((cbfp->b_type & NOTE_TYPE 
		|| cbfp->b_type & NOTE_CAL_TYPE
		|| cbfp->b_type & NOTE_TODO_TYPE) && dont_edit()
	) {
	if(confirm("Delete note","",0)) {	
	 	dir_left(0);
		delete_tagnote(1);
	};
 };
#endif
 // MESG("delete_line:");

 if(dont_edit()) return FALSE;

 offs start=LineBegin(Offset());
 offs end=start;
 // MESG("delete_line: n=%d",n);
 while(n--) { 
 	end = FNextLine(cbfp,end);
	// MESG(" upto: %ld",end);
 }
 set_Offset(start);
 // MESG("delete_line: from %ld size=%ld",start,end-start);
 status=DeleteBlock(0,end-start);

 set_update(cwp,UPD_EDIT);
 set_modified(cwp->w_fp);
 return(status);
}

/* region related functions */

/* cut region beetween start and end marks, put in clipboard */
/* emacs key "^W" (or to ^X in case ^X esc sequences are not supported). */
/* if no flag then no update */
int cut_region(num flag)
{
 offs so,eo;
 num len;
	if(dont_edit()) return FALSE;
	if(cbfp->b_flag & FSDIRED) return(0);
	if(!cwp->selection) return FALSE;

	so = tp_offset(cwp->w_smark);
	eo = tp_offset(cwp->w_emark);
	if(so<0) return FALSE;

	if(eo < so ) { // change mark/point so point is always at start of block to delete
		textpoint_set(cwp->tp_current,eo);
		textpoint_set(cwp->w_smark,eo);
		textpoint_set(cwp->w_emark,so);
		len=so-eo;
	}
	else {
		len=eo-so;
		textpoint_set(cwp->tp_current,so);
	};

	// put it in the local clipboard also
	if(flag) {
		clipboard_copy(MainClipBoard);
		export_region(MainClipBoard);
	};
	if(cwp->selection == REGION_COLUMN) return(delete_box(flag));

	if(flag) setmark(0);

	if(flag) beginundogroup(cbfp->main_undo);
	if(flag) cbfp->main_undo->head_position=Offset();

	if(DeleteBlock(0,len)) {
		if(flag) {
			EndUndoGroup(cbfp->main_undo);
		};
		reset_region_textpoints();
		set_goal_column(cwp->tp_current->col,"cut");
		set_update(cwp,UPD_EDIT);
		set_modified(cbfp);
		return TRUE;
	} else return FALSE;
}

int delete_region()
{
 offs so,eo;
 num len;

	so = tp_offset(cwp->w_smark);
	eo = tp_offset(cwp->w_emark);
	if(so<0) return FALSE;

	if(eo < so ) { // change mark/point so point is always at start of block to delete
		textpoint_set(cwp->tp_current,eo);
		textpoint_set(cwp->w_smark,eo);
		textpoint_set(cwp->w_emark,so);
		len=so-eo;
	}
	else {
		len=eo-so;
		textpoint_set(cwp->tp_current,so);
	};

	if(cwp->selection == REGION_COLUMN) return(delete_box(0));
 	check_update_highlight(0);
	if(DeleteBlock(0,len)) {
		if(!check_update_highlight(2)) {
				cbfp->line_from=GetLine();
				cbfp->line_to=LastLine();
		};
		set_modified(cwp->w_fp);
		return TRUE;
	} else return FALSE;
}

/* copy region to clipboard */
/* Bound to "M-W" or ^C. */
int copy_region(num n)
{
 char s[80];

 if(!clipboard_copy(MainClipBoard)) return FALSE;
 export_region(MainClipBoard);
 
 set_update(cwp,UPD_WINDOW);
 setmark(0); // remove selection

 if(MainClipBoard->rect) 
	snprintf(s,80,"[%lldx%lld bytes copied]",MainClipBoard->width,MainClipBoard->height);
 else
	snprintf(s,80,"[%lld bytes copied]",MainClipBoard->width);
 msg_line(s);
 return TRUE;
}

int move_line_up(num n)
{
 if(tp_line(cwp->tp_current)<1) {
	// MESG("move_line_up: on first line!");
 	return false;
 };
 // mark current line
	// MESG("move_line_up:s b=%ld s=%ld e=%ld",cbfp->tp_current->offset,cwp->w_smark->offset,cwp->w_emark->offset);
	setmark(0);
	ToLineBegin();
	setmark(REGION_CHAR);
	// MESG("move_line_up:0 b=%ld s=%ld e=%ld",cbfp->tp_current->offset,cwp->w_smark->offset,cwp->w_emark->offset);
	MoveLineCol(tp_line(cwp->tp_current)+1,0);
	// MESG("move_line_up:1 b=%ld s=%ld e=%ld",cbfp->tp_current->offset,cwp->w_smark->offset,cwp->w_emark->offset);
	ToLineBegin();
	tp_copy(cwp->w_emark,cwp->tp_current);
	// MESG("move_line_up:2 b=%ld s=%ld e=%ld",cbfp->tp_current->offset,cwp->w_smark->offset,cwp->w_emark->offset);
	
 // put in clipboard marked region
	clipboard_copy(MainClipBoard); 
	// MESG("move_line_up:3 copy %ld",MainClipBoard->width);
	// MESG("move_line_up:3 b=%ld s=%ld e=%ld",cbfp->tp_current->offset,cwp->w_smark->offset,cwp->w_emark->offset);
	
 // delete current
	delete_region(1); setmark(0);
 // go up one line and paste
	MoveLineCol(tp_line(cwp->tp_current)-1,0);
	paste_region(1);
 // go up one line
	MoveLineCol(tp_line(cwp->tp_current)-1,0);
	set_update(cwp,UPD_EDIT);
	return 1;
}

int move_line_down(num n)
{

 if(tp_line(cwp->tp_current)> tp_line(cbfp->tp_text_end)-2) {
	// MESG("move_line_down: on last line!");
 	return false;
 };
 // mark current line
	MESG("move_line_down:s b=%ld s=%ld e=%ld",cbfp->tp_current->offset,cwp->w_smark->offset,cwp->w_emark->offset);
	setmark(0);
	ToLineBegin();
	setmark(REGION_CHAR);
	// MESG("move_line_down:0 b=%ld s=%ld e=%ld",cbfp->tp_current->offset,cwp->w_smark->offset,cwp->w_emark->offset);
	MoveLineCol(tp_line(cwp->tp_current)+1,0);
	// MESG("move_line_down:1 b=%ld s=%ld e=%ld",cbfp->tp_current->offset,cwp->w_smark->offset,cwp->w_emark->offset);
	ToLineBegin();
	tp_copy(cwp->w_emark,cwp->tp_current);
	// MESG("move_line_down:2 b=%ld s=%ld e=%ld",cbfp->tp_current->offset,cwp->w_smark->offset,cwp->w_emark->offset);
	
 // put in clipboard marked region
	clipboard_copy(MainClipBoard); 
	// MESG("move_line_down:3 copy %ld",MainClipBoard->width);
	// MESG("move_line_down:3 b=%ld s=%ld e=%ld",cbfp->tp_current->offset,cwp->w_smark->offset,cwp->w_emark->offset);
	
// delete current
	delete_region(1); setmark(0);
 // go down one line and paste
	MoveLineCol(tp_line(cwp->tp_current)+1,0);
	paste_region(1);
 // go up one line
	MoveLineCol(tp_line(cwp->tp_current)-1,0);
	set_update(cwp,UPD_EDIT);
	return 1;
}

// paste_region function (^V)
// operates on cbfp
int paste_region(num n)
{
 int status;
	if(dont_edit() || cbfp->b_flag & FSDIRED )return false;

	status=clipboard_paste(MainClipBoard);
	if(cwp) {
    	set_update(cwp,UPD_EDIT);
		// reset_region_textpoints();
		setmark(0);
	};
 return(status);
}

ClipBoard * clipboard_new()
{
	ClipBoard *clip;
	clip=(ClipBoard *)emalloc(sizeof(ClipBoard),"clipboard_new");
	clip->text=NULL;
	clip->width=0;
	clip->height=0;
	return clip;
}

void init_clipboard()
{
	MainClipBoard=clipboard_new();
}

void normalize_region(FILEBUF *bp, offs *offs1, offs *offs2)
{
 offs o;
	if(*offs1>*offs2) {
		o=*offs1;
		*offs1=*offs2;
		*offs2=o;
	};
	if(*offs2>FSize(bp)) *offs2=FSize(bp);
}

/* Lower case region. */
/* emacs key "C-X C-L" */
int lowerregion(num  n)
{
	char *region_str=NULL;
	offs offs1,offs2;

	if(cbfp->b_flag & FSDIRED) return(0);
	if(dont_edit()) return FALSE;
	offs1=Offset();
	offs2=tp_offset(cwp->w_smark);

	normalize_region(cbfp,&offs1, &offs2);

//	MESG("lowerregion: from %ld to %ld",offs1,offs2);
	region_str = (char *)emalloc(offs2-offs1+1,"lowerregion");
	GetBlock(cbfp,(char *)region_str,offs1,offs2-offs1);
	region_str[offs2-offs1]=0;
//	MESG("String to lower is [%s]",region_str);

	get_lowercase_string(region_str,region_str);
	set_Offset(offs1);
	delins(offs2-offs1,(char *)region_str);
	efree(region_str,"lowereregion");
	set_modified(cwp->w_fp);
	setmark(0);
	set_update(cwp,UPD_WINDOW);
	return (TRUE);
}

/* Capitalize region */
/* emacs key "c-x c-u".  */
int upperregion(num  n)
{
	char *region_str=NULL;
	offs offs1,offs2;

	if(cbfp->b_flag & FSDIRED) return(0);
	if(dont_edit()) return FALSE;
	offs1=Offset();
	offs2=tp_offset(cwp->w_smark);

	normalize_region(cbfp,&offs1, &offs2);

//	MESG("upperregion: from %ld to %ld",offs1,offs2);
	region_str = (char *)emalloc(offs2-offs1+1,"upperregion");
	GetBlock(cbfp,region_str,offs1,offs2-offs1);
	region_str[offs2-offs1]=0;

	get_uppercase_string(region_str,region_str);
	set_Offset(offs1);
	delins(offs2-offs1,(char *)region_str);
	efree(region_str,"uppereregion");
	set_modified(cwp->w_fp);
	setmark(0);
	set_update(cwp,UPD_EDIT);
	return (TRUE);
}

// delete_box: undo information is passed for each line separatly. FIXME
int delete_box(num n)
{
 num h,l1,l2,col;
 offs cofs,lbo;
 int w;
 FILEBUF *fp = cwp->w_fp;
 cofs=tp_offset(cwp->tp_current);

 l1=tp_line(cwp->tp_current);
 l2=tp_line(cwp->w_emark);
 if(l2>l1) h=l2-l1+1;
 else h=l1-l2+1;

 if(tp_col(cwp->tp_current)>tp_col(cwp->w_smark)) 
 {
	col = Column(tp_offset(cwp->w_smark));
	w = Column(tp_offset(cwp->tp_current))-col+1;
 } else {
	col=Column(tp_offset(cwp->tp_current));
	w=Column(tp_offset(cwp->w_emark))-col+1;
 };

 reset_region_textpoints();
 lbo=LineBegin(tp_offset(cwp->tp_current));
 while(h-->0) {
	if(LineEnd(lbo)-lbo>col) {
		offs p1,p2;
		textpoint_set(cwp->tp_current,lbo);
		detab_line(1);

		p1=MoveToColumn(col);
		p2=MoveToColumn(col+w);

		beginundogroup(fp->main_undo);
		fp->main_undo->head_position=Offset();

 		DeleteBlock(p2-p1,0);
		EndUndoGroup(fp->main_undo);
	};
	lbo=FNextLine(fp,lbo);
 };
 set_Offset(cofs);
 setmark(0);
 set_goal_column(cwp->tp_current->col,"del box");
 set_update(cwp,UPD_EDIT+UPD_WINDOW);
 set_modified(cwp->w_fp);
 return TRUE; 
}

void set_global_flag(int option_flag,int value)
{
 if(value) gmode |= option_flag;
 else gmode &= ~option_flag;
}

void toggle_global_flag(int option_flag)
{
 if(gmode & option_flag) gmode &= ~option_flag;
 else gmode |= option_flag;
// MESG("toggle_global_flag: new is %d",gmode);
}

/* options related functions */

int set_optionf(int option_flag,int global)
{
 char value[80];
 int n;
 FILEBUF *fp=cwp->w_fp;

 if(fp == NULL) return(FALSE);
 value[0]=0;
 if(nextarg("Overwrite :",value,80,true)!=TRUE) return(FALSE);
 n=get_val();
 if(n==1) {
	if (global) gmode |= option_flag;
 	fp->b_mode |= option_flag;
 } else {
	if (global) gmode &= ~option_flag;
 	fp->b_mode &= ~option_flag;
 };
 update_status();
 return(TRUE);
}


void toggleoptionf(int option_flag, int global)
{
 FILEBUF *fp=cbfp;
 if(global) {
 	if (gmode & option_flag) gmode &= ~option_flag;
 	else gmode |= option_flag;

 };
// MESG("toggleoptionf: f=%d global=%d gmode=%X",option_flag,global,gmode);

 if(fp == NULL) return;

 if(fp->b_mode & option_flag) fp->b_mode &= ~option_flag;
 else fp->b_mode |= option_flag;
 update_status();
}

/* set edit mode overwite */ 
int set_over(num n)
{
 gmode_over=n;
 return TRUE;
}

/* set edit mode case sensitive search */
int set_case(num n)
{
 gmode_exact_case=n;
 return TRUE;
}

/* toggle highlight on/off */
int toggle_highlight(num n)
{
 if(syntaxh) syntaxh=0;else syntaxh=1;
 set_update(cwp,UPD_ALL);
 return TRUE;
}

/* set regular expression in search */
int set_regexp(num n)
{
// MESG("set_regexp n=%d",n);
 gmode_reg_exp=n;
 return TRUE;
}

/* toggle edit mode case sensitive search */
int toggle_case(num n)
{
 if(gmode_exact_case) gmode_exact_case=0;else gmode_exact_case=1;
 return TRUE;
}

/* toggle regular expression in search */
int toggle_regexp(num n)
{
 if(gmode_reg_exp) gmode_reg_exp=0;else gmode_reg_exp=1;
 return TRUE;
}

/* Toggle insert - ovewrite mode.  */
int toggle_over(num n)
{
 if(gmode_over) gmode_over=0;else gmode_over=1;
 update_status();
 return TRUE;
}

int set_crypt(num n)
{
	set_optionf(EMCRYPT,0);
 return TRUE;
}

int toggle_crypt(num n)
{
 if(cbfp->b_flag & FSMMAP) {
	msg_line("mmap cannot not be crypted, goto edit mode first!");
 	return FALSE;
 };
 toggleoptionf(EMCRYPT,0);
 return TRUE;
}

int set_just(num n)
{
 set_optionf(EMJUST,0);
 return TRUE;
}

/* toggle justify mode */ 
/* emacs key ^XJ */
int toggle_just(num n)
{
 toggleoptionf(EMJUST,0);
 return TRUE;
}


int read_colors(num n)
{
// MESG("read_colors:n=%d",n);
 if(color_scheme_read())
 {
	msg_line("Read from color file!");
	change_color_scheme(color_scheme_ind+1);
	return(TRUE);
 } else return 0;
}


int save_colors(num n)
{
	return color_scheme_save();
}

/* --- */
