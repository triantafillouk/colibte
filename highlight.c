/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

*/

/* highlight code  */

#include	"xe.h"

#include "highlight.h"

int hselection=0;
int hquotem=0;	/* highlight quote mask */
// int prev_hquotem=-1;
int hprev_line=-1;
int slang=0;		/* script language in html */
int hstate=0;
int in_array=0;
int first=1;
short prev_set=-1;
short h_hquote_start=0;
short h_line_set=0;
short h_prev_space=0;
short h_bold=0;

unsigned char flag_word=0;
unsigned char single_quoted=0;
unsigned char double_quoted=0;

int stop_word_highlight;
int start_word_highlight;
int hnote=0;
int note_type=0;
int line_col=0;

extern int h_type[];
extern FILEBUF *cbfp;

/* read1line */
int read1line(FILEBUF *);
char *fline=NULL;
int flen=0;

void	ToLineBegin();

/* select highlight type from list  */
int select_highlight(num n)
{
	int htype=0;
	int highlight_num=0;
	int i;
	char **highlight_name;

	for(highlight_num=0;hts[highlight_num].description != NULL;highlight_num++) ;
	highlight_name = malloc((highlight_num+1)*sizeof(char *));
	for(i=0;i<highlight_num;i++) {
		highlight_name[i]=hts[i].description ;
	};highlight_name[i]=NULL;
	htype=selectl("Highlight type",highlight_name,highlight_num,20,10,1,20,-1);
//	MESG("select_highlight: %d",htype);
	if(htype<0) return false;
	set_highlight(cbfp,htype);
	free(highlight_name);
	set_update(cwp,UPD_ALL);
	return 1;
}

void set_highlight(FILEBUF *bf,int type)
{
	// MESG("set_highlight: type=%d",type);
	bf->hl=&hts[type];
	if(file_type_is("BIN",type))
	{
		bf->view_mode = VMHEX;
	};
//	for(int i=0;i<2;i++) MESG("set_highlight: h_word[%d]=[%s]",i,bf->hl->w1[i]);
}

/* find file highlight type by parsing the start of the file contents */
int get_highlight(FILEBUF *fp)
{
 int i=0;
 int j=0;
 int num;
 int ht;
 int flen0=0;
 // int status;
 int ftype=fp->b_type % NOTE_TYPE;
 int ind2;
 struct stat    st;
 fstat(fp->file_id,&st);
 if(st.st_size > 100000000 && ftype==0) return(highlight_index("BIN",&ind2));
 ht = ftype;
 if(hts[ht].description==NULL) return(highlight_index("NONE",&ind2)); 
 
 if(fp->b_mode & EMCRYPT)
	lseek(fp->file_id,1L,SEEK_SET); else lseek(fp->file_id,0L,SEEK_SET);
 // find the first not null line
 while((flen0=read1line(fp))>=0)
 {
	i++;
	if(i>100) return(highlight_index("GTEXT",&ind2));
	if(flen0>3) {	/* non empty line  */
		fline[flen0]=0;
//		MESG("check: [%s]",fline);
		// goto the begining before return!
		if(fp->b_mode & EMCRYPT) lseek(fp->file_id,1L,SEEK_SET); else lseek(fp->file_id,0L,SEEK_SET);
		/* check binary */
		if(fline[0]=='x' && fline[1]==1) return(highlight_index("BIN",&ind2)); // git files
		for(j=0,num=0;j<flen0;j++) {
			if(fline[j]==0) return(highlight_index("BIN",&ind2));
				if(fline[j]<32 && fline[j]!=13 && fline[j]!=9 && fline[j]!=10 && fline[j]!=27) num++;
		}; 
		if(num*3>flen0) 
		{
			return(highlight_index("BIN",&ind2)); 	/* too many control characters in the line!  */
		};

		if(!strcmp(hts[ht].description,"OBJC"))
		{	/* this is .m file. find if a matlab or objc  */
//			MESG("	test objc [%s]",fline);
			if(strstr(fline,"#")!=NULL) return(highlight_index("OBJC",&ind2));
			if(strstr(fline,"@")!=NULL) return(highlight_index("OBJC",&ind2));
			if(strstr(fline,"function")!=NULL) return(highlight_index("MATLAB",&ind2));
			if(strstr(fline,"%")!=NULL) return(highlight_index("MATLAB",&ind2));
			if(fline[0]=='/') return(highlight_index("OBJC",&ind2));
//			return(highlight_index("MATLAB",&ind2));
			continue;
		};
		if(!strcmp(hts[ht].description,"C"))
		{
			if(strstr(fline,"@")!=NULL) return(highlight_index("OBJC",&ind2));
			if(strstr(fline,"import")!=NULL) return(highlight_index("OBJC",&ind2));
		};
		if(strstr(fline,"#include")) return(highlight_index("C",&ind2));
		if(strstr(fline,"#pragma")) return(highlight_index("C",&ind2));
		if(ht>0) return(ht);
		
		if(fline[0]=='<'||fline[3]=='<') {
			if(strstr(fline,"xml")!=NULL) return(highlight_index("XML",&ind2));
			if(strstr(fline,"HTML")!=NULL) return(highlight_index("HTML",&ind2));
			if(strstr(fline,"WML")!=NULL) return(highlight_index("WML",&ind2));
			if(strstr(fline,"HEAD")!=NULL) return(highlight_index("HTML",&ind2));
			return(highlight_index("STRUCT",&ind2));
		};
		if(fline[0]=='!') return(highlight_index("XPROP",&ind2));
		if(strstr(fline,"From ")!=NULL) return(highlight_index("MAIL",&ind2));
		if(!strncmp(fline,"@ssg",4))  return(highlight_index("SSG",&ind2));
		if(!strncmp(fline,"@SSG",4))  return(highlight_index("SSG",&ind2));
		if(!strncmp(fline,"@. ",3))  return(highlight_index("ECL",&ind2));
		if(fline[0]=='@') return(highlight_index("ECL",&ind2));
		/* check shell */
		/* check comment */
		if(fline[0]=='#') {//  msg_line("we have shell script comment");
			if(fline[1]=='!') {
				// msg_line("we have executable");
				if(strstr(fline,"perl")!=NULL) return(highlight_index("PERL",&ind2));
				if(strstr(fline,"wish")!=NULL) return(highlight_index("TCL",&ind2));
				if(strstr(fline,"python")!=NULL) return(highlight_index("PYTHON",&ind2));
				if(strstr(fline,"sh")!=NULL) return(highlight_index("SHELL",&ind2));
				/* check for perl, bash, sh, tcsh, ksh, tcl, python */
			} else {
				if(ftype>0) return(ftype);
				if(strstr(fline,"KDE")!=NULL) return(highlight_index("TEXT",&ind2));
				return(highlight_index("GTEXT",&ind2));
			};
		};
		if(strstr(fline,"SQL") ||strstr(fline,"sql") ) return(highlight_index("SQL",&ind2));
		if(strstr(fline,"PROC")) return(highlight_index("COBOL",&ind2));
		if(strstr(fline,"PROGRAM-ID")) return(highlight_index("COBOL",&ind2));
		if(strstr(fline,"IDENTIFICATION DIVISION")) return(highlight_index("COBOL",&ind2));
		if(!strncmp(fline,"      *",7)) return(highlight_index("COBOL",&ind2));
		if(!strncmp(fline,"skel",4))  return(highlight_index("SSG",&ind2));
		if(!strncmp(fline,"*. ",3))  return(highlight_index("SSG",&ind2));
		
		if((fline[0]=='/'&& fline[1]=='*') || fline[0]==CHR_CURLR) {
		// msg_line("we have a c comment");
		return(highlight_index("C",&ind2));
		};
		// did not recognise anything from parsing!
		// if(ht>0) return(ht);
 	}
 }
 return(highlight_index("GTEXT",&ind2));
}

/* read a line from an open file (put it in fline) at most flen bytes   */
// used only from get_highlight!!!
int read1line(FILEBUF *fp)
{
    int c;		/* current character read */
    int i;		/* current index into fline */
	char sc[1];
	int s;
	int len=0;
	/* read the line in */
    i = 0;
	if(flen==0) {
 		flen=512;
 		fline = (char *)malloc(flen);
 	};
	// goto the start of the next line to check!
	lseek(fp->file_id,fp->bytes_read,0);
	errno=0;
	while((s=read(fp->file_id,sc,1))==1)
	{
		c=sc[0];
        fline[i++] = c;

		if(i>=flen) break;
		if(c=='\n') {len=i; break;}
		if(c=='\r') {len=i; break;}
	}
	len=i;
	if(s==0||errno) return(-1); 
	fp->bytes_read += len;
	/* terminate and decrypt the string */
    fline[len] = 0;
#if	CRYPT
	/* decrypt the line */
	if (fp->b_mode & EMCRYPT) crypt_string(fline, i);
#endif
	return(len);
}

int file_type_is(char *extention,int check_type)
{
 int ct=check_type%NOTE_TYPE;
// MESG("file_type_is: compare [%s] <> hts[%d]=[%s]",extention,ct,hts[ct].description,check_type);
 if(!strcmp(hts[ct].description,extention)) return TRUE;
 else return FALSE;
}

int comment_with_string(char *comment_string,int start)
{
 FILEBUF *fp=cbfp;
 	// MESG("comment_with_string: type=%d",start);
	if(fp->b_flag<4) {
		offs curoffs=FOffset(fp);
		offs pos;
		offs bol_offs;
		int size;
		ToLineBegin();
		bol_offs = FOffset(fp);
		pos = find_string_inline(comment_string);
		if(pos<bol_offs) {	/* not found !  */
			if(start==C_COLSTART) ToStartColumn();
			if(start==C_LINEEND) ToLineEnd();
			if(start==C_LINEBEG) ToLineBegin();
			insert_string(fp,comment_string,strlen(comment_string));
			insert_string(fp," ",1);
			set_Offset(curoffs+strlen(comment_string)+1);
			size=strlen(comment_string)+1;
		};
		if(pos>=bol_offs) {
			// delete it!
			ToLineBegin();
			// textpoint_move(fp->tp_current,pos-bol_offs);
			textpoint_set(fp->tp_current,pos);
			int c = FCharAt(fp,pos+strlen(comment_string));
			int space=0;
			if(c==' '||c=='\t') space=1;
			DeleteBlock(fp,0,strlen(comment_string)+space);
			if(curoffs==pos+strlen(comment_string)) { set_Offset(curoffs-strlen(comment_string));}
			else if(curoffs>pos+strlen(comment_string)){ set_Offset(curoffs-strlen(comment_string)-space);} 
			else {set_Offset(curoffs); }
			size=-strlen(comment_string)-space;
		};
		if(cwp) set_update(cwp,UPD_EDIT);
		set_modified(fp);
		return(size);
	} else {
		msg_line("read only!");
		return false;
	}
}

int comment_c(int dummy)
{
 FILEBUF *fp = cbfp;
 offs pos;
 offs s0;
	// MESG("comment_c:");
	if(cwp->selection) {
		offs start=tp_offset(cwp->w_smark);
		offs end=tp_offset(cwp->w_emark);
		if(start>end) {
			offs tmp=end;
			end=start;start=tmp;
		};
		set_Offset(start);
		// ToLineBegin();
		s0=FOffset(fp);
		pos = find_string_inline("/*");
		if(pos<s0) {
			set_Offset(s0);
			insert_string(fp,"/* ",3);
			set_Offset(end+3);
			// ToLineEnd();
			insert_string(fp," */",3);
		} else {
			set_Offset(s0);
			DeleteBlock(fp,0,3);
			set_Offset(end-6);
			DeleteBlock(fp,0,3);
		};
		setmark(0);
	} else { 
#if	1
		ToLineBegin();
		s0 = Offset();
		pos = find_string_inline("/*");
		if(pos>=s0) {
			set_Offset(pos+3);	
		} else {
			ToLineEnd();
			pos = FOffset(fp);
			insert_string(fp,"	/*   */",8);
			set_Offset(pos+4);
		};
#else
		ToLineBegin();
		s0 = FOffset(fp);
		pos = find_string_inline("//");
		if(pos>=s0) {
			set_Offset(pos+3);	
		} else {
			ToLineEnd();
			pos = FOffset(fp);
			insert_string(fp,"	// ",4);
			set_Offset(pos+4);
		};
#endif
	};
	set_update(cwp,UPD_MOVE);
	set_modified(cbfp);
	return 1;
}

int comment_lua(int dummy)
{
 FILEBUF *fp = cbfp;
 offs pos;
 offs s0;
	// MESG("comment_lua:");
	if(cwp->selection) {
		offs start=tp_offset(cwp->w_smark);
		offs end=tp_offset(cwp->w_emark);
		if(start>end) {
			offs tmp=end;
			end=start;start=tmp;
		};
		set_Offset(start);
		// ToLineBegin();
		s0=FOffset(fp);
		pos = find_string_inline("--[[");
		if(pos<s0) {
			set_Offset(s0);
			insert_string(fp,"--[[ ",3);
			set_Offset(end+3);
			// ToLineEnd();
			insert_string(fp," --]]",3);
		} else {
			set_Offset(s0);
			DeleteBlock(fp,0,3);
			set_Offset(end-6);
			DeleteBlock(fp,0,3);
		};
		setmark(0);
	} else { 
		ToLineBegin();
		s0 = FOffset(fp);
		pos = find_string_inline("--");
		if(pos>=s0) {
			set_Offset(pos+3);	
		} else {
			ToLineEnd();
			pos = FOffset(fp);
			insert_string(fp,"	-- ",4);
			set_Offset(pos+4);
		};
	};
	set_update(cwp,UPD_MOVE);
	set_modified(cbfp);
	return 1;
}

int comment_css2()
{
 FILEBUF *fp = cbfp;
 offs pos;
 offs s0;
	if(cwp->selection) {
		offs start=tp_offset(cwp->w_smark);
		offs end=tp_offset(cwp->w_emark);
		if(start>end) {
			offs tmp=end;
			end=start;start=tmp;
		};
		set_Offset(start);
		// ToLineBegin();
		s0=FOffset(fp);
		pos = find_string_inline("/*");
		if(pos<s0) {
			set_Offset(s0);
			insert_string(fp,"/* ",3);
			set_Offset(end+3);
			// ToLineEnd();
			insert_string(fp," */",3);
		} else {
			set_Offset(s0);
			DeleteBlock(fp,0,3);
			set_Offset(end-6);
			DeleteBlock(fp,0,3);
		};
		setmark(0);
	} else { 
		ToLineBegin();
		s0 = FOffset(fp);
		pos = find_string_inline("/*");
		if(pos>=s0) {
			set_Offset(pos+3);	
		} else {
			ToLineEnd();
			pos = FOffset(fp);
			insert_string(fp,"	/*  */ ",8);
			set_Offset(pos+4);
		};
	};
	set_update(cwp,UPD_MOVE);
	set_modified(cbfp);
	return 1;
}


