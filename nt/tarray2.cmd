cls
c=[1.1 3.2 3, 4 5 6,7 8.1 9]

c:array 12, slot 0, rows 3,cols 3

c[0][2]	:     3
c[0][1]=1.234
c:array 12, slot 0, rows 3,cols 3
d=[ 1.1 2.4 3 4 5 6 7 8 9 10]
d[0]	: 1.100000
DET(c)	: -4.356000
c:array 12, slot 0, rows 3,cols 3
for(j=0;j<3;j=j+1) print("row:"+j+","+c[j][0]+","+c[j][1]+","+c[j][2])
c[0][1]	: 1.234000
