/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

*/
/* 
	Cairo gtk2 drawing driver ...
*/

void redraw(GtkWidget *widget);
void paint_image(cairo_t *cr, char *name, int x,int y,double scale,double angle);
void paint_rectangle(cairo_t *cr, double *v);
void paint_triangle(cairo_t *cr, double *v);
void paint_elipsep(cairo_t *cr,double a, double v1,double v2);

void draw_coords(cairo_t *cr);
void draw_objects(cairo_t *cr);
void draw_objects2(cairo_t *cr);
void set_default_matrix(cairo_t *cr);

struct {
  int count;
  double coordx[100];
  double coordy[100];
} glob;

GtkWidget *darea=NULL;

static int angle1=0;
static int posx1 = 40;
static int posy1 = 40;
double fill_color[4]={0.8,0.8,0.8,0.5};
double stroke_color[4]={1.0,1.0,0,1};
double line_width=1;
double current_angle=0;
double current_scale[2];

//int show_frames=1;

void set_fill_color(cairo_t *cr)
{
 cairo_set_source_rgba(cr,fill_color[0],fill_color[1],fill_color[2],fill_color[3]);
}

void set_stroke_color(cairo_t *cr)
{
 cairo_set_source_rgba(cr,stroke_color[0],stroke_color[1],stroke_color[2],stroke_color[3]);
}

void on_draw_event(GtkWidget *widget, GdkEventExpose *event)
{
	if(event->area.width > 10)	redraw(widget);
}

void plot_redraw()
{
 if(darea!=NULL) {
	redraw(darea);
 }
}


void redraw(GtkWidget *widget)
{
 static int redrawing=0;
 cairo_t *cr;
 int coords_before=0;
	if(redrawing) return;
	redrawing=1;
    //Create the cairo context
	cr = gdk_cairo_create(widget->window);

	/* initialize default variables  */
	alpha_value=1.0;
	relative_flag=0;

	set_default_matrix(cr);	
	/* clear canvas and init coords  */
	set_coords();
//	cairo_set_source_rgb(cr, 0.9, 0.9, 0.90);
	cairo_set_source_rgb(cr,color_back[0],color_back[1],color_back[2]);
	cairo_paint(cr);
	cairo_fill(cr);
	cairo_stroke(cr);
	if(coords_before) show_coords(cr);
#if	0
	/* title  */
	cairo_move_to (cr, 10.0, 40.0);
	cairo_set_source_rgb(cr,0,0,0);
	cairo_select_font_face(cr,"Menlo",CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr,18);
//    cairo_show_text (cr, "Καλημέρα, κόσμε!");
	if(splot_title!=NULL) cairo_show_text(cr,splot_title);
#endif
	draw_objects2(cr);
	draw_objects(cr);
	if(!coords_before) show_coords(cr);
    //Destroy the cairo context and/or flush the destination image
    cairo_destroy(cr);
	redrawing=0;
}

void set_default_matrix(cairo_t *cr)
{
	set_ratio();

	rx0 = ratx * (-xmin);
	ry0 = ratx * (-ymin);

	cairo_identity_matrix(cr);
//	cairo_scale(cr,1/scalex,1/scalex);
//	cairo_rotate(cr, angle1*3.14/180);
	current_angle=0;
	current_scale[0]=1;
	current_scale[1]=1;
}


