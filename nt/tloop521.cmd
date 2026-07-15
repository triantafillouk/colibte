# test loop with proc inside
cls
print("--- tloop52.cmd")
function fact(num)
{
 if(num<1) 
 {
 	return(1)
 } else {
	n1=num-1
	f=num*fact(n1)
	# return(num*fact(num-1))
	return(f)
 }
}

function tadd(max)
{
	k=0
	fori(i=0;max;1) k+=i
	# print(i,"k=",k)
	return(k)
}

#function showtime()
#{
#	print("time")
#}

#tadd()
#showtime

fact(3):     6

tadd(10):

p=0.3; l=0; 
print(_time)	: "Mon Dec 28 12:53:58 2015"
show_time("Loop with proc 51",0)

fori(i=1;11;1){ 
	fori(j=1;800001;1) { 
		p +=  i*j+32+tadd(130)
	} 
	show_time("loop:"+i+" p="+p,1)
}
_time	: "Mon Dec 28 12:54:09 2015"
# message("i<"+i+" and j< "+j+" sets p="+p)
print(_time)	: "Mon Dec 28 12:54:09 2015"
tdif=show_time("End test",2)
print(_version+": test concluded in "+tdif+" seconds")