int comment_cc(int n)
{
// MESG("comment_cc: n=%d",n);
 	if(n==C_STARTEND) return comment_c(1);
	if(n==C_COLSTART) return comment_with_string("//",C_COLSTART);
	if(n==C_LINEEND) return comment_with_string("//",C_LINEEND);
	return comment_with_string("//",C_LINEBEG);
}

int comment_perl(int n)
{
	// MESG("comment_perl:");
	return comment_with_string("#",n);
}

int comment_ini(int n)
{
	// MESG("comment_ini: n=%d",n);
	if(n==C_COLSTART) return comment_with_string(";",C_COLSTART);
	if(n==C_LINEEND) return comment_with_string(";",C_LINEEND);
	return comment_with_string(";",C_LINEBEG);
}

int comment_none(int n)
{
	return 0;
}

int comment_matlab(int n)
{
	return comment_with_string("; ",n);
}

int comment_sql(int n)
{
	return comment_with_string("-- ",0);
}

int comment_basic(int n)
{
	return comment_with_string("' ",0);
}

int comment_ecl(int n)
{
	return comment_with_string(". ",n);
}

int comment_html(int n)
{
 FILEBUF *fp = cbfp;
 offs pos;
 offs s0;
 if(n==3) return 0;
 // MESG("comment_html: n=%d",n);
	if(cwp->selection) {
		offs start=tp_offset(cwp->w_smark);
		offs end=tp_offset(cwp->w_emark);
		if(start>end) {
			offs tmp=end;
			end=start;start=tmp;
		};
		set_Offset(start);
		// ToLineBegin();
		s0=FOffset(fp);
		pos = find_string_inline("<!--");
		if(pos<s0) {
			set_Offset(s0);
			insert_string(fp,"<!-- ",5);
			set_Offset(end+5);
			// ToLineEnd();
			insert_string(fp," -->",4);
		} else {
			set_Offset(s0);
			DeleteBlock(fp,0,5);
			set_Offset(end-9);
			DeleteBlock(fp,0,4);
		};
		setmark(0);
	} else { 
		ToLineBegin();
		s0 = FOffset(fp);
		insert_string(fp,"<!--   -->",10);
		insert_newline(cbfp);
		set_Offset(s0+5);
	};
	set_update(cwp,UPD_MOVE);
	set_modified(cbfp);
	return 1;
}

int comment_md(int comment_type)
{
 FILEBUF *fp = cbfp;
 offs pos;
 offs s0;
 offs start=FOffset(fp);
 offs end;
 // if(n==3) return 0;
 	MESG("comment_md: comment_type=%d",comment_type);
	if(cwp->selection) {
		start=tp_offset(cwp->w_smark);
		end=tp_offset(cwp->w_emark);
		if(start>end) {
			offs tmp=end;
			end=start;start=tmp;
		};
	};
	if(cwp->selection && comment_type==C_STARTEND) {
		// ToLineBegin();
		set_Offset(start);
		s0=FOffset(fp);
		pos = find_string_inline("```");
		if(pos<s0) {
			set_Offset(s0);
			insert_string(fp,"```",3);
			new_line(1);
			set_Offset(end+3+cwp->w_fp->EolSize);
			new_line(1);
			// ToLineEnd();
			insert_string(fp,"```",3);
		} else {
			set_Offset(s0);
			DeleteBlock(fp,0,3+cwp->w_fp->EolSize);
			set_Offset(end-6-cwp->w_fp->EolSize);
			DeleteBlock(fp,0,3+cwp->w_fp->EolSize);
		};
		setmark(0);
		set_update(cwp,UPD_MOVE);
		set_modified(cbfp);
		return 1;
	};
	if(comment_type==C_COLSTART && cwp->selection) 
	{
		set_Offset(start);
		if(CharAt(start)=='`') {
			DeleteBlock(fp,0,1);
			pos=find_string_inline("`");
			if(pos>FOffset(fp)) {
				set_Offset(pos);
			} else {
				set_Offset(end-1);
			};
			DeleteBlock(fp,0,1);
		} else {
			insert_string(fp,"`",1);
			set_Offset(end+1);
			insert_string(fp,"`",1);
		};
		set_update(cwp,UPD_MOVE);
		set_modified(fp);
		return 1;
	};
	if(
		// comment_type==C_LINEBEG && 
		! cwp->selection) {
		ToLineBegin();
		s0 = FOffset(fp);
		set_Offset(s0);
		pos = find_string_inline("- ");
		set_Offset(s0);
		// MESG("	s0=%ld pos=%ld",s0,pos);
		if(pos<s0) {
			insert_string(fp,"- ",2);
			set_Offset(s0+2);
		} else {
			DeleteBlock(fp,0,2);	
		};
	};
	set_update(cwp,UPD_MOVE);
	set_modified(fp);
	return 1;
}

int comment_css(int n)
{
	// MESG("comment_css: n=%d",n);
	if(n==3) return comment_html(2);
	return comment_css2();
}

int highlight_index(char *extention,int *ind2)
{
 int index=0;
 *ind2=0;
 for(index=0;hts[index].description!=NULL;index++)
 {
	int i;
	int len=strlen(extention);
	char **ext=hts[index].file_extentions;
	for(i=0;ext[i][0]!=0;i++){
		if(strlen(ext[i])!=len) continue;
		if(!strncasecmp(extention,ext[i],len)){
			*ind2=i;
			return index;
		};
	};
 };
 return 0;
}

int highlight_type(char *extention)
{
 int index=0;
 for(index=0;hts[index].description!=NULL;index++)
 {
	int len=strlen(extention);
	if(!strncasecmp(extention,hts[index].description,len)){
		return index;
	};
 };
 return 0;
}


/*	returns true if full window update is needed 
	updates window flags selecting lines to draw or all
*/
int check_update_highlight(int flag)
{
 if(!discmd) return 0;
 FILEBUF *fp = cwp->w_fp;
 int stat=0;
 static int prev_init=0;
 static num oldlines;
 static int h1;
 static num check_offset1=0;
 static num check_offset2=0;
 // MESG("check_update_highlight:f=%d cwp hline=%ld",flag,tp_offset(cwp->tp_hline));
 if(flag==0) {
	if(prev_init) return 0;
 	oldlines=fp->lines;
	highlight_restore_state(cwp,0);
	// int htemp=hquotem;
	check_offset1 = FNextLine(fp,tp_offset(cwp->tp_current));
	fquote_state(check_offset1,tp_offset(cwp->tp_hline),cwp);
	h1=hquotem;
	prev_init=true;
	// MESG("check_update_highlight: old lines=%ld o0=%ld quote = %X -> %X",oldlines,check_offset1,htemp,h1);
 } else {
	if(!prev_init) return 0;
 	num newlines=fp->lines;
	highlight_restore_state(cwp,0);
	check_offset2 = FNextLine(fp,tp_offset(cwp->tp_current));
	fquote_state(check_offset2,tp_offset(cwp->tp_hline),cwp);
	// MESG("c: o0=%ld (l=%ld h1=%d)  o1=%ld (l=%d h1=%d)",check_offset1,oldlines,h1,check_offset2,newlines,hquotem);
	if(newlines!=oldlines || h1!=hquotem) {
		set_update(cwp,UPD_EDIT);
		update_buffer_windows(fp,UPD_EDIT);
		stat=1;
		// MESG("check_update_highlight: lines %ld->%ld quote=%d %d",oldlines,newlines,h1,hquotem);
	} else {
		// MESG("check_update_highlight: lines=%ld quote=%d",oldlines,hquotem);
		set_update(cwp,UPD_LINE);	// make it faster, shoulf have been UPD_LINE
		update_buffer_windows(fp,UPD_LINE);
	};
	prev_init=false;
 };
 return stat;
}



void init_line_highlight(WINDP *wp)
{
 wp->w_fp->hl->h_function(CHR_CR); 
 start_word_highlight=0;
 stop_word_highlight=999999;
 hstate=HS_LINESTART;
}

void set_selection(int f)
{
 hselection=f;
}

int get_selection()
{
 return hselection;
}

#define	TRACE_BACK 1000000

void init_highlight_structure(MLQUOTES *hs)
{
	hs->w_hquotem = 0;
	hs->w_prev_set = -1;
	hs->w_hselection =0;
	hs->w_slang=0;
	hs->w_notes=0;
	hs->w_first = 0;
	hs->w_in_array = 0;
	hs->flag_word = 0;
	hs->w_prev_space = 1;
	hs->single_quoted = 0;
	hs->double_quoted = 0;
	hs->w_bold = 0;
	hs->w_hquote_start = 0;
	hs->w_line_set=0;
	hs->w_hstate=0;
	hs->w_prev_line=0;
	hs->known_offset=0;
}

#if	0
void update_highlight(WINDP *wp)
{
 if(!syntaxh) return;
 offs hline_offset = tp_offset(wp->tp_hline);
	// MESG("update_highlight_none:");
	if(wp->hs[0].known_offset == hline_offset) return;
#if	1
	if(wp->hs[0].known_offset < hline_offset) {
#if	0	//check if fquote is ok!
		hquotem=0;
		slang=0;
		hselection=0;
		fquote_state(wp->hs[0].known_offset, 0,wp);
		highlight_save_state(wp,1,wp->hs[0].known_offset);
		MESG("uh: nkown_offset (%ld  %ld)",wp->hs[0].known_offset,wp->hs[1].known_offset);
		MESG("  : hquotem      (%X  %X)",wp->hs[0].w_hquotem,wp->hs[1].w_hquotem);
		MESG("  : slang        (%d  %d)",wp->hs[0].w_slang,wp->hs[1].w_slang);
#else
		highlight_restore_state(wp,0);
#endif
		fquote_state(hline_offset,wp->hs[0].known_offset,wp);
		highlight_save_state(wp,0,hline_offset);
	} else 
#endif
	{
		hquotem=0;
		slang=0;
		hselection=0;
		fquote_state(hline_offset, 0,wp);
		highlight_save_state(wp,0,hline_offset);
	};
}
#else
void update_highlight(WINDP *wp)
{
 if(!syntaxh) return;
 num known_offset=tp_offset(wp->tp_hsknown);
 // FindLineCol(wp->tp_hline);
 // MESG_time("update_highlight: known=%lld top=%lld",known_offset,tp_offset(wp->tp_hline));

//	prev_slash=prev_ast=hquote5=0;
//	previous top line is tp_hsknown
//	previous line with known state is tp_pknown
//	we must go to tp_hline
	
	note_type = (wp->w_fp->b_type >= NOTE_TYPE);
	if(note_type) { hnote=1;} else hnote=0;
//	MESG("update_highlight: note_type=%d ----------------------",note_type);

	highlight_restore_state(wp,0);	/* in any case read again current window top line highlight	*/
	if(tp_offset(wp->tp_hline) == tp_offset(wp->tp_hsknown)) {
		// MESG("	==");
		if(tp_offset(wp->tp_hline)==0) {
			if(note_type) { hnote=1;} else hnote=0;
			wp->w_fp->hl->h_function(CHR_RESET);
			highlight_save_state(wp,0,known_offset);
		} else {
			highlight_restore_state(wp,0);
		}
		return;
	};
	if(tp_offset(wp->tp_hline) > tp_offset(wp->tp_hsknown)) {
//		get info from tp_hsknown
		// MESG("	>");
#if	1
		known_offset = 	highlight_restore_state(wp,0); 
#else
		known_offset = tp_offset(wp->tp_hsknown); 
		highlight_restore_state(wp,0); 
#endif
		fquote_state(tp_offset(wp->tp_hline), known_offset,wp);
		known_offset = tp_offset(wp->tp_hline);
	} else if ( tp_offset(wp->tp_hline) > tp_offset(wp->tp_hmknown) && tp_offset(wp->tp_hline) - tp_offset(wp->tp_hmknown) < TRACE_BACK) 
	{
//		get info from wp_hmknown
		// MESG("	> 2");
		known_offset = highlight_restore_state(wp,1);
		fquote_state( tp_offset(wp->tp_hline), known_offset,wp);
		known_offset = tp_offset(wp->tp_hline);
	} else {
		if( tp_offset(wp->tp_hline) > TRACE_BACK ) {
			// MESG("	trace back");
//			set wp->tp_hmknown to tp_line - TRACE_BACK;
			if(is_wrap_text(wp->w_fp)) textpoint_set(wp->tp_hmknown,tp_offset(wp->tp_hline) - TRACE_BACK);
			else
			textpoint_set(wp->tp_hmknown,FLineBegin(wp->w_fp,tp_offset(wp->tp_hline) - TRACE_BACK));
			init_highlight();
			fquote_state(tp_offset(wp->tp_hmknown),0,wp);
			known_offset = tp_offset(wp->tp_hmknown);
//			save highlight info for tp_pknown
//			if(note_type) { hnote=1;} else hnote=0;
			highlight_save_state(wp,1,known_offset);
			fquote_state(tp_offset(wp->tp_hline), tp_offset(wp->tp_hmknown),wp);
			known_offset=tp_offset(wp->tp_hline);
		} else {
			// MESG("	init from 0");
			init_highlight();
			fquote_state(tp_offset(wp->tp_hline),0,wp);
			known_offset=0;
		} 
	};
//	if(note_type) { hnote=1;} else hnote=0;
	highlight_save_state(wp,0,known_offset);
	// MESG_time("update_highlight:end");
}
#endif

void update_highlight_line(WINDP *wp)
{
 if(!syntaxh) return;
//	prev_slash=prev_ast=hquote5=0;
//	previous top line is tp_hsknown
//	previous line with known state is tp_pknown
//	we must go to tp_hline
	// MESG("update_highlight_line:");
	highlight_restore_state(wp,0);	/* in any case read again current window top line highlight	*/
	hquotem=0;
	slang=1;
	hselection=0;
	highlight_save_state(wp,0,tp_offset(wp->tp_hline));
}

void update_highlight_none(WINDP *wp)
{
 if(!syntaxh) return;
//	prev_slash=prev_ast=hquote5=0;
//	previous top line is tp_hsknown
//	previous line with known state is tp_pknown
//	we must go to tp_hline
	// MESG("update_highlight_none:");
	highlight_restore_state(wp,0);	/* in any case read again current window top line highlight	*/
	hquotem=0;
	slang=0;
	hselection=0;
	highlight_save_state(wp,0,tp_offset(wp->tp_hline));
}

