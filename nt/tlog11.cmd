# logical test, result should be 1
cls
a3=1
a4=1

a3 & a4	:   1.0

j=4:   4.0
i=4:   4.0
j==4:   1.0
"ttttt="+((j==4 & i)==4):   0.0

print("tt="+((j==4 & i)==4))

if((j==4) & (i==4)) print("test2 ok!")
(j==4) & (i==4)	:   1.0
if(j==4 & i==4) print("test3  ok!")
#else print("test3 not ok!")


