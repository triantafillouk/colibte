# array ops, not correct in normal mode
cls
print("--- farray4.cmd")
b=3
a1 = [1.22, 2.44, 3.11]
a1[1] = a1[2]+3 : 6.110000
print(a1)	

for (i=0;i<3;i++)
{
	a1[i] -= 2.1
	print("a1["+i+"] = "+a1[i])
}

print(a1)