void on_key_press(GtkWidget *widget, GdkEventKey *ev, gpointer user_data)
{
 int n=0;
 int ks;
 if(widget==NULL) return;
 GtkWidget *darea = GTK_WIDGET(user_data);
 ks = ev->keyval;
 n = ev->length;
// printf("key %d\n",ks);
 	switch(ks) {
		case CHR_LBRA: 
//			printf("rotate right\n");
			angle1 -= 5;
			if(angle1<0) angle1+=360;
			plot_redraw();
			break;
		case CHR_RBRA: 
//			printf("rotate left\n");
			angle1 += 5;
			if(angle1>360) angle1-=360;
			plot_redraw();
			break;
		case 65361:
//			posx1 += 5;
			plot_move(0);
			plot_redraw();
			break;
			plot_move(1);
		case 65363:
//			posx1 -= 5;
			plot_move(2);
			plot_redraw();
			break;

		case 65362:
//			posy1 += 5;
			plot_move(1);
			plot_redraw();
			break;
		case 65364:
//			posy1 -= 5;
			plot_move(3);
			plot_redraw();
			break;
		case 'f':
			if(show_frames) show_frames=0;
			else show_frames=1;
			plot_redraw();
			break;
		case 'F':	/* toggle flag fill  */
			if(flag_fill) flag_fill=0;else flag_fill=1;
			plot_redraw();
			break;
		case 'q':
			plot_close(1);
			break;
		case '+':
		case '=': plot_zoom(1); break;
		case '-': plot_zoom(-1); break;
		case 'g': toggle_gridmode();plot_redraw(); break;
		case 'n': toggle_nummode();plot_redraw(); break;
		case 'z': toggle_zoom();plot_redraw();break;
		case 'c': toggle_center();break;
		case 'm': toggle_mid();plot_redraw();break;
		case 'o': toggle_origin();plot_redraw();break;
		case '?': show_values();break;
		case 'l': plot_redraw();break;
	};
}

void paint_elipsep(cairo_t *cr, double angle,double rx,double ry)
{
    double x, y;
    int i_theta;
	double ARC_FINE=1;
//	printf("elipse: a=%f x=%f y=%f\n",angle,rx,ry);
	cairo_save(cr);
//	cairo_translate(cr,dtransx(plot_x),dtransy(plot_y));
//	cairo_move_to(cr,dtransx(plot_x),dtransy(plot_y));
//	cairo_translate(cr,dtransx(-rx/2),dtransy(ry/2));
	cairo_translate(cr,dtransx(plot_x),dtransy(plot_y));
	cairo_rotate(cr,angle*G_PI/180.0);
#if	0
	/* defining circle  */
	cairo_arc(cr,0,0,rx/scalex,0,2*G_PI);
#endif
	cairo_scale(cr,1,rx/ry);
	cairo_arc(cr,0,0,rx/scalex,0,2*G_PI);

	cairo_stroke(cr);
	cairo_restore(cr);
}

void paint_rectangle(cairo_t *cr, double *v)
{
	double offsx,offsy;

	double xo=0;
	double yo=0;
	double x,y;

	if(mid_flag) {
		offsx=-(v[0]/2);
		offsy=+(v[1]/2);
	} else {
		offsx=0;
		if(plot_origin==0) offsy=0;
		else	offsy=v[1];
	};
	xo=dtransx(plot_x);
	yo=dtransy(plot_y);
	x=dtransx(plot_x+offsx);
	y=dtransy(plot_y+offsy);

	set_stroke_color(cr);
	cairo_save(cr);

	cairo_translate(cr, xo, yo);
//	cairo_rotate(cr,angle1*3.14/180);
	cairo_rotate(cr,current_angle*3.14/180);
	cairo_scale(cr,current_scale[0],current_scale[1]);	/*   */
	cairo_translate(cr, -xo, -yo);

	cairo_rectangle(cr,x,y,v[0]/scalex,v[1]/scalex);
	if(flag_fill) {
		cairo_stroke_preserve(cr);
		set_fill_color(cr);
		cairo_fill(cr);
	} else {
		cairo_stroke(cr);
	};
	cairo_restore(cr);
}

void paint_polyline(cairo_t *cr, plot_element *pe)
{
 double xo,yo;
 int i=0;
 double *v=pe->v;
 int num=v[0];
 double x1=0,y1=0;
 xo=dtransx(plot_x);
 yo=dtransy(plot_y);
 cairo_move_to(cr,xo,yo);
 for(i=1;i<num;i+=2) {
 	x1=dtransx(v[i]);
	y1=dtransy(v[i+1]);
	cairo_line_to(cr,x1,y1);
	xo=x1;yo=y1;
 }; 
 if(!pe->stroke) {
 	cairo_close_path(cr);
	set_fill_color(cr);
	cairo_fill(cr);
 } else {
 if(pe->closed) {
	cairo_close_path(cr);
	if(flag_fill) {
	 	cairo_stroke_preserve(cr);
		set_fill_color(cr);
		cairo_fill(cr);
	 } else  cairo_stroke(cr);
 } else {
 	cairo_stroke(cr);
 };
 };
}

