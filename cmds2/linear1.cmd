
cls

l1=183
l2=52
xmin=2
ymin=1
max=800
xmax=5
zmin=max
if(xmax*l1 > max) {
	xmax=trunc(max/l1)
}
4*183+52: 784.0
for(x=xmin;x<xmax ;x=x+1)
{
 # print("x="+x)
 y1=(max-x*l1)/l2
 ty1=trunc(y1)
 z=y1-ty1
 if(z<=zmin) {
 	print("!! x="+x+" y1="+ty1+" z="+trunc(z*l2))
 	zmin=z
 } else {
 	print("   x="+x+" y1="+ty1+" z="+trunc(z*l2))
 }
}

