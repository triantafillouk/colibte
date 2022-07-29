# test while loop inside if/else constructs

i=1
total=0
while (i<3)
{
	i1=l2=0
	print(i)
	if (i==1){
		p = 1
		while (i1<5){
			i1=i1+1
		}
	} else {
		p = 2
		while (i2<5){
			i2=i2+1
		}
	}
	total = total+p
	i1	:     0
	l2	:     0
	p	:     2
	l2 = l	:     0
	m1 = "while i="+i+" p="+p+" i1="+i1+" i2="+i2	: [while i=2.000000 p=2.000000 i1=0.000000 i2=5]
	message_wait(m1)
#	getchar()
	message("loop "+str$(i)+"|"+str$(k)+"|"+str$(l)+"|"+str$(p))
	message("i = "+str$(i))
	i=i+1
}
print("test end total is:"+total)
total


