/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

	File buffer functions
*/

#include "xe.h"

extern alist *window_list;
extern SHLIGHT hts[];

extern int drv_type;

/* local variables */

char *tp_name[] = {
	"Current",
	"save_current",
	"Base",
	"Text_end",
	"hline",
	"prev_hline",
	"save_hline",
	"w_smark",
	"w_emark",
	"b_smark",
	"b_emark",
	"hsknown",
	"hmknown",
	"save_hsknown",
	"save_hmknown",
	"base_point: delete_block",
	"t: delete_block",
	"line_start",
	"line_end",
	"comment_start",
	"comment_end",
	"double dot",
	"temp current"
};

/* Local function declarations */

void  CheckPoint(FILEBUF *fp);
offs  ScanForCharForward(FILEBUF *fp,offs start,byte ch);
void  EmptyText(FILEBUF *);
int   ReplaceTextFromFile(char *name,FILEBUF *bf,int fd,offs size,offs *act_read);
int CheckMode(mode_t mode);
void textpoint_reset(TextPoint *tp);
int chardline(WINDP *wp);
// offs  Fcheck_UtfCharAt(FILEBUF *bf, offs offset, char *c);
offs  FUtfCharAt_nocheck(FILEBUF *bf, offs offset, utfchar *uc);

FILEBUF *cbfp=NULL;
int clen_error=0;
void undo_CheckSize(UNDOS *u);
void undo_change_undo(undo_Change *uc);
void undo_change_redo(undo_Change *uc);
void   ToLineEnd();
offs   FNextLine(FILEBUF *fp,offs ptr);
int BolAt(offs o);
int FBolAt(FILEBUF *fp,offs o);
int EolAt(offs o);
int Eol();
byte FCharAt(FILEBUF *fp,offs offset);
byte FCharAt_NoCheck(FILEBUF *,offs offset);
offs FUtfCharAt(FILEBUF *,offs,utfchar *);
byte   Char();
void   MoveLineCol(num l,num c);
void   HardMove(num l,num c);
void   NewLine();
int  InsertChar(char ch);
void   ExpandTab();
void    DeleteChar();
int   WriteBlock(int fd,offs from,offs size,offs *act_written);
void   PreModify(FILEBUF *);
UNDOS *undo_new();
void show_points(FILEBUF *bf,FILE *fp);

offs   LineBegin(offs ptr);
offs   FLineBegin(FILEBUF *fp,offs ptr);
offs   PrevLine(offs ptr);
offs   FPrevLine(FILEBUF *fp,offs ptr);
offs   LineEnd(offs ptr);

void FindLineCol(TextPoint *tp);

void MESG_time(const char *fmt, ...)
{
 if(!debug_flag()) return;
 va_list args;
 static char mline[512];
 if(!(strcmp(cbfp->b_fname,"[out]"))) return;
    if (fmt != NULL) {
		va_start(args,fmt);
		vsnprintf(mline,511,fmt,args);
		va_end(args);

		show_time(mline,1);
		fflush(stderr);
    }
}

void MESG_time_start(const char *fmt, ...)
{
 if(!debug_flag()) return;
 va_list args;
 static char mline[512];
 if(!(strcmp(cbfp->b_fname,"[out]"))) return;
    if (fmt != NULL) {
		va_start(args,fmt);
		vsnprintf(mline,511,fmt,args);
		va_end(args);

		show_time(mline,0);
		fflush(stderr);
    }
}

void *emalloc(long size,char *des)
{
 void *a;
 a=malloc(size);
 if(a==NULL) SYS_ERROR("emalloc: %s size=%ld",des,size);
 // MESG("emalloc: [%s] %d pos=0x%llX",des,size,(long long)a);
 return a;
}

void efree(void *p,char *des)
{
 // MESG("efree:%s [%llX]",des,(long long)p);
 if(p!=NULL) {
 	free(p);
 } else {
 	SYS_ERROR("efree: %s, %ld is NULL! ",des,(long)p);;
 }
}

void  FError(const char *s)
{
	msg_line("File: [%s] %s errno=%d",cbfp->b_fname,s,errno);
	SYS_ERROR("File: [%s] %s errno=%d",cbfp->b_fname,s,errno);
}


FILEBUF *current_filebuf()
{
	return cbfp;
}

void set_current_filebuf(FILEBUF *fp)
{
	cbfp = fp;
}

// on cbfp
bool test_b_flag(int flag)
{
	return (cbfp->b_flag & flag) ? true:false;
}

void set_b_flag(int flag)
{
	cbfp->b_flag = flag;
}
/* set modify flag to buffer */
void set_modified(FILEBUF *bp)
{
	if((bp->b_state & FS_CHG)) return;
	bp->b_state |= FS_CHG;
	update_file_status(bp);
}

void inodeinfo_set(FILEBUF *bf,struct stat *st)
{
 bf->FileTime=st->st_mtime;
 bf->FileMode=st->st_mode;
/* 
 other infos to consider are st_size,st_dev,st_ino
 */
}

void set_end_line(FILEBUF *fp,num line)
{
	if(fp->line_from > line) {
		fp->line_to = fp->line_from;
		fp->line_from = line;
	} else {
		fp->line_to = line;
	}
}

TextPoint *textpoint_init(int tp_type,int window_id,FILEBUF *fp)
{
 static int next_num=1;
 TextPoint *tp=(TextPoint *)malloc(sizeof(TextPoint));
 if(fp==NULL) SYS_ERROR("textpoint_init: null filebuf for name=%s!!!",tp_name[tp_type]);
 tp->offset=0;
 tp->line=0;
 tp->col=0;
 tp->flags=0;
 tp->num=next_num++;
 tp->t_next=NULL;
 tp->fp=fp;
 tp->tp_type=tp_type;
 tp->window=window_id;
 return tp;
}

#if	0
void test_time1()
{
 int i,loops = 1000000;
 msg_line("Time test started ..");
 show_time("test_time1:",0);

 for(i=0;i<loops;i++) {
  fquote_state(FSize(cbfp),0,cwp);
 }

 show_time("test_time1: end",1);
 msg_line("Time test ended !");
}
#endif

extern char *file_ext[];
extern int h_type[];

/* create message string */
#define SMESG(format,...) { \
    stat=snprintf(s,width, format, ##__VA_ARGS__);\
	if(stat>=width)	s[stat]=0; /* this is needed because snprintf does not guarantee a 0 at the end of the string */ \
	sm[i++]=strdup(s);\
	sm[i]=NULL;\
	if(drv_type>0) MESG("show_info: %s",s);\
}

extern int hmcurrent;
extern int hmstart;
extern int hmnum;
extern BMARK hmark[];

int show_info(num n)
{
 char s[2048];
 char s1[256];
 FILEBUF *bp=cbfp;
 int i=0;
 int width=80;
 char **sm = malloc(128*sizeof(char *));
 int b_typ = bp->b_type % NOTE_TYPE;
 int stat;
	SMESG("%s",date_string(1));
	SMESG("--- Application info ----------------------------------------");
	SMESG("%s version %s",PROGNAME,VERSION);
#ifdef	GVERS
	SMESG("git:%s",GVERS);
#endif
	SMESG("Current directory: %s",getcwd(s1,256));
	SMESG("Start dir: %s",get_start_dir());
	SMESG("Default codepage: [%s], local [%s]",codepage_str[default_lang],codepage_str[default_local_codepage]);
	SMESG("--- File buffer information ---------------------------------");
	SMESG("Buffer name     : [%s] index=%d",bp->b_fname,bp->b_index);
	if(strlen(bp->b_dname)>0) {
		SMESG("Buffer directory: [%s]",bp->b_dname);
	};
	if(bp->connect_buffer) SMESG(" connected to %s",bp->connect_buffer->b_fname);
	if(debug_flag()) {
		SMESG("Flags: b_mode=0x%X b_flag=0x%X view_mode=0x%X b_type=0x%X is_wrap=%d",cbfp->b_mode,cbfp->b_flag,cbfp->view_mode,cbfp->b_type,is_wrap_text(cbfp));
	};
	if(cbfp->b_mode) {
		strcpy(s,"Buffer mode: ");	
		if(cbfp->b_mode & EMCRYPT) strlcat(s,"Encrypted ",width);
		if(cbfp->b_mode & EMUNIX)  strlcat(s,"unix ",width);
		if(cbfp->b_mode & EMMAC)   strlcat(s,"mac ",width);
		if(cbfp->b_mode & EMDOS)   strlcat(s,"dos ",width);
		sm[i++]=strdup(s);sm[i]=0;
	};
	strcpy(s,"Buffer flags: ");
	if(cbfp->b_state & FS_ACTIVE) 	strlcat(s,"Active,",width);
	if(cbfp->b_state & FS_CHG) 		strlcat(s,"Changed,",width);
	if(cbfp->b_state & FS_VIEW) 	strlcat(s,"Read only,",width);
	if(cbfp->b_flag & FSINVS) 		strlcat(s,"Internal ",width);
	if(cbfp->b_flag & FSDIRED) 		strlcat(s,"Dir list,",width);
	if(cbfp->b_flag & FSMMAP) 		strlcat(s,"Memory mapped,",width);
	if(cbfp->b_flag & FSFILTER) 	strlcat(s,"Filtered",width);
#if	TNOTES
	if(cbfp->b_flag & FSNOTES) 		strlcat(s,"Notes Tag column,",width);
	if(cbfp->b_flag & FSNOTESN) 	strlcat(s,"Notes Note column,",width);
#endif
	if(cbfp->b_flag & FSNLIST) 		strlcat(s,"Note list",width);
// 	if(cbfp->b_flag & FSNCALIST) 	strlcat(s,"Calendar list view",width);
	sm[i++]=strdup(s);sm[i]=0;
	if(debug_flag()) {
		if(cbfp->slow_display) { SMESG("buffer slow!");} else { SMESG("buffer fast display");};
	};
	if(debug_flag()) {
		SMESG(" ptr1=%lld ptr2=%lld size=%lld file size=%lld,gap=%lld",bp->ptr1,bp->ptr2,bp->BufferSize,FSize(bp),bp->GapSize);
	};
	if(cbfp->view_mode !=0) {
		strcpy(s,"View mode: ");	
		if(cbfp->view_mode & VMHEX) strlcat(s,"HEX ",width);
		if(cbfp->view_mode & VMINP) strlcat(s,",Input ",width);
		strlcat(s,". show: ",width);
		if(cbfp->view_mode & VMLINES) strlcat(s,"lines ",width);
		else if(cbfp->view_mode & VMOFFSET) strlcat(s,"offset ",width);
		else if(cbfp->view_mode & VMINFO) 	strlcat(s,"info",width);
		else if(cbfp->view_mode & VMWRAP) 	strlcat(s,"wrap",width);
		else strlcat(s,"no info",width);
		sm[i++]=strdup(s);sm[i]=0;
	};
	if(cbfp->b_flag >= FSNOTES) {
		if(cbfp->b_flag & FSNLIST) {
			SMESG("top line=%d current line=%d",cwp->top_note_line,cwp->current_note_line);
			if(cbfp->b_flag & FSDIRED) {
				SMESG("Number of files in dir: %u",bp->dir_list_str->size);
				stat = dir_getfile(s1,0);
				SMESG("--- Current file info (%s) ---",s1);
				stat = dir_getfile(s1,3);
				SMESG("%s",s1);
				stat = dir_getfile(s1,4);
				SMESG("permissions: %s",s1);
			};
		}
#if	TNOTES
		else {
			SMESG("Tags:  top %3d,current %3d,total %4d",cwp->top_tag_line,cwp->current_tag_line,bp->b_tag_list->size);
			SMESG("Notes: top %3d,current %3d,total %4d",cwp->top_note_line,cwp->current_note_line,bp->dir_list_str->size);
		}
#endif
	} else {
		if(debug_flag()) {
			SMESG("Position info: line=%lld col=%lld offset=%lld char=[0x%lX]",getcline()+1,GetCol()+1,Offset(),utf_value());
			SMESG(" row %d , %d, %lld",chardline(cwp)+1,getwline(),tp_line(cwp->tp_current)-tp_line(cwp->tp_hline));
		} else {
			SMESG("Position info: line=%lld col=%lld offset=%lld char=[0x%lX] row %d",getcline()+1,GetCol()+1,Offset(),utf_value(),getwline());
		};


		SMESG("File type: [%s] (%d) highlight type [%s] syntaxh=%d slang=%d",hts[b_typ].description,b_typ,bp->hl->description,syntaxh,cwp->hs[0].w_slang);
		if(bp->b_type & NOTE_TYPE) SMESG("Note type");
		// SMESG("Highlight type: [%s] syntaxh=%d slang=%d",bp->hl->description,syntaxh,cwp->hs[0].w_slang);
		if(debug_flag()) {
//			SMESG("  b_type=%d",b_type);
			SMESG("  hknown[%d] h=%X n=%d slang=%d first=%d o=%lld",0,cwp->hs[0].w_hquotem,cwp->hs[0].w_notes,cwp->hs[0].w_slang,cwp->hs[0].w_first,cwp->hs[0].known_offset);
			SMESG("  hknown[%d] h=%X n=%d slang=%d first=%d o=%lld",1,cwp->hs[1].w_hquotem,cwp->hs[0].w_notes,cwp->hs[1].w_slang,cwp->hs[1].w_first,cwp->hs[1].known_offset);
			SMESG("  hknown[%d] h=%X n=%d slang=%d in_array=%d o=%lld",0,cwp->hs[0].w_hquotem,cwp->hs[0].w_notes,cwp->hs[0].w_slang,cwp->hs[0].w_in_array,cwp->hs[0].known_offset);
			SMESG("  hknown[%d] h=%X n=%d slang=%d in_array=%d o=%lld",1,cwp->hs[1].w_hquotem,cwp->hs[0].w_notes,cwp->hs[1].w_slang,cwp->hs[1].w_in_array,cwp->hs[1].known_offset);
		};
		if(debug_flag()) {
			SMESG("Buffer size : %lld readed=%lld lines %lld",bp->BufferSize-bp->GapSize,bp->bytes_read,bp->lines);
			SMESG("End of line: size=%d",bp->EolSize);
		} else {
			SMESG("Buffer size:%lld, lines:%lld, Eol size:%d",bp->BufferSize-bp->GapSize,bp->lines,bp->EolSize);
		};
	};
	SMESG("Document codepage: %s (%d)",codepage_str[bp->b_lang],bp->b_lang);
	SMESG("Current window: %2d H=%d W=%d lcol=%lld",cwp->id,cwp->w_ntrows,cwp->w_ntcols,cwp->w_lcol);
	if(debug_flag()) {
	SMESG("Gap size: %lld",bp->GapSize);
	SMESG("prt1: %lld, old: %lld",bp->ptr1,bp->oldptr1);
	SMESG("prt2: %lld, old: %lld",bp->ptr2,bp->oldptr2);
	SMESG("Number of windows: %d",bp->b_nwnd);
	SMESG("Editor buffer mode: %08o",bp->b_mode);
	SMESG("[0] quotem=%X selection=%X slang=%X ",cwp->hs[0].w_hquotem,cwp->hs[0].w_hselection,cwp->hs[0].w_slang);
	SMESG("[1] quotem=%X selection=%X slang=%X ",cwp->hs[1].w_hquotem,cwp->hs[1].w_hselection,cwp->hs[1].w_slang);
	};
	if(debug_flag()) {
		TextPoint   *scan;
		// show points of current bufferf
		SMESG("--- points of current buffer ------------------------------");
		for(scan=bp->tp_last ; scan; scan=scan->t_next) {
			SMESG("- point %02d [%s] o=%lld line=%lld col=%lld f=%02X win=%d",scan->num,tp_name[scan->tp_type],scan->offset,scan->line,scan->col,scan->flags,scan->window);
		};
		{	/* show history marks  */
			int i0,i1;
			i1=hmstart;
			SMESG("history marks: start=%d current=%d num=%d",hmstart,hmcurrent,hmnum);
			for(i1=0;i1<=hmnum;i1++){
				i0=(i1+hmstart)%BKMAX;
				SMESG("hmark[%2d] [%2d] %s %ld",i0,i1,hmark[i0].fname,hmark[i0].line);
			};
		}
		WINDP *wp;
		lbegin(window_list);
		SMESG("--- Window list ---------------------------------------------");
		while((wp=(WINDP *)lget(window_list))!=NULL) {
			SMESG("window id %d",wp->id);
			if(wp==cwp) { SMESG(" %d is current!",wp->id);};
		};
	};
	SMESG("--- Driver information --------------------------------------");
	SMESG("%s",drv_info());
	selectl("File info",sm,i,20,10,3,width,-1);
#if	0
	test_time1();
#endif
	clear_snames(sm,i);
	set_update(cwp,UPD_MOVE);
	return true;
}

