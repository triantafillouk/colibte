/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.
*/

/* Some general file support functions */

#include "config.h"	// application specific configuration flags and dirs
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>

#include "support.h"
#include "alist.h"

int utf_num_chars(char *utf);
void msg_line(char *);
char * utf8_rtruncate(char *utf_string, int len);
int	macbug = 0;		/* macro debuging flag		*/

extern int xwin;
extern int discmd;

// Application starting dir
char start_dir_val[MAXFLEN] = ".";
char current_working_dir[MAXFLEN];

int set_debug(long long n)
{
	macbug=n;
	return true;
}

int debug_flag()
{
	return macbug;
}

void increase_debug_flag()
{
	macbug++;
}

void decrease_debug_flag()
{
	macbug--;
	if(macbug<0) macbug=0;
}

char *get_start_dir()
{
 return start_dir_val;
}

void set_start_dir(char *start)
{
 char *sd;
 if(start==NULL) {
	if((sd=getcwd(start_dir_val,MAXFLEN))==NULL) {ERROR("cannot get cwd!");};
//	MESG("set_start_dir: [%s]",start_dir_val);
 }
 else 
 strlcpy(start_dir_val,start,MAXFLEN);
 // MESG("set_start_dir: to [%s]",start_dir_val);
}

void set_working_dir(char *dir)
{
	strlcpy(current_working_dir,dir,MAXFLEN);
//	MESG("set_working_dir:[%s]",dir);
}

char *get_working_dir()
{
	if(strlen(current_working_dir)==0) {
		char *sd;
//		MESG("get_working_dir: null, try set it to start dir!");
		sd = start_dir_val;
		if(sd==NULL) {
			ERROR("cannot get statr dir");
			set_working_dir(getenv("HOME"));
		} else 
		set_working_dir(sd);
	};
	
	return &current_working_dir[0];
}

#define	TEST3	1

/* 
 remove the directory name from the file name 
 base name buffer must already be exist.
 No check for buffer maximum length!!
*/
void create_base_name(char *base_name,char *full_name)
{
        register char *cp1;
        register char *cp2;
        cp1 = full_name;
        while (*cp1 != 0) ++cp1; // goto the end

        while (cp1!=full_name && cp1[-1]!='/')  --cp1;

        cp2 = base_name;
        while (cp2!= (base_name+MAXFLEN-1) && *cp1!=0 && *cp1!=';')
                *cp2++ = *cp1++;
        *cp2 = 0;
		// MESG("create_base_name: [%s] full name [%s]",base_name,full_name);
}

/* return a pointer of the base name from a full file name */
char *get_base_name(char *full_name)
{
	register char *cp1;
	if(full_name==NULL) return NULL;
	cp1 = full_name;
	while (*cp1 != 0) ++cp1; // goto the end
	while (cp1!=full_name && cp1[-1]!='/')  --cp1;
	return (cp1);
}

/* 	creates the dir name from a full file name 
	new area is allocated for the string
*/
char *get_dir_name(char *full_name)
{
	register char *cp1;
	char *dir_name;
	int len;
	cp1 = full_name;
	while (*cp1 != 0) ++cp1; // goto the end
	while (cp1!=full_name && cp1[-1]!='/')  --cp1;
	len=cp1-full_name;
	dir_name=(char *)malloc(len);
	strlcpy(dir_name,full_name,len);
	dir_name[len-1]=0;

	return(dir_name);
}

int file_exist(char *fname)
{
 int status;
 struct stat s;
 status = stat(fname, &s);

 if (status == 0 && S_ISREG (s.st_mode)) return (TRUE);
 return FALSE;
}

