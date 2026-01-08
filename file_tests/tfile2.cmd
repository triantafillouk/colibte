# check file edit operations 1

a = "butterfly 1"
b = "butterfly 2"
open_file("basicf.txt")

a	: <0.00000>[butterfly 1]
bof()
insert ( a)
newline(1)
eof()
insert (b)
newline(1)
bof()
newline(1)
#search_forward("eof")
save_file(1)
close_file(1)

