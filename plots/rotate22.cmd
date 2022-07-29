
plot_init("images..")

x=50
y=200
#translate(100,200)
line_width(10)
stroke_color(200,0,0)
fill_color(200,200,200,150)
point(x,y)
image("heart.png",0.5,45)
point(x+100,y)
image("../../Pictures/p2.jpg",0.5,10)
line_width(10)
stroke_color(0,0,200)
point(x,y)

x=x+150
#y=y+50

line_width(10)
stroke_color(200,0,0)
point(x,y)
set_alpha(50);
image("heart.png",1,45)
line_width(10)
stroke_color(0,0,200)
point(x,y)

x=x+150
#y=y+50

line_width(10)
stroke_color(200,0,0)
point(x,y)
set_alpha(80)
image("heart.png",1.1,45)
line_width(10)
stroke_color(0,0,200)
point(x,y)

line_width(1)
point(50,200)
rectangle(200,100)
scale(2,0.5)
rotate(45)
rectangle(200,100)
