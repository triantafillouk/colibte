#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
// #include	<fcntl.h>
// #include	<sys/types.h>
#include	<sys/stat.h>
#include	<string.h>
#include "alist.h"
#include "support.h"
//#include "system.h"


BTREE *tags_tree;
char *separators="!@#$%^&*()=+.,[];:'\"?><";

void msg_line(char *mline)
{
}


int is_separator(int c)
{
	if(c<'0') return TRUE;
	if(c>='0' && c<='9'
	|| c>='A' && c<='Z'
	|| c>='a' && c<='z'
	|| c>=0xA0) return FALSE;
	if(c=='_') return FALSE;
	return TRUE;
}

int find_tags(char *fname,char *buffer,size_t size)
{
 int num_of_tags=0;
 char *cstart=NULL;
 char *cend=NULL;
 char *c;
 char *buffer_end=buffer+size;
 char tag[128];
 int tags_num=0;
 int discrete_tags =0;
 int ok_for_tag=1;
 for(c=buffer;c<buffer_end;c++) {
	// MESG(" [%c] %ld",*c,c-buffer);
	if(*c<=' ' && cstart==NULL) { ok_for_tag=1;continue;};
 	if(*c=='#' && ok_for_tag) {
	 cstart=c;
	 continue;
	} ;
	if(cstart) {
	 // MESG(" [%c] %2X %ld",*c,*c,c-buffer);
	 if(is_separator((int)(*c))) {
	 	
		if(c-cstart<2) { cstart=NULL;continue;};
	 	cend=c;
		int tag_size=cend-cstart;
		// if(tag_size>sizeof(tag)-1) { cstart=NULL;continue;};
		size_t tag_position = cstart-buffer;
		// MESG(" [%d] is separator size %d",*c,tag_size);
		memcpy(tag,cstart,tag_size);
		tag[tag_size]=0;
		// MESG("	tag [%s] size %d",tag,strlen(tag));
		if(!(tag[1]>=0&&tag[1]<='9' || tag_size<4))  {
			BTNODE *tag_node = find_btnode(tags_tree,tag);
			if(tag_node==NULL) {
				add_btnode(tags_tree,tag);
				discrete_tags++;
				MESG("	new tag [%s]  at position %d  of %s",tag,tag_position,get_base_name(fname));
			} else {
				// MESG("	old tag [%s] %X at position %d size %d",tag,tag[tag_size],tag_position,tag_size);
			};
			tags_num++;

		};
		cstart=NULL;
	 } else {
	 	ok_for_tag=0;
	 };
	} else { ok_for_tag=0;};
 };
 return discrete_tags;
}

int check_for_tags(char *fname,size_t size)
{
 FILE *f = fopen(fname,"r");
 if(f) {
 char *buffer = malloc(size);
 int tags_num=0;
 if(buffer) {
	size_t bytes_read=fread(buffer,size,1,f);
 	// MESG("Check for tags in %s size %ld",fname,size);
	tags_num=find_tags(fname,buffer,size);
	
 	free(buffer);
 } else { 
 	MESG("cannot allocate memory of %ld bytes",size);
 	fclose(f);
	return 0;
 };
   return tags_num;
 } else { MESG("cannot open file %s",fname);return 0;}; 
}


int main(int arg_count,char **args)
{
 
 set_debug(1);
 if(arg_count<2) {
 	MESG("usage is 'find_tags <dir_name>'");
	return 1;
 };

 char **file_names;
 char cmd[1024];
 char tmp_file[100];
 char *dir_name = args[1];
 int size=0;
 int status=0;
 MESG("check files in %s",args[1]);
 int dir_name_size=strlen(args[1])+1;
 sprintf(tmp_file,"/tmp/notes_contents.out");
 sprintf(cmd,"find -L %s  >%s 2>/dev/null",dir_name,tmp_file);
 status = system(cmd);
 if(status!=0) { MESG("cannot read directory %s!",dir_name);return 2;};
 sync();
 tags_tree = new_btree("tag_tree",0);

 file_names = read_sarray(tmp_file,&size);
 int i,total_files=0;
 int total_dirs=0;
 int total_tags=0;
 for(i=0;file_names[i]!=NULL;i++) {
	struct stat st;
	if(!stat(file_names[i],&st))
	if(!S_ISDIR(st.st_mode)) 
	{
		if(size<10) continue;
		int file_name_size=strlen(file_names[i]);
		if(!strncmp(file_names[i]+dir_name_size,".git",4)) {
			continue;
		};
		if(strcmp(file_names[i]+file_name_size-4,".txt")==0 
		|| strcmp(file_names[i]+file_name_size-3,".md")==0  
		|| strcmp(file_names[i]+file_name_size-4,".cal")==0  
		|| strcmp(file_names[i]+file_name_size-4,".tdc")==0  
		) { 
		// MESG("Check [%s]",file_names[i]+file_name_size-3);
		total_files++;
	 	// MESG("%2d %s",i,file_names[i]);
		total_tags+=check_for_tags(file_names[i],st.st_size);
		//FILEBUF *bp;
		};
	} else total_dirs++;
 };
 MESG("total %d files in %d dirs to parse, total_tags=%d",total_files,total_dirs,total_tags);
 return 0;
}
