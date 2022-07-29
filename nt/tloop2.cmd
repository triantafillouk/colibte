#check double loop, speed test 1

cls

print(_time)	: [Fri Oct  3 15:47:31 2014]
show_time("Loop test while/for 1",0)

i=l=p=0

while (i<10)
{
	for (j=1;j<800000;j=j+1)
	{ 
		l = i*j + 32
		p = p+l
	}
#	print("loop:"+i+" p="+p)
	show_time("loop "+i,1)
	i=i+1
}
_time	: [Fri Oct  3 15:47:33 2014]

message("i<"+i+" and j< "+j+" sets p="+p)
print(_time)	: [Fri Oct  3 15:47:33 2014]
tdif=show_time("End test",2)
print("Test concluded in "+tdif+" seconds")
