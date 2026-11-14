cls
print("--- array91.cmd")

function sc(ind)
{
 scale=[14000, 4; 3000, 20; 3000, 35; 5000, 37; 100000, 20]
 scale[1][1]=ind+1.23456
 # print(scale)
 # scale:
 return scale
}

fori(i=10;200;1){
 a=sc(i)
 # a:
 print("a[1][1]=",a[1][1])
}
"end of test!":


