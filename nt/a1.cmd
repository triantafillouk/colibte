cls
max1=20000000
print("Initializing arrays with ",max1," elements")
print("")
show_time("start allocate/define array a",0)
# defining the max index of the array at the beggining is faster!
# Loop to check allocation one item per time
print("starting loop")
fori(i=0;max1;1)
{
	a[i]=1.1+i
}
show_time("array a allocated/defined!",1)

print("defining the max element of the array at the begining is faster!")
show_time("start allocate array b",0)
b[max1]=0
show_time("array b allocated, start defining!",1)
# Loop to check allocation one item per time
fori(i=0;max1;1)
{
	b[i]=1.1+i
}
show_time("array b allocated and defined!",1)



