
cls

function test_array(a,l)
{
	# b=a*1
	for (i=0;i<l;i=i+1)
	{
		b[i]=i+0.1
		print(i+" "+b[i])
	}
}

function test_string(s)
{
	s=s+"-"
	print(s)
}

a1 = [1.22 2.44 3.11]

test_array(a1,3)
test_array(a1,3)
test_array(a1,3)

s0="kostas"
test_string(s0)
test_string(s0)
print("final name is "+s0)