char *find_file(char *subdir, char *fname, int check_start_dir, int create_if_not_found)
{
	char *path;	/* environmental PATH variable */
	char *sp;	/* pointer into path spec */
	static char fspec[MAXFLEN];	/* full path spec to search */
	int slen=0;
	// MESG("find_file:fname=[%s] in subdir=[%s] check_start_dir=%d [%s]",fname,subdir,check_start_dir,start_dir_val);	
	if(check_start_dir) { // check application's start dir
		if(subdir[0]!=0) slen=snprintf(fspec,sizeof(fspec),"%s/%s/%s",start_dir_val,subdir,fname);
		else slen=snprintf(fspec,sizeof(fspec),"%s/%s",get_start_dir(),fname);
		if(slen>MAXFLEN) { MESG("truncated!");return NULL;};
		if(file_exist(fspec)) {
			// MESG("	found [%s]",fspec);
			return(fspec);
		};
	}; 
	
	// check the home dir under app dir
	if(subdir[0]!=0) snprintf(fspec,sizeof(fspec),"%s/%s/%s/%s",getenv("HOME"),APPLICATION_DOT_DIR,subdir,fname);
	else snprintf(fspec,sizeof(fspec),"%s/%s/%s",getenv("HOME"),APPLICATION_DOT_DIR,fname);
	if(file_exist(fspec)) {
		// MESG("	found [%s]",fspec);
		return(fspec);
	};

	// check the home dir
	if(subdir[0]!=0) snprintf(fspec,sizeof(fspec),"%s/%s/%s",getenv("HOME"),subdir,fname);
	else snprintf(fspec,sizeof(fspec),"%s/%s",getenv("HOME"),fname);
	if(file_exist(fspec)) {
		// MESG("	found [%s]",fspec);
		return(fspec);
	};

	snprintf(fspec,sizeof(fspec),"%s/%s/%s",APPLICATION_DIR,subdir,fname);

	if(file_exist(fspec)) {
		// MESG("	found [%s]",fspec);
		return(fspec);
	};

	/* check under the paths of the CMDPATH variable */
	path = (char *)getenv(APP_PATH);
	if (path != NULL)
		while (*path) {

			/* build next possible file spec */
			sp = fspec;
			while (*path && (*path != PATHCHR))
				*sp++ = *path++;
			*sp++ = PATHCHR; *sp = 0;
			strlcat(fspec,subdir,MAXFLEN);
			strlcat(fspec,DIRSEPSTR,MAXFLEN);
			strlcat(fspec, fname,MAXFLEN);
			/* and try it out */
			if(file_exist(fspec)) {
				// MESG("	found [%s]",fspec);
				return(fspec);
			};
			if (*path == PATHCHR) ++path;
		};
	if(create_if_not_found) {	/* Create it in . home dir  */
		if(subdir) 	slen=snprintf(fspec,sizeof(fspec),"%s/%s/%s/%s",getenv("HOME"),APPLICATION_DOT_DIR,subdir,fname);
		else slen=snprintf(fspec,sizeof(fspec),"%s/%s/%s",getenv("HOME"),APPLICATION_DOT_DIR,fname);
		// MESG("create new file [%s]",fspec);
		return(fspec);
	} else
	MESG("file [%s] not found!",fname);
	return(NULL);
}

void msg_log(int priority,const char *fmt, ...)
{
 va_list args;
    if (fmt != NULL) {
		va_start(args,fmt);
		vsyslog(priority,fmt,args);
		va_end(args);
    }
}

void open_log(char *program_name)
{
 openlog(program_name,LOG_PID,17);	/* local 1  */
}

void ERROR(const char *fmt, ...)
{
 va_list args;
 static char mline[512];
    if (fmt != NULL) {
		va_start(args,fmt);
		vsnprintf(mline,sizeof(mline),fmt,args);
		va_end(args);
		msg_log(1,mline);
		msg_line(mline);
		// if(debug_flag()) fprintf(stderr,"Error: %s\n",mline);
    }
}

