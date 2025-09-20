/*
	Curses,gtk editor,notes,directory browser
	file convertion utilities
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.
*/

#include	"xe.h"
#if	USE_GLIB
#include	<glib-2.0/glib/gconvert.h>
#endif

void  textpoint_OrFlags(FILEBUF *fp,int mask);
void reset_points(FILEBUF *bf);

num  ConvertFromUnixToDos(FILEBUF *fp)
{
 num ptr=0L;
 num size=FSize(fp);
// TextPoint *te=fp->tp_text_end;
 int line=0;
// MESG("convertFromUnixToDos: size=%lld",size);
 while(size>0) {
      if(CharAt(ptr)=='\n') {
	 	textpoint_set(fp->tp_current,ptr);
	 	InsertChar(fp,'\r');
	 	ptr++;
		line++;
//		MESG("u2d line=%d e=%lld lines=%lld col=%lld f=%d",line,te->offset,te->line,te->col,te->flags);
      };
      size--;
      ptr++;
   };
   MESG("convertFromUnixToDos: lines=%lld size=%lld %lld",line,ptr,FSize(fp));
   return(ptr);
}

num  ConvertFromDosToUnix(FILEBUF *fp)
{
 num ptr=0L;
 num size=FSize(fp);
// TextPoint *te=fp->tp_text_end;
 int line=0;
   while(size>0)
   {
      if(FCharAt(fp,ptr)=='\r' && FCharAt(fp,ptr+1)=='\n') {
	 	textpoint_set(fp->tp_current,ptr);
	 	DeleteChar();
	 	size--;
		line++;
//		MESG("d2u line=%d e=%lld lines=%lld col=%lld f=%d",line,te->offset,te->line,te->col,te->flags);
      };
      size--;
      ptr++;
   };
   fp->lines=line;
   MESG("ConvertFromDisToUnix: lines=%ld ptr=%d",fp->lines,ptr);
   return(ptr);
}

int dos2unix(num n)
{
 FILEBUF *cfp=current_filebuf();
 num newsize=0;
 num oldsize=FSize(cfp);
 if (cfp->EolSize==1) return false;
 newsize=ConvertFromDosToUnix(cfp);
 if(newsize==oldsize) return false; 	/* no conversion occured!  */
 cfp->EolSize=1;
 strlcpy(cfp->EolStr,"\n",2);

 if(cfp->b_mode & EMCRYPT) cfp->b_mode =EMCRYPT|EMUNIX;
 else cfp->b_mode = EMUNIX;
 textpoint_OrFlags(cfp,COLUNDEFINED|LINEUNDEFINED);
 if(cfp->b_nwnd>0)
 if(cwp->w_fp == cfp) {
 	textpoint_set(cwp->tp_hline,0);
 };
 textpoint_set(cfp->tp_current,0);
// MESG("dos2unix: current=(%ld %ld %ld f=%X)",cfp->tp_current->line,cfp->tp_current->col,cfp->tp_current->offset,cfp->tp_current->flags);
// MESG("dos2unix: tmp_current=(%ld %ld %ld f=%X)",cfp->tmp_current->line,cfp->tmp_current->col,cfp->tmp_current->offset,cfp->tmp_current->flags);
// MESG("dos2unix: text_end=(%ld %ld %ld f=%X)",cfp->tp_text_end->line,cfp->tp_text_end->col,cfp->tp_text_end->offset,cfp->tp_text_end->flags);
// MESG("dos2unix: text_o=(%ld %ld %ld f=%X)",cfp->tp_text_o->line,cfp->tp_text_o->col,cfp->tp_text_o->offset,cfp->tp_text_o->flags);
// MESG("dos2unix: oldsize=%lld newsize=%lld end column: %lld %lld",oldsize,newsize,tp_col(cfp->tp_text_end),tp_col(cfp->tp_text_o));
 
 // MESG("	dos2unix: newsize=%ld",newsize);
 textpoint_set(cfp->tp_text_end,newsize);
 textpoint_set(cfp->tp_text_o,newsize);
// MESG("dos2unix: text_end=(%ld %ld %ld f=%X)",cfp->tp_text_end->line,cfp->tp_text_end->col,cfp->tp_text_end->offset,cfp->tp_text_end->flags);
// MESG("dos2unix: text_o=(%ld %ld %ld f=%X)",cfp->tp_text_o->line,cfp->tp_text_o->col,cfp->tp_text_o->offset,cfp->tp_text_o->flags);

 cfp->lines = tp_line(cfp->tp_text_end);
 // MESG(" dos2unix: lines = %ld",cfp->lines);
 if(cfp->b_nwnd>0){
	set_update(cwp,UPD_EDIT);
 };
 return true;
}

