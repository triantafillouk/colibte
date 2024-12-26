/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.
*/
COLOR_SCHEME *get_scheme_by_index(int scheme_num);
COLOR_SCHEME *get_scheme_by_name(char *name);

extern alist *color_scheme_list;
extern int drv_colors;

char *basic_color_names[] = {
	"black","red","green","orange","blue","magenta","cyan","white",
	"brown","lred","lgreen","yellow","lblue","lmagenta","lcyan","lwhite",NULL
};

// default color schemes
char *default_scheme_names[] = {
	"Snow","Midnight","Cyan","Blue","Gray","Dark_gray",
	"Sunny","Twilight","Lunar","Velvet",
	NULL
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

int color_change(int color,int c)
{
 int c1=80 ;
 if(c=='+'||c=='=') { c1=color+8;};
 if(c=='-') { c1=color-8;};
 if(c==1||c==-1) c1=color+c;
 if(c1<0) c1=0;
 if(c1>255) c1=255;
 return c1;
}

int color_edit(num n)
{
 int c;
 int r,g,b;
 msg_line("in color edit! (-,+,r,g,b,R,G,B, esc to exit");
 current_scheme = get_scheme_by_index(color_scheme_ind);
 while((c=drv_getc(0))!=27){
	sscanf(current_scheme->color_style[0].color_value,"#%2X%2X%2X",&r,&g,&b);
	switch(c) {
		case '+':case '=':
		case '-':
			r = color_change(r,c);
			g = color_change(g,c);
			b = color_change(b,c);
			break;
		case 'b': b = color_change(b,1);break;
		case 'g': g = color_change(g,1);break;
		case 'r': r = color_change(r,1);break;
		case 'B': b = color_change(b,-1);break;
		case 'G': g = color_change(g,-1);break;
		case 'R': r = color_change(r,-1);break;
		default:
			continue;
	};
	sprintf(current_scheme->color_style[0].color_value,"#%02X%02X%02X",r,g,b);
 	change_color_scheme(color_scheme_ind+1);
 	msg_line("new color %s  r=%d g=%d b=%d",current_scheme->color_style[0].color_value,r,g,b);
	
	update_screen(FALSE);
 }; 
 return true;
}

int color_scheme_read()
{
 FILE *f1;
 char *fname;
 static char name1[MAXFLEN];
 char *b,bline[MAXFLEN];
 int i,j;
 char left;
 COLOR_SCHEME *scheme=NULL;

 MESG("color_scheme_read: drv_colors=%d",drv_colors);
#if	PCURSES
 if(drv_colors<16) {
	 if((fname=find_file("",".colors8",1,0))==NULL) return FALSE;
 } else 
#endif
 if((fname=find_file("",".colors",1,0))==NULL) return FALSE;

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
		// MESG("read color_scheme [%s]",name1);
		scheme = get_scheme_by_name(name1);

		if(!scheme) { 	/* a new scheme!!  */
			scheme = malloc(sizeof(COLOR_SCHEME));
			add_element_to_list((void *)scheme,color_scheme_list);
			scheme->scheme_name = strdup(name1);
			// MESG("	new color scheme [%s]",name1);
			int i;
			for(i=0;i<COLOR_TYPES;i++) scheme->color_style[i].color_value="#203040";
			// MESG("	create new scheme %s schemes now %d",name1,color_scheme_list->size);
		};
		// MESG("-------- read scheme named %s -----------",name1);
		continue;
	};
	char **arg_list = split_2_sarray(b,' ');
	char **sa = arg_list;
	char *item = *sa++; 
	j = sarray_index(color_type,item);	/* get color type  */
	if(j<0) { free_sarray(arg_list); continue;};
	scheme->color_style[j].color_index=0;
	if(j>=0) {
	// loop for attributes
		scheme->color_style[j].color_attr=0;
		while((item = *sa++) !=NULL) {
			i=sarray_index(basic_color_names,item);
			if(i>=0) {
				if(i>7) scheme->color_style[j].color_attr |= FONT_STYLE_BOLD;
				scheme->color_style[j].color_index=i%8;	/* 8 colors index  */
			} else {
				if(scheme->color_style[j].color_index==0) scheme->color_style[j].color_index=j;
				if(!strcasecmp(item,"bold"))	 		scheme->color_style[j].color_attr |= FONT_STYLE_BOLD;
				else if(!strcasecmp(item,"underline"))  scheme->color_style[j].color_attr |= FONT_STYLE_UNDERLINE;
				else if(!strcasecmp(item,"italic"))     scheme->color_style[j].color_attr |= FONT_STYLE_ITALIC;
				else if(!strcasecmp(item,"dim"))        scheme->color_style[j].color_attr |= FONT_STYLE_DIM;
				else if(!strcasecmp(item,"reverse"))    scheme->color_style[j].color_attr |= FONT_STYLE_REVERSE;
				else scheme->color_style[j].color_value=strdup(item);	/* otherwise it is a color value  */
			};
		};
	};
	free_sarray(arg_list);
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
 fname=find_file("",".colors",0,1);
 MESG("color_scheme_save: ->[%s]",fname);
 f1=fopen(fname,"w");

 if(f1!=NULL) {
	COLOR_SCHEME *scheme;
	lbegin(color_scheme_list);
	while((scheme=(COLOR_SCHEME *)lget(color_scheme_list))!=NULL){
		MESG("save_scheme: %s",scheme->scheme_name);	
		fprintf(f1,"[%s]\n",scheme->scheme_name);
		 for(i=0;i<COLOR_TYPES;i++){
			char attr[64];
			strcpy(attr,"");
			if(scheme->color_style[i].color_attr & FONT_STYLE_BOLD) strcat(attr,"bold ");
			if(scheme->color_style[i].color_attr & FONT_STYLE_UNDERLINE) strcat(attr,"underline ");
			if(scheme->color_style[i].color_attr & FONT_STYLE_ITALIC) strcat(attr,"italic ");
			if(scheme->color_style[i].color_attr & FONT_STYLE_DIM) strcat(attr,"dim ");
			if(scheme->color_style[i].color_attr & FONT_STYLE_REVERSE) strcat(attr,"reverse ");
		  fprintf(f1,"%s=%s %s\n",color_type[i],scheme->color_style[i].color_value,attr);
		 };
	};
	 fclose(f1);
	 msg_line("colors saved");
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

	// fprintf(stderr,"init scheme %d [%s] ----------------------\n",scheme_ind,scheme->scheme_name);
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
int change_color_scheme(num  scheme)
{
 // MESG("change_color_scheme: n=%d,color_scheme_ind=%d",scheme,color_scheme_ind);
 if(scheme<1 || scheme> color_scheme_list->size) scheme=1;
 set_current_scheme(scheme);
 if(!discmd) return (TRUE);
 set_bt_num_val("xcolor_scheme",color_scheme_ind); 
 set_update(cwp,UPD_ALL);
 // update also the vertical window separator lines
 drv_back_color();
 set_cursor(0,"change_color_scheme");
 set_windows_color();
 drv_update_styles();
 // MESG("change_color_scheme: end!");
 return(TRUE);
}
