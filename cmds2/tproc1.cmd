# function calls tests
# problem with argument values ,ok

function test2(a,b)
{
 print("a="+a)
 print("b="+b)

 print("test2: a+b="+a+b)
 return (a+b)
}

function test3(a,b)
{
 print("a="+a)
 print("b="+b)
 print("test3: a*b="+a*b);
 return(a*b)
}

k=4
c1=test2(k,2)
c2=test3(2.2,3.31)
c1: <    6 | 0x6 | 0o6>
c2: <7.282000>

if(deq(c2,7.282)) print("test ok!")
