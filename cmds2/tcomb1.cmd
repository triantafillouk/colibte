# combined file operations and while loops
cls
if(open_file("kk"))
{
clear_buffer("kk")
print("clear buffer")

i=p=0
insert(_time)
newline(1)
next_line(1)
loops=5: <    5 | 0x5 | 0o5>
_time	: [Sat Oct  4 16:39:43 2014]
print("start loop")
while (i<loops)
{
	j=0
	while (j<200000)
	{
		l = i*j + 33.2
		p = p+l
		j=j+1
	}
	message(i)
	insert(str$(i)+"   "+str$(p))
	newline(1)
	print("i="+i)
	i=i+1
}
_time	: [Sat Oct  4 16:39:43 2014]
insert(_time)
newline(1)
next_line(1)
message("i<"+str$(i)+" and j<"+str$(j)+" sets p="+str$(p))
insert("i<"+str$(i)+" and j<"+str$(j)+" sets p="+str$(p))
}
#unmark_buffer
# quit()

