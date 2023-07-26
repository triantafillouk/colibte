cls
max1=8000000
print("Initializing arrays")
print("show time")
show_time("start allocate array a",0)
# defining the max index of the array at the beggining is faster!
# Loop to check allocation one item per time
print("starting loop")
for(i=0;i<max1;i=i+1)
{
	a[i]=1.1+i
}
show_time("array a allocated!",1)

print("defining the max element of the array at the beggining is faster!")
show_time("start allocate array b",0)
b[max1]=0
# Loop to check allocation one item per time
for(i=0;i<max1;i=i+1)
{
	b[i]=1.1+i
}
show_time("array b allocated!",1)



