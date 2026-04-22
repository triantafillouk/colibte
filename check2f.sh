rm $2.out
rm $2.err
time for i in `cat nt.lst`;do echo $i;$1 -f $i;done  2>> $2.err | tee $2.out
