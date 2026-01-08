# test call macro file, return 1 if ok
# 0 if any error

print("go and exec_file: ....")
b=exec_file("tcall2.cmd")	: 2.100000
b: 2.100000
if(b==2.1) print("test ok!")
else print("test not ok!")
