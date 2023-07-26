cls
a=1
if(a==1) {
	print(a,2,3,4)
} else {
	print("?????????")
}
b[100]=1
b:array 3, slot 1 type=2 rows 101,cols 1
b[5000]=1
b[500]=1.3
for(i=0;i<5000;i++) {
	print(i,"	:",b[i])
}