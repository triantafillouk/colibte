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

int check_type(char *type)
{
 int i=0;
 for(i=0;i<COLOR_TYPES;i++) {
 	if(!strcmp(type,color_type[i])) return i;
 };
 return -1;
}

int color_scheme_read()
{
 FILE *f1;
 char *fname;
 char *b,bline[MAXFLEN];
 char name1[MAXFLEN];
 int i,j;
 int scheme_ind=0;
// MESG("color_scheme_read:");

 if((fname=find_file(NULL,".xcolors",0,0))==NULL) return FALSE;

 f1=fopen(fname,"r");
 if(f1!=NULL) {

 while((b=fgets(bline,MAXFLEN,f1))!=NULL)
 {
	if(strlen(b)>0) b[strlen(b)-1]=0;
	if(lstartwith(b,';')) continue;
	if(lstartwith(b,'#')) continue;
	if(lstartwith(b,0)) continue; 	/* skip blank lines  */
	if(lstartwith(b,CHR_LBRA)) {
		strcpy(name1,b+1);
		name1[strlen(name1)-1]=0;
		scheme_ind=-1;
		for(i=0;i<COLOR_SCHEMES;i++) {
			if(strcmp(name1,scheme_names[i])==0){ scheme_ind=i;break;};
		};

		if(scheme_ind<0) return(0); // error
		MESG("read scheme name [%s] [%s] -> ind=%d",b,name1,scheme_ind);
		continue;
	};
	char **a_as;
	a_as = split_2_sarray(b,'=');
	j=check_type(a_as[0]);
		// MESG("	read color [%s][%d] = [%s]",a_as[0],j,a_as[1]);
	if(j>=0) {
		basic_color_values[scheme_ind][j]=strdup(a_as[1]);
	};
	free_sarray(a_as);
 };

 fclose(f1);
 } else {
 	ERROR("color_scheme_read: cannot open file %s",fname);
 };
 return 1;
}

int color8_scheme_read()
{
 FILE *f1;
 char *fname;
 char *b,bline[MAXFLEN];
 char name1[MAXFLEN];
 int i,j;
 int scheme_ind=0;
// MESG("color_scheme_read:");

 if((fname=find_file(NULL,".colors8",0,0))==NULL) return FALSE;

 f1=fopen(fname,"r");
 if(f1!=NULL) {

 while((b=fgets(bline,MAXFLEN,f1))!=NULL)
 {
	if(strlen(b)>0) b[strlen(b)-1]=0;
	if(lstartwith(b,';')) continue;
	if(lstartwith(b,'#')) continue;
	if(lstartwith(b,0)) continue; 	/* skip blank lines  */
	if(lstartwith(b,CHR_LBRA)) {
		strcpy(name1,b+1);
		name1[strlen(name1)-1]=0;
		scheme_ind=-1;
		for(i=0;i<COLOR_SCHEMES;i++) {
			if(strcmp(name1,scheme_names[i])==0){ scheme_ind=i;break;};
		};

		if(scheme_ind<0) {
			// we should create a new scheme with name
			return(0); // error
		};
		MESG("read scheme name [%s] [%s] -> ind=%d",b,name1,scheme_ind);
		continue;
	};
	char **a_as;
	a_as = split_2_sarray(b,'=');
	j=check_type(a_as[0]);
		// MESG("	read color [%s][%d] = [%s]",a_as[0],j,a_as[1]);
	if(j>=0) {
		basic_color_values[scheme_ind][j]=strdup(a_as[1]);
	};
	free_sarray(a_as);
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
		MESG("save scheme %d",scheme_ind);
		 fprintf(f1,"[%s]\n",scheme_names[scheme_ind]);
		 for(i=0;i<COLOR_TYPES;i++){
		  fprintf(f1,"%s=%s\n",color_type[i],basic_color_values[scheme_ind][i]);
		 };
	 }

	 fclose(f1);
	 msg_line("color scheme %d saved",scheme_ind+1);
 } else msg_line("color_save: cannot create file %s",fname);
 return 1;
}
