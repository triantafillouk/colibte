/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

*/
/* common screen functions for all interfaces  */

#include "xe.h"
#include "color.h"

void vtputwc(WINDP *wp, utfchar *uc);
extern int save_trace(int n);
extern int drv_type;
void set_window_font(WINDP *wp);
void drv_start_window_update(WINDP *wp);
int get_utf_length(utfchar *utf_char_str);
int  SUtfCharAt(char *utfstr, int offset, utfchar *uc);
int eq1(int x,int y,int z);
void vteeoc(WINDP *wp, int endcol);
int get_current_line();
void upd_move(WINDP *wp,char *from);
void vteeol(WINDP *wp, int selected,int inside);
int SUtfCharLen(char *utfstr,int offset,utfchar *uc);
int line_bcolor=0;
void drv_clear_line(WINDP *wp,int row);
void upd_all_wrap_lines(WINDP *wp,char *from);
void update_window_nowrap(WINDP *wp,int force);
void update_window_wrap(WINDP *wp,int force);
void upd_part_wrap(WINDP *wp,char *from);
offs check_next_char(FILEBUF *fp,offs o,int *col);

extern COLOR_SCHEME *current_scheme;

int specialh=0;
/* highlight state  */
extern int hquotem;
extern int prev_hquotem;
extern int slang;
extern int stop_word_highlight;
extern int start_word_highlight;
extern FILEBUF *cbfp;
extern int drv_colors;
extern int sel_tags[11];
extern int num_of_selected_tags;

int col0,col1;

extern int cursor_showing;
extern alist *window_list;
extern float CLEN;
extern int CHEIGHTI;

int	update_all  = TRUE;                 /* TRUE if screen is garbage	*/
int	noupdate=1;		/* no need to update the screen , no display started */

utfchar double_hline = { "═" };
utfchar single_hline = { "↪" };
utfchar next_l = { " " };
// utfchar next_l = { "↘" };
utfchar first_l = { "►" };

//num cline_end=0;
//num cstart_offset;

#if	NUSE
int addutfvchar(char *str, vchar *vc, int pos, FILEBUF *w_fp)
{
 unsigned char ch;
 int i;

 if(w_fp->b_lang == 0) {
 for(i=0;i<4;i++) {
 	ch=vc->uval[i];
	if(ch) str[pos++]=ch;
	else break;
 };
 } else str[pos++]=vc->uval[0];
 return pos;
}
#endif

void set_screen_update(int flag)
{
	noupdate = flag ? 0:1;
}

/* set virtual character with color */
void svwchar(vchar *vc, utfchar *uc,int b_color,int f_color)
{
 int i;
 for(i=0;i<8;i++) {
 	vc->uval[i]=uc->uval[i];
 };
 if(uc->uval[0]==0xF0) {
	vc->uval[4]=0;
	vc->uval[5]=0;
	vc->uval[6]=0;
	vc->uval[7]=0;
 };
 vc->bcolor=b_color;
 vc->fcolor=f_color%256;
 if(f_color>255) {
 	vc->attr = f_color - (vc->fcolor);
	// MESG("! %1s f=%2d a=%X",vc->uval,vc->fcolor,vc->attr);
 } else {
 	vc->attr = current_scheme->color_style[vc->fcolor].color_attr;
	// if(vc->uval[0]!=' ')
	// MESG("~ %1s f=%2d a=%X",vc->uval,vc->fcolor,vc->attr);
 };
//  vc->attr = f_color - vc->fcolor;
#if	0
 vc->display_width=CLEN;
 vc->display_height=CHEIGHTI;
#endif
}

/* set non utf char with color  */
void  svchar(vchar *vc,int val,int b_color,int f_color)
{
 memset(vc->uval,0,8);
 vc->uval[0]=val;
 // if(vc->attr) MESG("svchar ul");
 vc->bcolor=b_color;
 vc->fcolor=f_color%256;
 if(f_color>255) {
 	vc->attr = f_color - (vc->fcolor);
	// MESG("# %1s f=%2d a=%X",vc->uval,vc->fcolor,vc->attr);
 } else {
 	vc->attr = current_scheme->color_style[vc->fcolor].color_attr;
	// if(vc->uval[0]!=' ')
	// MESG("- %1s f=%2d a=%X",vc->uval,vc->fcolor,vc->attr);
 };
}

/* set virtual color */
void  svcolor(vchar *vc,int b_color,int f_color)
{
 vc->bcolor=b_color;
	 vc->fcolor=f_color % 256;
	 if(f_color>255) {
	 	vc->attr = f_color - (f_color % 256);
		 // MESG("- %1s f=%2d a=%X",vc->uval,vc->fcolor,vc->attr);
	 } else {
		 vc->attr = current_scheme->color_style[vc->fcolor].color_attr;
		 // MESG("+ %1s f=%2d a=%X",vc->uval,vc->fcolor,vc->attr);
	};
}

/* set virtual special multiple  character */
void svmchar(vchar *vc,int ustr,int b_color,int f_color,int count)
{
 // if(f_color>255) MESG("svmchar: ul count=%d",count);
 while(count>0) {
 	svchar(vc,ustr,b_color,f_color);
	count--;
	vc++;
 };
}

/* Set the update flag for all windows  */
void set_update(WINDP *wp_toset, int flag)
{
 FILEBUF *fp;
 WINDP *wp;

 if(wp_toset==NULL) {
 	update_all=true;
	return;
 };
 fp = wp_toset->w_fp;
 if(flag & UPD_EDIT) {
	{	/* First change, so	*/
		flag |= UPD_STATUS; 		/* update mode lines.	*/
 	}
 };
 wp_toset->w_flag |= flag;

 if( ( flag & (UPD_EDIT) ) || flag & (UPD_FULL)  ) {
 lbegin(window_list);
 while((wp=(WINDP *)lget(window_list))!=NULL)
 {
	// if ((wp->w_fp == fp && (flag & UPD_EDIT)) && wp!=wp_toset ) 
	if ((wp->w_fp == fp && (flag & UPD_EDIT)) ) 
	{
		if( (((tp_line(wp->tp_hline) <= tp_line(wp_toset->tp_current)) &&
			(tp_line(wp_toset->tp_current) <= (tp_line(wp->tp_hline)+wp->w_ntrows)))) || is_wrap_text(wp->w_fp)
		)
		{
			wp->w_flag |= flag;
			tp_copy(wp->tp_hsknown, wp_toset->tp_hsknown);
			wp->hs[0].w_hquotem = wp_toset->hs[0].w_hquotem;
			wp->hs[0].w_slang = wp_toset->hs[0].w_slang;
			// MESG("set_update: window %d flag %d",wp->id,wp->w_flag);
		};
	};
 };
 };

 if(flag & UPD_ALL) update_all=1;
}

int show_key(int key)
{
 char *ptr;
 char *description;
 char st[MAXMLEN];
 char *d;
	ptr = function_name(key_function(key,1),&description);
	d=description;
	if(ptr[0]!=0) { 
		if(strcmp("execsub",ptr)==0) snprintf(st,MAXMLEN,"subroutine [%s]",xe_key_name(key));
		else snprintf(st,MAXMLEN,"%s = %s,",xe_key_name(key),ptr);
		strlcat(st,d,MAXMLEN);
	} else {
		snprintf(st,MAXMLEN,"%s not assigned key=%X!",xe_key_name(key),key);
	};
	msg_line("%s",st);
	return FALSE;
}

/* Print on message line */
void msg_line(char *fmt, ...)
{
 va_list args;
 char mline[512];
	if(macro_exec && !execmd) return;

    va_start(args, fmt);
    vsnprintf(mline,512, fmt, args);
	
    va_end(args);
	
	if(execmd) {
		if(strlen(mline)>0 && mline[0]!=' ') printf("%s\n",mline);
	} else { 
		if(app_error || debug_flag()) 
		{ 
			if(strlen(mline)>0 && mline[0]!=' ') MESG("%s",mline); 
		};
		drv_msg_line(mline);
		set_update(cwp,UPD_MOVE);	/* to force restore cursor after message  */
	};
}

int error_line(char *error_message,...)
{
 va_list args;
 static char mline[512];
 	va_start(args,error_message);
	vsnprintf(mline,511,error_message,args);
	va_end(args);
	app_error=1;
	msg_line("%s",mline);
	if(debug_flag()) MESG("%s",mline);
	app_error=0;
	return FALSE;
}

/*
 * Set the virtual cursor to the specified row and column 
 */
 void vtmove(WINDP *wp,int row,int col)
{
	wp->vtrow = row;
    wp->vtcol = col;
}

void upd_column_pos_hex()
{
	if(cbfp->view_mode & VMINP) {
		cwp->curcol += 10 + 2*cwp->curcol+get_hex_byte();
	} else {
		cwp->curcol+=HSTART+1;
	};
	if(cwp->curcol+10<cwp->w_ntcols) cwp->w_lcol=0;
	else if(cwp->curcol - cwp->w_lcol > cwp->w_ntcols-2)
	if(cwp->w_ntcols - cwp->curcol+cwp->w_lcol < 2) cwp->w_lcol=cwp->curcol - cwp->w_ntcols+5;
}

void upd_column_pos_wrap()
{
	// MESG("upd_column_pos_wrap: w=%d",cwp->w_width);
	cwp->w_lcol=0;
	cwp->curcol = cwp->curcol % (cwp->w_width) + cwp->w_infocol;
	// MESG("upd_column_pos_wrap: %d < %d",cwp->curcol,cwp->w_ntcols);
}

/*	upd_column_pos:	update the column position of the hardware cursor and handle extended lines.  */
void upd_column_pos()
{
 int mo;
 int total_columns;
 // if(is_wrap_text(cbfp)) show_time("upd_column_pos1",1);
	// cwp->curcol = GetCol();
	cwp->curcol = tp_col(cbfp->tp_current);
	// MESG("upd_column_pos: curcol=%d",cwp->curcol);
 // if(is_wrap_text(cbfp)) show_time("upd_column_pos2",1);
	total_columns=cwp->w_width;
	if(cwp->w_ntcols==0) return;
	if(cbfp->view_mode & VMHEX){ 
		upd_column_pos_hex();
	} else 
	if(is_wrap_text(cbfp)) {
		// MESG("upd_column_pos_wrap:");
		upd_column_pos_wrap();
		// MESG("upd_column_pos_wrap:ok");
	} else
	if(cwp->curcol < (cwp->w_lcol)+2 && cwp->w_lcol>0) 
	{
		if(cwp->curcol+1 < total_columns ) { 
			cwp->w_lcol=0;
		} else {
			cwp->w_lcol-= (total_columns)/3;
		};

	} else 
	if(cwp->curcol > cwp->w_lcol+(total_columns)-4) 
	{
		cwp->w_lcol = cwp->curcol-(total_columns)/1.5;
//		MESG("upd_column_pos: w_lcol=%d curcol=%d total=%d w_ntcols=%d",cwp->w_lcol,cwp->curcol,total_columns,cwp->w_ntcols);
	};
	// MESG("1");
	if(cwp->curcol < cwp->w_lcol) {
//		ERROR("left column is %lld < curcol=%lld plcol=%lld",cwp->w_lcol,cwp->curcol,cwp->w_plcol);
		cwp->w_lcol=cwp->curcol;
	};
	if(cwp->w_lcol%tabsize !=0) 
	{
		mo=cwp->w_lcol%tabsize;
		cwp->w_lcol-=mo;
	};
	// MESG("2");
	if(cwp->w_lcol<0) {
//		ERROR("left column is %lld<0, curcol=%lld",cwp->w_lcol,cwp->curcol);
		cwp->w_lcol=0;
	};
	// MESG("3");
	/* if a new left column then force update! */
	if(cwp->w_lcol != cwp->w_plcol) {
		cwp->w_flag |= UPD_FULL|UPD_EDIT;
		cwp->w_plcol = cwp->w_lcol;
	};
	// MESG("upd_column_pos:	w_lcol=%d",cwp->w_lcol);
}

/* get the current line number  */
num getcline()	
{
	if(cwp->w_fp->b_flag & FSNLIST) return cwp->current_note_line;
	return (cwp->tp_current->line);
}

num getccolumn()
{
	return (cwp->tp_current->col);
}

/* update all the status, menu lines */
void update_status()
{
	WINDP *wp;
	lbegin(window_list);
	while((wp=(WINDP *)lget(window_list))!=NULL)
	{
		wp->w_flag |= UPD_STATUS;
	}
}

