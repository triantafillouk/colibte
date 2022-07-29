# speed test 3 with a function inside the loop

cls

print(_time)	: "Thu Aug 13 14:18:00 2015"
show_time("Loop test while/for sin 3",0)

i=l=p=0

while (i<10)
{
	for (j=1;j<800000;j=j+1)
	{ 
		l = i*j + 32.1+sin(0.3)
		p = p+l
	}
#	print("loop:"+i+" p="+p)
	show_time("loop "+i,1)
	i=i+1
}
_time	: "Thu Aug 13 14:18:02 2015"

message("i<"+i+" and j< "+j+" sets p="+p)
print(_time)	: "Thu Aug 13 14:18:02 2015"
tdif=show_time("End test",2)
print("Test concluded in "+tdif+" seconds")
