/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

 	This is the GTK version of display. Each window is drawned in its own
	GTK window.
 */

#include	"xe.h"

extern int quote_flag;
extern int describe_flag;
extern int sort_mode;
extern alist *window_list;
extern int nnarg;
extern FILEBUF *cbfp;

void free_virtual_window(WINDP *wp);
char *time2a();

//int newxy(WINDP *wp);
WINDP * make_split(WINDP *oldwindow);
GWINDP *ge_cedit_new(GtkWidget *parent, WINDP *wp,int ptype);
void delete_gwindow(WINDP *wp);
void drv_free_win(WINDP *wp);
void set_cursor(int flag,char *from);

GtkWidget *popup_select_on, *popup_select_off, *popup_boxed_region, *popup_sort_menu;


/* initialize screen */
void vtinit(int argc, char **argv)
{
    drv_init(argc, argv);	/* initialize the display driver */
    drv_open();				/* open the screen */
}

/*
 * Split the current window horizontally
 * Bound to "C-X 2". 
 */
int split_window(int n)
{
 GWINDP *gwnew;	// new window
 WINDP *wpnew;
 // MESG("/nsplit_window: %d",cwp->id);
 if(cwp->w_ntrows < 15) {
 	msg_line("too small to horizontal split!");
	return(FALSE);
 };
 wpnew = make_split(cwp);
 if(wpnew==NULL) return(FALSE);
 gwnew = ge_cedit_new((GtkWidget *)cwp->gwp->bte->pbox,wpnew,1); // vpan
 if(gwnew==NULL) return(FALSE);
 events_flush();
 set_cursor(0,"split_window");
 return(TRUE);
}

/*
 * Split the current window vertically
 * Bound to "C-X 3". 
 */
int vsplit_window(int n)
{
 GWINDP *gwnew;	// new window
 WINDP *wpnew;
 if(cwp->w_ntcols < 40) {
//	MESG("vsplit_window: cols=%d",cwp->w_ntcols);
 	msg_line("too small for vertical split!");
	return(FALSE);
 };
 wpnew = make_split(cwp);
 if(wpnew==NULL) return(FALSE);
 gwnew = ge_cedit_new((GtkWidget *)cwp->gwp->bte->pbox,wpnew,2);
 if(gwnew==NULL) return(FALSE);
 events_flush();
 return(TRUE);
}


/*
 * Delete the current window, rearange screen space
 * Bound to ^X0.
 */
int delete_window(int n)
{	/* new gtk version! */
	WINDP *new_wp;	/* window to go and receive deleted space */
	WINDP *to_delete=cwp;
	if(window_list->size==1) return(TRUE);	/* this is the only window */


	if(window_list->head->data == cwp) {
		lbegin(window_list);
		lmove_to_next(window_list,1);
		new_wp=lget_current(window_list);
		lbegin(window_list);
	} else {
		lfind_data((void *)cwp,window_list);
		lmove_to_previous(window_list,1);
		new_wp=(WINDP *)lget(window_list);
	};

	/* remove current window */
	unlink_filebuf(to_delete);
	lfind_data((void *)to_delete,window_list);
	remove_current_from_list(window_list);

	set_current_window(new_wp,"delete_window");
	free_window_data(to_delete); 

	set_update(cwp,UPD_WINDOW|UPD_ALL);
	update_status();

	events_flush();
	return(TRUE);
}

/* dummy for GTK */
void list_dir1(char *st)
{
//s MESG("list_dir1:[%s] this is dummy in GTK!",st);
}

/* resize rows. Dummy for gtk */
int window_row_resize(int n)
{
	return FALSE;
}

/* resize columns. Dummy for gtk */
int window_column_resize(int n)
{
	return FALSE;
}


int window_row_increase(int n)
{
	return FALSE;
}

int window_column_increase(int n)
{
	return FALSE;
}

int window_row_decrease(int n)
{
	return FALSE;
}

int window_column_decrease(int n)
{
	return FALSE;
}


/*
 * Send a command to the terminal to move the hardware cursor to row "row"
 * and column "col". The row and column arguments are origin 0.
 */ 
void movecursor(int row, int col)
{
	show_position_info(0);
	// MESG("movecursor: row=%d",row);
	drv_move(row+(cwp->w_fp->b_header!=NULL),col);
}

int mouse_move(int n)
{
	return true;
}

/* clear the common status line */
void clear_message_line()
{
	if(app_error == 0)  {
		drv_msg_line(time2a());
	} else app_error--;
}

int describe_key(int n)	/* describe the command for a certain key */
{
	/* prompt the user to type us a key to describe */
	msg_line(": describe-key ");
	describe_flag=1;
	return TRUE;
}

/*
 * Quote the next character, and insert it into the buffer. All the characters
 * are taken literally. Bound to "^Q"
 */
