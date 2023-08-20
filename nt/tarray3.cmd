cls
c=[	1.1 	3.2 	3
	4 		5 		6
	7		8.1		9]
print("The original array C")
#c:array 24, slot 0, rows 3,cols 3
print(c)

c[0][0]=10.1
c[1][1]=20.1
c[0][1]	: 3.200000
k=c[0][1]
k: 3.200000
c[1][0]=k	: 3.200000
c[1][0]		: 3.200000
print("Modified array C")
#c:array 24, slot 0, rows 3,cols 3
print(c)
inv=INV(c)
print("Inverse array of C inv")
#inv:array 28, slot 2, rows 3,cols 3
print(inv)
d=c*inv
print("Unity array C*inv")
print(d)
#d:array 29, slot 3, rows 3,cols 3
print("ok!")