void highlight_c(int c)
{
//  MESG("h %c %3d flag=%d dquoted=%d",c,c,flag_word,double_quoted);
  if(highlight_note(c)) return;
 
  if(prev_set>=0) { hquotem=prev_set;prev_set=-1;};
 
  switch(c) {
	case (CHR_RESET) : // initialize
		hstate=0;
		slang=1;
		double_quoted=0;
		flag_word=0;
		h_prev_space=1;
		hquotem=0;
		prev_set=-1;
//		MESG("highlight_c: reset");
		break;
	/* double quotes */
	case CHR_DQUOTE:
		if(hstate==HS_PREVESC) { hstate=0;break;};
		if(hquotem&H_QUOTEC) break;
		// if(flag_word==2) { hstate=0;break;};
		if(hquotem&H_QUOTE5) { hstate=0; break;};
		// if(hquotem&H_QUOTE6) { hstate=0; break;};
//		if(single_quoted) { hstate=0;break;};
// 		hquotem = (hquotem&H_QUOTE2)? hquotem & ~H_QUOTE2: hquotem | H_QUOTE2;
		hquotem = (hquotem&H_QUOTE2)? 0: H_QUOTE2;
		double_quoted = (double_quoted)? 0:1;
		hstate=0;
		break;
	/* c comments */
	case '*': 
		if(hquotem&H_QUOTE2) { hstate=0;break;};
		if(hquotem!=H_QUOTE5) {
			if(hstate==HS_PREVSLASH) hquotem = H_QUOTEC;
		};
		hstate=HS_PREVAST;
		break;
	case '#':
		if(hquotem&H_QUOTE2) {  hstate=0;break;};
		if(hquotem&H_COMMENT) { hstate=0;break;};

		if(hstate==HS_LINESTART) {
			hquotem = (hquotem)? hquotem : H_QUOTE6;
				flag_word=1;
		};
		hstate=0;
		break;
	/* c,c++ comments */
	case '/':
		if(hquotem&H_QUOTE2) { hstate=0;break;};
		if(hquotem!=H_QUOTE2) {
			if(hstate==HS_PREVSLASH) prev_set=H_QUOTE5;
			else if(hstate==HS_PREVAST) hquotem &= ~H_QUOTEC;
		};
		if(hquotem!=H_QUOTEC && hquotem!=H_QUOTE2) hstate=HS_PREVSLASH;
		break;
	case CHR_SQUOTE: 
		if(!(hquotem&H_QUOTE2) ) hstate=HS_PREVESC;
		break;
	case CHR_BSLASH:
		if(hquotem&H_QUOTE2) {
			hstate=(hstate==HS_PREVESC)?0:HS_PREVESC;
		};
		break;
	case '\n':
	case CHR_CR:
		hquotem &= ~(H_QUOTE6|H_QUOTE4|H_QUOTE5|H_QUOTE9);
		hstate=HS_LINESTART;
		h_prev_space=1;
//		single_quoted=0;
//		double_quoted=0;
		flag_word=0;
		break;
	case '\t':
	case ' ':
		if(double_quoted) {hstate=0; break;};
//		flag_word=0;	/* Use this for the old style (all line is colored as preprocessor)  */
		if(hstate!=HS_LINESTART) hstate=0;
		if(!h_prev_space){
			if(flag_word==2) {
				hquotem &= ~H_QUOTE6;
				flag_word=3;
				hquotem |= H_QUOTE9;
			} 
			else
			if(flag_word==3) {
				hquotem &= ~H_QUOTE9;
				flag_word=0;
			};
			h_prev_space=1;			
		};
		break;
	case CHR_PARL:
		if(flag_word==3) break;
		if(flag_word>1) {
			hquotem &= ~H_QUOTE9;
			flag_word=0;
		};hstate=0;
		break;	
	default: { 
		hstate=0;
		if(flag_word==1) flag_word=2;
	};
  };
  h_prev_space = (c==' '||c=='\t');
}

void highlight_zig(int c)
{
//  MESG("h %c %3d flag=%d dquoted=%d",c,c,flag_word,double_quoted);
  if(highlight_note(c)) return;
 
  if(prev_set>=0) { hquotem=prev_set;prev_set=-1;};
 
  switch(c) {
	case (CHR_RESET) : // initialize
		hstate=0;
		slang=1;
		single_quoted=0;
		double_quoted=0;
		flag_word=0;
		h_prev_space=1;
		hquotem=0;
		prev_set=-1;
//		MESG("highlight_c: reset");
		break;
	/* double quotes */
	case CHR_DQUOTE:
		if(hquotem&H_QUOTEC) break;
//		if(flag_word==3) word_is_quoted=2;
		if(flag_word==2) { hstate=0;break;};
		if(hquotem&H_QUOTE5) { hstate=0; break;};
//		if(single_quoted) { hstate=0;break;};
		if(hstate!=HS_PREVESC) { hquotem = (hquotem&H_QUOTE2)? hquotem & ~H_QUOTE2: hquotem | H_QUOTE2;
			double_quoted = (double_quoted)? 0:1;
		};
		hstate=0;
		break;
	/* zig comments */
	case '/':
		if(hquotem&H_QUOTE2) { hstate=0;break;};
		if(hquotem!=H_QUOTE2) {
			if(hstate==HS_PREVSLASH) prev_set=H_QUOTE5;
			else if(hstate==HS_PREVAST) hquotem &= ~H_QUOTEC;
		};
		if(hquotem!=H_QUOTEC && hquotem!=H_QUOTE2) hstate=HS_PREVSLASH;
		break;
	case CHR_SQUOTE:
	case CHR_BSLASH:
		if(hquotem&H_QUOTE2) {
			hstate=(hstate==HS_PREVESC)?0:HS_PREVESC;
		};
		break;
	case '\n':
	case CHR_CR:
		hquotem &= ~(H_QUOTE6|H_QUOTE4|H_QUOTE5|H_QUOTE9);
		hstate=HS_LINESTART;
		h_prev_space=1;
//		single_quoted=0;
//		double_quoted=0;
		flag_word=0;
		break;
	case '\t':
	case ' ':
		if(double_quoted) {hstate=0; break;};
		if(single_quoted) {hstate=0; break;};
//		flag_word=0;	/* Use this for the old style (all line is colored as preprocessor)  */
		if(hstate!=HS_LINESTART) hstate=0;
		if(!h_prev_space){
			if(flag_word==2) {
				hquotem &= ~H_QUOTE6;
				flag_word=3;
				hquotem |= H_QUOTE9;
			} 
			else
			if(flag_word==3) {
				hquotem &= ~H_QUOTE9;
				flag_word=0;
			};
			
		};
		break;
#if	0
	case CHR_PARL:
		if(flag_word==3) break;
		if(flag_word>1) {
			hquotem &= ~H_QUOTE9;
			flag_word=0;
		};hstate=0;
		break;	
#endif
	default: { 
		hstate=0;
		if(flag_word==1) flag_word=2;
	};
  };
  h_prev_space = (c==' '||c=='\t');
}

void highlight_julia(int c)
{
//  MESG("h %c %3d flag=%d dquoted=%d",c,c,flag_word,double_quoted);
  if(highlight_note(c)) return;
  if(prev_set>=0) { hquotem=prev_set;prev_set=-1;};

  switch(c) {
	case (CHR_RESET) : // initialize
		hstate=0;
		slang=1;
		single_quoted=0;
		double_quoted=0;
		flag_word=0;
		hquotem=0;
		prev_set=-1;
//		MESG("highlight_julia: reset");
		break;
	/* single quotes */ 
	case CHR_SQUOTE: 
		if(hquotem&H_QUOTEC) break;
		if(flag_word==2) { hstate=0;break;};
		if(hquotem&H_QUOTE2) { hstate=0;break;};
//		if(double_quoted) { hstate=0;break;};
		if(hstate!=HS_PREVESC) { 
			if(hquotem) hquotem=hquotem & ~H_QUOTE1;
			else prev_set = H_QUOTE1;
			// hquotem = (hquotem)? hquotem & ~H_QUOTE1: hquotem | H_QUOTE1;
			single_quoted = (single_quoted)? 0:1;
		}; 
		hstate=0;
		break;
	/* double quotes */
	case CHR_DQUOTE:
		if(hquotem&H_QUOTEC) break;
		if(flag_word==2) { hstate=0;break;};
		if(hquotem&H_QUOTE1) { hstate=0;break;};
//		if(single_quoted) { hstate=0;break;};
		if(hstate!=HS_PREVESC) { hquotem = (hquotem)? hquotem & ~H_QUOTE2: hquotem | H_QUOTE2;
			double_quoted = (double_quoted)? 0:1;
		};
		hstate=0;
		break;
	/* julia mulitiline comments */
	case '=': 
		if(hquotem&H_QUOTE1 || hquotem&H_QUOTE2) { hstate=0;break;};
		if(hquotem!=H_QUOTE5) {
			if(hstate==HS_PREVSLASH) hquotem = H_QUOTEC;
		};
		hstate=HS_PREVAST;
		break;
	/* julia comments */
	case '#':
		if(hquotem&H_QUOTE1 || hquotem&H_QUOTE2) { hstate=0;break;};
		if(hquotem!=H_QUOTE2 && hquotem!=H_QUOTE1) {
			if(hstate==HS_PREVSLASH) prev_set=H_QUOTE5;
			else if(hstate==HS_PREVAST) hquotem &= ~H_QUOTEC;
		};
		if(hquotem!=H_QUOTEC && hquotem!=H_QUOTE2) hstate=HS_PREVSLASH;
		break;
	case '\\':{
		hstate=(hstate==HS_PREVESC)?0:HS_PREVESC;
		};
		break;
	case '\n':
	case CHR_CR:
		hquotem &= ~(H_QUOTE6|H_QUOTE4|H_QUOTE5|H_QUOTE9);
		hstate=HS_LINESTART;
		flag_word=0;
		break;
	case '\t':
	case ' ':
		if(double_quoted) {hstate=0; break;};
		if(single_quoted) {hstate=0; break;};

		if(hstate==HS_PREVSLASH) hquotem=H_QUOTE5;
		break;
	case CHR_PARL:
		if(flag_word==3) break;
		if(flag_word>1) {
			hquotem &= ~H_QUOTE9;
			flag_word=0;
		};hstate=0;
		break;	
	default: { 
		hstate=0;
		if(flag_word==1) flag_word=2;
	};
  };
}

void highlight_rust(int c)
{
  if(highlight_note(c)) return;

  if(prev_set>=0) { hquotem=prev_set;prev_set=-1;};

  switch(c) {
	case (CHR_RESET) : // initialize
		hstate=0;
		slang=1;
		prev_set=-1;
		break;
	/* double quotes */
	case CHR_DQUOTE:
		if(hquotem&H_QUOTEC) break;
		if(hstate!=HS_PREVESC) hquotem = (hquotem)? hquotem & ~H_QUOTE2: H_QUOTE2;
		hstate=0;
		break;
	/* c comments */
	case '*': 
		if(hquotem&H_QUOTE2) { hstate=0;break;};
		if(hquotem!=H_QUOTE5) {
			if(hstate==HS_PREVSLASH) hquotem = H_QUOTEC;
		};
		hstate=HS_PREVAST;
		break;
	case '#':
		if(hstate==HS_LINESTART) hquotem = (hquotem)? hquotem : H_QUOTE6;
		hstate=0;
		break;
	/* c,c++ comments */
	case '/':
		if(hquotem&H_QUOTE2) { hstate=0;break;};
		if(hquotem!=H_QUOTE2) {
			if(hstate==HS_PREVSLASH) prev_set=H_QUOTE5;
			else if(hstate==HS_PREVAST) hquotem &= ~H_QUOTEC;
		};
		if(hquotem!=H_QUOTEC && hquotem!=H_QUOTE2) hstate=HS_PREVSLASH;
		break;
	case CHR_SQUOTE:
	case '\\':{
		hstate=(hstate==HS_PREVESC)?0:HS_PREVESC;
		};
		break;
	case '\n':
	case CHR_CR:
		hquotem &= ~(H_QUOTE6|H_QUOTE4|H_QUOTE5);
		hstate=HS_LINESTART;
		break;
	case ' ':
	case '\t':
	case 0xA0:
	case 0xC2:
		if(hstate!=HS_LINESTART) hstate=0;
		break;		
	default: { 
		hstate=0;
	};
  };
}

/*
	Highlight html new style
*/
#define CHECK_WORDS		10

#define START_SCRIPT	2
#define STOP_SCRIPT		1
#define START_STYLE		4
#define	STOP_STYLE		3
#define START_COND		5
#define START_COMMENT	6
#define STOP_COMMENT	7
#define START_DATA		8
#define STOP_DATA		9
#define START_MDCODE	10

#define	LANG_SCRIPT		1
#define LANG_CSS		2
#define LANG_DATA		4

char *ssword[CHECK_WORDS] = {
	"</SCRIPT","<SCRIPT",
	"</STYLE","<STYLE",
	"<!--[", 
	"<!--", "-->",
	"![CDATA","!]]>"
	, "```"					/* md code */
};

int ssind[CHECK_WORDS];

int check_words(int c)
{
 int matchw=0;
 int i;

 if(c==0||c==CHR_LINE||c==CHR_RESET||c==' ') {
	for(i=0;i<CHECK_WORDS;i++) ssind[i]=0;
	return(0);
 } else {
 	if(c>='a' && c<='z') c-=32;
	for(i=0;i<CHECK_WORDS;i++) {
 		if ( ssword[i][ssind[i]]==c) {
			ssind[i]++;
			if(ssind[i]==strlen(ssword[i])) {
				ssind[i]=0;
				matchw=i+1;
				break;
			};
		} else ssind[i]=0;
	};
 };
 return(matchw);
}

