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
	print(" a is true")
	t=t+1
 }
 else
 {
	print(" a is false")
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

 t	:   2.0
 print("level "+level+" (if,else test) result should be 2, is "+t)
 if(t==2) 
 {
	ok=2.2
 }
}
ok	: 2.200000

# test for loop
print("start test for loop , ok="+ok)
level=1
t=0
if(ok==2.2 & level>=start)
{
 print("start loop")
 t=0
 for(i=1;i<10;i=i+1)
 {
#	print("i="+i)
	t=t+1
 }
 t	:   9.0
 print("level "+level+" (for loop) result should be 9, is "+t)
 if(t!=9)
 {
  ok=0
 }
} else {
	print("loop not started ok="+ok+" level="+level+" start="+start)
}


ok	: 2.200000
# test while loop
print("start test while loop , ok="+ok)
level=2
t=0
if(ok & level>=start)
{
 print("start while loop")
 t=0
 i=10
 while(i)
 {
	i=i-1
	t=t+1
 }
 t:  10.0
 print("level "+level+" (while loop) result should be 10, is "+t)
 if(t!=10) 
 {
 	ok=0
 }

}
ok: 2.200000

 print("test again if loop ok="+ok)
ok=1

if(ok)
{
 print("ok="+ok)
 a:   1.0
}
else
{
 print("ok is not ok!")
 a:
}

# test function

print("start test function call , ok="+ok)
level=3
t=0
if(ok==1 & level>=start)
{
 print("define function")
 
function tadd(va1,va2)
{
	print(va1)
	print(va2)
	va1	:
	va2	:
	a1=va1+va2
	print ("in tadd a1="+a1+",va1="+va1+",va2="+va2);	
	return(a1)
}

 print("call function")
 a=11
 b=tadd(a,22.34)
 b: 33.340000
}
else 
{
 a=10	:
}

if(deq(b,33.34)) 
{
 ok=1
} 
else 
{
 ok=0
}
 a1	:   0.0
 b: 33.340000
 print("result from function call is b="+b+" ok="+ok)
