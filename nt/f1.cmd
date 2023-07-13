# test loop with proc inside
cls
function fact(num)
{
 if(num<1) 
 {
 	return(1)
 } else {
	f=num*fact(num-1)
	return(f)
 }

}

i=3
a=fact(i)
print("fact("+i+") is "+a)


