# bnf1.cmd

print("--- bnf1.cmd")

function d2(a,k1)
{
	b=a+1
	c=b*k1+2
	show_vars()
	return (c*2)
}

cls

k0
k1
k2
b=d2(5,1):  16.0
show_vars()
b:  16.0