#if	!USE_GLIB
char *lstr_to_upper(char *s)
{
 char *us = strdup(s);
 char *us0=&us[0];
 while(*us0) *us0++=toupper((unsigned char)*s++);
 return us;
}

char *lstr_to_lower(char *s)
{
 char *us = strdup(s);
 char *us0=&us[0];
 while(*us0) *us0++=tolower((unsigned char)*s++);
 return us;
}
#endif

/*
 * Update a single virtual line in a window to the physical one. 
 */
int draw_window_line(WINDP *wp, int row)
{
 int i=0,i1,i2,i3;
 unsigned char c;
 int l;
 int bcol;
 extern char patmatch[];
 extern char search_pattern[];
 extern char search_lowercase_pattern[];
 char *match;
 char *lower_match;
 VIDEO *vp1 = wp->vs[row];

 // MESG(">draw_window_line: row=%d",row);
 if(gmode_reg_exp) {
#if	USE_GLIB
	if(gmode_exact_case) match=strdup(patmatch);
 	else match = g_utf8_strup(patmatch,-1);
	lower_match = g_utf8_strdown(match,-1);
#else
	if(gmode_exact_case) match=strdup(patmatch);
 	else match = lstr_to_upper(patmatch);
	lower_match = lstr_to_lower(match);
#endif
 } else {
 	match=search_pattern;
 	lower_match = search_lowercase_pattern;
 }
 l=strlen(match);
//  vp1 = wp->vs[row];
// MESG("draw_window_line: %d match=[%s]",row,match);
 /* highlight search pattern  */
 if(l) {
  i3=0;
  for(i=0,i1=0;i< (wp->w_ntcols);i++) {
    bcol = vp1->v_text[i].bcolor;
    if(bcol==COLOR_SELECT_BG) continue;	/* not if selection is on! */
	if(vp1->v_text[i].uval[0]==0xFF) {
		continue;
	};
	for(i2=0;i2<4;i2++) {
		int first_match=0;
		c = vp1->v_text[i].uval[i2];
		if(c==0) break;
		if(gmode_reg_exp) { if(eq1(c,match[i1],lower_match[i1])) first_match=1;}
		else {
			first_match=eq1(c,match[i1],lower_match[i1]);
		};
		if(first_match)
		{
			i1++;
			if(i2==0) i3++;
			if(i1==l ) {
				int j1;
				int j;
				for(j=0,j1=i;j<i3;j++,j1--){
					if((vp1->v_text+j1)->uval[0]==0xFF) {
						j1--;
					};
					svcolor(vp1->v_text+j1,COLOR_SEARCH_BG,COLOR_FG);
				};
				i1=0;	/* start from the begining for next match  */
			};
		} else {i1=0;i3=0;break;};
	};
  };
 };
#if	USE_GLIB
 if(gmode_reg_exp) { g_free(lower_match);g_free(match);};
#else
 if(gmode_reg_exp) { free(lower_match);free(match);};
#endif
 put_wtext(wp,row,wp->w_ntcols);
 vp1->v_flag =0;
 // MESG("ok!");
 return(TRUE);
}

/* update virtual window to physical */
void draw_window(int flag, WINDP *wp,char *from)
{
 // static int ind=0;
 register int i;
 int ulines=0;
	// MESG(" draw_window: id=%d from=%s ind=%d",wp->id,from,ind);
	drv_start_window_update(wp);
 	prepare_converter(wp->w_fp->b_lang);
	set_window_font(wp);

	// MESG("draw window lines");
#if	GTK3
	for (i = 0; i < wp->w_ntrows-2+half_last_line; i++) 
#else
	for (i = 0; i < wp->w_ntrows-1+half_last_line; i++) 
#endif
	{
		/* for each line that needs to be updated*/
		if ((wp->vs[i]->v_flag) ) 
		{
			if(wp->vs[i]->slow_line==1) wp->vs[i]->slow_line++;
			else 
			if(wp->vs[i]->slow_line==2) 
			{
				drv_clear_line(wp,i);
				wp->vtcol=0;
				wp->vs[i]->slow_line=0;
			};
			draw_window_line(wp,i);
			ulines++;
		}
	};
	// MESG("# -- draw_window:[%s] %d drawned lines %d flag=%X wflag=%X ind=%d",from,wp->id,ulines,flag,wp->w_flag,ind++);
	wp->draw_flag=0;
	expose_window(wp); /* (use this once unstead to expose every line sepatately, is it faster??) */
	if(flag /* && (wp->w_flag != UPD_STATUS) */) 
	{
		show_slide(wp);
	};
	if(wp->w_flag & UPD_STATUS) {
		status_line(wp);
	};

	wp->draw_flag=0;
	// MESG(" draw_window: end");
}

void set_draw_flag(WINDP *wp,char *from)
{
	wp->draw_flag=1;
	// MESG("set_draw_flag: window %d from %s",wp->id,from);
}

void update_physical_windows()
{
	WINDP *wp;

	// MESG("update_physical_windows:-----");
	/* for every window */
	lbegin(window_list);
	while((wp=lget(window_list))!=NULL)
	{
		// MESG("update_physical_windows: wid=%d",wp->id);
		if(wp->draw_flag){
			draw_window(1,wp,"update_physical_windows");
		};
	};
	// drv_post_windows_update();
	// MESG("update_physical_windows: end");
}

// check of numerics in line mask from start, set to type if ok
int checknumerics(FILEBUF *fp,char *line, int *start, int type)
{
 int hex=0;
 int firstzero=0;
 int isnum=1;
 int i0=*start;
 int count=0;
 int c1;
	while((c1=line[i0+count])!=0) {
		if(hex) {
			if((c1 >='0' && c1<='9') || c1=='.' ||(c1>='A'&&c1<='F')||(c1>='a'&&c1<='f')) {
				count++;
			} else {
				if(fp->hl->c_inword(c1)) isnum=0;
				break;
			};				
		} else {
			if((c1 >='0' && c1<='9') || c1=='.') {
				if(c1=='0' && count==0) firstzero=1;
				count++;
			} else {
				if(firstzero && (c1=='x'||c1=='X')) {
					count++;
					hex=1;
				} else if(firstzero && c1=='o') {
					count++;
				}  
				
				else {
					if(fp->hl->c_inword(c1)) isnum=0;
					break;
				};		 							
			}
		};
	};
	if(isnum) {
		int j;
		for(j=i0;j<i0+count;j++) line[j]=type;
	}
	i0+=count;
	if(count) i0--;

	*start=i0;
	return(count);
}

// check of words in line mask from start, set to type if ok
int checkwords(FILEBUF *fp,char *line, int *start, char **words,int type)
{
 int j1;
 int i2;
 int i1;
 int c; 
 int i0=*start;
 int wordfound=0;
// MESG("checkwords: type=%d []",type,words[0]);	
	for(j1=0;words[j1]!=NULL && words[j1][0];j1++) {
		
	 	i2=i0;
		c=line[i2];
		i1=0;
		if(fp->hl->ignorecase && (c >='a' && c <= 'z') ) c -= 32;  
	 	while (c==words[j1][i1]) {
			i1++;
			i2++;
			if(words[j1][i1]==0) break;
			c=line[i2];
			if(fp->hl->ignorecase && (c >='a' && c <= 'z') ) c -= 32;  
		};
		if((unsigned int)(i2-i0) ==  strlen(words[j1]) ) {
			c=line[i2];
			if(fp->hl->ignorecase && (c >='a' && c <= 'z') ) c -= 32;  
			if(!fp->hl->c_inword(c))
			{ /* if the following char is not a valid for words */
				i0=i2-1;
				int j;
				for(j=i0-i1+1;j<=i0;j++) line[j]=type;
				wordfound=1;
	 			i0++;
			};
 			break;	
		}
	};
	if(wordfound) i0--;
	*start=i0;
	return(wordfound);
}

num utf_FLineLen(FILEBUF *fp, offs ptr);

/*
	put a string on a virtual screen at row,start_col,with max_size
*/
void vt_str(WINDP *wp,char *str,int row,int index,int start_col,int max_size,int selected)
{
 int first_column;	// first column to show
 int c=0;
 utfchar uc;
 int i=0;
 num col;
 num llen=0;	/* Number of characters in the line  */
 vchar *v_text;
 int i0;
 char *header = str;
 int header_size=strlen(header);
 int ptr1=0;
 char *info_mask="           ";
 static char *vtlm=NULL; // virtual line character mask
 static num  vtla=0; 	 // mask allocated bytes
 int rlen=0;	// real line len
 int num_columns=0;	/* columns of line number shown  */
 int last_column=wp->w_ntcols;

 line_bcolor=wp->w_bcolor;

 if(max_size>0) last_column=start_col+max_size;
 if(last_column>wp->w_ntcols) last_column=wp->w_ntcols;

 vtmove(wp,row,start_col);
 if(row<5 && start_col==0){
	 // MESG("vt_str: at row=%2d index=%2d col=%2d selected=%d [%s]",row,index,start_col,selected,str);
	 // MESG("vt_str: at row=%2d col=%2d",wp->vtrow,start_col);
 };
 wp->vs[0]->v_flag=1;
 v_text = wp->vs[wp->vtrow]->v_text;

 int fg_color=COLOR_FG;
 int bg_color=COLOR_BG;
 if(wp->w_fp->b_flag & FSDIRED) {
	if(str[1]=='l') fg_color=COLOR_SQUOTE_FG;	// TAGFORE;
	else {
		if(str[0]=='#') fg_color=COLOR_PREP_FG;	/* H_QUOTE6  */
		else if(str[0]=='c' ) fg_color=COLOR_WORD1_FG;	/* H_QUOTE7  */
		else if(str[0]=='b') fg_color=COLOR_WORD2_FG;	/* H_QUOTE4  */
	};
 };

 first_column = wp->w_lcol;	/* first column to show at the beginning of the line */
 if(max_size>0 && start_col+max_size<first_column) return;	/* nothing to show here!  */
	col = start_col;

	hquotem &= ~H_UTFERR; 
	llen = start_col+utf_num_chars(header);
	
//	Mask creation (vtlm)
	if(llen>=vtla) { // increase allocation as needed
		vtla=llen+128;
		if(vtlm!=NULL) { efree(vtlm,"free old vtlm"); vtlm=NULL;};
		vtlm=emalloc(vtla,"increase vtlm allocation");
	};

//	create mask for the whole line without tabs of special characters
	int p=ptr1;
	for(i=start_col;i<llen;i++) {
		if(p>header_size) break;
		p = SUtfCharAt(header,p,&uc);
		c=uc.uval[0];
		if(c>127) {
			int size;
			size=get_utf_length(&uc);
			if((c==0xE0 /* && uc.uval[1]>=0xB0 */) || c==0xF0) { 	/* slow down for thai chars  */
				wp->vs[row]->slow_line=1;
#if	USE_SLOW_DISPLAY
				wp->w_fp->slow_display=1;
#endif
				// MESG("set slow display");
			}
			col += size-1;
			c='C';
		};
		++col;

		while(rlen<col){
			if(rlen>vtla) { vtla+=256; vtlm=realloc((void *)vtlm,vtla);};
			vtlm[rlen++]=c;
		};
	};
	vtlm[rlen]=0;

//	find the offset of the first column
	col=start_col;
	for (i=start_col;  col<first_column  &&  i < llen; ++i) { // invisible characters before first column shown
		if(ptr1>=header_size) break;
		ptr1 = SUtfCharAt(header,ptr1,&uc);
		c=uc.uval[0];
		if(c>127) {
			int size;
			size=get_utf_length(&uc);
			col += size-1;
			c='m';
		};
        ++col;
	};
	if(i>llen)	{ // all chars before left limit
		vteeol(wp,0,0);
		return;
	};

//	character creation

	wp->vtcol=start_col;

	if(num_columns>2){
		for(i0=0;i0<num_columns;i0++){	/* put the info columns  */
			vtputc(wp,info_mask[i0]);
		};
	} else {
		for(i0=0;i0<num_columns;i0++){	/* put the info columns  */
			vtputc(wp,CHR_SPACE);
		};
	};

	// MESG("vt_str: num_columns=%d vtcol=%d w_infocol=%d",num_columns,wp->vtcol,wp->w_infocol);
	for (; i <=  llen && wp->vtcol < wp->w_ntcols; i++) 
	{	// this is the on screen shown area of the line
		int display_size=0;
		if(ptr1>=header_size) break;
		memset(uc.uval,0,8);
		// show selection

		num char_bytes=ptr1;
		ptr1 = SUtfCharAt(header,ptr1,&uc);
		char_bytes = ptr1-char_bytes;
		display_size=get_utf_length(&uc);
		// display_size=SUtfCharLen(header,ptr1,&uc);
		c=uc.uval[0];
#if USE_GLIB	// Convert to composed character if possible to view it!
		// if(uc.uval[2]==0xCC || uc.uval[2]==0xCD || ((uc.uval[1]==0xCC||uc.uval[1]==0xCD))) 
		if(uc.uval[3]!=0)
		{
			char *composed = g_utf8_normalize((char *)uc.uval,-1,G_NORMALIZE_ALL_COMPOSE);
//				MESG("[%s] -> [%s] display_size=%d bytes=%d",uc.uval,composed,display_size,char_bytes);
			if(strlen((char *)uc.uval)>strlen(composed)) {
				strncpy((char *)uc.uval,composed,sizeof(uc.uval));
			} else {
//					MESG("compose normalization failed [%s][%s]",uc.uval,composed);
				// put a similar character without accent!
				uc.uval[utf8_countbytes(uc.uval[0])]=0;
			};
		};
#endif
		if(wp->vtcol==wp->w_ntcols-1 && display_size>1) { // do not show last double width character!
			memset(uc.uval,0,8);
			uc.uval[0]=' ';
			uc.uval[1]=0;
			vtputwc(wp,&uc);
		} else 
		{
			vtputwc(wp,&uc);
			if(display_size>1){ 	/* put a dummy char to skip  */
				while(display_size>1)	// this may be unecessary!
				{
					memset(uc.uval,0,8);
					uc.uval[0]=0xFF;
					uc.uval[1]=0xFF;
					vtputwc(wp,&uc);
					display_size--;
				};		
			};
		}
	};
	/* highlight according to evaluated mask */
	int start_color_column=num_columns;
	int end_column=wp->w_ntcols;
	
	if(max_size<0) start_color_column=start_col+num_columns;
	else end_column=max_size+num_columns;
	if(row>0) {	/* not for header!  */
		if(index==row) {
			// bg_color=COLOR_SELECT_BG;
			bg_color=COLOR_SELECT_BG;	/* code  */
			if(drv_colors==8) 
				fg_color=COLOR_FG;
			else if(selected>0) fg_color=COLOR_STANDOUT_FG;
#if	UNDERLINE_CURRENT_DIR_LINE
			if(wp==cwp) fg_color |= FONT_STYLE_UNDERLINE;
#endif
		} else {
			if(selected>0) {
				if(drv_colors==8) bg_color=COLOR_SELECT_BG;
				else bg_color=COLOR_INACTIVE_BG;
			} else {
				bg_color=COLOR_BG;
			};
		};
#if	TNOTES
		if(!(wp->w_fp->b_flag & FSNLIST)) fg_color=COLOR_STANDOUT_FG;
		if(start_col!=0) {
			if(wp->w_fp->b_flag==FSNOTES) {
				fg_color=COLOR_FG;
			};
		} else {
			if(wp->w_fp->b_flag!=FSNOTES && !(wp->w_fp->b_flag & FSNLIST)) 
				fg_color=COLOR_FG;
		};
#endif
		for(i0=start_color_column;i0<= end_column;i0++) svcolor(v_text+i0,bg_color,fg_color);
	} else 
	{
		if(drv_colors==8) { bg_color=COLOR_SELECT_BG;fg_color=COLOR_FG+FONT_STYLE_UNDERLINE;}
		else { bg_color=COLOR_CODE_BG;fg_color=COLOR_FG|FONT_STYLE_UNDERLINE;};
		line_bcolor=bg_color;
		for(i0=start_color_column;i0<= end_column;i0++)
		{
			svcolor(v_text+i0,bg_color,fg_color);
		};
		vteeol(wp,2,1);
		return;
	};
	// MESG("vt_str: end max_size=%d",max_size);
	if(max_size<0) {
		if(index==row) vteeol(wp,1,0);	// current line
		else if(selected>0) vteeol(wp,3,0);	// selected
		else vteeol(wp,0,0);
	} else vteeoc(wp,max_size);
}

