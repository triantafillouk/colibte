time for i in nt/*.cmd;do echo $i;cte -D -x $i;done 2> $1.err |tee $1.out