/* return the byte size of accents at offset */
int is_utf_accent(FILEBUF *fp, offs o)
{
 int ch,ch1;
 ch=FCharAt(fp,o);
 ch1=FCharAt(fp,o+1);
 if(((ch==0xCC || ch==0xCD) && (ch1<0xB0 && ch1>0x7F))
// 	|| (ch==0xCD && (ch1<0xB0))
 ){	// check for double accent
	if(FEofAt(fp,o+3))	return 0;
	ch=FCharAt(fp,o+2);
	ch1=FCharAt(fp,o+3);
	if(((ch==0xCC || ch==0xCD) && (ch1<0xB0 && ch1>0x7F))
//		|| (ch==0xCD && (ch1<0xB0))
	){
//		MESG("double accent at %ld",o);
		return 4;
	} else {
//		MESG("accent at %ld",o);
		return 2;
	};
 } else return 0;
}


int utf8charlen_nocheck(int ch)
{
 if(ch<129) return 1;
 if(ch<0xE0) return 2;
 if(ch<0xF0) return 3;
 return 4;
}

/* 
 return the number of bytes of a (utf) character with accents
 sets clen_error. 
 but gives more info about errors.
*/
int FUtfCharLen(FILEBUF *fp,offs o)
{
 int ch;
 int ch1;
 int clen=1;
	clen_error=0;
	fp->utf_accent=0;
	
	if(FEolAt(fp,o)) return fp->EolSize;

	if(fp->view_mode & VMHEX) return 1;
	if(fp->b_lang == 0) {
		ch=FCharAt(fp,o);
		if(ch<0xC0) {
			if(ch>128) {
				clen_error=1;	/* this is not a valid start for utf  */
				return 1;
			};
//			MESG("0: ch=%X %d",ch,ch);
			if(ch<32) return clen;
		} else if(ch<0xE0) {
//			MESG("1: ch=%X %d",ch,ch);
			if(o+1<FSize(fp)){
				ch1=FCharAt(fp,o+1);
				if(ch1<128 || ch1>0xBF) { clen_error=2;return 1;};	/* not a middle utf char  */
				clen=2;
//#if	DARWIN || PCURSES
				if((ch==0xCC||ch==0xCD) && !utf8_error() /* && drv_type<3 */ ) 
				{
					fp->utf_accent=1;
//					MESG("allone accent! clen=%d");
					return clen;	/* return without checking for next accent  */
				};
//#endif
			} else {
				clen_error=3;	/* incomplete, eof  */
				return clen;
			};
		}
		else if(ch<0xF0) {
			ch1=FCharAt(fp,o+1);
			if(ch1<128 || ch1>0xBF) { clen_error=4;return 1;};	/* not a middle utf char  */
			ch1=FCharAt(fp,o+2);
			if(ch1<128 || ch1>0xBF) { clen_error=5;return 1;};	/* not a middle utf char  */
			clen=3;
		} else if(ch<0xF8) {
			char ch2,ch3;
			ch1=FCharAt(fp,o+1);
			if(ch1<128 || ch1>0xBF) { clen_error=6;return 1;};	/* not a middle utf char  */
			ch2=FCharAt(fp,o+2);
			if(ch2<128 || ch2>0xBF) { clen_error=7;return 1;};	/* not a middle utf char  */
			ch3=FCharAt(fp,o+3);
			if(ch3<128 || ch3>0xBF) { clen_error=8;return 1;};	/* not a middle utf char  */
			clen=4;
//			MESG("- 4:o=%ld %2X %2X %2X %2X",o,ch,ch1,ch2,ch3);
		} else {
			clen_error=9;
			return 1;
		};
//#if	DARWIN || PCURSES
		if(clen<3 && !fp->utf_accent /* && drv_type<2 */) {	/* check next char for accent!  */
		if(!FEofAt(fp,o+clen+1)){
			clen += is_utf_accent(fp,o+clen);
			// if(clen>2) MESG("total size=%d at %ld",clen,o);
		}};
//#endif
	};
	return clen;
}

// check next char, update offset,physical column!
offs check_next_char(FILEBUF *fp,offs o,int *col) 
{
	int c;
	utfchar uc;
	o = FUtfCharAt(fp,o,&uc);
 	c=uc.uval[0];
	if (c == CHR_TAB) {
		*col=next_tab(*col);
	} else {
		*col += get_utf_length(&uc);
	};
	return o;
}

int DiffColumns(FILEBUF *fp, offs start_col,offs col_offs,char *from)
{
 int col = 0;
 offs o=start_col;
 if(col_offs>FSize(fp)) col_offs=FSize(fp);
 // if((start_col % 4)!=0) MESG("DiffColumns[%s] %ld %d ERROR!!!!",from,start_col,start_col%4);
 if(fp->b_lang==0 && !utf8_error()){
//  MESG("diffcol: from %ld to %ld",o,col_offs);
  while (o < col_offs ) {
	o = check_next_char(fp,o,&col);
	if(clen_error) {
		set_utf8_error(1);
		int c=DiffColumns(fp,start_col,col_offs,"DiffColumns");
		set_utf8_error(0);
		return c;
	};
  };
 } else {
  while (o < col_offs) {
	int c;
	c=FCharAt(fp,o);
	if (c == CHR_TAB) {
		col=next_tab(col);
	} else {
		col ++;
	};
	o++;
  }
 };
 return(col);
}

// DiffColumn depends on the display driver, 
// each character can have different display lentgh
int DiffColumn(FILEBUF *fp, offs *dbo,offs col_offs,char *from)
{
 int col = 0;
 offs o=*dbo;
 offs file_size=FSize(fp);
 if(col_offs>file_size) col_offs=file_size;
 if(fp->b_lang==0 && !utf8_error()){
//  MESG("diffcol: from %ld to %ld",o,col_offs);
  while (o < col_offs) {
	int c;
	utfchar uc;
	o = FUtfCharAt_nocheck(fp,o,&uc);
 	c=uc.uval[0];
	if (c == CHR_TAB) {
		col=next_tab(col);
	} else {
		col += get_utf_length(&uc);
		if(clen_error) {
			set_utf8_error(1);
			c=DiffColumn(fp,dbo,col_offs,"inside");
			set_utf8_error(0);
			return c;
		};
	};
  };
 } else {
  while (o < col_offs) {
	int c;
	c=FCharAt(fp,o);
	if (c == CHR_TAB) {
		col=next_tab(col);
	} else {
		col ++;
	};
	o++;
  }
 };
 *dbo=o;
 return(col);
}


#if	0
#include "findlinecol0.c"
#else
#include "findlinecol1.c"
#endif

void textpoint_move(TextPoint *tp,offs shift)
{
 offs newoffs=tp->offset+shift;
   if(newoffs<0)  newoffs=0;
   if(newoffs>FSize(tp->fp))  newoffs=FSize(tp->fp);
   tp->offset=newoffs;
   tp->flags |= LINECOLUNDEFINED;
   FindLineCol(tp);
}

offs tp_offset(TextPoint *tp)
{
	return tp->offset;
}

num tp_line(TextPoint *tp)
{
	return(tp->line);
}

num	tp_col(TextPoint *tp)
{
	return(tp->col);
}

void  ResetTextPoints(FILEBUF *bp,int);
void  OrFlags(int mask);
void  FindOffset(TextPoint *tp);
int   GetBlock(FILEBUF *bp,char *copy,offs start,offs size);


/* textpoint.cc */
// link textpoint with a buffer
void  textpoint_link(TextPoint *tp,FILEBUF *fp)
{
   if(tp==NULL) return;
   if(fp==NULL) { ERROR("textpoint_link: %s fp is null",tp_name[tp->tp_type]);};
   tp->fp=fp;
   tp->t_next= tp->fp->tp_last;
   tp->fp->tp_last=tp;
   tp->fp->npoints++;
}

// remove textpoint from buffer point list
void  textpoint_unlink(TextPoint *tp)
{
 TextPoint *scan,*p;
 FILEBUF *fp=tp->fp;
 if(tp==NULL) return;
 fp->npoints--;
 if(tp==fp->tp_last) {
 	fp->tp_last=tp->t_next;
	return;
 } else {
 	p=fp->tp_last;
	for(scan=fp->tp_last->t_next; scan; scan=scan->t_next) {
      if(scan==tp)
      {
         p->t_next=scan->t_next;
		 return;
      } else p=p->t_next;
	}
 };
 ERROR("textpoint to unlink not found! %d",tp->num);
}

/* remove textpoint from list and free */
void  textpoint_delete(TextPoint *tp)
{
 FILEBUF *fp=tp->fp;
 if(tp==fp->tp_last) {
 	fp->tp_last=tp->t_next;
	textpoint_free(tp);
	return;
 } else {
	TextPoint *scan,*p;
 	p=fp->tp_last;
	for(scan=fp->tp_last->t_next; scan; scan=scan->t_next) {
      if(scan==tp)
      {
         p->t_next=scan->t_next;
         textpoint_free(tp);
		 return;
      } else p=p->t_next;
	}
 };
}

void textpoint_free(TextPoint *tp)
{
 if(tp!=NULL) {
	free(tp); 
 };
}

TextPoint *textpoint_o(FILEBUF *bf,offs o,int tp_type,int window)
{
 TextPoint *tp;
 
	tp=textpoint_init(tp_type,window,bf);
	textpoint_link(tp,bf);
	tp->offset=o;
	tp->flags =LINECOLUNDEFINED;
	FindLineCol(tp);
	return tp;
}

TextPoint *textpoint_new(FILEBUF *fp,int tp_type,int window)
{
 TextPoint *tp=NULL;
 tp=textpoint_init(tp_type,window,fp);
 textpoint_link(tp,fp);
 return tp;
}

TextPoint *new_textpoint_at(FILEBUF *bf,int tp_type,offs o)
{
 TextPoint *tp;
	tp=textpoint_new(bf,tp_type,0);
	textpoint_set(tp,o);
 	return(tp);
}

void textpoint_set_lc(TextPoint *tp,num l, num c)
{
 tp->line=l;
 tp->col=c;
 tp->flags=OFFSUNDEFINED;
 FindOffset(tp);
}

void tp_copy(TextPoint *tp1,TextPoint *tp2)
{
 if(tp1==tp2) return;
 if(tp1->fp!=tp2->fp) {
	ERROR("wrong buffer! at tp_copy tp1->fp=[%s] != tp2->fp=[%s]",tp1->fp->b_fname,tp2->fp->b_fname);
	exit(0);
 };

	tp1->offset=tp1->line=tp1->col=tp1->flags=0;
	if(tp2->flags & (LINECOLUNDEFINED)) {
//		ERROR("tp_copy: without decided info! old=[%s] new=[%s]",tp_name[tp2->tp_type],tp_name[tp1->tp_type]);
		FindLineCol(tp2);
	};

	if(tp2->flags!=0) ERROR("tp_copy: error off %s in w_id=%d, f=%d",tp_name[tp2->tp_type],tp2->window,tp2->flags);
   tp1->offset=tp2->offset;
   tp1->line=tp2->line;
   tp1->col=tp2->col;
   tp1->flags=tp2->flags;
}

TextPoint *tp_dup(TextPoint *tp,int window_id)
{
 TextPoint *t;
 if(tp->flags) {
	if(tp->flags==OFFSUNDEFINED) {
		ERROR("tp_dup: findoffset");
		FindOffset(tp);
	} else {
		ERROR("tp_dup: without decided info! [%s] window %d flags=%d",tp_name[tp->tp_type],window_id,tp->flags);
		FindLineCol(tp);
	}
 };

 t=textpoint_init(tp->tp_type,window_id,tp->fp);
 textpoint_link(t,tp->fp);

 tp_copy(t,tp);
 return t;
}

void  FindOffset(TextPoint *tp)
{
    TextPoint   *found=NULL;
	offs dist=INT_MAX;
	offs new_dist=INT_MAX;
	FILEBUF *fp=tp->fp;
#if	WRAPD
	num o0,l0,c0,t0,o1,l1,c1,o2;
	MESG("FindOffset: l=%ld c=%ld t=%d",tp->line,tp->col,tp->flags);
#endif
	if(fp->view_mode & VMHEX) {
		tp->offset = tp->line * HEX_LINE_LEN + tp->col;
		tp->flags=0;
		if(tp->offset > FSize(tp->fp)) {
			tp->offset=FSize(tp->fp);
			tp->flags = LINECOLUNDEFINED;
			FindLineCol(tp);
		};
		return;
	};
	TextPoint *scan;
	for(scan=fp->tp_last ; scan; scan=scan->t_next) {
		if(scan==tp) continue;
		if(!(scan->flags))
		{
			new_dist=llabs(tp->line - scan->line);
			if(  new_dist < dist )
			{
				found=scan;
				dist=new_dist;
				if(dist<2) break;
			}
		};
	};

   offs  o;
   num   l,c;

   if(found)  {
#if	1
     o=found->offset;
     c=found->col;
#else
      if(found->flags&COLUNDEFINED) {
         o=FLineBegin(fp,found->offset);
         c=0;
      } else  {
         o=found->offset;
         c=found->col;
      };
#endif
      l=found->line;
#if	WRAPD
	  o0=o;l0=l;c0=c;t0=found->tp_type;
#endif
   } else {
	  o=c=l=0;
#if	WRAPD
	  t0=0;
#endif
   };

   if(l>tp->line) {
      while(l>tp->line) {
         o=FPrevLine(fp,o);
         l--;
      };
      c=0;
   }
   else if(l<tp->line) {
      if(FEofAt(fp,o)) {
         tp->offset=o;
         tp->line=l;
         tp->col=c;
         tp->flags = FULLDEFINED;
         return;
      };
      while(l<tp->line) {
         o=FLineEnd(fp,o);
         if(FEofAt(fp,o)) {
            tp->offset=o;
            tp->line=l;
            tp->col=0;
            tp->flags = FULLDEFINED;
            return;
         }
         o=FNextLine(fp,o);
         l++;
         if(FEofAt(fp,o)) {
            tp->offset=o;
            tp->line=l;
            tp->col=0;
            tp->flags = FULLDEFINED;
            return;
         }
      }
      c=0;
   }
	/* we have found the correct line  */
#if	WRAPD
   o1=o;l1=l;c1=c;
#endif
#if	0
	o=FLineBegin(fp,o);c=0;
#else
   if(c>tp->col) { // it is difficult to correctly backspace with tabs,multiple width utf chars, go to the start of the line!!
		o=FLineBegin(fp,o);c=0;
   };
#endif
#if	WRAPD
	o2=o;
#endif
   while(c<tp->col) {
	utfchar uc;
	o=FUtfCharAt(fp,o,&uc);
	if(uc.uval[0]=='\n'||uc.uval[0]=='\r') {
		o--;
		MESG("FindOffset: EOL, column bigger than expected! %d %d",c,tp->col);
		break;
	};
	if(uc.uval[0]=='\t') c=next_tab(c);
	else c+=get_utf_length(&uc);
   };   

   tp->col=c;
   tp->offset=o;
   tp->flags = FULLDEFINED;
#if	WRAPD
   sprintf(mesgs,"(o=%lld l=%lld c=%lld t [%s]) (o=%lld l=%lld c=%lld) beg=%lld final=%lld",
   	o0,l0,c0,tp_name[t0],o1,l1,c1,o2,o);
#endif
}

