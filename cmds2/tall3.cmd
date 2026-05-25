# test all language structures correct bnf TBC
# problem with syntax check bool expressions inside if check
cls
ok=1
start=0	:
# test if,else
level=0
bool=ok and level>=start	:
 print("start=",start)

if(ok and level>=start)
{
 print("if ok!")
 if(a==1) ok=1
 else ok=0
}

if(ok!=1) {
	print("Not ok!")
}


print("end program")
