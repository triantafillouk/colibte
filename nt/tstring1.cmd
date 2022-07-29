# string compare
a=c=c1="fly1"	: "fly1"
b="fly2"		: "fly2"

d=d1=3.14		: 3.140000
d1=a			: "fly1"
c1=d			: 3.140000
a==b	:     0
a==c	:     1
a!=b	:     1
a!=c	:     0

a<b		:     1
a<c		:     0
a>b		:     0
a>c		:     0

a<=b		:     1
a<=c		:     1
a>=b		:     0
a>=c		:     1
a==d		:     0 ,err string comparison error
a!=d		:     1 ,err string comparison error
