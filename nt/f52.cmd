# f5.cmd

cls
print("# ---- f52.cmd ----")

show_time("Start fori test loop",0)
m=0
a=0
max=1000000
inc=0.01
fori(m=0;max+inc;inc)
{
	a+=sin(m)
	# print("m=",m," sin=",cos(m)," a=",a)
}
a: 
print("sum1 is ",a)
tdif=show_time("End loop 1",1)
b=0
m=0
max1=max*100+inc*100-1
fori(i=0;max1;inc*100)
{
	b+=sin(m+=inc)
	# print("m=",m," sin=",cos(m)," b=",b)
}
b: 
print("sum2 is ",b)
tdif=show_time("End loop 2",2)
