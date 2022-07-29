# check / set  editors option values

fillcol				:    72
lmargin				:     0
keydebug			:     0
global				:     0
print_precision		:     6
clear_output		:     1
show_hex			:     0
tabsize				:     4

fillcol=80
lmargin=5
keydebug=1
global=1
show_hex=0
print_precision=5

i1=fillcol	:    80
i2=lmargin	:     5
i3=keydebug	:     1
i4=global	:     1

3.14		: 3.14000

s_compile	: "gcc -funsigned-char -O2  "
s_make		: "make"
s_error		:     0
s_lib		: " "
s_grep		: "*.c *.h *.cpp"

s_compile="make new compilation"
s_compile		: "make new compilation"


message("s_compile="+s_compile)
message("s_compile="+s_compile)
message("s_make="+s_make)
message("s+error="+s_error)
message("s_lib="+s_lib)
message("a=s_compile="+a)
message("b=s_make="+b)

print("test end")
