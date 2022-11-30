/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

	Common gtk driver code
*/

extern VAR option_names[];
extern int nnarg;
extern FILEBUF *cbfp;
extern alist *color_schemes;

GtkWidget *wlist;

void set_current_scheme(int scheme)
{
 int scheme_ind=0;
 MESG("set_current_scheme: scheme=%d drv_colors=%d",scheme,drv_colors);

 color_scheme_ind=scheme-1;
 set_btval("color_scheme",-1,NULL,color_scheme_ind+1); 
 lbegin(color_schemes);
 while((current_scheme=(COLOR_SCHEME *)lget(color_schemes))!=NULL) {
	// MESG(" check scheme  %d <> %d [%s]",scheme_ind,color_scheme_ind,current_scheme->scheme_name);
 	if(scheme_ind==color_scheme_ind) {
		MESG("	set_current_scheme %d [%s]",scheme,current_scheme->scheme_name);
		break;
	};
	scheme_ind++;
 };
 
 MESG("set_current_scheme:end");
}

#include "xthemes.c"

int confirm(char *title,char *prompt,int always) {
  int result; 
  if(!always && ((int)bt_dval("safe_ops")==0)) return true;
  GtkWidget *dialog;
  dialog = gtk_message_dialog_new(GTK_WINDOW(parent),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_QUESTION,
            GTK_BUTTONS_YES_NO,
            "%s",prompt);
  gtk_window_set_title(GTK_WINDOW(dialog), title);
  result=gtk_dialog_run(GTK_DIALOG(dialog));
  // MESG("result: [%s] -> %d",title,result);
  // MESG("GTK_RESPONSE_ACCEPT: %d",GTK_RESPONSE_ACCEPT);
  // MESG("GTK_RESPONSE_NONE: %d",GTK_RESPONSE_NONE);
  // MESG("GTK_RESPONSE_DELETE_EVENT: %d",GTK_RESPONSE_DELETE_EVENT);
  // MESG("GTK_RESPONSE_YES: %d",GTK_RESPONSE_YES);
  // MESG("GTK_RESPONSE_NO: %d",GTK_RESPONSE_NO);
  gtk_widget_hide(dialog);
  gtk_widget_destroy(dialog);
  if(result==GTK_RESPONSE_YES||result==GTK_RESPONSE_ACCEPT) return true;
  return false;
}

void drv_open()
{
 static int opened=0;
 char *display_name = NULL;

 if(opened) return;	// only once!
 opened=1;

	dis0 = XOpenDisplay(display_name);
	screen_num = DefaultScreen(dis0);

	color_scheme_ind=(int)bt_dval("xcolor_scheme");
	init_color();

	parent = create_parent();
	if(cwp==NULL) { ERROR("no cwp!");exit(0);};
	wlist=create_select_window();	// this is the list window (only one!)
	gtk_widget_realize(parent);
	gtk_widget_show(parent);

	gtk_widget_grab_focus(cwp->gwp->draw);
}

void show_cursor_dl(int pos)
{
	return ;
}

char *drv_info()
{
  static char *info="GTK driver";
  return info;
}

void drv_close()
{
}

void events_flush()
{
 // static int ind=0;
 while(gtk_events_pending()>0) {
 	gtk_main_iteration_do(1);
	// usleep(100);
	// printf("events_flush: %d\n",ind++);
 }
}

GWINDP * drv_new_twinp()
{
 return NULL;
}

int checking_break_key=0;

static int key_index=0;
static int key_buf[300];
static int key_wait=0;

int drv_getc (int quote) 
{
   int ret0;
   // static int ind=0;
   key_wait=1;
	// MESG("drv_getc:%d",ind++);
	if(key_index<1) {
		// we have nothing in the buffer, wait till we do
		do {
			events_flush();
			usleep(10000);
			// printf("drv_getc loop:%d\n",ind++);
		} while(key_index<1);
	};
	ret0=key_buf[key_index-1];

	key_index--;
	key_wait=0;
	lastkey=ret0;
	return(ret0);
}

char *get_font_string()
{
 static char fstr[128];
 snprintf(fstr,128,"%s %d",current_font_name,font_size);
 // MESG("get_font_string:[%s]",fstr);
 return fstr;
}

int put_wstring(WINDP *wp, char *st,int ulen)
{
 GeEditDisplay *wd = GTK_EDIT_DISPLAY(wp->gwp->draw);
// show text line
 int width=0,height=0;	/* Pango measurements  */
 if(wd->layout==NULL){
 	wd->layout = pango_cairo_create_layout (wd->cr);
	// MESG("put_wstring: new layout!");
 };
 pango_layout_set_text (wd->layout, st, -1);
 pango_layout_set_font_description (wd->layout, wd->ge_font_desc);
 pango_layout_get_size (wd->layout, &width, &height);
 int y_pos_correction=0;
 int c_width=CLEN*ulen;
 if(width<(int)(CLEN*PANGO_SCALE*ulen)) {
	px=0 ;
	return -1;
	c_width=width/PANGO_SCALE+2*CLEN;
 };
 // MESG("put_wstring %d %d ulen=%d",width,(int)(CLEN*PANGO_SCALE*ulen),ulen);	/*   */
 cairo_set_source_rgb(wd->cr,ccolorb.red,ccolorb.green,ccolorb.blue);
 // c_width=width/PANGO_SCALE+2*CLEN;
 if(height>CHEIGHTI*PANGO_SCALE) y_pos_correction = height/PANGO_SCALE-CHEIGHTI;
 cairo_rectangle(wd->cr, px, py, c_width,CHEIGHTI);
 cairo_fill(wd->cr);
 cairo_move_to(wd->cr,px+1,py+1-y_pos_correction);
 cairo_set_source_rgb(wd->cr,ccolorf.red,ccolorf.green,ccolorf.blue);
 pango_cairo_show_layout (wd->cr, wd->layout);
 // px += width/PANGO_SCALE;
 px += ulen*CLEN;
 return width/PANGO_SCALE;
}

unsigned int put_wchar(WINDP *wp, char *st)
{
 GeEditDisplay *wd = GTK_EDIT_DISPLAY(wp->gwp->draw);
// show text line
 int width=0,height=0;	/* Pango measurements  */
 if(wd->layout==NULL){
 	wd->layout = pango_cairo_create_layout (wd->cr);
	// MESG("put_wchar: new layout!");
 };
 pango_layout_set_text (wd->layout, st, -1);
 pango_layout_set_font_description (wd->layout, wd->ge_font_desc);
 pango_layout_get_size (wd->layout, &width, &height);
 int y_pos_correction=0;
 int c_width=CLEN;
 cairo_set_source_rgb(wd->cr,ccolorb.red,ccolorb.green,ccolorb.blue);
 if(width>CLEN*PANGO_SCALE) c_width=3*CLEN;
 if(height>CHEIGHTI*PANGO_SCALE) y_pos_correction = height/PANGO_SCALE-CHEIGHTI;
 cairo_rectangle(wd->cr, px, py, c_width,CHEIGHTI);
 cairo_fill(wd->cr);
 cairo_move_to(wd->cr,px+1,py+1-y_pos_correction);
 cairo_set_source_rgb(wd->cr,ccolorf.red,ccolorf.green,ccolorf.blue);
 pango_cairo_show_layout (wd->cr, wd->layout);

 px += width/PANGO_SCALE;
 return width/PANGO_SCALE;
}

