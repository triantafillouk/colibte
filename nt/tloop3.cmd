# speed test 2 with double fori loops

cls

print(_time)	: "Sat Oct 18 05:35:09 2014"
show_time("Start test 2 fori",0)

l=p=0

fori (i=0;10;1)
{
	fori (j=1;800000;1)
	{ 
		l = i*j + 32
		p = p+l
	}
#	print("loop:"+i+" p="+p)
	show_time("loop "+i,1)
}
_time	: "Sat Oct 18 05:35:11 2014"

message("i<"+i+" and j< "+j+" sets p="+p)
print(_time)	: "Sat Oct 18 05:35:11 2014"
tdif=show_time("End test",2)
print("Test concluded in "+tdif+" seconds")