void highlight_html(int c)
{
  int hstruct=0;
// NEW_STYLE
  if(highlight_note(c)) return;

  hstruct=check_words(c);
  switch(hstruct) {
  	case START_SCRIPT:
		if(!(hquotem & H_QUOTEC)){
			slang=LANG_SCRIPT;
			hquotem=0;
		}
		break;
	case STOP_SCRIPT:
		slang=0;
		break;
	case START_STYLE:
		if(!(hquotem & H_QUOTEC)){
			slang=LANG_CSS;
		};
		break;
	case STOP_STYLE:
		if(!(hquotem & H_QUOTEC)){
			slang=0;
		};
		break;
	case START_COND:
		if(!slang) { // only in html
			hquotem &= ~H_QUOTEC;
		};
		break;
	case START_DATA:
//		slang=LANG_DATA;
		break;
	case STOP_DATA:
//		slang=0;
		break;
	case START_COMMENT:
		if(!slang) { // only in html
			hquotem |= H_QUOTEC;
		};
		break;
	case STOP_COMMENT:
		if(!slang) { // only in html
			hquotem &= ~H_QUOTEC;
		};
		break;
  };

  switch(c) {
	case (CHR_RESET) : // initialize
//		MESG("html reset:");
		hstate=0;
		slang=0;
//		hquotem=0;
		break;
	/* single quotes */
	case CHR_SQUOTE: 
		if(slang || (hquotem & H_QUOTE7))
		if(hstate!=HS_PREVESC) {
			if(hquotem & H_QUOTE7 && !(hquotem & H_QUOTE2)) {
				if(hquotem == H_QUOTE7) {
					hquotem = H_QUOTE1 | H_QUOTE7;
				} else {
					hquotem = H_QUOTE7;  
				}
			} else hquotem = (hquotem)? hquotem & ~H_QUOTE1: H_QUOTE1;
		};
		hstate=0;
		break;
	// NEW_STYLE for ccs's
	case CHR_CURLL:
		if(!(hquotem & H_QUOTEC))
		if(slang==LANG_CSS && hquotem!=H_QUOTEC) hquotem=0;
		break;
	case CHR_CURLR:
		if(!(hquotem & H_QUOTEC))
		if(slang==LANG_CSS && hquotem!=H_QUOTEC) hquotem=0;
		break;
	case ':':
		if(!(hquotem & H_QUOTEC))
		if(slang==LANG_CSS && hquotem!=H_QUOTEC) hquotem=H_QUOTE7;
		break;
	case ';':{
		if(!(hquotem & H_QUOTEC))
		if(slang==LANG_CSS || slang==0) hquotem=0;
		break;}
	case '&':
		if(!(hquotem & H_QUOTEC))
		if(!slang && hquotem==0) hquotem=H_QUOTE7;
		break;

	/* double quotes */
	case CHR_DQUOTE:
		if(hquotem&H_QUOTEC) break;
//		if(hstate!=HS_PREVESC) hquotem = (hquotem)? hquotem & ~H_QUOTE2: H_QUOTE2;
		if(slang || (hquotem & H_QUOTE7))
		if(hstate!=HS_PREVESC) {
			if(hquotem & H_QUOTE7 && !(hquotem & H_QUOTE1)) {
				if(hquotem == H_QUOTE7) {
					hquotem = H_QUOTE2 | H_QUOTE7;
				} else {
					hquotem = H_QUOTE7;
				}
			} else hquotem = (hquotem)? hquotem & ~H_QUOTE2: H_QUOTE2;
		};
		hstate=0;
		break;
	/* c comments */
	case '*': 
		if(hquotem&H_QUOTE1 || hquotem&H_QUOTE2) { hstate=0;break;};
		if(slang==LANG_SCRIPT||slang==LANG_CSS){
			if(hquotem!=H_QUOTE5) {
				if(hstate==HS_PREVSLASH) hquotem = H_QUOTEC;
			};
			hstate=HS_PREVAST;
		};
		break;
	case '#':
		if(slang==LANG_SCRIPT){	
		if(hstate==HS_LINESTART) hquotem = (hquotem)? hquotem : H_QUOTE6;
		hstate=0;
		};
		break;
	/* comments */
	case CHR_SLASH:
		if(hquotem&H_QUOTE1 || hquotem&H_QUOTE2) { hstate=0;break;};
		if(hstate==HS_PSMALLER) break;
		if(slang==LANG_SCRIPT||slang==LANG_CSS) {
		if(hquotem!=H_QUOTE2 && hquotem!=H_QUOTE1) {
			if(hstate==HS_PREVSLASH) hquotem=H_QUOTE5;
			else if(hstate==HS_PREVAST) {hquotem &= ~H_QUOTEC;}
			else if(hstate!=HS_PREVESC ){
				if(hquotem & H_QUOTE7) hquotem &= ~H_QUOTE7;
				else if(hstate==HS_SPEC) hquotem |= H_QUOTE7;
			}
		};
		if(hquotem!=H_QUOTEC) hstate=HS_PREVSLASH;
		} // else hstate=0;
		break;
	case CHR_BSLASH:{
		hstate=(hstate==HS_PREVESC)?0:HS_PREVESC;
		};
		break;
	case CHR_LINE:
	case CHR_CR:
		hquotem &= ~(H_QUOTE6|H_QUOTE5);
		hstate=HS_LINESTART;
		break;
	case CHR_SMALLER:
//		MESG("	< slang=%d",slang);
		if(!slang) hstate=HS_PSMALLER;
		else hstate=0;
		break;
	case CHR_BIGER:
		if(slang==LANG_SCRIPT && !(hquotem&H_QUOTEC)) { 
			if(hstate==HS_QMARK || hstate==HS_ES2) {
				slang=0;
			};
		} else {
			if(hquotem & H_QUOTEC) {
				if(hstate==HS_ES2) hquotem=0;
			} else {
				hquotem=0;
			}
			if(hquotem & H_QUOTE8) hstate &= ~H_QUOTE8;
		};
		hstate=0;
		break;
	case '?':
		if(hstate==HS_PSMALLER) {
			slang=LANG_SCRIPT;
		}
		hstate=HS_QMARK;
		break;
	case CHR_PARL:
		hstate=HS_SPEC;
		break;
	case ' ':
	case '\t':
		if(hstate!=HS_LINESTART && hstate!=HS_PSMALLER && hstate!=HS_SPEC) hstate=0;
		if(hquotem==H_QUOTE8) hquotem=H_QUOTE7;
		break;		
	default: { 
		if(hstate==HS_PSMALLER && hquotem==0) hquotem = H_QUOTE8;
		hstate=0;
		
	};
  };
}


void highlight_css(int c)
{
 if(highlight_note(c)) return;
 if(c) highlight_html(c);
 else {
		slang=LANG_CSS;
		hquotem=0;
 }
}

void highlight_md(int c)
{
  int hstruct=0;
  // if(highlight_note(c)) return;

  hstruct=check_words(c);
  if(prev_set>=0) { hquotem=prev_set;prev_set=-1;hprev_line=-1;};

  if((c!='-' && c>31) && hquotem & H_LINESEP)
 	 { h_line_set=0; hquotem ^= H_LINESEP ;};

  switch(hstruct) {
	case START_COMMENT:
		if(!slang) { // only in html
			hquotem = H_QUOTEC;
		};
		break;
	case STOP_COMMENT:
		if(!slang) { // only in html
			hquotem =0;
		};break;
	case START_MDCODE:
		if(slang) {
			slang=0;
			hprev_line=0;
			hquotem=0;
			// MESG("end mdcode>");
		} else {
			slang=LANG_SCRIPT;
			hprev_line = H_QUOTE12;
			hquotem=0;
			// MESG("<start mdcode");
		};
		break;
  };

  switch(c) {
	case CHR_LINE:
	case CHR_CR:
#if	1
// ----
		// if(slang) MESG("LINE_START! hprev_line=%d hquotem=%d",hprev_line,hquotem);
		if(hprev_line>0 && slang) { 
			if(hprev_line==H_QUOTE12)
			{
				hquotem=hprev_line;
				//hprev_line=-1;
			} else {
				hquotem=0;
				hprev_line=-1;
			};
		}
// ----
#else
		// if(slang) MESG("LINE_START! hprev_line=%d hquotem=%d",hprev_line,hquotem);
		if(hprev_line>0 && slang) { 
			if(hprev_line==H_QUOTE12)
			{
				hquotem=hprev_line;
				//hprev_line=-1;
			} else {
				hquotem=0;
				hprev_line=-1;
			};
		}
#endif
		else {
			// hquotem &= ~(H_QUOTE1|H_QUOTE4|H_QUOTE5|H_QUOTE6|H_QUOTE10|H_QUOTE11|H_LINESEP);
			hquotem=0;
			hprev_line=-1;
		};
		hstate=HS_LINESTART;
		h_line_set=0;
		break;
	case (CHR_RESET) : // initialize
//		MESG("html reset:");
		hstate=HS_LINESTART;
		hprev_line=-1;
		prev_set=-1;
		slang=0;
		hquotem=0;
		h_bold=0;
		break;
	case '-':
		if(slang==1) break;
		if(hquotem & H_QUOTE12) break;
		if(hstate==HS_LINESTART) {
			h_line_set++;
			if(h_line_set==2) {
				hquotem=H_LINESEP;
				hstate=0;
				h_line_set=0;	
			} else {
				hquotem=H_QUOTE6;
			};
		};
		break;
	case '\\':
		if(slang==1) break;
		hstate=(hstate==HS_PREVESC)?0:HS_PREVESC;
		break;
	case '[':
		if(slang==1) break;
		if(hstate==HS_PREVESC) { hstate=0;break;};
		if(hquotem & H_QUOTE10||hquotem & H_QUOTE11|| hquotem & H_QUOTE12) break;
		prev_set = hquotem | H_QUOTE9;
		hstate=0;
		break;
	case ']':
		if(slang==1) break;
		if(hstate==HS_PREVESC) { hstate=0;break;};
		if(hquotem & H_QUOTE10||hquotem & H_QUOTE11|| hquotem & H_QUOTE12) break;
		hquotem &= ~(H_QUOTE9);
		// prev_set=-1;
		hstate=0;
		break;
	case CHR_DQUOTE:
		if(slang==1) break;
		if(hquotem & H_QUOTE12) {
		if(hstate!=HS_PREVESC) { 
			if(hquotem==H_QUOTE12) hquotem=H_QUOTE12+H_QUOTE2;
			else hquotem=H_QUOTE12;
		};
		// hstate=0;
		};break;

	case '`': // code block 
		if(slang==1) break;
		if(hstate==HS_PREVESC) { hstate=0;break;};
		if(hstate==HS_LINESTART) hquotem=0;
		if(slang==0) {
			if(hquotem & H_QUOTE11) {
				hquotem &= ~H_QUOTE11;
				prev_set=-1;
				hstate=0;
			} else {
				prev_set = hquotem | H_QUOTE11;
				hstate=0;
			};
		};
		break;
	case '#': {	// Headers
		if(slang==1) break;
		if(hstate==HS_PREVESC) { hstate=0;break;};
		if(hquotem & H_QUOTE10||hquotem & H_QUOTE11|| hquotem & H_QUOTE12) break;
		if(slang==0) {
			if(hquotem==H_QUOTE6) prev_set=H_QUOTE1;
			else if(hquotem==H_QUOTE1) {
				prev_set=H_QUOTE1;
				hquotem=H_QUOTE6;
			};
			if(hstate==HS_LINESTART){ 
				if(hquotem!=H_QUOTE12) hquotem=H_QUOTE6;
				hstate=0;
			};
			break;
		};
	};

	case '<':
		if(slang==1) break;
		if(hstate==HS_PREVESC) { hstate=0;break;};
		if(hquotem & H_QUOTE12) break;
		prev_set = H_COMMENT;
		hstate=0;
		h_hquote_start=hquotem;
		hquotem=H_QUOTE8;
		break;

	case CHR_BIGER:
		if(slang==1) break;
		if(hstate==HS_PREVESC) { hstate=0;break;};
		if(hquotem & H_QUOTE10||hquotem & H_QUOTE11|| hquotem & H_QUOTE12) break;
		if(hstate==HS_LINESTART) {
			prev_set = H_QUOTE10;
			hquotem=0;
			hstate=0;
		} else if(hquotem & H_QUOTE10) {
			prev_set = H_QUOTE10;
			hquotem=0;
			hstate=0;
		} else {
			prev_set = h_hquote_start;
			h_hquote_start=0;
			hquotem = H_QUOTE8;
			hstate=0;
		};
		break;
	case '_':
		if(slang==1) break;
		if(hstate==HS_PREVESC) { hstate=0;break;};
		if(hquotem & H_QUOTE10||hquotem & H_QUOTE11|| hquotem & H_QUOTE12) break;
		if(hstate==HS_PREVSLASH||hstate==HS_PREVSPACE||hstate==HS_LINESTART) {
			hstate=HS_PREVSLASH;
			h_bold++;
		} else {
			h_bold--;
		};
		if(h_bold==0) { 
			hquotem=0;
			hstate=0;
		};
		if(h_bold==1) {	/* italics  */
			prev_set=H_QUOTE9;
			hquotem=0;
		};
		if(h_bold==2) {	/* bold  */
			prev_set=H_QUOTE8;
			hquotem=0;
		};
		if(h_bold==3) {	/* bold+italics  */
			prev_set=H_QUOTE2;
			hquotem=0;
		};
		break;
	case '*':
		if(slang==1) break;
		if(hquotem & H_QUOTE6) break;
		if(hstate==HS_PREVESC) { hstate=0;break;};
		if(hquotem & H_QUOTE10||hquotem & H_QUOTE11|| hquotem & H_QUOTE12) break;
		if(hstate==HS_PREVAST||hstate==HS_PREVSPACE||hstate==HS_LINESTART) {
			hstate=HS_PREVAST;
			h_bold++;
		} else {
			h_bold--;
		};
		if(h_bold==0) { 
			hquotem=0;
			hstate=0;
		};
		if(h_bold==1) {	/* italics  */
			prev_set=H_QUOTE9;
			hquotem=0;
		};
		if(h_bold==2) {	/* bold  */
			prev_set=H_QUOTE8;
			hquotem=0;
		};
		if(h_bold==3) {	/* bold+italics  */
			prev_set=H_QUOTE2;
			hquotem=0;
		};
		break;
	case ' ':
	// case '\t':
		if(slang==1) break;
		if(h_bold==1) { hquotem=0; prev_set=0;h_bold=0;};
		if(hstate!=HS_LINESTART) hstate=HS_PREVSPACE;
		if(hquotem==H_LINESEP) hquotem=0;
		break;
		
	case '\t':
		if(slang==1) break;
		if(hstate!=HS_LINESTART) hstate=HS_PREVSPACE;
		else hquotem=0;
		break;		
	default: { 
		if(slang==1) break;
		if(hstate==HS_PSMALLER && hquotem==0) hquotem = H_QUOTE8;
		if(hstate==HS_PREVAST) { 
			hstate=HS_SPEC;
			// MESG(" set bold state to HS_SPEC=%d",hstate);
		} else 
		if(hstate==HS_PREVSLASH) { 
			hstate=HS_SPEC;
			// MESG(" set bold state to HS_SPEC=%d",hstate);
		} 

		else if(hstate!=HS_SPEC) hstate=0;
		if(hquotem==H_LINESEP) hquotem=0;		
	};
  };
}

