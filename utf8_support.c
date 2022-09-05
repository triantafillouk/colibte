/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

*/

#if	!defined _UTF8_SUPPORT
#define	_UTF8_SUPPORT

#include "utf8_support.h"
#include <string.h>
#include "support.h"


int utf_error=0;

int utf8_error()
{
	return utf_error;
}

void set_utf8_error(int error)
{
	utf_error=error;
}

/*
	Invalid characters return value below zero
*/
int utf8_countBytes[256] = {
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, // 128
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 
	-2,-2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,	// 192
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
	4,4,4,4,4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4
};

int utf8_countbytes(int c)
{
	return utf8_countBytes[c];
}

/* return number of utf-8 characters of a string.
	returns -1 on utf-8 error.
	sets utf_error to position of first errored character
*/
int utf_num_chars(char *utf)
{
 int len=strlen(utf);
 int i,j;
 int trail=0;
 int ch;
 int utflen=0;

 utf_error=0;
 i=0;
 while(i<len) {
 	ch=utf[i];
	trail=utf8_countBytes[ch]-1;
	if(trail<0) {
		utf_error=trail;
		return -1;
	};
	i++;
	utflen++;
	for(j=0;j<trail;j++,i++) {
		if(i>=len) {
			utf_error=i-j;
			return -1;
		};
		if(utf[i]<128 || utf[i]>191) {
			utf_error=i-j;
			return -1;
		};
	};
 };
 return utflen;
}

int SUtfCharLen(char *utfstr,int offset,utfchar *uc)
{
 int ch;
 int ch1;
 int clen=1;
 int utf_accent=0;
 int o=offset;
 int size=strlen(utfstr);
	if(*utfstr==0) { uc->uval[0]=0;return 0;};

		ch=utfstr[o];
		if(ch<0xC0) {
			if(ch>128) {
				set_utf8_error(1);	/* this is not a valid start for utf  */
				return 1;
			};
			if(ch<32) return clen;
		} else if(ch<0xE0) {
			if(o+1<=size){
				ch1=utfstr[o+1];
				if(ch1<128 || ch1>0xBF) { set_utf8_error(2);return 1;};	/* not a middle utf char  */
				clen=2;
				if((ch==0xCC||ch==0xCD) && !utf8_error() /* && drv_type<3 */ ) 
				{
					utf_accent=1;
					// MESG("allone accent! clen=%d");
					return clen;	/* return without checking for next accent  */
				};
			} else {
				set_utf8_error(3);	/* incomplete, eof  */
				return clen;
			};
		}
		else if(ch<0xF0) {
			ch1=utfstr[o+1];
			if(ch1<128 || ch1>0xBF) { set_utf8_error(4);return 1;};	/* not a middle utf char  */
			ch1=utfstr[o+2];
			if(ch1<128 || ch1>0xBF) { set_utf8_error(5);return 1;};	/* not a middle utf char  */
			clen=3;
		} else {
			char ch2,ch3;
			ch1=utfstr[o+1];
			if(ch1<128 || ch1>0xBF) { set_utf8_error(6);return 1;};	/* not a middle utf char  */
			ch2=utfstr[o+2];
			if(ch2<128 || ch2>0xBF) { set_utf8_error(7);return 1;};	/* not a middle utf char  */
			ch3=utfstr[o+3];
			if(ch3<128 || ch3>0xBF) { set_utf8_error(8);return 1;};	/* not a middle utf char  */
			clen=4;
		}
		if(clen<3 && !utf_accent ) {	/* check next char for accent!  */
		{
			// MESG("- s=[%s] o=%d",utfstr+o,o);
			clen += s_is_utf_accent(utfstr,o+clen);
			// if(clen>2) MESG("[%s][%s] total size=%d at %ld",utfstr,utfstr+o,clen,o);
		}};
	return clen;
}

int s_is_utf_accent(char *utfstr, int o)
{
 int ch,ch1;
 int size=strlen(utfstr);
 ch=utfstr[o];

 if(ch==0) return 0;
 ch1=utfstr[o+1];
 if(ch==0) return 0;
 // MESG("check accent at %d size=%d ch=%X ch1=%X",o,size,ch,ch1);
 if(((ch==0xCC || ch==0xCD) && (ch1<0xB0 && ch1>0x7F))
// 	|| (ch==0xCD && (ch1<0xB0))
 ){	// check for double accent
	if(o+3>size)	return 2;
	ch=utfstr[o+2];
	ch1=utfstr[o+3];
	if(((ch==0xCC || ch==0xCD) && (ch1<0xB0 && ch1>0x7F))
	){
		return 4;
	} else {
		return 2;
	};
 } else return 0;
}

