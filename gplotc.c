/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

*/

/* Gtk version of plot */
#include	<sys/time.h>
#include	<signal.h>
#include	"xe.h"
#include	<X11/Xlib.h>
#include	<gdk/gdkx.h>

#include	"gcanvasc.h"
#include	"gtk_support.h"

#include "plot.h"
#include "gplot.h"


extern GtkWidget *Interface_pixmap_new(GtkWidget *parent, gchar **data);
GtkWidget * new_toolbar(GtkWidget *parent, GtkWidget *box, T_ELEMENT telem[], int orient, int icon_size,int side);
double expression(char *title);
void skip_comma();
void pimage(char *s, double zoom,double angle,int f);
void str_tik1(cairo_t *cr,double x,double y,double value,double precision);
int plot_on();
plot_element *padd_plot_element(int type, int size);

double ratx;	// ratios from real to plot points
double plot_x,plot_y;
double alpha_value=1;
char *splot_title=NULL;

void show_values();

// Colormap cmap;
Visual	*vis;
int depth;
int plot_sync=0;
int redrawing_on=0;
int center_zoom=0;
int mid_flag=0;

#include "support.h"
gboolean on_plot_motion(GtkWidget *w, GdkEventMotion *ev, gpointer data);

void plot_reset(int,int);

#define TIKLEN	5

plot_element *plist=(plot_element*)NULL ,*plist_start=(plot_element*)NULL;
char * kt_getstr(char *);

GtkWidget *wplot=NULL;
GtkWidget *wplotp=NULL;
GtkWidget *wplot_tools=NULL;
GtkWidget *wplot_sline=NULL;	// status
GtkWidget *wplot_pline=NULL;	// position
GdkFont *font_plot;

GdkCursor cur1;
GdkGC *pgc,*ffgc, *bpgc;	// plot graphics context

void draw_line(cairo_t *cr,int x0,int y0,int x1, int y1);
void draw_line_to(cairo_t *cr,int x1, int y1);
void toggle_center();
void toggle_mid();
void toggle_origin();

#include "plot_commonc.c"

double dtransx(double x)
{
 double ri;
 ri = x*ratx+rx0;
 return(ri);
}

double dtransy(double y)
{
 double ri;
 ri=plot_height - (y*ratx+ry0);
 return(ri);
}


#include "plot_cairo.c"

int plot_clear(num);
int plot_update=0;

void draw_line(cairo_t *cr,int x0,int y0,int x1, int y1)
{
 cairo_move_to(cr,x0,y0);
 cairo_line_to(cr,x1,y1);
}

void draw_line_to(cairo_t *cr,int x1, int y1)
{
 cairo_line_to(cr,x1,y1);
}

int plot_close(num n) 
{
 if(plot_is_maped) 
 {
	plot_is_maped=0;
	gtk_widget_destroy(wplot_sline);
	gtk_widget_destroy(wplotp);
	wplot=NULL;
 	free_plot_list();
	wplotp=NULL;
	darea=NULL;
 };
 return 1;
}


int plot_rotate(num n)
{
 double a;
 double *v;
 if(!plot_is_maped) return(0);
 a=n;
 v=add_plot_element(PROTATE,1);
// v[0]=a * 3.14/180;
 v[0]=a;
 return 1;
}

int plot_translate(num n)
{
 double x,y;
 double *v;
 if(!plot_is_maped) return(0);
 x=get_val();
 y=expression("y");
 v=add_plot_element(PTRANSLATE,2);
 v[0]=x;
 v[1]=y;
 return 1;
}

int plot_scale(num n)
{
 double x,y;
 double *v;
 if(!plot_is_maped) return(0);

 x=get_val();
 y=expression("y");
 v=add_plot_element(PSCALE,2);
 v[0]=x;
 v[1]=y;
 return 1;
}

int plot_move_to(num n)
{
 double x,y;
 double *v;
 if(!plot_is_maped) return(0);

 x=get_val();
 y=expression("y");
 v=add_plot_element(PMOVE,2);
 v[0]=x;
 v[1]=y;

 plot_x=x;plot_y=y;
 return 1;
}

int plot_elipsep(num n)
{
 double x,y;
 double *v;
 if(!plot_is_maped) return(0);
 
 v=add_plot_element(PELIPSE,3);
 v[0]=expression("a");skip_comma();
 v[1]=expression("x");skip_comma();
 v[2]=expression("y");
 
 plot_x=v[1];plot_y=v[2];
 return 1;
}

