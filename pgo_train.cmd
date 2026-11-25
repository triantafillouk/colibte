# synthetic training workload for `make ce-pgo` -- exercises the macro
# evaluator's hot paths (bnf_expr.c / mlang.c) with enough iterations to give
# gcc's profile-guided optimizer a representative branch/call distribution.
# Not a correctness test: regression coverage for correctness comes from
# nt2.lst / cmds2 / cmds3, which ce-pgo also replays during training.
cls

function fact(num)
{
	if(num<1) { return(1) } else { return(fact(num-1)*num) }
}

function tadd(a1, a2)
{
	return(a1+a2)
}

s=0
fori(i=0;1000000;1){
	s=s+i*2-1
}

t=0
fori(i=0;300000;1){
	t=tadd(t,i)
}

fori(i=0;30000;1){
	f=fact(10)
}

arr[2000]=0
fori(i=0;2000;1){
	arr[i]=i*2
}
total=0
fori(k=0;300;1){
	fori(i=0;2000;1){
		total=total+arr[i]
	}
}

sa[200]=""
fori(i=0;200;1){
	sa[i]="item"+i
}

s1=""
fori(i=0;20000;1){
	s1="x"+i
}

print("pgo_train done: s="+s+" t="+t+" f="+f+" total="+total+" s1="+s1)
