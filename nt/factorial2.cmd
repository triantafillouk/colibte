# check recursive functions 2

num=1
var1=10

function fact(num , st)
{
 print("fact "+num+" var1="+var1+" st=["+st+"]")
 if(num==1) 
 {
 	return(1)
 } else {
#	print("fact:0 num="+num);
	f=num*fact(num-1 ,st)
#	print("fact:1 num="+num+",f="+f)
	return(f)
 }
}

f=20
n=fact(f,"test")

n: 2432902008176640000.0
print("fact("+f+") is "+n)
return(n)

