
for i in `cat nt2.lst`;do echo $i; ./cte -D -d -x $i ;done 2> check4_964_n.err  |tee check4_964_n.out
