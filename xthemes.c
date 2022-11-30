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

#if	GTK
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

	if(lstartwith(b,CHR_LBRA)) {	/* new color scheme  */
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
	j=sarray_index(color_type,a_as[0]);
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
#else
int color8_scheme_read()
{
 FILE *f1;
 char *fname;
 static char name1[MAXFLEN];
 static char name2[MAXFLEN];
 char *b,bline[MAXFLEN];
 char ctype[256];
 int i,j;
 char left;
 COLOR_SCHEME *scheme=NULL;

// MESG("color_scheme_read:");
 if((fname=find_file(NULL,".colors8",1,0))==NULL) return FALSE;

 f1=fopen(fname,"r");
 if(f1!=NULL) {
 while((b=fgets(bline,MAXFLEN,f1))!=NULL)
 {
	// RGB_DEF *rv;
	int eq_ind=0;
	char *eq_chr;
	if(strlen(b)>0) b[strlen(b)-1]=0;

	eq_chr = strchr(b,'=');
	if(eq_chr) {
		eq_ind = eq_chr-b;
		b[eq_ind]=' ';
	}
	if(lstartwith(b,';')) continue;
	if(lstartwith(b,'#')) continue;
	if(lstartwith(b,0)) continue; 	/* skip blank lines  */
	name2[0]=0;
	if(lstartwith(b,CHR_LBRA)) {	/* new color scheme  */
		COLOR_SCHEME *cs;
		int ind=0;
		sscanf(b,"%c%s]\n",&left,name1);
		name1[strlen(name1)-1]=0;

		scheme=NULL;
		lbegin(color_schemes);
//		MESG(" read scheme named [%s]",name1);
		while((cs=(COLOR_SCHEME *)lget(color_schemes))!=NULL)
		{
			if(strcmp(name1,cs->scheme_name)==0){ 
				scheme = cs;
				break;
			};
			ind++;
		};
		if(!scheme) { 	/* a new scheme!!  */
			scheme = malloc(sizeof(COLOR_SCHEME));
			add_element_to_list((void *)scheme,color_schemes);
			scheme->scheme_name = strdup(name1);
			MESG("	create new scheme %s schemes now %d -----------------",name1,color_schemes->size);
		};
		continue;
	};
	sscanf(b,"%s %s %s",ctype,name1,name2);
	j = sarray_index(color_type,ctype);
	if(j>=0)
	{	/* check, this must be color type!  */
			i=sarray_index(basic_color_names,name1);
			scheme->color_attr[j].index=i;
			scheme->color_attr[j].attrib=0;
			// set attrib !!
#if	1
			// convert name2 to attrib!
			if(strstr(name2,"bold") || i>8) scheme->color_attr[j].attrib |= A_BOLD;
			if(strstr(name2,"underline")) scheme->color_attr[j].attrib |= A_UNDERLINE;
			if(strstr(name2,"reverse")) scheme->color_attr[j].attrib |= A_REVERSE;
			if(strstr(name2,"dim")) scheme->color_attr[j].attrib |= A_DIM;
#endif
		MESG("ctype b=[%s] ctype=[%s] name1=[%s] name2=[%s] -> %d (%s) a=%d",b,ctype,name1,name2,j,color_type[j],scheme->color_attr[j].attrib);
	} else {
		// MESG("ctype b=[%s] ctype=[%s] name1=[%s] name2=[%s]  not found!!!",b,ctype,name1,name2);
	};
 };
	MESG("color file read ok!");
	fclose(f1);
	return 1;
 } else {
 	ERROR("color_scheme_read: cannot open file %s",fname);
	return 0;
 };
}

int color_scheme_read()
{
 FILE *f1;
 char *fname;
 static char name1[MAXFLEN];
 char *b,bline[MAXFLEN];
 int j;
 char left;
 COLOR_SCHEME *scheme=NULL;

 if(drv_colors<16) return color8_scheme_read();

// MESG("color_scheme_read:");
 if((fname=find_file(NULL,".colors16",1,0))==NULL) return FALSE;

 f1=fopen(fname,"r");
 if(f1!=NULL) {
 while((b=fgets(bline,MAXFLEN,f1))!=NULL)
 {
	if(strlen(b)>0) b[strlen(b)-1]=0;
	if(lstartwith(b,';')) continue;
	if(lstartwith(b,'#')) continue;
	if(lstartwith(b,0)) continue; 	/* skip blank lines  */

	if(lstartwith(b,CHR_LBRA)) {	/* new color scheme  */
		COLOR_SCHEME *cs;
		int ind=0;
		sscanf(b,"%c%s]\n",&left,name1);
		name1[strlen(name1)-1]=0;

		scheme=NULL;
		lbegin(color_schemes);
		// MESG(" read scheme named [%s]",name1);
		while((cs=(COLOR_SCHEME *)lget(color_schemes))!=NULL)
		{
			if(strcmp(name1,cs->scheme_name)==0){ 
				scheme = cs;
				break;
			};
			ind++;
		};
		if(!scheme) { 	/* a new scheme!!  */
			scheme = malloc(sizeof(COLOR_SCHEME));
			add_element_to_list((void *)scheme,color_schemes);
			scheme->scheme_name = strdup(name1);
			MESG("	create new scheme %s schemes now %d",name1,color_schemes->size);
		};
		continue;
	};
	char **a_as;
	a_as = split_2_sarray(b,'=');
	// j=check_type(a_as[0]);
	j=sarray_index(color_type,a_as[0]);
		MESG("	read color b=[%s] a=[%s][%d] = [%s]",b,a_as[0],j,a_as[1]);
	if(j>=0) {
		scheme->color_values[j]=strdup(a_as[1]);
	};
	free_sarray(a_as);
 };
	MESG("color file read ok!");
	fclose(f1);
	return 1;
 } else {
 	ERROR("color_scheme_read: cannot open file %s",fname);
	return 0;
 };
}
#endif

/* save default colors to home dir   */
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

extern alist *color_schemes;

void init_default_schemes()
{
 int scheme_ind;
 for(scheme_ind=0;scheme_ind<COLOR_SCHEMES;scheme_ind++){
	int i;
	COLOR_SCHEME *scheme = malloc(sizeof(COLOR_SCHEME));
	scheme->scheme_name = scheme_names[scheme_ind];

	// fprintf(stderr,"scheme %d [%s] ----------------------\n",scheme_ind,scheme->scheme_name);
	int total_colors=FG_COLORS+BG_COLORS;
		for(i=0;i<total_colors;i++) 
		{
			scheme->color_values[i]=basic_color_values[scheme_ind][i];
			scheme->color_attr[i].index = color_t[scheme_ind][i];
			scheme->color_attr[i].attrib = 0;
		};
//	show_debug_color_attr(scheme->color_attr);
	add_element_to_list((void *)scheme,color_schemes);
 };
}

