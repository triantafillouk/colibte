cls()

# test_print()
a = 1
b = 3
if (b==2) e=1
a	:     1
(b == a)	:     0
a	:     1
a = (a + 1)	:     2
# after the following line the value of a should be 3 if the original a was 1
if ( b == ( a = (a+1) )) {
a	:     3
c = 1
} else {
a	:
c = 2
}
a	:     3
c	:     1
d=0
if ( b == a) { d=1 }

if ( b == a) 
{ e = 1
 print ("e=1")
} else 
{
 e = 2
 print("e=2")
}


a	:     3
b	:     3
c	:     1
d	:     1
e	:     1

if (c == 1) 
{
 print  ("Computed ok !")
 message ("Computed ok!")
} else {
 message ("This is wrong!")
 print ("This is wrong!")
}