// system errors
void SYS_ERROR(const char *fmt, ...)
{
 va_list args;
 static char mline[512];
    if (fmt != NULL) {
		va_start(args,fmt);
		vsnprintf(mline,sizeof(mline),fmt,args);
		va_end(args);

		if(errno) {
			char *s=strerror(errno);
			msg_log(1,mline);
			msg_log(1,s);
			msg_line(mline);
			if(debug_flag()) perror("NOTOK!!");
			errno=0;
		} else {
			msg_log(1,mline);
			if(debug_flag()) fprintf(stderr,"Error: %s",mline);
		};
    }
}

#if	!defined _MESG_DEFINED
void MESG(const char *fmt, ...)
{
 va_list args;
 static char mline[512];
 if(!debug_flag()) return;
 // printf("mesg:\n");
    if (fmt != NULL) {
		va_start(args,fmt);
		vsnprintf(mline,sizeof(mline),fmt,args);
		va_end(args);

		if(mline[0]!=0) 	/*   */
		{
			msg_log(1,"%s",mline);	// send to system log
			fprintf(stderr,"%s\n",mline);
			fflush(stderr);
		};
    }
}


# define _MESG_DEFINED
# endif

char **read_sarray(char *file_name,int *size)
{
 char **array=NULL;
   char *buffer = NULL;
   int string_size, read_size;
   FILE *handler = fopen(file_name, "r");
	int lines=0;
	// MESG("read_sarray:");
   if (handler)
   {
       // Seek the last byte of the file
       fseek(handler, 0, SEEK_END);
       // Offset from the first to the last byte, or in other words, filesize
       string_size = ftell(handler);
       // go back to the start of the file
       rewind(handler);

       // Allocate a string that can hold it all
       buffer = (char*) malloc(sizeof(char) * (string_size + 1) );
	   // MESG("read_sarray: size=%d",string_size);
	   if(buffer){
		int i;
		int l=0;
		read_size = fread(buffer, sizeof(char), string_size, handler);
		if(read_size != string_size) { MESG("cannot correclty read the array");};
	    fclose(handler);
		
		for(i=0;i<read_size;i++){
			if(buffer[i]=='\r'){
				lines++;
				buffer[i]=0;
				if(buffer[i+1]=='\n') { 
					buffer[i+1]=0;
				};
				continue;
			};
			if(buffer[i]=='\n'){
				lines++;
				buffer[i]=0;
			};
		};
		// MESG("read_sarray: lines=%d",lines);
		array=malloc((lines+1)*sizeof(char *));
		for(l=0,i=0;l<lines;l++){
			array[l]=buffer+i;
			// MESG("%3d [%s] pos %d",l,array[l],i);
			while(i<read_size && buffer[i]>0) i++;
			while(i<read_size && buffer[i]==0) i++;
		};
		array[l]=NULL;
		*size=lines;
	   } else {
	    fclose(handler);
	   	return NULL;
	   }
    }
 return array;
}


char **split_2_sarray(char *in_string,int split_chr)
{
 char **array=NULL;
   char *buffer = NULL;
   int string_size;
   int count=0;
   string_size=strlen(in_string);
	if(string_size==0) return NULL;

   // Allocate a string that can hold it all
   char *start=in_string;
   while(*start==' '||*start==9) start++;	/* trim starting spaces */
   char *end=in_string+string_size-1;
   while(*end==' '||*end==9) *end--=0;		/* trim ending spaces  */
   buffer = strdup(start);
   // MESG("split_2_sarray [%s]",start);
   if(buffer){
	int i;
	int l;
	for(i=0,l=0;i<string_size;i++){
#if	1
		if(split_chr!=' ')
		if(buffer[i]==' ') { 
			// MESG("	space l=%d i=%d",l,i);
			// buffer[i]='_';
			continue;
		}; 	/* trim starting spaces  */
#endif
		if(buffer[i]==split_chr){
			count++;
			buffer[i]=0;
		}
	};
	// buffer[i]=0;
	// MESG("	%d components",count+1);
	array=malloc((count+2)*sizeof(char *));
	for(l=0,i=0;l<count+1;l++){
		array[l]=buffer+i;
		// MESG("	pos=%d array[%d]=[%s]",i,l,array[l]);
		while(buffer[i]>0) i++;
		i++;
	};
	array[l]=NULL;
   } else {
   	return NULL;
   }
 return array;
}

