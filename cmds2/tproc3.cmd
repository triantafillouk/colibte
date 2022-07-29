# test function call

function aa(a)
{
 if(a==1)
 {
	print("aaaaa")
	return(1)
 }
 print("bbbbbb")
 return(2)
}

i1=aa(2)	: <    2 | 0x2 | 0o2>
i1	: <    2 | 0x2 | 0o2>
i1=aa(1)
i1	: <    1 | 0x1 | 0o1>

print("test ok")
