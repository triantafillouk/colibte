# a recursive function that adds 3 arguments with a quirk

function add3(a,b,c)
{
 print("-args:a="+a+" b="+b+" c="+c)
 if(c==0) { return (0) }
 d=a+b+add3(a,b,c-1)
 return(d)
}

cls

result = add3(1 ,2 ,2)
result:   3.0

`
 this should act as a multiline comment
`
print("result is new "+result);

