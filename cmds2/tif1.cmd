# test if construction
cls
k=1
print ("start test if construction"+k)

l=0
a=0
c=0
d=0

if(l)
 {
 a=1
 c=1
 print("part 1")
} else {
 a=3
 d=1
 print("part 2")
}
k=k+1
#print()
#a: <3.00000 | 0x3 | 0o3>
print("test end: a="+a+" c="+c+" d="+d)

if(a==3 & c==0 & d==1) print("test end ok!")
else print("test false!")
