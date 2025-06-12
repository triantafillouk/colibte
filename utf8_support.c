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
#include <uchar.h>

//#define __USE_XOPEN
#include <wchar.h>

#include "support.h"

static int utf_error=0;

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
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, // 0x80
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, // 0x90
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, // 0xA0
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, // 0xB0
	-2,-2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,	// 0xC0
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,	// 0xD0
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,	// 0xE0
	4,4,4,4,4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4	// 0xF0
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

#if	NUSE
int utf_num_chars_tnl(char *utf,long *size)
{
 int len=strlen(utf);
 int i,j;
 int trail=0;
 int ch;
 int utflen=0;
 char *eol=memchr(utf,10,len); // up to newline
 if(eol) {
 	*size=eol-utf;
	// if(eol[1]==13) size++;
	len = *size;
 } else {
 	*size = len;
 };

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
 if(utf[*size]==10) (*size)++;
 if(utf[*size]==13) (*size)++;
 return utflen;
}

void count_strsize(char *utf,long *maxchars,long *height)
{
 *maxchars=0;
 *height=0;
 long start=0;
 long ssize=strlen(utf);
 long chars_in_line=0;
 while(start<ssize) {
 	long line_len=0;
 	chars_in_line=utf_num_chars_tnl(utf+start,&line_len);
	if(chars_in_line>*maxchars) *maxchars=chars_in_line;
	start += line_len;
	(*height)++;
 };
}
#endif
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
			if(ch<32) return 1;
		} else if(ch<0xE0) {
			if(o+1<=size){
				ch1=utfstr[o+1];
				if(ch1<128 || ch1>0xBF) { set_utf8_error(2);return 1;};	/* not a middle utf char  */
				clen=2;
				if((ch==0xCC||ch==0xCD) && !utf8_error() /* && drv_type<3 */ ) 
				{
					utf_accent=1;
					// MESG("allone accent! clen=%d");
					return 2;	/* return without checking for next accent  */
				};
			} else {
				set_utf8_error(3);	/* incomplete, eof  */
				return 1;
			};
		}
		else if(ch<0xF0) {
			ch1=utfstr[o+1];
			if(ch1<128 || ch1>0xBF) { set_utf8_error(4);return 1;};	/* not a middle utf char  */
			ch1=utfstr[o+2];
			if(ch1<128 || ch1>0xBF) { set_utf8_error(5);return 1;};	/* not a middle utf char  */
			return 3;
		} else {
			char ch2,ch3;
			ch1=utfstr[o+1];
			if(ch1<128 || ch1>0xBF) { set_utf8_error(6);return 1;};	/* not a middle utf char  */
			ch2=utfstr[o+2];
			if(ch2<128 || ch2>0xBF) { set_utf8_error(7);return 1;};	/* not a middle utf char  */
			ch3=utfstr[o+3];
			if(ch3<128 || ch3>0xBF) { set_utf8_error(8);return 1;};	/* not a middle utf char  */
			return 4;
		}
		if(clen<3 && !utf_accent ) {	/* check next char for accent!  */
			// MESG("- s=[%s] o=%d",utfstr+o,o);
			clen += s_is_utf_accent(utfstr,o+clen);
		} else {
			// MESG("clen=%d",clen);
		};
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
 if(ch==0xEF && ch1==0xB8) return 3;
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
	if(ulen>12) { error_line(" SUtfcharAt: [%s] o=%d ulen=%d",utfstr,o,ulen);ulen=12;};
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

// this is like utf8_to_wchar but for 4 bytes and no error handlind!
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