#if	NUSE
// use this instead of realloc to debug
char * resize_buffer(char *buffer,num old_size,num new_size)
{
 // MESG("resize_buffer: old=%ld new=%ld",old_size,new_size);
 char *new_allocation=emalloc(new_size,"vtlm new allocation2");
 memcpy(new_allocation,buffer,old_size);
 if(buffer!=NULL) efree(buffer,"resize, free old buffer");
 return new_allocation;
}
#endif

int line_sep=0;
vchar *init_vt_line(WINDP *wp)
{
 vchar *v_text = wp->vs[wp->vtrow]->v_text;
 int i;
 for(i=0;i<wp->w_width;i++) {
 	v_text[i].uval[0]='A';
	v_text[i].uval[0]=0;
 };
 memset(v_text,0,sizeof(struct vchar)*wp->w_ntcols);
 return v_text;
}

//	Mask prepare (vtlm)
char *vt_mask_init(num llen)
{
 static num vtla=0;
 static char *vtlm=NULL;
	if(vtla==0) { // initial mask allocation
		vtla=256;
		// MESG("vt_mask_init:initial alloc of 128");
		vtlm=emalloc(vtla,"initial allocation vtlm");
	};
	if(llen>=vtla) {
		vtla=llen+256;
		// MESG("	vt_mask_init: realloc new with %ld",vtla);
		vtlm=realloc(vtlm,vtla);
	}
 return vtlm; 
}

int color_mask_create(WINDP *wp,offs start,int llen,char *vtlm,int vtla)
{
 FILEBUF *fp = wp->w_fp;
 offs p=start;
 int i;
 int col=0;
 int c=0;
 utfchar uc;
 int c1,i0;
 int rlen=0;
		for(i=0;i<llen;i++) {
			if(fp->b_lang == 0 && !utf8_error()) {
				p = FUtfCharAt(fp,p,&uc);
				c=uc.uval[0];
				if(c>127) {
					int size;
					size=get_utf_length(&uc);
					col += size-1;
					c='C';
				};
			} else {
				c = FCharAt(fp,p++);
			};
        	if (c == '\t' ) {
				col = next_tab(col);
				c=CHR_SPACE;
			} else {
				++col;
			};
			if(c<32) c='C';

			if(col>vtla) vtlm=vt_mask_init(vtla+col);
			while(rlen<col){
				vtlm[rlen++]=c;
			};

		};
		vtlm[rlen]=0;
		// if(col>=vtla) MESG("vtlm[%d]=[%s] col=%d vtla=%d",wp->vtrow,vtlm,col,vtla);

		int canstart=1;
		for(i0=0 ;i0< rlen;i0++) {
			// Check for boundary characters
				c1 = vtlm[i0];
				if(!fp->hl->c_inword(c1))
				{ 
					canstart=1;
					continue;
				}
			if(canstart) {
			// MESG("		421 i0=%d",i0);
			// Highlight numerics, set type to H_NUMERIC
				if(!checknumerics(fp,vtlm,&i0,COLOR_STANDOUT_FG))
			// check for words of type 1, set type to H_WORD1
				if(!checkwords(fp,vtlm,&i0,fp->hl->w0,COLOR_WORD1_FG))
			// check for words of type 2, set type to H_WORD2
				checkwords(fp,vtlm,&i0,fp->hl->w1,COLOR_WORD2_FG);
				canstart=0;
				// MESG("	422 i0=%d",i0);
			}
 		};

	// MESG("[%2d %s]",wp->vtrow,vtlm);
	return rlen;
}

int vt_hex_line(WINDP *wp,char *vtlm, int llen, num ptr1)
{
		char hs[4];
		int j,j1;
		int i=0;
		int c;
		num sptr=ptr1;
		FILEBUF *fp=wp->w_fp;

		if(llen>=0) {
		sprintf(vtlm,"%08llX: ",ptr1);
		i=10;
		for(j=0;j<=15;j++) {
			if(j<=llen) {
			c=FCharAt(fp,sptr++);
			sprintf(hs,"%02X ",c);
			} else strlcpy(hs,"-- ",4);
			for(j1=0;j1<strlen(hs);) {
				vtlm[i++]=hs[j1++];
			};
		};
		vtlm[i++]='|';
		vtlm[i++]=' ';
		sptr=ptr1;
		j1=i;

		for(j=0;j<=llen;j++) {
			c=FCharAt(fp,sptr++);
			vtlm[i++]=c;
		};
		} else i=0;
		vtlm[i]=0;
		return i;
} 

char *set_info_mask(WINDP *wp,num ptr1,num line_num)
{
 static char info_mask[20];
	if(wp->w_fp->view_mode == VMLINES) {
		snprintf(info_mask,11,"%07llu ",line_num+1);
	} else {
		if(wp->w_fp->view_mode == VMOFFSET) {
			snprintf(info_mask,11,"%07llX ",ptr1);
		} else {
			snprintf(info_mask,11,"==");
		}
	};
 return info_mask;
}

int draw_info_mask(WINDP *wp,num ptr1,char *info_mask)
{
 int i0;
 int num_columns=wp->w_infocol;
	wp->vtcol=0;
	if(num_columns>2){
		for(i0=0;i0<num_columns;i0++){	/* put the info columns  */
			vtputc(wp,info_mask[i0]);
		};
	} else {
		for(i0=0;i0<num_columns;i0++){	/* put the info columns  */
			if(BolAt(ptr1)) vtputc(wp,'*');	// vtputc(wp,CHR_SPACE);
			else vtputc(wp,'-');
		};
	};

 return i0;
}


