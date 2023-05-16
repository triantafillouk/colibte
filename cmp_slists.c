/*
	- compare 2 string lists
	- The second list may have a comma separator (separates the name, full name)
	find how many of the names in the second list do not appear in the first
	list. Show the name of full name in case a comma separator exists.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "alist.h"

int main(int argc, char **argv)
{
	if(argc<3) { fprintf(stderr,"wrong arguments\n");exit(1);};
	char *list1 = argv[1];
	char *list2 = argv[2];
	char s[1000];
	// printf("%s %s\n",list1,list2);
	FILE *file=fopen(list1,"r");
	BTREE *list1_table = new_btree("list1",0);
	int index=0;
	while(fgets(s,1000,file)) {
	 	s[strlen(s)-1]=0; // remove newline
		if(strlen(s)==0) continue;
		insert_bt_element(list1_table,s,0,index++);
	};
	printf("found %d items\n",index);
	fclose(file);
	file=fopen(list2,"r");
	int total=0;
	int diffs=0;
	while(fgets(s,1000,file)) {
	 	s[strlen(s)-1]=0; // remove newline
		total++;
		char *comma = strchr(s,',');
	
		if(comma) {
			comma[0]=0;
			if(!find_btnode(list1_table,s)) {
				diffs++;
				printf("%s\n",comma+1);
			};

		} else {
			if(!find_btnode(list1_table,s)) {
				printf("%s\n",s);
				diffs++;
			};
		};
	};
	printf("total=%d diffs=%d\n",total,diffs);
	exit(0);
}