void paint_triangle(cairo_t *cr, double *v)
{
	double offsx=0,offsy=0;

	double xo=0;
	double yo=0;
	double x,y;

	xo=dtransx(plot_x);
	yo=dtransy(plot_y);
	x=dtransx(plot_x+offsx);
	y=dtransy(plot_y+offsy);

	set_stroke_color(cr);
	cairo_save(cr);

	cairo_translate(cr, xo, yo);
	cairo_rotate(cr,angle1*3.14/180);
//	cairo_rotate(cr,current_angle*3.14/180);
	cairo_scale(cr,current_scale[0],current_scale[1]);	/*   */
	cairo_translate(cr, -xo, -yo);

	cairo_move_to(cr,x,y);
	cairo_line_to(cr,dtransx(v[0]),dtransy(v[1]));
	cairo_line_to(cr,dtransx(v[2]),dtransy(v[3]));
	cairo_line_to(cr,x,y);

	if(flag_fill) {
		cairo_stroke_preserve(cr);
		set_fill_color(cr);
		cairo_fill(cr);
	} else {
		cairo_stroke(cr);
	};
	cairo_restore(cr);
}

#define PNGLOAD	0
// paint rotated image around upper left corner
void paint_image(cairo_t *cr, char *name, int px,int py,double obj_scale,double angle_rads)
{
	char st[164];
	double scale=obj_scale/scalex;
#if	!PNGLOAD
	GError *gerr = NULL;
//	GdkPixbuf *img = gdk_pixbuf_new_from_file(name,gerr);
	GdkPixbuf *img = NULL;
	img = gdk_pixbuf_new_from_file(name,&gerr);
#else
    cairo_surface_t *img = cairo_image_surface_create_from_png(name);
#endif
	if(img==NULL) { 
		ERROR("cannot open image %s to draw",name);
		return;
	};
	double angle = angle_rads * 3.14/180;
	double a0 = 45*3.14/180;
#if	!PNGLOAD
	int width = gdk_pixbuf_get_width(img);
	int height = gdk_pixbuf_get_height(img);
#else
	int width = cairo_image_surface_get_width(img);
	int height = cairo_image_surface_get_height(img);
#endif
	double x=0,y=0;

	double offsx=0;
	double offsy=0;
	double xo=0;
	double yo=0;

// 	set_default_matrix(cr);
	if(mid_flag) {
		offsx=-(width/2)*scalex;
		offsy=+(height/2)*scalex;
	} else {
		offsx=0;
		if(plot_origin==0) offsy=0;
		else offsy=height*scalex;
	};

	xo=dtransx(px);
	yo=dtransy(py);
	x=dtransx(px+offsx);
	y=dtransy(py+offsy);

	if(show_frames) {
		cairo_save(cr);
		cairo_set_source_rgb(cr, 0.50, 0.0, 0.0);
		cairo_set_line_width(cr, 0.5);

		cairo_translate(cr,xo,yo);
		cairo_scale(cr,scale,scale);
//		cairo_rotate(cr,angle1*3.14/180);
		cairo_rotate(cr,angle);
		cairo_translate(cr,-xo,-yo);

		cairo_rectangle(cr,x,y,width,height);
		cairo_stroke(cr);
		cairo_restore(cr);
//	 	set_default_matrix(cr);
	}
	cairo_save(cr);
		cairo_translate(cr,xo,yo);
		cairo_scale(cr,scale,scale);
//		cairo_rotate(cr,angle1*3.14/180);
		cairo_rotate(cr,angle);
		cairo_translate(cr,-xo,-yo);
#if	!PNGLOAD
		gdk_cairo_set_source_pixbuf(cr,img,x,y);
#else
		cairo_set_source_surface(cr,img,x,y);
#endif
		cairo_paint_with_alpha(cr,alpha_value);
	cairo_restore(cr);
// 	set_default_matrix(cr);
}