void drv_move(int y,int x) 
{
	if(y<0) y=0;
	if(x<0) x=0;
	px=CLEN *x;
	py=CHEIGHTI*y;
	cposx=x;
	cposy=y;
	// MESG("drv_move: x=%d y=%d",x,y);
}

void top_menu(int init) {
 /* a dummy one. Menu is handled by gtk */
}

/* init environment */
int init_drv_env()
{
 half_last_line=1;

 slide_flag=0; 
 xwin=2;
 {
	color_scheme_ind=4;
 }

 default_lang=0;	// default is utf
 return DRIVER_GTK2;
}

void drv_init(int argc, char **argv)
{
  setlocale(LC_CTYPE,"");
//  gtk_set_locale();
  gtk_init (&argc, &argv);
}

/* Dummy function in this driver  */
int text_mouse_key(int *c)
{
	return FALSE;
}

// this is used from selectl
int wait_for_value()
{
 // static int ind=0;
 // MESG("wait_for_value:\n");
 while(novalue!=0) {
	events_flush();
	usleep(100000);
	// MESG("wait_for_value: %d",ind++);
 };
 list_off();

 	if(entry_mode==KNORMAL) {
		 gtk_widget_grab_focus(cwp->gwp->draw);
	}
	else {
		gtk_window_set_focus(GTK_WINDOW(parent),gs_entry);
		gtk_widget_grab_focus(gs_entry);
		gtk_widget_set_sensitive(gs_entry,TRUE);
	}
 return(index_value);
}

/* show a list in a gtk window and wait for response */
int selectl(char *title, char *glist[],int nu,int lmax,int sx,int sy,int maxl,int active)
{
 int status;
 int x,y,w,h;

  if(nu<2) { return(0);};
  activate_list_mode();
  gtk_window_get_position(GTK_WINDOW(parent),&x,&y);
  gtk_window_get_size(GTK_WINDOW(parent),&w,&h);
  novalue=1;
  gtk_window_set_title (GTK_WINDOW (wlist), title);

  set_dtlist(list1, glist);
  if(nu < lmax) lmax=nu;
  if(lmax>25) lmax=25;
  if(lmax<0) lmax=25;

//  gtk_window_set_default_size(GTK_WINDOW(wlist), maxl*5+100, lmax*8+100);
  gtk_window_resize(GTK_WINDOW(wlist), maxl*5+100, lmax*8+200);

  gtk_window_move(GTK_WINDOW(wlist),x+w/3,y+40);
  gtk_widget_show(wlist);
  status = wait_for_value();
  return(status); // index_value
}

void gtk_entry_commit_cb 
	(GtkIMContext *context, const gchar  *str, gpointer data) 
{
//	put the string in the buffer!
 strlcpy(utfokey,str,10);
}

gboolean
on_parent_key_press_event        (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data)
{
	if(event->keyval==0xFE20) event->keyval=0xFF09;	// what is the problem with shift tab ?
	if(entry_mode==0) gtk_widget_grab_focus(cwp->gwp->draw);
	else { 
		gtk_widget_activate(gs_entry);
	};
	return(FALSE);
}

void update_cursor_position()
{
	// MESG("update_cursor_position:");
#if	TNOTES
	if(cwp->w_fp->b_flag == FSNOTES) movecursor(cwp->current_tag_line-cwp->top_tag_line, WGetCol());
	else if(cwp->w_fp->b_flag & FSNOTESN) movecursor(cwp->current_note_line-cwp->top_note_line, WGetCol());
	else
#endif
	if(cwp->w_fp->b_flag & FSNLIST) {
		// MESG("move %d %d",cwp->current_note_line-cwp->top_note_line, 0);
		movecursor(cwp->current_note_line-cwp->top_note_line, 0);
	} else movecursor(cwp->currow,WGetCol());
	// MESG("update_cursor_position: ->sc");
	show_cursor("update_cursor_position");
}


void drv_post_init()
{
}

void drv_post_windows_update()
{
}

int dra_width(GWINDP *gwp)
{
 int rw = GTK_EDIT_DISPLAY(gwp->draw)->edit_rect.width;
	return(rw);
}

int dra_height(GWINDP *gwp)
{
 int rh = GTK_EDIT_DISPLAY(gwp->draw)->edit_rect.height ;
	return(rh);
}

void button_color_save(GtkWidget *wd, gpointer *data)
{
// print_colors();
 color_scheme_save();
}

void drv_start_checking_break()
{
	checking_break_key=1;
}

void drv_stop_checking_break()
{
	checking_break_key=0;
	utflen=0;
	utfokey[0]=0;
	key_index=0;
}

int drv_check_break_key()
{
 int key=0;
 static int count=0;
 // MESG("drv_check_break_key:");
 if(checking_break_key) {
 count++;
 if(count>10000) {
	events_flush();
	if(key_index>0) {
		key=key_buf[key_index];
		key_index--;
	};
	count=0;

	if(key==3) {
		set_break();
		return 1;
	};
 };
 };
 return 0;
}

int increase_font_size(int n)
{
 font_size  +=1;
 set_font(current_font_name);
 return true;
}

int decrease_font_size(int n)
{
 if(font_size > 8) {
 	font_size-=2;
	set_font(current_font_name);
 };
 return true;
}

int toggle_bold(int n)
{
	if(font_weight==CAIRO_FONT_WEIGHT_NORMAL) font_weight=CAIRO_FONT_WEIGHT_BOLD;
	else font_weight=CAIRO_FONT_WEIGHT_NORMAL;
	set_font(current_font_name);
	return true;
}

int set_fontindex(int n) {
// dummy function
 return TRUE;
}

int export_region(ClipBoard *clip)
{
#if	1
 return ext_system_copy();
#else
 long size;
 int nbytes;
 if(clip->height>1) size=(clip->width+1)*clip->height;
 else size=clip->width;
 if(gtk_selection_owner_set(GTK_WIDGET(parent),GDK_SELECTION_PRIMARY,GDK_CURRENT_TIME )) {
	XStoreBuffer(dis0, clip->text,size,0);
	gtk_selection_add_target(parent, GDK_SELECTION_PRIMARY,
				 GDK_SELECTION_TYPE_STRING, 0);
	/* we need to get it back, otherwise it gets much complicated to paste from onother instance */
//	XFetchBytes(dis0, &nbytes);
 } else {
	msg_line("cannot set selection owner");
 }
 return(1);
#endif
}

