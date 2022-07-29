# test all language structures
# problem with syntax check bool expressions inside if check
cls
print("-- Start test ["+_time+"] --------------------------")
ok=1
start=0
# test if,else
level=0

if((ok & level>=start))
{
 test_name="(if, else commands)"
 result=2
 print("-- start test ["+test_name+"]")

 a=1
 t=0
 
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

 t	:     2
 print("level "+level+" (if,else test) result should be 2, is "+t)
 if(t!=2) 
 {
	ok=0
 }

 print("["+level+"] test "+test_name+" result should be "+result+", is "+t+" ok="+ok)
}
ok	:     1

# test for loop
level=1
if(ok==1 & level>=start)
{
 test_name="(for loop command)"
 result=9
 print("-- start test ["+test_name+"]")

 t=0
 for(i=1;i<10;i=i+1)
 {
#	print("i="+i)
	t=t+1
 }
 t	:     9
# print("level "+level+" (for loop) result should be 9, is "+t)
# print("["+level+"] test "+test_name+" result should be "+result+", is "+t+" ok="+ok)
 if(t!=result)
 {
  ok=0
 }

 print("["+level+"] test "+test_name+" result should be "+result+", is "+t+" ok="+ok)
}

ok	:     1
# test while loop
level=2
if(ok==1 & level>=start)
{
# _debug=2
 test_name="(while loop)"
 result=10
 print("-- start test ["+test_name+"]")

 t=0
 i=10
 while(i)
 {
	i=i-1
	t=t+1
 }
 t:    10
 print("level "+level+" (while loop) result should be 10, is "+t)
 if(t!=result) 
 {
 	ok=0
 }

 print("["+level+"] test "+test_name+" result should be "+result+", is "+t+" ok="+ok)
}
ok:     1

# test function

level=3
if(ok==1 & level>=start)
{
 test_name="(function call)"
 result=33.34
 print("-- start test ["+test_name+"]")
 t=0
 print("define function")
 
function tadd(va1,va2)
{
	print(va1)
	print(va2)
	va1	:
	va2	:
#_debug=20
	a1=va1+va2
#_debug=0
	print ("in tadd a1="+a1+",va1="+va1+",va2="+va2);	
	return(a1)
}

 print("call function")
 a=11
 t=tadd(a,22.34)
 t: 33.340000
}
else 
{
 a=10	:
}

if(!deq(t,result)) 
{
 ok=0
} 

 a:    11
 a1	:     0
 t: 33.340000
 print("result from function call is t="+t+" ok="+ok)
 print("["+level+"] test "+test_name+" result should be "+result+", is "+t+" ok="+ok)

print("-- End test ["+_time+"] --------------------------")
