/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

*/
#if	USE_GLIB
#include <glib.h>
#endif

typedef struct utfchar {
	unsigned char uval[12];
} utfchar;

int utf_num_chars(char *utf);
int  SUtfCharAt(char *utfstr, int offset, utfchar *uc);
int utf8_countbytes(int c);
void utf_string_break(char *utf_string, int column);
int utf8_str_left(char *st,int pos);
int utf8_error();
void set_utf8_error(int error);
#if	NUSE
char *utf_check(char *utf, gsize *utf_len);
#endif
unsigned char *prev_utf8_char(unsigned char *string,int pos);
int utf8_len(char *str);
int utf8_ord(char *str);
char * utf_string_rtruncate(char *utf_string, int len);
