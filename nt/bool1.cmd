# some boolean ops
a=3:   3.0
c=55:   0.0
d=1:   1.0

a==3:   1.0
c==55:   1.0
d==1:   1.0

b1 = a==3 & c==55	:   1.0
b1:   0.0
b2 =(a==3) & (c==55)	:   1.0
b2:   1.0
b3 = a==3 & c==55 & d==1:   1.0
b3:   0.0

if(b2) {
 print("test end ok!")
} else {
	print("test false!")
}
