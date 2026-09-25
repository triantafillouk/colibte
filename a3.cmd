# fibonachi numbers

function fib2_inner(a,b,n)
{
#	print("fib2_iner: a="+a+" b="+b+" n="+n)
	if(n==0) { return(a) }
#	print ("fib2_iner: "+n+" "+fib2_inner(b,a+b,n-1))
	return fib2_inner(b ,a+b ,n-1)
}

print("fib of 150 is",fib2_inner(0,1,20))

