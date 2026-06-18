
for i in `cat nt2.lst`;do ./cte  -$1 $i |grep Result;done 2>&1|tee check4$1.out
