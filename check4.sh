
for i in `cat nt2.lst`;do echo $i; ./cte963n -d -x $i ;done 2>&1|tee check4_963_n.out
