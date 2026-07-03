
for i in `cat nt2.lst`;do echo $i; ./cte965n -d -x $i ;done  |tee check4_965_n.out

