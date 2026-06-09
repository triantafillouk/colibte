
./cte -x nt/$1.cmd -d > outx
./cte -f nt/$1.cmd -d > outf
diff outx outf
