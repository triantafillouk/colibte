# test all language structures

cls

ok=1
start=0

# test if,else
level=0
t=0
if(ok & level>=start)
{
 a=1
 if(a)
 {
#	print("aaaa")
	t=t+1
 }
 else
 {
#	print("other")
	t=0
 }

 if(a==0)
 {
	t=t-1
 } 
 else
 {
	t=t+1
 }

 t	: <    2 | 0x2 | 0o2>
 print("level "+level+" (if,else test) result should be 2, is "+t)
 if(t==2) 
 {
	ok=2.2
 }
}
ok	: <2.200000>

# test for loop
print("start test for loop , ok="+ok)
level=1
t=0
if(ok==1 & level>=start)
{
 print("start loop")
 t=0
 for(i=1;i<10;i=i+1)
 {
#	print("i="+i)
	t=t+1
 }
 t	:
 print("level "+level+" (for loop) result should be 9, is "+t)
 if(t!=9)
 {
  ok=0
 }
}


ok	: <2.200000>
# test while loop
print("start test while loop , ok="+ok)
level=2
t=0
if(ok==1 & level>=start)
{
# _debug=2
 print("start while loop")
 t=0
 i=10
 while(i)
 {
	i=i-1
	t=t+1
 }
 t:
 print("level "+level+" (while loop) result should be 10, is "+t)
 if(t!=10) 
 {
 	ok=0
 }

}
ok: <2.200000>

 print("test again if loop ok="+ok)
ok=1

if(ok)
{
 print("ok="+ok)
 a: <    1 | 0x1 | 0o1>
}
else
{
 print("ok is not ok!")
 a:
}

print("end ok!")
