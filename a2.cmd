# fibonachi numbers
cls
# print("--- fib.cmd")

function fib2_inner(a,b,n)
{
#	print("fib2_iner: a="+a+" b="+b+" n="+n)
	if(n==0) { return(a) }
#	print ("fib2_iner: "+n+" "+fib2_inner(b,a+b,n-1))
	return fib2_inner(b ,a+b ,n-1)
}

# show_time("Fibonachi start" ,0)
#print("result for fib :"+fib(fib_num))
#show_time("fib" ,1);
#print("result for fib1 :"+fib1(fib_num))
#show_time("fib1" ,1);
#print("result for fib2_inner "+fib2_inner(0 ,1 ,fib_num))
# show_time("fib2_inner" ,1);
# for(i=5;i<15;i+=1) {print("fib2_inner(",i,")=",fib2_inner(0 ,1 ,i));}
# for(i=5;i<15;i+=1) {
	# ff=fib2_inner(0,1,5)
	ff=5:
	# print("fib2_inner(",i,")=",ff," ",fib2_inner(0,1,i));
	# print("fib2_inner("+i+")="+ff+" "+fib2_inner(0,1,i));
	print("fib2_inner("+ff+")="+fib2_inner(0,1,ff));
# }
# show_time("end " ,2)
"ok!":

