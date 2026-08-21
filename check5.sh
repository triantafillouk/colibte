
for i in `cat nt2.lst`;do echo $i;$1 -d -D -x $i;done 2> check_$2.err |tee check_$2.out