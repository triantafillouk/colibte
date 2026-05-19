# arrays as function args!!!!! (correct element additions in bnf not ok in normal!)
# last print not ok in bnf TBD
cls

function test_array(a ,l)
{
	print("aaaaaaaaaaaaaaaaaaaaa")
	print(a)
	b=a+1
	print("bbbbbbbbbbbbbbbbbbbbb")
	print(b)
	for (i=0;i<l;i=i+1)
	{
		#print("  ",i," ----------")
		b[i]+=0.1
		#print("b[",i,"] = ",b[i])
		print(i+"] = "+b[i])
	}
	print("ccccccccccccccccccccc")
	print(b)
	print("================")
}

function test_string(s)
{
	s1=s
	print(s1)
	return(s)
}

a1 = [1.22, 2.44, 3.11]
a1[1] : 2.440000
print("1111111111111111111")
print(a1)	
#a1[1000]=5.3
#a1[1000] :   0.0
print("2222222222222222222")
test_array(a1 ,3)

#test_array(a1 ,3)
#test_array(a1 ,3)


s0="kostas": "kostas"
test_string(s0)
test_string(s0)
t1=test_string("mar")

print("final name is "+s0)
print("return sval is "+t1)

print(a1[1000]):   0.0