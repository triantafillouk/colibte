/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.
*/
/*
 common code for ploting interface
*/
double dtransx(double);
double dtransy(double);

int plot_is_maped=0;
int plot_width,plot_height;
int plot_clen,plot_cheight,plot_fontbase;
int resizing=0;

double rx0,ry0;

double xmin=0,xmax=0,ymin=0,ymax=0;
double logx,logy;	// for logarithmic plots
double scalex;	// scale and tiks and moves 
int iscalex,iscaley;	// same but in plot points
double pw,ph,dw,dh;
int pmode_grid=0;	// show grid
int pmode_num=1;	// show numerics on axes
int flag_zoom=0;
int flag_fill=1;
int plot_updated=0;

extern Display *dis0;

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
  plot_redraw();
 };
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
 double scale;
 // define the density of the tics
 // rules
 // not less than 20 points per half tik (40 points)
// scale = pow(10.0,m1);
 scale=(double)diff/(double)len;

 return(scale);
}

#include "mlang.h"
tok_struct *current_token();
char *tok_info(tok_struct *tok);

// cartesians coordinates , axes
int plot_coord(int n) 
{
 
 if(!plot_on()) { plot_init(0);};
// MESG("plot_coord: [%s]",tok_info(current_token()));
 xmin=expression("xmin"); skip_comma();
// MESG("xmin: %f [%s]",xmin,tok_info(current_token()));
 xmax=expression("xmax"); skip_comma();
// MESG("xmax: %f [%s]",xmax,tok_info(current_token()));
 ymin=expression("ymin"); skip_comma();
// MESG("ymin: %f [%s]",ymin,tok_info(current_token()));
 ymax=expression("ymax");
// MESG("ymax: %f [%s]",ymax,tok_info(current_token()));
 scalex = find_scale(xmax-xmin,plot_width,1);
// set_coords();
 return(1);
}

void set_ratio()
{
	ratx=((plot_width)/(xmax-xmin));
	ymax = ymin + plot_height / ratx;
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

 set_ratio();
 if(scalex == 0) scalex =find_scale(xmax-xmin,plot_width,1);
 d = xmin/scalex;
 rx0 = ratx * ( -xmin);	// -xmin is the distance of origin from left

 d = ymin/scalex;
 ry0 = rint(ratx * (-ymin)); 

}

int path_start(int n)
{
 double *v;
 char *s;
 s=strdup(kt_getstr("path"));
 v=add_plot_element(PPATH,1+strlen(s));
 return 1;
}