int x_insert ()
{
 int nbytes;
 char *dat;
 static GdkAtom targets_atom = GDK_NONE;
 	// MESG("x_insert:");
	if(targets_atom == GDK_NONE)
//	    targets_atom = gdk_atom_intern("COMPOUND_TEXT", FALSE);
//	    targets_atom = gdk_atom_intern("GTK_TEXT_BUFFER_CONTENTS", FALSE);
	    targets_atom = gdk_atom_intern("UTF8_STRING", TRUE);
		selection_received_flag=0;
		gtk_selection_convert(
		parent, 
		GDK_SELECTION_PRIMARY,
		targets_atom, 
		GDK_CURRENT_TIME
	);
    while(selection_received_flag==0) {
		events_flush();
    };
    if(import_buffer!=NULL) 
	{ // got a selection
		insert_string(cbfp,import_buffer,import_length);
		msg_line("imported %d bytes",import_length);
		return import_length;
	} else {
		dat = XFetchBytes (dis0, &nbytes);
   		if (dat != NULL) {
		 	insert_string(cbfp,dat,nbytes);
   	 		XFree (dat);
   		};
	   msg_line("Imported %d bytes",nbytes);
	   return nbytes;
	};

}


// put a utfchar on virtual screen, convert to local character if error!
int addutfvchar1(char *str, vchar *vc, int pos,FILEBUF *w_fp)
{
 unsigned char ch;
 int i;

 if(vc->uval[1]!=0) {	// utf character
	for(i=0;i<6;i++) {
		ch=vc->uval[i];
		if(ch==0) break;
		str[pos++]=ch;
	};
 } else {
 	ch=vc->uval[0];
	if(ch<128) str[pos++]=ch;
	else { // this is a local char, convert to utf
	 char *utf_char,*s;
	 char locchar[2];
	 gsize r,w;
	 int local_lang=w_fp->b_lang;

	 if(local_lang==0) local_lang=default_local_codepage;
	 // MESG(": pos %3d lang=%d",pos,local_lang);
	 locchar[0]=ch;locchar[1]=0;
	 s=utf_char = g_convert(locchar,-1,"UTF-8",codepage_str[local_lang],&r,&w,NULL);
	 if(s==NULL) { 
//		MESG("NULL utfchar ! %X",locchar[0]);
		str[pos++]=' ';
	 } else {
//		 MESG("pos %d: %X -> [%s]",pos,ch,s);
		 while(*s) str[pos++]=*s++;
		 g_free(utf_char);
	 }
	 };
	}
 return pos;
}

void put_wtext_slow(WINDP *wp, int row,int maxcol)
{
 int col;
 int imax=0;
 int imin=0;
 int i1;
 vchar *v1, *vtext;
 VIDEO *vp1;
 int ccolor,cattr;
 char st[MAXBLEN];

 vp1 = wp->vs[row];
 v1=vtext=vp1->v_text;

 imin=0;
 ccolor=v1[0].fcolor;
 cattr=v1[0].bcolor;
 // MESG("---> draw row slow %d",row);
 drv_color(ccolor,cattr);
	imax=maxcol;

	/* count of trailing spaces is maxcol-imax */
	drv_move(row,imin);

	for(col=imin,i1=0;col<imax;col++) {
		// drv_move(row,col);
		if(v1[col].uval[0]==0xFF) {	/* skip space of wide utf chars  */
			if(v1[col].uval[1]==0xFF) continue;
		};
		ccolor=v1[col].fcolor;
		cattr=v1[col].bcolor;

		drv_color(ccolor,cattr); 
		i1=addutfvchar1(st,&v1[col],i1,wp->w_fp);
		st[i1]=0;i1=0;
		drv_move(row,col);
		put_wchar(wp,st);
	}
//	MESG("-- end row %d i1=%d [%s]",row,i1,st);
	expose_line(row,wp);	/* is needed for GTK2!  */
}

// draw window row on screen
void put_wtext(WINDP *wp, int row,int maxcol)
{
 int col;
 int imax=0;
 int imin=0;
 int i1;
 // int p_space=0;
 vchar *v1, *vtext;
 VIDEO *vp1;
 int ccolor,cattr;
 char st[MAXBLEN];
 int ccolor_p,cattr_p;
 int imove=0;
 vp1 = wp->vs[row];
 v1=vtext=vp1->v_text;

 imin=0;
 ccolor=v1[0].fcolor;
 cattr=v1[0].bcolor;
 ccolor_p=ccolor;
 cattr_p=cattr;
 // MESG("---> draw row %d",row);
 	drv_color(ccolor,cattr);
	imax=maxcol;

	/* count of trailing spaces is maxcol-imtrax */
	drv_move(row,imin);

	for(col=imin,i1=0;col<imax;col++) {
		if(v1[col].uval[0]==0xFF) {	/* skip space of wide utf chars  */
			// imove--;
			if(v1[col].uval[1]==0xFF) continue;
		};
		ccolor=v1[col].fcolor;
		cattr=v1[col].bcolor;
		if(ccolor!=ccolor_p || cattr!=cattr_p) {
			if(i1>0) {
				st[i1]=0;
				drv_move(row,imove);
				if(put_wstring(wp,st,col-imove)==-1) {
					put_wtext_slow(wp,row,maxcol);
					return;
				};
			};
			drv_color(ccolor,cattr);
			i1=0;
			imove=col; 
			ccolor_p=ccolor;cattr_p=cattr;
		};
		// if(v1[col].uval[0]==0x20) p_space=1;
		i1=addutfvchar1(st,&v1[col],i1,wp->w_fp);
		st[i1]=0;
		// if(v1[col+1].uval[0]!=0xFF) p_space=0;
	};
	if(i1>0) {
		st[i1]=0;
		if(put_wstring(wp,st,col-imove)==-1) {
			put_wtext_slow(wp,row,maxcol);
		};
	};
//	MESG("-- end row %d i1=%d [%s]",row,i1,st);
	expose_line(row,wp);	/* is needed for GTK2!  */
}

int select_font_mono(int n)
{
 selectl("Select font",mono_fonts_list,mono_fonts_number,20,5,1,30,-1);
 if(index_value>=0) set_font(mono_fonts_list[index_value]);

 return true;
}

GtkWidget *new_combo_box(char **list,int active)
{
 GtkWidget *cbox;

 int max=0;
  cbox = gtk_combo_box_text_new ();

  if(active<0) active=0;

  for(;*list!=NULL;list++) {
	max++;
	gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (cbox), *list);
  }
  if(active>max-1) active=0;
  gtk_combo_box_set_active((GtkComboBox *)cbox,active);
  gtk_widget_show(cbox);

 return cbox;
}

