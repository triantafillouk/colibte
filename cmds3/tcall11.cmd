# test call macro file, return 1 if ok not ok in normal in bnf works ok TBC
# 0 if any error

print("go and exec_file: ....")
b=do_file("tcall2.cmd")	: 2.100000
b: 2.100000
if(b==2.1) print("test ok!")
else print("test not ok!")
;
b:
