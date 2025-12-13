
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

