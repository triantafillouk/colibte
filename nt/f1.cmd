# test loop with proc inside
cls
print("--- f1.cmd")
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

i=1
a=fact(i+2)	:   6.0
fact(3)	:   6.0
print("fact(",i+2,") is ",a)


