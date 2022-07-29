
plot_init("rotate2 new")

x=50
y=200
#plot_translate(100,200)
line_width(10)
stroke_color(200,0,0)
point(x,y)
image("heart.png",0.5,45)
image("/Users/kt/Downloads/images/kerkura-blaxerna-pontikonisi.jpg",0.5,0)
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
