
for i in `cat nt2.lst`;do echo $i; e -d -x $i ;done 2>&1 |tee check4_1800.out

