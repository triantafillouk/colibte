/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

*/
/* Seach routines  */

#include	"xe.h"

#define	PTBEG	0	/* Leave the point at the beginning on search	*/
#define	PTEND	1	/* Leave the point at the end on search		*/

typedef struct {
	char c;	/* character  */
	char l;	/* lower character  */
	char t;	/* flag  */
} REXP_PAT;

#define M_EOF		0
#define M_START		1
#define M_END		2
#define M_ANY		4
#define M_FIND		32

int found=0;

void set_current_file_buffer(FILEBUF *bp);
FILEBUF *current_file_buffer();

char	search_pattern[MAXLLEN];            /* Search pattern	*/
char	replace_pattern[MAXLLEN];			/* replacement pattern		*/
char	search_lowercase_pattern[MAXLLEN];		/* lower case search pattern  */
void MESG_time_start(const char *fmt, ...);
num prof_count=0;

unsigned int	matchlen = 0;	/* lenght of matched string  */
char		patmatch[MAXLLEN];	/* string that satisfies the search command.  */

/* start of match  */
offs match_offs;

int	rexp_scanner(REXP_PAT *patptr, int direction, int beg_or_end);
int	scanner(char *pattern, int direction, int beg_or_end);
#if	TNOTES
void update_tag_linecol();
#endif
void set_global(int beg_or_end,int direction);
int	replaces(int query,int f,num n);
int	nextch(int direction);
int regexp_str(char *pat, char *lower,REXP_PAT *regexp_pat,int dir );
int	amatch(char c1,REXP_PAT *pat,int direct);
int utf8charlen_nocheck(int ch);

void rvstrcpy(char *rvstr, char *str,int maxlen);
char *str2ctrl(char *);
int set_window_filebuf(WINDP *wp,FILEBUF *bp);
int drv_search_dialog(int f);

offs curoffs;		/* current offset in file  */
offs last_offs;		/* position of last replace  */
int found_bytes_len;		/* found string length of a reg expression  */
int found_char_len;	/* found character length of reg expression  */

int eq_ignorecase(int x,int y,int z)
{
 return (x==y || x==z);
}

int eq1(int x,int y,int z)
{
// MESG("	eq1(%X %X %X)",x,y,z);
 if(gmode_exact_case) return (x==y);
 else return (x==y || x==z);
}

void get_lowercase_string(char *lower, char *string)
{
#if	USE_GLIB
	gchar *tmp_string;
#else
	char *tmp_string;
#endif
	int size=strlen(string)+1;
	if(size>MAXLLEN) size=MAXLLEN;
#if	USE_GLIB
	tmp_string = g_utf8_strdown(string,-1);
#else
	tmp_string = lstr_to_lower(string);
#endif
	strlcpy(lower,tmp_string,size);
#if	USE_GLIB
	g_free(tmp_string);
#else
	free(tmp_string);
#endif
	// MESG("get_lowercase_string:[%s]->[%s]",string,lower);
}

void get_uppercase_string(char *upper, char *string)
{
#if	USE_GLIB
	gchar *tmp_string;
#else
	char *tmp_string;
#endif
	int size=strlen(string)+1;
	if(size>MAXLLEN) size=MAXLLEN;
#if	USE_GLIB
	tmp_string = g_utf8_strup(string,-1);
#else
	tmp_string = lstr_to_upper(string);
#endif
	strlcpy(upper,tmp_string,size);
#if	USE_GLIB
	g_free(tmp_string);
#else
	free(tmp_string);
#endif
	// MESG("get_uppercase_string:[%s]->[%s]",string,upper);
}

/*
 * forwsearch -- Search forward.  Get a search string from the user, and
 *	search for the string. 
 */
int forwsearch(num n)
{
	register int status = TRUE;
#if	TNOTES
	FILEBUF *fp=current_file_buffer();
#endif
//	MESG("forwsearch: n=%d macro_exec=%d",n,macro_exec);
	if(n==0) return FALSE;
	/* If n is negative, search backwards. */
	if (n < 0)	return(backsearch(-n));

	set_list_type(LSRC);

	if(!macro_exec) {
		if(drv_search_dialog(1)) return TRUE;
	};

	if ( (status = readpattern("Search", search_pattern, TRUE))==TRUE ) { 
	// MESG("forwsearch: pattern=%s",search_pattern);
		get_lowercase_string(search_lowercase_pattern,search_pattern);
#if	TNOTES
		if(fp->b_flag & FSNOTES) ToLineEnd();
#endif
		found=forw_stat(n);
	} else found=0;
	

#if	TNOTES
	if(fp->b_flag & FSNOTES) update_tag_linecol();
#endif
	if(found) { 
		set_hmark(1,"forw_search");
	}; 
	set_update(cwp,UPD_FULL);
	return(found);
}

char *get_line_at(FILEBUF *fb,offs offset);

/*
 * find_next -- Search forward for a previously acquired search string.
 */

int find_next(num n)
{
#if	TNOTES
 FILEBUF *fp=current_file_buffer();
#endif
	if (n < 0)	return(find_previous(-n));
	if (search_pattern[0] == 0)
	{
		msg_line("No pattern set");
		found=0;
		return FALSE;
	}

	/* Search for the pattern for as long as
	 * n is positive	 */
#if	TNOTES
	if(fp->b_flag & FSNOTES) {
		ToLineEnd();
	};
#endif
	found=forw_stat(n);
//	MESG("find_next: n=%d [%s] found=%d FSNVTAG=%X b_flag=%X",n,search_pattern,found,FSNVTAG,fp->b_flag);
#if	TNOTES
	if(fp->b_flag & FSNOTES) update_tag_linecol();
#endif
	if(found) {
		set_hmark(0,"find_next");
	};
	return(found);
}

/* 
	Search backwords 
 */
