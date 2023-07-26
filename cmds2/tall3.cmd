# test all language structures
# problem with syntax check bool expressions inside if check
cls
ok=1
start=0
# test if,else
level=0
bool=ok & level>=start	:
 print("start")

if(ok & level>=start)
{
 print("ok!")
 if(a==1) ok=1
}

if(ok!=1) {
	print("Not ok!")
}


print("end program")