/* update line starting at offset start of specific window */
offs vtline(WINDP *wp, offs tp_offs)
{
 int first_column;	// first column to show
 int c=0;
 utfchar uc;

 int i=0;
 num col;
 num llen=0;	/* Number of characters in the line (utf or sible byte)  */
 vchar *v_text;
 int i0;
 FILEBUF *fp = wp->w_fp;; 
 int c1;
 int fcol=COLOR_FG;
 int bcol=COLOR_BG;
 offs s1,s2,ptr1,ptr2;
 offs cur_lend=0;
 int hexmode = wp->w_fp->view_mode & (VMHEX||VMINP);
 char *info_mask;
 char *vtlm=NULL;
 static num  vtla=0; 	 // mask allocated bytes
 int rlen=0;	// real line len
 num line_num = wp->tp_hline->line + wp->vtrow;

 v_text = init_vt_line(wp);

 first_column = wp->w_lcol;	/* first column to show at the beginning of the line */
 
	col = 0;
	s1=tp_offset(wp->w_smark);
	s2=tp_offset(wp->w_emark);

	ptr1 = FLineBegin(fp,tp_offs);
	cur_lend = FLineEnd(fp,ptr1);
	ptr2=cur_lend;
	set_utf8_error(0);	/* reset utf_error on each line!!  */
	if((wp->w_fp->b_lang == 0 
	   ) && (!hexmode)) {
		llen = utf_FLineLen(wp->w_fp,ptr1);
		if(utf8_error()) llen=ptr2-ptr1;
	} else {
		llen = ptr2 - ptr1;
	};

	init_line_highlight(wp);
	
 	if(!slang || hquotem==0) 
		line_bcolor=wp->w_bcolor;

	if(utf8_error()) {
		hquotem |= H_UTFERR;
		llen = ptr2 - ptr1;
	} else {
		hquotem &= ~H_UTFERR; 
	};

	if(s2<s1) { 	/* reverse it  */
		ptr2=s1;
		s1=s2;s2=ptr2;
	};
	if(wp->selection==REGION_COLM) {
		ptr2=ptr1;
		col0=FColumn(wp->w_fp,tp_offset(wp->w_smark)-wp->w_lcol);
		col1=FColumn(wp->w_fp,tp_offset(wp->w_emark)-wp->w_lcol);
		if(col0>col1) {
			i0=col0;
			col0=col1;col1=i0;
		};
	};

	vtlm=vt_mask_init(llen);

	if(hexmode) {
		vtla=vt_hex_line(wp,vtlm,llen,ptr1);
	} else
	{
		if(llen>=0 && wp->w_infocol) 
		{
			info_mask=set_info_mask(wp,ptr1,line_num);
		};
		line_sep=0;
		if(syntaxh) 
		{

//	create mask for the whole line without tabs or special characters
#if	1
	rlen=color_mask_create(wp,ptr1,llen,vtlm,vtla);
#else
		offs p=ptr1;
		
		// col=0;
		for(i=0;i<llen;i++) {
			if(fp->b_lang == 0 && !utf8_error()) {
				p = FUtfCharAt(fp,p,&uc);
				c=uc.uval[0];
				if(c>127) {
					int size;
					size=get_utf_length(&uc);
					col += size-1;
					c='C';
				};
			} else {
				c = FCharAt(fp,p++);
			};
        	if (c == '\t' ) {
				col = next_tab(col);
				c=CHR_SPACE;
			} else {
				++col;
			};
			if(c<32) c='C';

			if(col>vtla) vtlm=vt_mask_init(vtla+col);
			while(rlen<col){
				vtlm[rlen++]=c;
			};

		};
		vtlm[rlen]=0;
		// if(col>=vtla) MESG("vtlm[%d]=[%s] col=%d vtla=%d",wp->vtrow,vtlm,col,vtla);

		int canstart=1;
		for(i0=0 ;i0< rlen;i0++) {
			// Check for boundary characters
				c1 = vtlm[i0];
				if(!fp->hl->c_inword(c1))
				{ 
					canstart=1;
					continue;
				}
			if(canstart) {
			// MESG("		421 i0=%d",i0);
			// Highlight numerics, set type to H_NUMERIC
				if(!checknumerics(fp,vtlm,&i0,COLOR_STANDOUT_FG))
			// check for words of type 1, set type to H_WORD1
				if(!checkwords(fp,vtlm,&i0,fp->hl->w0,COLOR_WORD1_FG))
			// check for words of type 2, set type to H_WORD2
				checkwords(fp,vtlm,&i0,fp->hl->w1,COLOR_WORD2_FG);
				canstart=0;
				// MESG("	422 i0=%d",i0);
			}
 		};
#endif
 		};	// syntaxh
	};
	
//	find the offset of the first column
	col=0;	
	if(hexmode) { col=first_column;}
	else
	for (i=0;  col<first_column  &&  i < llen; ++i) { // invisible characters before first column shown
		if(fp->b_lang == 0 && !utf8_error()) {
			ptr1 = FUtfCharAt(fp,ptr1,&uc);
			c=uc.uval[0];
			
			if(c>127) {
				int size;
				size=get_utf_length(&uc);
				if(c==0xE0 /* &&  uc.uval[1]>=0xB0 */ )  {
					wp->vs[wp->vtrow]->slow_line=1;
#if	USE_SLOW_DISPLAY
					fp->slow_display=1; /* slow down for thai chars  */
#endif
				};
				col += size-1;
				c='m';
			};
		} else {
			c = FCharAt(fp,ptr1++);
		}
        if (c == '\t' ) col = next_tab(col);
        else ++col;
		if(syntaxh) {
			prev_hquotem=hquotem;
			wp->w_fp->hl->h_function(c); 
		};
	};
	int leave_space=col-first_column;
	
	if(i>llen && !hexmode)	{ // all chars before left limit
		return cur_lend+wp->w_fp->EolSize;
	};
// ----------------------------------------------
//	character creation
	if(hexmode) 
	{
		num sptr=ptr1;
		ptr1=sptr;
		wp->vtcol=0;

		for (i=first_column; i < vtla && wp->vtcol< wp->w_ntcols; i++) 
		{
			if(i>HSTART) {
				if(wp->selection) {
					if(ptr1>=s1 && ptr1 < s2) {
						set_selection(true);
					} else {
						set_selection(false);
					};
				};
					vtputc(wp,vtlm[i]);
					ptr1++;
			} else {
				vtputc(wp,vtlm[i]);
			};
		}
	} else { // not hex mode

	wp->vtcol=0;

	i0=draw_info_mask(wp,ptr1,info_mask);

	if(leave_space==1) {
		memset(uc.uval,0,8);
		uc.uval[0]='~';
		vtputwc(wp,&uc);
	};
	for (; i <  llen && wp->vtcol < wp->w_ntcols; i++) 
	{	// this is the on screen shown area of the line
		int display_size=0;
		memset(uc.uval,0,8);
		// show selection
		if(wp->selection) {
			if(ptr1>=s1 && ptr1 < s2) 
			{
				set_selection(true);
			} else {
				set_selection(false);
			};
			if(wp->selection==REGION_COLM){
				if(s1<cur_lend && s2>tp_offs) set_selection(true);
				if(((wp->vtcol-wp->w_infocol) < col0) || ((wp->vtcol - wp->w_infocol) >= col1)) set_selection(false);
			};
		};

		if(fp->b_lang == 0 && !utf8_error()) {
			num char_bytes=ptr1;
			ptr1 = FUtfCharAt(fp,ptr1,&uc);
			char_bytes = ptr1-char_bytes;
			display_size=get_utf_length(&uc);
			c=uc.uval[0];
			if(c==0xE0 /* && uc.uval[1]>=0xB0 */){
				wp->vs[wp->vtrow]->slow_line=1;
#if	USE_SLOW_DISPLAY
				fp->slow_display=1;	/* slow down for thai chars  */
#endif
			};
#if USE_GLIB	// Convert to composed character if possible to view it!
			// if(uc.uval[2]==0xCC || uc.uval[2]==0xCD || ((uc.uval[1]==0xCC||uc.uval[1]==0xCD))) 
			if(uc.uval[3]!=0)
			{
				char *composed = g_utf8_normalize((char *)uc.uval,-1,G_NORMALIZE_ALL_COMPOSE);
//				MESG("[%s] -> [%s] display_size=%d bytes=%d",uc.uval,composed,display_size,char_bytes);
				if(strlen((char *)uc.uval)>strlen(composed)) {
					strncpy((char *)uc.uval,composed,sizeof(uc.uval));
				} else {
//					MESG("compose normalization failed [%s][%s]",uc.uval,composed);
					// put a similar character without accent!
					uc.uval[utf8_countbytes(uc.uval[0])]=0;
				};
			} else {
//				if(uc.uval[2]>0)
//				MESG("-- %s   ] display_size=%d",uc.uval,display_size);
			};
#endif
			if(wp->vtcol==wp->w_ntcols-1 && display_size>1) { // do not show last double width character!
				memset(uc.uval,0,8);
				uc.uval[0]=' ';
				uc.uval[1]=0;
				vtputwc(wp,&uc);
			} else 
			{
				vtputwc(wp,&uc);
//				MESG("	col %d put: size=%d %X",wp->vtcol,display_size,uc.uval[0]);
				if(display_size>1){ 	/* put a dummy char to skip  */
					while(display_size>1)	// this may be unecessary!
					{
						memset(uc.uval,0,8);
						uc.uval[0]=0xFF;
						uc.uval[1]=0xFF;
						vtputwc(wp,&uc);
						display_size--;
					};		
				};
			}
		} else {
			vtputc(wp, FCharAt(fp,ptr1++));
		}
	};
	// show a line separator (when highlight_md)
	if(line_sep) {
		VIDEO *vp=wp->vs[wp->vtrow];
		for(wp->vtcol=wp->w_infocol;wp->vtcol< wp->w_width;wp->vtcol++){ 
			vchar *vc = vp->v_text+wp->vtcol;

			svwchar(vc,&double_hline,vc->bcolor,COLOR_COMMENT_FG);	/* double line separator */
		};
	};

	/* highlight according to evaluated mask */
	if(syntaxh && slang)
	{
		for(i0=wp->w_infocol;i0< wp->w_ntcols;i0++){
			if(i0+first_column > rlen+wp->w_infocol) break;
			c1=vtlm[i0+first_column-wp->w_infocol];

			fcol = v_text[i0].fcolor;
			bcol = v_text[i0].bcolor;

			if(bcol!=COLOR_SELECT_BG)	
			if(bcol!=COLOR_BG || fcol!=COLOR_FG) {continue;};

			if(i0>stop_word_highlight) { continue;};	// in yaml only!
			if(bcol==COLOR_BG && fcol!=COLOR_COMMENT_FG && fcol!=COLOR_PREP_FG)
			{
				if(c1==COLOR_WORD1_FG) svcolor(v_text+i0,bcol,COLOR_WORD1_FG);
				else if(c1==COLOR_WORD2_FG) svcolor(v_text+i0,bcol,COLOR_WORD2_FG);
				else if(c1==COLOR_STANDOUT_FG) {
					svcolor(v_text+i0,bcol,COLOR_STANDOUT_FG);
				} else svcolor(v_text+i0,bcol,fcol);
			} 
		};
	};

	// in case of utf error show local chars with different color!
	// if(slang)
	if(fp->b_lang==0){
		for(i0=0;i0<wp->w_ntcols;i0++) {
			if(v_text[i0].uval[0]>128) 
			{
				if(v_text[i0].uval[1]==0) {
					 svcolor(v_text+i0,v_text[i0].bcolor,COLOR_WORD2_FG);
				};
			};
		}
	};
	/* find highlight till the end of the line outside window .. */
	if(syntaxh) for (; i<llen ; i++) {
		ptr1 = FUtfCharAt(fp,ptr1,&uc);
		c=uc.uval[0];
		if(c>127) {
				c='m';
		};
		prev_hquotem=hquotem;
		wp->w_fp->hl->h_function(c);
	};
	};
	// MESG("end line: >");
	return cur_lend+wp->w_fp->EolSize;
}

#include "wrap_line.h"

/* this is called from vtline, vt_str, vtputwc ! */
void vtputc(WINDP *wp, unsigned int c)
{
	utfchar uc;
	memset(uc.uval,0,8);
	if(c==0) c='.';
	uc.uval[0]=c;
	vtputwc(wp,&uc);
}