/* put to uc the utf value, advance by the length, return new offset */
int  SUtfCharAt(char *utfstr, int offset, utfchar *uc)
{
 int i,ulen;
 int o=offset;
	memset(uc->uval,0,12);
	// ulen=utf8_countBytes[(int)utfstr[offset]];
	ulen=SUtfCharLen(utfstr,offset,uc);
	// MESG(" SUtfcharAt: [%s] o=%d ulen=%d",utfstr,o,ulen);
	for(i=0;i<ulen;i++) uc->uval[i]=utfstr[o+i];
	return o+ulen;
}

// The utf string must not be constant!!!!!
void utf_string_break(char *utf_string, int column)
{
	 char *s=utf_string;
	// MESG("utf_string_break: [%s] column=%d",utf_string,column);
	 char *end_string = utf_string+strlen(utf_string);
	 int i=0;

	 while(s < end_string) {
	 	utfchar uc;
		if(i > column) { 
			*s=0;
			// MESG("break at %d [%s]",s-utf_string,utf_string);
			return;
		};
#if	0
		i = SUtfCharAt(s,0,&uc);
#else
		SUtfCharAt(s,0,&uc);
		i += get_utf_length(&uc);
#endif
		s += utf8_countBytes[(int)*s];
	 };	
}

/* Reverse truncate to specic length */
char * utf8_rtruncate(char *utf_string, int len)
{
	int pos=strlen(utf_string);
	while(len-- >0) {
		if(pos<=0) break;
		pos = utf8_str_left(utf_string,pos);
	};
	// MESG("rtruncate: [%s] size=%d slen=%d pos=%d",utf_string,size,slen,pos);
	// MESG("         : [%s]",utf_string+pos);
	return utf_string+pos;
}

/* position of previous utf char.
   Does not take into account composed accented character!  
*/
int utf8_str_left(char *st,int pos)
{
 int i;
 int ch;
 int err=0;
 i=pos-1;

 if(st[i]<128) return i;
 while((ch=st[i])<192) {
 	i--;
 	if((ch=st[i])<128) err=1; 
 };
 	/* check  */
 if(utf8_countBytes[ch] != pos-i) err=2;
 if(err) MESG("utf8_str_left:err=%d [%s] w=%d pos=%d i=%d",err,st,utf8_countBytes[ch],pos,i);
 return i;
}

#if	NUSE
/* return string,len  if utf-8 valid else NULL  */
char *utf_check(char *utf, gsize *utf_len)
{
 int len=strlen(utf);
 int stat=0,i;
 int w=0;
 int ch;
 int error=0;
 int utflen=0;

 set_utf8_error(0);
 for(i=0;i<len;i++) {
 	ch=utf[i];
	if(ch<128) {utflen++;continue;};
	// utf or local string! 
	if(ch<192) {
		w--;
		if(w<0) { 
			w=0;error++;
		};
		continue;
	};
	if(ch<224) { 
		if(w>0) { 
			error+=2;
		};
		w=1;
		utflen++;
		if(!stat)stat=1;
		continue;
	};
	if(ch<240) { if(w>0) {error+=3;};
		w=2;
		utflen++;
		if(stat<2) stat=2;
		continue;
	};
	if(w>0){ error+=4;};
	utflen++;
	if(stat<3) stat=3;
	w=3;
 };
 *utf_len=utflen;
 if(error) {
	*utf_len=len;
	set_utf8_error(error);

	return NULL;
 } else {
	return utf;
 };
}
#endif

// no check previous utf8 char
unsigned char *prev_utf8_char(unsigned char *string,int pos)
{
	if(pos==0) return string;
	pos--;
	// printf("[%s] len=%d last = %X\n",string,pos+1,string[pos]);
	if(string[pos]<128) return string+pos;
	pos--;
// 	printf("%2X %2X\n",string[pos],string[pos+1]);
	if(string[pos]<0xE0 && string[pos]>=0xC0) return string+pos;
	pos--;
	if(string[pos]<0xF0 && string[pos]>=0xE0) return string+pos;
	pos--;
	return string+pos;
}

// utf8 character len no check!!
int utf8_len(char *str)
{
 unsigned char ch;
 if(str==NULL) return 0;
 ch=str[0];
 if(ch==0) return 0;
 if(ch<128) return 1;
 if(ch<0xE0) return 2;
 if(ch<0xF0) return 3;
 return 4;
}


int utf8_ord(char *str)
{
 unsigned char ch1,ch2,ch3,ch4;
 ch1=str[0];
 if(ch1<128) return (int)ch1;
 ch2=str[1];
 if(ch1<0xE0) return ((ch1-0xC0)*0x40+ch2%0x40);
 ch3=str[2];
 if(ch1<0xF0) return (ch1-0xE0)*0x1000+(ch2-0x80)*64+ch3%0x40;
 ch4=str[3];
 return (ch1-0xF0)*64*0x1000+(ch2-0x80)*0x1000+(ch3-0x80)*64+ch4%0x40;
}

#endif

/* --- */