int set_font(char *font_name)
{
 WINDP *wp;
 GeEditDisplay *wd;
 current_font_name = font_name;
 if(!window_list) return FALSE; 
 // MESG("set_font:");
 /* change cairo font for all windows */
 lbegin(window_list);
 hide_cursor("set_font");
 while((wp=(WINDP *)lget(window_list))!=NULL) {
	getwquotes(wp,0);
	wd = GTK_EDIT_DISPLAY(wp->gwp->draw);
	if(wd!=NULL) {
		free_virtual_window(wp);
		drv_start_window_update(wp);
		set_window_font(wp);
		end_draw(wd,"");

		wd->ncols = wd->width/CLEN;
		wd->nrows = wd->height/CHEIGHTI;

		wp->w_ntrows = wd->nrows;
		if(wd->nrows * CHEIGHTI < wd->height) wp->w_ntrows++;
		wp->w_ntcols = wd->ncols;
		if(wd->ncols * CLEN < wd->width) wp->w_ntcols++;
		wp->w_ntrows+=half_last_line;
		allocate_virtual_window(wp);
	};
 };
   /* we must redraw all the windows */
	if(!startup_exe) {
//	MESG("font change, redraw all windows!");
	set_update(cwp,UPD_ALL);
	update_full(1);
	msg_line("set font %s %d",current_font_name,font_size);
   	update_status();
   };
//  MESG("set_font: end");
  return(TRUE);
}

gboolean
on_dra0_key_press_event(GtkWidget       *widget,
                        GdkEventKey     *event,
                        gpointer         user_data)
{
 guint ks,n_chars;
 static guint flag=0;
 char *bufp;
 char buf[MAXMLEN];
 // static int ind=0;
 unsigned int key_state=0;
 // GeEditDisplay *wd=GTK_EDIT_DISPLAY(widget);
 int emacs_emul = (int)bt_dval("keyboard_emulation");
 ks = event->keyval;
 key_state=event->state;
 n_chars = event->length; 

 // MESG("key %X state=%X len=%d",ks,key_state,n_chars);

 if(cursor_status()==0) 
 {
//	MESG("key press skipped: %X",ks);
	events_flush();
	usleep(1000);
	return TRUE;
 };

 if(key_state>8191) { key_state-=8192;/* key_lang=ks/0x100;*/ };
   gtk_im_context_filter_keypress(im_context, event);

// if(event->state & GDK_MOD1_MASK) MESG("alt key pressed!");

 if(checking_break_key) {
	key_index=1;
	if(event->state & GDK_CONTROL_MASK)
	 	key_buf[key_index]=ks%32;
	else 
	 	key_buf[key_index]=ks;
	return 1;
 };


 if(entry_mode==KENTRY) {
//	MESG("entry_mode=1 ks=%X",ks);
	if(ks % 0x100 !=13) {
		gtk_widget_grab_focus(gs_entry);
		return FALSE;
	};

	if(ks==GDK_KEY_Return || ks==GDK_KEY_Escape || ((ks==106 || ks==103) && (key_state & GDK_CONTROL_MASK))) 
		entry_mode=KNORMAL;

	gtk_widget_hide(gs_entry);
	gtk_widget_hide(toolbar2);
	gtk_widget_hide((GtkWidget *)search_tbar);
	return(FALSE);
 };

  utflen=strlen(utfokey);

  if(utflen>n_chars) n_chars=utflen;
  // printf("key:%d  %X n=%d\n",ind++,ks,n_chars);
	if(n_chars>1)
	{ // This is utf8 multibyte character
	if(key_state<3 ||  key_state==128 || key_state==129) {
		int ul=strlen(utfokey);
		utflen=ul;
//		MESG("1 key = %X utfokey=[%s] utflen=%d n_chars=%d key_state=%d",ks,utfokey,utflen,n_chars,key_state);

		if(cbfp->b_lang==0) {
			ks=key_buf[key_index++]=utfokey[0];
			n_chars=0;
		} else { // convert to local codepage and insert in the text!
			char *outkey;
			gsize r,w;
			outkey=g_convert(utfokey,-1,codepage_str[cbfp->b_lang],"UTF-8",&r,&w,NULL);
			if(outkey!=NULL) {
			ks=key_buf[key_index++]=outkey[0];
			g_free(outkey);
			};
			n_chars=0;
			utflen=0;
			utfokey[0]=0;
		}
	} else {
//		MESG("2 key = %X utfokey=[%s] utflen=%d n_chars=%d key_state=%d",ks,utfokey,utflen,n_chars,key_state);
		n_chars=1;
		ks=normalize(ks);
	};
	} else {	// One byte character or control key!
//		MESG("3 key = %X utfokey=[%s] utflen=%d n_chars=%d key_state=%d ks=%X",ks,utfokey,utflen,n_chars,key_state,ks);
#if	NUSE
		if(ks<0xFE00 ) {
			if(key_state<3 ||  key_state==128 || key_state==129) {
			if(cbfp->b_lang==0 || utflen==1) {
//				MESG("needs utf! utflen=%d [%s]",utflen,utfokey);
			} else {
			char *outkey;
			gsize r,w;
			int ks1=0;
			outkey=g_convert(utfokey,-1,codepage_str[default_local_codepage],"UTF-8",&r,&w,NULL);
			ks1=ks=outkey[0];
			MESG("ks<0xFE00: ks=%X ks1=%X [%s]",ks,ks1,utfokey);
			n_chars=0;
			utflen=0;
			utfokey[0]=0;
			g_free(outkey);
			};
		}
		};
#endif
			utflen=0;
			utfokey[0]=0;
//			if(ks>0xFE00) { n_chars=0;ks=0;};
	};

  if (ks >= 0xFF00) {
//	MESG("control key %X",ks);
	ks = ks & 0xFF;

	if(ks==CHR_ESC) {
		if(!quote_flag)	key_buf[key_index++]=7;
		else key_buf[key_index++]=ks;
		n_chars=0;
	} else {
	if (key_state & GDK_SHIFT_MASK) {
		bufp = Shift_KeySym_Mapping [ks];
	}
	else if (key_state & GDK_CONTROL_MASK) {
		bufp = Control_KeySym_Mapping [ks];
	}
	else if (key_state & GDK_MOD1_MASK) {
		bufp = Alt_KeySym_Mapping [ks];
	}
	else {
		bufp = KeySym_Mapping [ks];
	};
	if (0 != (n_chars = (int) *bufp++)) {
		 memcpy(buf, bufp, n_chars);
	} else { return 1 ;};

	while(n_chars>0) { 
		key_buf[key_index++]=buf[n_chars-1]; n_chars--; 
	};
   };
 } else
 {
  if (n_chars == 1) {
	if (key_state & GDK_MOD1_MASK) {	// this is alt
		key_buf[key_index++] = ks;
		key_buf[key_index++] = CHR_META;
	} else if (key_state & GDK_CONTROL_MASK) {
// The following is a hack specific for X11, find another way!
		if(quote_flag) {
			ks=ks%32;	
			key_buf[key_index++]=ks;
			n_chars=0;
		} else {
		if(ks=='+') ks='^' ;
		if(ks=='=') ks='^' ;
		if(ks=='-') ks='_' ;
		if( ks < 64 && ks > 32 ) { 
			 key_buf[key_index++] = ks ;
			 if(emacs_emul){
			 	key_buf[key_index++] = 24; // control_x
			 }
			 n_chars--;
		} else {
	       if (ks == ' ') { 
			key_buf[key_index++]=0;
			};
		   if(ks == CHR_LBRA) {flag |= META; return 0;};
		   if((ks == 'X' || ks=='x') && emacs_emul) {
		   	if(!(flag & CTLX)) { flag |= CTLX;return 0;};
		   };
		   key_buf[key_index++] = ks % 32;
		   if (flag & CTLX && emacs_emul) key_buf[key_index++] = 24; // control_x
		   if (flag & META) key_buf[key_index++] = CHR_META;
		}
	}
	}
	else if (ks == 27) { 
//				ks= CNTRL | 'G'; 
		if(!quote_flag)	key_buf[key_index++]=7;
		else key_buf[key_index++]=ks;
	} else { 
		key_buf[key_index++]=ks;
		if(flag & CTLX && emacs_emul) key_buf[key_index++]=24; // control_x
	};
  	n_chars--;
  } else {
	if(ks==32) {
		key_buf[key_index++]=0;
	} else {
#if	NUSE
		if(ks<0xFE00) msg_line("unknown key ! k=%d , %d, len=%d",event->keyval,event->keyval & 0xFF, event->length);
#endif
	};
  }
 }
 if(key_wait) return TRUE;

 if(quote_flag) { /* insert control character in the file */
  int c=0;
	quote_flag=FALSE;
	if(key_index==0) {	/* insert null character */
		insert_chr(nnarg,0);
	} else {
		while(--key_index>=0) {
			c=key_buf[key_index];
			if(kbdmode==STOP) record_key(c);
			if(c<32) {
				insert_chr(nnarg,c);
			} else {
			if(c<256) insert_chr(nnarg,c);
			else {
				insert_chr(nnarg,27);
			};
			};
		};
	};
	nnarg=1;
 	update_screen(0);
	flag = 0;key_index=0;
//	gtk_widget_grab_focus(widget);
  	return TRUE;	
 };
  ks = getcmd();
  if(describe_flag==1) {
	show_key(ks);
  	describe_flag=0;
	return(FALSE);
  };
//  hide_cursor("	on_dra0_key_press: 2");
//	MESG("	on_dra0_key_press: ks=%d=%X",ks,ks);
  main_execute(ks);
//	MESG("	on_dra0_key_press: executed ks=%d=%X",ks,ks);
  update_screen(0);
  flag = 0;
//  gtk_widget_grab_focus(widget);
  return FALSE;
}

