# simple double numeric array ops
# ok with bnf, uncorrect with normal SELECTBNF

cls
print("--- a210.cmd")
v=1.5
a=[	v, 2, 3
	v+v, 5, 6
	7, 8, 9]

print("original array",a)
print("original a[0][0]=",a[0][0])
print("original a[1][0]=",a[1][0])
a/=2 : array 1:[a] type [2:ARRAY] rows 3,cols 3
print("after array a/=2",a)
a[1][0]++ :   2.0
print("after element increase a[1][0]=",a[1][0])
a[1][0]*=3 : 1.500000
print("after element mulby 3 a[1][0]=",a[1][0])
a[0][0]: 1.500000
a[1][0]+=2: 3.500000
print("after element updateby 2 a[1][0]=",a[1][0])
a[0][0]: 3.500000
# a[1][2]=a[1][1]*2+1 :
print("final a[0][0]=",a[0][0])

