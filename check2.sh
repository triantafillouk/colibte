rm normal2.out
rm normal2.err
time for i in `cat nt2.lst`;do echo $i;$1 -d -x $i;done  2>> normal2.err | tee normal2.out
