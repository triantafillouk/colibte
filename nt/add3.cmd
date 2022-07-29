function add3(a,b,c)
{
 if(c==0) { return (0) }
 d=a+b+add3(a,b,c-1)
 return(d)
}


result = add3(1,2,4)
result:    12 | 0xC | 0o14