int backsearch(num n)
{
#if	TNOTES
 FILEBUF *fp=current_file_buffer();
#endif
	register int status = TRUE;
	if(n==0) return FALSE;

	/* If n is negative, search forwards. */
	if (n < 0)	return(forwsearch(-n));

	set_list_type(LSRC);
#if	TNOTES
	if(fp->b_flag & FSNOTES) ToLineBegin();
#endif
	if ((status = readpattern("Reverse search", &search_pattern[0], TRUE))==TRUE) 
	{
		get_lowercase_string(search_lowercase_pattern,search_pattern);
		set_hmark(1,"back_search");
		found=back_stat(n);
	} else found=0;
	

//	MESG("backsearch: n=%d [%s] found=%d b_flag=%X",n,search_pattern,found,fp->b_flag);
#if	TNOTES
	if(fp->b_flag & FSNOTES) update_tag_linecol();
#endif
	set_update(cwp,UPD_FULL);
	return(found);
}

/*
 * find_previous -- Reverse search for a previously acquired search string,
 */
int find_previous(num n)
{
#if	TNOTES
 FILEBUF *fp=current_file_buffer();
#endif
	if(n==0) return FALSE;
	if (n < 0)	return(find_next(-n));

//	MESG("find_previous: [%s]",search_pattern);
#if	TNOTES
	if(fp->b_flag & FSNOTES) ToLineBegin();
#endif
	if (search_pattern[0] == '\0')
	{
		msg_line("No pattern set");
		return FALSE;
	}
	found=back_stat(n);
//	MESG("find_previous: n=%d [%s] FSNVTAG=%X found=%d b_flag=%X",n,FSNVTAG,search_pattern,found,fp->b_flag);
#if	TNOTES
	if(fp->b_flag & FSNOTES) update_tag_linecol();
#endif
	return(found);
}


/*
 * scanner -- Search for a pattern in either direction.  If found,
 *	reset the "." to be at the start or just after the match string,
 *	and (perhaps) repaint the display.
 */
int	scanner(char *pattern, int direct, int beg_or_end)
/* patrn :  string to scan for */
/* direct :  which way to go.*/
/*beg_or_end: put point at beginning or end of pattern.*/
{
	register int	c;		/* character at current position */
	char *patptr;	/* pointer into pattern */

	char reverse_pattern[MAXLLEN];
	char *patrn;

	if(direct==FORWARD) patrn = pattern;
	else {
		rvstrcpy(reverse_pattern,pattern,MAXLLEN);
		patrn = reverse_pattern;
	};

	curoffs=Offset();
	match_offs=curoffs;
	/* Scan each character until we find the first character */
	
	while ( ( c = nextch(direct)) >=0)
	{
//		MESG("ck c=%c p=%c (%d %d)",c,patrn[0],c,patrn[0]);
		if(c==patrn[0])
		{
		/* Save the current position in case we match
		 * the search string at this point.*/
			match_offs=curoffs;

			/* Setup scanning pointers.*/
			patptr = &patrn[0];

			/* Scan through the pattern for a match.*/
			while (*++patptr != '\0')
			{
//				prof_count++;
				c = nextch(direct);
				if (c<0) goto fail;
				if (c != *patptr) 
				{
					nextch(direct ^ REVERSE);
					goto fail;
				};
			}

			/* A SUCCESSFULL MATCH!!!
			 * reset the global "." pointers
			 */
			// MESG("succcesfull match! case at %ld %ld",match_offs,curoffs);
			set_global(beg_or_end,direct);
			return TRUE;
		}
fail:;			/* continue to search */
//		MESG("fail: c=%ld m=%ld",curoffs,match_offs);
	}
//	MESG("not found: o=%lld",Offset());
	return FALSE;	/* We could not find a match */
}

int	scanner_ignorecase(char *pattern, char *lower_case_pattern, int direct, int beg_or_end)
/* patrn :  string to scan for */
/* direct :  which way to go.*/
/*beg_or_end: put point at beginning or end of pattern.*/
{
	register int	c;		/* character at current position */
	char *patptr;	/* pointer into pattern */
	char *lower_patptr;
	char *lower_pattern;
	char *lower_patrn;
	char reverse_lower_pattern[MAXLLEN];
	char reverse_pattern[MAXLLEN];
	char *patrn;

	lower_pattern = lower_case_pattern;
	if(direct==FORWARD) {
		patrn = pattern;
		lower_patrn = lower_pattern;	
	} else {
		rvstrcpy(reverse_pattern,pattern,MAXLLEN);
		rvstrcpy(reverse_lower_pattern,lower_pattern,MAXLLEN);
		patrn = reverse_pattern;
		lower_patrn = reverse_lower_pattern;
	};
	
	curoffs=Offset();
	match_offs=curoffs;
//	MESG("scanner_ignorecase: [%s] lower=[%s] o=%ld",pattern,lower_pattern,match_offs);
	/* Scan each character until we find the first character */
	
	while ( ( c = nextch(direct)) >=0)
	{
//		MESG("ck c=%c p=%c (%d %d)",c,patrn[0],c,patrn[0]);
		if (eq_ignorecase(c, patrn[0],lower_patrn[0]))
		{
		/* Save the current position in case we match
		 * the search string at this point.*/
			match_offs=curoffs;

			/* Setup scanning pointers.*/
			patptr = &patrn[0];
			lower_patptr = &lower_patrn[0];

			/* Scan through the pattern for a match.*/
			while (*++patptr != '\0')
			{
//				prof_count++;
				lower_patptr++;
				c = nextch(direct);
				if (c<0) goto fail;
				if (!eq_ignorecase(c, *patptr, *lower_patptr)) {
					nextch(direct ^ REVERSE);
					goto fail;
				};
			}

			/* A SUCCESSFULL MATCH!!!
			 * reset the global "." pointers
			 */
			// MESG("succcesfull match ignore case! at %ld %ld",match_offs,curoffs);
			set_global(beg_or_end,direct);
			return TRUE;
		}
fail:;			/* continue to search */
//		MESG("fail: c=%ld m=%ld",curoffs,match_offs);
	}
//	MESG("not found: o=%lld",Offset());
//	g_free(lower_pattern);
	return FALSE;	/* We could not find a match */
}

