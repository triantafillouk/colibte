rm $2.err
time for i in nt/*.cmd;do echo $i;$1 -d -x $i;done 2>> $2.err |tee $2.out