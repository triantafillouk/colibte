/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.
*/
/*
 common code for ploting interface
*/
int plot_is_maped=0;
int plot_width,plot_height;
int plot_clen,plot_cheight,plot_fontbase;
int resizing=0;

long rx0,ry0;
double ax0,ay0;	// x,y values of orizin
double ty,tx; //  tx=rint(xmin/scalex) (xmin should be a multiple of scalex!).
int cx0,cy0;
double xmin,xmax,ymin,ymax;
double logx,logy;	// for logarithmic plots
double scalex,scaley;	// scale and tiks and moves 
int iscalex,iscaley;	// same but in plot points
double pw,ph,dw,dh;
int pmode_grid=0;	// show grid
int pmode_num=1;	// show numerics on axes
int zoom_priority=1;	
int flag_zoom=0;
int flag_fill=0;
int plot_updated=0;

extern Display *dis0;

void str_tik(int x,int y,double value,double precision);
double find_scale(double,int,int);
void set_coords();
void show_coords();
void plot_redraw();
void pl_clear();

void do_timer_plot()
{
 if(!plot_updated) {
  plot_updated=1; 
  if(resizing) return;
  pl_clear();
  plot_redraw();
 };
}

int transx(double x)
{
 int ri;
 ri = rint(x*ratx+rx0);
 if(ri<-32768) ri=-32700;
 if(ri>32768) ri=32700;
 return(ri);
}

int transy(double y)
{
 int ri;
 ri=rint(plot_height - (y*raty+ry0));
 if(ri<-32768) ri=-32700+plot_height;
 if(ri>32768) ri=32700;
 return(ri);
}

plot_element *padd_plot_element(int type, int size)
{
	plist->next = new_plot_element(size);
	plist = plist->next;
	plist->type=type;
	plist->next = (plot_element *)NULL;
	plist->closed=0;	/* not closed!  */
	plist->stroke=1;	/* paint stroke!  */
	plist->relative=0;
	plist->subtype=0;
	plist->selected=0;
 return(plist);
}

int plot_close_path(int n)
{
	if(plist!=NULL) {
		plist->closed=1;
		return 1;
	} else return 0;
}

int plot_poly_nostroke(int n)
{
	if(plist!=NULL) {
		plist->stroke=0;
		plist->closed=1;
		return 1;
	} else return 0;
}

int plot_polyline(int n)
{
 double *v;
 int i=1;
 int alloc=4;
 plot_element *pe;

	pe=padd_plot_element(PPOLY,5);
	v=pe->v;
	pe->closed=0;	/* not closed!  */
	pe->stroke=1;	/* paint stroke!  */
	pe->relative=relative_flag;
	pe->subtype=0;
	pe->selected=0;
	while(1){
		if(i>alloc) {
			v=realloc(v,(i+16)*sizeof(double));
			pe->v=v;
		}
		v[i++]=expression("x");skip_comma();
		v[i++]=expression("y");
		if(check_rparenthesis()) break;
		else skip_comma();
	};
	v[0]=i;
	return 1;
}

int plot_triangle(int n)
{
 double *v;
	v=add_plot_element(PTRIANGLE,4);
	v[0]=expression("x");skip_comma();
	v[1]=expression("y");skip_comma();
	v[2]=expression("x");skip_comma();
	v[3]=expression("y");

	return 1;
}

int plot_set_color(int n)
{
 double *v;
 plot_element *pe;

 pe=padd_plot_element(PSETCOLOR,4);
	pe->pe_name=strdup(kt_getstr("type"));
	v=pe->v;
	v[0]=expression("name");skip_comma();
	
	v[0]=expression("x");skip_comma();
	v[1]=expression("y");skip_comma();
	v[2]=expression("x");
 v[0]=v[0]/255.0;
 v[1]=v[1]/255.0;
 v[2]=v[2]/255.0;

 if(!strcmp(pe->pe_name,"back")) { 
 	color_back[0]=v[0];
	color_back[1]=v[1];
	color_back[2]=v[2];
 };
 if(!strcmp(pe->pe_name,"coord")) {
 	color_coord[0]=v[0];
	color_coord[1]=v[1];
	color_coord[2]=v[2];
 };
 if(!strcmp(pe->pe_name,"grid")) {
	color_grid[0]=v[0];
	color_grid[1]=v[1];
	color_grid[2]=v[2];
 };
 if(!strcmp(pe->pe_name,"tik")) {
	color_tik[0]=v[0];
	color_tik[1]=v[1];
	color_tik[2]=v[2];
 };
 if(!strcmp(pe->pe_name,"title")) {
	color_title[0]=v[0];
	color_title[1]=v[1];
	color_title[2]=v[2];
 };
 return 1;
}