int	readpattern(char *prompt, char *apat, int srch)
{
	register int status;
	char instr[MAXLLEN+20];
	char sprompt[256];
	char default_pattern[MAXLLEN];
	int stat=0;

	strlcpy(default_pattern,get_last_in_list(LSRC),MAXLLEN);
	// MESG("readpattern:prompt[%s] pattern=[%s]",prompt,default_pattern);
	stat=snprintf(sprompt,256,"%s [%s]: ",prompt,default_pattern);
	if(stat>=256) MESG("prompt is > 256 chars");

	instr[0]=0;
	set_list_type(LSRC);
	if ((status = nextarg(sprompt, instr, MAXLLEN,true)) == TRUE)
	{
		if(instr[0]==0) strlcpy(instr,default_pattern,MAXLLEN); // if we press enter then get the previous pattern
		// MESG("readpattern:1 [%s]",apat);
		return(setpattern(apat,instr,srch));
	}
//	MESG("readpattern:2 [%s]",apat);
	return(status);
}

char *str2ctrl(char *s2)	/* ??????? SIZE  */
{
 static char st1[256];
 char *s1;
 int quotef=0;
 s1=st1; 
	for(;*s2;s2++) {
		if(quotef==0) if(*s2=='\\') { quotef=-1;continue;};
		if(*s2=='^') { quotef++;
			if(quotef) continue;
		};
		if(quotef>0) {
			if(*s2>96 && *s2<128) *s1 = *s2-96;
			else if(*s2>64 && *s2<96) *s1 = *s2-64;
		} else
		 *s1=*s2;
		quotef=0;
//		MESG("str2ctrl: s1=%d s2=%d",*s1,*s2);
		s1++;
	};
	*s1=0;
	 return st1;
}

// set search pattern to string
int	setpattern(char apat[], char *st,int srch)
{
	register int status;
	status=1;
//	MESG("setpattern:");
	if (gmode_reg_exp) strlcpy(apat, st,MAXLLEN);
	else  strlcpy(apat, str2ctrl(st),MAXLLEN);
	if(srch)
	{
		matchlen = strlen(apat);
		if (gmode_exact_case == 0) {
			get_uppercase_string(apat,apat);
		};
	};
	// reset pattern match
	patmatch[0]=0;
//	MESG("setpattern srch=%d,st=[%s] apat=[%s] exact=%d",srch,st,apat,gmode_exact_case);
	return(status);
}

/*
 * savematch -- We found the pattern?  Let's save it away.
 */

void savematch(int direction)
{
 num o;
 FILEBUF *fp=current_file_buffer();
 if(!gmode_reg_exp) {
 	strlcpy(patmatch,search_pattern,MAXLLEN);
	matchlen=strlen(search_pattern);
	return;
 };

 matchlen=found_bytes_len;
 if(matchlen>MAXLLEN-1) matchlen=MAXLLEN-1;	
 /* set the match_offs to the beginning */
 if(direction==FORWARD) o=match_offs-1;
 else o=match_offs-matchlen+1;
// MESG("savematch: o=%ld match_offs=%ld matchlen=%u found_bytes_len=%d",o,match_offs,matchlen,found_bytes_len);
 match_offs = o;

 /* get the matched text! */ 
	get_text_offs(fp,patmatch,o,matchlen);
	patmatch[matchlen]=0;
//	MESG("savematch: [%s] len=%d",patmatch,matchlen);
}

/*
 * rvstrcpy -- Reverse string copy.
 */
void rvstrcpy(char *rvstr, char *str,int maxlen)
{
	register int i;
	i=strlen(str);
	if(i>maxlen) i=maxlen;
	str += (i = strlen(str));

	while (i-- > 0)
		*rvstr++ = *--str;

	*rvstr = '\0';
}

/*
 * sreplace -- Search and replace.
 */
int sreplace(num n)
{
	return(replaces(FALSE,0, n));
}

/*
 * qreplace -- search and replace with query.
 */
int qreplace(num n)
{
	return(replaces(TRUE,0, n));
}

int replace_next(num n)
{
	return(replaces(FALSE,1,n));
}

/*
 * replaces -- Search for a string and replace it with another
 */
