# ok, TBC from 899
cls
# examples
# variables, strings and conditional expressions
# All variables have a numeric value and also sometimes a string value
# The result of an expression can be printed after the ':
# The numeric value is printed first inside < > and the 
# string value after inside []
print("test string operations")
a="6k2" : "6k2"
b="8k2"	: "8k2"
c="6k2" : "6k2"
b: "8k2"
# multiple setting in one expression
k1=k2=k3=k4=12+34+10	:    56 | 0x38 | 0o70
a+1	: "6k21"
# a string value printed on status line
message_wait("-1-")
a2=k3+(a+1)	: "56.0000006k21"
k3+a	: "56.0000006k2"
# conditional expressions
(a==b)	:     0 | 0x0 | 0o0
(b==a)	:     0 | 0x0 | 0o0
a==c	:     1 | 0x1 | 0o1
a-b	:    -2 | 0x0 | 0o0
b-a	:     2 | 0x2 | 0o2
(b-a)	:     2 | 0x2 | 0o2
(b-a)==-1	:     0 | 0x0 | 0o0
(b-a)==2	:     1 | 0x1 | 0o1
message("-2-")
getchar()
# substruction of strings gives a numeric value equal to the difference 
# of their first bytes
"z"-"a"	:    25 | 0x19 | 0o31
message("-3-")
getchar()
a: "6k2"
b: "8k2"
b>a	:     1 | 0x1 | 0o1
b<a	:     0 | 0x0 | 0o0
message("-31-"+(b<a)+".."+(b>a))
getchar()

# b*a	:
t=(b==a)	:     0 | 0x0 | 0o0
t1=b-a	:     2 | 0x2 | 0o2
triantafilloukostas=22	:    22 | 0x16 | 0o26

#quit()
#return()
# --------------------------------

r==a+b		:     0 | 0x0 | 0o0
print("a="+a+" b="+b+" r="+r)

r=="k2k2"	:     1 | 0x1 | 0o1
#(a+b)==r		:

message("-4-")
getchar()
a = left ("kostas", 2)		: "ko"
b = right ("kostas", 2)		: "as"
c = mid ("kostas", 2,3)		: "sta"
mid (("kos"+"kat"),3,4) : "kat"
d = mid (("kos"+"kat"),3,4)+"ll"	: "katll"
mid ( mid ("kos"+"kat",3,4)+"ll",2,3)	: "tll"
mid ( a + "ll" ,2,3)			: "ll"
upper("kostas")			: "KOSTAS"
lower("KOSTAS")			: "kostas"
# the asci value of the first character
s_asc("ko")	:   107 | 0x6B | 0o153
# character value. Numeric value and string at the same time !
char (12) :    12 | 0xC | 0o14
# this is the left margin of the editor
lmargin = 10
a = lmargin	:    10 | 0xA | 0o12
# Random value
i = rand(100)	:    83 | 0x53 | 0o123
# random seed
seed(33)
#b = seed	:
b1 = rand(100)	:    86 | 0x56 | 0o126
b1	:    86 | 0x56 | 0o126
message(rand(100)):
getchar()
# All the variables are global. That means that they are valid till the are
# changed, the editor ends or the initialize function is used !
# initialize all variables !
initialize()

