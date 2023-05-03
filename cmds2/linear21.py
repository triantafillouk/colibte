
import math
import array

class item:
 def __init__(self,lentgh,min,max,number):
    self.len = lentgh
    self.min = min
    self.max = max
    self.number = number

def frem(max_size, item=[], *a):
    ms = max_size
    array_len=len(item)
    print("Frem function array size is %d size to fill=%2d" % (len(item),max_size))
    if array_len < 1:
        return 0

    item[0].number = math.trunc(ms/item[0].len)
    if item[0].number > item[0].max:
        item[0].number = item[0].max
    item[0].max = item[0].number
    if item[0].number < item[0].min:
        item[0].min = item[0].number

    rest = ms - item[0].number * item[0].len

    if array_len>1:
        # item[0].max = math.trunc(rest-item[1].len*item[1].min)/item[0].len
        for x0 in range(item[0].min, item[0].max:
            remain = ms - item[0].len*x0
            a = item[1:]
            remain = frem(remain,a)
            
        item[1].number = math.trunc(rest/item[1].len)
        if item[1].number > item[1].max:
            item[1].number = item[1].max

        l2_max = math.trunc(item[0].max)

        for x in range(item[1].min,itemmax):
            y1=(ms-x*item[0].len)/item[1].len
            ty1=math.trunc(y1)
            z=y1-ty1
            if z <= rest:
                print("x=%2d y1=%2d rest=%2d" %(item[0].len,ty1,rest))
                rest = z

        if array_len >2:
            a = item[2:]
            return frem(rest*item[1].len,a)
        return rest

    print ("another chunk of %2d of len %2d rest=%2d" % (item[0].number,item[0].len,rest))   
    return rest

l1 = item(52,1,100,1)
l2 = item(183,1,100,1)
l3 = item(5,1,100,1)

ar = [l1,l2,l3]

frem(800,ar)

for x in ar:
    print(x.len,x.min,x.max,x.number)