gboolean
on_wlist_key_press (GtkWidget *widget,GdkEventKey *event,gpointer data)
{
 GtkTreeIter iter;
 GtkTreeModel *mlist1;
 GtkTreeSelection *selection;
	// put here any logic for the wlist!!

	if(event->keyval==GDK_KEY_Return)
	{ // enter key
		selection=gtk_tree_view_get_selection (GTK_TREE_VIEW (list1));
		mlist1 = gtk_tree_view_get_model(GTK_TREE_VIEW(list1));
		if(gtk_tree_selection_get_selected (selection, (GtkTreeModel **)&mlist1, &iter))
		{
			GtkTreePath *path;
			path = gtk_tree_model_get_path(mlist1, &iter);
			index_value = atoi(gtk_tree_path_to_string(path));

			if(index_value==0) {
				on_list1_activate_row(selection,mlist1);
			}
		};
	};
	// - add case for search in the list and go to a specific item (TODO)
	if(event->keyval==GDK_KEY_Escape)
	{ // escape key -> cancel dialog
		gtk_widget_hide(widget);
		list_off();
		index_value=-1;novalue=0;
	};
	return(FALSE);
}

gboolean on_gs_entry_key_press_event(GtkEntry *g_entry,
	GdkEventKey     *event, gpointer  user_data)
{
  guint ks;
  char st[MAXLLEN];
  static int quotef=0;
  ks = event->keyval;

  if(entry_mode==KNORMAL) return(FALSE);

  st[0]=0;
  strlcpy(st,gtk_entry_get_text(GTK_ENTRY(g_entry)),MAXLLEN);

  if(quotef) {
	if(event->state & GDK_CONTROL_MASK) strlcat(st,"^",MAXLLEN);
	st[strlen(st)+1]=0; st[strlen(st)]=ks;
  	quotef=0;
	return FALSE;
  };
  if(event->state & GDK_CONTROL_MASK && ks=='q') {
	quotef=1;
	return FALSE;
  };
  if(ks==GDK_KEY_Down || ks==GDK_KEY_Up || ks==GDK_KEY_Tab) 
  {
	strlcpy(st,gtk_entry_get_text(GTK_ENTRY(g_entry)),MAXLLEN);
	if(ks==GDK_KEY_Tab) {
		list_select(st,0); 
//		MESG("tab set text [%s]",st);
		gtk_entry_set_text(GTK_ENTRY(g_entry),st);
		return(TRUE);
	};
  	if(ks==GDK_KEY_Up) { 
		list_select(st,-1); 
		gtk_entry_set_text(GTK_ENTRY(g_entry),st);
	};
	if(ks==GDK_KEY_Down) { 
		list_select(st,1); 
		gtk_entry_set_text(GTK_ENTRY(g_entry),st);
	};
	return(TRUE);
  };

  if(ks==GDK_KEY_Return || ks==GDK_KEY_Escape || ((ks==106 || ks==103) && (event->state & GDK_CONTROL_MASK))) { // enter
		gtk_widget_grab_focus(cwp->gwp->draw);
		if(ks==103 || ks==GDK_KEY_Escape) gs_entry_return_flag=0;
		else gs_entry_return_flag=1;
		entry_mode=KNORMAL;
#if	GTK3
		gtk_widget_hide((GtkWidget *)search_tbar);
#endif
		return TRUE;
  };
  return FALSE;
}

gboolean on_parent_destroy(GtkWidget *w, GdkEvent *ev, gpointer data)
{
	quit(1);
	return TRUE;
}

gboolean
on_gs_entry_ok_event        (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data)
{
  const char *ss;
	// MESG("on_gs_entry_ok_event: entry_mode=%d",entry_mode);
  if(entry_mode==KNORMAL ) return(FALSE);

	gs_entry_return_flag=1;
	if(entry_mode==KCONFIRM) 
	{
	key_buf[key_index++]='Y';
	key_wait=1;
	};
	ss=gtk_entry_get_text(GTK_ENTRY(gs_entry));
	// MESG("gs_entry_ok_event: %s",ss);
	entry_mode=KNORMAL;
	if(ss==NULL) return FALSE;
	if(ss[0]==0) return FALSE;
//	confirm_mode=0;
	return FALSE;
}

gboolean
on_gs_entry_cancel_event        (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data)
{
//	MESG("on_gs_entry_cancel_event: entry_mode=%d",entry_mode);
  if(entry_mode==KNORMAL) return(FALSE);
	gs_entry_return_flag=0;
	if(entry_mode==KCONFIRM) 
	{
	key_buf[key_index++]='N';
	key_wait=1;
	};
	entry_mode=KNORMAL;
	return FALSE;
}