int	replaces(int query, int next_only, num n)
/* query: Query enabled flag */
{
	register int status;	/* success flag on pattern inputs */
	register int rlength;	/* length of replacement string */
	num numsub;	/* number of substitutions */
	num nummatch;	/* number of found matches */
	int nlflag;		/* last char of search string a <NL>? */
	int nlrepl;		/* was a replace done on the last line? */
	int c;			/* input char for query */
	FILEBUF *fp=current_file_buffer();

	offs current_offset=tp_offset(fp->tp_current);
	offs initial_offset=current_offset;
	
	offs last_offset;
	char lline[1024];
	char pattern_ori[MAXLLEN];
	int stat=0;

	REXP_PAT rpattern[MAXLLEN];	// regexp pattern
	// MESG("replaces:=== smark=%lld emark=%lld",tp_offset(cwp->w_smark),tp_offset(cwp->w_emark));

	offs  end_offset=tp_offset(fp->tp_text_end);
	if(cwp)
	if(cwp->selection){
		end_offset = tp_offset(cwp->w_emark);
		if(tp_offset(cwp->w_smark) > end_offset) {
			end_offset = tp_offset(cwp->w_smark);
			current_offset = tp_offset(cwp->w_emark);
			textpoint_set(cwp->tp_current,tp_offset(cwp->w_emark));
		} else {
			textpoint_set(cwp->tp_current,tp_offset(cwp->w_smark));
			end_offset = tp_offset(cwp->w_emark);
			current_offset = tp_offset(cwp->w_smark);
		}
	};
//	MESG("current_offset=%ld end_offset=%ld",current_offset,end_offset);

	last_offset = Offset();

	if(dont_edit()) return FALSE;
	if(!macro_exec){
		if(drv_search_dialog(2)) return TRUE;
	};
	/* Ask the user for the text of a pattern.*/
	set_list_type(LSRC);
	if(macro_exec) {
		nextarg("replace",search_pattern,MAXLLEN,true);
		num_expression();
		if (gmode_exact_case == 0) {
			get_uppercase_string(search_pattern,search_pattern);
		};

		matchlen=strlen(search_pattern);
		nextarg("with",replace_pattern,MAXLLEN,true);
	} else {
		if (
			(status = readpattern(
		    	(query == FALSE ? "Replace" : "Query replace"), search_pattern, TRUE)
			) != TRUE
		) { return(status); };
		strlcpy(pattern_ori,search_pattern,MAXLLEN);

	/* Ask for the replacement string.*/
	if ((status = readpattern("with", &replace_pattern[0], FALSE)) != TRUE)
		return(status);
	};

	/* Find the length of the replacement string.*/
	rlength = strlen(&replace_pattern[0]);
	get_lowercase_string(search_lowercase_pattern,search_pattern);
//	MESG("search_pattern=[%s] size=%d lower=[%s]",search_pattern,matchlen,search_lowercase_pattern);
//	MESG("replace_pattern=[%s] size=%d",replace_pattern,rlength);

	regexp_str(search_pattern,search_lowercase_pattern,rpattern,REVERSE);
	/* Set up flags so we can make sure not to do a recursive
	 * replace on the last line.
	 */
	nlflag = (search_pattern[matchlen - 1] == '\n');
	nlrepl = FALSE;
	matchlen=0;
	found_bytes_len = strlen(search_pattern);
	last_offs=-1;
	/* Save original . position, init the number of matches and
	 * substitutions, and scan through the file to replace all.
	 */

	initial_offset = Offset();
	numsub = 0;
	nummatch = 0;

	if(!query) {
		stat=snprintf(lline,1024,"Replacing '%s' with '%s' please wait ..",pattern_ori,replace_pattern);
		if(stat>=256) MESG("truncated 4");
		if(!execmd) msg_line(lline);
		// events_flush();
	};

	prof_count=0;
	stat=snprintf(lline,1024,"sreplace: %s with %s",search_pattern,replace_pattern);
	if(stat<256) MESG("truncated 5");
	MESG_time_start(lline);

	while ( (!next_only || n > nummatch) &&
		(nlflag == FALSE || nlrepl == FALSE) )
	{
		/* Search for the pattern.
		 * If we search with a regular expression,
		 * matchlen is reset to the true length of
		 * the matched string.
		 */
		if ( gmode_reg_exp )
		{
//			MESG("go for rexp_scanner!");
			if (!rexp_scanner(&rpattern[0], FORWARD, PTBEG))
				break;
		}
		else { 
			if(gmode_exact_case) {
				if (!scanner(&search_pattern[0], FORWARD, PTBEG)) {
					break;		/* all done */
				};
			} else {
				if (!scanner_ignorecase(&search_pattern[0],search_lowercase_pattern, FORWARD, PTBEG)) {
					break;		/* all done */
				};
			};
		};
		current_offset = Offset();
//		MESG("current_offset after find = %ld exact=%d",current_offset,gmode_exact_case);
		++nummatch;	/* Increment # of matches */
		if((nummatch % 100000)==0) 
		{
			stat=snprintf(lline,256,"count=%lld at %lld",(long long int)nummatch,current_offset);
			MESG_time(lline);
		};
		/* Check if we are on the last line.*/
		nlrepl = FEof(fp);

		/* Check for query.*/
		if (query)
		{
			/* Get the query.*/
pprompt:
		/* prompt to
		replace found_bytes_len==matchlen with rlengh chars or
		undo	matchlen chars with rlengh chars
		*/
			
			stat=snprintf(lline,256,"Replace %d chars with %s or undo %d chars?(y|n|u|.|G|?) ",found_bytes_len,replace_pattern,matchlen);
			if(stat>=256) MESG("truncated 5");
qprompt:
			msg_line(lline);
			update_screen(TRUE);  /* show the proposed place to change */
			c = getcmd();			/* and input */

			/* And respond appropriately.		 */
			switch (c)
			{
				case 'y':	/* yes, substitute */
				case ' ':
					hide_cursor("replaces");
					savematch(FORWARD);
					last_offs=Offset();
					break;

				case 'n':	/* no, onword */
					next_character(1);
					continue;

				case '!':	/* yes/stop asking */
					query = FALSE;
					break;

				case 'u':	/* undo last and re-prompt */

					/* Restore old position.*/
					if(last_offs < 0)
					{
						msg_line("nothing to undo!");
						goto pprompt;
					}
					textpoint_set(fp->tp_current,last_offs);
					/* Delete the new string.*/
					status = delins(rlength, patmatch);
					if (status != TRUE)	return (status);

					/* Record one less substitution,
					 * backup, and reprompt.
					 */
					--numsub;
					textpoint_set(fp->tp_current,last_offs);
					match_offs=last_offs;
					last_offs=-1;
					
					found_bytes_len = matchlen;
					matchlen = 0;
					goto pprompt;

				case '.':	/* abort! and return */
					/* restore old position */
					set_Offset(initial_offset);
					set_update(cwp,UPD_MOVE);
				case 'G':
				case CHR_ABORT:	/* abort! and stay */
					msg_line("Aborted!");
					return(TRUE);

				default:

				case '?':	/* help me */
					strcpy(lline,"(Y)es, (N)o, (!)Do rest, (U)ndo last, (^G)Abort, (.)Abort back, (?)Help:");
					goto qprompt;

			}	/* end of switch */
		}	/* end of "if query" */

		if(current_offset > end_offset) 
		{
			textpoint_set(cwp->tp_current,last_offset);
			break;
		};
		end_offset += rlength - found_bytes_len;
		status = delins(found_bytes_len, &replace_pattern[0]);
		last_offset = Offset();
		set_Offset(current_offset+strlen(replace_pattern));
		// set_Offset(current_offset+1);

		// MESG("after replace, end_offset=%ld last_offset=%ld",end_offset,last_offset);
		if (status != TRUE)
			return (status);

		numsub++;	/* increment # of substitutions */
	};

	stat=snprintf(lline,256,"replaced %lld items count=%lld",(long long int)numsub,prof_count);
//	if(stat<256) 
	MESG_time(lline);
	/* And report the results.*/
	setmark(0);
	set_modified(fp);
	if(!macro_exec)	msg_line("%d substitutions", numsub);
	return(TRUE);
}

