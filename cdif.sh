
./cte -x $1 -d > outx
./cte -f $1 -d > outf
echo "====" $1
diff outx outf
