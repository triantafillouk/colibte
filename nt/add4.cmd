# a recursive function that adds 3 arguments with a quirk

function add3(a,b,c)
{
 print("-args:a="+a+" b="+b+" c="+c)
 if(c==0) { 
 	print("add3: c=0")
	return (0) 
 } else { 
 d=a+b
 print("add3: d="+d)
 return(d)
 }
}

cls

result = add3(1 ,2 ,0)
result:   3.0

`
 this should act as a multiline comment
`
print("result is new "+result);