GtkWidget * create_top_menu(MENUS *menu_des , GtkWidget *parent,GtkAccelGroup  *menu_accels)
{
 GtkWidget *n_menu_menu;
 int i;

 n_menu_menu = gtk_menu_new();
 for(i=0;;i++) {
	if(menu_des->element[i].high==0) break;
 	create_menu_element( &menu_des->element[i], n_menu_menu, parent, menu_accels,0); 
 };
 gtk_menu_reposition((GtkMenu *)n_menu_menu);
 return (n_menu_menu);
}

/* get a string from the status line ..... */
int getstring(char *prompt, char *st1, int maxlen,int disinp)
{
//	gsize r,w;
	char st2[MAXLLEN];

	if(maxlen>MAXLLEN-1) maxlen=MAXLLEN-1;
	if(execmd) {
		printf("%s",prompt);
		if(fgets(st1,maxlen,stdin)==NULL) return false;
		return(true);
	};
	init_input();
	strlcpy(st2,st1,MAXLLEN-1);
//	MESG("getstring: kbdmode=0x%X",kbdmode);
	if(kbdmode==PLAY) {
		return(set_play_string(st1));
	};
	if(kbdmode==BPLAY) {
		return(set_play_string1(st1));
	};

	gtk_entry_set_text(GTK_ENTRY(gs_entry),st1);
//  if in encrypted then show only asterixes !!!!!!!!!!
	if(!disinp) {
		gtk_entry_set_visibility(GTK_ENTRY(gs_entry),0);
	} else {
		gtk_entry_set_visibility(GTK_ENTRY(gs_entry),1);
	};
	entry_mode=KENTRY;
	start_interactive(prompt);
	entry_mode=KENTRY;
	gtk_widget_show(gs_entry);


	gtk_widget_set_can_focus (gs_entry,TRUE);
	gtk_widget_grab_focus(gs_entry);

	while(entry_mode==KENTRY) {
		// MESG("KENTRY:");
		events_flush();
		usleep(100000);
	};
//	MESG("getstring: end1");
	if(cbfp->b_lang == 0) {
		strlcpy(st2,gtk_entry_get_text(GTK_ENTRY(gs_entry)),MAXLLEN);
		strlcpy(st1,st2,MAXLLEN);
	} else {
		char *ustr;
		gsize r,w;
		strlcpy(st2,gtk_entry_get_text(GTK_ENTRY(gs_entry)),MAXLLEN);
		ustr = g_convert(st2,-1,codepage_str[default_local_codepage],"UTF-8",&r,&w,NULL);
		strlcpy(st1, ustr ,MAXLLEN);
		g_free(ustr);
	};

	list_update(st2);

	set_record_string(st2);
//	MESG("getstring: before hide");
	gtk_widget_hide(gs_entry);
	end_interactive();
//	MESG("getstring: got [%s]",st2);
	if(gs_entry_return_flag) return(TRUE);
	else return(FALSE);
}



void drv_window_delete(WINDP *wp)
{
}

void drv_free_win(WINDP *wp)
{
	free_virtual_window(wp);
	delete_gwindow(wp);
}

/* split a window and create a new one linked next to this */
WINDP * make_split(WINDP *wp)
{
    register WINDP *new_wp;	// the new window
//	MESG("make_split: ======================");
	new_wp = dublicate_window(wp);
	if(wp) setwquotes(new_wp,0,tp_offset(wp->tp_hsknown));
	else setwquotes(new_wp,0,0);
    return (new_wp);
}

int set_sposition(WINDP *wp, int *st, int *l)
{
 int cline;

 if(wp != NULL) {
	if(wp->w_fp == NULL) { cline=0;*st=0;*l=0;return(cline);};


	if(wp->w_fp->b_flag & FSNLIST) cline=wp->top_note_line;
#if	TNOTES
	else if(wp->w_fp->b_flag & FSNOTESN) cline=wp->top_note_line;
	else if(wp->w_fp->b_flag & FSNOTES) cline=wp->top_tag_line;
#endif
	else cline = tp_line(wp->tp_hline);

	*l = wp->w_ntrows;
	*st = cline-1;
 } else { cline=0;*st=0;*l=0; };

 return(cline);
}

// past function
int system_paste(int n)
{
 if(dont_edit() || cbfp->b_flag & FSDIRED )return false;
// hide_cursor("system_paste");
 x_insert();
 update_screen(1);

 return TRUE;
}

/* populate a gtkclist with text from a string array */
void  set_dtlist(GtkWidget *dlist, char *glist[])
{
 int i;
 GtkTreeModel *mlist;
 GtkTreeIter iter;
 GtkTreeSelection *list1_select;

 list1_select = gtk_tree_view_get_selection (GTK_TREE_VIEW (dlist));
 mlist = gtk_tree_view_get_model((GtkTreeView *)dlist);
 gtk_list_store_clear((GtkListStore *)mlist);

 for(i=0;;i++) {
	if(glist[i]==NULL) break;
	gtk_list_store_append((GtkListStore *)mlist, &iter);
	gtk_list_store_set((GtkListStore *)mlist, &iter,
		0,glist[i],
	-1);
 };
 // select first element after population
 gtk_tree_model_get_iter_first(GTK_TREE_MODEL(mlist),&iter);
 gtk_tree_selection_select_iter(list1_select, &iter);
 
}

void cb_menu(GtkWidget *widget, M_element *element)
{
  int n;
  int status;
  char *s;

  n = (long int) element->menu;
  if(n==0) n=1;

//  MESG("menu element [%s] key [%c] [%s]",element->txt,element->high,element->macro);  
  status = (*element->func)(n);
  s = element->macro;
  record_key(META|*s);
  for(;*s!=0;s++){
	record_key(*s);
  };
  if(status>0) { 
	update_screen(1); 
  } else {
	show_cursor("cb_menu");
  }
}

GtkFileChooser *show_file_chooser(char *title, int action,char *name)
{
    GtkFileChooser *fileChooser;
	char *uname;	// utf string
#if	0
    GtkFileFilter *filter;
#endif
    fileChooser = (GtkFileChooser *) gtk_file_chooser_dialog_new(title, GTK_WINDOW(parent),
			action,
//            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
//            GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, 
			"_Cancel",  GTK_RESPONSE_REJECT,
			"_OK",   GTK_RESPONSE_ACCEPT,
			NULL);
	if(name!=NULL) 
	if(name[0]!=0)
	if(name[0]!='/' || name[1]!=0){
		uname=g2utf(name);
		if(action==GTK_FILE_CHOOSER_ACTION_SAVE)
		{
			gtk_file_chooser_set_current_name   (fileChooser,uname);
		} else {
			gtk_file_chooser_select_filename   (fileChooser,uname);
		};
		g_free(uname);
	};

#if	NUSE
    filter = GTK_FILE_FILTER(gtk_file_filter_new());
    gtk_file_filter_set_name(filter, "all files");
    gtk_file_filter_add_pattern(filter, "*");
    gtk_file_chooser_add_filter(fileChooser, filter);

    filter = GTK_FILE_FILTER(gtk_file_filter_new());
    gtk_file_filter_set_name(filter, "C files");
    gtk_file_filter_add_pattern(filter, "*.c");
    gtk_file_chooser_add_filter(fileChooser, filter);


    filter = GTK_FILE_FILTER(gtk_file_filter_new());
    gtk_file_filter_set_name(filter, "h files");
    gtk_file_filter_add_pattern(filter, "*.h");
    gtk_file_chooser_add_filter(fileChooser, filter);
#endif
    gtk_widget_show(GTK_WIDGET(fileChooser));
 return fileChooser;
}

