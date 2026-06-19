
for i in `cat nt2.lst`;do echo $i; ./cte945f -d -$1 $i ;done 2>&1|tee check4_945_$1.out
