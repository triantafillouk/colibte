cls
max1=8000000
show_time("start allocate array",0)
# defining the max index of the array at the beggining is faster!
# a[max1]=0
# Loop to check allocation one item per time
for(i=0;i<max1;i=i+1)
{
	a[i]=1.1+i
}
show_time("array allocated!",1)
show_time("end",2)
# a:array 0, slot 1 rows 81,cols 1



