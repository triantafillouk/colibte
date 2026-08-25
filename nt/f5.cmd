# check function optimization

cls
print("# ---- f5.cmd ----")

show_time("Start fori test loop 1",0)
m=0
a=0
fori(i=0;100000001,1)
{
	a+=sin(m+=0.01)
	# a:
}
a:
print("sum is ",a)
tdif=show_time("End test",2)
