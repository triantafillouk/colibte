
import math
import array

class item:
 def __init__(self,lentgh,min,max,number):
    self.len = lentgh
    self.min = min
    self.max = max
    self.number = number
    self.test = 0

def frem(level,m_remain,max_size, item=[], *a):
    ms = max_size
    array_len=len(item)
    # print("%s :%d, m_remain %d, max %d, %d" % ("+".rjust(2*level,' '),level,m_remain,max_size,item[level].number))
    max1 = math.trunc(max_size/item[level].len)
#    if max1==0:
#        return m_remain
    min1 = 0
    if max1>item[level].max:
        max1 = item[level].max

    print("%s l=%d max=%d"%("<".rjust(2*level,' '),level,max1))

    item_number = 0
    remain = m_remain
    min_remain = remain

    if level<array_len-1:
        # a = item
        num_of_min_remain=0
        item[level].test=0
        for x0 in range(0, max1+1):
            item[level].test = x0
            # print("(%d->%2d) "%(level,x0) ,end='')
            print("%s l=%d x0=%d call max=%d"%(":".rjust(2*level,' '),level,x0,ms-item[level].len*x0))
            remain = frem(level+1,min_remain,ms - item[level].len*x0,item)
            # print("%s %d: %3d - %d x %d = %d %d" % ("<".rjust(2*level,' '), level,ms,x0,item[level].len,remain,min_remain))
            if remain < min_remain:
                num_of_min_remain = x0
                item[level].number = x0
                min_remain = remain
                if remain==0:
                    print("remain is 0!")
                    return remain
            # print("%s %d: %3d - %d x %d = %d %d" % (">".rjust(2*level,' '), level,ms,x0,item[level].len,remain,min_remain))
        print("%s %d remain is %d"%(">".rjust(2*level,' '),level,remain))
        return remain
    else:
        if ms>item[level].len:
            item_number = math.trunc(ms/item[level].len)
            if item_number > item[level].max - item[level].min:
                item_number = item[level].max - item[level].min
            remain = ms - item_number*item[level].len

        if remain < min_remain:
            min_remain=remain
            item[level].number = item_number
        item[level].test = item_number
        total=0
        for l in range(0,array_len):
            total = total + item[l].len*item[l].test
            print(" (%3d x %2d) "%(item[l].len,item[l].test),end='')
        print(" >= %3d remain = %d"%(total,remain))
#         print("(%d->%2d) r=%d"%(level,item_number,remain))
        # print("%s %d"%(">".rjust(2*level,' '),level))

        return remain

# define array with initial info (size, minimum, maximum, result number)
l1 = item(250,0,100,0)
l2 = item(218,0,100,0)
l3 = item(189,0,100,0)
l4 = item(158,0,100,0)
l5 = item(128,0,100,0)
l6 = item(98,0,100,0)
tsize = 665

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


for i in range (0,3):
    print ("-- %d" %(i))