/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.
*/
COLOR_SCHEME *get_scheme_by_index(int scheme_num);
COLOR_SCHEME *get_scheme_by_name(char *name);

extern alist *color_scheme_list;

char *basic_color_names[] = {
	"black","red","green","orange","blue","magenta","cyan","white",
	"brown","lred","lgreen","yellow","lblue","lmagenta","lcyan","lwhite",NULL
};

// default color schemes
char *default_scheme_names[] = {
	"Snow","Midnight","Cyan","Blue","Gray","Dark gray",
	"Sunny","Twilight","Lunar","Velvet",
	NULL,NULL
};


COLOR_SCHEME *get_scheme_by_index(int scheme_num)
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

COLOR_SCHEME *get_scheme_by_name(char *name)
{
 COLOR_SCHEME *cs;

 lbegin(color_scheme_list);
 while((cs=(COLOR_SCHEME *)lget(color_scheme_list))!=NULL) {
	if(strcmp(name,cs->scheme_name)==0){ 
		break;
	};
 };
 return cs;
}

int color_scheme_read()
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

 MESG("color_scheme_read:");
#if	PCURSES
 if(drv_colors<16) {
	 if((fname=find_file(NULL,".colors8",1,0))==NULL) return FALSE;
 } else 
#endif
 if((fname=find_file(NULL,".colors",1,0))==NULL) return FALSE;

 f1=fopen(fname,"r");
 if(f1!=NULL) {
 while((b=fgets(bline,MAXFLEN,f1))!=NULL)
 {
	if(strlen(b)>0) b[strlen(b)-1]=0;
	int eq_ind=0;
	char *eq_chr;

	eq_chr = strchr(b,'=');
	if(eq_chr) {
		eq_ind = eq_chr-b;
		b[eq_ind]=' ';
	}

	if(lstartwith(b,';')) continue;
	if(lstartwith(b,'#')) continue;
	if(lstartwith(b,0)) continue; 	/* skip blank lines  */

	if(lstartwith(b,CHR_LBRA)) {	/* new color scheme  */

		sscanf(b,"%c%s]\n",&left,name1);
		name1[strlen(name1)-1]=0;

		scheme = get_scheme_by_name(name1);

		if(!scheme) { 	/* a new scheme!!  */
			scheme = malloc(sizeof(COLOR_SCHEME));
			add_element_to_list((void *)scheme,color_scheme_list);
			scheme->scheme_name = strdup(name1);
			int i;
			for(i=0;i<COLOR_TYPES;i++) scheme->color_style[i].color_value="#203040";
			// MESG("	create new scheme %s schemes now %d",name1,color_scheme_list->size);
		};
		// MESG("-------- read scheme named %s -----------",name1);
		continue;
	};
	strcpy(name2,"");
	sscanf(b,"%s %s %s",ctype,name1,name2);
	j=sarray_index(color_type,ctype);
	if(j>=0) {
		i=sarray_index(basic_color_names,name1);
		if(i>=0) {
			scheme->color_style[j].color_index=i;	/* 8 colors index  */
		} else {
			scheme->color_style[j].color_value=strdup(name1);	/* color value  */
			scheme->color_style[j].color_index=j;
		};
		scheme->color_style[j].color_attr=0;

			if(!strcasecmp(name2,"bold") || i>8) scheme->color_style[j].color_attr |= FONT_STYLE_BOLD;
			if(!strcasecmp(name2,"underline"))   scheme->color_style[j].color_attr |= FONT_STYLE_UNDERLINE;
			if(!strcasecmp(name2,"italic"))      scheme->color_style[j].color_attr |= FONT_STYLE_ITALIC;
			if(!strcasecmp(name2,"dim"))         scheme->color_style[j].color_attr |= FONT_STYLE_DIM;
			if(!strcasecmp(name2,"reverse"))     scheme->color_style[j].color_attr |= FONT_STYLE_REVERSE;
			// if(scheme->color_style[j].color_attr!=0) 
			// MESG(" b=[%s] -> ctype=%s: name1=%s name2=%s a=[%X]",b,ctype,name1,name2,scheme->color_style[j].color_attr);
	};
 };
	MESG("color file read ok !");
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
 fname=find_file(NULL,".colors",0,1);
 f1=fopen(fname,"w");

 if(f1!=NULL) {
	COLOR_SCHEME *scheme;
	lbegin(color_scheme_list);
	while((scheme=(COLOR_SCHEME *)lget(color_scheme_list))!=NULL){
		MESG("save_scheme: %s",scheme->scheme_name);	
		fprintf(f1,"[%s]\n",scheme->scheme_name);
		 for(i=0;i<COLOR_TYPES;i++){
		  char *attr;
		  switch(scheme->color_style[i].color_attr) {
		  	case FONT_STYLE_BOLD		: attr="bold";break;
			case FONT_STYLE_UNDERLINE	: attr="underline";break;
			case FONT_STYLE_ITALIC		: attr="italic";break;
			case FONT_STYLE_DIM			: attr="dim";break;
			case FONT_STYLE_REVERSE		: attr="reverse";break;
			default: attr="";
		  };
		  fprintf(f1,"%s=%s %s\n",color_type[i],scheme->color_style[i].color_value,attr);
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
			scheme->color_style[i].color_value=basic_color_values[scheme_ind][i].color_value;
			scheme->color_style[i].color_index=basic_color_values[scheme_ind][i].color_index;
			scheme->color_style[i].color_attr =basic_color_values[scheme_ind][i].color_attr;;
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