/*
 * delins -- Delete a specified length from the current
 *	point, then insert the string.
 */
int delins(int dlength, char *instr)
{
	FILEBUF *fp=current_file_buffer();
	// MESG("delins: o=%ld delete %d chars, insert [%s]",Offset(),dlength,instr);
	if(!DeleteBlock(fp,0,dlength)) return FALSE;
	if(!InsertBlock(fp,instr,strlen(instr),0,0)) return FALSE;
	return (TRUE);
}


// works on cbfp !!!!
offs find_string_inline(char *patrn)
{
 curoffs = Offset();
 char *patptr;
 int c;
	while ( ( c = nextch(FORWARD)) >=0)
	{
		if(c==patrn[0])
		{
		/* Save the current position in case we match
		 * the search string at this point.*/
			match_offs=curoffs;
			/* Setup scanning pointers.*/

			patptr = &patrn[0];

			/* Scan through the pattern for a match.*/
			while (*++patptr != '\0')
			{
//				prof_count++;
				c = nextch(FORWARD);
				if (c<0) goto fail;
				if (c != *patptr) 
				{
					nextch(REVERSE);
					goto fail;
				};
			}

			/* A SUCCESSFULL MATCH!!! */
//			MESG("found at c=%ld m=%ld",curoffs,match_offs);
			return match_offs-1;

		}
fail:;			/* continue to search */
		if( c=='\n') {
//			MESG("Not found: c=%ld m=%ld",curoffs,match_offs);
			return -1;
		};
	};
	return -1; 
}

/*
 * nextch -- retrieve the next/previous character in the buffer,
 *	and advance/retreat the point.
 *	The order in which this is done is significant, and depends
 *	upon the direction of the search.  Forward searches look at
 *	the current character and move, reverse searches move and
 *	look at the character.
 */
int nextch(int dir)
{
 FILEBUF *fp=current_file_buffer();
	if (dir == FORWARD) {
		if(curoffs>FSize(fp)) return -1;

		if(EolAt(curoffs)) { 
			curoffs += fp->EolSize;
			return '\n';
		};
		return FCharAt(fp,curoffs++);
	} else {
		if(FBolAt(fp,curoffs)) {
			curoffs-=fp->EolSize;
			if(curoffs<0) return -1;
			return '\n';
		};
		if(curoffs<1) return -1;
		return FCharAt(fp,--curoffs);
	};
}

char *strcasestr(const char *haystack, const char *needle);;

int forw_stat(num n)
{
 int status=0;
 FILEBUF *fp=current_file_buffer();
 REXP_PAT rpattern[MAXLLEN];	// regexp pattern
 regexp_str(search_pattern,search_lowercase_pattern,rpattern,FORWARD);
// MESG("forwstat: exact=%d  [%s]",gmode_exact_case,search_pattern);
// MESG("	sp=[%s] rp=[%s]",search_pattern,rpattern);

 if(fp->b_flag >=FSNOTES) {
 	// set current
	int ind=-1;
	istr *idata;
	alist *search_list=NULL;
	int current_line=0;
	if(fp->b_flag & FSNLIST) {search_list=fp->dir_list_str;current_line=cwp->current_note_line;};
#if	TNOTES
	if(fp->b_flag & FSNOTESN) {search_list=fp->dir_list_str;current_line=cwp->current_note_line;};
	if(fp->b_flag & FSNOTES) {search_list=fp->b_tag_list;current_line=cwp->current_tag_line;};
#endif
	lbegin(search_list);
	// printf("search pattern: [%s] current_line=%d\n",&search_pattern[0],current_line);

	while((idata=(istr *)lget(search_list))!=NULL)
	{
 		char *str = &idata->start;
		ind++;
		if(ind <= current_line) continue;
#if	USE_GLIB
		gchar *tmp_str = g_utf8_strup(str,-1);
#else
		char *tmp_str = lstr_to_upper(str);
#endif
		// if(strcasestr(str,&search_pattern[0])!=NULL) 
		if(strstr(tmp_str,&search_pattern[0])!=NULL) 
		{
#if	TNOTES
			if(fp->b_flag & FSNOTES)
				cwp->current_tag_line=ind;
			else
#endif
				cwp->current_note_line=ind;

			set_update(cwp,UPD_WINDOW);
#if	USE_GLIB
			g_free(tmp_str);
#else
			free(tmp_str);
#endif
			return 1;
		};
#if	USE_GLIB
		g_free(tmp_str);
#else
		free(tmp_str);
#endif

		// printf("%2d [%s]\n",ind,str);
	};
	return 0;
 };

	while(n--) {
		if ( gmode_reg_exp )
			status = rexp_scanner(&rpattern[0], FORWARD, PTEND);
		else {
			if(gmode_exact_case) {
				status = scanner(&search_pattern[0], FORWARD, PTEND);
			} else {
				status = scanner_ignorecase(&search_pattern[0],search_lowercase_pattern, FORWARD, PTEND);
			};
		};
		if(!status) break;
	};

	return(lfound(status,FORWARD));
}


