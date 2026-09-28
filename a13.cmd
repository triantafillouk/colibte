# fibonachi numbers
print("--- fib.cmd")

function fib2_inner(a,b,n)
{
#	print("fib2_iner: a="+a+" b="+b+" n="+n)
	if(n==0) { return(a) }
#	print ("fib2_iner: "+n+" "+fib2_inner(b,a+b,n-1))
	return fib2_inner(b ,a+b ,n-1)
}

cls
i=2
# show_time("Fibonachi start" ,0)
# for(i=100;i<1500;i+=100) 
	print("fib2_inner(",i,")=",fib2_inner(0 ,1 ,i))
# fori(i=100;1500;100) {
	# f=fib2_inner(0 ,1 ,i)
	# print("fib2_inner(",i,")=",f)
# }
# show_time("end " ,2)