int quote(int n)
{
	if(dont_edit()) return FALSE;
//	MESG("quote: n=%d nnarg=%d",n,nnarg);
	if(kbdmode==BPLAY||kbdmode==PLAY) {
		char c=get_next_key(kbdmode);
		insert_chr(n,c);
		return TRUE;
	};
	quote_flag=TRUE;
	
	return OK_ARG;
}


extern GtkWidget *parent;

int show_version(int n)
{
 char version_str[64];
 int res;
 res=snprintf(version_str,64,"Version %s",VERSION);
 if(res==64) MESG("version truncated!");
 gtk_show_about_dialog(GTK_WINDOW(parent),
//        "authors", authors,
        "copyright", "Copyright Kostas Triantafillou",
        "comments", "A text editor with a macro language\n and active scripting,\na note organizer\nand a directory viewer",
        "version", version_str,
//        "website", "http:www.griv.org",
        "license", "GPL2",
        NULL
    );
 return 1;
}

/* update status line  */
void status_line(WINDP *wp)
{
    char *cp;
    int c;
    int n=0;		/* cursor position count */
    FILEBUF *bp;
    int i;			/* loop index */
	unsigned int max_t=100;
	char st[512], *stp;
	char m_st[32]; // mode string
	char fdname[MAXLLEN];
	char modecode[]=MODECODE;

	if(!discmd) return;
	if(wp==NULL) return;

	// create mode string
    bp = wp->w_fp;
	// MESG("status_line: %d",wp->id);
//	in debug mode print window number at bottom left
	if(debug_flag()) 	snprintf(m_st,32,"%d",wp->id);
	else m_st[0]=0;

	strlcpy(fdname,bp->b_dname,MAXLLEN);
	stp = m_st+strlen(m_st);
	*stp++ = ' ';
#if	TNOTES
		if(bp->b_type & NOTE_TYPE 
			|| bp->b_type & NOTE_CAL_TYPE
			|| bp->b_type & NOTE_TODO_TYPE
		) {
			*stp++='N';n++;
		};
#endif
	if(wp->w_fp->b_state & FS_CHG) {*stp++ = '*'; n++;}

	if(wp->w_fp->b_flag & FSDIRED){
		if(wp->w_fp->b_state & FS_VIEW) {*stp++ = 'V'; n++;}
		else {*stp++ = 'D'; n++;};
	} else {
		if(wp->w_fp->b_state & FS_VIEW) {*stp++ = 'V'; n++;}
	};
	if(wp->w_fp->b_flag & FSMMAP) {*stp++ = 'M'; n++;};
	if(wp->w_fp->b_flag & FSNLIST) {*stp++ = 'L'; n++;};

#if	0
	if(wp->w_fp->b_flag & FSDIRED) {	/* add sort mode  */
		if(sort_mode>7) *stp++ = 'A'; 
		else if(sort_mode>5) *stp++ = 'C';
		else if(sort_mode>3) *stp++ = 'T';
		else if(sort_mode>1) *stp++ = 'S';
		else *stp++ = 'N';
		if((sort_mode %2)==1) { *stp++ = 'r'; };
	};	
#endif
	*stp++=' ';
	if(gmode_over) *stp++ = 'O';

//	MESG("b_mode=%X",wp->w_fp->b_mode);
	for (i = 0; i < 8 ; i++)	/* add in the mode flags */
		if (wp->w_fp->b_mode & (1 << i)) {
//			MESG(" - i=%d m=%c ",i,modecode[i]);
			*stp++ = modecode[i];n++;
		};
	if(wp->w_fp->bom_type == FTYPE_UTF8BOM) { *stp++ = '8';};
	if(wp->w_fp->bom_type == FTYPE_UTF16BOM) { 
		*stp++ = '1';*stp++ = '6';
	};
	*stp=0;

	/* directory name */
	stp = st;*stp++=' ';

    if( bp->b_fname[0]!=CHR_LBRA )
	{
	 cp = get_pfname(fdname,bp->b_fname,max_t);
	 for(;(c= *cp++);n++) *stp++ = c;
    } else {
		if( bp->b_flag & FSDIRED) {
	 		cp = &bp->b_dname[0];
			if(strlen(cp)+strlen(bp->b_fname) < max_t  || bp->b_fname[0]==0 ) 
			{
			if(strlen(cp)>max_t) cp = cp + (strlen(cp) - max_t);
			for(;(c= *cp++);n++) *stp++ = c;
			} else {
				*stp++ = '.';n++;
			};
			*stp++ = '/';n++;
		}
	    if (bp->b_fname[0] != 0)            /* File name. */
	    {
	        cp = &bp->b_fname[0];
			for(;(c= *cp++);n++) {
				*stp++ = c;
			};
		
	    	*stp++ = ' ';n++;
	    }
	};
	*stp=0;

	// create file name string
	put_string_statusline(wp,m_st,-1);	
	put_string_statusline(wp,st,0);	
	wp->w_flag &= ~UPD_STATUS;
	if(wp==cwp) titletext();
}

// dummy function
void set_1window()
{
}


extern int clen_error;


/* -- */