/* set tp to offset , independant */
void textpoint_set(TextPoint *tp,offs o)
{
   if(o<0) o=0;
   if(o>FSize(tp->fp)) o=FSize(tp->fp);

   tp->offset=o;
   tp->flags = LINECOLUNDEFINED;
   FindLineCol(tp);
}

// Reset all buffer textpoints, if flag==0 do not reset size of the file
void  ResetTextPoints(FILEBUF *bp,int flag)
{
 num size=0;
 TextPoint *scan;
 if(!flag) { size = tp_offset(bp->tp_text_end); };
 // MESG("ResetTextPoints: size=%ld lines=%ld",size,tp_line(bp->tp_text_end),tp_line(bp->tp_last));
 for(scan=bp->tp_last; scan; scan=scan->t_next) {
  	scan->line=scan->col=scan->offset=0;
 	scan->flags = FULLDEFINED;
 }; 
 if(!flag) {
	textpoint_set(bp->tp_text_end,size);
	// MESG("	new o=%ld lines=%ld new lines=%ld b_mode=%X",tp_offset(bp->tp_text_end),tp_line(bp->tp_text_end),bp->lines,bp->b_mode);
	textpoint_set(bp->tp_text_o,size);
	if( bp->b_mode > VMINP) 
	bp->lines=tp_line(bp->tp_text_end);
 };
}

void  textpoint_OrFlags(FILEBUF *fp,int mask)
{
	TextPoint *scan;
   mask&=COLUNDEFINED|LINEUNDEFINED;
   for(scan=fp->tp_last; scan; scan=scan->t_next) {
      if(scan->offset>0) scan->flags|=mask;
   };
}


offs  ScanForCharForward(FILEBUF *fp,offs start,byte ch)
{
   char *pos;

   if(start<fp->ptr1) {
      pos=(char*)memchr(fp->buffer+start,ch,fp->ptr1-start);
      if(pos) return pos-fp->buffer;
      start=fp->ptr1;
   }
   if(start<FSize(fp)) {
      pos=(char*)memchr(fp->buffer+fp->GapSize+start,ch,FSize(fp)-start);
      if(pos) return pos-fp->buffer-fp->GapSize;
   }
   return -1;
}

// operates on cbfp,cwp
int clipboard_copy(ClipBoard *clip)
{
  static int full_path=0;
  FILEBUF *fp = cwp->w_fp;
  utfchar uc;
  
	if (clip->text!=NULL) { efree(clip->text,"clipboard_copy");clip->text=NULL;};
#if	1
	if(cbfp->b_flag & FSNLIST && cbfp->b_flag & FSDIRED)
	{
		char fname[MAXFLEN];
		dir_getfile(fname,1);
		clip->rect=0;
		clip->height=1;
		clip->width=strlen(fname);
		if(full_path) clip->width+=strlen(cbfp->b_dname)+1;
		clip->text=(char*)emalloc(clip->width,"dir name");
		if(full_path) {
			strcpy(clip->text,cbfp->b_dname);
			clip->text[strlen(clip->text)]='/';
			strcpy(clip->text+strlen(clip->text),fname);
			full_path=0;
		} else {
			full_path=1;
			strcpy(clip->text,fname);
		};
		return(TRUE);
	} else
#endif
	if(cwp->selection == REGION_COLM)
   {
      clip->rect=TRUE;

      num   i,j;
      num   line1=tp_line(cwp->w_smark);
      num   line2=tp_line(cwp->w_emark);
      num   col1=FColumn(cbfp,tp_offset(cwp->w_smark));
      num   col2=FColumn(cbfp,tp_offset(cwp->w_emark));
	  offs	size;

      clip->width=llabs(col2-col1)+1;
      clip->height=llabs(line2-line1)+1;
	  size=((clip->width+1)*clip->height+1)*4;	/* each utf can have 4 bytes  */
      clip->text=(char*)emalloc(size,"clipboard_copy");

      if(!clip->text) {
      	ERROR("clipboard_copy:rect  Not enough memory");
	 	return(FALSE);
      };

      num c=col1;
      offs o=tp_offset(cwp->w_smark);
	  offs o1=LineBegin(o);

      char *text_ptr=clip->text;

	  	for(j=0; j< clip->height; j++,o1=FNextLine(cbfp,o1)) {
	 		i=0;
			o=o1;
			c=0;
			// go to the start of rectangle column

	    	while(c<col1) {
				if(EolAt(o)) break;
				o = FUtfCharAt(cbfp,o,&uc);
				if(uc.uval[0]=='\t') {
					c=next_tab(c);
	      		} else {
		  			c += get_utf_length(&uc);
		  		};
	    	};
			// copy the line
	    while(i< clip->width) {
	       if(EolAt(o)) break;
		   o = FUtfCharAt(cbfp,o,&uc);
	       if(uc.uval[0]=='\t') {
		  	num c1=next_tab(c);
		  	int k=c1-c;
		  	if(c<col1) k-=(col1-c);
		  	if(k>(clip->width-i)) k=(clip->width-i);
		  	i+=k;
		  	while(k-- >0)  *text_ptr++=' ';
		  	c=c1;
		  	continue;
	       } else {;
		    int len=get_utf_length(&uc);
			int i1;
		       c+=len;
		       i+=len;
		       for(i1=0;i1<4 && uc.uval[i1]!=0;i1++) { *text_ptr++ = uc.uval[i1];};
		   }
	    };
		// add space at the end of the line till fills rectangle width
	 	while(i<clip->width) {
	    	i++;
	    	*text_ptr++=' ';
	 	};
		// add a new line at end of each line for X11 exporting reasons!
		*text_ptr++='\n';o++;
      }
   }
   else /* !rblock */
   {
      clip->rect=0;

      offs start=tp_offset(cwp->w_smark);
	  offs end=tp_offset(cwp->w_emark);
	  offs tmpval=0;

	  if(end<start) {
		tmpval=start;start=end;end=tmpval;	  	
	  };
	  if(cwp->selection==REGION_LINE) clip->width=end-start+fp->EolSize;
	  else      clip->width=end-start;
      clip->height=1;
      clip->text=(char*)emalloc(clip->width*clip->height,"rectangle clipboard");
      if(!clip->text) {
		ERROR("clipboard_copy: Not enough memory");
		return(false);
      };
	  if(tmpval)
      	GetBlock(cwp->w_fp,clip->text,tp_offset(cwp->w_emark),clip->width);
	  else
      	GetBlock(cwp->w_fp,clip->text,tp_offset(cwp->w_smark),clip->width);
   };
   full_path=0;
   return(true);
}

// update current point to end of insert
// operates on cbfp
int clipboard_paste(ClipBoard *clip)
{
   int res=true;
   FILEBUF *fp=cbfp;
   if(!clip->text)  return true;

   if(clip->rect) {
      num l=tp_line(fp->tp_current);
      num c=GetCol();
      int i;
	  char *blanks;
	  char *tmp_str=emalloc(clip->width*4+1,"clip_tmprect");
	  char *s;
	  blanks=(char *)emalloc(clip->width,"clipboard_paste");
	  memset((void *)blanks,' ',clip->width);

      char *text_ptr=clip->text;

      for(i=0; i<clip->height; i++) {
	 	HardMove(l+i,c);
	 	num ll=clip->width;
	    if(Eol()) {
	       while(ll>0 && clip->text[i*(clip->width+1)+ll-1]==' ') {
			ll--;
			};
	    };
		for(s=tmp_str;*text_ptr != 0;text_ptr++){
			if(*text_ptr=='\n') {break;};
			*s++ = *text_ptr;
		};*s=0;
		text_ptr++;
		if(gmode_over) res=del_char(strlen(tmp_str));
	    res=InsertBlock(fp,tmp_str,strlen(tmp_str),NULL,0);
	    if(res!=true)  return false;
      };

	  efree(tmp_str,"clip tmp str");
	  efree(blanks,"clipboard_paste, blanks");blanks=NULL;
   } else {
#if	0
	  return ext_system_paste();
#else
	  if(gmode_over) res=del_char(clip->width*clip->height);
      res=InsertBlock(fp,clip->text,clip->width*clip->height,NULL,0);
      if(res!=true) return false;
	  textpoint_move(fp->tp_current,(offs)clip->width*clip->height);
#endif
   }
	set_modified(fp);
   return true;
}

void reset_region_textpoints()
{
	textpoint_set(cwp->w_smark,tp_offset(cwp->tp_current));
	textpoint_set(cwp->w_emark,tp_offset(cwp->tp_current));
	cwp->selection=0;
}


void check_line_mode(FILEBUF *bf)
{
 int i;
 num DosLastLine=0;
 num UnixLastLine=0;
 num MacLastLine=0;

      for(i=0; ; i++) {
	 	i=ScanForCharForward(bf,i,'\n');
	 	if(i<0)  break;
	 	UnixLastLine++;
	 	if(i>0 && FCharAt_NoCheck(bf,i-1)=='\r')  DosLastLine++;
      };
//	MESG("check_line_mode d=%ld u=%ld",DosLastLine,UnixLastLine);

	if(UnixLastLine==0) {
		for(i=0;;i++) {
		 	i=ScanForCharForward(bf,i,'\r');
		 	if(i<0)  break;
	 		MacLastLine++;
		};
	};

//	MESG("	d=%ld u=%ld",DosLastLine,UnixLastLine);
	  // if we permit dos file usage
      if(UnixLastLine/2<DosLastLine) /* check if the file has unix or dos format */
      {
		bf->EolSize=2;
		strlcpy(bf->EolStr,"\r\n",3);
		bf->b_mode |= EMDOS;
		// MESG("	dos mode");
		textpoint_OrFlags(bf,COLUNDEFINED|LINEUNDEFINED);
      } else  
	  if(MacLastLine>0){
		bf->EolSize=1;
		strlcpy(bf->EolStr,"\r",3);
		bf->b_mode |= EMMAC;
		// MESG("	mac mode");
		textpoint_OrFlags(bf,COLUNDEFINED|LINEUNDEFINED);
	  } else
	  {
		// MESG("	unix mode");
		bf->b_mode |= EMUNIX;
      };
}


offs MoveToColumn(int go_col)
{
 byte c;
 int    col;
 offs	dbo=0;
 offs	db0=0;
 utfchar uc;
 col = 0;
 dbo=db0=LineBegin(FOffset(cbfp));
 
 while (!FEofAt(cbfp,dbo)) {
 	db0=dbo;
 	if(FEolAt(cbfp,dbo)) {
		break;
	}
	dbo = FUtfCharAt(cbfp,dbo,&uc);
 	c=uc.uval[0];

	if (c == CHR_TAB) {
		col=next_tab(col);
	} else {
		col += get_utf_length(&uc);
	};
	if(col > go_col) break;
 };
 set_Offset(db0);
 return (db0);
}


/* Moves exactly to specified line/column, edits the text as needed */
// operates on cbfp
void   HardMove(num l,num c)
{
   MoveLineCol(l,0);
   while(GetCol()<c) {
   		if(Eol()) break;
		MoveRightChar(cbfp);
	}
   if(FEof(cbfp)) {	// insert as many lines as needed to reach specified line
      while(GetLine()<l){
	  	NewLine();
	  	textpoint_move(cbfp->tp_current,cbfp->EolSize);
	  };
	};
   if(Eol()) {	// insert as many spaces as needed to go to specified column
      while(GetCol()<c) { 
	  	InsertChar(' ');
	  	MoveRightChar(cbfp);
	  };
	};
   if(GetCol()>c) {
		MoveLeftChar(cbfp);
      ExpandTab();
      while(GetCol()<c) MoveRightChar(cbfp);
   }
}

// operates on cbfp
void   NewLine()
{
	InsertBlock(cbfp,cbfp->EolStr,cbfp->EolSize,NULL,0);
	set_modified(cbfp);
}

// operates on cbfp, calculate goal from col
void   MoveLineCol(num line,num col)
{
 offs o;
 TextPoint *tp=cbfp->tp_current;

 textpoint_set_lc(tp,line,0);
#if	WRAPD
 o = tp_offset(tp);
 MESG("MoveLineCol: line=%ld o=%ld col=%ld valid=%ld",line,tp_offset(tp),col,valid_offset(o,col));
#endif
 if(col==0) return;
 o = tp_offset(tp);
 textpoint_set(tp,o+valid_offset(o,col));
}

// operates on cbfp
void set_Char(offs offset,byte b)
{
	// without undo option!
	if(offset>=cbfp->ptr1) offset+=cbfp->GapSize;
	cbfp->buffer[offset]=b;
}

byte  FCharAt(FILEBUF *bf, offs offset)
{
	if(offset >= bf->ptr1) {
		offset+=bf->GapSize;
		if(offset < bf->BufferSize) return(bf->buffer[offset]);
	} else {
		if(offset>=0) return(bf->buffer[offset]);
	};

   return(0);
}

// specific on current buffer */
byte  CharAt(offs offset)
{
	if(offset >= cbfp->ptr1) {
		offset+=cbfp->GapSize;
		if(offset < cbfp->BufferSize) return(cbfp->buffer[offset]);
	} else {
		if(offset>=0) return(cbfp->buffer[offset]);
	};
   return(0);
}


/* put to uc the utf value, advance by the length, return new offset */
offs  FUtfCharAt(FILEBUF *bf, offs offset, utfchar *uc)
{
 int i,ulen;
 offs o=offset;
	ulen=FUtfCharLen(bf,o);
	memset(uc->uval,0,8);
	for(i=0;i<ulen;i++){
			uc->uval[i]=FCharAt(bf,o+i);
	};
	return o+ulen;
}

offs  FUtfCharAt_nocheck(FILEBUF *bf, offs offset, utfchar *uc)
{
 int i,ulen;
 offs o=offset;
	// ulen=FUtfCharLen(bf,o);
	char ch=FCharAt_NoCheck(bf,o);
	ulen = utf8charlen_nocheck(ch);
	memset(uc->uval,0,8);
	for(i=0;i<ulen;i++){
			uc->uval[i]=FCharAt_NoCheck(bf,o+i);
	};
	return o+ulen;
}