/* redraw plot elements  */
void draw_objects2(cairo_t *cr)
{
 plot_element *pe;
 double *v;
 int ielem;
 int n1=0;
 double x,y,xx0,xx1,yy0,yy1,r1;
 int ix0,iy0,ix1,iy1;
 char st[128];
 static double x0,y0,x1,y1;
 int path_on=0;

 plot_sync=1;
 redrawing_on=1;

 pe = plist_start;
 set_fill_color(cr);
 set_stroke_color(cr);
 cairo_stroke(cr);
 if(pe->next == (plot_element *)NULL) return;
 do {
	pe = pe->next;
	v=pe->v;
	switch (pe->type) {
		case 0: break;
		case PPOINT: {
			 x=v[0];y=v[1];
			 xx0 = dtransx(x);
			 yy0 = dtransy(y);
			 cairo_set_line_cap  (cr, CAIRO_LINE_CAP_ROUND);
			 draw_line(cr, xx0,yy0,xx0,yy0);
			 cairo_stroke(cr);
			 plot_x=x;plot_y=y;
		};break;
		case PLWIDTH: {
			line_width=v[0];
			if(scalex>1) cairo_set_line_width(cr,line_width/scalex);
			else cairo_set_line_width(cr,line_width);
		};break;
		case PCOLOR: {
			double r,g,b;
			stroke_color[0]=r = v[0]/256.0;
			stroke_color[1]=g = v[1]/256.0;
			stroke_color[2]=b = v[2]/256.0;
			cairo_set_source_rgb(cr,r,g,b);
		};break;
		case PFCOLOR: {
			double r,g,b,a;
			fill_color[0]=r = v[0]/256.0;
			fill_color[1]=g = v[1]/256.0;
			fill_color[2]=b = v[2]/256.0;
			fill_color[3]=a = v[3]/256.0;
			cairo_set_source_rgba(cr,r,g,b,a);
		};break;
		case PLINE:
			draw_line(cr,dtransx(v[0]),dtransy(v[1]),dtransx(v[2]),dtransy(v[3]));
			if(!path_on) {
				cairo_stroke(cr);
			 	plot_x=v[2];plot_y=v[3];
			};
			break;
		case PLINETO:
			if(!path_on) {
				draw_line(cr, dtransx(plot_x),dtransy(plot_y),dtransx(v[0]),dtransy(v[1]));
				cairo_stroke(cr);
				 plot_x=v[0];plot_y=v[1];
			 } else {
			 	if(relative_flag) {
					cairo_line_to(cr,dtransx(plot_x+v[0]),dtransy(plot_y+v[1]));
				} else {
			 		cairo_line_to(cr,dtransx(v[0]),dtransy(v[1]));
					plot_x=v[0];plot_y=v[1];
				}
			 };
			break;
		case PELIPSE:
			paint_elipsep(cr,v[0],v[1],v[2]);
			break;
		case PCIRCLE:
			set_stroke_color(cr);
			cairo_arc(cr,dtransx(v[0]),dtransy(v[1]),v[2]/scalex,0,2*3.14);
			if(flag_fill) {
				cairo_stroke_preserve(cr);
				set_fill_color(cr);
				cairo_fill(cr);
			} else {
				cairo_stroke(cr);
			};
			plot_x=v[0];plot_y=v[1];
			break;
		case PRCIRCLE:
			if(flag_fill) {
			set_fill_color(cr);
			cairo_arc(cr,dtransx(plot_x),dtransy(plot_y),v[0]/scalex,0,2*3.14);
				if(!path_on) {
					cairo_fill_preserve(cr);
					set_stroke_color(cr);
				}
			};
//			cairo_set_line_width(cr,5);
			cairo_arc(cr,dtransx(plot_x),dtransy(plot_y),(v[0]-line_width/2)/scalex,0,2*3.14);
			if(!path_on) cairo_stroke(cr);
			cairo_move_to(cr,dtransx(plot_x),dtransy(plot_y));
			break;

		case PARC:
			 xx0 = dtransx(plot_x);
			 yy0 = dtransy(plot_y);
			if(flag_fill) {
			
			};
			cairo_arc(cr,xx0,yy0,v[0]/scalex,v[1],v[2]);
			if(!path_on) cairo_stroke(cr);
			break;
		case PFILL:
			flag_fill = v[0];
			break;
		case PIMAGE:
			paint_image(cr,(char *)&v[3],plot_x,plot_y,v[0],v[1]);
			break;
		case PALPHA:
			alpha_value=v[0]/100;
			break;
		case PROTATE:
//			cairo_rotate(cr,v[0]);
			current_angle=v[0];			
			break;
		case PSCALE:
			current_scale[0]=v[0];
			current_scale[1]=v[1];
			break;
		case PTRANSLATE:
			cairo_translate(cr,v[0]/scalex,-v[1]/scalex);
			cairo_save(cr);
			break;
		case PRECTANGLE:
			paint_rectangle(cr,v);
			break;
		case PTRIANGLE:
			paint_triangle(cr,v);
			break;
		case PPOLY:
			paint_polyline(cr,pe);
			break;
		case PRELATIVE:
			relative_flag=1;
			break;
		case PABSOLUTE:
			relative_flag=0;
			break;
		case PCURVETO:
			if(!path_on) {
				set_stroke_color(cr);
				cairo_move_to (cr,dtransx(plot_x),dtransy(plot_y));
			};
			if(relative_flag) {
				cairo_curve_to(cr,dtransx(plot_x+v[0]),dtransy(plot_y+v[1]),dtransx(plot_x+v[2]),dtransy(plot_y+v[3]),dtransx(plot_x+v[4]),dtransy(plot_y+v[5]));
//				cairo_line_to(cr,dtransx(plot_x+v[0]),dtransy(plot_y+v[1]));
			} else { 
				cairo_curve_to(cr,dtransx(v[0]),dtransy(v[1]),dtransx(v[2]),dtransy(v[3]),dtransx(v[4]),dtransy(v[5]));
//				cairo_line_to(cr,dtransx(v[0]),dtransy(v[1]));
			}
			if(!path_on) cairo_stroke(cr);
#if	0
			cairo_set_source_rgb(cr,0,200,0);
			cairo_move_to (cr,dtransx(plot_x),dtransy(plot_y));   cairo_line_to (cr,dtransx(v[0]),dtransy(v[1]));
			cairo_move_to (cr,dtransx(v[2]),dtransy(v[3])); cairo_line_to (cr,dtransx(v[4]),dtransy(v[5]));			
			cairo_stroke(cr);
#endif
		case PMOVE:
			plot_x=v[0];
			plot_y=v[1];
//			cairo_move_to(cr,dtransx(plot_x),dtransy(plot_y));
			break;
		case PPATH:
			path_on=1;
			cairo_save(cr);
			cairo_new_sub_path(cr);
			cairo_translate(cr,dtransx(plot_x),dtransy(plot_y));	/* rotate center  */
//			cairo_rotate(cr,current_angle*3.14/180);
			cairo_rotate(cr,(angle1+current_angle)*3.14/180);
			cairo_translate(cr,-dtransx(plot_x),-dtransy(plot_y));
			cairo_move_to(cr,dtransx(plot_x),dtransy(plot_y));	/* starting path origin  */
			break;
		case PTEXT:
			{int font_size;
			font_size=16/scalex;
			set_fill_color(cr);
			cairo_move_to(cr,dtransx(plot_x),dtransy(plot_y));
			cairo_set_font_size(cr,font_size);
			cairo_show_text(cr,pe->pe_name);
//			set_fill_color(cr);
//			set_stroke_color(cr);
			cairo_move_to(cr,dtransx(plot_x),dtransy(plot_y));	/* do not move ??  */
			};break;
		case PEND:
			path_on=0;
			cairo_close_path(cr);
			set_fill_color(cr);
			cairo_fill_preserve(cr);
			set_stroke_color(cr);
			cairo_stroke(cr);
			cairo_restore(cr);
			break;
//		case PTEXT:
 	};
 
 } while ( pe->next != (plot_element *)NULL);
}

