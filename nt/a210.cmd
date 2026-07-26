# simple double numeric array ops
# ok with bnf, uncorrect with normal SELECTBNF

cls
print("--- a210.cmd")
v=1.5
a=[	v, 2, 3
	v*2, 5, 6
	7, 8, 9]

# print("original array",a)
print("original a[0][0]=",a[0][0])
print("original a[1][0]=",a[1][0])
a++
print("after array increase a[1][0]=",a[1][0])
a[1][0]++
print("after element increase a[1][0]=",a[1][0])
a[1][0]*=3
print("after element mulby 3 a[1][0]=",a[1][0])
a[0][0]:
a[1][0]+=2:     7 | 0x7 | 0o7
print("after element updateby 2 a[1][0]=",a[1][0])
a[0][0]:
# a[1][2]=a[1][1]*2+1 :
print("final a[0][0]=",a[0][0])

