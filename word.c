/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

*/
/* word or paragraph related commands */

#include	"xe.h"

/*	list of recognized environment variables	*/
int infword(int c);
extern FILEBUF *cbfp;

// set selection on current word
void select_current_word()
{
num o,o_start,o_end;
	o=Offset();
	while(cbfp->hl->c_inword(CharAt(o))) {o--;};o++;
	o_start=o;
	while(cbfp->hl->c_inword(CharAt(o))) {o++;};
	o_end=o;
	// Mark word under cursor
	textpoint_set(cwp->w_smark,o_start);
	textpoint_set(cwp->w_emark,o_end);
	textpoint_set(cwp->tp_current,o_end);
	cwp->selection=1;
}

int prev_word(int n)
{
 int p;
	if (n < 0)	return (next_word(-n));
	if (prev_character(1) == FALSE) return (0);
	while (n--) {
		while (inword() == FALSE) {
			p=prev_character(1);
			if (p == FALSE){
				return (0);
			}
		}
		while (inword() != FALSE) {
			p=prev_character(1);
			if (p == FALSE) {
				return (1);
			};
		}
	};
	next_character(1);
	return(OK_RSTGOAL);
}

/*
 * Move the cursor forward by the specified number of words. All of the motion
 * is done by "next_character". Error if you try and move beyond the buffer's end.
 */
int next_word(int n)
{
	if (n < 0)	return (prev_word(-n));
	while (n--) {
//		MESG("next_word: check skip current word");
		while (inword() == TRUE) {
			if (next_character(1) == FALSE)	return (FALSE);
		}

//		MESG("next_word: check skip not in word");
		while (inword() == FALSE) {
			if (next_character(1) == FALSE)
				return (FALSE);
		}
	}
	return(OK_RSTGOAL);
}

// goto_eow(n)
// goto to end of current word
// if n==0 then no move if at end of a word
int goto_eow(int n)
{
	if (n < 0)	return (prev_word(-n));
	if(n==0) {
		while (inword() == TRUE) {
			if (next_character(1) == FALSE)
				return (FALSE);
		}
	} else {
	while (n--) {
		while (inword() == FALSE) {
			if (next_character(1) == FALSE)
				return (FALSE);
		}
		while (inword() == TRUE) {
			if (next_character(1) == FALSE)
				return (FALSE);
		}
	}
	}
	return(OK_RSTGOAL);
}

/*
 * delete words. 
 * Bound to "M-D".
 */
int delete_word(int  n)
{
	offs curoffs;
	register int c;		/* temp char */
	long size;		/* # of chars to delete */
	
	if(dont_edit()) return FALSE;
	if (n < 1)	return (FALSE);

	/* save the current cursor position */
	curoffs=Offset();
	/* figure out how many characters to give the axe */
	size = 0;

	/* get us into a word.... */
	while (inword() == FALSE) {
		if (next_character(1) == FALSE)
			return(FALSE);
		++size;
	}

	if (n == 0) n=1;
	/* here we deleted the special case whith n==0 */

		/* skip n words.... */
	while (n--) {
	
			/* if we are at EOL; skip to the beginning of the next */
			while(Eol()) {
				set_Offset(FNextLine(cbfp,Offset()));
				size+=cbfp->EolSize;
			};
			/* move forward till we are at the end of the word */
			while (inword() == TRUE) {
				if (next_character(1) == FALSE)
					return(FALSE);
				++size;
			}
	
			/* if there are more words, skip the interword stuff */
			if (n != 0)
				while (inword() == FALSE) {
					if (next_character(1) == FALSE)
						return(FALSE);
					++size;
				}
	}
	
		/* skip whitespace and newlines */
		while(Eol() || ((c = Char()) == CHR_SPACE) || (c==CHR_TAB)) {
				if (next_character(1) == FALSE) break;
				++size;
		};
	/* restore the original position and delete the words */
	set_Offset(curoffs);
	if(DeleteBlock(0,size))	
	{
		set_update(cwp,UPD_EDIT|UPD_MOVE);
		return OK_RSTGOAL;
	} else return FALSE;
}

