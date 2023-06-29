# check recursive functions 1
# factorial with a second argument to print

num=1
var1=10
print("Start factorial1")

function fact(num,s)
{
 print("fact "+s+num+" var1="+var1)
 a="aa"	# check 
 a1=""
 if(num==1) 
 {
 	return(1)
 } else {
#	print("fact:0 num="+num);
	f=num*fact(num-1,s)
#	print("fact:1 num="+num+",f="+f)
	return(f)
 }
}

f=3
print("call fact("+f+")")
n=fact(f,"test ")

n:   6.0
print("fact("+f+") is "+n)
print("end test") 
if(n==6) print("result is correct!")
return(n)

