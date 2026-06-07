cls
max1=50
print("Initializing arrays with ",max1," elements")
print("")
# defining the max index of the array at the beggining is faster!
# Loop to check allocation one item per time

show_time("start allocate/define array a",0)
fori(i=0;max1;1)
{
	a[i]=1.1+i
}
print(a)
show_time("array a allocated/defined!",1)

b[max1]=0
show_time("array b allocated, start defining!",1)
#
fori(i=0;max1;1)
{
	b[i]=1.1+i
}
show_time("array b allocated and defined!",1)
print(b)

