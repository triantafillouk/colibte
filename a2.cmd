cls
# print("--- factorial1.cmd")
function fact(num)
{
 if(num<2) 
 {
 	return(1)
 } else {
	f=num*fact(num-1)
	return(f)
 }
}

fact(3):

