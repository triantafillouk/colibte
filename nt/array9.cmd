
print("--- array9.cmd")
function sc(ind)
{
 scale=[14000, 4; 3000, 20; 3000, 35; 5000, 37; 100000, 20]
 ptotal=0
 print(scale) 
 ptotal += scale[ind][0]
 return ptotal
}

"start of test!":
 scale1=[14000-2, 4+3.1;  3000, 20*3+1;  3000, 35;  5000, 37;  100000, 20]
"Show the array":
 scale1:

 print("----------")

 a=scale1[1][1]:
 a:
sc(3):
"end of test!":


