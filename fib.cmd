# fibonachi numbers

function fib(n)
{
	if (n==0) { return (0) }
	if (n==1) { return (1) }
	return fib(n-1) + fib(n-2)
}

function fib1(n)
{
	if (n<2) { return (n) }
	return fib1(n-1) + fib1(n-2);
}

function fib2_inner(a,b,n)
{
#	print("fib2_iner: a="+a+" b="+b+" n="+n)
	if(n==0) { return(a) }
#	print ("fib2_iner: "+n+" "+fib2_inner(b,a+b,n-1))
	return fib2_inner(b ,a+b ,n-1);
}

fib_num =  35
cls
print ("Evaluate fibonachi "+fib_num)
show_time("Fibonachi start" ,0)
print("result for fib :"+fib(fib_num))
show_time("fib" ,1);
print("result for fib1 :"+fib1(fib_num))
show_time("fib1" ,1);
print("result for fib2_inner "+fib2_inner(0 ,1 ,fib_num))
show_time("fib2_inner" ,1);
show_time("end " ,2)