byte  FCharAt_NoCheck(FILEBUF *fp,offs offset)
{
   if(offset>=fp->ptr1) offset+=fp->GapSize;
   return(fp->buffer[offset]);
}

/* character at textpoint */
byte tp_char(TextPoint *tp)
{
 FILEBUF *fp=tp->fp;
 return FCharAt(fp,tp->offset); 
}

// file size, independent
offs FSize(FILEBUF *fp)
{
   return(fp->BufferSize - fp->GapSize);
}

// Size of current buffer (cbfp)
offs CSize()
{
	return(cbfp->BufferSize - cbfp->GapSize);
}

// operates on fp, takes info only localy
// must not be used from window operations
// // operates on cbfp
 offs    Offset()
{
   return(cbfp->tp_current->offset);
}

 offs    FOffset(FILEBUF *fp)
{
   return(fp->tp_current->offset);
}

// operates on cbfp
void   ExpandTab()
{
 int i,size;
	if(Char()!='\t') return;
	size=tabsize-GetCol()%tabsize;
	for(i=size; i>0; i--) InsertChar(' ');
	DeleteChar();
	textpoint_move(cbfp->tp_current,-size);
}

int FEof(FILEBUF *fp)
{
   return(FOffset(fp)>=FSize(fp));
}

int FEofAt(FILEBUF *fp,offs o)
{
   return(o >= fp->BufferSize-fp->GapSize);
}


int FBof(FILEBUF *fp)
{
   return(FOffset(fp)<=0);
}


// operates on cbfp
byte    Char()
{
   return(CharAt(Offset()));
}

int EofAt(offs o)
{
	return(o >= cbfp->BufferSize-cbfp->GapSize);
}

/* return utf character value with accent, to display hex value */
long int utf_value()
{
 if(utf8_error()) { 
	return Char();
 };
 if(cbfp->b_lang!=0) return Char();
 else {
  offs o=Offset();
  int ulen,i;
  long int uchar=0;
  ulen=FUtfCharLen(cbfp,o);

  for(i=0;i<ulen;i++){
  	uchar += CharAt(o+i);
	if(i<ulen-1) uchar <<=8;
  };
  return(uchar);
 };
}

long int utf_value_len(int *len)
{
 if(utf8_error()) { 
	return Char();
 };
 if(cbfp->b_lang!=0) {
	*len=1;
 	return Char();
 } else {
  offs o=Offset();
  offs o0=o;
  utfchar uc;
  int ulen,i;
  long int uchar=0;
  o = FUtfCharAt(cbfp,o,&uc);
  ulen=o-o0;

  for(i=0;i<ulen;i++){
  	uchar += uc.uval[i];
	if(i<ulen-1) uchar <<=8;
  };
  *len = get_utf_length(&uc);
  return(uchar);
 };
}

num physical_column(num vcol)
{
 num physical_column;
	if(cwp->w_fp->view_mode & VMHEX) {
		if(cwp->w_fp->view_mode & VMINP) {
			physical_column = 10+vcol*3- cwp->w_lcol+get_hex_byte();
		} else {
			physical_column = vcol - cwp->w_lcol+HSTART+1;
		};
	}
	else {
#if	1
		physical_column = vcol - cwp->w_lcol + cwp->w_infocol;
#else
		if(cwp->w_infocol>2) {
			physical_column = vcol - cwp->w_lcol+VMCOLS;
		} else {
			if(cwp->w_infocol>0) physical_column = vcol - cwp->w_lcol + cwp->w_infocol;		
			else physical_column = vcol - cwp->w_lcol;
		}
#endif
	}
 return physical_column; 
}

num WGetCol()
{
// num col1;
 num col2;
//	col1 = tp_col(cwp->tp_current) - cwp->w_lcol;
#if	0
 col2=tp_col(cwp->tp_current);
#else
 if(cwp->w_fp->b_flag & FSNOTESN) col2=NOTES_COLUMN+2;
	else col2 = physical_column(FColumn(cbfp,cwp->tp_current->offset));
 // MESG("wg: (%ld - %d+%d) col2=%ld",tp_col(cwp->tp_current),cwp->w_lcol,cwp->w_infocol,col2);
#endif
 return col2;
}

// returns the current column in current file buffer
num GetCol()
{
// TEST !!!!
#if	1
 // num now_col=FColumn(cbfp,FOffset(cbfp));
 // if(now_col!=tp_col(cbfp->tp_current)) ERROR("wrong getcol!");
 return tp_col(cbfp->tp_current);
#else
	return(FColumn(cbfp,FOffset(cbfp)));
#endif
}

// returns the current line in current file buffer
num GetLine()
{
	return(tp_line(cbfp->tp_current));
}

// returns the current column in current file buffer
num GetColumn()
{
	return(tp_col(cbfp->tp_current));
}

num LastLine()
{
	num last_line;
	
	last_line=tp_line(cbfp->tp_text_end)+1;
	if(last_line != cbfp->lines) {
		cbfp->lines=last_line;
	};
	// MESG("Last_line: %ld",last_line);
	return(last_line);
}

num FColumn(FILEBUF *fp,offs o)
{
 int    col;
 offs	dbo;
 // MESG("FColumn: o=%ld",o);
 dbo=FLineBegin(fp,o);
 if(fp->view_mode & VMHEX) col = (o-dbo)%0x10;
 else 
#if	0
 	col=DiffColumns(fp,dbo,o,"FColumn:OK:BEG!");
#else
	col=DiffColumn(fp,&dbo,o,"FColumn");
#endif
// MESG("FColumn:%d %ld",col,tp_col(cwp->tp_current));
 return (col);
}

num Column(offs o)
{
 int    col;
 offs	dbo;
 dbo=LineBegin(o);
 if(cbfp->view_mode & VMHEX) col = (o-dbo)%0x10;
 else col=DiffColumns(cbfp,dbo,o,"Column:OK:BEG");
// MESG("FColumn:%d %ld",col,tp_col(cwp->tp_current));
 return (col);
}

num  FGetLine(FILEBUF *fp)
{
   return(tp_line(fp->tp_current));
}

void refresh_textpoint(TextPoint *scan)
{
	switch(scan->flags){
		case 0:
			return;
		case COLUNDEFINED:
		case LINEUNDEFINED:
		case LINECOLUNDEFINED:
			FindLineCol(scan);
			return;
		case OFFSUNDEFINED:
			FindOffset(scan);
	};
}

void refresh_textpoints(FILEBUF *fp)
{
 TextPoint *scan;
	for(scan=fp->tp_last; scan; scan=scan->t_next) {
		refresh_textpoint(scan);
	}
}

// delete current character
void    DeleteChar()
{
 utfchar uc;
 int len;
	FUtfCharAt(cbfp,FOffset(cbfp),&uc);
	len=get_utf_length(&uc);
	DeleteBlock(0,len);
	set_modified(cbfp);
}

int EolAt(offs o)
{
 int stat=0;
 if(cbfp->view_mode & VMHEX) {
 	if( (o % HEX_LINE_LEN == HEX_LINE_LEN-1) || FEofAt(cbfp,o)) stat=1;
 } else {
	if(FEofAt(cbfp,o)) return 1;
	stat=(CharAt(o)==cbfp->EolStr[0] && (cbfp->EolSize<2 
	|| CharAt(o+1)==cbfp->EolStr[1]));
 };
 return stat;
}

int FEolAt(FILEBUF *bf,offs o)
{
 int stat=0;
 if(bf->view_mode & VMHEX) {
 	if((o % HEX_LINE_LEN == HEX_LINE_LEN-1) || o>=FSize(bf)) stat=1;
 } else {
   if(o>=FSize(bf)) return(1);
   stat=(FCharAt(bf,o)==bf->EolStr[0] && (bf->EolSize<2 || FCharAt(bf,o+1)==bf->EolStr[1]));
 };
 return stat;
}

int BolAt(offs o)
{
 int stat=0;
 if(cbfp->view_mode & VMHEX) {
	if( o % HEX_LINE_LEN == 0 ) stat=1;
 } else {
	if(o<1) stat=1;
	else
 	stat=( CharAt(o-cbfp->EolSize)==cbfp->EolStr[0] 
   	&& (cbfp->EolSize<2 || CharAt(o-1)==cbfp->EolStr[1]));
 };
 return stat;
}

int FBolAt(FILEBUF *bf,offs o)
{
 int stat=0;
 if(bf->view_mode & VMHEX) {
	if( (o) % HEX_LINE_LEN == 0 ) stat=1;
 } else {
 	if(o==0) return 1;
 	stat= (FCharAt(bf,o-bf->EolSize)==bf->EolStr[0] 
   	&& (bf->EolSize<2 || FCharAt(bf,o-1)==bf->EolStr[1]));
 };

 return stat;
}

int Eol()
{
   return(EolAt(Offset()));
}

int Bol()
{
   return(BolAt(Offset()));
}

int  InsertChar(char ch)
{
   return(InsertBlock(cbfp,&ch,1,NULL,0));
   set_modified(cbfp);
}

// read write lock
int    LockFile(int fd,bool drop)
{
   struct  flock   Lock;
   struct flock   Lock1;
   Lock.l_start=1;
   Lock.l_len=0;
   Lock.l_type=F_WRLCK;
   Lock.l_whence=SEEK_SET;

	if(drop) Lock.l_type=F_UNLCK;
	
   if(fcntl(fd,F_SETLK,&Lock)==-1)
   {
		if(errno==EACCES || errno==EAGAIN) {
         char   msg[100];

         struct  stat   st;

		if(errno==EAGAIN) msg_line("error:  file: already locked!");
         Lock1=Lock;
         fcntl(fd,F_GETLK,&Lock1);
         if(Lock1.l_type==F_UNLCK)
         {
            return(-2);
         } else {
		 
		 };
         fstat(fd,&st);

         snprintf(msg,100,"by prosess %ld",(long)Lock1.l_pid);

		if(confirm("File locked",msg,1)) return(0);
		else return -2;
      }
      else
      {
         return(-2);
      }
   }
   return(0);
}

/* get device size on unixes */
off_t  GetDevSize(int fd)
{
#ifdef BLKGETSIZE
   unsigned long sect=0;
   if(ioctl(fd,BLKGETSIZE,&sect)==0)
      return ((off_t)sect)<<9;
#endif

   off_t lower=0;
   off_t upper=0x10000;
   char buf[1024];
   off_t pos,mid;
   int res;	
   
   
   for(;;)
   {
      pos=lseek(fd,upper,SEEK_SET);
	  if(pos<0 || errno) break;
      if(pos!=upper) break;
      res=read(fd,buf,sizeof(buf));
      if(res!=sizeof(buf))	 break;
      lower=upper;
      upper*=2;
   }

   for(;;)
   {
      if(upper<=lower) break;
      mid=(upper+lower)/2;
      pos=lseek(fd,mid,SEEK_SET);
      if(pos!=mid)
      {
	 	upper=mid;
	 	continue;
      }
      int res=read(fd,buf,sizeof(buf));
      if(res>0)	 lower=mid+res;
      else	 upper=mid;
   }
   return upper;
}

void update_lines(FILEBUF *bp)
{
//  num old_lines=bp->lines;
	textpoint_set(bp->tp_text_o,FSize(bp));
	if(debug_flag()) MESG_time("update_lines:1");
	textpoint_set(bp->tp_text_end,FSize(bp));
	bp->lines=tp_line(bp->tp_text_end)+1;
	// MESG("update_lines: old=%ld new=%ld s=%ld",old_lines,bp->lines,FSize(bp));
}

num get_lines(FILEBUF *bp)
{
	if(bp->b_flag & FSNLIST) return bp->dir_list_str->size;
#if	TNOTES
	else if(bp->b_flag & FSNOTESN) return bp->dir_list_str->size;
	else if(bp->b_flag & FSNOTES) return bp->b_tag_list->size;
#endif
	else return bp->lines;
}

num get_utf2to16_size(char *file_name)
{
 FILE *fi=fopen(file_name,"r");
 size_t res;
 unsigned short in_i;
 num size=0;
 fseek(fi,2,0);
	while((res=fread((void *)&in_i,2,1,fi))==1) {
		unsigned int c,d;
		// char wchar[5];
		// printf("%4X\n",in_i);
		// in_bytes+=2;
		// in_chars++;
	    c= in_i;
		
        if ((c & 0xFC00) == 0xD800) {    /* surrogates */
			res=fread((void *)&in_i,2,1,fi);
			// in_bytes+=2;
	    	if (res!=1) {  break; };         /* shouldn't happens */
			d = in_i; // second part of surrogate
	    
            if ((d & 0xFC00) == 0xDC00) {
                c &= 0x03FF;
                c <<= 10;
                c |= d & 0x03FF;
                c += 0x10000;
            } else {
				c = '?';
	    	}
        }

		/* assertion: c is a single UTF-4 value */
		int i=0;
        if      (c <    0x80) {  i=1 ;}
		else if (c <   0x800) {  i=2 ;}
        else if (c < 0x10000) {  i=3 ;}
		else                  {  i=4 ;}

		size += i;
		// out_chars ++;
	};

	// =======================================================================
	fclose(fi);// start after bom
	return size;
}