void vtputwc(WINDP *wp, utfchar *uc)
{
	FILEBUF *fp=wp->w_fp;
	unsigned int ctl_f=0,ctl_b=0;
	int i=0;
	unsigned int c=uc->uval[0];
	register VIDEO *vp;	/* ptr to line being updated */
	int start_column=0;
	vp = wp->vs[wp->vtrow];

	if(fp->view_mode & VMHEX) {
	 unsigned char c1=0;

	 for(i=0;i<1;)
	 {
	 	c=uc->uval[i];
		ctl_f=wp->w_fcolor;
		ctl_b=wp->w_bcolor;
		if(c<0x80) {
			c1=c;
			ctl_b=wp->w_bcolor; 
		} else { 
			ctl_b=COLOR_QUOTE_BG;
#if	1	/* show corresponding lower ascii character  */
			c1=c-0x80;
#else	/* show corresponding local codepage character  */
			c1=c;
#endif
		};
		if(c1<0x20) {
			ctl_f=COLOR_FG;
			c1+=0x40;
		};
		if(c1==0x7f) {
			ctl_f=COLOR_FG;
			c1='?';
		};

		if(get_selection()){
			ctl_f = COLOR_FG;
			ctl_b = COLOR_SELECT_BG;
		};
		svchar(vp->v_text+wp->vtcol,c1,ctl_b,ctl_f);
		wp->vtcol++;

	 	i++;
	 	if(i>3||uc->uval[i]==0) break;
	 }
	 return;
	} else {
		if(wp->w_fp->b_flag < FSNOTES)
			start_column=wp->w_infocol;
	};
	if(wp->vtcol < start_column-1){
		svchar(vp->v_text+wp->vtcol++,c,COLOR_INFO_BG,COLOR_FG);
		return;
	};

	if(wp->vtcol == start_column-1){
#if	DARWIN || PCURSES
		svchar(vp->v_text+wp->vtcol++,CHR_NBSPACE,COLOR_INFO_BG,COLOR_FG);
#else
		svchar(vp->v_text+wp->vtcol++,CHR_SPACE,COLOR_INFO_BG,COLOR_FG);
#endif
		return;
	};

	if(hquotem & H_LINESEP) line_sep=1;else line_sep=0;

	if(syntaxh) {
		prev_hquotem=hquotem;
		wp->w_fp->hl->h_function(c); 
	};
	if (c == '\t') {              
		do {
#if	DARWIN
			vtputc(wp,CHR_NBSPACE);
#else
			vtputc(wp,CHR_SPACE);
#endif
		} while (((wp->vtcol + wp->w_lcol - start_column) % tabsize) != 0);
	}  else 
	if (((c < 0x20 ) || c == 0x7F)) 
	{
	  if(specialh) {
		if (c==12 && wp->vtcol==start_column) { /* single line red */
			vt1eol(wp,'_',COLOR_CTRL_FG);	/* new page line  */
			return;
		};
	  };
	  /* other control characters */
		ctl_f=COLOR_CTRL_FG; ctl_b=COLOR_QUOTE_BG;

		unsigned char c1=c^0x40;
		svchar(vp->v_text+wp->vtcol++,c1,ctl_b,ctl_f);
		return;
	} else 

	if ( (c>0x80 && c<0xA0) && wp->w_fp->b_lang < 10   )
	{
	  /* other control characters */
		ctl_b=COLOR_CTRL_FG;
		ctl_f=COLOR_SELECT_BG;

		unsigned char c1=(c-128)^0x40;
		svchar(vp->v_text+wp->vtcol++,c1,ctl_b,ctl_f);
		return;	
	} else
	if (c==0x80) {
		ctl_b=COLOR_CTRL_FG; 
		ctl_f=COLOR_SELECT_BG;
		svchar(vp->v_text+wp->vtcol++,'@',ctl_b,ctl_f);
	} else
	{ /* all the rest characters */
		if(syntaxh) {
			switch(hquotem) {
			/* single quotes */
			case H_QUOTE1:
			case H_QUOTE7+H_QUOTE1:  
				ctl_f=COLOR_SQUOTE_FG;
				ctl_b=line_bcolor;
				break;
			/* double quotes */
			case H_QUOTE2:
			case H_QUOTE7+H_QUOTE2:
				if(c!=CHR_DQUOTE) 
				{ 
					ctl_f = COLOR_FG;
					ctl_b=COLOR_QUOTE_BG;
				};
				break;
			case H_QUOTE4:
//			case H_UTFERR:
				ctl_f = COLOR_HORIZON_FG;// TAGFORE ;
				ctl_b=line_bcolor;
				break;
			/* C,C++ Comments */
			case H_QUOTEC:
			case H_QUOTE5:
			case H_QUOTEC+H_QUOTE8:
			case H_QUOTEC+H_QUOTE7:
			case H_QUOTEC+H_QUOTE7+H_QUOTE1:
			case H_QUOTEC+H_QUOTE7+H_QUOTE2:
				ctl_f = COLOR_COMMENT_FG;
				ctl_b = line_bcolor;
				break;
			/* Preprocessing */
			case H_QUOTE6:
				ctl_f=COLOR_PREP_FG;
				ctl_b=line_bcolor;
				break;
			case H_QUOTE12:
				line_bcolor=COLOR_CODE_BG;	/* code  */
				ctl_b=line_bcolor;
				break;
			case H_QUOTE7:	/* next words in html tags  */
				ctl_f=COLOR_WORD3_FG;ctl_b=wp->w_bcolor;
				break;
			/* % tag */
			case H_QUOTE8:	/* first word after < in html  */
				ctl_f = COLOR_WORD1_FG; // TAGFORE;
				ctl_b=line_bcolor;
				break;
			case H_QUOTE9:
				ctl_f=COLOR_WORD2_FG;
				ctl_b=line_bcolor;
				break;
			case H_QUOTE10:
				line_bcolor=COLOR_INFO_BG;
				ctl_b=line_bcolor;
				ctl_f=wp->w_fcolor;
				break;			
			case H_QUOTE11:
				ctl_f=wp->w_fcolor;
			case H_QUOTE11+H_QUOTE9:
				ctl_b=COLOR_INACTIVE_BG;
				break;		
			case 0:
				ctl_b=line_bcolor=wp->w_bcolor;	
			};
		};
		if(get_selection()){
			ctl_f = COLOR_FG;
			ctl_b = COLOR_SELECT_BG;
		};

		if( hquotem==H_QUOTE7 && c=='=') {
			ctl_f=COLOR_STANDOUT_FG;
		};
		// highlight special chars in all highlight types
		if((ctl_f==0 && (ctl_b==0||ctl_b==line_bcolor)))
		switch(c) { 
			case 39: 	/* single quote  */
			case CHR_LBRA: case CHR_RBRA:
			case '(': case ')':
			case '{': case '}':
			case ';': case ':':
			case '*': case '=':
			case '+': case '-':
			case '&': case '%':
			// case CHR_DQUOTE:
			case '<': case '>': case ',':
				ctl_f = COLOR_SPEC_FG;ctl_b=line_bcolor;
			break;

			default:
				ctl_f=wp->w_fcolor;ctl_b=line_bcolor;
		};
		if(prev_hquotem!=hquotem && c==CHR_DQUOTE) {
			ctl_f = COLOR_HORIZON_FG;ctl_b=line_bcolor;
		};

		/* orizon different color creates problems if utf and local char set (utf string error)  */
		/* if on the orizon make it a different color */
		if(is_wrap_text(wp->w_fp)) {
			if (((wp->vtcol == wp->w_width+wp->w_infocol-1)) || (wp->vtcol==0 && wp->w_lcol > 0)) {
				ctl_f = COLOR_HORIZON_FG;
			};
		} else 
		if (((wp->vtcol == wp->w_ntcols-1)) || (wp->vtcol==0 && wp->w_lcol > 0)) {
			ctl_f = COLOR_HORIZON_FG;
		};

//		MESG("--> at col=%d [%s]",wp->vtcol,uc->uval);
		svwchar(vp->v_text+wp->vtcol,uc,ctl_b,ctl_f);
		wp->vtcol++;
	};
	return;
}

/*
 * Erase to end of screen line starting from current vt point!? 
 * show selection 
 */
void vteeol(WINDP *wp, int selected,int inside)
{
    VIDEO  *vp;
    int ctl_f,ctl_b;
	int blank;
    vp = wp->vs[wp->vtrow];	// vtrow
	// MESG("vteeol: row=%d selected=%d col=%d",wp->vtrow,selected,wp->vtcol);
    ctl_f=wp->w_fcolor;
	// if(wp->vtrow==0 && inside) ctl_f+=256;
	// ctl_b=wp->w_bcolor;
	ctl_b = line_bcolor;
	// if(wp->vtrow==0) ctl_b=INFOBACK;
#if	DARWIN
	blank=CHR_NBSPACE;	// use this for mac terminal!
#else
	blank=CHR_SPACE;
#endif
	if(inside && wp->selection==REGION_COLM) {
		if(wp->vtcol<col0) {
			 svmchar(vp->v_text+wp->vtcol,blank,ctl_b,ctl_f,col0 - wp->vtcol);
			 svmchar(vp->v_text+col0,blank,COLOR_SELECT_BG,wp->w_fcolor,col1-col0+1);
			 svmchar(vp->v_text+col1+1,blank,ctl_b,ctl_f,wp->w_ntcols-col1-1);
		} else if(wp->vtcol<=col1) {
			 svmchar(vp->v_text+wp->vtcol,blank,COLOR_SELECT_BG,wp->w_fcolor,col1-wp->vtcol+1);
			 svmchar(vp->v_text+col1+1,blank,ctl_b,ctl_f,wp->w_ntcols-col1-1);
		} else {
			svmchar(vp->v_text+wp->vtcol,blank,ctl_b,ctl_f,wp->w_ntcols-wp->vtcol);
		};
	} else {
			if(selected) {
				if(selected==2)       { 	/* top line headers  */
					if(drv_colors>8) { 
						ctl_b=COLOR_CODE_BG;ctl_f=COLOR_FG|FONT_STYLE_UNDERLINE;
					} else { ctl_b=COLOR_SELECT_BG;ctl_f=COLOR_FG|FONT_STYLE_UNDERLINE;};
				}	// header
				else if(selected==3)  { if(drv_colors>8)  ctl_b=COLOR_INACTIVE_BG;else ctl_b=COLOR_SELECT_BG;}	// just selected
				else if(selected==-1) { if(drv_colors==8) ctl_b=COLOR_SELECT_BG ;}	// empty
				else                  {
					ctl_b=COLOR_SELECT_BG;	// current line, code
#if	UNDERLINE_CURRENT_DIR_LINE
					if(wp==cwp) ctl_f |= FONT_STYLE_UNDERLINE;
#endif
				};
				svmchar(vp->v_text+wp->vtcol,blank,ctl_b,ctl_f,wp->w_ntcols-wp->vtcol);
			} else {
				// MESG("	from col=%d line_bcolor=%X %X width=%d",wp->vtcol,line_bcolor,ctl_b,wp->w_ntcols-wp->vtcol);
				svmchar(vp->v_text+wp->vtcol,blank,ctl_b,ctl_f,wp->w_ntcols-wp->vtcol);
			};
	}
	wp->vtcol=wp->w_ntcols;
}

	/* erase till end of column  */
void vteeoc(WINDP *wp, int endcol)
{
    VIDEO  *vp;
    int ctl_f,ctl_b;
	int blank;
    vp = wp->vs[wp->vtrow];	// vtrow

    ctl_f=wp->w_fcolor;
	ctl_b=line_bcolor;
	ctl_b=COLOR_QUOTE_BG;
#if	DARWIN
	blank=CHR_NBSPACE;	// use this for mac terminal!
#else
	blank=CHR_SPACE;
	blank='@';
#endif
	if(wp->vtcol>endcol){
		svmchar(vp->v_text+wp->vtcol,blank,ctl_b,ctl_f,endcol-wp->vtcol);
	}
	wp->vtcol=wp->w_ntcols;
}

/* fill the rest of the line with a char and foreground color 
	used mainly for separator lines
*/
void vt1eol(WINDP *wp,int c,int color)
{
	vchar *vpt;
    vpt = wp->vs[wp->vtrow]->v_text;
	svmchar(vpt+wp->vtcol,c,line_bcolor,color,wp->w_width-wp->vtcol);
	wp->vtcol=wp->w_width;
}

void updgar()
{
	int i;
	WINDP *wp;

	lbegin(window_list);
	while((wp=lget(window_list))!=NULL)
	{
		for (i = 0; i < wp->w_ntrows; i++) {
			wp->vs[i]->v_flag = 1;
		};
		window_clear(wp);	// clear to default color
		wp->w_flag = (UPD_FULL+UPD_EDIT);
	}
}

void update_buffer_windows(FILEBUF *bp,int flag)
{
 if(discmd) {
 WINDP *wp;

	lbegin(window_list);
	while((wp=lget(window_list))!=NULL){
		if(wp->w_fp==bp) {
			set_update(wp,flag);
		};
	};
 };
}

int check_cursor_position_notes(WINDP *wp)
{
	FILEBUF *fp = wp->w_fp;
	int head=(wp->w_fp->b_header!=NULL);
	
	if(fp->b_flag == FSNOTES) {
		if( wp->current_tag_line>=0 &&
			(wp->current_tag_line <= wp->top_tag_line))
		{
			// we are out of bounds. reposition!
			wp->w_flag = UPD_FULL;
			wp->top_tag_line=wp->current_tag_line;
		} else 
		// if (wp->current_tag_line-wp->w_ntrows-head-1 > wp->top_tag_line)
		if(wp->current_tag_line>=wp->top_tag_line+wp->w_ntrows-head-3)
		{
			// wp->current_tag_line>=wp->top_tag_line+wp->w_ntrows-head-1;
			wp->top_tag_line=wp->current_tag_line+3-wp->w_ntrows+head;
			// wp->top_tag_line=wp->current_tag_line-wp->w_ntrows-head-1;
			wp->w_flag = UPD_FULL;
		} else {
			// MESG("check_cursor_position_notes: %d < %d < %d tag no reposition!",wp->top_tag_line, wp->current_tag_line,wp->top_tag_line+wp->w_ntrows-head-1);
			return true;
		}
	} else 
	{
		if( wp->current_note_line>=0 &&
			(wp->current_note_line < wp->top_note_line))
		{
			wp->w_flag = UPD_FULL;
			wp->top_note_line=wp->current_note_line;
		} else 
		if(wp->current_note_line>wp->top_note_line+wp->w_ntrows-head-3)
		{
			wp->top_note_line=wp->current_note_line+3-wp->w_ntrows+head;
			wp->w_flag = UPD_FULL;
		} else {
			// MESG("check_cursor_position_notes: note no reposition!");
			return true;
		};
	};
	/* and reset the current line at top of window */
	wp->w_flag |= UPD_WINDOW;
	if(update_all) wp->w_flag |= UPD_FULL;
	return(TRUE);
}