int back_stat(num n)
{
 int status;
 char reverse_pattern[MAXLLEN];
 REXP_PAT reverse_rpattern[MAXLLEN];	// reverse regular expression pattern
 char reverse_lower_pattern[MAXLLEN];
 FILEBUF *fp=current_file_buffer();

 if(fp->b_flag >=FSNOTES) {
 	// set current
	int ind=-1;
	int is_last=1;
	istr *idata;
	alist *search_list=NULL;
	int current_line=0;
	if(fp->b_flag & FSNLIST) {search_list=fp->dir_list_str;current_line=cwp->current_note_line;};
#if	TNOTES
	if(fp->b_flag & FSNOTESN) {search_list=fp->dir_list_str;current_line=cwp->current_note_line;};
	if(fp->b_flag & FSNOTES) {search_list=fp->b_tag_list;current_line=cwp->current_tag_line;};
#endif
	if(current_line==0) return 0;
	
	// printf("back_stat: search pattern: [%s]\n",&search_pattern[0]);
	// giti current line!
	lbegin(search_list);
	// while((idata=(istr *)lget(search_list))!=NULL)
	while((idata=(istr *)lget_next_nmv(search_list))!=NULL)
	{
		ind++;
		// printf("ind0=%d %2d [%s]\n",ind,idata->index,str);
		if(ind == current_line) { is_last=0;break;};
		lmove_to_next(search_list,0);
	};
	// printf("-- current ind=%d %d\n",ind,current_line);
	idata=(istr *)lget_previous(search_list);
#if	USE_GLIB
	gchar *tmp_str;
#else
	char *tmp_str;
#endif
	// printf("is_last=%d\n",is_last);
	
	while((idata=(istr *)lget_current(search_list)))
	{
 		char *str = &idata->start;
		ind--;
		// printf("check %d\n",ind);
		if(ind<-1) break;
#if	USE_GLIB
		tmp_str = g_utf8_strup(str,-1);
#else
		tmp_str = lstr_to_upper(str);
#endif
		if(strstr(tmp_str,&search_pattern[0])!=NULL) {
			// printf("found ind=%d [%s] in [%s]\n",ind,&search_pattern[0],str);
#if	TNOTES
			if(fp->b_flag & FSNOTES)
				cwp->current_tag_line=ind+is_last;
			else
#endif
				cwp->current_note_line=ind+is_last;
			set_update(cwp,UPD_WINDOW);
#if	USE_GLIB
			g_free(tmp_str);
#else
			free(tmp_str);
#endif
			return 1;
		};
#if	USE_GLIB
		g_free(tmp_str);
#else
		free(tmp_str);
#endif
		// printf("ind1=%d %2d [%s]\n",ind,idata->index,str);
		lmove_to_previous(search_list,0);
	};
	return 0;
 };


 rvstrcpy(reverse_pattern,search_pattern,MAXLLEN);
 rvstrcpy(reverse_lower_pattern,search_lowercase_pattern,MAXLLEN);
 regexp_str(reverse_pattern,reverse_lower_pattern,reverse_rpattern,REVERSE);

	do {
		if (gmode_reg_exp ) 
		{
			status = rexp_scanner(reverse_rpattern,REVERSE,PTBEG);
		}
		else
		{
			if(gmode_exact_case) {
				status = scanner(&search_pattern[0], REVERSE, PTBEG);
			} else {
				status = scanner_ignorecase(&search_pattern[0],search_lowercase_pattern, REVERSE, PTBEG);
			}
		};
	} while ((--n > 0) && status);

 /* Save away the match, or complain if not there. */

 return(lfound(status,REVERSE));
}

int lfound(int status,int direction)
{
//	MESG("lfound: %d, patmatch=%s",status,patmatch);
	if (status) {
		savematch(direction);
		found=1;
		if(!gmode_reg_exp) msg_line("Found");
	}else {
		msg_line("Not found");
		found=0;
	};
 return(found);
}

// It is supposed that pattern, lowe_pattern have equal string lentgth!
int regexp_str(char *pattern, char *lower_pattern,REXP_PAT *rxpat,int dir )
{
 register unsigned int i;
 register int j;
 register char c;
 register char l;
 int list;
 int quote = FALSE;
 int quotectl = FALSE;
 // MESG("regexp_str: pat=%s dir=%d len=%d",pattern,dir,strlen(pattern));
 memset(rxpat,0,3*MAXLLEN);	// clear 
 list=0;
 for(i=0,j=0;i<strlen(pattern);i++) {
   if(j>=MAXLLEN) break;
   c=pattern[i];
   l=lower_pattern[i];

   if(dir==REVERSE) {
	if(pattern[i+1]=='^' && pattern[i+2]!='\\') quotectl=TRUE;
    if(pattern[i+1]=='\\') quote=TRUE;
    };
   if(quotectl) {
	rxpat[j].l = l;
	rxpat[j++].c = c;

	quotectl=0;
	quote=0;
   } else 
   if(quote) {
    	rxpat[j].t += list;
		rxpat[j].l = l;
		rxpat[j++].c = c;
		quote=0;
	continue;
   } else
   switch(c) {
    case '%' :{
    	if((i==0 && dir!=REVERSE)  || (i==strlen(pattern)-1 && dir==REVERSE) ) {
		  if(dir==REVERSE) rxpat[j-1].t|=M_START;
		  else
		  rxpat[j].t |= M_START;	  		
		} else {
			rxpat[j].l = l;
			rxpat[j++].c = c;
	  		quote=0;
		};
	break;
    };
    case '$' :{
		if(strlen(pattern)==1) break;
		if((i==strlen(pattern)-1 && dir!=REVERSE) || (i==0 && dir==REVERSE) ) {
				if(dir!=REVERSE) rxpat[j-1].t |= M_END;
				else rxpat[j].t |=M_END;
	 	} else { 
				rxpat[j].l = l;
	  			rxpat[j++].c = c;
	  			quote=0;
	  	};
	break;
    };
	case '^' :{
		if(dir==REVERSE) break;
		quotectl=1;
		break;
	};
    case '\\' : {
     if(dir==REVERSE) break;
     quote=TRUE;
     break;
    };

    case '?' : {
		rxpat[j].c='?';
		rxpat[j].l='?';
		rxpat[j++].t = M_ANY;
	break;
    };
    case '*' : {
	/* When ** is occured the second is quoted */
		rxpat[j].t = M_FIND;
	
		if(i==strlen(pattern)-1) {
			rxpat[j].c = '*';
			rxpat[j].l = '*';
			rxpat[j].t |= M_ANY;
			j++;
		};
	break;
    };
    default: {
      	rxpat[j].t += list;
		rxpat[j].l = l;
		rxpat[j++].c = c;
		quote=0;
    };
  }
 };
 rxpat[j].l=M_EOF;
 rxpat[j].t=M_EOF;
 rxpat[j].c=M_EOF;
// for(i=0;i<j;i++) MESG("%2d %X %X %X",i,rxpat[i].c,rxpat[i].l,rxpat[i].t);
 return(TRUE);
}