void on_destroy(GtkWidget *widget)
{
// MESG("destroy: ---");
}

int open_file_dialog(char *tname,int n)
{
 GtkFileChooser *fileChooser;
 GSList *list, *saveList;
 char *filename=NULL;
 char full_name[MAXPLEN];
 int stat;
 	if(is_scratch_buffer(cbfp)) {
		if(chdir(get_start_dir())) {
			MESG("cannot go to start dir!");
			return false;
		};
	};

	if(!file_exist(tname)) {
		tname[0]=0;
		if(n==1) {
			if(is_scratch_buffer(cbfp)){
				set_full_name(full_name,get_start_dir(),cbfp->b_fname,MAXPLEN);
			} else {
				set_full_name(full_name,cbfp->b_dname,cbfp->b_fname,MAXPLEN);
			}
		} else {
			if(is_scratch_buffer(cbfp)){
				strlcpy(full_name,get_start_dir(),MAXPLEN);
			} else {
				strlcpy(full_name,cbfp->b_dname,MAXPLEN);
			}
		}
	}
	else strlcpy(full_name,tname,MAXPLEN);

	if(n==1)	
	fileChooser=show_file_chooser("Open file",GTK_FILE_CHOOSER_ACTION_OPEN,full_name);
	else
	fileChooser=show_file_chooser("Open dir",GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,full_name);
    if(gtk_dialog_run(GTK_DIALOG(fileChooser))==GTK_RESPONSE_ACCEPT) 
	{
        for(list=saveList=gtk_file_chooser_get_filenames(fileChooser);
        	list!=NULL; list=g_slist_next(list)) 
		{
            filename = list->data;
        }
        g_slist_free(saveList);
	};

    gtk_widget_destroy(GTK_WIDGET(fileChooser));

	if(filename) {
		set_bfname(full_name,filename);

		stat=open_file_named(filename);

		set_hmark(1,"open_file_dialog");
		return stat;
	} else return 0;
}

void on_select_hide(GtkWidget *wd,GtkListStore *mlist1)
{
  int back_index=index_value;
// MESG("on_select_hide");
 /* clear the corresponding list */
 gtk_list_store_clear(GTK_LIST_STORE(mlist1));
 index_value=back_index;
}

int on_select_destroy(GtkWidget *wd,GtkListStore *mlist1)
{
// MESG("on_select_destroy:");
 gtk_widget_hide(wlist);
 list_off();
 index_value=-1;novalue=0;
 return TRUE;
}

int on_select_delete(GtkWidget *wd,GdkEventAny *event)
{
// MESG("on_select_delete:");
 gtk_widget_hide(wlist);
 list_off();
 index_value=-1;novalue=0;
 return TRUE;
}

void on_list1_change_row(GtkTreeSelection *selection, GtkTreeModel *tlist1)
{
 GtkTreePath *path;
 GtkTreeIter selected_row;
 
 if (gtk_tree_selection_get_selected (selection, &tlist1, &selected_row))
 {
	path = gtk_tree_model_get_path(tlist1, &selected_row);
	index_value = atoi(gtk_tree_path_to_string(path));
 } else return;

}

void on_list1_activate_row(GtkTreeSelection *selection, GtkTreeModel *mlist1)
{
 gtk_widget_hide(wlist);
	novalue=0;
	list_off();
}



void cb_close_window(GtkWidget *w, WINDP * wp)
{
 set_current_window(wp,"cb_close_window");
 delete_window(1);
 update_screen(1);
}

gboolean
toggle_button (GtkWidget *widget,
	int type)
{
 int status;
// char *option_name = option_names[type].name;

 status = gtk_toggle_tool_button_get_active((GtkToggleToolButton *)widget);

// MESG("Toggle button %d %s toggle status=%d name=[%s] = [%s]",type,option_name,status,option_name,option_names[type].name);
 
// set_btval(option_name,-1,NULL,status);
 switch(type) {
 	case EMCCASE: {
		gmode_exact_case = status;
	};break;
	case EMCREGEXP: {
		gmode_reg_exp = status;
		break;
	};
	case EMCOVER: {
		gmode_over = status;
	};
 };
	return FALSE;
}

// toolbar callback function
void cb_tools(GtkWidget *w, T_ELEMENT *telem )
{
 int status;
// MESG("cb_tools: type=%d",telem->type);
 if(telem->type >100) {
 	status = toggle_button(w, telem->type-100);
	return;
 } else {
	status = (*telem->func)(1,telem->type);
 };
 if(status>0) {
 	update_screen(1);
 	show_cursor("cb_tools");
 };
};

/* put a string on message line after correctly interpret it */
void drv_msg_line(char *arg)
{
	if (discmd == FALSE || macro_exec) return;

	put_string_dl((char *)str2out(arg));
	drv_flush();
}

typedef struct RGB_DEF {
 int r,g,b;
 char *color_name;
} RGB_DEF;
RGB_DEF *get_rgb_values(char *color_name);

#if	NUSE
void init_default_schemes()
{
 MESG("init_default_schemes:");
 int scheme_ind;
 for(scheme_ind=0;scheme_ind<COLOR_SCHEMES;scheme_ind++){
	int i;
	COLOR_SCHEME *scheme = malloc(sizeof(COLOR_SCHEME));
	scheme->scheme_name = scheme_names[scheme_ind];

	int total_colors=FG_COLORS+BG_COLORS;
	for(i=0;i<total_colors;i++) 
	{
		// fprintf(stderr,"- color %d\n",i);
		scheme->color_values[i]=basic_color_values[scheme_ind][i];
	};
	add_element_to_list((void *)scheme,color_schemes);
 };
	
}
#endif

void set_cursor(int val,char *from)
{
	cursor_showing=val;
//	MESG("set_cursor: val=%d %s",val,from);
}



extern short *kbdptr;
int repeat_arg=1;

int (*get_menucmd(MENUS *m1,int first))()
{
 int (*execf)();
 short line=first;

//	MESG("get_menucmd: PLAY [%s] line=%d ---------",m1->title,line);
	
	if( m1->element[line].type == MMENU) {
		int line2;
		MENUS *m2 = m1->element[line].menu; 
//		MESG("       :call    menu line %d = [%s] first=%d %c ",line,m1->element[line].txt,first,first);
		
		first = *kbdptr++;
//		MESG("		get submenu:[%s] letter=[%c]",m2->title,first);
		for(line2=0;;line2++)
		{
			if(m2->element[line2].high==0) {
				execf=do_nothing;
//				MESG("get_menucmd	: found no function for letter [%c]!",first);
				return(execf);
			};
//			MESG("	check line %d [%s] %c first=%X %c",line2,m2->element[line2].txt,m2->element[line2].high,first,first);
			if(m2->element[line2].high==capital(first)) break;
		};
//		MESG("	get menu line2=%d",line2);

		if(m2->element[line2].type == MMENU){
//			MESG("	is menu line2 %d",line2); 
			execf = get_menucmd(m2->element[line2].menu,line2) ;
		} else {
//			MESG("	line2 %d is command",line2);
			execf = m2->element[line2].func;
			repeat_arg = (long int)m2->element[line2].menu;
//			MESG("	line2(%d) [%s] %ld",line2,m2->element[line2].txt,repeat_arg);
		};
	} else {
		execf = m1->element[line].func ;
		repeat_arg = (long int)m1->element[line].menu;
//		MESG("       :command replay line %d [%s] repeat=%ld",line,m1->element[line].txt,repeat_arg);
	};
	return(execf);
}