void highlight_jscript(int c)
{
  if(highlight_note(c)) return;

  switch(c) {
	case (CHR_RESET) : // initialize
		hstate=0;
		slang=LANG_SCRIPT;
		break;
	case CHR_PARL:
	case '=':
		if(hstate==0) hstate=HS_ASSIGN;
		break;
	/* single quotes */
	case CHR_SQUOTE: 
		if(hquotem & H_QUOTE4) break;
		if(hquotem & H_QUOTE2) break;
		if(hquotem&H_QUOTEC) break;
		if(hstate==HS_PREVESC) { hstate=0;break;};
		if(!(hquotem & H_QUOTE7)) {
			if(hquotem & H_QUOTE4 && !(hquotem & H_QUOTE2)) {
				if(hquotem == H_QUOTE4) {
					hquotem = H_QUOTE1 | H_QUOTE4;
				} else {
					hquotem = H_QUOTE4;  
				}
			} else hquotem = (hquotem)? hquotem & ~H_QUOTE1: H_QUOTE1;
		};
		hstate=0;
		break;
	/* double quotes */
	// case '`':
	case CHR_DQUOTE:
		if(hquotem&H_QUOTEC) break;
		if(hstate==HS_PREVESC) { hstate=0;break;};
		if(!(hquotem & H_QUOTE7)) {
			if(hquotem & H_QUOTE4 && !(hquotem & H_QUOTE1)) {
				if(hquotem == H_QUOTE4) {
					hquotem = H_QUOTE2 | H_QUOTE4;
				} else {
					hquotem = H_QUOTE4;
				}
			} else hquotem = (hquotem)? hquotem & ~H_QUOTE2: H_QUOTE2;
		};
		hstate=0;
		break;
	/* c comments */
	case '*': 
		if(hquotem & H_QUOTE2) break;
		if(hquotem & H_QUOTE4) break;
		if(hstate==HS_PREVESC) { hstate=0;break;};
		if(hquotem!=H_QUOTE5) {
			if(hstate==HS_PREVSLASH) hquotem = H_QUOTEC;
		};
		hstate=HS_PREVAST;
		break;
	case '#':
		if(hquotem & H_QUOTE2) break;
		if(hquotem & H_QUOTE4) break;
		if(hstate==HS_LINESTART) hquotem = (hquotem)? hquotem : H_QUOTE6;
		hstate=0;
		break;
	/* comments */
	case '/':
		if(hquotem & H_QUOTE2) break;
		if(hquotem & H_QUOTE4) break;
		if(hstate==HS_ASSIGN) { 
			hstate=0;
			hquotem=H_QUOTE12;break;
		};
		if(hstate==HS_PREVESC) { hstate=0;break;};
		if(hquotem==H_QUOTE12) { hstate=0;hquotem=0;break;};
		if(hquotem!=H_QUOTE2 && hquotem!=H_QUOTE1) {
			if(hstate==HS_PREVSLASH) hquotem=H_QUOTE5;
			else if(hstate==HS_PREVAST && hquotem!=H_QUOTE12) {hquotem &= ~H_QUOTEC;hstate=0;break;};
			if(hquotem & H_QUOTE7) hquotem &= ~H_QUOTE7;
			// else if(hstate==HS_SPEC) hquotem |= H_QUOTE7;
		};
		// if(hquotem!=H_QUOTEC && hquotem!=H_QUOTE2) hstate=HS_PREVSLASH;
		if(hquotem==0) hstate=HS_PREVSLASH;
		break;
	case '\\':{
		hstate=(hstate==HS_PREVESC)?0:HS_PREVESC;
		};
		break;
	case '\n':
	case CHR_CR:
		hquotem &= ~(H_QUOTE6|H_QUOTE5|H_QUOTE4|H_QUOTE12);
		hstate=HS_LINESTART;
		break;
	case '!':
		if(hstate==HS_PSMALLER) hquotem |= H_QUOTEC;
		hstate=0;
		break;
	case '-':
		if(hquotem==H_QUOTEC) {
			if(hstate==HS_END1) hstate=HS_END2;
			else hstate=HS_END1;
		} else hstate=0;
		break;
#if	0
	case CHR_SMALLER:
//		if(hquotem==0) hquotem=H_QUOTE4;
		hstate=HS_PSMALLER;
		break;
	case CHR_BIGER:
		if(hquotem==H_QUOTE4) hquotem=0;
		else 
		if(hquotem & H_QUOTEC) hquotem &= ~H_QUOTEC;
		hstate=0;
		break;
#endif
	case '?':
		if(hstate==HS_PSMALLER) hquotem=0;
		hstate=0;
		break;
	case ' ':
	case '\t':
		if(hstate!=HS_LINESTART && hstate!=HS_PSMALLER && hstate!=HS_SPEC && hstate!=HS_ASSIGN) hstate=0;
		break;		
	default: { 
		if(hstate==HS_PSMALLER && hquotem==0) hquotem = H_QUOTE4;
		hstate=0;
	};
  };
}

void highlight_perl(int c)
{
  if(highlight_note(c)) return;

  switch(c) {
	case (CHR_RESET) : // initialize
		hstate=0;
		slang=LANG_SCRIPT;
		break;
	/* single quotes */
	case CHR_SQUOTE: 
//		if(hstate!=HS_PREVESC) hquotem = (hquotem)? hquotem & ~H_QUOTE1: H_QUOTE1;
		if(hstate!=HS_PREVESC) {
			if(hquotem & H_QUOTE4 && !(hquotem & H_QUOTE2)) {
				if(hquotem == H_QUOTE4) {
					hquotem = H_QUOTE1 | H_QUOTE4;
				} else {
					hquotem = H_QUOTE4;  
				}
			} else hquotem = (hquotem)? hquotem & ~H_QUOTE1: H_QUOTE1;
		};
		hstate=0;
		break;
	/* double quotes */
	case CHR_DQUOTE:
		if(hquotem&H_QUOTE5) break;
		if(hstate!=HS_PREVESC) {
			if(hquotem & H_QUOTE4 && !(hquotem & H_QUOTE1)) {
				if(hquotem == H_QUOTE4) {
					hquotem = H_QUOTE2 | H_QUOTE4;
				} else {
					hquotem = H_QUOTE4;
				}
			} else hquotem = (hquotem)? hquotem & ~H_QUOTE2: H_QUOTE2;
		};
		hstate=0;
		break;
	/* comments */
	case '#':
		if((hstate!=HS_PREVESC) && (hquotem!=H_QUOTE2 && hquotem!=H_QUOTE1)) {
			hquotem |= H_QUOTE5;
		};
		hstate=0;
		break;
	case '\\':
	case '$':{
		hstate=(hstate==HS_PREVESC)?0:HS_PREVESC;
		};
		break;
	case '\n':
	case CHR_CR:
		hquotem &= ~(H_QUOTE6|H_QUOTE4|H_QUOTE5|H_QUOTE9);
		hstate=HS_LINESTART;
		break;
	case '!':
		if(hstate==HS_PSMALLER) hquotem |= H_QUOTEC;
		hstate=0;
		break;
	case '-':
		if(hquotem==H_QUOTEC) {
			if(hstate==HS_END1) hstate=HS_END2;
			else hstate=HS_END1;
		} else hstate=0;
		break;
	case '?':
		if(hstate==HS_PSMALLER) hquotem=0;
		hstate=0;
		break;
	case CHR_PARL:
		hstate=HS_SPEC;
		break;
	case ' ':
	case '\t':
		if(hstate!=HS_LINESTART && hstate!=HS_PSMALLER && hstate!=HS_SPEC) hstate=0;
		break;		
	default: { 
		if(hstate==HS_PSMALLER && hquotem==0) hquotem = H_QUOTE4;
		hstate=0;
	};
  };
}

// powershell
void highlight_ps1(int c)
{
  if(highlight_note(c)) return;

  switch(c) {
	case (CHR_RESET) : // initialize
		hstate=0;
		slang=LANG_SCRIPT;
		break;
	/* single quotes */
	case CHR_SQUOTE: 
//		if(hstate!=HS_PREVESC) hquotem = (hquotem)? hquotem & ~H_QUOTE1: H_QUOTE1;
		if(hstate!=HS_PREVESC) {
			if(hquotem & H_QUOTE4 && !(hquotem & H_QUOTE2)) {
				if(hquotem == H_QUOTE4) {
					hquotem = H_QUOTE1 | H_QUOTE4;
				} else {
					hquotem = H_QUOTE4;  
				}
			} else hquotem = (hquotem)? hquotem & ~H_QUOTE1: H_QUOTE1;
		};
		hstate=0;
		break;
	/* double quotes */
	case CHR_DQUOTE:
		if(hquotem&H_QUOTE5) break;
		if(hstate!=HS_PREVESC) {
			if(hquotem & H_QUOTE4 && !(hquotem & H_QUOTE1)) {
				if(hquotem == H_QUOTE4) {
					hquotem = H_QUOTE2 | H_QUOTE4;
				} else {
					hquotem = H_QUOTE4;
				}
			} else hquotem = (hquotem)? hquotem & ~H_QUOTE2: H_QUOTE2;
		};
		hstate=0;
		break;
	/* comments */
	case '#':
		if((hstate!=HS_PREVESC) && (hquotem!=H_QUOTE2 && hquotem!=H_QUOTE1)) {
			hquotem |= H_QUOTE5;
		};
		hstate=0;
		break;
	//case '\\':
	case '$':{
		hstate=(hstate==HS_PREVESC)?0:HS_PREVESC;
		};
		break;
	case '\n':
	case CHR_CR:
		hquotem &= ~(H_QUOTE6|H_QUOTE4|H_QUOTE5|H_QUOTE9);
		hstate=HS_LINESTART;
		break;
	case '!':
		if(hstate==HS_PSMALLER) hquotem |= H_QUOTEC;
		hstate=0;
		break;
	case '-':
		if(hquotem==H_QUOTEC) {
			if(hstate==HS_END1) hstate=HS_END2;
			else hstate=HS_END1;
		} else hstate=0;
		break;
	case '?':
		if(hstate==HS_PSMALLER) hquotem=0;
		hstate=0;
		break;
	case CHR_PARL:
		hstate=HS_SPEC;
		break;
	case ' ':
	case '\t':
		if(hstate!=HS_LINESTART && hstate!=HS_PSMALLER && hstate!=HS_SPEC) hstate=0;
		break;		
	default: { 
		if(hstate==HS_PSMALLER && hquotem==0) hquotem = H_QUOTE4;
		hstate=0;
	};
  };
}


void highlight_json(int c)
{
  if(highlight_note(c)) return;

  switch(c) {
	case (CHR_RESET) : // initialize
		hstate=0;
		first=1;
		in_array=0;
		break;
	case ':' :
		if(hstate==0) first=0;
		hstate=0;
		break;
	case CHR_CURLL:
		if(hquotem==0) { if(hstate==0) { first=1;in_array=0;};};
		hstate=0;
		break;
	case CHR_CURLR:
		if(hquotem==0) { if(hstate==0) first=1;};
		hstate=0;
		break;
	case CHR_DQUOTE:
		if(hstate==HS_PREVESC) { hstate=0;break;};
		if(hquotem == 0) {
			if(first) hquotem = H_QUOTE2;
			else hquotem = H_QUOTE5;
		
		} else hquotem=0;
		hstate=0;
		break;

	case '\n':
	case CHR_CR:
		hquotem = 0;
		hstate=HS_LINESTART;
		break;

	case '#':
		if(hstate==HS_LINESTART) hquotem = (hquotem)? hquotem : H_QUOTE6;
		hstate=0;
		break;
	case '\\':{
		hstate=(hstate==HS_PREVESC)?0:HS_PREVESC;
		};
		break;
	case ',':
		if(in_array==0) first=1; 
		hstate=0;
		break;
	case CHR_LBRA:
		if(hquotem==0) {
			in_array=1;
			first=0;
		};
		hstate=0;
		break;
		
	case CHR_RBRA:
		if(hquotem==0) {
			in_array=0;
			first=1;
		};
		hstate=0;
		break;
	case CHR_FLUSH:
		break;
	default: { 
		hstate=0;
	};
  };
}

void highlight_ini(int c)
{
  if(highlight_note(c)) return;
  if(prev_set>=0) { hquotem=prev_set;prev_set=-1;};

  switch(c) {
	case (CHR_RESET) : // initialize
		hstate=0;
		first=1;
		in_array=0;
		break;
#if	1
	case '=':
		if(hquotem==0) { prev_set = H_QUOTE1;};
		hstate=0;
		break;
#endif
	case '\n':
	case CHR_CR:
		hquotem = 0;
		hstate=HS_LINESTART;
		first=1;
		break;

	case ';':
		if(hquotem==0 || hquotem==H_QUOTE1) hquotem = H_COMMENT;
		hstate=0;
		break;
	case CHR_LBRA:
		if(hquotem==0) {
			prev_set = H_QUOTE6;
		};
		hstate=0;
		break;
		
	case CHR_RBRA:
		if(hquotem) hquotem=0;
		hstate=0;
		break;

	case CHR_FLUSH:
		break;
	default: { 
		hstate=0;
	};
  };
}