int	rexp_scanner(REXP_PAT *patptr, int direct, int beg_or_end)
{
	register int	c;		/* character at current position */

	/* If we are going in reverse, then the 'end' is actually
	 * the beginning of the pattern.  Toggle it.
	 */
	beg_or_end ^= direct;
	curoffs=Offset();
//	MESG("rexp_scanner: [%s] o=%ld",patptr,curoffs);
	/* Scan each character until we hit the head link record.
	 */
	while ( ( c = nextch(direct)) >=0)
	{
		/* Scan through the pattern for a match.  */
		if(amatch(c,patptr,direct)) {
			/* A SUCCESSFULL MATCH!!!
			 * reset the global "." pointers */
			set_global(beg_or_end,direct);
			return TRUE;
		};
	}

	return FALSE;	/* We could not find a match */
}

int amatch(char c1,REXP_PAT *pat,int direct)
{
  int c;
  REXP_PAT *p;
  REXP_PAT *p_find=NULL;
  int ok=0;
//  int m_found=0;
  p=pat;
  c=c1;
  match_offs=curoffs;
//  MESG("amatch:----------------------------------------- o=%ld",curoffs);
  for(found_bytes_len=0;(p->t!=0 || p->c!=0) && c>0 ;p++,c=nextch(direct),found_bytes_len++) {
//   if(found_bytes_len) MESG("-- at %ld c=%X foun_bytes_len=%d poffs=%d p->c p->l p->t",curoffs-1,c,found_bytes_len,p-pat,p->c,p->l,p->t);
   ok=0;
   if(p->t & M_START) {
		if( direct!=REVERSE && !BolAt(curoffs-1)) return(FALSE);
		if( direct==REVERSE && !BolAt(curoffs)) return(FALSE);
	};
   if((p->t & M_END)) {
   	 if( direct!=REVERSE && !EolAt(curoffs) ) return(FALSE);
   	 if( direct==REVERSE && !EolAt(curoffs-1) ) return(FALSE);
	};
//   if(m_found) MESG("m_found: check next o=%ld pat_o=%d check c=%X <> %X or %X p->t=%X",curoffs,p-pat,c,p->c,p->l,p->t);

    if(p->t & M_FIND) {
		p_find=p;
    	while( !eq1(c,p->c,p->l) ) {
		c=nextch(direct);
		found_bytes_len++;
		if(c<0) return(FALSE);
		if(c=='\n') return(FALSE);
	};
	ok=TRUE;
	continue;
    } 
	else {
//		if(found_bytes_len)	MESG("check for match o=%d po=%d c=%X p->c=%X p->l=%X",curoffs,p-pat,c,p->c,p->l);
    if (eq1(c,p->c,p->l)) 
	{
    	ok=TRUE;
//		if(c<128) MESG("- match c=%X=%c  p.c=%X p.l=%x",c,c,p->c,p->l);
//		else MESG("- match c=%c  p.c=%X p.l=%x",c,p->c,p->l);
	} else 
	{
		if(p->t == M_ANY) ok=TRUE;
		else {
			if(p_find!=NULL && c!='\n') {
				p=p_find-1;
				ok=0;
//				if(c<128) MESG("???? skip no match c=%X=%c  p.c=%X p.l=%x",c,c,p->c,p->l);
//				else MESG("??? skip no match c=%c  p.c=%X p.l=%x",c,p->c,p->l);
				continue;
			};
		return(FALSE);
		};
	};
	};
  };
//  MESG("---finished --- ok=%d",ok);
  if(ok)  {nextch(direct ^ REVERSE);
  	if(!execmd) msg_line("Match %d bytes",found_bytes_len);
//	MESG("Match %d bytes",found_bytes_len);
  };
  return(ok);
}

void set_global(int beg_or_end,int direct)
{
//	MESG("set_global: update current window! beg_or_end=%d %d",beg_or_end,PTEND);
	if (beg_or_end == PTEND)	/* after the end of the string */
	{
		set_Offset(curoffs);
	}
	else		/* at beginning of string */
	{
	 if(direct==REVERSE) {
		match_offs=curoffs;
		set_Offset(curoffs);
	 } else {
		curoffs = match_offs;
		nextch(REVERSE);
		match_offs=curoffs;
		set_Offset(match_offs);
	 }
	}
	// set_update(cwp,UPD_MOVE);
}

int gmode_back=0;
int exact_back=0;
int regexp_back=0;

void backup_gmode()
{
	gmode_back=gmode;
	exact_back = gmode_exact_case;
	regexp_back=gmode_reg_exp;
}

void restore_gmode()
{
	gmode=gmode_back;
	gmode_exact_case = exact_back;
	gmode_reg_exp=regexp_back;
}