offs update_top_position_wrap()
{

 int o_now=tp_offset(cwp->tp_current);
	offs o;
#if	1
	// MESG_time("update_top_position_wrap: %ld",o_now);
	o = FLineBegin(cwp->w_fp,o_now);
	// MESG_time("update_top_position_wrap: begin=%ld",o);
#else
	static offs known=0;
	if(known<o_now-cwp->w_width) o=known;
	else o=FLineBegin(o_now);
#endif
 	// MESG("update_top_position_wrap: o_now=%ld begin=%ld",o_now,o);
	int col=0;
	// offs new_hline=tp_offset(cwp->tp_hline);
	offs new_hline=o;
	// MESG("		: start evaluate new hline from %ld new current is %ld",new_hline,o_now);
	while(1){
		o=check_next_char(cwp->w_fp,o,&col);
		if(o>=o_now) { 
			// MESG(" 	up to %ld, hline=%ld",o_now,new_hline);
			break;
		}
		if((col % (cwp->w_width))==0) {
			new_hline=o;
			// MESG("		set new hline to %ld",new_hline);
			// if(o<o_now-1000) known=o;
		};
	};
	textpoint_set(cwp->tp_hline,new_hline);
	tp_copy(cwp->w_fp->save_hline,cwp->tp_hline);
	// FindLineCol(cwp->tp_hline);
	MESG_time("update_top_position_wrap end hline=%ld",tp_offset(cwp->tp_hline));
	return(tp_offset(cwp->tp_hline));
}

int check_cursor_position_wrap(WINDP *wp)
{
	offs cur_offs;
	// FILEBUF *fp = wp->w_fp;
	// int i;
	// MESG("check_cursor_position_wrap: hline o=%ld l=%ld, current o=%ld, l= %ld",tp_offset(wp->tp_hline),tp_line(wp->tp_hline),
		// tp_offset(wp->tp_current),tp_line(wp->tp_current));
	cur_offs=tp_offset(wp->tp_current);
	int wcl=window_cursor_line(wp);
	// if(is_wrap_text(wp->w_fp)) show_time("check_cursor_position 1",1);
	if( cur_offs <= tp_offset(wp->tp_hline)) {
		// MESG("	< ppline=%ld",wp->w_ppline);
		update_top_position_wrap();
		return FALSE;
	}  else if  (wcl > wp->w_ntrows-2
				&& wcl <= wp->w_ntrows
	) {
		// MESG("	> ppline=%ld currow=%d",wp->w_ppline,wp->currow);
		move_window(-1);
		return FALSE;
	} else if(wcl>wp->w_ntrows) {
		update_top_position_wrap();
		return FALSE;
	};
	return TRUE;
}

void set_top_hline(WINDP *wp,offs cof)
{
	offs b0=0;
	if(is_wrap_text(wp->w_fp)) b0=cof;
	else b0=FLineBegin(wp->w_fp,cof);
	textpoint_set(wp->tp_hline,b0);
}

/*	check_cursor_position:	check to see if the cursor is on screen */
int check_cursor_position(WINDP *wp)
{
	if(is_wrap_text(wp->w_fp))return(check_cursor_position_wrap(wp));

	offs cur_offs,cof;
	int i;
	FILEBUF *fp = wp->w_fp;
	int force_reposition=0;
	int head=(wp->w_fp->b_header!=NULL);

	if(
#if	TNOTES
		fp->b_flag==FSNOTES || fp->b_flag&FSNOTESN || 
#endif
		fp->b_flag & FSNLIST) return check_cursor_position_notes(wp);

	cur_offs=tp_offset(wp->tp_current);
	if( cur_offs < tp_offset(wp->tp_hline) 
		|| tp_line(wp->tp_current) > tp_line(wp->tp_hline)+wp->w_ntrows-2-half_last_line-head) 
	{
		// we are out of bounds. reposition!
		force_reposition=1;
//		MESG("check_cursor_position: reposition!!");
	};
	if(!force_reposition) {
		if(is_wrap_text(wp->w_fp)) {
			// MESG("	Not force_reposition!");
			int new_row=window_cursor_line(wp);
			if(new_row>wp->w_ntrows-2) force_reposition=1;
			else return TRUE;
		};
		return TRUE;
	};
	wp->w_flag = UPD_FULL;
	/* reaching here, we need a window refresh */
	i= wp->w_ppline;
	// MESG("check_cursor_position: refresh! ppline=%d",i);
	/* how far back to go? */
	if (i > 0) {
		if (--i >= wp->w_ntrows) i = wp->w_ntrows-2;
	}
	else {
		if(i==0) i = wp->w_ntrows / 2;
		else i=1;
	};
	/* backup to new line at top of window */
	cof=cur_offs;
	while (i-- && cof>0) {
		cof=FPrevLine(fp,cof);
	};
	/* and reset the current line at top of window */
	tp_copy(wp->prev_hline,wp->tp_hline);
	set_top_hline(wp,cof);
	wp->w_flag |= UPD_WINDOW;
	if(update_all) wp->w_flag |= UPD_FULL;
	return(TRUE);
}

/*
 * Display the current cursor position information on the statusline
 */
int  show_position_info(int short_version)
{
	num col;
	num loffs;
	char str[MAXSLEN];
	int sstat=0;
	// MESG("show_position_info: b_flag=%X",cbfp->b_flag);
	if(cwp==NULL) return false;
#if	TNOTES
	if(cbfp->b_flag>=FSNOTES) {
	  if(cbfp->b_flag & FSDIRED) 
	  {
	  	// MESG("show dir info");
	  	if(!short_version) {
			char finfo[MAXFLEN];
	
			sstat=dir_getfile(finfo,2);
			sstat=snprintf(str,MAXSLEN,"%6lld|%s",getcline()+1,finfo);
		} else {
			sstat=snprintf(str,MAXSLEN,"%6lld",getcline()+1);
		};
	  } else {
		sstat=snprintf(str,MAXSLEN,"%s",get_notes_status());
	  };
	} else
#else
	if(cbfp->b_flag & FSDIRED) 
	{
		// MESG("show dir info");
		if(!short_version) {
			char finfo[MAXFLEN];
		
			sstat=dir_getfile(finfo,2);
			sstat=snprintf(str,MAXSLEN,"%6lld|%s",getcline()+1,finfo);
		} else {
			sstat=snprintf(str,MAXSLEN,"%6lld",getcline()+1);
		};
	} else
#endif 
	{
	loffs=Offset()-LineBegin(Offset())+1;
	// MESG("show_position_info: o=%ld loff=%ld b_flag=%X b_mode=%X b_state=%X",Offset(),loffs,cbfp->b_flag,cbfp->b_mode,cbfp->b_state);
	if(cbfp->view_mode & VMHEX) {
		if(short_version==0) {
			sstat=snprintf(str,MAXSLEN,"%5lld %5llX (%02lX)",Offset(),Offset(),utf_value());
		} else sstat=snprintf(str,MAXSLEN,"%5llX",Offset());
	} else {
	  	// MESG("show row/col info");
#if	1
		col=tp_col(cwp->tp_current);
#else
		col=GetCol()+1;
#endif
		if(short_version) {
			sstat=snprintf(str,MAXSLEN,"%6lld",getcline()+1);
		} else {
			if(Eol()) {
				if(is_wrap_text(cbfp)){
					sstat=snprintf(str,MAXSLEN,"%6lld %7lld ",getcline()+1,loffs);
				} else {
				if(bt_dval("show_coffset")) {
					sstat=snprintf(str,MAXSLEN,"%6lld %7lld %7lld ",getcline()+1,Offset(),col);
				} else {
					sstat=snprintf(str,MAXSLEN,"%6lld %7lld %7lld ",getcline()+1,loffs,col);
				};
				}
				if((int)bt_dval("show_cdata")) { 
					if(cwp->w_fp->b_mode & EMDOS) strlcat(str,"0D0A",MAXSLEN);
					else if(cwp->w_fp->b_mode & EMUNIX) strlcat(str,"000A",MAXSLEN);
					else if(cwp->w_fp->b_mode & EMMAC) strlcat(str,"000D",MAXSLEN);
					else strlcat(str,"    ",MAXSLEN);
				};
			} else {
				sstat=snprintf(str,MAXSLEN,"%6lld ",getcline()+1);
				
				if(is_wrap_text(cbfp)){
					sstat=snprintf(str+strlen(str),MAXSLEN-strlen(str),"%7lld ",loffs);
				} else {
					if(bt_dval("show_coffset")) {
						sstat=snprintf(str+strlen(str),MAXSLEN-strlen(str),"%7lld ",Offset());
					} else {
						sstat=snprintf(str+strlen(str),MAXSLEN-strlen(str),"%7lld ",loffs);
					};
					sstat=snprintf(str+strlen(str),MAXSLEN-strlen(str),"%7lld ",col);
				};

				if((int)bt_dval("show_cdata")) {
					int size=1;
					long value=utf_value_len(&size);
					// if(debug_flag()) sstat=snprintf(str+strlen(str),MAXSLEN-strlen(str),"%04lX %d",value,size);
					// else 
					sstat=snprintf(str+strlen(str),MAXSLEN-strlen(str),"%04lX",value);
				};
				if(strlen(str) < 10) short_version=1;
			};
	  };
	};
	};
	if(sstat){
	if(short_version==1) put_string_statusline(cwp, str, 2);
	else put_string_statusline(cwp, str, 1);
	};

	return TRUE;
}

/*
 * Make sure that the display is right. 
 */
int update_screen(int force)
{
	WINDP *wp;
	static int count=0;
	count++;
	int cw_flag=cwp->w_flag;
	if (noupdate) return TRUE;

	/* experiment with screen updating  */
	if(cw_flag==0) return(FALSE);

	if (force == FALSE && kbdmode == PLAY)	return(TRUE);

	/* update any windows that need refreshing */
	// MESG_time("! update_screen: ---------------");
	hide_cursor("update_screen: start");
	// MESG("hide_cursor: ok!");
	if(cwp->selection) {
		if(cwp->selection == REGION_LINE) 
			textpoint_set(cwp->w_emark,LineEnd(tp_offset(cwp->tp_current)));
		else
			tp_copy(cwp->w_emark,cwp->tp_current);
	}

	upd_column_pos();	/* update column position  */
	// MESG_time("update_screen: 1");
	/* if screen is garbage, re-plot it */
	if (update_all)	{ 
		updgar();
	};

	// MESG("loop windows");
	lbegin(window_list);
	while(1) {
		// MESG(";start of while:");
		wp=(WINDP *)lget_current(window_list);
		if(wp==NULL) break;
		// MESG("	+++ loop1: %d",wp->id);
		_el *l_current = window_list->current;
		// MESG("	++++ loop window now is %d wrap=%d",wp->id,is_wrap_text(wp->w_fp));
		if(is_wrap_text(wp->w_fp))	update_window_wrap(wp,force);
		else update_window_nowrap(wp,force);
		set_current(window_list,l_current);
		// MESG(" +++  move to next!");
		lmove_to_next(window_list,0);
		// MESG(" ___ moved!");
	};

	// MESG("	----- after update");
	lbegin(window_list);
	while((wp=(WINDP *)lget(window_list))!=NULL){
		wp->w_fp->line_from=-1;	
		wp->w_fp->line_to=-1;	
	};
	// MESG("go update physical");
	/* update the virtual screen to the physical screen */
	// MESG_time("update_physical");
	update_physical_windows();
	// MESG_time("update_physical end",1);	
	/* update the cursor and flush the buffers */
	update_cursor_position();
	/* set previous line */
	// MESG(";update_screen: set new ppline");
	cwp->w_ppline = window_cursor_line(cwp);
	cwp->w_flag=0;
	update_all=0;
	drv_flush();
	// MESG_time("update_screen: end");
	return(TRUE);
}


void update_window_nowrap(WINDP *wp,int force)
{
	int cw_flag=cwp->w_flag;
		if (wp==cwp) {
			check_cursor_position(wp); /* check if on screen */
			wp->currow = window_cursor_line(wp);

			if(wp->selection || (wp->w_flag & UPD_WINDOW) || force||update_all) 
			{
				upd_all_virtual_lines(wp,"update_screen:3");
// 				continue;
			} else
			if (wp->w_flag & UPD_EDIT) 
			{
				upd_all_virtual_lines(wp,"update_screen:4");	/* update all lines */
			} else 
			if (wp->w_flag & UPD_LINE) 
			{
				// upd_all_virtual_lines(wp,"update_screen:32");
				upd_part(wp,"update_screen: 0");
			} else
			if(wp->w_flag & UPD_MOVE) { 	/* for notes only!  */
				upd_move(wp,"update_screen: 2");
			};
		} else {
			if((wp->w_fp == cwp->w_fp 
				&& ((cw_flag & UPD_EDIT)
				|| (cw_flag & UPD_LINE))
			))
			{
				if((wp->w_flag & UPD_EDIT) || force || update_all) {
					upd_all_virtual_lines(wp,"update_screen:6");
				} else {
					upd_part(wp,"update_screen: 3");
				};
			} else
			if(wp->w_flag){
				if((wp->w_flag & UPD_EDIT) || force || update_all) {
					upd_all_virtual_lines(wp,"update_screen:7");
				} else {
					upd_part(wp,"update_screen: 4");
				};
			} else
			if(update_all) {
				upd_all_virtual_lines(wp,"update_screen:8");
			};
		};
		if ((wp->w_flag & UPD_STATUS) || force) {
			status_line(wp);	/* update statusline */
		};
		wp->w_flag = 0;
}

