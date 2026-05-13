# check recursive functions 2

num=1
var1=10

function test(num,st)
{
 num:
 st:
}

function fact(num , st)
{
 # print("fact "+num+" var1="+var1+" st=["+st+"]")
 num:
 if(num<2) 
 {
 	return(1)
 } else {
#	print("fact:0 num="+num);
	f=num*fact(num-1 ,st)
#	print("fact:1 num="+num+",f="+f)
	return(f)
 }
}

f=3: 
n=test(f,"test")

#n=fact(f,"test")

n: 
print("fact("+f+") is "+n)
return(n)

