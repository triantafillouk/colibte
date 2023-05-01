
cls

l1=52
l2=183
xmin=1
ymin=1
max=800
zmin=max

for(x=xmin;x*l1<max-l2;x=x+1)
{
 # print("x="+x)
 y1=(max-x*l1)/l2
 ty1=trunc(y1)
 z=y1-ty1
 if(z<=zmin) {
 print("x="+x+" y1="+ty1+" z="+trunc(z*l2))
 zmin=z
 }
}

