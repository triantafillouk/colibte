
for i in `cat nt2.lst`;do echo $i; ./cte -D -d -x $i ;done  2>&1|tee all.out