/* insert file in current buffer 
   if ir_flag is true to insert
*/
int ifile0(FILEBUF *bf,char *name,int ir_flag)
{
   struct stat    st;
   num	act_read=0;
   num	to_read;
   int status=0;
   int file;
   int temp_used=0;

	bf->line_from=0;
	if( bf->b_flag & FSDIRED ) {
		int i;
		i=insert_dir(bf,0);
		bf->line_to = tp_line(bf->tp_text_end);
		return(i);
	};
   status=stat(name,&st);
   // MESG("ifile0: [%s] dir=[%s] name=[%s] status=%d ir_flag=%d",bf->b_fname,bf->b_dname,name,status,ir_flag);
	MESG_time_start("ifile0: %s",bf->b_fname);
   if(status==-1 && errno==ENOENT)
   {
	if(name[0]!=CHR_LBRA)	msg_line("New file \"%s\"",name);
	return(false);
   } ;
   if(!CheckMode(st.st_mode)) {
		SYS_ERROR("[%s] not a regular file!");
		msg_line("[%s] not a regular file!");
	   	return(false);
   };

  if(!ir_flag) {
  	status=init_ftype(bf,name,&temp_used,0); // if not insert then get file type
	if(status!=TRUE) return FALSE;
  };
   errno=0;
	// MESG("ifile0: 1 name=%s",name);
   /* clear the 'temporarily read-only' bit */
   bf->b_state &= ~FS_VIEW;
   if(!name[0])
   {
		bf->b_flag &= ~FSMMAP;
		msg_line("No name for memory mapped file!");
      	return(true);
   }
	// MESG("ifile0: 2 name=%s errno=%d",name,errno);

   if(errno==0) {
		bf->FileMode=st.st_mode;
		bf->FileTime=st.st_mtime; // moddify time
      	if((!(bf->b_flag & FSMMAP) 
			&& (
				S_ISBLK(bf->FileMode)
				|| S_ISCHR(bf->FileMode)))
			 	|| S_ISFIFO(bf->FileMode)) 
		{
	 		msg_line("%s is a special file or a pipe\nthat I cannot edit.",name);
	 		EmptyText(bf);
	 		return(false);
      	};
      	if(S_ISDIR(bf->FileMode))	{
			 bf->b_state |= FS_VIEW;
		};
   };
	// if(bf->b_flag & FSMMAP) MESG("ifile0: memory mapped file %s!",bf->b_fname);
#if	0
	if(bf->b_mode & EMCRYPT) {	/* no mmap fro encrypted files!  */
		bf->b_flag &= ~FSMMAP;
	};
#else
	if(bf->bom_type != 0) {	/* no mmap */
		bf->b_flag &= ~FSMMAP;
	};
#endif
	// MESG("ifile0: 4 name=%s",name);
	file=open(name,O_RDONLY);

	if(file<3) {
	 	FError(name);
	 	EmptyText(bf);
		SYS_ERROR("ifile0: cannot open file %s errno=%d",name,errno);
	 	return(false);
	}

	// re-stat the file in case it was created
	status=fstat(file,&st);
	if(status==0) {
		bf->FileMode=st.st_mode;
	} else {
	 	FError(name);
	 	EmptyText(bf);
		SYS_ERROR("ifile0: cannot restat the file %s errno=%d status=%d",name,errno,status);
	 	return(false);
	};

	/* this is proc file or zero lenght, do not use mmap */
	if(st.st_blocks==0 ) bf->b_flag &= ~FSMMAP; 

	if(!(bf->b_flag & FSMMAP))  // not memory mapped
	{
		to_read=st.st_size;
		// MESG("no memmap!");
		if(bf->bom_type==FTYPE_UTF8BOM) to_read-=3;
		if(bf->bom_type==FTYPE_UTF16BOM) {
			to_read=get_utf2to16_size(name);
			// MESG("file %ld -> %ld",st.st_size,to_read);
		};
		if(to_read==0 && bf->bytes_read>1) { // a bloc/proc file ??
			to_read=GetDevSize(file);
			lseek(file,0L,0);
		};

		if(bf->b_mode & EMCRYPT) {
			to_read -=1;	// already readed file crypt header
			lseek(file,1,0L);
		};
		if(ir_flag) {
			if(ReadBlock(name,file,to_read,&act_read)!=true) {
		 		if(errno)   FError(name);
		 		EmptyText(bf);
		 		return(false);
			}
		} else {
			// if(!execmd) msg_line("insert text from file %lld",to_read);
			// MESG("replace text!");
	    	if(ReplaceTextFromFile(name,bf,file,to_read,&act_read)!=true) {
		 		if(errno)   FError(name);
	 			EmptyText(bf);
	 			return(false);
			}
		};

	    CheckPoint(bf);

		if(bf->b_mode & EMCRYPT){ // decrypt the rest of the file
			crypt_string(bf->buffer,act_read);
		};
	}
	else /* buffer_mmapped */
	{
		// MESG("memmap!");
		to_read = st.st_size;
//		MESG("	buffer mmapped! size=%ld",to_read);
		if((S_ISBLK(bf->FileMode) || S_ISCHR(bf->FileMode))
	      && st.st_size<=0)
		{	// try to get device size
		 	to_read = st.st_size = GetDevSize(file);
			lseek(file,0L,0);
		};

	    if(to_read>0)
		{
		 	bf->buffer=(char*)mmap(0,to_read,
					PROT_READ
	//				|(PROT_WRITE)
				    ,MAP_SHARED
	#if	LINUX
					|MAP_NONBLOCK
	#endif
					|MAP_NORESERVE,file,0);
		 if(bf->buffer==(char*)MAP_FAILED) {
		    FError(name);
		    EmptyText(bf);
			close(file);
		    return false;
		 };
		 bf->BufferSize=st.st_size;
		 bf->ptr1=bf->ptr2=bf->BufferSize;
		 bf->GapSize=0;
		}
   };
	check_line_mode(bf);

	update_lines(bf);
//	MESG("	lines updated!");
	if(!(bf->b_flag & FSMMAP)) 
		set_modified(bf);

//	MESG("	check fstat again!");
	status=fstat(file,&st);
	if(status==0) {
	   	bf->FileMode=st.st_mode;
		inodeinfo_set(bf,&st);
	} else {
		bf->FileMode=0;
	 	FError(name);
//	 	EmptyText(bf);
		SYS_ERROR("ifile0: cannot stat the file after reading! %s errno=%d status=%d",name,errno,status);
//	 	return(false);
	};

	textpoint_set(bf->tp_current,0);	// goto the beginning
	MESG_time("ifile0: end");
	close(file);
	if(!execmd) msg_line("%s: chars=%lld,lines=%lld type %s",bf->b_fname,FSize(bf),bf->lines,bf->hl->description);
	if(temp_used) {
		// MESG("remove temporary %s",name);
		unlink(name);
		bf->b_state |= FS_VIEW;
	};
	bf->line_to = tp_line(bf->tp_text_end);

//	MESG("ifile0:end b_type=%d",bf->b_type);
   	return(true);
}

	/* set ir_flag to insert file at current buffer position   */
int ifile(FILEBUF *bf,char *name,int ir_flag)
{
 int stat;
 // MESG("ifile: [%s] ir_flag=%d view_mode=%d",name,ir_flag,bf->view_mode);
 stat=ifile0(bf,name,ir_flag);


 return(stat);
}


int CreateBak(FILEBUF *bp)
{
   char	 *command;
   int	 res=true;
   int nsize = 	strlen(bp->b_dname)*2+strlen(bp->b_fname)*2+10;
	command = malloc(nsize);
	snprintf(command,nsize,"mv %s/%s %s/%s~",bp->b_dname,bp->b_fname,bp->b_dname,bp->b_fname);
	res=system(command);
	free(command);
	if(res==-1) return FALSE;
	else return TRUE;
}

int CheckMode(mode_t mode)
{
   if(S_ISBLK(mode)) return true;	// this is a block mode device!
   if((mode&S_IFMT)!=S_IFREG)
   {
     return(FALSE);
   }
   return(TRUE);
}

/* write the current file to name */
int	writeout(char *name, FILEBUF *bf)
{
	struct stat st;
	int   nfile;
	offs   act_written;
	char cr1[2];
	bool bak_created=0;
	// MESG("writeout:");
   if(bf->b_flag & FSMMAP)
   {
      if(!strcmp(name,bf->b_fname)) return true; // same file already saved!
	  msg_line("cannot save a mmap file with different name!");
	  return FALSE;
   }

	if(bf->b_mode & EMCRYPT) {
		resetkey(bf);
		cr1[0]=26;
		cr1[1]=0;
	};
   if(!execmd) msg_line("Saving the file \"%.60s\"...",name);
   
   if(stat(name,&st)!=-1)
   {
     if(!CheckMode(st.st_mode)) return(false);

	 if(bf->FileTime != st.st_mtime)
	 {
	 	if(discmd)
		if(!confirm("File changed by other","Replace? ",1)) return(false);
     }
     inodeinfo_set(bf,&st);
     if((int)bt_dval("make_backup")==1) 
     {
	 	if(CreateBak(bf)!=true)	 {
			if(!confirm("Cannot create backup file", "continue ?",0)) return(false);
      	} else bak_created=TRUE;
	 };
   }   else   {

     if(errno!=ENOENT) {
       FError(name);
       return(false);
	};
    st.st_mode=bf->FileMode|0644;
  };

   errno=0;
   nfile=open(name,O_CREAT|O_RDWR|O_TRUNC,st.st_mode);

   if(nfile==-1)
   {
     FError(name);
	 return error_line("cannot open file %s to write",name);
   }

   int lock_res=LockFile(nfile,false);
   if(lock_res==-1)   {
     close(nfile);
	 return error_line("cannot lock file %s for write!",name); 
   };

   if(lock_res==-2) msg_line("Warning: file %s locking failed",name);

   /* write the buffer contents */
   errno=0;
	if(bf->b_mode & EMCRYPT) {
		if(write(nfile,cr1,1)!=1) { SYS_ERROR("cannot write");close(nfile);return(false);};
	};	
   if(WriteBlock(nfile,0,FSize(bf),&act_written)!=true)
   {
     if(errno) FError(name);
     close(nfile);
     return(false);
   };


   if(bf->bom_type!=FTYPE_UTF16BOM && act_written!=FSize(bf))
   {
	 FError("Cannot write the file up to end, disk is full?");
	 LockFile(nfile,true);
     close(nfile);
     return(false);
   }

   CheckPoint(bf);

   stat(name,&st);
   inodeinfo_set(bf,&st);
   LockFile(nfile,true);
   close(nfile);

    if(bak_created) msg_line("file \"%.60s\" saved with backup",name);
	else msg_line("file \"%.60s\" saved",name);

   return(true);
}

/* undo function ! */

bool undo_change_join(undo_Change *c1, undo_Change *c);

size_t undo_GetSize(undo_Change *uc)
{
	return (uc->left?uc->left_size:0)
		  +(uc->right?uc->right_size:0)
		  +sizeof(undo_Change);
}

void undo_init(UNDOS *u)
{
   u->chain_head=0;
   u->chain_ptr=0;
   u->chain_tail=0;
   u->group_open=0;
   u->current_group=0;
   u->head_position=0;
   u->enabled=true;
   u->locked=false;
   u->last_change_time=0;

   u->glue_changes=true;
   u->glue_max_time=5;
   u->min_groups=4;
   u->max_size=0x10000000;
}

UNDOS *undo_new()
{
 UNDOS *u=(UNDOS *)emalloc(sizeof(UNDOS),"undo_new");
	undo_init(u);
//	MESG("undo_new:%llX",(long long)u);
	return u;
}

bool undo_enabled(FILEBUF *fp)
{
 UNDOS *u;
 u=fp->main_undo;
 if(fp->b_flag & FSDIRED) return false;
 return u->enabled && !u->locked;
}

bool undo_locked(FILEBUF *fp)
{
	return fp->main_undo->locked;
}

void undo_clear(UNDOS *u)
{
	if(u==NULL) return;
   while(u->chain_head!=NULL)
   {
		struct undo_Change *to_delete = u->chain_head;

		u->chain_head=u->chain_head->next;
		efree(to_delete,"undo_clear: u->chain_head");
   };
   u->chain_tail=u->chain_ptr=0;
   u->chain_head=NULL;
   u->current_group=0;
   u->group_open=0;
	u->head_position=-1;
}

void beginundogroup(UNDOS *u)
{
   if(!u->group_open)
   {
      u->current_group++;
	  u->head_position=0;
   }
   u->group_open++;
}

void undo_set_glue()
{
	cbfp->main_undo->glue_changes=true;
}

void undo_set_noglue()
{
	cbfp->main_undo->glue_changes=false;
}

void undo_AddChange(FILEBUF *fp,undo_Change *c)
{
 UNDOS *u;
 u=fp->main_undo;

   if(!undo_enabled(fp))
   {
      return;
   };
   while(u->chain_ptr)
   {
      if(u->chain_ptr==u->chain_tail)  u->chain_tail=u->chain_ptr->prev;
      if(u->chain_ptr==u->chain_head) u->chain_head=u->chain_ptr->next;
      if(u->chain_ptr->next) u->chain_ptr->next->prev=u->chain_ptr->prev;
      if(u->chain_ptr->prev) u->chain_ptr->prev->next=u->chain_ptr->next;

      u->chain_ptr=u->chain_ptr->next;
   };
	u->head_position=Offset();

	c->head_position=u->head_position;
	u->head_position=-1;


   bool joined=false;

   time_t now=time(0);

   if(u->glue_changes && u->chain_tail
   	&& now >=  u->last_change_time 
	&& now - u->last_change_time <= u->glue_max_time) {
      joined=undo_change_join(u->chain_tail,c);
	} else { // not joined!
     u->current_group++;
	};
	c->group=u->current_group;

   u->last_change_time=now;

   if(joined)
   {
      return;
   }

   c->prev=u->chain_tail;
   c->next=0;
   if(u->chain_tail)  u->chain_tail->next=c;
   u->chain_tail=c;
   if(!u->chain_head) u->chain_head=c;
   undo_set_glue();
}

void EndUndoGroup(UNDOS *u)
{
	u->head_position=-1;
	if(u->group_open<=0)  return;
	u->group_open--;
	undo_CheckSize(u);
}

void undo_CheckSize(UNDOS *u)
{
   undo_Change *scan=0;
   if(u->chain_ptr)
      scan=u->chain_ptr->prev;
   else
      scan=u->chain_tail;
   if(!scan)      return;
   unsigned g=scan->group;
   int count=1;
   offs size=0;
   while(scan)
   {
      size+=undo_GetSize(scan);
      scan=scan->prev;
      if(!scan)	 break;
      if(scan->group!=g)
      {
	 g=scan->group;
	 count++;
	 if(size>=u->max_size && count>u->min_groups)
	 {
	    scan->next->prev=0;
	    u->chain_head=scan->next;
	    while(scan)
	    {
	       undo_Change *to_delete=scan;
	       scan=scan->prev;
			free(to_delete);
	    }
	    break;
	 }
      }
   }
}

void undo_set_Change(undo_Change *uc,type_t t, char *l,num ls, char *r,num rs)
{
   uc->left=0;
   uc->left_size=0;
   uc->right=0;
   uc->right_size=0;

   uc->undo_type=t;
   uc->pos=Offset();
   uc->next=uc->prev=0;

   if(ls>0)
   {
      uc->left=(char*)emalloc(ls,"undo_set_change left");
      if(!uc->left)	 return;
   }
   if(rs>0)
   {
      uc->right=(char*)emalloc(rs,"undo_set_change right");
      if(!uc->right)
      {
	 efree(uc->left,"undo_set_change left");
	 uc->left=NULL;
	 return;
      }
   }
   uc->left_size=ls;
   uc->right_size=rs;
   if(ls>0)
      memcpy(uc->left,l,ls);
   if(rs>0)
      memcpy(uc->right,r,rs);
}

undo_Change * undo_new_Change(type_t t, char *l,num ls, char *r,num rs)
{
	undo_Change *uc=(undo_Change *)emalloc(sizeof(undo_Change),"undo_new_change");
	undo_set_Change(uc, t,l,ls,r,rs);
	return uc;
}


void UndoGroup(UNDOS *u)
{
   if(!u->chain_tail){
		return;
   };
   if(!u->chain_ptr)      u->chain_ptr=u->chain_tail;
   else
   {
      if(!u->chain_ptr->prev){
		cbfp->b_state &= ~(FS_CHG);

		msg_line("no more undos");
	  	return;
	 };
      u->chain_ptr=u->chain_ptr->prev;
   }
   unsigned g=u->chain_ptr->group;
   u->locked=true;
   for(;;)
   {
		undo_change_undo(u->chain_ptr);
		if(u->chain_ptr->prev==0 || u->chain_ptr->prev->group!=g) {
			break;
		};
		u->chain_ptr=u->chain_ptr->prev;
   }
   u->locked=false;
}

void RedoGroup(UNDOS *u)
{
   if(!u->chain_ptr)      return;
   unsigned g=u->chain_ptr->group;
   u->locked=true;
   for(;;)
   {
		undo_change_redo(u->chain_ptr);
		u->chain_ptr=u->chain_ptr->next;
      if(!u->chain_ptr || u->chain_ptr->group!=g) break;
   }
   u->locked=false;
}

void UndoOne(UNDOS *u)
{
   if(!u->chain_tail)      return;
   if(!u->chain_ptr) u->chain_ptr=u->chain_tail;
   else
   {
      if(!u->chain_ptr->prev)	 return;
      u->chain_ptr=u->chain_ptr->prev;
   }
   u->locked=true;
   undo_change_undo(u->chain_ptr);
   u->locked=false;
}

