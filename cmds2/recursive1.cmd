# recursion test
# test with factorial , doll function

print("start");

cls
# initialize
print("after init!")

function doll(size)
{
 print("We have "+size+" dolls")
 if(size==0) 
 {
	print("	all dolls removed !!!")
 	return(0)
 } 
 else 
 {
	print("	remove doll from "+size+" dolls")
 	return(doll(size-1))
 }
}

# factorial function
function fact(num)
{
 if(num==1) 
 {
 	return(1)
 }
 else
 {
	return(num*fact(num-1))
 }

}

function fact2(num)
{
 if(num==1) 
 {
 	return(1)
 }
 else
 {
	f=num*fact2(num-1);
	print("fact "+num+" is "+f);
	return(f)
 }
}

#_debug=20
print("start")

b=doll(10)

n=fact(6) : 720.0
n=fact2(6) : 720.0

n: 720.0
b:   0.0
if(b==0 & n==720) print("Test correct!")