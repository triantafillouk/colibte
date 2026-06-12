# check loops with break 

cls
print("--- tloop12.cmd")
print(_time)	: [Wed Oct  1 23:28:47 2014]
show_time("Start test loop break",0)

l=p=i=0

while (i<10)
{
	for (j=1;j<10;j=j+1)
	{ 
		l = i*j + 32
		p = p+l
		if(j>5) break 
		# print("--")
	}
#	print("loop:"+i+" p="+p)
	show_time("loop "+i+" p="+p,1)
	i=i+1
#	if(i>3) { print("break2");break }
	# print("end while!")
}
_time	: [Wed Oct  1 23:28:47 2014]

message("i<"+i+" and j< "+j+" sets p="+p)
print(_time)	: [Wed Oct  1 23:28:47 2014]
tdif=show_time("End test",2)
print("Test concluded in "+tdif+" seconds")
p: <301370134 | 0x11F68B16 | 0o2175505426>
#25.39,25.387
