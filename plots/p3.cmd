
plot_init("p3")
#plot_coord(-100,100,-100,100)
fill_color(100,200,150,200)
i=1	: <    1 | 0x1 | 0o1>
x=30	: <   30 | 0x1E | 0o36>
y=30
while (i<6)
{
	r=i*30
	stroke_color(r,100,100)
	circle(x,y,30)
	line(x,y,130,130)
	i=i+1
	x=x+20
	y=y+30
}

point(50,50)