int unix2dos(num n)
{
 FILEBUF *cfp=current_filebuf();
 num newsize=0;
 num oldsize=FSize(cfp);
 if(cfp->EolSize==2) return false;
 cfp->EolSize=2;
 strlcpy(cfp->EolStr,"\r\n",3);

 newsize=ConvertFromUnixToDos(cfp);
 if(newsize==oldsize) return false;	/* no conversion occured  */
 if(cfp->b_mode & EMCRYPT) cfp->b_mode=EMCRYPT|EMDOS;
 else cfp->b_mode = EMDOS;
 
// textpoint_OrFlags(cfp,COLUNDEFINED|LINEUNDEFINED);
 if(cfp->b_nwnd>0)
 if(cwp->w_fp == cfp) {
 	textpoint_set(cwp->tp_hline,0);
 };
 textpoint_set(cfp->tp_current,0);
 textpoint_set(cfp->tp_text_end,newsize);
 textpoint_set(cfp->tp_text_o,newsize);
// MESG("unix2dos: current=(%ld %ld %ld f=%X",cfp->tp_current->line,cfp->tp_current->col,cfp->tp_current->offset,cfp->tp_current->flags);
// MESG("unix2dos: tmp_current=(%ld %ld %ld f=%X",cfp->tmp_current->line,cfp->tmp_current->col,cfp->tmp_current->offset,cfp->tmp_current->flags);
// MESG("unix2dos: oldsize=%lld newsize=%lld end column: %d %d",oldsize,newsize,tp_col(cfp->tp_text_end),tp_col(cfp->tp_text_o));
 cfp->lines = tp_line(cfp->tp_text_end);
// MESG("new lines=%lld",cfp->lines);
 if(cfp->b_nwnd>0){
	set_update(cwp,UPD_EDIT);
 };
 return true;
}

#if	USE_GLIB

/* Translate output to ISO-8859-7 , IBMPC 737 */
char x_out[2][128] = {
 {
 193,194,195,196,197,198,199,200, 201,202,203,204,205,206,207,208,
 209,211,212,213,214,215,216,217, 225,226,227,228,229,230,231,232,
 233,234,235,236,237,238,239,240,241,243,242,244,245,246,247,248,
 2  ,2  ,2  , 25,22 ,22 ,22 , 12,12 ,22 ,25 ,12 ,11 ,11 ,11 ,12 ,
// 14 , 23, 24, 21, 18,15 ,21 , 21,14 ,13, 23, 24, 21, 18 ,15, 23 ,
 131 , 23, 24, 21, 18,143 ,21 , 21,131 ,13, 23, 24, 21, 18 ,143, 23 ,
// 23 , 24, 24, 14, 14, 13, 13, 15, 15,11 , 13, 2 ,2  ,2 , 2 , 2,
 23 , 24, 24, 131, 131, 134, 134, 143, 143,11 , 134, 2 ,2  ,2 , 2 , 2,
 249,220,221,222,250,223,252,253,251,254,182,184,185,186,188,190,
 191,177, 27, 26,1  , 1 , 1,1,176,183,31,164,1,178,1,1    
 },
 {
 176,177,178,179,180,181,183,183, 184,185,186,187,188,189,190,191, /* 128 */
 192,193,194,195,196,197,198,199, 200,201,202,203,204,205,206,207,
 208,209,210,211,212,213,214,215, 216,217,218,219,220,221,222,223, /* 160 */
 48, 49, 50, 51, 52, 53, 54, 55,  56, 57, 58, 59, 60, 61, 62, 63,  /* 186 */
 0xE5,'A','B',0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E, /* 192 */
 0x8F,0x90,32 ,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x88,0x93,0xE1,0xE2,0xE3,0xE5, /* 208 */
 0xE7,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6, /* 224 */
 0xA7,0xA8,0xAA,0xA9,0xAB,0xAC,0xAD,0xAE,0xAF,0xE0,0xE4,0xE8,0xE6,0xE7,0xE9,255  /* 240 */
 },
};