void set_plot_timer()
{
 struct itimerval tim1;
// we must use a timer for redraws
 tim1.it_interval.tv_sec=0;
 tim1.it_interval.tv_usec=100000;
 tim1.it_value.tv_sec=0;
 tim1.it_value.tv_usec=100000;
 setitimer(ITIMER_REAL,&tim1,0);
 plot_updated=1;
 signal(SIGALRM, do_timer_plot);
}

int plot_frames(int n)
{
	double *v;
	v = add_plot_element(PFRAMES,1);
	if(n) show_frames=1;
	else show_frames=0;
	v[0]=n;
	return 0;
}

double find_scale(double diff,int len,int type)
{
 int pref_tiks,t1;
 int m1;
 static double d1,scale,mesa,t2;
 // define the density of the tics
 // rules
 // not less than 20 points per half tik (40 points)
 pref_tiks = len / 40;
 d1 = (diff) / pref_tiks;
 m1 = rint(d1);
 mesa=1;
// scale = pow(10.0,m1);
 scale=(double)diff/(double)len;
 t1 = rint((diff) / scale);
 t2 = (double)t1 / pref_tiks;

 if(t2 > 7) mesa=10;
 else if(t2>3) mesa=5;
 else if(t2>1.5) mesa=2;
 else if(t2<.1) mesa=0.1;
 else if(t2<.3) mesa=0.2;
 else if(t2<.7) mesa=0.5;
 scale = scale * mesa;
 return(scale);
}

// cartesians coordinates , axes
int plot_coord(int n) 
{
 if(!plot_is_maped) { plot_init(0);};
 xmin=expression("xmin"); skip_comma();
// MESG("xmin: %f [%s]",xmin,tok_info(tok));
 xmax=expression("xmax"); skip_comma();
// MESG("xmax: %f [%s]",xmax,tok_info(tok));
 ymin=expression("ymin"); skip_comma();
// MESG("ymin: %f [%s]",ymin,tok_info(tok));
 ymax=expression("ymax");
// MESG("ymax: %f [%s]",ymax,tok_info(tok));
 scalex = find_scale(xmax-xmin,plot_width,1);
 scaley=scalex; 
 set_coords();
 show_coords();
 return(1);
}

void set_ratio()
{
	if(zoom_priority==1){
		raty=((plot_height)/(ymax-ymin));
	 	ratx = raty;
		scalex = scaley;
	 	xmax = xmin + plot_width / ratx ;
	} else {
		ratx=((plot_width)/(xmax-xmin));
		raty = ratx;
		scaley = scalex;
		ymax = ymin + plot_height / raty;
	};
}

void set_coords()
{
 double dw,dh;
 double pw,ph;
 double d;

 dw = xmax-xmin;
 dh=ymax-ymin;
 pw = plot_width;
 ph = plot_height;

 if(xmax==xmin) xmax=xmin+100;
 if(ymax==ymin) ymax=ymin+100;

 set_ratio();
 if(scalex == 0) scalex =find_scale(xmax-xmin,plot_width,1);
 d = xmin/scalex;
 tx = rint(d);
 rx0 = rint(ratx * ( -xmin));	// -xmin is the distance of origin from left

 ax0 = (tx+1.0)*scalex;
 cx0 = rint((ax0-xmin)*ratx);

 if(scaley == 0) scaley = find_scale(ymax-ymin,plot_height,2);
 d = ymin/scaley;
 ty = rint(d);
 ry0 = rint(raty * (-ymin)); 

 ay0 = (ty+1.0)*scaley;
// cy0 = rint(scaley * raty);
 cy0 = rint((ay0-ymin)*raty);

 if(xmin<0 && xmax>0) { // point 0:0 is on screen
	cx0 = rx0 ;
	ax0 = 0.0;
 } else { // point 0:0 is off screen
	ax0 = (tx+1.0)*scalex;
	cx0 = rint(scalex * ratx);
 };

 if(ymin<0 && ymax>0) { // point 0:0 is on screen
	cy0 = ry0 ;
	ay0 = 0.0;
 } else { // point 0:0 is off screen
	ay0 = (ty+1.0)*scaley;
	cy0 = rint(scaley * raty);
 };

}

int path_start(int n)
{
 double *v;
 char *s;
 s=strdup(kt_getstr("path"));
 v=add_plot_element(PPATH,1+strlen(s));
 return 1;
}

int path_end(int n)
{
 double *v;
 v=add_plot_element(PEND,0);
 return 1;
}

int set_absolute(int n)
{
	double *v;
	v = add_plot_element(PABSOLUTE,0);
	relative_flag=0;
	return 0;
}

int set_relative(int n)
{
	double *v;
	v = add_plot_element(PRELATIVE,0);
	relative_flag=1;
	return 1;
}

int plot_text(int n)
{
 char s[MAXLLEN];

 plot_element *pe;
 strlcpy(s, kt_getstr("text"),MAXLLEN);
 pe = padd_plot_element(PTEXT,0);
 pe->pe_name=strdup(s);
 return 1;
}