void highlight_bicep(int c)
{
  if(highlight_note(c)) return;

  if(prev_set) hquotem=prev_set;
  prev_set=0;

  switch(c) {
	case (CHR_RESET) : // initialize
		hstate=0;
		first=1;
		in_array=0;
		prev_set=0;
		slang=LANG_SCRIPT;
		break;
	case ':' :
		if(hstate==0) first=0;
		break;
	case CHR_CURLL:
		if(hquotem==0) { if(hstate==0) { first=1;in_array=0;};};
		break;
	case CHR_CURLR:
		if(hquotem==0) { if(hstate==0) first=1;};
		break;
	case CHR_DQUOTE:
		if(hstate==HS_PREVESC) { hstate=0;break;};
		if(first) {
			if(hquotem == 0 && prev_set==0) prev_set=H_QUOTE2;
			else {
				hquotem = 0;
			}
		} else {
			if(hquotem == 0 && prev_set==0) {
				prev_set=H_QUOTE5;
			} else {
				hquotem =0;
			}
		}
		hstate=0;
		break;

	case '/':
		if(hquotem&H_QUOTE1 || hquotem&H_QUOTE2) { hstate=0;break;};
		if(hquotem!=H_QUOTE2 && hquotem!=H_QUOTE1) {
			if(hstate==HS_PREVSLASH) hquotem |=H_QUOTE5;
			else hstate=HS_PREVSLASH;
		};
		break;
	case '\n':
	case CHR_CR:
		hquotem = 0;
		hstate=HS_LINESTART;
		break;

	case '#':
		if(hstate==HS_LINESTART) hquotem = (hquotem)? hquotem : H_QUOTE6;
		hstate=0;
		break;
	case '\\':{
		hstate=(hstate==HS_PREVESC)?0:HS_PREVESC;
		};
		break;
	case ',':
		if(in_array==0) first=1; 
		break;
	case CHR_LBRA:
		if(hquotem==0) {
			in_array=1;
			first=0;
		};
		break;
	case CHR_RBRA:
		if(hquotem==0) {
			in_array=0;
			first=1;
		};
		break;
	default: { 
		hstate=0;
	};
  };
}

void highlight_terraform(int c)
{
  if(highlight_note(c)) return;

  switch(c) {
	case (CHR_RESET) : // initialize
		hstate=0;
		slang=LANG_SCRIPT;
		break;
	/* single quotes */
	case CHR_SQUOTE: 
//		if(hstate!=HS_PREVESC) hquotem = (hquotem)? hquotem & ~H_QUOTE1: H_QUOTE1;
		if(hstate!=HS_PREVESC && !(hquotem & H_QUOTE7)) {
			if(hquotem & H_QUOTE4 && !(hquotem & H_QUOTE2)) {
				if(hquotem == H_QUOTE4) {
					hquotem = H_QUOTE1 | H_QUOTE4;
				} else {
					hquotem = H_QUOTE4;  
				}
			} else hquotem = (hquotem)? hquotem & ~H_QUOTE1: H_QUOTE1;
		};
		hstate=0;
		break;
	/* double quotes */
	case CHR_DQUOTE:
		if(hquotem&H_QUOTEC) break;
//		if(hstate!=HS_PREVESC) hquotem = (hquotem)? hquotem & ~H_QUOTE2: H_QUOTE2;
		if(hstate!=HS_PREVESC && !(hquotem & H_QUOTE7)) {
			if(hquotem & H_QUOTE4 && !(hquotem & H_QUOTE1)) {
				if(hquotem == H_QUOTE4) {
					hquotem = H_QUOTE2 | H_QUOTE4;
				} else {
					hquotem = H_QUOTE4;
				}
			} else hquotem = (hquotem)? hquotem & ~H_QUOTE2: H_QUOTE2;
		};
		hstate=0;
		break;
	/* c comments */
	case '*': 
		if(hquotem!=H_QUOTE5) {
			if(hstate==HS_PREVSLASH) hquotem = H_QUOTEC;
		};
		hstate=HS_PREVAST;
		break;
	case '#':
		if(hstate==HS_LINESTART) hquotem = (hquotem)? hquotem : H_QUOTE6;
		hstate=0;
		break;
	/* comments */
	case '/':
//		if(hstate==HS_PSMALLER) break;
		if(hquotem!=H_QUOTE2 && hquotem!=H_QUOTE1) {
			if(hstate==HS_PREVSLASH) hquotem=H_QUOTE5;
			else if(hstate==HS_PREVAST) hquotem &= ~H_QUOTEC;
			else if(hstate!=HS_PREVESC ){
				if(hquotem & H_QUOTE7) hquotem &= ~H_QUOTE7;
				else if(hstate==HS_SPEC) hquotem |= H_QUOTE7;
			}
		};
		if(hquotem!=H_QUOTEC && hquotem!=H_QUOTE2) hstate=HS_PREVSLASH;
		break;
	case '\\':{
		hstate=(hstate==HS_PREVESC)?0:HS_PREVESC;
		};
		break;
	case '\n':
	case CHR_CR:
		hquotem &= ~(H_QUOTE6|H_QUOTE5|H_QUOTE4);
		hstate=HS_LINESTART;
		break;
	case '!':
		if(hstate==HS_PSMALLER) hquotem |= H_QUOTEC;
		hstate=0;
		break;
	case '-':
		if(hquotem==H_QUOTEC) {
			if(hstate==HS_END1) hstate=HS_END2;
			else hstate=HS_END1;
		} else hstate=0;
		break;
#if	0
	case CHR_SMALLER:
//		if(hquotem==0) hquotem=H_QUOTE4;
		hstate=HS_PSMALLER;
		break;
	case CHR_BIGER:
		if(hquotem==H_QUOTE4) hquotem=0;
		else 
		if(hquotem & H_QUOTEC) hquotem &= ~H_QUOTEC;
		hstate=0;
		break;
#endif
	case '?':
		if(hstate==HS_PSMALLER) hquotem=0;
		hstate=0;
		break;
	case CHR_PARL:
		hstate=HS_SPEC;
		break;
	case ' ':
	case '\t':
		if(hstate!=HS_LINESTART && hstate!=HS_PSMALLER && hstate!=HS_SPEC) hstate=0;
		break;		
	default: { 
		if(hstate==HS_PSMALLER && hquotem==0) hquotem = H_QUOTE4;
		hstate=0;
	};
  };
}


void highlight_yaml(int c)
{
  if(highlight_note(c)) return;

  line_col++;

  if(prev_set>0) hquotem=prev_set;
  prev_set=0;

  switch(c) {

	case CHR_CR:
	case CHR_LINE:
		hquotem=0;
		hstate=HS_LINESTART;
		line_col=0;
		break;
	case (CHR_RESET) : // initialize
		hstate=0;
		first=1;
		in_array=0;
		prev_set=0;
		hquotem=0;
		break;
	case ':' :
		if(!(hquotem == H_QUOTE6 || hquotem==0)) break;
		if(hquotem != H_QUOTE6){
		if(hstate==0) first=0;
		stop_word_highlight=line_col;
		hquotem=H_QUOTE4;
		};
		break;
	case CHR_CURLL:
		if(hquotem != H_QUOTE6)
		if(hstate==0) { first=1;in_array=0;};
		break;
	case CHR_CURLR:
		if(hquotem != H_QUOTE6)
		if(hstate==0) first=1;
		break;

	case CHR_SQUOTE:
		if(!(hquotem&H_QUOTE1 || hquotem==0)) break;
		if(hquotem != H_QUOTE6 && !(hquotem&H_QUOTE2)){
			if(hstate==HS_PREVESC) { hstate=0;break;};
			if(first) {
				if(hquotem == 0 && prev_set==0) prev_set=H_QUOTE1;
				else {
					hquotem = 0;
				}
			} else {
				if(hquotem == 0 && prev_set==0) {
					prev_set=H_QUOTE5;
				} else {
					hquotem =0;
				}
			}
			hstate=0;
		};break;

	case CHR_DQUOTE:
		if(hquotem&H_QUOTEC) break;
		if(hquotem  != H_QUOTE6){
			if(hstate==HS_PREVESC) { hstate=0;break;};
			if(first) {
				if((hquotem == 0||hquotem==H_QUOTE4) && prev_set==0) prev_set=H_QUOTE2;
				else {
					hquotem = 0;
				}
			} else {
				if((hquotem == 0||hquotem==H_QUOTE4) && prev_set==0) {
					prev_set=H_QUOTE5;
				} else {
					hquotem =0;
				}
			}
			hstate=0;
		};
		break;

	case '#':
		if(hstate==0) break;
		if(hquotem != H_QUOTE1 && hquotem != H_QUOTE2) {
			hquotem = H_QUOTE6;
			hstate=0;
		};
		break;

	case '\\':
		if(hquotem != H_QUOTE6)	{
			hstate=(hstate==HS_PREVESC)?0:HS_PREVESC;
		};
		break;
	case '-':
		if(hquotem==H_QUOTEC) {
			if(hstate==HS_END1) hstate=HS_END2;
			else hstate=HS_END1;
		} else hstate=0;
		first=0;
		break;
	case ',':
		if(hquotem != H_QUOTE6)
		if(in_array==0) first=1; 
		break;
	case CHR_LBRA:
		if(hquotem==0) {
			in_array=1;
			first=0;
		};
		break;
	case CHR_RBRA:
		if(hquotem==0) {
			in_array=0;
			first=1;
		};
		break;
	case ' ':
	case CHR_TAB: 
		break;
	default: { 
		hstate=0;
	};
  };
}

void highlight_matlab(int c)
{
 static int dot_count=0;
 static int dotm=-1;

  if(highlight_note(c)) return;

  if(c=='.') {
  	dot_count++;
	if(dot_count==3) {
		if(hquotem==H_QUOTE5) {
			if (dotm!=-1) hquotem=dotm;
		} else {
			dotm=hquotem;
			hquotem=H_QUOTE5;
		};
		dot_count=0;
	};
	return;
  } else dot_count=0;

  switch(c) {
	case (CHR_RESET) : // initialize
		hstate=0;
		slang=1;
		break;
	/* double quotes */
	case CHR_DQUOTE:
		if(hstate!=HS_PREVESC) hquotem = (hquotem)? hquotem & ~H_QUOTE2: H_QUOTE2;
		hstate=0;
		break;
	/* c comments */
	case CHR_CURLL: 	/* block comment start  */
		if(hstate==HS_PREVSLASH) {
			hquotem = H_QUOTEC;
			dotm=-1;
		}
//		hstate=HS_PREVAST;
		break;
	case CHR_CURLR: 	/* block comment end  */
//		if(hquotem==H_QUOTE5) 
		{
			if(hstate==HS_PREVSLASH) hquotem = 0;
		};
//		hstate=HS_PREVAST;
		break;
	case '#':
		if(hstate==HS_LINESTART) hquotem = (hquotem)? hquotem : H_QUOTE6;
		hstate=0;
		break;
	/*  comments */
	case '%':
		if(hquotem!=H_QUOTE2 && hquotem!=H_QUOTE1) {
			if(hquotem!=H_QUOTEC) {// ??
				hquotem=H_QUOTE5;
				dotm=-1;
			};
			hstate=HS_PREVSLASH;
		};
		break;
	case '\\':{
		hstate=(hstate==HS_PREVESC)?0:HS_PREVESC;
		};
		break;
	case '\n':
	case CHR_CR:
		hquotem &= ~(H_QUOTE6|H_QUOTE4|H_QUOTE5);
		hstate=HS_LINESTART;
		break;
	case ' ':
	case '\t':
		if(hstate!=HS_LINESTART) hstate=0;
		break;		
	default: { 
		hstate=0;
	};
  };
}

void highlight_pascal(int c)
{
  if(highlight_note(c)) return;

	switch(c){
		case CHR_RESET:
			hquotem=0;
			slang=LANG_SCRIPT;
			break;
		case CHR_CR:
		case CHR_LINE:
			hstate=HS_LINESTART;
			break;
		case CHR_CURLL:
			hquotem &= H_QUOTEC;
			hstate=0;
			break;
		case CHR_CURLR:
			hquotem &= ~H_QUOTEC;
			hstate=0;
			break;
		default:
			hstate=HS_LINESTART;
	};
}

void highlight_python(int c)
{
  if(highlight_note(c)) return;

  switch(c) {
	case CHR_RESET:
		hstate=0;
		// prev_dquote=0;
		// prev_squote=0;
		single_quoted=0;
		double_quoted=0;
		slang=LANG_SCRIPT;
		break;
	/* single quotes */
	case CHR_SQUOTE: 
		// if(hquotem&H_QUOTEC) break;
		if(hstate!=HS_PREVESC) {
			if(single_quoted==2) {
				hquotem = (hquotem & H_COMMENT)? hquotem & ~H_COMMENT: H_COMMENT;
			} else {
				if(single_quoted==1) {
					single_quoted=2;
					hquotem = (hquotem)? hquotem & ~H_QUOTE1: H_QUOTE1;
				} else {
					hquotem = (hquotem)? hquotem & ~H_QUOTE1: H_QUOTE1;
					single_quoted=1;
				}
			}
		};
		hstate=0;
		double_quoted=0;
		break;
	case '\\': {
		hstate=(hstate==HS_PREVESC)?0:HS_PREVESC;
		};
		break;
	/* double quotes */
	case CHR_DQUOTE:
		// if(hquotem&H_QUOTEC) break;
		if(hstate!=HS_PREVESC) {
			if(double_quoted==2) {
				hquotem = (hquotem & H_COMMENT)? hquotem & ~H_COMMENT: H_COMMENT;
				double_quoted=0;
			} else {
				if (double_quoted==1) {
					double_quoted=2;
					hquotem = (hquotem)? hquotem & ~H_QUOTE2: H_QUOTE2;
				} else {
					hquotem = (hquotem)? hquotem & ~H_QUOTE2: H_QUOTE2;
					double_quoted=1;
				}
			}
		};
		hstate=0;
		single_quoted=0;
		
		break;
	case '#': { // comment line
//		if(hstate==HS_LINESTART) 
			hquotem = (hquotem)? hquotem : H_QUOTE6;
		hstate=0;
		break;
	};
	case CHR_CR:
	case CHR_LINE:
		hstate=HS_LINESTART;
		double_quoted=0;
		single_quoted=0;

		if(hquotem & H_COMMENT) hquotem=H_COMMENT;	// comments continues on next line
		else
		if(hquotem & H_QUOTE2) hquotem=H_QUOTE2;	// double quote continues on next line
		else hquotem=0;

		break;
	case ' ':
	case '\t':
		if(hstate!=HS_LINESTART) hstate=0;
		double_quoted=0;
		single_quoted=0;
		break;		
	default: { 
		hstate=0;
		double_quoted=0;
		single_quoted=0;
	};
   }  
}



