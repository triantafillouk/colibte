/* 
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

	Some general file support functions 
*/

#ifndef _FSUPPORT_
#define _FSUPPORT_

//#include <glib.h>

/* low level errors */
#define	NOERROR	0	/* successfull operation */
#define	ERR_EOF	1	/* end of file           */
#define	ERR_FIO	2	/* file io error         */
#define	ERR_MEM	3	/* memory error          */

#define MAXBLEN	1024	/* max big string size */
#define MAXFLEN	1024	/* max file lenght */
#define MAXLLEN	1024	/* max line length */
#define	MAXPLEN 2048	/* max path name length  */
#define	MAXMLEN	256			/* maximum medium length */
#define	MAXSLEN	64			/* maximum small length */

#define	MAXSTRL	256		/* normal ?? string len */
#define MAXNLEN	512		/* max base name length */

#ifndef	TRUE
#define	TRUE	1
#define	FALSE	0
#endif

#define	PATHCHR	':'
#define DIRSEPSTR "/"


#if	NUSE
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define LIM(x,min,max) MAX(min,MIN(x,max))
#define ULIM(x,y,z) ((y) < (z) ? LIM(x,y,z) : LIM(x,z,y))
#define CLIP(x) LIM(x,0,65535)
#define SWAP(a,b) { a ^= b; a ^= (b ^= a); }
#endif

/* remove the directory name from the file name */
void create_base_name(char *base_name,char *full_name);
char *get_dir_name(char *full_name);

// base name (returns a pointer at the start of base name)
char *get_base_name(char *full_name);

/* find a file in a path */
char *find_file(char *subdir, char *fname, int check_start_dir,int create_if_not_found);

/* check if file exists */
int file_exist(char *fname);

/* print a time stamp, mainly for profiling, debugf must be true! */
double  show_time(char *description,int init);

char **split_2_sarray(char *as,int splitchr);

void MESG(const char *fmt, ...);
void MESG_time(const char *fmt, ...);
void ERROR(const char *fmt, ...);
void SYS_ERROR(const char *fmt, ...);
void msg_log(int priority,const char *fmt, ...);

#define IS_FLAG(x,u) ((x&u)==u)
#define IS_NOT_FLAG (x,u) ((x&u)==0)

#define MSG( LEVEL,format,...) { \
	if(debug_flag() > LEVEL && xwin) {\
		fprintf(stderr, format, ##__VA_ARGS__);\
    	fputc('\n', stderr);\
		MESG(format, ##__VA_ARGS__);\
	}\
}

char *get_start_dir();
void set_start_dir(char *);

#if	!DARWIN
/*
 * '_cups_strlcat()' - Safely concatenate two strings.
 */
size_t strlcat(char *dst,  const char *src, size_t size);
/*
 * '_cups_strlcpy()' - Safely copy two strings.
 */
size_t strlcpy(char *dst, const char *src, size_t size);

#endif

#endif
