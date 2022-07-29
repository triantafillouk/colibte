/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

*/
/* plot structures */

typedef struct plot_element
{
 unsigned int type:8;
 unsigned int fill:1;	/* fill it  */
 unsigned int stroke:1;	/* show stroke  */
 unsigned int closed:1;	/* close it  */
 unsigned int selected:2;
 unsigned int pshown:1;
 unsigned int relative:1;	/* relative to current point or absolute  */
 unsigned int subtype:4;
// unsigned int id;	/* ??  */
 char *pe_name;
 double *v;/* the first element can be the size of the data array  */
 struct plot_element *next;
} plot_element;

// element types
#define	NONE	0
#define	PPOINT	1		/* x1,y1 size */
#define PLINE	2		/* x0,y0 x1,y1  */
#define PLINETO	3		/* x1 y1  */
#define	PTRANSLATE	4	/* translate  */
#define	PPIE	5		/* x0,xy,r, type, list  */
#define	PTRIANGLE	6	/* x0,y0 x1,y1 x2,y2 */
#define	PRECTANGLE	7	/* x0,y0 x1,y1  */
#define PALPHA	8		/* alpha  */
#define	PARC	9		/* x1,y1,r,a0,a1  */
#define PCIRCLE		10	/* x1,y1,r */
#define PRCIRCLE	11	/* r */
#define PPOLY		12	/* x0 y0, x1 y1, ...  */
#define	PCURVETO	13	/* curve to  */
#define	PCOORDS		14	/* origin x,y, tics,grid  */
#define	PCOLOR		15	/* r g b a */
#define	PFCOLOR		16	/* rgba fill color */
#define PELIPSE		17	/* x1,y1,r1,r2,angle */
#define	PROTATE		18	/* rotate  */
#define PTITLE		19	/* string  */
#define PMOVE		20	/* x1,y1  */
#define	PSCALE		21	/* scale  */
#define	PTEXT		22	/* x,y plot string */
#define PFONT		23	/* name, font size, bold */
#define PSETCOLOR	24	/* set color of type  */
#define PANGLE		25	// plot angle
#define PIMAGE		26	/* x,y name,zoom,angle */
#define PITEM		27	// plot item structure
#define PIZOOM		28	// set item zoom
#define PIANGLE		29	// set item angle
#define PCONNECT	30	// connect items
#define PCONNECTION	31	// connection
#define PFILL		32	/* fill_flag (on,off), transparency */
#define PLWIDTH		33	// Line width
#define	PPATH		34	/* path start  */
#define	PEND		35	/* path end  */
#define	PRELATIVE	36
#define	PABSOLUTE	37
#define	PFRAMES		38	/* show frames  */

/* Plot modes */
#define PLOT_MODE_SELECT 1
#define PLOT_MODE_SPOINT 2
#define PLOT_MODE_GPOINT 3
#define PLOT_MODE_DISTANCE	4
#define PLOT_MODE_RECTANGLE 5
#define PLOT_MODE_CIRCLE	6
#define PLOT_MODE_ARC	7
#define	PLOT_MODE_LINE	8
#define PLOT_MODE_CONNECT 9
#define PLOT_MODE_POLY_OPEN	10
#define PLOT_MODE_POLY_CLOSED 11
#define PLOT_MODE_ARROW	12
#define PLOT_MODE_ARROW_CONNECT 13
#define PLOT_MODE_ECLIPSE	14
#define PLOT_MODE_ROTATE	15
#define PLOT_MODE_SCALE	16
#define PLOT_MODE_GRID_SNAP	17
#define PLOT_MODE_SCALED_LINE	19

/* Mark types */
#define PLOT_MARK_DOT1	1
#define PLOT_MARK_DOT2	2
#define PLOT_MARK_CIRCLE1	3
#define PLOT_MARK_CIRCLE2	4
#define PLOT_MARK_STAR	5
#define PLOT_MARK_TRIANGLE	6
#define PLOT_MARK_RECTANGLE	7

#define PORIGIN_UL	0	/* upper left  */
#define	PORIGIN_BL	1	/* bottom left  */

char *type_name[] = {
 "none","point","line","lineto","translate","pie","triangle","rectangle",
 "alpha","arc","circle","circle relative",
 "poly","curveto","coords","color","fill color",
 "elipse","rotate","title","move to","scale","text",
 "font","set color","angle","image","item","item zoom","item angle",
 "item connect","connection type","fill","line width","path start","path end",
 "relative","absolute","show frames",
 NULL
};

double color_back[3]={0.5,0.5,0.5};
double color_grid[3]={0.9,0.9,0.9};
double color_coord[3]={0.1,0.1,0.1};
double color_tik[3]={0,0,0};
double color_title[3]={0.1,0.1,0.1};

int plot_origin = PORIGIN_UL;
int plot_coordinates = PORIGIN_BL;
int relative_flag=0;
int show_frames=0;

plot_element *new_plot_element();
double *add_plot_element(int type, int size);
void free_plot_list();
plot_element *new_plot_element();
void new_plot_list();
void plot_redraw();
void do_timer_plot();