inline int xlate(int lang, int c)
{
	return x_out[lang][c-128];
}

/* Character convertions */
int convert_char(num n)
{
 num op;
 byte c;
 FILEBUF *cfp=current_filebuf();

 if(test_b_flag(FSDIRED)) return(0);
 if(dont_edit()) return FALSE;
 n--;
 set_update(cwp,UPD_FULL);
 // keep current line
 if(n<2) { 
 for(op=0;op<FSize(cfp);op++) {
	c=CharAt(op);
	if(c>=128) {
		if(n==0) c=xlate(0,c); // IBMPC737 to ISO8859-7
		else c=xlate(1,c);	   // ISO8859-7 to IBMPC737
		set_Char(op,c);
	};	
 }
 } 
 else { // utf8 conversions
	char *cc1=NULL,*c2;
	int len,i;
	num lbegin;
	num cline=GetLine();
	// MESG("convert arg=%d",n+1);
	if(n==2) {	// convert from local to utf
	op=0;
	
	// if(cfp->b_lang) MESG("convert from local to utf : b_lang=%d %s --> utf",cfp->b_lang,codepage_str[cfp->b_lang]);
	// else MESG("convert from default local page %d to utf: %s --> utf",default_local_codepage,codepage_str[default_local_codepage]);
	lbegin=op;
	while(!FEofAt(cfp,op)) {
		len=LineEnd(op)-lbegin;
		cc1=malloc(4*len+1);
		c2=cc1;
		for(i=0;i<len;i++,op++) {
			c=CharAt(op);
			if(c<128) *c2++=c;
			else
			/* this work for all locals but requires glib support! */
			{
			 char *ui1,*ui,u[2];
			 gsize r,w;
			 u[0]=c;u[1]=0;
			 if(cfp->b_lang)
			 	ui1 = ui = g_convert(u,1,"UTF-8",codepage_str[cfp->b_lang],&r,&w,NULL);
			 else
				ui1 = ui = g_convert(u,1,"UTF-8",codepage_str[default_local_codepage],&r,&w,NULL);
			 if(ui!=NULL) {
				while(*ui1!=0) *c2++ = *ui1++;
				g_free(ui);
			 } else {
				*c2++ = ' '; continue;
			 };

			};
	  };
	  *c2=0;
	  textpoint_set(cwp->tp_current,lbegin);
	  DeleteBlock(cfp,0,len);
	  textpoint_set(cwp->tp_current,lbegin);
	  InsertBlock(cfp,cc1,c2-cc1,0,0);
	  op=lbegin=FNextLine(cfp,lbegin);
	  free(cc1);
	};
	  cfp->b_lang=0;
	} else { // convert utf8 to local
	// MESG("convert from utf8 to local: -> %s",codepage_str[default_local_codepage]);
	op=0;
	lbegin=op;
	while(!FEofAt(cfp,op)) {
		len=LineEnd(op)-lbegin;
		cc1=malloc(len+1);
		c2=cc1;
		gchar *cc2;
		gsize r,w;
		for(i=0;i<len;i++,op++) {
			c=CharAt(op);
			*c2++=c;
		};
		cc2=g_convert(cc1,len,codepage_str[default_local_codepage],"UTF-8",&r,&w,NULL);

	  textpoint_set(cwp->tp_current,lbegin);
	  DeleteBlock(cfp, 0,len);
	  textpoint_set(cwp->tp_current,lbegin);
	  InsertBlock(cfp,cc2,w,0,0);
	  op=lbegin=FNextLine(cfp,lbegin);
	  free(cc1);
	  g_free(cc2);
	};

	};
	textpoint_set_lc(cwp->tp_current,cline,0);
 };
 set_modified(cfp);
 return(TRUE);
}
#endif

/* -- */
