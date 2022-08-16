/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

*/
/* various independent utility functions */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include "utils.h"
#include "support.h"

int scmp(const char* s, const char* t);

extern int discmd;

/* list of two elements  */
typedef struct delement 
{
 struct delement *next_pair;
 char *pair_name;
 char *pair_value;
} delement;

typedef struct dlist 
{
  int num;
  delement *first;
  delement *last;
  delement *current;
} dlist;


dlist * new_dlist()
{
 dlist *sl1;
 delement *e1;
 sl1=(dlist *)malloc(sizeof(struct dlist));
 e1=(delement *)malloc(sizeof(struct delement));
 sl1->num=0;
 e1->next_pair=NULL;
 e1->pair_name=NULL;
 e1->pair_value=NULL;
 sl1->first=e1;
 sl1->last=e1;
 return sl1;
}


void free_dlist(dlist *sl1,int fclear)
{
 delement *e1,*e2;
 e1=sl1->first;
 while(e1!=NULL) {
 	e2=e1->next_pair;
	if(fclear) { free(e1->pair_value);free(e1->pair_name);};
 	free(e1);
	e1=e2;
 };
 free(sl1);
}

void add_dlist_element(dlist *sl1, char *name, char *value)
{
 delement *e1;
 e1=(delement *)malloc(sizeof(struct delement));
 e1->next_pair=NULL;
 e1->pair_value=NULL;
 e1->pair_name=NULL;
 sl1->last->pair_value=value;
 sl1->last->pair_name=name;
 sl1->last->next_pair=e1;
 sl1->last=e1;
 sl1->num ++;
}

extern int xwin;

void out_print(char *s,int nl);

/* print a time stamp, mainly for profiling, debugf must be true! */
	/* init=0 initializes  */
 	/* init=1 returns diff from previous one  */
 	/* init=2 returns diff from initialization */
double  show_time(char *description,int init)
{ 
	static int start_sec=0;
	static int start_usec=0;
	static int prev_sec=0;
	static int prev_usec=0;
  	int	usec1,sec1=0,diff_sec,diff_usec,diff_start_sec,diff_start_usec;
	double vtime;
	struct timeval      timev;
	char sout[512];
	diff_sec=diff_usec=0;

	gettimeofday(&timev, NULL);
	sec1 = (int)timev.tv_sec;
	usec1 = (int)timev.tv_usec;
	if(init==0) {
		start_sec=sec1;start_usec=usec1;prev_sec=0;prev_usec=0;
		diff_start_sec=diff_start_usec=0;
	} else {
		diff_start_sec=sec1-start_sec;
		diff_start_usec=usec1-start_usec;
		if(diff_start_usec<0) {
			diff_start_usec+=1000000;
			diff_start_sec--;
		};
		diff_sec=sec1-prev_sec;
		diff_usec=usec1-prev_usec;
		if(diff_usec<0) {
			diff_usec+=1000000;
			diff_sec--;
		}
	};

	if(1) 
	{
	if(init==0) snprintf(sout,512,"[%35s] %d,%06d  %d,%06d --------",description,diff_start_sec,diff_start_usec,diff_sec,diff_usec);
	else snprintf(sout,512,"[%35s] %d,%06d  %d,%06d",description,diff_start_sec,diff_start_usec,diff_sec,diff_usec);
	if(xwin && discmd) MESG("%s",sout);
	out_print(sout,1);
	};

	prev_usec=usec1;
	prev_sec=sec1;
	if(init==1) {	
		vtime=diff_sec+diff_usec*0.000001;
	} else {
		vtime=diff_start_sec+diff_start_usec*0.000001;
	}
	return vtime;
}

/* find an a/a from a string array */
int vfind(char *s,char *m[],int nu)
{
 int i;
// mkupper(s);
 for(i=0;i<nu;i++)	
	if(scmp(m[i],s) == 0) { return(i);};
 return(-1);
}

/* read a file with pairs (name, value) */
int read_pairs(char *fname,char delimiter,char ***name,char ***value)
{
 dlist *sl1;
 delement *e1;
 FILE *f;
 char s[1000];
 char *svalue;
 char *cp1,*cp2;
 char *sname;
 char **s1,**s2;
 int i;
// initialize the list
 sl1=new_dlist();
 f=fopen(fname,"r");
 if(!f) { name=NULL; value=NULL;return 0;};
 
 while(fgets(s,1000,f)) {
	int delimit_ok=0;
 	s[strlen(s)-1]=0; // remove newline
	if(strlen(s)==0) continue;
	if(s[0]=='#' || s[0]==';'||s[0]=='[') continue;
	cp1=s;
	cp2=NULL;
	while( *cp1++) {
		if(*cp1==delimiter) {
			delimit_ok=1;
			*cp1=0;
			cp2=cp1+1;
			break;
		};
	};
	if(!delimit_ok) continue;	/* this is not pair!  */
	svalue=strdup(cp2);
	sname=strdup(s);
 	add_dlist_element(sl1, sname,svalue);
 };
 fclose(f);
 s1=(char **)malloc((sl1->num+1)*sizeof(char *));
 s2=(char **)malloc((sl1->num+1)*sizeof(char *));
 e1=sl1->first;
 for(i=0;i<sl1->num;i++) {
 	s1[i]=e1->pair_name;
 	s2[i]=e1->pair_value;
	e1=e1->next_pair;
 };
 s1[i]=s2[i]=NULL; /* last element always null */
 free_dlist(sl1,0);
 *name=s1;
 *value=s2;
 return(i);
}

