cls
# examples
# variables, strings and conditional expressions
# All variables have a numeric value and also sometimes a string value
# The result of an expression can be printed after the ':' colon
# The numeric value is printed first inside < > and the 
# string value after inside []
print("test string operations")
a="6k2" : "6k2"
b="8k2"	: "8k2"
c="6k2" : "6k2"
b: <0.00000>[8k2]
# multiple setting in one expression
k1=k2=k3=k4=12+34+10	:  56.0
a+1	: "6k21"
# a string value printed on status line
message_wait("-1-")
a2=k3+(a+1)	: "56.0000006k21"
k3+a	: "56.0000006k2"
# conditional expressions
(a==b)	:   0.0
(b==a)	:   0.0
a==c	:   1.0
a-b	:  -2.0
b-a	:   2.0
(b-a)	:   2.0
(b-a)==-1	:   0.0
(b-a)==2	:   1.0
message("-2-")
getchar()
# substruction of strings gives a numeric value equal to the difference 
# of their first bytes
"á"-" "	: 193.0
message("-3-")
getchar()
a:
b:
b>a	:   1.0
b<a	:   0.0
message("-31-"+(b<a)+".."+(b>a))
getchar()
return()
b*a	: <48.00000>[6k2]
t=(b==a)	:   0.0
t1=b-a	:   2.0
triantafilloukostas=22	:  22.0
# --------------------------------

r==a+b		:   0.0
print("a="+a+" b="+b+" r="+r)

r=="k2k2"	:   1.0
#(a+b)==r		:   0.0 ,err string comparison error

message("-4-")
getchar()
a = left$ ("kostas", 2)		: "ko"
b = right$ ("kostas", 2)		: [as]
c = mid$ ("kostas", 2,3)		: [ost]
mid$ (("kos"+"kat"),3,4) : "skat"
d = mid$ (("kos"+"kat"),3,4)+"ll"	: "skatll"
mid$ ( mid$ ("kos"+"kat",3,4)+"ll",2,3)	: [kat]
mid$ ( a + "ll" ,2,3)			: [oll]
upper$("kostas")			: [KOSTAS]
lower$("KOSTAS")			: [kostas]
# the asci value of the first character
s_asc("ko")	:   107
# character value. Numeric value and string at the same time !
char (12) :
# this is the left margin of the editor
lmargin = 10
a = lmargin	:     0
# Random value
i = rand	: 1004837922
# random seed
seed = 33
b = seed	:    33
b1 = rand	: 499555046
b1	: 499555046
message(rand)
getchar()
# All the variables are global. That means that they are valid till the are
# changed, the editor ends or the initialize function is used !
# initialize all variables !
initialize()

