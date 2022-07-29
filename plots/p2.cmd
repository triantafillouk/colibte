# plot tests 
initialize()
_debug=0
x0=130
y0=130
r=10

plot_init("ttt")
plot_resize(400,600)
#getchar()
plot_coord(-40,200,-40,200)
title("this is τιτλος");
#plot_resize(350,350)
i=0
message("press any key to start plot")
#a=getchar()
message("ok!")
a	: <    0 | 0x0 | 0o0>
stroke_color(100,20,20)
line_width(1)
line(2,2,11,11)
point(11,11)
line_to(101,11)
line_to(101,101)
line_to(11,101)
line_to(11,2)
stroke_color(20,100,20)
i=0
line(0,0,20,20)
stroke_color(20,20,100)
#getchar
r	: <   10 | 0xA | 0o12>
x=30
y=20
line_width(2)
point(x,y)
while (i<24)
{
	r1=r*(1+i)
	x=x0+r1*cos(i)	: <212.581571>
	y=y0+r1*sin(i)	: <-104.906969>
	line_to(x,y)
	i=i+0.1
	x	: <212.581571>
	y	: <-104.906969>
	#getchar()
}
y0	: <  130 | 0x82 | 0o202>
r	: <   10 | 0xA | 0o12>
sin(0.5)*r	: <4.794255>
#getchar()
stroke_color(150,230,200)
point(10,10)
line_to(100,10)
line_to(100,100)
line_to(10,100)
line_to(10,1)
#getchar()
i=100
stroke_color(200,100,50)
line_width(10)
while (i<120)
{
	point(i,i)
	i=i+1
}
stroke_color(200,100,200)
line_width(5)
point(0,0)
line_to(0,150)
line_to(150,150)
line_to(150,0)
line_to(0,0)
#message("press a key to close plot")
#getchar()
#message("plot closed")
#plot_close()