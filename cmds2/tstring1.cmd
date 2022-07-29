# check string operations
cls
bs="butterfly"

# string length
strlen(bs)	:     9

# upper/lower case convertion (just for latin characters)
Bs=upper$(bs)	: "BUTTERFLY"
b2=lower$(Bs)	: "butterfly"

# left, right, mid parts of strings
l1=left$(bs,3)	: "but"
r2=right$(bs,3)	: "fly"
m3=mid$(bs,3,2)	: "tt"

# conversion to string/numerical values
n4=str$(123.22)	: "123.220000"
n5=s_val("23.44 abc")	: 23.44000

# srting index function
a = s_index(bs,"fly")
a	:     7
b = s_index(bs, "tes")
b	:     0

# char to integet conversion (of first letter of a string)
a=s_asc("klm")	:   107
# integer to character conversion
b=chr$(107)		: "k"

print("test end: a="+a+" b="+b)