int plot_point(num n)
{
 double x,y;
 double *v;
 if(!plot_is_maped) return(0);

 x=get_val();
 y=expression("get y");
 v=add_plot_element(PPOINT,2);
 v[0]=x;
 v[1]=y;

 plot_x=x;plot_y=y;
 return 1;
}

int plot_line(num n) 
{
 double x0,y0,x1,y1;
 double *v;

 x0=expression("plot_line x0");skip_comma();
 y0=expression("plot line y0");skip_comma();
 x1=expression("plot_line x1");skip_comma();
 y1=expression("plot_line y1");
 v=add_plot_element(PLINE,4);
 v[0]=x0; v[1]=y0; v[2]=x1; v[3]=y1;

 plot_x=x1;plot_y=y1;
 return 1;
}

int plot_lineto(num n) 
{
 double  x0,y0;
 double x1,y1;
 double *v;

 x0=plot_x;
 y0=plot_y;

 x1=get_val();
 y1=expression("get y");

 v=add_plot_element(PLINETO,2);
 v[0]=x1; v[1]=y1;

 plot_x=x1,plot_y=y1;
 return 1;
}

int plot_curve_to(num n)
{
 double *v;
 if(!plot_is_maped) return(0);

 v=add_plot_element(PCURVETO,6);

 v[0]=expression("x1");skip_comma();
 v[1]=expression("y1");skip_comma();
 v[2]=expression("x2");skip_comma();
 v[3]=expression("y2");skip_comma();
 v[4]=expression("x3");skip_comma();
 v[5]=expression("y3");
 plot_x=v[4];plot_y=v[5];
 return 1;
}

// set rgb color (stroke color)
int plot_color(num n)
{
 int color;
 double *v;
 if(!plot_is_maped) return(0);
 color=n;
 v=add_plot_element(PCOLOR,3);
 v[0]=expression("red");skip_comma();
 v[1]=expression("green");skip_comma();
 v[2]=expression("blue");
 return 1;
}

	/* fill color  */
int plot_fcolor(num n)
{
 double r,g,b,a;
 double *v;
	r=expression("x");skip_comma();
	g=expression("y");skip_comma();
	b=expression("r");skip_comma();
	a=expression("a");
	v=add_plot_element(PFCOLOR,4);
	v[0]=r;v[1]=g;v[2]=b;v[3]=a;
 return n;
}

// set line width
int plot_lwidth(num n)
{
 double width;
 double *v;
 if(!plot_is_maped) return(0);
 width=get_val();
 v = add_plot_element(PLWIDTH,1);
 v[0]=width;
 return 1;
}

