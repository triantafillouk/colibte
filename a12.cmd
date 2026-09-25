cls
print("---- a12.cmd")

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

fact(3):   6.0

# print("fact of ",a," is ",fact(a++))