int read_list_array(char *file_name, alist *new_list)
{
   char *buffer = NULL;
   int string_size, read_size;
   FILE *handler = fopen(file_name, "r");
	char *start_line;
	char *temporary=NULL;
	// int lines=0;
	int start_point=0;
	empty_list(new_list);
   if (handler)
   {
       // Seek the last byte of the file
       fseek(handler, 0, SEEK_END);
       // Offset from the first to the last byte, or in other words, filesize
       string_size = ftell(handler);
       // go back to the start of the file
       rewind(handler);
       // Allocate a string that can hold it all
       buffer = (char*) malloc(sizeof(char) * (string_size + 1) );
	   if(buffer){
		int i;
		read_size = fread(buffer, sizeof(char), string_size, handler);
		if(read_size != string_size) { MESG("cannot correctly read the array");};
	    fclose(handler);
		start_line=buffer;
		start_point=0;
		for(i=0;i<read_size;i++){
			if(buffer[i]=='\r'){	/* msdos line type  */
				// lines++;
				buffer[i]=0;
				if(buffer[i+1]=='\n') { 
					buffer[i+1]=0;
					i++;
				};
				temporary=malloc(i-start_point+2);
				memcpy(temporary,start_line,i-start_point+1);
				add_element_to_list(temporary,new_list);
				start_line=buffer+i;
				start_point=i;
			};
			if(buffer[i]=='\n'){	/* unix line type  */
				// lines++;
				buffer[i]=0;
				temporary=malloc(i-start_point+2);
				memcpy(temporary,start_line,i-start_point+1);
				add_element_to_list(temporary,new_list);
				start_line=buffer+i+1;
				start_point=i;
			};
		};
		
	    free(buffer);
		return 1;
	   } else {
	    fclose(handler);
	   	return 0;
	   }
    }
 return 0;
}


int save_list_array(char *file_name, alist *array)
{
 FILE *handler=NULL;
 // int lines=0;
 char *line;
 int slen;
	if(array==NULL) { 
		ERROR("Null array");
		return 0;
	};
	handler = fopen(file_name, "w");
	if(handler){
		lbegin(array);
		while((line=(char *)lget(array))!=NULL) {
			// lines++;
			slen=strlen(line);
			fwrite(line,slen,1,handler);
			fwrite("\n",1,1,handler);	/* fix for DOS ending!  */
		}
		fclose(handler);
		return 1;
	} 
	else { 
		return 0;
	};
}

void free_sarray(char **sarray)
{
	char *buffer = sarray[0];
	free(buffer);	/* free contents  */
	free(sarray);	/* free pointer array  */
}


#if !DARWIN
size_t strlcat(char *dst, const char *src, size_t size)
{
  size_t    srclen;         /* Length of source string */
  size_t    dstlen;         /* Length of destination string */

  dstlen = strlen(dst);
  size   -= dstlen + 1;

  if (size<2) return (dstlen);        /* No room, return immediately... */

  srclen = strlen(src);

  if (srclen > size) srclen = size;

  memcpy(dst + dstlen, src, srclen);
  dst[dstlen + srclen] = '\0';

  return (dstlen + srclen);
}

/* '_cups_strlcpy()' - Safely copy two strings.   */

size_t strlcpy(char *dst, const char *src, size_t size)
{
  size_t    srclen;         /* Length of source string */

  size --;
  srclen = strlen(src);
  if (srclen > size)  srclen = size;
  memcpy(dst, src, srclen);
  dst[srclen] = '\0';

  return (srclen);
}
#endif

/* -- */