int find_tag(num n)
{
 char tword[128];
 
 FILEBUF *tagb;
 char linenum[20],fname[MAXFLEN],type[32];
 FILEBUF *cbp;
 char st[128];
 int i,j1;
 int status;
 char **tag_names;
 char *tag_name;
 int stat=0;
 backup_gmode();
 // MESG("find_tag:");
 
// get the current word or another word
	set_hmark(1,"find_tag");
	strlcpy(tword+1,getcurcword(),127);
	// MESG("find_tag:[%s]",tword);
	if(tword[1]==0) { 
		return(0);
	};
	tword[0]='%';	// go search at beginning of line
	i=strlen(tword);

	// add a space at the end
	tword[i]=' ';
	tword[i+1]=0;
	cbp = current_file_buffer();

	if((tagb=get_filebuf("[tagx.tags]",NULL,0)) == NULL) {
		// new buffer, create it and read from current dir
		if((tagb=new_filebuf("[tagx.tags]", FSINVS)) == NULL) { 
//			list_off();
			msg_line("cannot create tagx.tags buffer!");
			return(0);
		};
		select_filebuf(tagb);

		EmptyText(tagb);
		status=ifile(tagb,"tagx.tags",0);
	} else { 
		status=select_filebuf(tagb);
	};
	tagb->b_state &= ~(FS_CHG);
	if(status==0) { 
		if(!execmd) msg_line("update tagx.tags file");
		status=update_tags(0);
		
		if(status==0) { // no(?) ctags command or other problem
			ERROR("cannot create tagx.tags file!");
			select_filebuf(cbp);
//			list_off();
			return(0);
		};
	};
	select_filebuf(tagb);
	// goto the begining of tag file
	set_Offset(0);
	last_offs=-1;
	// set case, regexp mode to tagx.tags buffer  
	gmode_exact_case = true;
	gmode_reg_exp = true;

	// search for word in first column
	setpattern(search_pattern,tword,1);

	// create a list with found tags
	alist *tag_list = new_list(0,"tag_list");
	for(i=0;(status=find_next(1))>0;i++)
	{
		goto_bol(1);
		next_word(1);
		strlcpy(type,getnword(),32);
		next_word(1);
		strlcpy(linenum,getnword(),20);
		next_word(1);
		strlcpy(fname,getcurfword(),MAXFLEN);
		tag_name = (char *)malloc(MAXFLEN);
		stat=snprintf(tag_name,MAXFLEN,"%1d - %10s %5s %20s",i,type,linenum,fname);
		if(stat<MAXFLEN) add_element_to_list((void *)tag_name,tag_list);
	};

	if(i==0) { 
		msg_line("Tag [%s] not found!",tword+1);
		select_filebuf(cbp); 
//		list_off();
		restore_gmode();
		return(0);
	};
	tag_names = (char **) array_data(tag_list);
	msg_line("select tag from list");
//	set_list_type(0);
	j1=selectl("select tag",tag_names,i,10,5,1,60,-1);

	if(j1<0) { 
		select_filebuf(cbp); 
		restore_gmode();
		return(0);
	}; // abort command

	strlcpy(st,tag_names[status],128);

	free_list(tag_list,"tag_list");

	set_Offset(0);	
	status=find_next(j1+1);
	goto_bol(0);
	next_word(1);

	strlcpy(linenum,getnword(),20);
	if(atoi(linenum)==0 && linenum[0]!=0) {
		next_word(1);
		strlcpy(linenum,getnword(),20);
	};
	next_word(1);

	strlcpy(fname,getcurfword(),MAXFLEN);

	goto_file(fname);
	igotolinecol(atoi(linenum),1,-1);
	set_hmark(1,"find_tag");
//	list_off();
	restore_gmode();
	return(1);
}

offs fast_scanner4 (FILEBUF *bp, offs stringlen, char *pat, int patlen,offs start);
offs pattern_line;

int grep_buffer(num nuse)
{
	int status;		/* return status from CLI */
	FILEBUF *fp=current_file_buffer();	/* pointer to buffer */
    char sline[256];
	int pattern_len=0;
	char *sline2=NULL;

	WINDP *filter_window;
	long int count=0;
	int stat=0;

	offs buffer_size=FSize(fp);
	/* get the filter name and its args */
	set_list_type(LSRC);
	search_pattern[0]=0;

	if ((status=nextarg("Filter: ", search_pattern, MAXLLEN,true)) != TRUE) return(status);

	pattern_len=strlen(search_pattern);
	pattern_line=1;

//	MESG("grep_buffer: pattern=[%s] len=%d",search_pattern,pattern_len);

	FILEBUF *filter_buffer;
	filter_buffer = get_filebuf("[filter]",NULL,0);
	if(filter_buffer==NULL) {
		filter_buffer=new_filebuf("[filter]",0);

		split_window(1);
		next_window(1);
		filter_window=cwp;	
	} else { // find window with [filter] buffer and use it
		filter_window=find_buffer_window(filter_buffer);
		if(!filter_window) { // there is no window with filter_buffer!
			split_window(1);
			next_window(1);
			filter_window=cwp;
			prev_window(1);
		};
	};
	EmptyText(filter_buffer);

	stat=snprintf(sline,256,"filter for [%s], working ..",search_pattern);
	if(stat<256) msg_line(sline);
	discmd=0;
	set_current_file_buffer(fp);
	set_Offset(0);
	curoffs=0;

	while((curoffs = fast_scanner4(fp,buffer_size ,search_pattern,pattern_len,curoffs)))
	{
		stat=snprintf(sline,256,"%08lld: ",pattern_line);
		if(stat>=256) MESG("truncated 5");
		sline2=get_line_at(fp,curoffs);

		insert_string(filter_buffer,sline,strlen(sline));
		insert_string_nl(filter_buffer,sline2);

		count++;
		while(FCharAt(fp,curoffs)!=fp->EolStr[0] && curoffs< (fp->BufferSize-1)) curoffs++;
		pattern_line++;
	};

	discmd=1;
	stat=snprintf(sline,256,"filter[%s]: found %ld matches",search_pattern,count);
	if(stat>=256) MESG("truncated 6");

	msg_line(sline);
	cwp = filter_window;
	select_filebuf(filter_buffer);
	set_update(filter_window,UPD_EDIT+UPD_ALL);
	set_update(cwp,UPD_EDIT+UPD_ALL);
#if	0
	cwp = bp_window; // goto previous window
	select_filebuf(bp);

	set_Offset(bp_offset);
#endif
	set_update(cwp,UPD_WINDOW);
	set_update(filter_window,UPD_WINDOW);
	return(TRUE);
}

offs fast_scanner4 (FILEBUF *fb, offs stringlen, char *pat, int patlen,offs start) 
{
    register offs i;
	register offs start1=0;
	FILEBUF *fp=current_file_buffer();

    i = 0;
    while (i < (stringlen-start)) {
        register offs j = 0;
		start1=i+1;	/* starts at next char if mach is failed  */
        while (j < patlen && (FCharAt(fb,i+start) == pat[j])) {
           i++;
           j++;
        }
        if (j == patlen) {
			return start+i;
        }
        i = start1;
		if(fp->EolStr[0]==FCharAt(fb,i+start)) {pattern_line++;};
    }
    return 0;
}


/*
 Regular expression description

 '%' matches the beginning of line.
 '$' matches the end of line.
 '^' matched control characters
 '?' matches any one character
 * matches zero character or many characters in the line.
 '\' is used for quoting the above special characters

*/