/*
 * Kill backwards by "n" words. Move backwards by the desired number of words,
 * Bound to "M-Rubout" and to "M-Backspace".
 */
int delbword(int n)
{
	long size=0;
	offs o;

	if(dont_edit()) return FALSE;
	if (n < 1)	return (FALSE);
	textpoint_set(cwp->w_emark,Offset());

	if(Offset()<1) return FALSE;
	if(Offset()==1) return del_prev_char(1);
	o=Offset()-2;

	// if more than one space remove spaces
	if(isspace(CharAt(Offset()-1)) && isspace(CharAt(Offset()-2))) {
		size=1;
		while(isspace(CharAt(o))) {
			size++;
			if(o<1) break;
			o--;
		};
		size--;
		if(!DeleteBlock(size,0)) return FALSE;
		set_update(cwp,UPD_EDIT|UPD_MOVE);
		return(TRUE);
	};

	o=Offset()-1;size=1;	// check for quoted string!
	if(CharAt(o)=='"') {
	  if(o>0){
	  o--;size++;
	  while(CharAt(o-1)!='"'){
	  	if(o>=0) size++;
		if(o<1)break;
		o--;
	  };size++;
	  };
	}
	else {
	o=Offset()-2;size=1;
	while(n--) {
	if(isspace(CharAt(o))) {o--;size++;};
	while(cbfp->hl->c_inword(CharAt(o))) {
		if(o>=0) size++;
		if(o<1) break;
		o--;
	};
	};
	};
	if(!DeleteBlock(size,0)) return FALSE;

	set_update(cwp,UPD_EDIT|UPD_MOVE);
	return(TRUE);
}

/*
 * Return TRUE if the current character is a character that is considered to be
 * part of a word. 
 */
int inword()
{
	register int	c;
	if(Eol()) return (FALSE);
	c=Char();
//	MESG("inword:c=%d %c",c,c);
	return cbfp->hl->c_inword(c);
}


// fillpara: FIXME later. no unified undo
// fillpara: FIXME later. change the logic to more efficient
int fillpara (int n)	/* Fill the current paragraph according to the current fill column	*/
{
	int c,i;			/* current char durring scan	*/
	utfchar uc;
	int wordlen;		/* length of current word	*/
	int utf_wordlen;
	int clength;		/* position on line during fill	*/
	int eopflag;		/* Are we at the End-Of-Paragraph? */
	int first_word;	/* first word needs no space	*/
	offs eop_offs;		/* pointer to line just past EOP */
	offs bop1;
	num plen1,plen2;
	int	maxcol;
	char wbuf[MAXLLEN];		/* buffer for current word	*/
	char *pbuf,*p0;

	if(dont_edit()) return FALSE;
	if (bt_dval("fillcol") == 0) set_bt_num_val("fillcol",76);
	maxcol = bt_dval("fillcol");

	if(n==2||n==0) maxcol += bt_dval("lmargin");
	/* record the pointer to the line just past the EOP */
	/* To work ok I must be in the middle of the paragraph */
	goto_bop (1);
	bop1=Offset();
	goto_eop (1);
	plen1=Offset()-bop1;
	plen2=(plen1/(bt_dval("fillcol")-bt_dval("lmargin")))*(4+bt_dval("lmargin")+bt_dval("fillcol"));
	p0=pbuf=(char *)malloc(plen2+20);	// maximum size of the new paragraph

	eop_offs = FNextLine(cbfp,Offset());
	/* and back top the beginning of the paragraph */
	goto_bop(1);
	/* initialize various info */
	clength=tp_col(cbfp->tp_current); // current column
	if(clength && CharAt(LineBegin(Offset())=='\t')) clength = 8;

	wordlen = 0;
	utf_wordlen = 0;
	/* scan through lines, filling words */
	first_word = TRUE;
	eopflag = FALSE;
	for(i=0;i<bt_dval("lmargin");i++) *p0++ = ' ';
	while (!eopflag) {
		/* get the next character in the paragraph */
		if(Eol()) {
			c=' ';
			if(FNextLine(cbfp,Offset())>=eop_offs) eopflag = TRUE;
		} else {
			FUtfCharAt(cbfp,Offset(),&uc);
			c=uc.uval[0];
		}
		if(c==12) { eopflag=TRUE;continue;}
		/* if not a separator, just add it in */
		if (c != ' ' && c != '\t') {
			if (wordlen < MAXLLEN - 1) {
				int i;
				for(i=0;i<4;i++) {
					if(uc.uval[i]) wbuf[wordlen++] = uc.uval[i];
				};
				utf_wordlen ++;
			};
		} else if (wordlen) {
			/* at a word break with a word waiting */
			/* calculate new length with word added */
			if (clength + 1 + utf_wordlen <= maxcol-bt_dval("lmargin")) 
			{
				/* add word to current line */
				if (!first_word) {
					*p0++=' ';
					++clength;
				}
				first_word = FALSE;
			} else {
				/* start a new line */
				if(cbfp->b_mode & EMDOS) {
					*p0++='\r';
					*p0++='\n';
				} else {
					*p0++='\n';
				};
				clength=0;
				
				// create left margin
				for(i=0;i<bt_dval("lmargin");i++) *p0++ = ' ';
			}

			/* and add the word in in either case */
			for(i=0;i< wordlen;i++) *p0++ = wbuf[i];
			clength += utf_wordlen;
 			wbuf[wordlen]=0;
			*p0 = 0;
			wordlen = 0;
			utf_wordlen=0;
		};
		next_character(1);
	};
	/* add a line at the end of the paragraph */
	if(cbfp->b_mode & EMDOS) {
		*p0++='\r';
		*p0++='\n';
	} else {
		*p0++='\n';
	};


	/* and add a last newline for the end of our new paragraph */
	// remove old paragraph
	set_Offset(bop1);
	DeleteBlock(0,plen1);

	// insert new one
	insert_string(cbfp,pbuf,p0-pbuf);
	free(pbuf);
	set_update(cwp,UPD_WINDOW);
	return(OK_RSTGOAL);
}