void highlight_other(int c)
{

  if(highlight_note(c)) return;
  switch(c) {
	case CHR_RESET:
		hstate=0;
		slang=LANG_SCRIPT;
		break;
	case CHR_SQUOTE: 
		if(hstate!=HS_LETTER) {
			if(hstate!=HS_PREVESC) {
				hquotem = (hquotem)? hquotem & ~H_QUOTE1: H_QUOTE1;
			}
		} else {
			hquotem &= ~H_QUOTE1;
		};
		hstate=0;
		break;
	/* double quotes */
	case CHR_DQUOTE:
		if(hstate!=HS_LETTER) {
			if(hstate!=HS_PREVESC) hquotem = (hquotem)? hquotem & ~H_QUOTE2: H_QUOTE2;
		} else {
			hquotem &= ~H_QUOTE2;
		};
		hstate=0;
		break;
	case '#': {
//		if(hstate==HS_LINESTART) 
		if(!(hstate & HS_PREVSPACE)) {
			if(!hquotem) hquotem=H_QUOTE6;
		};
		hstate=0;
		break;
	};
	case ';': {
		if(hstate==HS_LINESTART) hquotem=H_QUOTE5;
		hstate=0;
		break;
	};
	case CHR_BSLASH:
		hstate=(hstate==HS_PREVESC)?0:HS_PREVESC;
		break;
	case '%': 
	case '!':
		if(hstate==HS_LINESTART) hquotem=H_QUOTE4;
		hstate=0;
		break;
	case CHR_CR:
	case CHR_LINE:
		if(hquotem!=H_QUOTE2) hquotem = 0;
		hstate=HS_LINESTART;
		break;
	case ' ':
	case '\t':
		hstate=HS_PREVSPACE;
		break;		
	default: { 
		if((c>='A' && c<='Z') || (c>='a' && c<='z') || c>128) hstate |= HS_LETTER;
		else hstate=0;
	};
  };
}


int highlight_note(int c)
{
  if(hnote==0) return false;
  switch(c) {
	case 12:
		hnote=0;
		return true;
	case CHR_RESET:
		hstate=0;
		slang=LANG_SCRIPT;
//		MESG("highlight_note: reset: slang=%d",slang);
		break;
	/* single quotes */
	case CHR_SQUOTE: 
		if(hstate!=HS_LETTER) {
			if(hstate!=HS_PREVESC) {
				hquotem = (hquotem)? hquotem & ~H_QUOTE1: H_QUOTE1;
			}
		} else {
			hquotem &= ~H_QUOTE1;
		};
		hstate=0;
		break;

	case CHR_DQUOTE:
		if(hstate!=HS_LETTER) {
			if(hstate!=HS_PREVESC) hquotem = (hquotem)? hquotem & ~H_QUOTE2: H_QUOTE2;
		} else {
			hquotem &= ~H_QUOTE2;
		};
		hstate=0;
		break;
	case '#': {
//		if(hstate==HS_LINESTART) 
		if(!hquotem) hquotem=H_QUOTE6;
		hstate=0;
		break;
	};
	case ';': {
		if(hstate==HS_LINESTART) hquotem=H_QUOTE5;
		hstate=0;
		break;
	};
	case CHR_BSLASH:
		hstate=(hstate==HS_PREVESC)?0:HS_PREVESC;
		break;
	case CHR_CR:
	case CHR_LINE:
		if(hquotem!=H_QUOTE2) hquotem = 0;
		hstate=HS_LINESTART;
		break;
	case ' ':
	case '\t':
		if(hstate!=HS_LINESTART) hstate=0;
		break;		
	default: { 
		if((c>='A' && c<='Z') || (c>='a' && c<='z') || c>128) hstate |= HS_LETTER;
		else hstate=0;
	};
  };
  return false;
}

void highlight_shell(int c)
{
  switch(c) {
	case 12:
	case CHR_RESET:
		hstate=0;
		slang=LANG_SCRIPT;
		break;
	/* double quotes */
	case CHR_SQUOTE: 
		if(hstate!=HS_LETTER) {
			if(hstate!=HS_PREVESC) {
				hquotem = (hquotem)? hquotem & ~H_QUOTE1: H_QUOTE1;
			}
		} else {
			hquotem &= ~H_QUOTE1;
		};
		hstate=0;
		break;

	case CHR_DQUOTE:
		if(hstate!=HS_LETTER) {
			if(hstate!=HS_PREVESC) hquotem = (hquotem)? hquotem & ~H_QUOTE2: H_QUOTE2;
		} else {
			hquotem &= ~H_QUOTE2;
		};
		hstate=0;
		break;
	case '#': {
		if(hstate!=HS_LINESTART) break;
		if(!hquotem) hquotem=H_QUOTE6;
		hstate=0;
		break;
	};
	case ';': {
		if(hstate==HS_LINESTART) hquotem=H_QUOTE5;
		hstate=0;
		break;
	};
	case CHR_BSLASH:
		hstate=(hstate==HS_PREVESC)?0:HS_PREVESC;
		break;
	case '%': 
	case '!':
		if(hstate==HS_LINESTART) hquotem=H_QUOTE4;
		hstate=0;
		break;
	case CHR_CR:
	case CHR_LINE:
		if(hquotem!=H_QUOTE2) hquotem = 0;
		hstate=HS_LINESTART;
		break;
	case ' ':
	case '\t':
		if(hstate!=HS_LINESTART) hstate=0;
		break;		
	default: { 
		if((c>='A' && c<='Z') || (c>='a' && c<='z') || c>128) hstate |= HS_LETTER;
		else hstate=0;
	};
  };
}

void highlight_sln(int c)
{
  if(highlight_note(c)) return;
  switch(c) {
	case CHR_RESET:
		hstate=0;
		slang=LANG_SCRIPT;
		break;
	/* double quotes */
	case CHR_SQUOTE: 
		if(hstate!=HS_LETTER) {
			if(hstate!=HS_PREVESC) {
				hquotem = (hquotem)? hquotem & ~H_QUOTE1: H_QUOTE1;
			}
		} else {
			hquotem &= ~H_QUOTE1;
		};
		hstate=0;
		break;

	case CHR_DQUOTE:
		if(hstate!=HS_LETTER) {
			if(hstate!=HS_PREVESC) hquotem = (hquotem)? hquotem & ~H_QUOTE2: H_QUOTE2;
		} else {
			hquotem &= ~H_QUOTE2;
		};
		hstate=0;
		break;
	case '#': {
//		if(hstate==HS_LINESTART) 
		if(!hquotem) hquotem=H_QUOTE6;
		hstate=0;
		break;
	};
	case ';': {
		if(hstate==HS_LINESTART) hquotem=H_QUOTE5;
		hstate=0;
		break;
	};
#if	0
	case CHR_BSLASH:
		hstate=(hstate==HS_PREVESC)?0:HS_PREVESC;
		break;
#endif
	case '%': 
	case '!':
		if(hstate==HS_LINESTART) hquotem=H_QUOTE4;
		hstate=0;
		break;
	case CHR_CR:
	case CHR_LINE:
		if(hquotem!=H_QUOTE2) hquotem = 0;
		hstate=HS_LINESTART;
		break;
	case ' ':
	case '\t':
		if(hstate!=HS_LINESTART) hstate=0;
		break;		
	default: { 
		if((c>='A' && c<='Z') || (c>='a' && c<='z') || c>128) hstate |= HS_LETTER;
		else hstate=0;
	};
  };
}

void highlight_tags(int c)
{
  if(highlight_note(c)) return;
  switch(c) {
	case CHR_RESET:
		hstate=0;
		slang=LANG_SCRIPT;
		break;
	/* double quotes */
	case CHR_SQUOTE: 
		if(hstate!=HS_LETTER) {
			if(hstate!=HS_PREVESC) {
				hquotem = (hquotem)? hquotem & ~H_QUOTE1: H_QUOTE1;
			}
		} else {
			hquotem &= ~H_QUOTE1;
		};
		hstate=0;
		break;

	case CHR_DQUOTE:
		if(hstate!=HS_LETTER) {
			if(hstate!=HS_PREVESC) hquotem = (hquotem)? hquotem & ~H_QUOTE2: H_QUOTE2;
		} else {
			hquotem &= ~H_QUOTE2;
		};
		hstate=0;
		break;
	case '#': {
//		if(hstate==HS_LINESTART) 
		if(!hquotem) hquotem=H_QUOTE6;
		hstate=0;
		break;
	};
	case ';': {
		if(hstate==HS_LINESTART) hquotem=H_QUOTE5;
		hstate=0;
		break;
	};
	case CHR_BSLASH:
		hstate=(hstate==HS_PREVESC)?0:HS_PREVESC;
		break;
	case '%': 
	case '!':
		if(hstate==HS_LINESTART) hquotem=H_QUOTE4;
		hstate=0;
		break;
	case CHR_CR:
	case CHR_LINE:
		if(hquotem!=H_QUOTE2) hquotem = 0;
		hstate=HS_LINESTART;
		break;
	case ' ':
	case '\t':
		if(hstate!=HS_LINESTART) hstate=0;
		break;		
	default: { 
		if((c>='A' && c<='Z') || (c>='a' && c<='z') || c>128) hstate |= HS_LETTER;
		else hstate=0;
	};
  };
}

void highlight_cobol(int c)
{
  if(highlight_note(c)) return;
  line_col++;
  switch(c) {
	case CHR_RESET:
		hstate=0;
		slang=LANG_SCRIPT;
		break;
	/* double quotes */
	case CHR_SQUOTE: 
		if(hstate!=HS_LETTER) {
			if(hstate!=HS_PREVESC) {
				hquotem = (hquotem)? hquotem & ~H_QUOTE1: H_QUOTE1;
			}
		} else {
			hquotem &= ~H_QUOTE1;
		};
		hstate=0;
		break;

	case CHR_DQUOTE:
		if(hstate!=HS_LETTER) {
			if(hstate!=HS_PREVESC) hquotem = (hquotem)? hquotem & ~H_QUOTE2: H_QUOTE2;
		} else {
			hquotem &= ~H_QUOTE2;
		};
		hstate=0;
		break;
	case '@': {
//		if(hstate==HS_LINESTART) 
		if(!hquotem) hquotem=H_QUOTE6;
		hstate=0;
		break;
	};
	case '*':
		if(line_col==7) hquotem=H_QUOTE5;
		break;
	case '.':
		hstate=HS_PREVSLASH;
		break;
		
	case CHR_CR:
	case CHR_LINE:
		hquotem = 0;
		hstate=HS_LINESTART;
		line_col=0;
		break;
	case ' ':
		if(hstate==HS_PREVSLASH && hquotem==H_QUOTE6) hquotem=H_QUOTE5;
	case '\t':
		if(hstate!=HS_LINESTART) hstate=0;
		break;		
	default: { 
		if((c>='A' && c<='Z') || (c>='a' && c<='z') || c>128) hstate |= HS_LETTER;
		else hstate=0;
	};
  };
}

void highlight_sql(int c)
{
  if(highlight_note(c)) return;
  switch(c) {
	case CHR_RESET:
		hstate=0;
		slang=LANG_SCRIPT;
		break;
	/* double quotes */
	case CHR_SQUOTE: 
		if(hstate!=HS_LETTER) {
			if(hstate!=HS_PREVESC) {
				hquotem = (hquotem)? hquotem & ~H_QUOTE1: H_QUOTE1;
			}
		} else {
			hquotem &= ~H_QUOTE1;
		};
		hstate=0;
		break;

	case CHR_DQUOTE:
		if(hstate!=HS_LETTER) {
			if(hstate!=HS_PREVESC) hquotem = (hquotem)? hquotem & ~H_QUOTE2: H_QUOTE2;
		} else {
			hquotem &= ~H_QUOTE2;
		};
		hstate=0;
		break;
	case '#': {
		if(hstate==HS_LINESTART) hquotem=H_QUOTE6;
		hstate=0;
		break;
	};
	case ';': {
		if(hstate==HS_LINESTART) hquotem=H_QUOTE5;
		hstate=0;
		break;
	};
	case '-':
		if(hstate==HS_LINESTART) {
			if(hstate==HS_PREVSLASH) hquotem=H_QUOTE5;
			else hstate=HS_PREVSLASH;
		};
		break;
	case '\\':
		hstate=(hstate==HS_PREVESC)?0:HS_PREVESC;
		break;
	case '%': case '!':
		if(hstate==HS_LINESTART) hquotem=H_QUOTE4;
		hstate=0;
		break;
	case CHR_CR:
	case CHR_LINE:
		hquotem = 0;
		hstate=HS_LINESTART;
		break;
	case ' ':
	case '\t':
		if(hstate!=HS_LINESTART) hstate=0;
		break;		
	default: { 
		if((c>='A' && c<='Z') || (c>='a' && c<='z') || c>128) hstate |= HS_LETTER;
		else hstate=0;
	};
  };
}


void highlight_lua(int c)
{
  if(highlight_note(c)) return;

  if(prev_set>=0) { hquotem=prev_set;prev_set=-1;};

  switch(c) {
	case CHR_RESET:
		hstate=0;
		slang=LANG_SCRIPT;
		prev_set=-1;
		break;
	/* double quotes */
	case CHR_SQUOTE: 
		if(hstate!=HS_LETTER) {
			if(hstate!=HS_PREVESC) {
				hquotem = (hquotem)? hquotem & ~H_QUOTE1: H_QUOTE1;
			}
		} else {
			hquotem &= ~H_QUOTE1;
		};
		hstate=0;
		break;

	case CHR_DQUOTE:
		if(hstate==HS_PREVESC) break;
#if	1
		hquotem = (hquotem)? hquotem & ~H_QUOTE2: H_QUOTE2;
#else
		if(hstate!=HS_LETTER) 
		{
			hquotem = (hquotem)? hquotem & ~H_QUOTE2: H_QUOTE2;
		} else {
			hquotem &= ~H_QUOTE2;
		};
#endif
		hstate=0;
		break;

	case '-':
		if(hquotem&H_QUOTE1 || hquotem&H_QUOTE2) { hstate=0;break;};
		{
			if(hstate==HS_PREVSLASH) prev_set=H_QUOTE5;
			else if(hstate==HS_PREVAST) hquotem &= ~H_QUOTEC;
		};
		if(hquotem!=H_QUOTEC && hquotem!=H_QUOTE5)  hstate=HS_PREVSLASH;
		break;

	case '[':
		if(hquotem & H_QUOTE5) prev_set = H_QUOTEC;
		break;
	case ']':
		if(hquotem & H_QUOTEC) {prev_set=0;hquotem=0;};
		break;
	case '\\':
		hstate=(hstate==HS_PREVESC)?0:HS_PREVESC;
		break;
	case CHR_CR:
	case CHR_LINE:
		if(hquotem!=H_QUOTEC) hquotem = 0;
		hstate=HS_LINESTART;
		prev_set=-1;
		break;
	case ' ':
	case '\t':
		if(hstate!=HS_LINESTART) hstate=0;
		break;		
	default: { 
		if((c>='A' && c<='Z') || (c>='a' && c<='z') || c>128) hstate |= HS_LETTER;
		else hstate=0;
	};
  };
}

