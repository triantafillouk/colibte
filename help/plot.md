
# Colibri text editor ploting functionality is totally experimental!.

# Plot drawing commands

## basic plot commands
	plot_init(title), start show plot window
	title(title), title at top of the screen (also shows plot window)
	move_to(x,y)	, set current position
	line_to(x,y)	, line from current position to x,y
	circle(x,y,r) , circle at position, radius
	rcircle(r)	, circle with radius at current position
	arc(r,a1,a2) , arc with radius, from angle a1 to a2
	rectangle(w,h) , rectangle from current position
	triangle(x2,y2,x3,y3), triangle at current position
	polyline(x1,y1,x2,y2, ....), polyline at current position
	make_solid	: polyline is only filled
	poly_nostroke: >>
	plot_set_color("type",r,g,b)
		type is "grid" "coord" "tik" "back" "title"
	plot_resize(w,h)	: resize plot window to width x height
	image("filename",scale,rotate)
	text("string")
	relative	: now coords are relative
	absolute	: now coords are absolute
	plot_frames(flag) : show or not frames
	alpha,set_alpha(alpha)	, set transparency
	rotate,set_rotate(angle)	, set current rotation
	translate,set_translate(dx,dy) , set current translation
	scale,set_scale(scalex,scaley), set current scale

## path commands
	path("name")
	pmove(x,y)
	pline(x,y) : path line to
	pcurve(x1,y1,x2,y2 ..) : bezier curve to
	pend : end path
	close_path : close and end path

## interactive key commands
```
	'm'	: togle mid, togle origin in rectanges, images (mid position or corners)
	'+','=' : zoom in
	'-' : zoom out
	'f' : toggle frames flag
	'g' : grid on off
	'n' : togle num mode, numerics on coordinate axes
	'z' : togle zoom mode , zoom on window resize or stay fixed
	'o' : togle origin , changes the start corner of images,rectangles (top left or bottom left)
	'l' : redraw
```