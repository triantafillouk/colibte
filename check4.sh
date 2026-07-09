
for i in `cat nt2.lst`;do echo $i; ./cte970f -d -x $i ;done  |tee check4_970_f.out