/*	wordcount:	count the # of words in the marked region,
			along with average word sizes, # of chars, etc,
			and report on them.			*/
int countwords(int n) // ignored arguments
{
	offs curoffs;
	offs start_offset,end_offset;
	long size;		/* size of region left to count */
	register int ch;	/* current character to scan */
	register int wordflag;	/* are we in a word now? */
	register int lastword;	/* were we just in a word? */
	long nwords;		/* total # of words */
	long nchars;		/* total number of chars */
	long schars;		/* spaces and delimiters */
	int region_lines;		/* total number of lines in region */
	int avgch;		/* average number of chars/word */

	/* make sure we have a region to count */
	start_offset=tp_offset(cwp->w_smark);
	end_offset=tp_offset(cwp->w_emark);
	size=end_offset-start_offset;
	if(size<0) { 
		size=-size;
		curoffs=start_offset;
		start_offset=end_offset;
		end_offset=curoffs;
	};

	/* count up things */
	lastword = FALSE;
	nchars = 0L;
	nwords = 0L;
	schars = 0L;
	region_lines = 0;
	for(curoffs=start_offset;curoffs<end_offset; )
	{
		/* get the current character */
		if(FEolAt(cbfp,curoffs)) {
			ch='\n';
			curoffs=FNextLine(cbfp,curoffs);
			region_lines++;
		} else {
			ch=CharAt(curoffs++);
		};

		wordflag = cbfp->hl->c_inword(ch);
		if (wordflag == TRUE && lastword == FALSE)
			++nwords;
		if(!wordflag) ++ schars;
		lastword = wordflag;
		++nchars;
	}
	/* and report on the info */
	if (nwords > 0L) {
		avgch = (int)((100L *  (nchars - schars)) / nwords);
	}
	else
		avgch = 0;
	msg_line("Words %ld Chars %ld Lines %d Avg chars/word %.2f",
		nwords, nchars, region_lines + 1,(((float)avgch)/100));
	setmark(0);
	return(TRUE);
}

