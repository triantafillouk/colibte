/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.
*/
/*
	These functions are shared bettween xlib and gtk versions, both
	are using X11 color schemes
*/


int color_scheme_read()
{
 FILE *f1;
 char *fname;
 char *b,bline[MAXFLEN];
 char name1[MAXFLEN];
 int i,j;
 char left;
 int scheme_ind=0;
// MESG("color_scheme_read:");

 if((fname=find_file(NULL,".colors",0,0))==NULL) return FALSE;

 f1=fopen(fname,"r");
 if(f1!=NULL) {

 while((b=fgets(bline,MAXFLEN,f1))!=NULL)
 {
	if(strlen(b)>0) b[strlen(b)-1]=0;
	if(lstartwith(b,';')) continue;
	if(lstartwith(b,'#')) continue;
	if(lstartwith(b,0)) continue; 	/* skip blank lines  */
	if(lstartwith(b,CHR_LBRA)) {
		sscanf(b,"%c%s]\n",&left,name1);
		name1[strlen(name1)-1]=0;
		scheme_ind=-1;
		for(i=0;i<COLOR_SCHEMES;i++) {
			if(strcmp(name1,scheme_names[i])==0){ scheme_ind=i;break;};
		};
		if(scheme_ind<0) return(0); // error
		continue;
	};
	sscanf(b,"%d=%s",&j,name1);
	if(j<XCOLOR_TYPES) {
		if(strcmp(color_name[scheme_ind][j],name1)) color_name[scheme_ind][j]=strdup(name1);
	};
 };

 fclose(f1);
 } else {
 	ERROR("color_scheme_read: cannot open file %s",fname);
 };
 return 1;
}

int color_scheme_save()
{
 FILE *f1;
 char *fname;
 int i;
 int scheme_ind;

// sprintf(name1,".color%1d.col",scheme_ind);
 fname=find_file(NULL,".xcolors",0,1);
 f1=fopen(fname,"w");

 if(f1!=NULL) {
	 for(scheme_ind=0;scheme_ind<COLOR_SCHEMES;scheme_ind++)
	 {
		 fprintf(f1,"[%s]\n",scheme_names[scheme_ind]);
		 for(i=0;i<XCOLOR_TYPES;i++){
		  fprintf(f1,"%02d=%s\n",i,color_name[scheme_ind][i]);
		 };
	 }

	 fclose(f1);
	 msg_line("color scheme %d saved",scheme_ind+1);
 } else msg_line("color_save: cannot create file %s",fname);
 return 1;
}
