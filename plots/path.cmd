# path example

plot_init("path example")
title("path example 1")

fill_color(200,50,50,200)
stroke_color(50,200,50)
absolute
point(100,100)
rcircle(100)
fill_color(20,50,250,200)
move_to(100,200)	# rotation center
text("hello!")
move_to(100,150)
fill_color(20,250,50,100)
rotate(90)
path("path1")
relative
 fill_color(200,0,0,255)
 text("path1")
 fill_color(20,250,50,100)
 stroke_color(20,20,250)
 line_to(200,200)
# rcircle(100)
 line_to(250,160)
 line_to(300,100)
 curve_to(200,00,100,00,100,50)
pend
absolute
rotate(10)
move_to(100,-50)
path("path2")
	fill_color(200,150,50,100)
	relative
 	line_to(100,0)
	line_to(100,-100)
	line_to(0,-100)
pend
