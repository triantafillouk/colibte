# checks break inside a block

cls()
for(j=0;j<10;j=j+1)
{
 print("j="+j)
 if(j>3) 
 {
 	print("j is bigger than 3 , it is "+j)
	break
 };
}

print("after for loop, j="+j)

i=10
while(i>0)
{
 if(i<5)
 {
 	break
 };
 print("while loop i="+i)

 i=i-1
}

print("end of test, i="+i+" j="+j)
if((j==4) & (i==4)) {print("test ok!")}
