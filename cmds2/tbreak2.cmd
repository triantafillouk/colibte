# Check break from inside loop

cls
print("after for loop, i="+i)

i=10
while(i>0)
{
 if(i<5)
 {
 	break
 }
 print("while loop i="+i)

 i=i-1
}

print("end of test, i="+i)
if(i==4) print("test ok!")
