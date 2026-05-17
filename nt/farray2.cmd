# arrays as function args!!!!! (not working ok with bnf! TBD)
cls

function test_array(a ,l)
{
	b=a*1
	print(b)
	for (i=0;i<l;i=i+1)
	{
		b[i]=b[i]+0.1
		#print("b[",i,"] = ",b[i])
		print("b["+i+"] = "+b[i])
	}
	print(b)
	print("================")
}

a1 = [1.22, 2.44, 3.11]
print("1111111111111111111111")
a1[1] = a1[2]+3 : 
print(a1)	
print("2222222222222222222222")

test_array(a1 ,3)


print(a1) 