void RedoOne(UNDOS *u)
{
	if(!u->chain_ptr)      return;
	u->locked=true;
	undo_change_redo(u->chain_ptr);
	u->chain_ptr=u->chain_ptr->next;
	u->locked=false;
}

void undo_change_undo(undo_Change *uc)
{
   switch(uc->undo_type)
   {
   case DELETE:
      set_Offset(uc->pos-uc->left_size);
      InsertBlock(cbfp,uc->left,uc->left_size,uc->right,uc->right_size);
      break;
   case INSERT:
      set_Offset(uc->pos+uc->left_size);
      DeleteBlock(uc->left_size,uc->right_size);
      break;
   };
	set_modified(cbfp);
	set_Offset(uc->head_position);
}

void undo_change_redo(undo_Change *uc)
{
   set_Offset(uc->pos);
   switch(uc->undo_type)
   {
   case DELETE:
      DeleteBlock(uc->left_size,uc->right_size);
      break;
   case INSERT:
      InsertBlock(cbfp,uc->left,uc->left_size,uc->right,uc->right_size);
      break;
   };
   set_modified(cbfp);
}

static bool mappend(char **buf,offs *size,const char *add,offs add_size)
{
   if(add_size<=0)
      return true;
   char *newbuf=(char*)realloc(*buf,*size+add_size);
   if(!newbuf)
      return false;
   *buf=newbuf;
   memmove(*buf+*size,add,add_size);
   *size+=add_size;
   return true;
}

static bool mprepend(char **buf,offs *size,const char *add,offs add_size)
{
   if(add_size<=0)
      return true;
   char *newbuf=(char*)realloc(*buf,*size+add_size);
   if(!newbuf)
      return false;
   *buf=newbuf;
   memmove(*buf+add_size,*buf,*size);
   memmove(*buf,add,add_size);
   *size+=add_size;
   return true;
}

bool undo_change_join(undo_Change *c1, undo_Change *c)
{
   if(c->undo_type!=c1->undo_type)      return false;
   if(c->head_position != c->pos) return false;
   switch(c1->undo_type)
   {
   case DELETE:
      if(c1->pos-c1->left_size!=c->pos)
	 return false;
      if(!mappend(&c1->right,&c1->right_size,c->right,c->right_size))
	 return false;
      if(!mprepend(&c1->left,&c1->left_size,c->left,c->left_size))
      {
	 c1->right_size-=c->right_size;
	 return false;
      }
      break;
   case INSERT:
      if(c1->pos+c1->left_size!=c->pos)
	 return false;
      if(!mappend(&c1->left,&c1->left_size,c->left,c->left_size))
	 return false;
      if(!mprepend(&c1->right,&c1->right_size,c->right,c->right_size))
      {
	 c1->left_size-=c->left_size;
	 return false;
      }
      break;
   }
   return true;
}

void   PreModify(FILEBUF *fp)
{
   offs   shift=FOffset(fp) - fp->ptr1;

   if(fp->b_flag & FSMMAP) return;

   if(shift>0)
   {
      memmove(fp->buffer+fp->ptr1,fp->buffer+fp->ptr2,shift);
      fp->oldptr1=fp->ptr1+=shift;
      fp->oldptr2=fp->ptr2+=shift;
   }
   else if(shift<0)
   {
      memmove(fp->buffer+fp->ptr2+shift,fp->buffer+fp->ptr1+shift,-shift);
      fp->oldptr1=fp->ptr1+=shift;
      fp->oldptr2=fp->ptr2+=shift;
   };
//	MESG("Premodify: o=%ld shift=%ld ptr1=%ld ptr2=%ld gap=%ld",FOffset(fp),shift,fp->ptr1,fp->ptr2,fp->GapSize);
	refresh_textpoints(fp);
}

// return line len of a textpoint
int tp_llength(TextPoint *tp)
{
 offs o=tp_offset(tp);
 return LineEnd(o)-LineBegin(o);
}

// independent
// return line len at an offset
int offs_llength(offs o)
{
 return LineEnd(o)-LineBegin(o);
}

offs   LineBegin(offs ptr)
{
   if(cbfp->view_mode & VMHEX) {
   	offs o;
	o = (ptr) % HEX_LINE_LEN;
	ptr -= o;
   } else {
	while(!BolAt(ptr))  ptr--;
   };
	return(ptr);
}

offs   FLineBegin(FILEBUF *fp,offs ptr)
{
	// MESG("FLineBegin: ptr=%ld",ptr);
   if(fp->view_mode & VMHEX) {
   	offs o;
	o = (ptr) % HEX_LINE_LEN;
	ptr -= o;
   } else {

	if(tp_line(fp->tp_text_end)==0) {
	    // MESG("FlineBegin: %ld end line=%ld",ptr,tp_line(fp->tp_text_end));
		return 0;
	};
	while(!FBolAt(fp,ptr))  ptr--;
   };
	return(ptr);
}

/* Offset of current  point  */
offs CurrentLineBegin()
{
	return(LineBegin(Offset()));
}

// returns offset of line end
offs   LineEnd(offs ptr)
{
   if(cbfp->view_mode & VMHEX) {
		ptr += HEX_LINE_LEN - (ptr % HEX_LINE_LEN) -1;
		if(ptr>(FSize(cbfp))) ptr=FSize(cbfp);
	} else {
	   while(!EolAt(ptr)) ptr++;
	};
	return(ptr);
}

offs   FLineEnd(FILEBUF *fp,offs ptr)
{
 // MESG("FLineEnd: %lld",ptr);
   if(fp->view_mode & VMHEX) {
		offs o;
		o = ptr % HEX_LINE_LEN+1;
		ptr += HEX_LINE_LEN - o;
		if(ptr>FSize(fp)) ptr=FSize(fp);
	} else {
	   while(!FEolAt(fp,ptr)) ptr++;
	};
   return(ptr);
}

// no utf error yet!!
num utf_FLineLen(FILEBUF *fp, offs ptr)
{
 num len=0;
 utfchar uc;
 // MESG_time("utf_FLineLen: from %ld",ptr);
 while(!FEolAt(fp,ptr)) {
 	ptr=FUtfCharAt(fp,ptr,&uc);
	if(clen_error) set_utf8_error(1);
	len++;
 };
 // MESG_time("utf_FLineLen: till %ld len=%ld",ptr,len);
 return len;
}

int StartColumn(offs o)
{
	offs so;
	int c;
	offs start=LineBegin(o);
	for(so=start;(c=CharAt(so));so++) {
		if(c!=' '&&c!='	') break;
	};
	return(int)(so-start);
}

void	ToStartColumn()
{
	offs so;
	int c;
	for(so=LineBegin(Offset());(c=CharAt(so));so++) {
		if(c!=' '&&c!='	') break;
	};
	set_Offset(so);
}

void	ToLineBegin()
{
	set_Offset(LineBegin(Offset()));
}

void	ToLineEnd()
{
	set_Offset(LineEnd(Offset()));
}

void   MoveLeftChar(FILEBUF *fp)
{
 int clen=1;
 int i=0;
 int cl;
 offs o=FOffset(fp);
//	MESG("MoveLeftChar: o=%ld",o);
   if(fp->view_mode & VMHEX) {
	if(fp->view_mode & VMINP) {
		if(get_hex_byte()==0) {
			textpoint_set(fp->tp_current,o-1);
		};
		toggle_hex_byte();
	} else {
		textpoint_set(fp->tp_current,o-1);
	};
	return;
   }

   if(FBolAt(fp,FOffset(fp))) textpoint_set(fp->tp_current,o - fp->EolSize);
   else
   {
		if(fp->b_lang == 0) { /* in case of utf encoding  */

			// int previous_accents=0;
			for(i=2;i<5;i++){
				cl=FUtfCharLen(fp,o-i);
//				MESG("prev_char: o=%ld i=%d cl=%d accent=%d",o,i,cl,fp->utf_accent);
				if(cl==i) {
					if(fp->utf_accent) {
						// previous_accents++;
						clen=i;
//						MESG("	pc: accent! clen=%d previous_accents=%d",clen,previous_accents);
						if(clen==2) {
							cl=FUtfCharLen(fp,o-i-2);
//							MESG("prev_char:	cl=%d",cl);
							if(cl==4) clen+=2;
							else clen+=cl;
							if(fp->utf_accent) {
								cl=FUtfCharLen(fp,o-i-4);
//								MESG("	before double accent cl=%d",cl);
								clen+=cl;
							};
							break;
						};
					};
					clen=i;
					if(!clen_error)
					break;
				};
			};
			// MESG_time("move left:0");
			textpoint_move(fp->tp_current,-clen);
			// MESG_time("move left:1");
		} else textpoint_move(fp->tp_current,-1);
   }
}

offs   FPrevUtfCharAt(FILEBUF *fp,offs o, utfchar *uc)
{
 int clen=1;
 int i=0;
 int cl;

   if(FBolAt(fp,o)) {
	for(i=0;i<=fp->EolSize;i++) uc->uval[i]=fp->EolStr[i];
		return o-fp->EolSize;
   } else
   {
		if(fp->b_lang == 0) { /* in case of utf encoding  */

			// int previous_accents=0;
			for(i=2;i<5;i++){
				cl=FUtfCharLen(fp,o-i);
//				MESG("prev_char: o=%ld i=%d cl=%d accent=%d",o,i,cl,fp->utf_accent);
				if(cl==i) {
					if(fp->utf_accent) {
						// previous_accents++;
						clen=i;
//						MESG("	pc: accent! clen=%d previous_accents=%d",clen,previous_accents);
						if(clen==2) {
							cl=FUtfCharLen(fp,o-i-2);
//							MESG("prev_char:	cl=%d",cl);
							if(cl==4) clen+=2;
							else clen+=cl;
							if(fp->utf_accent) {
								cl=FUtfCharLen(fp,o-i-4);
//								MESG("	before double accent cl=%d",cl);
								clen+=cl;
							};
							break;
						};
					};
					clen=i;
					if(!clen_error)
					break;
				};
			};
		} else clen=1;
		o-=clen;
		o = FUtfCharAt(fp,o,uc);
   };
   return o-clen;
}

void set_Offset(offs offset)
{
/* tmp_current makes faster textpoint operations (about 50%)  */
// Handle cache of current point
 if(cbfp->tp_current->flags==0){
	cbfp->tmp_current->offset = cbfp->tp_current->offset;
	cbfp->tmp_current->line = cbfp->tp_current->line;
	cbfp->tmp_current->col = cbfp->tp_current->col;
	cbfp->tmp_current->flags = 0;
	cbfp->tmp_current->window = cbfp->tp_current->window;
 };

 textpoint_set(cbfp->tp_current,offset);
}

/* move current buffer pointer on character to the right */
void   MoveRightChar(FILEBUF *fp)
{
 int clen=1;
   if(fp->view_mode & VMHEX)
   {
	if(fp->view_mode & VMINP) {
		if(get_hex_byte()) textpoint_move(fp->tp_current,1);
		toggle_hex_byte();
	} else {
		textpoint_move(fp->tp_current,1);
	};
    return;
   };
	clen=FUtfCharLen(fp,FOffset(fp));
	// MESG("MoveRightChar:1 clen=%d o=%ld col=%ld",clen,fp->tp_current->offset,fp->tp_current->col);
	textpoint_move(fp->tp_current,clen);
	// MESG("MoveRightChar:2 clen=%d o=%ld col=%ld",clen,fp->tp_current->offset,fp->tp_current->col);
	tp_copy(fp->save_current,fp->tp_current);
}

int next_utf8_error(num n)
{
 int clen=0;
 FILEBUF *fp=cbfp;
 if(fp->b_lang!=0) {
	msg_line("this is not a utf8 file!");
	return(0);
 };
 offs o=tp_offset(fp->tp_current);
 clen_error=0;
 while(!FEofAt(fp,o)) {
 	clen=FUtfCharLen(fp,o);
	if(clen_error) break;
	o+=clen;
 };
 if(!FEofAt(fp,o)) {
 	textpoint_set(fp->tp_current,o);
	set_update(cwp,UPD_WINDOW);
	msg_line("Found utf8 error!");
	return 1;
 } else {
 	msg_line("No utf8 error!");
	return 0;
 }

}

#define	MemStep	(0x2000)

int    GetSpace(FILEBUF *fp,offs s)
{
   char   *nb;
   offs    _add;
   offs   nptr1=fp->ptr1;
   offs   nptr2=fp->ptr2;

   if(fp->b_flag & FSMMAP) return false;

if(fp->BufferSize < fp->ptr2) {
		fp->BufferSize = tp_offset(fp->tp_text_end);
		if(fp->BufferSize< nptr2) { 
			fp->BufferSize = nptr2;
		};
	};


   if(nptr1< fp->oldptr1)  nptr1=fp->oldptr1;
   if(nptr2> fp->oldptr2)  nptr2=fp->oldptr2;
   if(nptr2-nptr1>=s) return(true);

	   _add=(s-nptr2+nptr1+(MemStep));
	// MESG("GetSpace: size=%ld ptr2=%ld nptr2=%ld _add=%ld",fp->BufferSize,fp->ptr2,nptr2,_add);
//	MESG("GetSpace: realloc to %ld",fp->BufferSize+_add);
   if((nb=(char*)realloc(fp->buffer,fp->BufferSize+_add))==NULL)
   {
     ERROR("[%s] Not enough memory",fp->b_fname);
     return(false);
   }
	
   memmove(nb+nptr2+_add,nb+nptr2,fp->BufferSize-nptr2);

   fp->ptr2+=_add;
   fp->oldptr2+=_add;
   fp->BufferSize+=_add;
   fp->GapSize+=_add;
   fp->buffer=nb;
//	MESG(" : g=%ld ptr2=%ld s=%ld",fp->GapSize,fp->ptr2,fp->BufferSize);
   return(true);
}


/*
   CalculateLineCol - determines line and column in target point based
   on line and col in source point
*/
void  CalculateLineCol(FILEBUF *fp,num *line,num *col,offs source,offs target)
{
   offs	 bol_point;
	// MESG("CalculateLineCol:");	
   if(source>target) {
      for( ; source>target; source--)  {
	 	if(FBolAt(fp,source)) {
	    	(*line)--;
	    	*col=-1;
	 	} else if(*col!=-1) {
	    	if(FCharAt_NoCheck(fp,source-1)=='\t') *col=-1;
	    	else    (*col)--;
	 	}
      }
   };
   if(*col==-1) {
      source=FLineBegin(fp,source);
      *col=0;
   }
   for(bol_point=source++; source<=target; source++) {
      if(FBolAt(fp,source)) {
	 	(*line)++;
	 	bol_point=source;
	 	*col=0;
      }
   };

	source=bol_point;
	col += DiffColumn(fp,&source,target,"CalculateLineCol");
}