int plot_set_color(num n)
{
 double *v;
 char *s;
 double val=0;
 plot_element *pe;

 pe=padd_plot_element(PSETCOLOR,4);
	val=expression("name");skip_comma();
	pe->pe_name=strdup(kt_getstr("type"));
	v=pe->v;
	
// MESG("plot_set_color:[%s]",s);
	v[0]=expression("r");skip_comma();
	v[1]=expression("g");skip_comma();
	v[2]=expression("b");
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

int plot_resize(num n)
{
 if(!plot_is_maped) return(0);

 plot_width=get_val();
 plot_height=expression("h");

 gtk_widget_set_size_request(wplotp,plot_width,plot_height);
 return 1;
}

void plot_status(int f,char *st)
{
 gtk_label_set_text((GtkLabel *)((GtkStatusbar *)wplot_pline)->label,st);
}

int plot_title(num n)
{
 char s[MAXLLEN];
 double *v;
 int size;
 if(!plot_is_maped) return(plot_init(0));
 strlcpy(s, kt_getstr("title"),MAXLLEN);
 size=strlen(s)+1;
 gtk_window_set_title(GTK_WINDOW(wplotp),s);
 v = add_plot_element(PTITLE,1+size/sizeof(double) );
 strlcpy((char *)v, s,size);	/* ???????????  */
 if(splot_title!=NULL) free(splot_title);
 splot_title=strdup(s);
 return 1;
}

int plot_text(num n)
{
 char s[MAXLLEN];
 double *v;
 plot_element *pe;
 strlcpy(s, kt_getstr("text"),MAXLLEN);
 pe = padd_plot_element(PTEXT,0);
 pe->pe_name=strdup(s);
 return 1;
}


int path_end(num n)
{
 double *v;

 v=add_plot_element(PEND,0);
 return 1;
}

double tdif(double xmax,double xmin)
{
 double diff=(xmax-xmin)/20;
 double tik=10;
 if(diff>1000000) tik=2000000;
 else if(diff>500000) tik=1000000;
 else if(diff>250000) tik=500000;
 else if(diff>100000) tik=200000;
 else if(diff>50000) tik=100000;
 else if(diff>25000) tik=50000;
 else if(diff>10000) tik=20000;

 else if(diff>5000) tik=10000;
 else if(diff>2500) tik=5000;
 else if(diff>1000) tik=2000;

 else if(diff>500) tik=1000;
 else if(diff>250) tik=500;
 else if(diff>100) tik=200;

 else if(diff>50) tik=100;
 else if(diff>25) tik=50;
 else if(diff>10) tik=20;

 else if(diff>5) tik=10;
 else if(diff>2.5) tik=5;
 else if(diff>1) tik=2;

 else if(diff>.5) tik=1;
 else if(diff>.25) tik=.5;
 else if(diff>.1) tik=.2;

 else if(diff>.05) tik=.1;
 else if(diff>.025) tik=.05;
 else if(diff>.01) tik=.02;
 return tik;
}

void show_coords(cairo_t *crp)
{
 double startx,starty;
 double s0;
 double step=tdif(xmax,xmin);

  startx=(rint(xmin/step)+1)*step;

// show static title
	cairo_identity_matrix(crp);
	cairo_move_to (crp, 10.0, 20.0);
	cairo_set_source_rgb(crp,color_title[0],color_title[1],color_title[2]);
	cairo_select_font_face(crp,"Menlo",CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(crp,18);

	if(splot_title) cairo_show_text(crp,splot_title);

	set_default_matrix(crp);

	startx=(rint(xmin/step))*step;
	starty=(rint(ymin/step))*step;
	if(pmode_grid){	/* show grid  */
		cairo_set_source_rgb(crp, color_grid[0],color_grid[1] ,color_grid[2]);	/* grid color  */
		cairo_set_line_width(crp, 0.2);	/* grig weight  */
		for(s0=startx;s0<xmax;s0 += step){
			draw_line(crp,dtransx(s0),dtransy(ymin),dtransx(s0),dtransy(ymax));
		};
		for(s0=starty;s0<ymax;s0 += step){
			draw_line(crp,dtransx(xmin),dtransy(s0),dtransx(xmax),dtransy(s0));
		};
		cairo_stroke(crp);
	};


	{	/* show tiks  */
	double cx0,cy0;
	if(xmin<=0 && xmax>0) cx0=0;else cx0=xmin;
	if(ymin<=0 && ymax>0) cy0=0;else cy0=ymin;

	cairo_set_source_rgb(crp, color_tik[0], color_tik[1], color_tik[2]);	/* tik color  */
	cairo_set_line_width(crp,2);
	for(s0=starty; s0<ymax; s0+=step) {
		if(pmode_num) str_tik1(crp,dtransx(cx0), dtransy(s0+TIKLEN*scalex),s0,scalex);
		draw_line(crp,dtransx(cx0),dtransy(s0),dtransx(cx0+TIKLEN*scalex),dtransy(s0));
	};
	for(s0=startx; s0<xmax; s0+=step) {
		if(pmode_num) str_tik1(crp,dtransx(s0+TIKLEN/2*scalex),dtransy(cy0+TIKLEN/2*scalex),s0,scalex); 
		draw_line(crp,dtransx(s0),dtransy(cy0),dtransx(s0),dtransy(cy0+TIKLEN*scalex));
	};
	}
	cairo_stroke(crp);

// show origin lines
	cairo_set_source_rgb(crp, color_coord[0], color_coord[1], color_coord[2]);	/* origin color  */
	cairo_set_line_width(crp, 0.5);	/* origin weight  */

	draw_line(crp,dtransx(0),dtransy(ymax),dtransx(0),dtransy(ymin));	/* yaxis  */
	draw_line(crp,0,dtransy(0),plot_width,dtransy(0)); // x axis

	cairo_stroke(crp);
}

void str_tik1(cairo_t *cr,double x,double y,double value,double precision)
{
 char st[80];
 double v;
 v=precision; 

 if(v < 0.01) snprintf(st,sizeof(st),"%1.3f",value);
 else if(v < 0.1) snprintf(st,sizeof(st),"%1.2f",value);
 else if(v<1) snprintf(st,sizeof(st),"%1.1f",value);
 else if(v<100) snprintf(st,sizeof(st),"%2.0f",value);
 else if(v<1000) snprintf(st,sizeof(st),"%3.0f",value);
 else if(v<10000) snprintf(st,sizeof(st),"%4.0f",value);

 cairo_set_font_size(cr,8);
 cairo_move_to(cr,x,y);
 cairo_show_text(cr,st); 
}

int plot_circle(num n) 
{
	double x,y,r;
	double *v;

	if(!plot_is_maped) return(0);
	x=expression("x");skip_comma();
	y=expression("y");skip_comma();
	r=expression("r");

	v=add_plot_element(PCIRCLE,3);
	v[0]=x;
	v[1]=y;
	v[2]=r;
	plot_x=x,plot_y=y;
	return 1;
}

int plot_rcircle(num n) 
{
	double r;
	double *v;

	if(!plot_is_maped) return(0);
	r=n;

	v=add_plot_element(PRCIRCLE,1);
	v[0]=r;
	return 1;
}

int plot_arc(num n) 
{
 double r,a1,a2;
 double *v;
 if(!plot_is_maped) return(0);
	r=expression("r");skip_comma();
	a1=expression("a1");skip_comma();
	a2=expression("a2");

	v=add_plot_element(PARC,3);
	v[0]=r;
	v[1]=a1*3.14/180;
	v[2]=a2*3.14/180;
	return 1;
}

int plot_rectangle(num n)
{
 double w,h;
 double *v;
 	w=get_val();
	h=expression("h");
	v=add_plot_element(PRECTANGLE,2);
	v[0]=w;
	v[1]=h;
	return 1;
}

int plot_triangle(num n)
{
 double *v;
	v=add_plot_element(PTRIANGLE,4);
	v[0]=expression("x");skip_comma();
	v[1]=expression("y");skip_comma();
	v[2]=expression("x");skip_comma();
	v[3]=expression("y");

	return 1;
}

int plot_polyline(num n)
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


int plot_image(num n) 
{
 char s[MAXLLEN];
 double *v;
 double zoom,angle;
 int size;
 if(!plot_is_maped) return(0);

	zoom=expression("image name");skip_comma();
	strlcpy(s,kt_getstr("Image"),MAXLLEN);
	size=strlen(s)+1;
	zoom=expression("zoom");skip_comma();
	angle=expression("angle");
	
	v = add_plot_element(PIMAGE,4+size/sizeof(double));
	v[0]=zoom;	v[1]=angle;

	strlcpy((char *)&v[3],s,size);

	return 1;
}


int set_absolute(num n)
{
	double *v;
	v = add_plot_element(PABSOLUTE,0);
	relative_flag=0;
	return 0;
}

int set_relative(num n)
{
	double *v;
	v = add_plot_element(PRELATIVE,0);
	relative_flag=1;
	return 1;
}

int plot_alpha(num n)
{
 double *v;
 double a;

	a=n;
	v = add_plot_element(PALPHA,1);
	v[0]=a;

	return true;
}

int plot_stat(num n) 
{
 if(!plot_is_maped) return(0);
	MESG("plot_stat not implemented yet!");	
	return 1;
}

void toggle_origin()
{
	if(plot_origin) plot_origin=0;else plot_origin=1;
}

void toggle_gridmode()
{
	if(pmode_grid) pmode_grid=0; else pmode_grid=1;
}

void toggle_nummode()
{
	if(pmode_num) pmode_num=0; else pmode_num=1;
}

void toggle_zoom()
{ // do we zoom at resize or not?
	if(flag_zoom) flag_zoom=0;else flag_zoom=1;
}

void toggle_center()
{ // do we zoom at resize or not?
	if(center_zoom) center_zoom=0;else center_zoom=1;
}

void toggle_mid()
{ // do we zoom at resize or not?
	if(mid_flag) mid_flag=0;else mid_flag=1;
}

double dmean(double x,double y)
{
 return( (x+y)/2.0);
}

int plot_zoom(num n) 
{
 double nx0,ny0;
 double d,dy;
 int ty;
	nx0 = dmean(xmax,xmin);
	ny0 = dmean(ymax,ymin);

	if(n==1) { // zoom
		d=(xmax - xmin)/3.0;
		dy=(ymax - ymin)/3.0; 
	} else {
		d=(xmax-xmin)*0.75;	
		dy=(ymax-ymin)*0.75; 
	};

	if(!center_zoom) {
		xmax = xmin + 2*d;
	} else {
		xmin = nx0 - (d);
		xmax = nx0 + (d);
	}

	if(!center_zoom) {
		ymax = ymin+2*dy;
	} else {
		ymin=ny0-dy;
		ymax=ny0+dy;
	};
	
	// set scale
	scalex = find_scale(xmax-xmin,plot_width,1);
	set_ratio();

	rx0 = ratx * (-xmin);
	ry0 = ratx * (-ymin);
	plot_redraw();

	return 1;
}

int plot_move(num direction) 
{
 double nx0;
	nx0 = tdif(xmax,xmin);
	switch (direction) {
		case 0: 
		{ 
			xmin -= nx0; xmax -= nx0; 
			rx0 = ratx * ( -xmin);
			break;
		};
		case 1: 
		{
			ymin += nx0; ymax += nx0; 
			ry0 = ratx * (-ymin);
			break;
		};
		case 2: 
		{ 
			xmin += nx0; xmax += nx0; 
			rx0 = ratx * ( -xmin);
			break;
		};
		case 3: {
			ymin -= nx0; ymax -= nx0; 
			ry0 = ratx * ( -ymin);
			break;
		};
	};	
	return 1;
}


char *kt_getstr(char *prompt)
{
 char arg[MAXLLEN];
 static char s[MAXLLEN];
 int status;
 arg[0]=0;
 status = nextarg(prompt, arg, MAXLLEN,true);
 strlcpy(s,arg,MAXLLEN);
 return(s);
}

void show_values()
{
 MESG("show values ------------------------------");
 MESG("xmin=%f xmax=%f ymin=%f ymax=%f",xmin,xmax,ymin,ymax);
 MESG("scalex=%f ratx=%f width=%d height=%d",scalex,ratx,plot_width,plot_height);
 MESG("zoom=%d",flag_zoom);
}

void new_plot_list()
{
 if(plist_start == (plot_element *)NULL) {
 plist = new_plot_element(1);
 plist_start = plist;
 plist->type=0;
 plist->v=0;
 plist->next = (plot_element *)NULL;
 };
}

// return a pointer to the data array 
double *add_plot_element(int type, int size)
{
 plist->next = new_plot_element(size);
 plist = plist->next;
 plist->type=type;
 plist->next = (plot_element *)NULL;
 return(plist->v);
}


int plot_close_path(num n)
{
	if(plist!=NULL) {
		plist->closed=1;
		return 1;
	} else return 0;
}

int plot_poly_nostroke(num n)
{
	if(plist!=NULL) {
		plist->stroke=0;
		plist->closed=1;
		return 1;
	} else return 0;
}

void free_plot_element(plot_element *pl)
{
 if(pl->pe_name!=NULL) free(pl->pe_name);
 if(pl->v!=NULL) free(pl->v);
 free(pl);
}

void free_plot_list()
{
 plot_element *pl,*pln;

 pl=plist_start;
 do {
	pln = pl->next;
	free_plot_element(pl);	
	pl=pln;
//	n1++;
 } while(pln !=(plot_element *)NULL);
 plist_start = (plot_element *)NULL;
}

plot_element *new_plot_element(int size)
{
	plot_element *pe;

	pe = (plot_element*) malloc(sizeof(plot_element));
	pe->pe_name=NULL;
	if(size>0) {
	pe->v = (double *) malloc(size*sizeof(double));
	pe->v[0]=size;
	} else pe->v=NULL;
	return(pe);
}


gboolean on_plot_motion(GtkWidget *w, GdkEventMotion *ev, gpointer data)
{
 char st[64];
 double xx,yy;
 xx = (ev->x - rx0) / ratx;
 yy = ((plot_height - ev->y) - ry0)/ratx; 
 snprintf(st,sizeof(st),"%3.2f %3.2f",xx,yy);
 plot_status(0,st);
 return TRUE;
}


void plot_reset(int w, int h)
{
 plot_width=w;
 plot_height=h;
	if(flag_zoom) {
		scalex = find_scale(xmax-xmin, plot_width,1);
		ratx = ((plot_width)/(xmax-xmin));
		ymax = ymin + plot_height / ratx;
	} else {
		xmax = xmin + plot_width / ratx;
		ymax = ymin + plot_height / ratx;
	};
}

void pl_clear()
{
}

/* -- */