void update_window_wrap(WINDP *wp,int force)
{
	int cw_flag=cwp->w_flag;
	// MESG("update_window_wrap: ------------------");
		if (wp==cwp) {
			// MESG_time("update_window_wrap:1 window %d",wp->id);
			check_cursor_position_wrap(wp); /* check if on screen */
			// MESG_time("update_window_wrap: check curpos",wp->id);
			wp->currow = window_cursor_line(wp);
#if	1
				upd_all_wrap_lines(wp,"wrap0");
				// show_time("after wrap_lines",1);
#else
			if(wp->selection || (wp->w_flag & UPD_WINDOW) || force||update_all) 
			{
				upd_all_wrap_lines(wp,"wrap1");
			} else
			if (wp->w_flag & UPD_EDIT) 
			{
				upd_all_wrap_lines(wp,"wrap2");	/* update all lines */
			} else 
			if (wp->w_flag & UPD_LINE) 
			{
				upd_part_wrap(wp,"wrap3 partial");
			}
#endif
		} else {
			if((wp->w_fp == cwp->w_fp 
				&& ((cw_flag & UPD_EDIT)
				|| (cw_flag & UPD_LINE))
			))
			{
				if((wp->w_flag & UPD_EDIT) || force || update_all) {
					upd_all_wrap_lines(wp,"wrap4");
				} else {
					upd_part_wrap(wp,"wrap5 partial");
				};
			} else
			if(wp->w_flag){
				if((wp->w_flag & UPD_EDIT) || force || update_all) {
					upd_all_wrap_lines(wp,"wrap6");
				} else {
					upd_part_wrap(wp,"wrap7 partial");
				};
			} else
			if(update_all) {
				upd_all_wrap_lines(wp,"wrap8");
			};
		};
		if ((wp->w_flag & UPD_STATUS) || force) {
			status_line(wp);	/* update statusline */
		};
		wp->w_flag = 0;
		// MESG_time(";update_window_wrap:end window %d",wp->id);
}

void upd_some_virtual_lines(WINDP *wp,char *from)
{
	register offs lp_offs;	/* offset of line to update */
	register int sline;	/* physical screen line to update */
	int head=0;

	if(noupdate) return;
	if(wp->vs == NULL) return;
	if(wp->w_fp == NULL) return;

	if(!(wp->w_fp->view_mode & VMHEX)) wp->w_fp->hl->h_update(wp);
	 set_selection(0);
	// MESG("upd_some_virtual_lines:w=%d from[%s] left=%d b_flag=0x%X w_flag=0x%X lines %d-%d",wp->id,from,wp->w_lcol,wp->w_fp->b_flag,wp->w_flag,wp->w_fp->line_from,wp->w_fp->line_to);
	/* search down the lines, updating them */
	lp_offs = tp_offset(wp->tp_hline);

	if(wp->w_fp->b_header) {
		head=1;
		vt_str(wp,wp->w_fp->b_header,0,0,0,-1,0);
	};
	if(wp->w_fp->b_flag!=FSNOTES && wp->w_fp->b_flag!=FSNOTESN && !(wp->w_fp->b_flag & FSNLIST)) 
	{	/* Buffer view  */
		// MESG("update virtual from buffer! wp=%d top offs=%ld",wp->id,lp_offs);
		for(sline=head;sline < wp->w_ntrows;sline++) 
		{
			/* and update the virtual line */
			if(sline >= wp->w_fp->line_from && sline <= wp->w_fp->line_to) {
				wp->vs[sline]->v_flag =1;	/* update flag  */
				// MESG("	update line %d",sline);
			} else wp->vs[sline]->v_flag=0;
			vtmove(wp,sline, 0);
	
			if (lp_offs <= FSize(wp->w_fp)) { // if not at the end of file
			/* if we are not at the end */
				if(cwp->selection==0) set_selection(false);
				lp_offs=vtline(wp,lp_offs);
				/* we must update the column selection here */
				if(cwp->selection) set_selection(false);
			};
			vteeol(wp,0,0);
		}
	}
	else if(wp->w_fp->b_flag & FSNLIST) {	/* list view  */

		int note_row;
		// MESG("update FSNLIST");
		lbegin(wp->w_fp->dir_list_str);
		for(note_row=0;note_row< wp->top_note_line; note_row++) lmove_to_next(wp->w_fp->dir_list_str,0);
		
		for(sline=head;sline<wp->w_ntrows-1;sline++)
		{
			istr *row_data;
			if(sline >= wp->w_fp->line_from &&
				sline <= wp->w_fp->line_to)
			wp->vs[sline]->v_flag =1;	/* update flag  */
			else wp->vs[sline]->v_flag=0;

			row_data=(istr *)lget_current(wp->w_fp->dir_list_str);
			if(row_data) {
				vt_str(wp,&row_data->start,sline,wp->current_note_line- wp->top_note_line+1,0,-1,row_data->selected);
				// MESG("- %2d : [%s]",sline,&row_data->start);
			} else {
				// MESG("- %2d : clear note line!",sline);
				vt_str(wp,"",sline,-1,0,-1,-1);
			};
			lmove_to_next(wp->w_fp->dir_list_str,0);
		};
	}
#if	TNOTES
	else 
	{	/* Notes view  */
		int tag_row;
		int note_row;

		// MESG("update virtual from notes list! wp=%d top tag_line=%d note_line=%d",wp->id,wp->top_tag_line,wp->top_note_line);
		// MESG("tag list size=%d note list size=%d",wp->w_fp->b_tag_list->size,wp->w_fp->dir_list_str->size);
		lbegin(wp->w_fp->b_tag_list);
		for(tag_row=0;tag_row < wp->top_tag_line ;tag_row++) lmove_to_next(wp->w_fp->b_tag_list,0);
		lbegin(wp->w_fp->dir_list_str);
		for(note_row=0;note_row< wp->top_note_line; note_row++) lmove_to_next(wp->w_fp->dir_list_str,0);
		
		for(sline=head;sline<wp->w_ntrows-1;sline++)
		{
			istr *row_data;
			wp->vs[sline]->v_flag =1;	/* update flag  */

			row_data=(istr *)lget_current(wp->w_fp->b_tag_list);
			if(row_data) {
				// MESG("- %2d : [%s]",sline,&row_data->start);
				// int selected = iarray_index(sel_tags,row_data->index,num_of_selected_tags);
				vt_str(wp,&row_data->start,sline,wp->current_tag_line- wp->top_tag_line+1,0,20,row_data->selected);
				// vteeoc(wp,TAGS_WIDTH+3);
			} else {
				// MESG("- %2d : clear tag line!",sline);
				vt_str(wp,"                 ",sline,-1,0,20,-1);
				// vteeoc(wp,TAGS_WIDTH+3);
			};
			row_data=(istr *)lget_current(wp->w_fp->dir_list_str);
			if(row_data) {
				vt_str(wp,&row_data->start,sline,wp->current_note_line- wp->top_note_line+1,TAGS_WIDTH+3,-1,row_data->selected);
				// MESG("- %2d : [%s]",sline,&row_data->start);
			} else {
				// MESG("- %2d : clear note line!",sline);
				vt_str(wp," ",sline,-1,TAGS_WIDTH+3,-1,-1);
			};
			lmove_to_next(wp->w_fp->b_tag_list,0);
			lmove_to_next(wp->w_fp->dir_list_str,0);
		};
	};
#endif
	if(wp->w_flag & UPD_STATUS) {
		wp->w_flag=UPD_STATUS;
	}
	else wp->w_flag=0;
	set_draw_flag(wp,"update_some_lines");
	getwquotes(wp,0);	// set highlight to the top line!
}

void upd_move(WINDP *wp,char *from)
{
	register int sline;	/* physical screen line to update */
	int line1,line2;
	int b_flag=wp->w_fp->b_flag;

	if(!(b_flag==FSNOTES || b_flag & FSNOTESN || b_flag & FSNLIST)) 
		return;
	if(noupdate) return;
	if(wp->vs == NULL) return;
	if(wp->w_fp == NULL) return;
#if	TNOTES
	if(b_flag==FSNOTES) {
		line1 = wp->w_prev_line-wp->top_tag_line+1;
		line2 = get_current_line()-wp->top_tag_line+1;
	} else
#endif 
	{
		line1 = wp->w_prev_line-wp->top_note_line+1;
		line2 = get_current_line()-wp->top_note_line+1;
	};

	// MESG("upd_move: window %d from [%s] lines %d - %d",wp->id,from,line1,line2);

	if(b_flag & FSNLIST) {
		int note_row;
		// MESG("update FSNLIST");
		lbegin(wp->w_fp->dir_list_str);
		for(note_row=0;note_row< wp->top_note_line; note_row++) lmove_to_next(wp->w_fp->dir_list_str,0);
		
		for(sline=1;sline<wp->w_ntrows-1;sline++)
		{
			istr  *row_data;
			if((sline==line1)||(sline==line2)) {
			wp->vs[sline]->v_flag =1;	/* update flag  */

			row_data=(istr *)lget_current(wp->w_fp->dir_list_str);
			if(row_data) {
				vt_str(wp,&row_data->start,sline,wp->current_note_line- wp->top_note_line+1,0,-1,row_data->selected);
			} else {
				// MESG("- %2d : clear note line!",sline);
				vt_str(wp,"",sline,-1,0,-1,-1);
			};
			};
			lmove_to_next(wp->w_fp->dir_list_str,0);
		};
	}
#if	TNOTES
	else {
		int tag_row;
		int note_row;

		lbegin(wp->w_fp->b_tag_list);
		for(tag_row=0;tag_row < wp->top_tag_line ;tag_row++) lmove_to_next(wp->w_fp->b_tag_list,0);
		lbegin(wp->w_fp->dir_list_str);
		for(note_row=0;note_row< wp->top_note_line; note_row++) lmove_to_next(wp->w_fp->dir_list_str,0);
		
		for(sline=1;sline<wp->w_ntrows-1;sline++)
		{
			istr *row_data;
			if(sline==line1|| sline==line2) {;
			wp->vs[sline]->v_flag =1;	/* update flag  */

			row_data=(istr *)lget_current(wp->w_fp->b_tag_list);
			if(row_data) {
				// MESG("- %2d : [%s]",sline,&row_data->start);
				vt_str(wp,&row_data->start,sline,wp->current_tag_line- wp->top_tag_line+1,0,20,row_data->selected);
				// vteeoc(wp,TAGS_WIDTH+3);
			} else {
				// MESG("- %2d : clear tag line!",sline);
				vt_str(wp,"                 │",sline,-1,0,20,-1);
				// vteeoc(wp,TAGS_WIDTH+3);
			};
			row_data=(istr *)lget_current(wp->w_fp->dir_list_str);
			if(row_data) {
				vt_str(wp,&row_data->start,sline,wp->current_note_line- wp->top_note_line+1,TAGS_WIDTH+3,-1,row_data->selected);
				// MESG("- %2d : [%s]",sline,&row_data->start);
			} else {
				// MESG("- %2d : clear note line!",sline);
				vt_str(wp," ",sline,-1,TAGS_WIDTH+3,-1,0);
			};
			};
			lmove_to_next(wp->w_fp->b_tag_list,0);
			lmove_to_next(wp->w_fp->dir_list_str,0);
		};
	};
#endif
	set_draw_flag(wp,"upd_move");
	wp->w_flag=UPD_STATUS;
	getwquotes(wp,0);	// set highlight to the top line!
}


