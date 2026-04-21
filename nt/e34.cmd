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

fact(3):     6

p=0; l=0; 

show_time("Loop with proc 51",0)

fori(i=1;11;1){ 
	fori(j=1;800001;1) { 
		l = i*j+32
		p = p+l+fact(10)
	} 
	show_time("loop:"+i+" p="+p,1)
}

tdif=show_time("End test",2)
print(": test concluded in "+tdif+" seconds")
