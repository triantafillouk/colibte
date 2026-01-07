# string operations on string arrays
a=["a1" "a2"]
a: array 0:[a] type [SARRAY] , slot 0 type=3 rows 1,cols 2
a[0]+="test1"
a[0]: "a1test1"
a[1]: "a2"
a+=" test2"
a[0]: "a1test1 test2"
a[1]: "a2 test2"
a[1]-="kk"
a[1]:
b=[1 2]
b[0]+=1.1
b[0]: 2.100000
b[1]-=0.2
b[1]: 1.800000

b+=4
b[0]: 6.100000
b[1]: 5.800000
