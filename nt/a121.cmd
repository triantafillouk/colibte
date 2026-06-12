cls
print("---- a121.cmd")
function fact(num)
{
 if(num<1) 
 {
 	return(1)
 } else {
 	n1=num-1
	f=num*fact(n1)
	return(f)
 }
}

fact(3):
a=10
#s="fact of "+a," is "+fact(a)
"fact of "+a:
#print(s)
