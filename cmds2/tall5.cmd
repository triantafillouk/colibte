cls
# examples
# variables, strings and conditional expressions
# All variables have a numeric value and also sometimes a string value
# The result of an expression can be printed after the ':
# The numeric value is printed first inside < > and the 
# string value after inside []
print("test string operations")
a="6k2" :
b="8k2"	:
c="6k2" :
b:
# multiple setting in one expression
k1=k2=k3=k4=12+34+10	:
a+1	:
# a string value printed on status line
message_wait("-1-")
a2=k3+(a+1)	:
k3+a	:
# conditional expressions
(a==b)	:
(b==a)	:
a==c	:
a-b	:
b-a	:
(b-a)	:
(b-a)==-1	:
(b-a)==2	:
message("-2-")
getchar()
# substruction of strings gives a numeric value equal to the difference 
# of their first bytes
"á"-" "	:
message("-3-")
getchar()
a:
b:
b>a	:
b<a	:
message("-31-"+(b<a)+".."+(b>a))
getchar()

b*a	:
t=(b==a)	:
t1=b-a	:
triantafilloukostas=22	:

quit()
return()
# --------------------------------

r==a+b		:
print("a="+a+" b="+b+" r="+r)

r=="k2k2"	:
#(a+b)==r		:

message("-4-")
getchar()
a = left ("kostas", 2)		:
b = right ("kostas", 2)		:
c = mid ("kostas", 2,3)		:
mid (("kos"+"kat"),3,4) :
d = mid (("kos"+"kat"),3,4)+"ll"	:
mid ( mid ("kos"+"kat",3,4)+"ll",2,3)	:
mid ( a + "ll" ,2,3)			:
upper("kostas")			:
lower("KOSTAS")			:
# the asci value of the first character
s_asc("ko")	:
# character value. Numeric value and string at the same time !
char (12) :
# this is the left margin of the editor
lmargin = 10
a = lmargin	:
# Random value
i = rand(100)	:
# random seed
seed = 33
b = seed	:
b1 = rand(100)	:
b1	:
message(rand(100))
getchar()
# All the variables are global. That means that they are valid till the are
# changed, the editor ends or the initialize function is used !
# initialize all variables !
initialize()