int   InsertBlock(FILEBUF *fp, char *block_left,offs size_left,char *block_right,offs size_right)
{
   num   i;
   offs  oldoffset;
   num   num_of_lines,num_of_columns,oldline;
   int   break_at;
   offs	 size;
   if(fp->b_flag & FSMMAP) return false;
   size=size_left+size_right;
   if(size==0) return(true);
	// MESG("InsertBlock:%s pos=%ld l=%ld r=%ld",fp->b_fname,FOffset(fp),size_left,size_right);
   PreModify(fp);

   if(size_left>0) {
      if(fp->oldptr1 > fp->ptr1) fp->oldptr1=fp->ptr1;
   }
	
   if(fp->ptr2 > fp->oldptr2 && size_right>0) {
      if(fp->ptr2-fp->oldptr2 > size_right || memcmp(fp->buffer+fp->ptr2-size_right,block_right,size_right))
   	 	fp->oldptr2=fp->ptr2;
   }

   if(GetSpace(fp,size_left+size_right)!=true) return(false);

   if(undo_enabled(fp)) {
		undo_AddChange(fp,undo_new_Change(INSERT,block_left,size_left,block_right,size_right));
	}

   memmove(fp->buffer+fp->ptr1,block_left,size_left);
   memmove(fp->buffer+fp->ptr2-size_right,block_right,size_right);

   oldoffset=FOffset(fp);
   break_at=-1;
   for(i=1; i<fp->EolSize; i++) {
      if(FBolAt(fp,oldoffset+i)) {
         break_at=i;
         break;
      }
   }

   num_of_lines=0;
   num_of_columns=tp_col(fp->tp_current);
   oldline=tp_line(fp->tp_current);

   fp->ptr1+=size_left;
   fp->ptr2-=size_right;
   fp->GapSize-=size;

   CalculateLineCol(fp,&num_of_lines,&num_of_columns,oldoffset,oldoffset+size_left);
   CalculateLineCol(fp,&num_of_lines,&num_of_columns,oldoffset+size_left,oldoffset+size);

//   MESG("	ins: old_offset=%ld left=%ld right=%ld lines=%ld break_at=%d join_at=%d",oldoffset,size_left,size_right,num_of_lines,break_at,join_at);
   TextPoint *scan;
   for(scan=fp->tp_last; scan; scan=scan->t_next) {
	  if(scan==fp->tp_current) {
//	  	MESG("	point: [%s] window %d,f=%d o=%ld l=%ld current skip!",tp_name[scan->tp_type],scan->window,scan->flags,scan->offset,scan->line);
	  	continue;
      } 
	  else { 
	  	if(scan->offset > oldoffset) 
		{
	         if(!(scan->flags&LINEUNDEFINED)) {
	            if(scan->line==oldline) scan->flags|=COLUNDEFINED;
	            scan->line+=num_of_lines;
	            if(break_at>0 && scan->offset>=oldoffset+break_at)
	               scan->line--;
	         }
	         scan->offset+=size;
//		  	MESG("	update point> [%s] window %d,f=%d o=%ld l=%ld",tp_name[scan->tp_type],scan->window,scan->flags,scan->offset,scan->line);
		} else {
//	  		MESG("	point: [%s] window %d,f=%d o=%ld l=%ld smaller skip!",tp_name[scan->tp_type],scan->window,scan->flags,scan->offset,scan->line);
	    };
	  }   
	}

//	MESG("	update current point= o=%ld->%ld l->%ld c=%ld",oldoffset,fp->tp_current->offset,fp->tp_current->line,fp->tp_current->col);
	// set_modified(fp);
	fp->b_state |= FS_CHG;
	update_lines(fp);
	return(true);
}

num fread16(char *file_name,char *buffer,num size)
{
 FILE *fi=fopen(file_name,"r");
 char *out = buffer;
 if(fi!=NULL) {
 MESG("fread16: %s size to read %ld",file_name,size);
 	size_t res;
	// size_t in_chars=0;
	// size_t out_chars=0;
	unsigned short in_i;

			fseek(fi,2,0);// start after bom
			
			while((res=fread((char *)&in_i,2,1,fi))==1) {
				unsigned int c,d;
				// char wchar[5];
			    c= in_i;
				
		        if ((c & 0xFC00) == 0xD800) {    /* surrogates */
					res=fread((char *)&in_i,2,1,fi);
					// in_bytes+=2;
			    	if (res!=1) {  break; };         /* shouldn't happens */
					d = in_i; // second part of surrogate
			    
		            if ((d & 0xFC00) == 0xDC00) {
		                c &= 0x03FF;
		                c <<= 10;
		                c |= d & 0x03FF;
		                c += 0x10000;
		            } else {
						MESG("error in surrogate!");
						c = '?';
			    	}
		        }
				int bits;
				/* assertion: c is a single UTF-4 value */
		        if      (c <    0x80) {  *out++=  c;                bits= -6; }
		        else if (c <   0x800) {  *out++= ((c >>  6) & 0x1F) | 0xC0;  bits=  0; }
		        else if (c < 0x10000) {  *out++= ((c >> 12) & 0x0F) | 0xE0;  bits=  6; }
		        else                  {  *out++= ((c >> 18) & 0x07) | 0xF0;  bits= 12; }
		 
		        for ( ; bits >= 0; bits-= 6) {
		            // if (out >= outend)  break;
		            *out++= ((c >> bits) & 0x3F) | 0x80;
		        }

			};
			
			fclose(fi);
			// at this point we have the file in the buffer 'out' as utf8.
	return out-buffer;
	} else return 0;
}

int   ReadBlock(char *fname,int fd,offs size,offs *act_read)
{
   FILEBUF *fp=cbfp;
   // MESG("ReadBlock:[%s] size=%ld",fname,size);
   if(fp->b_flag & FSMMAP)  return false;
   if(size==0)   {
      *act_read=0;
      return(true);
   }

   PreModify(fp);

   if(GetSpace(fp,size)!=true) return(false);
	if(fp->bom_type==FTYPE_UTF16BOM) {
		*act_read=fread16(fname,fp->buffer+fp->ptr1,size);
		fp->view_mode = EMDOS;
	} else {
	   *act_read=read(fd,fp->buffer+fp->ptr1,size);
	};
   if(*act_read==-1)	return(false);
   if(*act_read==0)		return(true);
   return(InsertBlock(fp,fp->buffer+fp->ptr1,*act_read,NULL,0));
}

int   ReplaceTextFromFile(char *file_name,FILEBUF *fp,int fd,offs size,offs *act_read)
{
   // buffer should be clear
   if(fp->b_flag & FSMMAP)      return false;
	// MESG("ReplaceTextFromFile:[%s] size=%ld ptr1=%ld ptr2=%ld",fp->b_fname,size,fp->ptr1,fp->ptr2);
   if(size==0) {
      *act_read=0;
      return(true);
   };
   if(GetSpace(fp,size)!=true)	return(false);
	if(fp->bom_type==FTYPE_UTF16BOM) {
		*act_read=fread16(file_name,fp->buffer+fp->ptr1,size);
		fp->view_mode = EMDOS;
	} else {
	   *act_read=read(fd,fp->buffer+fp->ptr1,size);
	};

   if(*act_read==-1)	return(false);
   if(*act_read==0)		return(true);

   fp->ptr1+=*act_read;
   fp->GapSize-=*act_read;
//	MESG("	ptr1=%ld ptr2=%ld gap=%ld",fp->ptr1,fp->ptr2,fp->GapSize);
   update_lines(fp);
   return(true);
}


int   GetBlock(FILEBUF *fp,char *copy,offs from,offs size)
{
   if(from<0)   {
     size+=from;
     from=0;
   }
   if(from+size > FSize(fp)) {
     size=FSize(fp)-from;
   }
   if(size<=0) return 0;

   if(from>=fp->ptr1)   {
      memcpy(copy,fp->buffer+from+fp->ptr2-fp->ptr1,size);
      return(size);
   }
   if(from+size<=fp->ptr1) {
      memcpy(copy,fp->buffer+from,size);
      return(size);
   }
   offs leftsize=fp->ptr1-from;
   memcpy(copy,fp->buffer+from,leftsize);
   memcpy(copy+leftsize,fp->buffer+fp->ptr2,size-leftsize);
   return(size);
}


// show as Unix or DOS or HEX
// operates on cbfp
int set_view_mode(num n)
{
	FILEBUF *fp=cbfp;
	offs offset=FOffset(fp);
	// MESG("set_view_mode: to mode %d",n);
   switch (n) {
	case 1: // Dos mode
		if ((fp->b_flag & FSDIRED) && !(fp->b_state & FS_VIEW)) break;
		if(fp->EolSize==1 || (fp->view_mode & VMHEX) ) {
		fp->EolSize=2;
		strlcpy(fp->EolStr,"\r\n",3);
		fp->b_mode |= EMDOS;
		fp->b_mode &= ~(EMMAC|EMUNIX);
		fp->view_mode &= ~VMHEX;
		};break;
   case 2: // Unix mode
		if ((fp->b_flag & FSDIRED) && !(fp->b_state & FS_VIEW)) break;
		if(fp->EolSize==2 || (fp->view_mode & VMHEX) || (fp->b_mode & EMMAC) ) {
		fp->EolSize=1;
   		strlcpy(fp->EolStr,"\n",3);
		fp->b_mode &= ~(EMDOS|EMMAC);
		fp->b_mode |= EMUNIX;
		fp->view_mode &= ~VMHEX;
		}; break;
	case 3: // hex mode
		if (fp->b_flag & FSDIRED && !(fp->b_state & FS_VIEW)) break;
		fp->view_mode = VMHEX;
		fp->EolSize=1;	/* this must be 1  */
		WINDP *wp;
		lbegin(window_list);
		while((wp=lget(window_list))!=NULL) {
			if(wp->w_fp == fp) {
				wp->w_infocol = 0;
			}
		};
		update_lines(fp);
		break;
   case 4: // Mac mode
		if (fp->b_flag & FSDIRED) break;
		{
		fp->EolSize=1;
   		strlcpy(fp->EolStr,"\r",3);
		fp->b_mode &= ~(EMDOS|EMUNIX);
		fp->b_mode |= EMMAC;
		fp->view_mode &= ~VMHEX;
		}; break;
	case 5: // Show lines
		{
		if (fp->b_flag & FSDIRED && !(fp->b_state & FS_VIEW)) break;
		if(fp->view_mode & VMHEX) {
		} else {
			fp->view_mode = VMLINES;
			WINDP *wp;
			lbegin(window_list);
			while((wp=lget(window_list))!=NULL) {
				if(wp->w_fp == fp) {
					wp->w_infocol = VMCOLS;
				}
			};
		}
		};break;
	case 6: // Show offset
		{
		if (fp->b_flag & FSDIRED && !(fp->b_state & FS_VIEW)) break;
		if(fp->view_mode & VMHEX) {
		} else {
			fp->view_mode = VMOFFSET;
			WINDP *wp;
			lbegin(window_list);
			while((wp=lget(window_list))!=NULL) {
				if(wp->w_fp == fp) {
					wp->w_infocol = VMCOLS;
				}
			};
		}
		};break;
	case 7: // Show vertical info lines
		{
		if (fp->b_flag & FSDIRED && !(fp->b_state & FS_VIEW)) break;
		if(fp->view_mode & VMHEX) {
		} else {
			fp->view_mode = VMINFO;
			WINDP *wp;
			int infocols=0;

			if((int)bt_dval("show_vinfo")) {
				infocols=0;
				set_bt_num_val("show_vinfo",0);
			} else {
				infocols=VMICOLS;
				set_bt_num_val("show_vinfo",1);
			};
			lbegin(window_list);
			while((wp=lget(window_list))!=NULL) {
					wp->w_infocol = infocols;
					wp->w_fp->view_mode |= VMINFO;
					set_update(wp,UPD_ALL);
			};
		}
		};break;
	case 8: // Hide info
		{
		if(fp->view_mode & VMHEX || is_wrap_text(fp)) {
		} else {
			if((int)bt_dval("wrap_mode")) break;
			fp->view_mode = 0;
			set_bt_num_val("show_vinfo",0);
			WINDP *wp;
			
			lbegin(window_list);
			while((wp=lget(window_list))!=NULL) {
				if(wp->w_fp == fp) {
					wp->w_infocol = 0;
				}
			};
		}
		};break;
	case 9: // Wrap mode
	  {
		// MESG_time("convert to wrap mode",0);
		// MESG("toggle wrap_mode! b_flag=%d",fp->b_flag);
		if (fp->b_flag >= FSNLIST || fp->view_mode & (VMHEX|VMINP)) break;
		// MESG("change wrap_mode, current is 0x%X",fp->view_mode);
		int v1=fp->view_mode & VMWRAP;

		if(v1) fp->view_mode &= ~VMWRAP;
		else fp->view_mode |= VMWRAP;
		// MESG("change view_mode new is  0x%X",fp->view_mode);
		
		if(v1) { // MESG("reset wrap_mode");
			fp->b_infocol=0;
		} else 
		{ // MESG("set wrap_mode");
			fp->b_infocol=1;
		};
		lbegin(window_list);
		while((wp=lget(window_list))!=NULL) {
			if(wp->w_fp == fp) {
				wp->w_infocol = fp->b_infocol;
				set_update(wp,UPD_ALL);
			}
		};
	  };break;
   };
	// MESG_time("before reset points",1);
	ResetTextPoints(cbfp,0);
	// MESG_time("after reset points",1);
	set_Offset(offset);
	set_update(cwp,UPD_EDIT|UPD_FULL);
	return true;
}

// operates on cbfp
int set_doc_lang(num n)
{
 if(n<1 || n>20) n=1;
 cbfp->b_lang = n-1;
 ResetTextPoints(cbfp,0);
 set_update(cwp,UPD_FULL);
 return OK_CLRSL;
}

int set_default_local(num n)
{
 if(n<1 || n>20) n=1;
 default_local_codepage = n-1;

 ResetTextPoints(cbfp,0);
 set_update(cwp,UPD_FULL);
 return OK_CLRSL;
}

size_t write_utf8_as_utf16(int file_id,char *in,size_t inlen)
{
 // FILE *fout;

    // unsigned short* out = (unsigned short*) outb;
    // const unsigned char* processed = in;
    // unsigned short* outstart= out;
    // unsigned short* outend;
    char* inend= in+inlen;
    unsigned int c, d;
    int trailing;
    // unsigned char *tmp;
//     unsigned short tmp1, tmp2;
	size_t out_len=0;
	// size_t res=0;
	unsigned short out_i=0xFEFF;
	// MESG("write_utf8_as_utf16: buffer size=%ld",inlen);
	if (in==NULL) return 0;
	// fout=fdopen(file_id,"w");
#if	0
	write(file_id,(char *)&out_i,2);
#endif
	if (inlen<1) { 
		// fclose(fout);
		return 0;
    };	// zero length file written!
    while (in < inend) {
      d= *in++;
	  // printf("%c",d);
      if      (d < 0x80)  { c= d; trailing= 0; }
      else if (d < 0xC0) {
        /* trailing byte in leading position */
		break;
      } else if (d < 0xE0)  { c= d & 0x1F; trailing= 1; }
        else if (d < 0xF0)  { c= d & 0x0F; trailing= 2; }
        else if (d < 0xF8)  { c= d & 0x07; trailing= 3; }
      else {
		/* no chance for this in UTF-16 */
		break;
      }

      if (inend - in < trailing) {
	  	  // MESG("break1:");
          break;
      } 

      for ( ; trailing; trailing--) {
          if ((in >= inend) || (((d= *in++) & 0xC0) != 0x80)){
		  	  // MESG("break2:");
	      	break;
		  };
		  // printf("%c",d);
          c <<= 6;
          c |= d & 0x3F;
      }

      /* assertion: c is a single UTF-4 value */
        if (c < 0x10000) {
			out_i = c;
			// fwrite((void *)&out_i,2,1,fout);
			if(write(file_id,(void *)&out_i,2)!=2) break;
			out_len+=2;			
        }
        else if (c < 0x110000) {
            c -= 0x10000;
			out_i = 0xD800 | (c >> 10);
			// fwrite((void *)&out_i,2,1,fout);
			if(write(file_id,(void *)&out_i,2)!=2) break;
			out_i = 0xDC00 | (c & 0x03FF);
			// fwrite((void *)&out_i,2,1,fout);
			if(write(file_id,(void *)&out_i,2)!=2) break;
			out_len+=2;
        } else {
			// MESG("breal3:");
			break;
		};
		// processed = in;
    }
	// fclose(fout);
	// MESG("write %ld -> %ld",inlen,out_len);
 return out_len;
}

