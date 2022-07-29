# plot 
a=10
b=11
a1=30
b1=40	
plot_init("g function")
plot_resize(400,450)
plot_coord(-40,200,-40,200)

#getchar()
#plot_line(a,b,a1,b1)
i=0
while (i<5)
{
	a=a1
	b=b1
	a1 = a1+10
	b1 = b1+15*i
	i	: 4.000000
	stroke_color(100*i,20,30)
	line(a,b,a1,b1)
	i=i+1
}
