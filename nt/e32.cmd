
p=0
l=0

show_time("Start fori test loop 1",0)
fori(i=1;11;1)
{
	fori(j=1;50000001;1)
	{
		l=i*j+32.1
		p += l
	}
	s="p = "+p
	show_time(s,1);
}
tdif=show_time("End test",2)

p:
