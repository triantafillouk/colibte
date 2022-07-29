
function margie(num,len)
{
 da=360/num;
 a1=0;
 for(i=0;i<num;i=i+1) {
# 	point(100,100)
	rotate(a1)
#	line_to(100,i)
 	rectangle(len,5)
	a1=a1+da;
 }
}

function circles(num)
{
 for(i=0;i<num;i=i+1){
	move_to(i*100,100)
	fill_color(100+i*20,i*30,i*40,255)
	circle(i*100,100,50)
 };
}

function color_rect(num)
{
 rotate(10)
 for(i=0;i<num;i=i+1) {
 	for(j=0;j<num;j=j+1) {
#		print("i="+i*10+" j="+j*10)
		fill_color(i*20,j*20,100,200)
		point(i*100,j*100)
		rectangle(50,50)
 	}
 }
}

plot_init("test1")
plot_coord(-50,500,-100,500)
fill_color(200,100,100,255)

circles(5)
color_rect(10)
move_to(100,0)
margie(8,50)
move_to(200,200)
margie(10,100)
move_to(100,100)
margie(4,10)