/* operates on cbfp */
int   WriteBlock(int fd,offs from,offs size,offs *act_written)
{
   FILEBUF *fp=cbfp;
   offs   leftsize;
	// MESG("writeblock: bom=%d",fp->bom_type);
   if(from<0) {
     size+=from;
     from=0;
   };
   if(from+size>FSize(fp)) {
     size=FSize(fp)-from;
   }
   if(size<=0) {
     *act_written=0;
     return(true);
   }
   if(fp->bom_type==FTYPE_UTF16BOM) {
   	unsigned short bom16 = 0xFEFF;
	if(write(fd,(char *)&bom16,2)!=2) {
		*act_written=0;
		return(false);
	};
   };
   if(from>=fp->ptr1) {
	if(fp->b_mode & EMCRYPT) crypt_string(fp->buffer+from+fp->ptr2-fp->ptr1,size);
	if(fp->bom_type==FTYPE_UTF16BOM) 
		*act_written=(unsigned long int)write_utf8_as_utf16(fd,fp->buffer+from+fp->ptr2-fp->ptr1,size);
    else 
		*act_written=write(fd,fp->buffer+from+fp->ptr2-fp->ptr1,size);

	if(fp->b_mode & EMCRYPT){
		resetkey(fp);
		crypt_string(fp->buffer+from+fp->ptr2-fp->ptr1,size);
	};
	  if(*act_written==-1) {
	     *act_written=0;
	     return(false);
	  };
	  return(true);
   }
   if(from+size<=fp->ptr1) {
   	  if(fp->b_mode & EMCRYPT) crypt_string(fp->buffer+from,size);
	  if(fp->bom_type==FTYPE_UTF16BOM) 
	  	*act_written=write_utf8_as_utf16(fd,fp->buffer+from,size);
	  else
      	*act_written=write(fd,fp->buffer+from,size);
   	  if(fp->b_mode & EMCRYPT) {
	  	resetkey(fp);
		crypt_string(fp->buffer+from,size);
	  };
      if(*act_written==-1) {
         *act_written=0;
         return(false);
      }
      return(true);
   }
   leftsize=fp->ptr1-from;
	if(fp->b_mode & EMCRYPT) crypt_string(fp->buffer+from,leftsize);
   	if(fp->bom_type==FTYPE_UTF16BOM) 
		*act_written=write_utf8_as_utf16(fd,fp->buffer+from,leftsize);
	else
    	*act_written=write(fd,fp->buffer+from,leftsize);

   if(*act_written==-1) {
      *act_written=0;
	  if(fp->b_mode & EMCRYPT) {
	  	resetkey(fp);
		crypt_string(fp->buffer+from,leftsize);
	  };
      return(false);
   };
   if(*act_written<leftsize) {
	  if(fp->b_mode & EMCRYPT) {
	  	resetkey(fp);
		crypt_string(fp->buffer+from,leftsize);
	  };
	 return(true);
 	};
	if(fp->b_mode & EMCRYPT) crypt_string(fp->buffer+fp->ptr2,size-leftsize);
	if(fp->bom_type==FTYPE_UTF16BOM) 
		*act_written=write_utf8_as_utf16(fd,fp->buffer+fp->ptr2,size-leftsize);
	else 
   		*act_written=write(fd,fp->buffer+fp->ptr2,size-leftsize);
	if(fp->b_mode & EMCRYPT){
		resetkey(fp);
		crypt_string(fp->buffer+from,leftsize);
		crypt_string(fp->buffer+fp->ptr2,size-leftsize);
	};

   if(*act_written==-1) {
      *act_written=leftsize;
      return(false);
   };
   *act_written+=leftsize;
   return(true);
}

void show_textpoints(char *title,FILEBUF *fp)
{
	TextPoint *scan;
	for(scan=fp->tp_last; scan; scan=scan->t_next){
		if(scan->offset>0)
		MESG(" %-10s %-12s o=%ld l=%ld c=%ld f=%d",title,tp_name[scan->tp_type],scan->offset,scan->line,scan->col,scan->flags);
	};
}

#include "replaceblock.c"

/* delete a block from left to right offset */
/* operates on cbfp */
int   DeleteBlock(offs left,offs right)
{
   offs  base;	// start of delete
   offs   size;	// size of delete
   num   i;
   num   newline;
   num   deleted_lines;
   int   join_at;
   int   break_at;
   FILEBUF *fp=cbfp;
//	MESG("DeleteBlock: at %ld l=%ld r=%ld ptr2=%ld size=%ld",FOffset(fp),left,right,fp->ptr2,fp->BufferSize);
   if(left<1 && right<1)  return(true);
   if(fp->b_flag & FSMMAP)  return false;
//	show_textpoints("DB:",fp);
   if(left > FOffset(fp))  left=FOffset(fp);
   if(right>FSize(fp)-FOffset(fp))      right=FSize(fp)-FOffset(fp);

   PreModify(fp);
	
   if(fp->oldptr1<fp->ptr1 && left>0)      fp->oldptr1=fp->ptr1;
   if(fp->oldptr2>fp->ptr2 && right>0)      fp->oldptr2=fp->ptr2;
   if(undo_enabled(fp)) {
		 undo_AddChange(fp,undo_new_Change(DELETE,fp->buffer+fp->ptr1-left,left,fp->buffer+fp->ptr2,right));
	};

   size=left+right;
   base=FOffset(fp)-left; // positive!

   TextPoint *base_point=textpoint_o(fp,base,TP_BP_DELETE_BLOCK,0);
   newline=tp_line(base_point);

	TextPoint *t=textpoint_o(fp,base+size,TP_DELETE_BLOCK,0);

	deleted_lines=tp_line(t)-newline;	// lines deleted
	textpoint_delete(t);

   break_at=-1;
   for(i=1; i<fp->EolSize; i++) {
      if(FBolAt(fp,base+size+i)) {
         break_at=i;
         break;
      }
   }

   fp->ptr1-=left;
   fp->ptr2+=right;
   fp->GapSize+=left+right;

   join_at=-1;
   for(i=1; i<fp->EolSize; i++) {
     if(FBolAt(fp,base+i)) {
       join_at=i;
       break;
     }
   }

	TextPoint *scan;
	for(scan=fp->tp_last; scan; scan=scan->t_next){
		if(scan->offset > base) { 
			scan->flags = LINECOLUNDEFINED;
		}
	}

   for(scan=fp->tp_last; scan; scan=scan->t_next) {
//	  MESG("check text point num=%d type %d= [%s] o=%ld bs=%ld o=%ld l=%ld",scan->num,scan->tp_type,tp_name[scan->tp_type],scan->offset,base+size,tp_offset(scan),tp_line(scan));
      if(scan->offset >= base+size) 
	  {
        scan->offset-=size;	// this change the column and possibly the line!!!!!!
         // if((scan->flags&LINEUNDEFINED)) 
		 {
            scan->line-=deleted_lines;
            if(break_at>0 && scan->offset>=base+break_at)  scan->line--;
            if(join_at>0  && scan->offset>=base+join_at)  scan->line++;
            if(scan->line==newline)  {
				scan->flags|=COLUNDEFINED;
				FindLineCol(scan);
			} else {
				scan->flags=0;
			};
         };
      } else if(scan->offset>=base) {
	 	tp_copy(scan,base_point);
      }
//	  MESG("	new o=%ld l=%ld",tp_offset(scan),tp_line(scan));
   }
	textpoint_delete(base_point);

	// set_modified(fp);
	update_lines(fp);
   return(true);
}


#if	1
offs	FCheckNextLine(FILEBUF *fp, offs ptr, num *display_size)
{
 utfchar uc;
 num col=0;;
 num file_size=FSize(fp);

 if(fp->EolSize>1) {
	char c0=fp->EolStr[0];
	char c1=fp->EolStr[1];
	while((ptr=FUtfCharAt_nocheck(fp,ptr,&uc)) < file_size){
		if(uc.uval[0]==c0) {
			if(FCharAt_NoCheck(fp,ptr)==c1) {
				ptr++;
			} else continue; 
			*display_size=col;
			// MESG("Line_size:0 %ld",*display_size);
			return ptr;
		};
		if(uc.uval[0]==CHR_TAB) col=next_tab(col);
		else col+=get_utf_length(&uc);
	}
 } else{
 	char c0=fp->EolStr[0];
	while(ptr <= file_size)
	{
		ptr=FUtfCharAt_nocheck(fp,ptr,&uc);
		if(uc.uval[0]==c0) {
			*display_size=col;
			return ptr;
		};
		if(uc.uval[0]==CHR_TAB) col=next_tab(col);
		else col+=get_utf_length(&uc);
	};
 };
 *display_size=col;
 // MESG("Line_size:0 %ld",*display_size);
 return ptr;
}
#else
offs	FCheckNextLine(FILEBUF *fp, offs ptr, num *display_size)
{
 utfchar uc;
 num col=0;;
 num file_size=FSize(fp);

 while((ptr=FUtfCharAt_nocheck(fp,ptr,&uc)) < file_size){
	if(uc.uval[0]==fp->EolStr[fp->EolSize-1]) {
		if(fp->EolSize>1) {
			if(FCharAt_NoCheck(fp,ptr)==fp->EolStr[1]) {
				ptr++;
			} else continue; 
		};
		*display_size=col;
		return ptr;
	};
	if(uc.uval[0]==CHR_TAB) col=next_tab(col);
	else col+=get_utf_length(&uc);
 };
 *display_size=col;
 return ptr;
}
#endif

offs   FNextLine(FILEBUF *fp,offs ptr)
{
   char eol=fp->EolStr[fp->EolSize-1];
   char *found=NULL;
   offs size=FSize(fp);
   char *start=fp->buffer;

	// MESG("FNextLine: o=%lld eolsize=%d eol=%d ptr1=%lld",ptr,fp->EolSize,eol,fp->ptr1);
	if(fp->view_mode & VMHEX) {
		if((ptr%16)!=0) ptr -= (ptr%16);
		ptr+=16;	
		if(ptr>size) ptr=size;
		return(ptr);
	};


	if(fp->EolSize>1) {
		if(FCharAt(fp,0)==eol) ptr++;
	};
   while(ptr < fp->ptr1) {
      found=(char*)memchr(fp->buffer+ptr,eol, fp->ptr1 - ptr);
	
	  if(found==NULL) {
		ptr=fp->ptr1;
		break;
	  } else 
      {
	    ptr=found+1-fp->buffer;
		// MESG("	found at: %lld",ptr);
		if(fp->EolSize>1) 
			if(*(fp->buffer+ptr-fp->EolSize)!=fp->EolStr[0]) {
				continue;
			};
		return ptr;
      }
   };

   start = fp->buffer+fp->GapSize;

   while(!found) {
      found=(char*)memchr(start+ptr,eol,size-ptr);
      if(!found) { 
	  	return size;
	  };
      ptr=found-start;
	  if(ptr+1<=size) { // ptr1 
		if(fp->EolSize>1) {
			if(*(found-1)!=fp->EolStr[0]) {
				ptr++;
				found=NULL;
				continue;
			};
		}
	  	return ptr+1;
	  }
   };

   return size;
}

offs   FPrevLine(FILEBUF *fp,offs ptr)
{
   // MESG("FPrevLine:");
   ptr=FLineBegin(fp,ptr);
   while(ptr>0 && !FBolAt(fp,--ptr));
   return(ptr);
}


void show_points(FILEBUF *bf,FILE *fp)
{
 TextPoint *scan;

 fprintf(fp,"%c[0mnum:  offset    line  column flags  pointer -------- buffer points\n",27);
 for(scan=bf->tp_last; scan; scan=scan->t_next) {
 	fprintf(fp,"%c[3%cm%3d: %10lld %5lld %5lld %5d %10ld  %s : %d%c[0m\n",27,49+scan->window,scan->num,scan->offset,scan->line,scan->col,scan->flags,(long)scan,tp_name[scan->tp_type],scan->window,27);
 };
}

void  CheckPoint(FILEBUF *fp)
{
	fp->oldptr1=fp->ptr1;
	fp->oldptr2=fp->ptr2;
}


int empty_tok_table(FILEBUF *fp);

void  EmptyText(FILEBUF *bp)
{
   // MESG("EmptyText:[%s]",bp->b_fname);
   if(bp==NULL) return;
   undo_clear(bp->main_undo);
   ResetTextPoints(bp,1);
   if(bp->buffer) {
      if(bp->b_flag & FSMMAP) {
	 	munmap(bp->buffer,bp->BufferSize);
      } else  {
	 	efree(bp->buffer,"EmptyText, bp->buffer");
      }
      bp->buffer=NULL;
   };

   bp->BufferSize=0;
   bp->GapSize=0;
   bp->oldptr1=bp->ptr1=bp->oldptr2=bp->ptr2=0;
   bp->lines=0;
   bp->EolSize=1;
   bp->b_state &= ~(FS_CHG);
	
   strlcpy(bp->EolStr,"\n",3);
   if(bp->tok_table!=NULL){
	empty_tok_table(bp);
   };
   bp->err=-1;

	if(bp->symbol_tree) {
		free_btree(bp->symbol_tree);
		bp->symbol_tree=NULL;
	};
#if	TNOTES
	if(bp->b_tag_list) free_list(bp->b_tag_list,"delete_filebuf tag list");
	bp->b_tag_list=NULL;
#endif
	bp->b_header=NULL;
}

// get size bytes in buf at o offset till eol
// used in search, eval
offs get_text_offs(FILEBUF *bf,char *buf, offs o, offs size)
{
 offs len=0;
 while(!FEolAt(bf,o)) {
 	buf[len]=FCharAt(bf,o++);
	len++;
	if(len>size) break;
 };
 buf[len]=0;
 return (len);
}

/* operates on cbfp,cwp */
int undo(num n)
{
//	if(cwp->w_fp->b_flag & FSDIRED) return 0;

	while(n--) {
		UndoGroup(cbfp->main_undo);
	};
   set_update(cwp,UPD_WINDOW|UPD_EDIT);
	return 1;
}

char *get_line_at(FILEBUF *fb,offs offset)
{
	static char line[2048];
	offs bol;
	offs eol;
	int ind=0;
	// MESG("get_line_at:");
	bol=FLineBegin(fb,offset);
	eol=FLineEnd(fb,offset);
	
	for(ind=0;ind<eol-bol;ind++){
		line[ind]=FCharAt(fb,bol+ind);
	};
	line[ind]=0;
	// MESG("get_line_at:[%s]",line);
	return(line);
}

/* operates on cbfp,cwp */
int redo(num n)
{
	while(n--) {
		RedoGroup(cbfp->main_undo);
	};
	set_update(cwp,UPD_WINDOW|UPD_EDIT);
	return 1;
}

/* -- */
