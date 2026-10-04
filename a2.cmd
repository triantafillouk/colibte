# function calls tests
# problem with argument values ,ok

function test2(a,b)
{
 print("a="+a)
 print("b="+b)

 print("test2:"+" a+b=",a+b)
 return (a+b)
}

# cls

k=4
k:
c1=test2(k,2)
c1: <    6 | 0x6 | 0o6>

