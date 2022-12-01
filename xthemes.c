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

extern alist *color_scheme_list;

char *basic_color_names[] = {
	"black","red","green","orange","blue","magenta","cyan","white",
	"brown","lred","lgreen","yellow","lblue","lmagenta","lcyan","lwhite"
};

// default color schemes
char *default_scheme_names[] = {
	"Snow","Midnight","Cyan","Blue","Gray","Dark gray",
	"Sunny","Twilight","Lunar","Velvet",
	NULL,NULL
};

#if	PCURSES
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
		lbegin(color_scheme_list);
//		MESG(" read scheme named [%s]",name1);
		while((cs=(COLOR_SCHEME *)lget(color_scheme_list))!=NULL)
		{
			if(strcmp(name1,cs->scheme_name)==0){ 
				scheme = cs;
				break;
			};
			ind++;
		};
		if(!scheme) { 	/* a new scheme!!  */
			scheme = malloc(sizeof(COLOR_SCHEME));
			add_element_to_list((void *)scheme,color_scheme_list);
			scheme->scheme_name = strdup(name1);
			// MESG("	create new scheme %s schemes now %d -----------------",name1,color_scheme_list->size);
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
			// convert name2 to attrib!
			if(strstr(name2,"bold") || i>8) scheme->color_attr[j].attrib |= A_BOLD;
			if(strstr(name2,"underline")) scheme->color_attr[j].attrib |= A_UNDERLINE;
			if(strstr(name2,"reverse")) scheme->color_attr[j].attrib |= A_REVERSE;
			if(strstr(name2,"dim")) scheme->color_attr[j].attrib |= A_DIM;

		// MESG("ctype b=[%s] ctype=[%s] name1=[%s] name2=[%s] -> %d (%s) a=%d",b,ctype,name1,name2,j,color_type[j],scheme->color_attr[j].attrib);
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
#endif

COLOR_SCHEME *get_scheme(int scheme_num)
{
 COLOR_SCHEME *cs;
 int scheme_ind=0;

 lbegin(color_scheme_list);
 while((cs=(COLOR_SCHEME *)lget(color_scheme_list))!=NULL) {
 	if(scheme_ind==scheme_num) {
		break;
	};
	scheme_ind++;
 };
 return cs;
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

#if	PCURSES
 if(drv_colors<16) return color8_scheme_read();
#endif
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
		lbegin(color_scheme_list);
		// MESG(" read scheme named [%s]",name1);
		while((cs=(COLOR_SCHEME *)lget(color_scheme_list))!=NULL)
		{
			if(strcmp(name1,cs->scheme_name)==0){ 
				scheme = cs;
				break;
			};
			ind++;
		};
		if(!scheme) { 	/* a new scheme!!  */
			scheme = malloc(sizeof(COLOR_SCHEME));
			add_element_to_list((void *)scheme,color_scheme_list);
			scheme->scheme_name = strdup(name1);
			int i;
			for(i=0;i<COLOR_TYPES;i++) scheme->color_values[i]="#203040";
			MESG("	create new scheme %s schemes now %d",name1,color_scheme_list->size);
		};
		continue;
	};
	char **a_as;
	a_as = split_2_sarray(b,'=');
	// j=check_type(a_as[0]);
	j=sarray_index(color_type,a_as[0]);
	// MESG("	read color b=[%s] a=[%s][%d] = [%s]",b,a_as[0],j,a_as[1]);
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

/* save default colors to home dir   */
int color_scheme_save()
{
 FILE *f1;
 char *fname;
 int i;
 int scheme_ind=0;
// sprintf(name1,".color%1d.col",scheme_ind);
 fname=find_file(NULL,".xcolors",0,1);
 f1=fopen(fname,"w");

 if(f1!=NULL) {
	COLOR_SCHEME *scheme;
	lbegin(color_scheme_list);
	while((scheme=(COLOR_SCHEME *)lget(color_scheme_list))!=NULL){
		MESG("save_scheme: %s",scheme->scheme_name);	
		fprintf(f1,"[%s]\n",scheme->scheme_name);
		 for(i=0;i<COLOR_TYPES;i++){
		  fprintf(f1,"%s=%s\n",color_type[i],scheme->color_values[i]);
		 };
	};
	 fclose(f1);
	 msg_line("color scheme %d saved",scheme_ind+1);
 } else msg_line("color_save: cannot create file %s",fname);
 return 1;
}

char **get_scheme_names()
{
 char **scheme_names = malloc(sizeof(char *)*color_scheme_list->size+1);
 char **name=scheme_names;
 COLOR_SCHEME *cs;
 lbegin(color_scheme_list);
 while((cs=(COLOR_SCHEME *)lget(color_scheme_list))!=NULL) {
	*name++ = strdup(cs->scheme_name);
 };
 *name=NULL;
 return scheme_names;
}

void init_default_schemes()
{
 int scheme_ind;
 for(scheme_ind=0;scheme_ind<COLOR_SCHEMES;scheme_ind++){
	int i;
	COLOR_SCHEME *scheme = malloc(sizeof(COLOR_SCHEME));
	scheme->scheme_name = default_scheme_names[scheme_ind];

	// fprintf(stderr,"scheme %d [%s] ----------------------\n",scheme_ind,scheme->scheme_name);
	int total_colors=FG_COLORS+BG_COLORS;
		for(i=0;i<total_colors;i++) 
		{
			scheme->color_values[i]=basic_color_values[scheme_ind][i];
			scheme->color_attr[i].index = color_t[scheme_ind][i];
			scheme->color_attr[i].attrib = 0;
		};
	add_element_to_list((void *)scheme,color_scheme_list);
 };
}


/* change color scheme */
int change_color_scheme(int  scheme)
{
 // MESG("change_color_scheme: n=%d,color_scheme_ind=%d",scheme,color_scheme_ind);
 if(scheme<1 || scheme> color_scheme_list->size) scheme=1;
 set_current_scheme(scheme);
 if(!discmd) return (TRUE);
 set_btval("xcolor_scheme",-1,NULL,color_scheme_ind); 
 set_update(cwp,UPD_ALL);
 // update also the vertical window separator lines
 drv_back_color();
 set_cursor(0,"change_color_scheme");
 set_windows_color();
 drv_update_styles();
 // MESG("change_color_scheme: end!");
 return(TRUE);
}
