
offs cashed_FLend(FILEBUF *fp,offs tp_offs,int reset)
{
 static offs end_offs=0;
 static offs start=-1;
 if(reset) {
	end_offs=FLineEnd(fp,tp_offs);	
	start=tp_offs;
	return start;
 };
	if(tp_offs==start || !FBolAt(fp,tp_offs) )
	{
		// MESG("	tp %ld - end %ld",start,end_offs);
		// end_offs=cline_end;
	} else {
		end_offs=FLineEnd(fp,tp_offs);	
		start=tp_offs;
	}
	return end_offs;
}

offs cached_llen(FILEBUF *fp,offs tp_offs,int reset)
{
 static offs llen=0;
 static num start=-1;
 if(reset){
 	llen = utf_FLineLen(fp,tp_offs);
	start=tp_offs;
 };
 if(tp_offs==start || !FBolAt(fp,tp_offs))
 {
	// MESG("	tp %ld - llen %ld",start,llen);
 } else {
 	llen = utf_FLineLen(fp,tp_offs);
	start=tp_offs;
 };
 return llen;
}


offs vt_wrap_line(WINDP *wp, offs tp_offs)
{
 int c=0;
 utfchar uc;
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
 char *info_mask;
 char *vtlm=NULL;
 static num  vtla=0; 	 // mask allocated bytes
 int real_line_len=0;	// real line len
 int num_columns=0;	/* columns of line number shown  */
 num line_num = wp->tp_hline->line + wp->vtrow;
 int line_start=0;
 num_columns=wp->w_infocol;

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
	if(BolAt(ptr1)) line_start=1;
	// set_utf8_error(0);	/* reset utf_error on each line!!  */
	if((wp->w_fp->b_lang == 0 )) {
	    llen=cached_llen(wp->w_fp,tp_offs,0);
		// llen = utf_FLineLen(wp->w_fp,tp_offs);
		if(utf8_error()) llen=ptr2-ptr1;
	} else {
		llen = ptr2 - tp_offs;
	};

	if(BolAt(ptr1))	init_line_highlight(wp);
	
 	if(!slang || hquotem==0) 
		line_bcolor=wp->w_bcolor;

	if(utf8_error()) {
		hquotem |= H_UTFERR;
	} else {
		hquotem &= ~H_UTFERR; 
	};

	vtlm=vt_mask_init(llen);

	if(llen>=0 && num_columns) 
	{
		info_mask=set_info_mask(wp,ptr1,line_num);
	};
	line_sep=0;
	if(syntaxh) {
	//	create mask for the whole line without tabs or special characters
		offs p=ptr1;
		
		// col=0;
		for(i=0;i<llen && i< wp->w_width;i++) {
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
			if(c<32) c='1';

			if(wp->w_ntcols > vtla) vtlm=vt_mask_init(vtla+wp->w_ntcols);
			while(real_line_len<wp->w_ntcols)
			{
				vtlm[real_line_len++]=c;
			};

		};
		vtlm[real_line_len]=0;
		// if(col>=vtla) MESG("vtlm[%d]=[%s] col=%d vtla=%d",wp->vtrow,vtlm,col,vtla);

		int canstart=1;
		for(i0=0 ;i0< real_line_len;i0++) {
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
 	};	// syntaxh
	
//	find the offset of the first column
	col=0;
	i=0;	

// ----------------------------------------------
//	character creation
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
	// MESG("vt_wrap_line: num_columns=%d vtcol=%d w_infocol=%d",num_columns,wp->vtcol,wp->w_infocol);

	for (; i <  llen && wp->vtcol < wp->w_width+wp->w_infocol; i++) 
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
				if(((wp->vtcol-num_columns) < col0) || ((wp->vtcol - num_columns) >= col1)) set_selection(false);
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
			if(wp->vtcol==wp->w_width+wp->w_infocol-1 && display_size>1) { // do not show last double width character!
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
	col=wp->vtcol;
	// show a line separator (when highlight_md)
	if(line_sep) {
		VIDEO *vp=wp->vs[wp->vtrow];
		for(wp->vtcol=num_columns;wp->vtcol< wp->w_width;wp->vtcol++){ 
			vchar *vc = vp->v_text+wp->vtcol;

			svwchar(vc,&double_hline,vc->bcolor,COLOR_COMMENT_FG);	/* double line separator */
		};
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
		for(i0=num_columns;i0< wp->w_width+wp->w_infocol;i0++){
			if(i0 > real_line_len+num_columns) break;
			c1=vtlm[i0-num_columns];

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

	// MESG("	col=%d w=%d cur_lend=%ld ptr1=%ld",col,cwp->w_width,ptr1);
	if(ptr1==cur_lend){
	  if(col==cwp->w_width+1) return ptr1;
	  else return ptr1+wp->w_fp->EolSize;
	} else return ptr1;
}

/*	upd_all_wrap_lines:	update all the lines in a window */
void upd_all_wrap_lines(WINDP *wp,char *from)
{
	register offs lp_offs;	/* offset of line to update */
	register int sline;	/* physical screen line to update */
	int head=0;
	MESG("Upd_all_wrap_lines:");
	if(noupdate) return;
	if(wp->vs == NULL) return;
	if(wp->w_fp == NULL) return;

	wp->w_fp->hl->h_update(wp);
	set_selection(0);

	/* search down the lines, updating them */
	lp_offs = tp_offset(wp->tp_hline);

	// cline_end=FLineEnd(wp->w_fp,lp_offs);
	// cstart_offset=lp_offs;
	cashed_FLend(wp->w_fp,lp_offs,1);

	for(sline=head;sline < wp->w_ntrows;sline++) 
	{
		/* and update the virtual line */
		wp->vs[sline]->v_flag =1;
		vtmove(wp,sline, 0);
		if (lp_offs <= FSize(wp->w_fp)) { // if not at the end of file
		/* if we are not at the end */
			if(cwp->selection==0) set_selection(false);

			// cline_end=cashed_FLend(wp->w_fp,lp_offs);
			lp_offs=vt_wrap_line(wp,lp_offs);
			/* we must update the column selection here */
			if(cwp->selection) set_selection(false);
		};
		
		vteeol(wp,0,0);
	}

	if(wp->w_flag & UPD_STATUS) {
		wp->w_flag=UPD_STATUS;
	}
	else wp->w_flag=0;
	set_draw_flag(wp,"upd_all_wrap_lines");
	getwquotes(wp,0);	// set highlight to the top line!
}
