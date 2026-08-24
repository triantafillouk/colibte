# a2.cmd matrix division

cls
print("-- a2.cmd matrix division --")
a=[3,-2;4,-3]
print("array a is",a)
b=[6, -10;1,-2]
print("array b is",b)
d1=DET(b)
print("Determinant of b is ",d1)
inv1 = INV(b)
print("Inverse of b is ",inv1)
c=a/b
print("result is ",c)
# should be
# [2,-9;2.5,-11]
d=a*inv1
print("d is ",d)
