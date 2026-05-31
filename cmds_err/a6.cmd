# TBC check boundaries in bnf arrays or create new ones??
# does nothing in normal mode
# a=[1, 2;3, 4]
#a=[1;1]
# the following creates a 4x4 array in bnf 
# Error:209 [a] tnum=1 ttype=72 array indexes out of bound! 
a[3][3]=1:
a:
a[3][3]:
