/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

*/
/* some generic utility functions */

#ifndef _UTILS_
#define	_UTILS_
double  show_time(char *description,int init);
int vfind(char *s,char *m[],int nu);
int read_pairs(char *fname,char delimiter,char ***name,char ***value);
void sarray_clear(char **array); /* free a string array */
void sarray_clear_data(char **array);
void revstr(char *st); /* reverse a string */
int sarray_index(char **sarray,char *st); /* get the index of an element in a string array */
void add2_sarray(char **array, char *str);
void remove_from_sarray(char **array, char *str);
void set_full_name(char *full_name,char *dir_name,char *file_name,int len);
int lstartwith(char *line,char c);
void string_2_nospaces(char *string);

// integer array utils (contain 0,positive integers)
int iarray_index(int *iarray,int i_to_find, int max_element);
int iarray_remove(int *iarray, int start, int max_element);
int iarray_add(int *iarray, int value, int max_element);

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#endif