void draw_objects(cairo_t *cr)
{
  int i, j;

// set default color,line width
  cairo_set_line_width(cr, 0.5);
  cairo_set_source_rgb(cr, .1, .5, 0.1);
  if(glob.count>1) {
  cairo_move_to(cr, glob.coordx[0], glob.coordy[0]);
	  for (i = 1; i < glob.count; i++ ) {
	          cairo_line_to(cr, glob.coordx[i], glob.coordy[i]);
	  }
  }

  cairo_stroke(cr);    
}



static gboolean clicked(GtkWidget *widget, GdkEventButton *event)
{
    if (event->button == 1) {
        glob.coordx[glob.count] = event->x;
        glob.coordy[glob.count] = event->y;
		printf("add point[%d] (%f %f)\n",glob.count,event->x, event->y);
		glob.count++;
		if(glob.count > 1) gtk_widget_queue_draw(widget);
    }

    if (event->button == 3) {
        gtk_widget_queue_draw(widget);	/* refresh ...  */
    }

    return TRUE;
}

int plot_on()
{
 if(wplotp==NULL) return 0;
 else return 1;
}

int plot_init(num n)
{
  GtkWidget *vbox1, *hbox,*hbox1;
  GtkWidget *toolbar_plot;

  char title[MAXLLEN];
  strlcpy(title,kt_getstr("init title"),MAXLLEN);

  if(wplotp==NULL) {
	ratx=1.0;
	rx0=0.0;ry0=0.0;
	
	glob.count = 0;
	
	wplotp = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	plot_width = 550;
	plot_height = 350;
	
	vbox1 = gtk_vbox_new (FALSE, 0);
	gtk_widget_set_usize(wplotp, plot_width+32,plot_height+16);
	gtk_widget_show(vbox1);
	gtk_container_add(GTK_CONTAINER(wplotp), vbox1);
	
	hbox = gtk_hbox_new (FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox1),hbox, TRUE,TRUE,0);
	gtk_widget_show(hbox);
	
	toolbar_plot=new_toolbar(wplotp,hbox, tbar_plot,1,1,0);
	
	hbox1 = gtk_hbox_new (FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox1),hbox1,FALSE,TRUE,0);
	gtk_widget_show(hbox1);
	// status line
	wplot_sline = gtk_statusbar_new();
	gtk_widget_show(wplot_sline);
	gtk_box_pack_start(GTK_BOX(hbox1),wplot_sline,TRUE,TRUE,0);
	
	gtk_widget_show(vbox1);
	
	wplot_pline = gtk_statusbar_new();
	gtk_widget_show(wplot_pline);
	gtk_box_pack_start(GTK_BOX(hbox1),wplot_pline,TRUE,TRUE,0);
	gtk_widget_show(wplot_pline);
	
	darea = ge_plot_area_new();
	
	gtk_box_pack_start(GTK_BOX(hbox), darea, TRUE,TRUE,0);
	gtk_widget_realize(darea);
	gtk_widget_show(darea);
	
	g_signal_connect(G_OBJECT(darea), "expose-event", G_CALLBACK(on_draw_event), NULL); 
	g_signal_connect(G_OBJECT(wplotp), "key_press_event", G_CALLBACK(on_key_press), darea); 
	
	gtk_signal_connect_object(GTK_OBJECT(wplotp), "destroy", (GtkSignalFunc) plot_close, GTK_OBJECT(wplotp));
	
	g_signal_connect(darea, "button-press-event",  G_CALLBACK(clicked), NULL);
	g_signal_connect(darea, "motion_notify_event", G_CALLBACK(on_plot_motion), GTK_OBJECT(darea));
	
	gtk_window_set_title(GTK_WINDOW(wplotp), "Lines");
	
	gtk_widget_show_all(wplotp);
	
	// initialize corresponding data
	plot_is_maped=1;
	scalex=1.0;
	plot_x=plot_y=0;	/* current plot position  */
	xmin=ymin=0;
	xmax=plot_width;
	ymax=plot_height;
	alpha_value=1.0;
  } else {;
	free_plot_list();
  };

  new_plot_list();
#if	1
 {
 double *v;
 int size;
 size=strlen(title)+1;
 gtk_window_set_title(GTK_WINDOW(wplotp),title);
 splot_title=strdup(title);
 v = add_plot_element(PTITLE,1+size/sizeof(double) );
 strlcpy((char *)v, title,size);
 };
#endif
 return true;
}

/* -- */