void upd_part(WINDP *wp,char *from)
{
	register offs lp_offs;	/* offset of line to update */
	register int sline;	/* physical screen line to update */
	int line1,line2;
	int out_of_view=0;
	int head=0;

	if(noupdate) return;
	if(wp->vs == NULL) return;
	if(wp->w_fp == NULL) return;
	if(wp->w_fp->b_flag >= FSNOTES) {
		line1 = wp->w_fp->line_from - wp->top_note_line;
		line2 = wp->w_fp->line_to - wp->top_note_line;
	} else {
		line1 = wp->w_fp->line_from-tp_line(wp->tp_hline);;
		line2 = wp->w_fp->line_to-tp_line(wp->tp_hline);;
	}
	// MESG("upd_part: window %d lcol=%d from [%s] lines %d - %d",wp->id,wp->w_lcol,from,line1,line2);
	if(wp->w_lcol!=wp->w_plcol) return upd_all_virtual_lines(wp,"upd_part w_lcol");
	if(line1<0 && line2<0) out_of_view=1;
	else {
		if(line1<0) line1=0;
		if(line2<0) out_of_view=1;
		if(line1>wp->w_ntrows) out_of_view=1;
		if(line2>wp->w_ntrows) line2=wp->w_ntrows;
	};

	if(wp->selection==0) set_selection(0);
	if(!out_of_view) {
	if(!(wp->w_fp->view_mode & VMHEX)) wp->w_fp->hl->h_update(wp);

	/* search down the lines, updating them */
	lp_offs = tp_offset(wp->tp_hline);
	
	if(wp->w_fp->b_header) {
		head=1;
		vt_str(wp,wp->w_fp->b_header,0,0,0,-1,0);
	};
	// MESG("	upd_part: line1=%d line2=%d",line1,line2);
	for(sline=head;sline <= line2 ;sline++) 
	{
		/* and update the virtual line */
		if(sline>=line1 && sline<=line2) wp->vs[sline]->v_flag =1;
		else wp->vs[sline]->v_flag =0;
		vtmove(wp,sline, 0);

		if (lp_offs <= FSize(wp->w_fp)) { // if not at the end of file
			lp_offs=vtline(wp,lp_offs);
			/* we must update the column selection here */
			set_selection(false);
		};
		vteeol(wp,0,0);
	};
	set_draw_flag(wp,"upd_part");
	};
	if(wp->w_flag & UPD_STATUS) wp->w_flag=UPD_STATUS;
	else wp->w_flag=0;
	getwquotes(wp,0);	// set highlight to the top line!
}

void upd_part_wrap(WINDP *wp,char *from)
{
	register offs lp_offs;	/* offset of line to update */
	register int sline;	/* physical screen line to update */
	int line1,line2;
	int out_of_view=0;
	int head=0;

	if(noupdate) return;
	if(wp->vs == NULL) return;
	if(wp->w_fp == NULL) return;
	if(wp->w_fp->b_flag >= FSNOTES) {
		line1 = wp->w_fp->line_from - wp->top_note_line;
		line2 = wp->w_fp->line_to - wp->top_note_line;
	} else {
		line1 = wp->w_fp->line_from-tp_line(wp->tp_hline);;
		line2 = wp->w_fp->line_to-tp_line(wp->tp_hline);;
	}
	MESG("upd_part: window %d lcol=%d from [%s] lines %d - %d",wp->id,wp->w_lcol,from,line1,line2);
	if(wp->w_lcol!=wp->w_plcol) return upd_all_wrap_lines(wp,"upd_part w_lcol");
	if(line1<0 && line2<0) out_of_view=1;
	else {
		if(line1<0) line1=0;
		if(line2<0) out_of_view=1;
		if(line1>wp->w_ntrows-1) out_of_view=1;
		if(line2>wp->w_ntrows-1) line2=wp->w_ntrows;
	};

	if(wp->selection==0) set_selection(0);
	if(!out_of_view) {
	if(!(wp->w_fp->view_mode & VMHEX)) wp->w_fp->hl->h_update(wp);

	/* search down the lines, updating them */
	lp_offs = tp_offset(wp->tp_hline);
	
	if(wp->w_fp->b_header) {
		head=1;
		vt_str(wp,wp->w_fp->b_header,0,0,0,-1,0);
	};
	// MESG("	upd_part: line1=%d line2=%d",line1,line2);
	
	// cached_llen(wp->w_fp,lp_offs,1);

	for(sline=head;sline <= line2 ;sline++) 
	{
		/* and update the virtual line */
		if(sline>=line1 && sline<=line2) wp->vs[sline]->v_flag =1;
		else wp->vs[sline]->v_flag =0;
		vtmove(wp,sline, 0);

		if (lp_offs <= FSize(wp->w_fp)) { // if not at the end of file
			lp_offs=vt_wrap_line(wp,lp_offs);
			/* we must update the column selection here */
			set_selection(false);
		};
		vteeol(wp,0,0);
	};
	set_draw_flag(wp,"upd_part");
	};
	if(wp->w_flag & UPD_STATUS) wp->w_flag=UPD_STATUS;
	else wp->w_flag=0;
	getwquotes(wp,0);	// set highlight to the top line!
}

/*	upd_all_virtual_lines:	update all the lines in a window */
void upd_all_virtual_lines(WINDP *wp,char *from)
{
	register offs lp_offs;	/* offset of line to update */
	register int sline;	/* physical screen line to update */
	int head=0;

	if(noupdate) return;
	if(wp->vs == NULL) return;
	if(wp->w_fp == NULL) return;

	if(!(wp->w_fp->view_mode & VMHEX)) wp->w_fp->hl->h_update(wp);
	 set_selection(0);
	
	// MESG("upd_all_virtual_lines:w=%d vinfo=%f from[%s] left=%d b_flag=0x%X w_flag=0x%X",wp->id,bt_dval("show_vinfo"),from,wp->w_lcol,wp->w_fp->b_flag,wp->w_flag);
	/* search down the lines, updating them */
	lp_offs = tp_offset(wp->tp_hline);

	if(wp->w_fp->b_header) {
		head=1;
		vt_str(wp,wp->w_fp->b_header,0,0,0,-1,0);
	};
	getwquotes(wp,0);
	if(wp->w_fp->b_flag!=FSNOTES && wp->w_fp->b_flag!=FSNOTESN && !(wp->w_fp->b_flag & FSNLIST)) 
	{	/* Buffer view  */
		// MESG("update virtual from buffer! wp=%d top offs=%ld",wp->id,lp_offs);
		for(sline=head;sline < wp->w_ntrows;sline++) 
		{
			/* and update the virtual line */
			wp->vs[sline]->v_flag =1;
			vtmove(wp,sline, 0);
			if (lp_offs <= FSize(wp->w_fp)) { // if not at the end of file
			/* if we are not at the end */
				if(cwp->selection==0) set_selection(false);
				lp_offs=vtline(wp,lp_offs);
				/* we must update the column selection here */
				if(cwp->selection) set_selection(false);
			};
			
			vteeol(wp,0,0);
		}
	}
	else if(wp->w_fp->b_flag & FSNLIST) {	/* list view  */

		int note_row;
		// MESG("update FSNLIST");
		lbegin(wp->w_fp->dir_list_str);
		for(note_row=0;note_row< wp->top_note_line; note_row++) lmove_to_next(wp->w_fp->dir_list_str,0);
		
		for(sline=head;sline<wp->w_ntrows-1;sline++)
		{
			istr *row_data;
			wp->vs[sline]->v_flag =1;	/* update flag  */

			row_data=(istr *)lget_current(wp->w_fp->dir_list_str);
			if(row_data) {
				vt_str(wp,&row_data->start,sline,wp->current_note_line- wp->top_note_line+1,0,-1,row_data->selected);
				// MESG("- %2d : [%s]",sline,&row_data->start);
			} else {
				// MESG("- %2d : clear note line!",sline);
				vt_str(wp,"",sline,-1,0,-1,0);
			};
			lmove_to_next(wp->w_fp->dir_list_str,0);
		};
	}
#if	TNOTES
	else 
	{	/* Notes view  */
		int tag_row;
		int note_row;

		// MESG("update virtual from notes list! wp=%d top tag_line=%d note_line=%d",wp->id,wp->top_tag_line,wp->top_note_line);
		// MESG("tag list size=%d note list size=%d",wp->w_fp->b_tag_list->size,wp->w_fp->dir_list_str->size);
		lbegin(wp->w_fp->b_tag_list);
		for(tag_row=0;tag_row < wp->top_tag_line ;tag_row++) lmove_to_next(wp->w_fp->b_tag_list,0);
		lbegin(wp->w_fp->dir_list_str);
		for(note_row=0;note_row< wp->top_note_line; note_row++) lmove_to_next(wp->w_fp->dir_list_str,0);
		
		for(sline=head;sline<wp->w_ntrows-1;sline++)
		{
			istr *row_data;
			wp->vs[sline]->v_flag =1;	/* update flag  */

			row_data=(istr *)lget_current(wp->w_fp->b_tag_list);
			if(row_data) {
				// MESG("- %2d : [%s]",sline,&row_data->start);
				vt_str(wp,&row_data->start,sline,wp->current_tag_line- wp->top_tag_line+1,0,20,row_data->selected);
				// vteeoc(wp,TAGS_WIDTH+3);
			} else {
				// MESG("- %2d : clear tag line!",sline);
				vt_str(wp,"                 │",sline,-1,0,20,0);
				// vteeoc(wp,TAGS_WIDTH+3);
			};
			row_data=(istr *)lget_current(wp->w_fp->dir_list_str);
			if(row_data) {
				vt_str(wp,&row_data->start,sline,wp->current_note_line- wp->top_note_line+1,TAGS_WIDTH+3,-1,row_data->selected);
				// MESG("- %2d : [%s]",sline,&row_data->start);
			} else {
				// MESG("- %2d : clear note line!",sline);
				vt_str(wp," ",sline,-1,TAGS_WIDTH+3,-1,0);
			};
			lmove_to_next(wp->w_fp->b_tag_list,0);
			lmove_to_next(wp->w_fp->dir_list_str,0);
		};
	};
#endif
	if(wp->w_flag & UPD_STATUS) {
		wp->w_flag=UPD_STATUS;
	}
	else wp->w_flag=0;
	set_draw_flag(wp,"upd_all_virtual_lines");
	getwquotes(wp,0);	// set highlight to the top line!
	// MESG("upd_all_virtual_lines: end!");
}


void free_virtual_window(WINDP *wp)
{
 int i=0;
 if(drv_type<2) return;	/* no need for curses,xlib for now, they use a common virtual area for all windows  */
	
 if(wp->vs!=NULL) { /* free it before allocate new size! */
	// MESG("free_virtual_window:");
 	for(i=0;i< wp->w_ntrows+2;i++) {
		if(wp->vs[i]!=NULL) {
			efree(wp->vs[i],"free row");
			wp->vs[i]=NULL;
		} else {
			break;
		};
	};
    efree(wp->vs,"free window vs");
	wp->vs=NULL;
 };
}

void allocate_virtual_window(WINDP *wp)
{
 VIDEO *vp;
 int i;
 // MESG("	- allocate_virtual_window: cols=%d rows=%d",wp->w_ntcols,wp->w_ntrows);
 wp->vs = (VIDEO **) emalloc(sizeof(VIDEO *) * (wp->w_ntrows+3),"allocation virtual window");
 for(i=0;i< wp->w_ntrows+2;i++) {
 	vp = (VIDEO *) emalloc(sizeof(VIDEO) + (wp->w_ntcols+2) * sizeof(struct vchar),"allocate virtual line");
 	if(vp==NULL) {
		ERROR("could not allocate memory for virtual window");
		exit(1);
	};
	vp->v_flag=0;
	vp->slow_line=0;
	wp->vs[i]=vp;
 };
 // set the last one as NULL to know till where to free!
 wp->vs[i]=NULL;
}

#if	0
# normal_bg=white
# quote_bg
# modeline_bg=cyan
# search_bg=red
# info_bg 
# spec_bg=white
# menu_bg=cyan (menu active)

 imodeline_bg -> info_bg
 select_bg == modeline_bg
 box_bg --> modeline_bg

----------------------
normal_fg=black
normal_bg=white
modeline_fg=lwhite
modeline_bg=cyan
rowcol_fg=yellow
search_fg=white
search_bg=red
quote_fg=lgreen
quote_bg=magenta
comment_fg=brown
comment_bg=white
prep_fg=orange
prep_bg=white
tag_fg=brown
word1_fg=red
spec_fg=lblue
spec_bg=white
ctrl_fg=lred
word2_fg=lmagenta
w_fore=green
orizon_fg=lmagenta
numeric_fg=blue
info_fg=blue
info_bg=magenta
box_fg=white
box_bg=cyan
menu_fg=white
menu_bg=cyan
imodeline_fg=lred
imodeline_bg=black

#endif
/* -- */
