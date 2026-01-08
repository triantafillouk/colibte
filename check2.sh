rm $2.out
time for i in nt/*.cmd;do echo $i;$1 -x $i;done >> $2.out