int words_in_line(offs o,int *nspace)
{
 offs l=LineBegin(o);
 int w=0;
 int c;
 int prev_space=1,s1=0;
 // skip starting space

 while(!EolAt(l)) {
	c=CharAt(l);
	if(c==CHR_TAB || c==' ') {
		prev_space=TRUE;
	} else {
		if(prev_space) w++;
		prev_space=FALSE;
		s1++;
	};
	l++;
 };
 if(prev_space) w--;
 *nspace=s1;
 return w;
}

int linefill( int indent)
/* ident
   = 0 > do not insert spaces at the beginning
   = 1 > insert lmargin spaces at the beginning
*/
{
 int c,i;
 int new_space,spaces;
 int x1,nospace;
 int big_space;
 int right=(int)bt_dval("fillcol");
 int left=(int)bt_dval("lmargin");

 offs curoffs,o1;
 num len,new_len;
 char *lbuf,*lo;

 /* delete double spaces and compute words = w */
 len=LineEnd(Offset())-LineBegin(Offset());
 lo=lbuf=(char *)malloc(4*right+1);	/* max for utf chars  */
 
 set_Offset(LineBegin(Offset()));
 o1=curoffs=Offset();

 spaces=words_in_line(curoffs,&nospace)-1;

 if(spaces<1) return FALSE;
 if(nospace > right-left) return FALSE;
 // put left margin
 if(indent) {
 	for(i=0;i<left;i++) *lo++ =' ';
	new_space = right - left-nospace;
 } else new_space = right - nospace;;

 x1=new_space/spaces; // mean space len
 big_space=new_space-x1*(spaces-1);	// first or last space

// go to first word
 while(!EolAt(o1)) {
 	if((c=CharAt(o1))!=' ' && c!=CHR_TAB) break;
 	o1++;
 };
 while(!EolAt(o1)) {
 	if((c=CharAt(o1))==' ' || c==CHR_TAB) { // in every space
		spaces--;
		if(spaces==0) {	// last space
			for(i=0;i<big_space;i++) *lo++ = ' ';
		} else {
			for(i=0;i<x1;i++) *lo++ = ' ';
		};
		o1++;
		while((c=CharAt(o1)==' '||c==CHR_TAB)) o1++;
	} else {
		utfchar uc;
		int i;
		o1=FUtfCharAt(cbfp,o1,&uc);	
		for(i=0;i<4;i++) {
			if(uc.uval[i]) *lo++ = uc.uval[i];
			else break;
		};
	};
 };
 *lo=0;
 new_len=lo-lbuf;

 DeleteBlock(0,len);

 insert_string(cbfp,lbuf,new_len);
 free(lbuf);
 set_update(cwp,UPD_EDIT|UPD_MOVE);
 return 0;
}


/* check if enough tabs at the beginning of the line */
/* it must be smart enough to find correct spacing */
int check_tabs(offs tpo, int ntabs)
{
 int i,c,j;
 offs lbegin;
 lbegin=LineBegin(tpo);
 for(i=0;i<ntabs;i++) {
	c=CharAt(lbegin+i);
	if(c=='\t') { 
		continue;
	};
	if(c==' ') { /* check for corresponding spaces */
		for(j=1;j<tabsize;j++) {
			c=CharAt(lbegin+i+j);
			if(c!=' ') return FALSE;
		};
	} else return FALSE;
 };
 return TRUE;
}

