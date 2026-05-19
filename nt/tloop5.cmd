# 
initialize
# CHECK!!!!!
cls

i=p=0

print(_time)	: [Thu Sep 25 16:59:11 2014]
show_time("Start test",0)
l=0
p=0
print("-----------")
for(k=0;k<5;k=k+1)
{
print("k loop "+k)
i=0
while (i<10)
{
	#print("	i loop "+i)
	for (j=1;j<5;j=j+1)
	{ 
		print(" j for loop "+j)		
		p = p+sin(i*j + 32)
		if(j>2) break
		# l=0 
	}
	print("	iloop:"+i+" p="+p)
#	show_time("loop "+i,1)
	i=i+1
	if(i>3) break
}
 print("k="+k)
}
_time	: [Thu Sep 25 16:59:11 2014]

message("End result: i<"+i+" and j< "+j+" sets p="+p)
print("End time is:",_time)	
tdif=show_time("End test",2)
print("Test concluded in "+tdif+" seconds")
p: < 1370 | 0x55A | 0o2532>
#25.39,25.387
