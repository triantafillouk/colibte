
offs cashed_FLend(FILEBUF *fp,offs tp_offs,int reset)
{
 static offs end_offs=0;
 static offs start=-1;
 if(reset) {
	end_offs=FLineEnd(fp,tp_offs);	
	start=tp_offs;
	return end_offs;
 };
	if(tp_offs==start || !FBolAt(fp,tp_offs) )
	{
		// MESG("	tp %ld - end %ld",start,end_offs);
		//end_offs=cline_end;
	} else {
		end_offs=FLineEnd(fp,tp_offs);	
		start=tp_offs;
	}
	return end_offs;
}


offs wrap_llen(WINDP *wp,offs tp_offs)
{
 int llen=0;
 utfchar uc;
	offs o=tp_offs;
	while(!FEolAt(wp->w_fp,o) && llen<wp->w_width){
		o=FUtfCharAt(wp->w_fp,o,&uc);
		llen++;
	};
 return llen;
}


offs vt_wrap_line(WINDP *wp, offs tp_offs)
{
 // int c=0;
 int i=0;
 num col;
 num llen=0;	/* Number of characters in the line */
 vchar *v_text;
 int i0;
 FILEBUF *fp = wp->w_fp;; 
 int c1;
 int fcol=COLOR_FG;
 int bcol=COLOR_BG;
 offs s1,s2,ptr1,ptr2;
 offs cur_lend=0;
 char *info_mask="";
 char *vtlm=NULL;
 static num  vtla=0; 	 // mask allocated bytes
 int real_line_len=0;	// real line len
 num line_num = wp->tp_hline->line + wp->vtrow;
 int line_start=0;

 v_text = init_vt_line(wp);

	col = 0;
	s1=tp_offset(wp->w_smark);
	s2=tp_offset(wp->w_emark);
	if(s2<s1) { 	/* reverse it  */
		ptr2=s1;
		s1=s2;s2=ptr2;
	};

	// ptr1 = FLineBegin(fp,tp_offs);
	ptr1=tp_offs;

	cur_lend = cashed_FLend(fp,tp_offs,0);

	ptr2=cur_lend;
	if(FBolAt(fp,ptr1)) line_start=1;
	// set_utf8_error(0);	/* reset utf_error on each line!!  */
	if(wp->w_fp->b_lang == 0 ) {
		llen=wrap_llen(wp,tp_offs);
		if(utf8_error()) llen=ptr2-ptr1;
	} else {
		llen = ptr2 - tp_offs;
	};
	// MESG("	wline: %d ptr=%ld",wp->vtrow,ptr1);
	if(FBolAt(fp,ptr1))	init_line_highlight(wp);
	
 	if(!slang || hquotem==0) 
		line_bcolor=wp->w_bcolor;

	if(utf8_error()) {
		hquotem |= H_UTFERR;
	} else {
		hquotem &= ~H_UTFERR; 
	};


	vtlm=vt_mask_init(llen);

	if(llen>=0 && wp->w_infocol) 
	{
		info_mask=set_info_mask(wp,ptr1,line_num);
	};
	line_sep=0;
	if(syntaxh) {
	//	create mask for the whole line without tabs or special characters
		if(llen>wp->w_width) llen=wp->w_width;
		real_line_len=color_mask_create(wp,ptr1,llen,vtlm,vtla);
 	};	// syntaxh
//	find the offset of the first column
	col=0;
	i=0;	

// ----------------------------------------------
//	character creation

	draw_info_mask(wp,ptr1,info_mask);

	// line_sep=0;
	for (; i <  llen && wp->vtcol < wp->w_width+wp->w_infocol; i++) 
	{	// this is the on screen shown area of the line
		// int display_size=0;
		// show selection
		if(wp->selection) {
			if(ptr1>=s1 && ptr1 < s2) 
			{
				set_selection(true);
			} else {
				set_selection(false);
			};
			if(wp->selection==REGION_COLUMN){
				if(s1<cur_lend && s2>tp_offs) set_selection(true);
				if(((wp->vtcol-wp->w_infocol) < col0) || ((wp->vtcol - wp->w_infocol) >= col1)) set_selection(false);
			};
		};
		if(FEolAt(fp,ptr1)) break;
		if(fp->b_lang == 0 && !utf8_error()) 
		{
			utfchar uc;
			memset(uc.uval,0,8);
			ptr1 = FUtfCharAt(fp,ptr1,&uc);
			vput_normalize(wp,uc);
		} else {
			vtputc(wp, FCharAt(fp,ptr1++));
		}
	};
	col=wp->vtcol;
	// show a line separator (when highlight_md)
	if(line_sep) {
		VIDEO *vp=wp->vs[wp->vtrow];
		for(wp->vtcol=wp->w_infocol;wp->vtcol< wp->w_width;wp->vtcol++){ 
			vchar *vc = vp->v_text+wp->vtcol;

			svwchar(vc,&double_hline,vc->bcolor,COLOR_COMMENT_FG);	/* double line separator */
		};
		// line_sep=0;
	};

	{
		VIDEO *vp=wp->vs[wp->vtrow];
		vchar *vc = vp->v_text;
		if(line_start) svwchar(vc,&first_l,vc->bcolor,COLOR_FG);
		else svwchar(vc,&next_l,vc->bcolor,COLOR_FG);
	}

	/* highlight according to evaluated mask */
	if(syntaxh && slang)
	{
		for(i0=wp->w_infocol;i0< wp->w_width+wp->w_infocol;i0++){
			if(i0 > real_line_len+wp->w_infocol) break;
			c1=vtlm[i0-wp->w_infocol];

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
		for(i0=0;i0<wp->w_width+wp->w_infocol;i0++) {
			if(v_text[i0].uval[0]>128) 
			{
				if(v_text[i0].uval[1]==0) {
					 svcolor(v_text+i0,v_text[i0].bcolor,COLOR_WORD2_FG);
				};
			};
		}
	};
	// MESG("end line: >");

	if(ptr1==cur_lend){
	  if(col==wp->w_width+1) return ptr1;
	  else return ptr1+wp->w_fp->EolSize;
	} else return ptr1;

}

/*	upd_all_wrap_lines:	update all the lines in a window */
void upd_all_wrap_lines(WINDP *wp,char *from)
{
	register offs lp_offs;	/* offset of line to update */
	register int sline;	/* physical screen line to update */
	int head=0;

	if(noupdate) return;
	if(wp->vs == NULL) return;
	if(wp->w_fp == NULL) return;
	/* search down the lines, updating them */
	lp_offs = tp_offset(wp->tp_hline);
	MESG_time("#update_all_wrap_lines:window %d width=%d top offset=%ld", wp->id,wp->w_width, lp_offs);
	wp->w_fp->hl->h_update(wp);
	set_selection(0);

	// cline_end=FLineEnd(wp->w_fp,lp_offs);
	// cstart_offset=lp_offs;
	cashed_FLend(wp->w_fp,lp_offs,1);
	highlight_restore_state(wp,0);
	// MESG("# update_all_wrap_lines: wrap=%d window %d",is_wrap_text(wp->w_fp),wp->id);
	for(sline=head;sline < wp->w_ntrows;sline++) 
	{
		/* and update the virtual line */
		wp->vs[sline]->v_flag =1;
		vtmove(wp,sline, 0);
		if (lp_offs <= FSize(wp->w_fp)) { // if not at the end of file
		/* if we are not at the end */
			if(wp->selection==0) set_selection(false);
			// MESG("	vt_wrap_line: %d at %ld",sline,lp_offs);
			// cline_end=cashed_FLend(wp->w_fp,lp_offs);
			lp_offs=vt_wrap_line(wp,lp_offs);
			/* we must update the column selection here */
			if(wp->selection) set_selection(false);
		};
		
		vteeol(wp,0,0);
	}

	if(wp->w_flag & UPD_STATUS) {
		wp->w_flag=UPD_STATUS;
	}
	else wp->w_flag=0;
	set_draw_flag(wp,"upd_all_wrap_lines");
	// MESG_time("#update_all_wrap_lines: end");
	highlight_restore_state(wp,0);	// set highlight to the top line!
}