// Returns the unicode point
wchar_t utf8_to_wchar(unsigned char* const utf8_str, int *size) 
{
	//First, grab the first byte of the UTF-8 string
	unsigned char* utf8_currentCodeUnit = utf8_str;
	wchar_t unic=0;
	// int utf8_str_iterator = 0;

		//Figure out the current code unit to determine the range. It is split into 6 main groups, each of which handles the data
		//differently from one another.
		if (*utf8_currentCodeUnit < 0x80) {
			//0..127, the ASCII range.
			unic = *utf8_currentCodeUnit;
			*size=1;
			return unic;
		}
		else if (*utf8_currentCodeUnit < 0xC0) {
			unic = 0;
			//0x80..0xBF, we ignore. These are reserved for UTF-8 encoding.
			// printf("error: reserved for UTF-8 encoding 0x%X\n",*utf8_currentCodeUnit);
			*size=0;
			utf_error=1;
			return unic;	/* this is an error!  */
		}
		else if (*utf8_currentCodeUnit < 0xE0) {
			//128..2047, the extended ASCII range, and into the Basic Multilingual Plane.

			//Work on the first code unit.
			char16_t highShort = (char16_t) ((*utf8_currentCodeUnit) & 0x1F);

			//Increment the current code unit pointer to the next code unit
			utf8_currentCodeUnit++;

			//Work on the second code unit.
			char16_t lowShort = (char16_t) ((*utf8_currentCodeUnit) & 0x3F);

			//Increment the current code unit pointer to the next code unit
			utf8_currentCodeUnit++;

			//Create the UTF-16 code unit, then increment the iterator.
			//Credits to @tbeu. 
			//Thanks to @k6l2 for explaining why we need 6 instead of 8.
			//It's because 0x3F is 6 bits of information from the low short. By shifting 8 bits, you are 
			//adding 2 extra zeroes in between the actual data of both shorts.
			int unicode = (highShort << 6) | lowShort;

			//Check to make sure the "unicode" is in the range [0..D7FF] and [E000..FFFF].
			*size=2;
			if ((0 <= unicode && unicode <= 0xD7FF) || (0xE000 <= unicode && unicode <= 0xFFFF)) {
				unic = unicode;
				return unic;	/* 2 byte utf8  */
			}
			// printf("error 2 bytes 0x%X\n",unicode);
		}
		else if (*utf8_currentCodeUnit < 0xF0) {
			//2048..65535, the remaining Basic Multilingual Plane.

			//Work on the UTF-8 code units one by one.
			//If drawn out, it would be 1110aaaa 10bbbbcc 10ccdddd
			//Where a is 4th byte, b is 3rd byte, c is 2nd byte, and d is 1st byte.
			char16_t fourthChar = (char16_t) ((*utf8_currentCodeUnit) & 0xF);
			utf8_currentCodeUnit++;
			char16_t thirdChar = (char16_t) ((*utf8_currentCodeUnit) & 0x3C) >> 2;
			char16_t secondCharHigh = (char16_t) ((*utf8_currentCodeUnit) & 0x3);
			utf8_currentCodeUnit++;
			char16_t secondCharLow = (char16_t) ((*utf8_currentCodeUnit) & 0x30) >> 4;
			char16_t firstChar = (char16_t) ((*utf8_currentCodeUnit) & 0xF);
			utf8_currentCodeUnit++;

			//Create the resulting UTF-16 code unit, then increment the iterator.
			int unicode = (fourthChar << 12) | (thirdChar << 8) | (secondCharHigh << 6) | (secondCharLow << 4) | firstChar;

			//Check to make sure the "unicode" is in the range [0..D7FF] and [E000..FFFF].
			//According to math, UTF-8 encoded "unicode" should always fall within these two ranges.
			*size=3;
			if ((0 <= unicode && unicode <= 0xD7FF) || (0xE000 <= unicode && unicode <= 0xFFFF)) {
				unic = unicode;
				return unic;	/* 3 byte utf8  */
			};
			// printf("error bytes 3 0x%X\n",unicode);
		}
		else if (*utf8_currentCodeUnit < 0xF8) {
			//65536..10FFFF, the Unicode UTF range

			//Work on the UTF-8 code units one by one.
			//If drawn out, it would be 11110abb 10bbcccc 10ddddee 10eeffff
			//Where a is 6th byte, b is 5th byte, c is 4th byte, and so on.
			char16_t sixthChar = (char16_t) ((*utf8_currentCodeUnit) & 0x4) >> 2;
			char16_t fifthCharHigh = (char16_t) ((*utf8_currentCodeUnit) & 0x3);
			utf8_currentCodeUnit++;
			char16_t fifthCharLow = (char16_t) ((*utf8_currentCodeUnit) & 0x30) >> 4;
			char16_t fourthChar = (char16_t) ((*utf8_currentCodeUnit) & 0xF);
			utf8_currentCodeUnit++;
			char16_t thirdChar = (char16_t) ((*utf8_currentCodeUnit) & 0x3C) >> 2;
			char16_t secondCharHigh = (char16_t) ((*utf8_currentCodeUnit) & 0x3);
			utf8_currentCodeUnit++;
			char16_t secondCharLow = (char16_t) ((*utf8_currentCodeUnit) & 0x30) >> 4;
			char16_t firstChar = (char16_t) ((*utf8_currentCodeUnit) & 0xF);
			// utf8_currentCodeUnit++;

			unic = firstChar | ((secondCharLow+4*secondCharHigh)) << 4 | (thirdChar << 8) | (fourthChar <<12) | (fifthCharLow+fifthCharHigh*4) << 16 | sixthChar << 20 ; 
			// printf("unicode : %X\n",unicode);
			// printf("1 %08X\n",firstChar);
			// printf("2 %08X\n",((secondCharLow+4*secondCharHigh)) << 4);
			// printf("3 %08X\n",thirdChar << 8);
			// printf("4 %08X\n",fourthChar << 12);
			// printf("5 %08X\n",(fifthCharLow+fifthCharHigh*4) << 16);

			// unic = unicode;
			*size=4;
			// printf("	-> %X %X %X %X %X\n",fifthCharLow+fifthCharHigh*4,fourthChar,thirdChar,secondCharLow+4*secondCharHigh,firstChar);
			return unic;	/* 4 byte code  */
		}
		else {
			//Invalid UTF-8 code unit, we ignore.
			// printf("error 0x%X > 0xF8\n",*utf8_currentCodeUnit);
			utf_error=1;
		}
	/* error !!!  */
	return 0;
}

#endif

/* --- */