void highlight_cmd(int c)
{
  if(highlight_note(c)) return;

  if(prev_set>=0) { hquotem=prev_set;prev_set=-1;};

  if((c!='-' && c>31) && hquotem & H_LINESEP)
 	 { h_line_set=0; hquotem ^= H_LINESEP ;};

  switch(c) {
	case CHR_RESET:
		hstate=0;
		slang=LANG_SCRIPT;
		h_prev_space=1;
		prev_set=-1;
		break;
	case CHR_SQUOTE: 
		if(hstate!=HS_PREVESC){
		if(h_prev_space)
		{
			if(hquotem&H_QUOTE1) hquotem = hquotem & ~H_QUOTE1;
			else prev_set = H_QUOTE1;
			// hquotem = (hquotem)? hquotem & ~H_QUOTE1: H_QUOTE1;
		} else {
			if(hquotem & H_QUOTE1) {
				hquotem = hquotem & ~H_QUOTE1;
			}
		}
		h_prev_space=0;
		hstate=0;
		};
		break;
	case CHR_DQUOTE:
		if(hstate!=HS_LETTER) {
			if(hstate!=HS_PREVESC) hquotem = (hquotem)? hquotem & ~H_QUOTE2: H_QUOTE2;
		} else {
			hquotem &= ~H_QUOTE2;
		};
		hstate=0;
		h_prev_space=0;
		break;
	case CHR_BQUOTE: 
		if(hquotem&H_QUOTE1 || hquotem&H_QUOTE2|| hquotem&H_QUOTE6) { hstate=0;break;};
		if(hquotem==H_QUOTEC) hquotem=0;else  prev_set = H_QUOTEC;
		 // if(hquotem==H_QUOTE11) hquotem=0;else  prev_set = H_QUOTE11;
		break;
	case '-':
		if(hstate==HS_LINESTART) {
			h_line_set++;
			if(h_line_set==2) {
				hquotem=H_LINESEP;
				hstate=0;
				h_line_set=0;	
			} else {
				hquotem=H_QUOTE6;
			};
		};
		break;

	case '#': {
		if(hquotem==H_QUOTE1 || hquotem==H_QUOTE2 || hquotem==H_QUOTEC) break;
		if(hstate==HS_LINESTART || hstate==HS_PREVSPACE) {
			hquotem=H_QUOTE6;
		};
		hstate=0;
		h_prev_space=0;
		break;
	};
	case '\\':
		hstate=(hstate==HS_PREVESC)?0:HS_PREVESC;
		h_prev_space=0;
		break;
	case CHR_CR:
	case CHR_LINE:
		if(hquotem!=H_QUOTEC) hquotem = 0;
		hstate=HS_LINESTART;
		h_prev_space=1;
		h_line_set=0;

		break;
	case ' ':
	case '\t':
		h_prev_space=1;
		if(hstate!=HS_LINESTART) hstate=0;
		break;		
	default: { 
		if((c>='A' && c<='Z') || (c>='a' && c<='z') || c>128) hstate |= HS_LETTER;
		else hstate=0;
		h_prev_space=0;
	};
  };
}

void highlight_gtext(int c)
{
  if(highlight_note(c)) return;

  if(prev_set>=0) { hquotem=prev_set;prev_set=-1;};

  if((c!='-' && c>31) && hquotem & H_LINESEP)
 	 { h_line_set=0; hquotem ^= H_LINESEP ;};

  switch(c) {
	case CHR_RESET:
		hstate=0;
		slang=LANG_SCRIPT;
		h_prev_space=0;
		prev_set=-1;
		break;
	case CHR_SQUOTE: 
		if(hquotem & H_QUOTE2) break;
		if(hstate!=HS_PREVESC){
		if(h_prev_space||hstate==HS_LINESTART)
		{
			if(hquotem&H_QUOTE1) hquotem = hquotem & ~H_QUOTE1;
			else prev_set = H_QUOTE1;
			// hquotem = (hquotem)? hquotem & ~H_QUOTE1: H_QUOTE1;
		} else {
			if(hquotem & H_QUOTE1) {
				hquotem = hquotem & ~H_QUOTE1;
			}
		}
		h_prev_space=0;
		hstate=0;
		};
		break;
	case CHR_DQUOTE:
		if(hstate!=HS_LETTER) {
			if(hstate!=HS_PREVESC) hquotem = (hquotem)? hquotem & ~H_QUOTE2: H_QUOTE2;
		} else {
			hquotem &= ~H_QUOTE2;
		};
		hstate=0;
		h_prev_space=0;
		break;
	case '-':
		if(hstate==HS_LINESTART && h_prev_space==0) {
			h_line_set++;
			if(h_line_set==2) {
				hquotem=H_LINESEP;
				hstate=0;
				h_line_set=0;	
			} else {
				// hquotem=H_QUOTE6;
			};
		};
		break;

	case '#': {
		if(hquotem==H_QUOTE1 || hquotem==H_QUOTE2) break;
		if(hstate==HS_LINESTART || h_prev_space) {
			hquotem=H_QUOTE6;
			hstate=HS_TAG;
		} else {
			hstate=0;
			hstate=0;
		};
		h_prev_space=0;
		break;
	};
	case ';': {
		if(hstate==HS_LINESTART) hquotem=H_QUOTE5;
		hstate=0;
		h_prev_space=0;
		break;
	};
	case '\\':
		hstate=(hstate==HS_PREVESC)?0:HS_PREVESC;
		h_prev_space=0;
		break;
	case '%': case '!':
		if(hstate==HS_LINESTART) hquotem=H_QUOTE4;
		hstate=0;
		h_prev_space=0;
		break;
	case CHR_CR:
	case CHR_LINE:
		if(hquotem!=H_QUOTEC) hquotem = 0;
		hstate=HS_LINESTART;
		h_prev_space=0;
		h_line_set=0;
		break;
	case ' ':
	case '\t':
		if(hquotem & H_QUOTE2) break;
		h_prev_space=1;
		if(hstate==HS_TAG) hquotem=H_QUOTE6;
		hstate = HS_PREVSPACE;
		if(hquotem!=H_QUOTE6) hquotem=0;
		break;
	default: { 
		if((c>='A' && c<='Z') || (c>='a' && c<='z') || c>128) {
			hstate |= HS_LETTER;
			if(hstate & HS_TAG) hquotem=H_QUOTE4; 
		} else hstate=0;
		h_prev_space=0;
	};
  };
}


void highlight_text(int c)
{
  if(highlight_note(c)) return;
  switch(c) {
	case CHR_RESET:
		hstate=0;
		slang=1;	/* if 1 highlights numerics  */
		break;
	case '#': {	/* Only on the beginning of the line  */
		if(hstate==HS_LINESTART) hquotem=H_QUOTE6;
		hstate=0;
		break;
	};

	case ';': {
		if(hstate==HS_LINESTART) hquotem=H_QUOTE5;
		hstate=0;
		break;
	};
	case '%': case '!':
		if(hstate==HS_LINESTART) hquotem=H_QUOTE4;
		hstate=0;
		break;
	case CHR_CR:
	case CHR_LINE:
		hquotem &= ~(H_QUOTE6|H_QUOTE4|H_QUOTE5|H_QUOTE1|H_QUOTE2);
		hstate=HS_LINESTART;
		break;
	case ' ':
	case '\t':
		if(hstate!=HS_LINESTART) hstate=0;
		break;
	default: {
		if((c>='A' && c<='Z') || (c>='a' && c<='z') || c>128) hstate=HS_LETTER;
		else hstate=0;
	};
  };
}

void highlight_ecl(int c)
{
  if(highlight_note(c)) return;
  switch(c) {
	case (CHR_RESET) : // initialize
		hstate=0;
		slang=1;
		break;
	/* single quotes */
	case CHR_SQUOTE: 
		hquotem = (hquotem)? hquotem & ~H_QUOTE1: H_QUOTE1;
		hstate=0;
		break;
	/* double quotes */
	case CHR_DQUOTE:
		hquotem = (hquotem)? hquotem & ~H_QUOTE2: H_QUOTE2;
		hstate=0;
		break;
	case '#':
		if(hstate==HS_LINESTART) hquotem = (hquotem)? hquotem : H_QUOTE6;
		hstate=0;
		break;
	/* ecl comments */
	case '.':
		if(hquotem!=H_QUOTE2 && hquotem!=H_QUOTE1) {
			hstate=HS_PREVSLASH;
		};
		break;
//	case '\\':{
//		hstate=(hstate==HS_PREVESC)?0:HS_PREVESC;
//		};
//		break;
	case '\n':
	case CHR_CR:
		hquotem = 0;
		hstate=HS_LINESTART;
		break;
	case '@':
//		if(hstate==HS_LINESTART) hquotem=H_QUOTE4;
		break;
	case ' ':
	case '\t':
		if(hstate==HS_PREVSLASH) hquotem=H_QUOTE5;
		break;		
	default: { 
		hstate=0;
	};
  };
}


void init_highlight()
{
//	MESG("init_highlight:");
	hquotem=0;
	hselection=0;
	hstate=0;
	slang=0;
	if(note_type) hnote=1;
}

/* find the quote state at the start of a line from previous lines */
void fquote_state(offs till_offs, offs from_offs, WINDP *wp)
{
 offs cof;
 int c;
 // int orig_hquotem=hquotem
 // MESG("fquote_state: from %ld -> %ld",from_offs,till_offs);;
 if(!syntaxh) return;
 if(till_offs>FSize(cbfp)) {
	till_offs=FSize(cbfp);
 };
 if(from_offs<1) {
 	wp->w_fp->hl->h_function(CHR_RESET); 
	from_offs=0;
 };
 // wp->w_fp->hl->h_function(CHR_CR); 
 for(cof=from_offs;cof<till_offs;cof++) {
 	c=FCharAt(wp->w_fp,cof);
	// prev_hquotem=hquotem;
	wp->w_fp->hl->h_function(c); 
 };
 // MESG_time("!fquote: (%lld %X) -> (%lld %X)",from_offs,orig_hquotem,till_offs,hquotem);
}

void highlight_save_state(WINDP *wp,int ind,num known_offset)
{
	// if(hprev_line>=0) hquotem=hprev_line;
	// MESG("highlight_save_state:[%s] ind=%d btype=%d slang=%d hstate=%d hquotem=%X ko=%lld ho=%lld",wp->w_fp->b_fname,ind,wp->w_fp->b_type,slang,hstate,hquotem,known_offset,tp_offset(wp->tp_hline));
	// wp->w_fp->hl->h_function(CHR_FLUSH);	/* is it needed ?, now only for json  */
	wp->hs[ind].w_hquotem = hquotem;
	wp->hs[ind].w_slang = slang;
	wp->hs[ind].w_hselection = hselection;
	wp->hs[ind].w_notes = hnote;
	wp->hs[ind].known_offset = known_offset;
	wp->hs[ind].w_hstate = hstate;
	wp->hs[ind].w_in_array = in_array;
	wp->hs[ind].w_first = first;
	wp->hs[ind].w_prev_set = prev_set;
	wp->hs[ind].flag_word = flag_word;
	wp->hs[ind].single_quoted=single_quoted;
	wp->hs[ind].double_quoted = double_quoted;
	wp->hs[ind].w_bold = h_bold;
	wp->hs[ind].w_hquote_start = h_hquote_start;
	wp->hs[ind].w_line_set = h_line_set;
	wp->hs[ind].w_prev_space = h_prev_space;
	wp->hs[ind].w_prev_line = hprev_line;
	// MESG("	w_hquotem[%d]=%X",ind,wp->hs[ind].w_hquotem);
	if(ind==0) tp_copy(wp->tp_hsknown,wp->tp_hline);
}

offs highlight_restore_state(WINDP *wp,int ind)
{
	hquotem=wp->hs[ind].w_hquotem;
	slang=wp->hs[ind].w_slang;
	hselection=wp->hs[ind].w_hselection;
	hnote = wp->hs[ind].w_notes;

	hstate = wp->hs[ind].w_hstate;
	in_array = wp->hs[ind].w_in_array;
	first = wp->hs[ind].w_first;
	prev_set = wp->hs[ind].w_prev_set;
	flag_word = wp->hs[ind].flag_word;
	single_quoted = wp->hs[ind].single_quoted;
	double_quoted = wp->hs[ind].double_quoted;
	h_bold = wp->hs[ind].w_bold;
	h_hquote_start = wp->hs[ind].w_hquote_start;
	h_line_set = wp->hs[ind].w_line_set;
	h_prev_space = wp->hs[ind].w_prev_space;
	hprev_line = wp->hs[ind].w_prev_line;
	// MESG("highlight_restore_state:[%s] ind=%d b_type=%d slang=%d hstate=%d hquotem=%X ko=%lld ho=%lld",wp->w_fp->b_fname,ind,wp->w_fp->b_type,slang,hstate,hquotem,wp->hs[ind].known_offset,tp_offset(wp->tp_hline));
	return wp->hs[ind].known_offset;
}

/*
	to recognise
	.m4 	(now  as C)
	.m4f 	(now as GTEXT)
	.pm (automake)	(now as perl modules)
	.tex	(now as GTEXT)	create new!
	.dtd	(now as NONE)	create new! or xml
	.awk	(now as GTEXT)	create new!
	.profile, .bashrc -> SHELL	(now as GTEXT)
	.md	-> GTEXT ? (now GTEXT)
	calendar.* -> C (now GTEXT)
	.mk	(make) now as GTEXT
	.cmake (now as GTEXT)	create new!
	.rst (cmake help files) now as GTEXT (maybe ok!)
	.icc (color files) now as BINARY (convert)
	check perl highlight (/usr/share/console-setup/KeyboardNames.pl)
	recognize "<-- ** --> at first line as xml
	recognize "<!DOCTYPE" at first line as xml (.conf files)
	recognise "if [" at first line as shell
	Makefile.in (-> SHELL or other) now as GTEXT.
	when viewing the last file in a dir and press 'left' it goes to the start of the line instead at start of the file name
	[! at the start is .md file
	.el -> emacs lisp , now as GTEXT , not so bad !
	.schema -> evolution schema, now GTEXT, soso
	%!PS -> postscript file
	.otf  fomt file starts with 'OTTO'
	groovy (file extensions .groovy, .gradle)
	stylus
	jsx		(like c++)
	dart	(like c++)		
	vbscript, vb
	svg		(html like comments)
	c#		(.cs, .asax, .ashx)
	scala
	coffeescript
	clojure
	sass
	lua
	diff
	markdown (.md, .markdown, mdown, mkdn)	(html like comments) 
	gfm (.gfm, github markdown) 	
	hx
	haskell
	pug	

*/