/* move line,s or region to the right or to the left n number of times */
/* for negative n then to the left. If 0 then one tab */
/* If  selection is  on then  for the  selected lines  otherwise for the
   current line 
*/
int insert_tab( int n)
{
 offs back_offset,curoffs;
 if(n<0) {
 /* in case of a selection and n==-1 check if enough tabs at the start of 
 the lines else check only current line.
 */
	return remove_tab(-n);
 } else {
 	if(cwp->selection) {
		if( cwp->selection == REGION_COLM) {
			if(cbfp->hl->notab) {
				int ispaces = tabsize-tp_col(cbfp->tp_current)%(tabsize);
				insert_chr(ispaces, CHR_SPACE);
			} else insert_chr(1,CHR_TAB);
			return TRUE;
		} else {
			if(tp_offset(cwp->w_smark) > tp_offset(cwp->w_emark)) {
				set_Offset( tp_offset(cwp->w_smark));
				textpoint_set(cwp->w_smark, LineBegin(tp_offset(cwp->w_emark)));
				textpoint_set(cwp->w_emark, Offset());
			} else {
				textpoint_set(cwp->w_smark, LineBegin(tp_offset(cwp->w_smark)));
			};
			back_offset = Offset();
			curoffs = tp_offset(cwp->w_smark);
			while(curoffs < FSize(cbfp)) {
				int ispaces=1;
				set_Offset(curoffs);
				ToLineBegin();
	
				if(cbfp->hl->notab) {
					ispaces = tabsize-tp_col(cbfp->tp_current)%(tabsize);
					insert_chr(ispaces, CHR_SPACE);
				} else insert_chr(1,CHR_TAB);
	
				if(back_offset>=curoffs) back_offset +=ispaces;
				curoffs=FNextLine(cbfp,curoffs);
				if(curoffs>tp_offset(cwp->w_emark)) break;
			};
			set_Offset(back_offset);
			return TRUE;
		};
	} else {
		/* insert tab */
		if(cbfp->hl->notab) {
			int ispaces = tabsize-tp_col(cbfp->tp_current)%(tabsize);
			return(insert_chr(ispaces, CHR_SPACE));
		} else {
			back_offset = Offset();
			insert_chr(1,CHR_TAB);
			back_offset++;
		}
	};	
 };
 set_Offset(back_offset);
 return OK_RSTGOAL;
}

/* remove tab/tabs from beginning of line */
/*
	No move on the left if one of the lines have not enough space at the
	beggining.
*/
int remove_tab(int n)
{
 int i,j;
 offs back_offset,curoffs;

 if(cwp->selection) {
	back_offset=curoffs=Offset();
	if(curoffs>tp_offset(cwp->w_smark)) {
		curoffs=tp_offset(cwp->w_smark);
	};
	if(tp_offset(cwp->w_smark) > tp_offset(cwp->w_emark)) {
		offs end_offset=tp_offset(cwp->w_smark);
		set_Offset( end_offset);
		textpoint_set(cwp->w_smark, curoffs);
		textpoint_set(cwp->w_emark, end_offset);
	};
	while(curoffs< FSize(cbfp)) {
		curoffs=FNextLine(cbfp,curoffs);
		if(curoffs>tp_offset(cwp->w_emark)) break;
	};
	/* delete tabs from each line */
	curoffs=LineBegin(tp_offset(cwp->w_smark));

	while(curoffs< FSize(cbfp)) {
		for(i=0;i<n;i++) {
			set_Offset(LineBegin(curoffs));
			
			if(CharAt(LineBegin(curoffs))=='\t') {
				forw_delete(1L);
                if(back_offset>curoffs) back_offset--;
			} else { // we have spaces here, delete them
				for(j=0;j<tabsize;j++) {
					if(CharAt(curoffs)!=' ') break;
					forw_delete(1L);
					if(back_offset>curoffs) 
					{
						back_offset--;
					};
				};
			};
		};
		curoffs=FNextLine(cbfp,curoffs);
		if(curoffs>tp_offset(cwp->w_emark)) break;
	};
 } else { /* check current line */
	back_offset = Offset();
	curoffs=LineBegin(Offset());
 	if(!check_tabs(curoffs,n)) {
		msg_line("not enough tabs in the line!");
		return FALSE;
	} else { /* remove the tabs */
		/* move to line begin  */
		set_Offset( curoffs);
		for(i=0;i<n;i++) {
			if(CharAt(curoffs)=='\t') {
				forw_delete(1L);
				if(curoffs<back_offset) back_offset--;
			} else { // we have spaces here, delete them
				for(j=0;j<tabsize;j++) {
					if(CharAt(curoffs)==' ') {
						forw_delete(1L);
						if(curoffs<back_offset) back_offset--;
					};
				};
			};
		};
	}
 };
 set_Offset( back_offset);
 return TRUE;
}