#if	NUSE
int write_pairs(char *fname,char *title, char *delimiter,char **name_array,char **value_array)
{
 FILE *f;
 char *name;
 char *value;
 char s[MAXLLEN];
 f=fopen(fname,"w");
 if(f) {
 	sprintf(s,"[%s]\n",title);
	fwrite(s,strlen(s),1,f);
	name=name_array[0];
	value=value_array[0];
	while(*name) {
		sprintf(s,"%s%s%s\n",name,delimiter,value);
		fwrite(s,strlen(s),1,f);
		name++;value++;
	};
 	fclose(f);
	return TRUE;
 };
 return FALSE;
}
#endif

/* clear string array data */
void sarray_clear_data(char **array)
{
// MESG("sarray_clear_data:");
 if(array!=NULL){
 	int i;
	for(i=0;array[i]!=NULL;i++){
		if(array[i]) free(array[i]);
		array[i]=NULL;
	};
 };
}

/* free a string array */
void sarray_clear(char **array)
{
 if(array!=NULL){
 	int i;
	for(i=0;array[i]!=NULL;i++) {
		if(array[i]!=NULL) free(array[i]);
	};
	free(array);
 };
}

/* Add a string to string array, must have space!  
	string must be abled to be freed!
*/
void add2_sarray(char **array, char *str)
{
 int i;
 if(array!=NULL){
	// goto end!
	for(i=0;array[i]!=NULL;i++){
//		if(array[i][0]==0) { free(array[i]);break;};
	};
//	MESG("add2_sarray:[%s] at position %d",str,i);
	array[i++]=str;
	array[i]=NULL;
 } else {
 	MESG("array to add [%s] is null!!!",str);
 };
}

void remove_from_sarray(char **array, char *str)
{
 int ind;
// MESG("remove_from_sarray:[%s]",str);
 ind = sarray_index(array,str);
 if(ind >=0) {
 	free(array[ind]);
	while(1) {
		array[ind]=array[ind+1];
		ind++;
		if(array[ind]==NULL) break; 
	};
 } else {
 	MESG("remove_from_sarray: string not in array!");
 };
}


/* reverse a string */
void revstr(char *st)
{
 char *ts, *ts1, c;
 ts=st;
 ts1=st+strlen(st)-1;
 for(;ts!=ts1;ts++,ts1--) {
   if(ts>ts1) break;
   c=*ts;
   *ts=*ts1;
   *ts1=c;  
 }; 
}

/* Get the index of an element in a string array 
 the last element in the array should be an empty string or NULL
 Not case sensitive.
 returns -1 if not found.
*/
int sarray_index(char **sarray,char *st)
{
 static int i=0;
 int slen=strlen(st);
 if(sarray==NULL) {
	return -1;
 };

  if(slen){
  for(i=0;sarray[i]!=NULL;i++) 
  {
	if(sarray[i][0]==0) {
		break;
	};
	if(slen!=strlen(sarray[i])) continue;

	if(!strncasecmp(st,sarray[i],slen)) 
	{
		return(i);
	};
 }};
 return(-1);
}

void string_2_nospaces(char *string)
{
 int i;
// MESG("string_2_nospaces:[%s]",string);
 for(i=0;i<strlen(string);i++) {
 	if(string[i]==32 || string[i]==9) string[i] = '_';
 } 
}

int iarray_index(int *iarray,int i_to_find, int max_element)
{
 int i;
 for(i=0;i<max_element;i++) {
 	if(iarray[i]==i_to_find) return i;
 };
 return -1;
}

int iarray_remove(int *iarray, int start, int max_element)
{
 int i;
 for(i=start;i<max_element-1;i++) iarray[i]=iarray[i+1];
 iarray[max_element-1]=-1;
 return max_element-1;
}

int iarray_add(int *iarray, int value, int max_element)
{
 iarray[max_element]=value;
 return max_element+1;
}

int lstartwith(char *line,char c)
{
 if(c){
 while(*line!=0) {
	if(*line==' '||*line=='\t') { line++;continue;};
 	if(*line==c) return 1;
	break;
 };
 return (0);
 } else {
 while(*line!=0) {
	if(*line==' '||*line=='\t') { line++;continue;};
	return(0);
 };
 return(1);  
 };
}

void set_full_name(char *full_name,char *dir_name,char *file_name,int len)
{
	if(dir_name[0]==0) {
		strlcpy(full_name,getcwd(NULL,MAXFLEN),len);
		strlcat(full_name,"/",len);
		strlcat(full_name,file_name,len);
	
	} else {
		strlcpy(full_name,dir_name,len);
		strlcat(full_name,"/",len);
		strlcat(full_name,file_name,len);
	}
}
