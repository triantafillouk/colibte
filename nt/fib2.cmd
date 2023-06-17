# fibonachi numbers 
# fast fibonachi

function fib2_inner(a,b ,n)
{
	if(n==0) { return(a) }
	f=fib2_inner(b ,a+b ,n-1) :
#	print("f="+f+" b="+b+" a2="+a2+" n2="+n2)
	return(f)
}

cls
fib_num =  10:  10.0
result = fib2_inner(0 ,2 ,fib_num)
result: 110.0

print("Fibonachi number "+fib_num+" is "+result)


