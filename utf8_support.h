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
	unsigned char uval[16];
} utfchar;

int error_line(char *error_message,...);
int utf_num_chars(char *utf);
int  SUtfCharAt(char *utfstr, int offset, utfchar *uc);
int utf8_countbytes(int c);
void utf_string_break(char *utf_string, int column);
int utf8_str_left(char *st,int pos);
int utf8_error();
void set_utf8_error(int error);
unsigned char *prev_utf8_char(unsigned char *string,int pos);
int utf8_len(char *str);
int utf8_ord(char *str);
char * utf_string_rtruncate(char *utf_string, int len);
int get_utf_length(utfchar *utf_char_str);
int SUtfCharLen(char *utfstr,int offset,utfchar *uc);
int s_is_utf_accent(char *utfstr, int o);