/* Next word in current line. */
char *getnword()
{
 static char nw[MAXLLEN];
 byte b;
 offs col;
 int i;
 col=Offset();
 for(i=0;!EolAt(col);col++,i++){
 	b=CharAt(col);
	if(!cbfp->hl->c_inword(b)) break;
	nw[i]=b;
 };
 nw[i]=0;
 return(nw); 
}

/* get file name under cursor  */
char *getcurfword()
{
 static char nw[MAXLLEN];
 byte b;
 offs col;
 int i;
 int move_offset=0;
 int word_offset=0;
 if(macro_exec==MACRO_MODE2) {
 	strlcpy(nw,get_sval(),MAXLLEN);
	return(nw);
 };
 col=Offset();

 while(infword(CharAt(col))) col--;
 col++;
 word_offset=Offset()-col;

 for(i=0;!EolAt(col);i++,col++) {
 	b=CharAt(col);
	if(!infword(b)) break;
 	nw[i]=b;
	move_offset++;
 };
 nw[i]=0;
 next_character(move_offset-word_offset);
 return(nw); 
}

/* Current word in current line. */
char *getcurcword()
{
 static char nw[MAXLLEN];
 register int col;
 int i;
 byte b;
 col=Offset();

 if(col>LineBegin(col)) {
 	while(cbfp->hl->c_inword(CharAt(col))) col--;
	col++;
 };

 for(i=0;!EolAt(col);i++,col++) {
	b=CharAt(col);
	if(!cbfp->hl->c_inword(b)) break;
 	nw[i]=b;
 };
 nw[i]=0;
 return(nw); 
}


/* Character in a C type file word */
int c_incword(int c)
{
	// if we are in C file type then underscore,period are part of a word
	if (c=='_') return(TRUE);
	if (c=='.') return(TRUE);
	if (c>='a' && c<='z')
		return (TRUE);
	if (c>='A' && c<='Z')
		return (TRUE);
	if (c>='0' && c<='9')
		return (TRUE);
	if (c>=128 && c<=255) /* All utf chars as part of words */
		return (TRUE);
	return (FALSE);
}

/* Character in a C type file word */
int c_injword(int c)
{
	// if we are in C file type then underscore,period are part of a word
	if (c=='_') return(TRUE);
	if (c=='.') return(FALSE);
	if (c>='a' && c<='z')
		return (TRUE);
	if (c>='A' && c<='Z')
		return (TRUE);
	if (c>='0' && c<='9')
		return (TRUE);
	if (c>=128 && c<=255) /* All utf chars as part of words */
		return (TRUE);
	return (FALSE);
}

/* Character in a text type file word */
int c_in_txt_word(int c)
{
	if (c=='_'||c=='@') return(TRUE);
	if (c>34 && c<39) return(TRUE);
	if (c>='a' && c<='z')
		return (TRUE);
	if (c>='A' && c<='Z')
		return (TRUE);
	if (c>='0' && c<='9')
		return (TRUE);
	if (c>=128 && c<=255)  /* All utf chars as part of words */
		return (TRUE);
	return (FALSE);
}

/* Character in a Cobol type file word */
int c_incobol_word(int c)
{
	if (c=='-') return(TRUE);	/* hyphen is part of cobol words!  */
	if (c=='_') return(TRUE);
	if (c>='a' && c<='z')
		return (TRUE);
	if (c>='A' && c<='Z')
		return (TRUE);
	if (c>='0' && c<='9')
		return (TRUE);
	if (c>=128 && c<=255)  /* All utf chars as part of words */
		return (TRUE);
	return (FALSE);
}

/* inside file name type word  (all this is by convetion only!!!!, spaces could be allowed also!!) */
int infword(int c)
{
	if (c=='_'  || c=='\\' || c=='(' || c==')' ) return(TRUE);
	if (c>='a' && c<='z')
		return (TRUE);
	if (c>='A' && c<='Z')
		return (TRUE);
	if (c>=',' && c<='9')	/* ,-./ and numbers */
		return (TRUE);
	if (c>=128 && c<=255)  /* All utf chars as part of words */
		return (TRUE);
	return (FALSE);
}

/* -- */
