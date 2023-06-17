cls
test=1
if(test==1){
c=[1 2
	3 4 5 6
	7 8]	:array 28, slot 1 type=2 rows 3,cols 4
	print("τεστ 1")
} else {
c=[1 2
	3 4 5 6
	7 8]	:array 13, slot 1, rows 3,cols 4
	print("τεστ 2")
}

for(i=0;i<10;i=i+1){
print("loop "+i);
c:array 28, slot 1 type=2 rows 3,cols 4
d=c*(10+i)
#a:
d:array 40, slot 3 type=2 rows 3,cols 4
}
print("check determinant")
b=[1 2 3 ; 0 5 6 ; 7 8 9]	:array 30, slot 4 type=2 rows 3,cols 3
DET(b)	: -24.0
print("ok!")