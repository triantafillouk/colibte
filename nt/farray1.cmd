
cls

function test_array(a,l)
{
	b=a*1
	for (i=0;i<l;i=i+1)
	{
		b[i]+=0.1
		print(i+" "+b[i])
	}
	print("================")
}

function test_string(s)
{
	s1=s
	print(s1)
	return(s)
}

a1 = [1.22 2.44 3.11]
a1[1000]=5.3

test_array(a1,3)
test_array(a1,3)
test_array(a1,3)

s0="kostas"
test_string(s0)
test_string(s0)
t1=test_string("mar")

print("final name is "+s0)
print("return sval is "+t1)

print(a1[1000]): 5.300000