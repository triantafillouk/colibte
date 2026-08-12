rm bnf2.out
rm bnf2.err
time for i in `cat nt2.lst`;do echo $i;$1 -d -f $i;done  2>> bnf2.err | tee bnf2.out
