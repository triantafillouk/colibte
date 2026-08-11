# check differences
e -x nt/$1 -d |tee 1.out
./cte -x nt/$1 -d |tee 2.out
meld 1.out 2.out

