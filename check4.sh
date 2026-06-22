
for i in `cat nt2.lst`;do echo $i; ./cte -d -$1 $i ;done 2>&1|tee check4_948_$1.out
