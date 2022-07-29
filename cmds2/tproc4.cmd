# check function calls 2

function test2(a)
{
 print(a)
 a=13.34
 print(a)
 a:
}

a=10
b=11

a: <   10 | 0xA | 0o12>
b: <   11 | 0xB | 0o13>

c=test2(12)
c: <13.340000>
if(deq(c,13.34)) print("test ok")
else print("test end, wrong result:"+c)
