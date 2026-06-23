cls
print("--- factorial1.cmd")
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

fori(i=1;25;1){
	print("factorial(",i,")=",fact(i))
}

