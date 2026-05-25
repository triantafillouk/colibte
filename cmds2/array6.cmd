# array_cols wrong in bnf TBD

function parray(a,b,c)
{
	print(a)
	print(b)
	print(c)
}

a=[1, 2; 3, 4; 5, 6]
b=["test1",10,34.3]
array_rows(a):     3 | 0x3 | 0o3
array_cols(a):     2 | 0x2 | 0o2
c=3.14
parray(a,b,c)
s3="String3"
parray("string1","string2",s3)
