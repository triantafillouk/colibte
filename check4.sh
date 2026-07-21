
for i in `cat nt2.lst`;do echo $i; ./cte -d -x $i ;done  |tee check4_980_f.out

