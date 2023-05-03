
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
    # print("Frem function array size is %d size to fill=%2d" % (len(item),max_size))
    if array_len < 1:
        return 0

    max1 = math.trunc(ms/item[0].len)
    print("#%d: %d/%d = %d" % (level,ms,item[0].len,max1))
    if max1<item[0].max:
        item[0].max=max1
    if item[0].min > item[0].max:
        item[0].min = item[0].max
    item[0].number = item[0].min

    remain = ms - item[0].min * item[0].len
    min_remain = m_remain
    print("-- %d: %3d - %d x %d = %d" % (level,ms,item[0].min,item[0].len,remain))
    if array_len>1:
        a = item[1:]
        print("     check level",level+1,"array remaning size is",len(a))
        for x0 in range(item[0].min, item[0].max):
            remain = frem(level+1,min_remain,ms - item[0].len*x0,a)
            if remain < min_remain:
                item[0].number = x0
                min_remain = remain
                print("%d: %2d x %3d = %3d remain %3d" % (level,x0,item[0].len,x0*item[0].len,remain))
                if remain==0:
                    return remain
    else:
        item[0].number = math.trunc(ms/item[0].len)
        min_remain = ms - item[0].number*item[0].len

    print ("another chunk of %2d of len %2d rest=%2d" % (item[0].number,item[0].len,min_remain))   
    return min_remain

l1 = item(5  ,1,100,1)
l2 = item(52 ,2,100,1)
l3 = item(183,2,100,1)

ar = [l1,l2,l3]

frem(0,800,800,ar)

for x in ar:
    print(x.len,x.min,x.max,x.number)
