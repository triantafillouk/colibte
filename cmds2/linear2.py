
import math
import array

class item:
 def __init__(self,lentgh,min,max,number):
    self.len = lentgh
    self.min = min
    self.max = max
    self.number = number

def frem(level,m_remain,max_size, item=[], *a):
    ms = max_size
    array_len=len(item)
    print("%s :%d, m_remain %d, max %d, %d" % ("+".rjust(2*level,' '),level,m_remain,max_size,item[0].number))
    if array_len < 1:
        return 0

    max1 = math.trunc(m_remain/item[0].len)
    if max1==0:
        return m_remain
    min1 = 0
    if max1>item[0].max:
        max1 = item[0].max
    # print("#%d:remain=%d %d/%d = %d" % (level,m_remain,ms,item[0].len,max1))

    item_number = 0
    remain = m_remain
    min_remain = remain

    if array_len>1:
        a = item[1:]
        # print("     check level",level+1,"array remaning size is",len(a))
        num_of_min_remain=0
        for x0 in range(0, max1+1):
            remain = frem(level+1,min_remain,ms - item[0].len*x0,a)
            print("%s %d: %3d - %d x %d = %d %d" % ("<".rjust(2*level,' '), level,ms,x0,item[0].len,remain,min_remain))
            if remain < min_remain:
                num_of_min_remain = x0
                item[0].number = x0
                # print("%s :%d, set %2d x %3d = %3d remain %3d" % ("#".rjust(2*level,' '),level,x0,item[0].len,x0*item[0].len,remain))
                min_remain = remain
                if remain==0:
                    return remain
            print("%s %d: %3d - %d x %d = %d %d" % (">".rjust(2*level,' '), level,ms,x0,item[0].len,remain,min_remain))
        return min_remain
    else:
        if ms>item[0].len:
            item_number = math.trunc(ms/item[0].len)
            if item_number > item[0].max - item[0].min:
                item_number = item[0].max - item[0].min
            remain = ms - item_number*item[0].len
        if remain < min_remain:
            min_remain=remain
            item[0].number = item_number
            # print("%s :%d, set %2d x %3d = %3d remain %3d" % ("#".rjust(2*level,' '),level,item[0].number,item[0].len,item[0].number*item[0].len,remain))
        return remain

# define array with initial info (size, minimum, maximum, result number)
l1 = item(250,0,100,0)
l2 = item(219,0,100,0)
l3 = item(189,0,100,0)
l4 = item(158,0,100,0)
l5 = item(128,0,100,0)
l6 = item(98,0,100,0)
tsize = 658

ar = [l1,l2,l3,l4,l5,l6]

# remove known minimum requirements
initial_min=0
for x in ar:
    initial_min = initial_min+x.min*x.len

if initial_min > tsize:
    print("Minimum values over total size: %d > %d",initial_min,tsize)
else:
    print("Total size = %d, initial min = %d" % (tsize,initial_min))
    frem(0,tsize-initial_min,tsize-initial_min,ar)

# show results
ind=1
print("ind  len  min   max   num   total")
total=0
for x in ar:
    total0 = (x.number+x.min)*x.len
    total = total + total0
    print("%d:  %3d  %3d   %3d   %3d    %4d   %4d" % (ind,x.len,x.min,x.max,x.number+x.min,total0,total))
    ind=ind+1
print("   remain is %3d - %3d = %3d" %(tsize,total,tsize-total)) 