int menu_command(int n)
{
 register int status=FALSE;
 register int (*execfunc)();		/* ptr to function to execute */
 int line;
 short key;
 MENUS *main_menu;
 main_menu=&m_topn;
 
// MESG("menu_command: kbdmode=0x%X",kbdmode);

 if(kbdmode==PLAY ||kbdmode==BPLAY){

	key=get_next_key(kbdmode);

	int line2;
	MENUS *m1;
//	MESG("play menu_command: n=%d key=%X %c--------------",n,key,key);
 	key=normalize(key);
	for(line=0;;line++) 
	{
		if(main_menu->element[line].high==0) {line=0;break;};
		if(main_menu->element[line].high==key) break;
	};
//	MESG("    main menu element %d [%s]",line,main_menu->element[line].txt);
	m1 = main_menu->element[line].menu;

	key=get_next_key(kbdmode);

//	MESG("    : second key=[%c] %d",key,key);
	for(line2=0;;line2++) 
	{
		if(m1->element[line2].high==0) {line2=0;break;};
		if(m1->element[line2].high==capital(key)) break;
	};
	
//	MESG("    menu element %d [%s]",line2,m1->element[line2].txt);
//	MESG("       : normalized key=%d [%c] line2=%d",key,key,line2);
	
	execfunc = get_menucmd(m1,line2);
//	MESG("	execute menu function with repeat arg %d",repeat_arg);
	if(execfunc != NULL) status=(*execfunc)(repeat_arg); // menu_command
//	MESG("            : status = %d",status);
	return(status);
 };
 return(true);
}

void start_interactive(char *prompt)
{
	if(compact1) {
		gtk_widget_hide(toolbar1);
	};
	gtk_widget_hide(statusbar1);

	gtk_label_set_text((GtkLabel *)gs_label,prompt);
	gtk_widget_show(gs_label);
	gtk_widget_show(toolbar2);
}

void end_interactive()
{
	gtk_widget_hide(toolbar2);
	gtk_widget_hide(gs_label);

	gtk_widget_show(statusbar1);

	entry_mode=KNORMAL;

	if(compact1) {
		gtk_widget_show(toolbar1);
	};
}


extern ClipBoard *MainClipBoard;

void on_selection_get_event(GtkWidget *widget, GtkSelectionData *seldata,
		   guint info, guint time_stamp, gpointer data)
{
	// I own the selection! set the data
	gtk_selection_data_set(
		seldata, 
//		seldata->target, 
		gtk_selection_data_get_target(seldata),
		8,
	(unsigned char *) MainClipBoard->text, 
	MainClipBoard->width*MainClipBoard->height);
}

void on_selection_received_event(GtkWidget *widget, GtkSelectionData *seldata,
		    gpointer data)
{
	// I receive a selection...
	if(import_buffer!=NULL) { 
		free(import_buffer);
		import_buffer=NULL;
	};
//	import_length=seldata->length;
	import_length=gtk_selection_data_get_length(seldata);

	if(import_length>0) {
		import_buffer=(char *)malloc(import_length);
		memcpy(import_buffer,
//			seldata->data,
			gtk_selection_data_get_data(seldata),
			import_length);
	};

	selection_received_flag=1;
}

char *g2utf(char *str)
{
 gsize r,w; // current and wide length
 char *utf;
 prepare_converter(cbfp->b_lang);
 utf = g_convert_with_iconv(str,-1,cd,&r,&w,NULL);
 return utf;
}

int new_shell(int n)
{
	// start terminal in background!, no return code!, so return always true.
	if(system("xterm&")<0) return(FALSE);
	else return(TRUE);
}

void main_loop()
{
	update_screen(TRUE);
	// MESG("start gtk_main:");
#if	0
	while(1){
		while(gtk_events_pending()) gtk_main_iteration();
		usleep(10000);
	};
#else
	gtk_main();
#endif
}

void prepare_converter(int buffer_lang)
{
 static int init_converter=-1;
 if(buffer_lang==default_local_codepage||buffer_lang==-default_local_codepage) return;
 if(buffer_lang==0) buffer_lang=default_local_codepage;
 if(init_converter<0) {
    cd = g_iconv_open (codepage_str[default_local_codepage],codepage_str[buffer_lang]);
	init_converter=buffer_lang;
 } else {
 	if(init_converter!=buffer_lang) {
		g_iconv_close(cd);
		if(buffer_lang>0) cd = g_iconv_open (codepage_str[default_local_codepage],codepage_str[buffer_lang]);
		else cd = g_iconv_open (codepage_str[-buffer_lang],codepage_str[default_local_codepage]);
		init_converter=buffer_lang;
	};
 };
}

int set_bg(int n)
{
	return(new_shell(n));
}

int toggle_mouse(int n)
{
	return FALSE;
}

int text_mouse_pos(int n)
{
	return FALSE;
}

int text_mouse_left_press(int n)
{
	return FALSE;
}

int text_mouse_release(int n)
{
	return FALSE;
}

int text_mouse_right_press(int n)
{
	return FALSE;
}

int text_mouse_error(int n)
{
	return FALSE;
}

int execute_menu(int n)
{
 FILEBUF *fp=cbfp;
 // MESG("execute_menu: n=%d b_flag=%X",n,fp->b_flag);
	if(fp->b_flag & FSDIRED && fp->b_flag & FSNLIST) {
		if(fp->b_flag & FSNLIST) {
			start_menu = &m_sort;	/* or a dir menu !!!!  */
			drv_show_menu_popup(popup_sort_menu);
		} else {
			if (cwp->selection) drv_show_menu_popup(popup_select_on);
			else drv_show_menu_popup(popup_select_off);
		};
	} 
#if	TNOTES0
	else if(fp->b_flag & FSNOTES) {
	 	start_menu=&m_notes_tag;
		drv_show_menu_popup(popup_tag_menu);
	} else if(fp->b_flag & FSNLIST) {
	 	start_menu=&m_notes_tag;
	} 
#endif
	else {
			if (cwp->selection) drv_show_menu_popup(popup_select_on);
			else drv_show_menu_popup(popup_select_off);
	};

//	events_flush();
	set_update(cwp,UPD_MOVE);
//	update_screen(TRUE);
	return 1;
}

/* -- */
