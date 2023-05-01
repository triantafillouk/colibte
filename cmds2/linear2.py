
import math
import array

class item:
 def __init__(self,lentgh,min,max,number):
    self.len = lentgh
    self.min = min
    self.max = max
    self.number = number

def frem(item=[], *a,max_size):
    ms = max_size
    array_len=len(item)
    print("Frem function array size is ",len(item))
    if array_len>1:
        item[0].max = math.trunc(ms-item[1].len*item[1].min)/item[0].len
        for x in range(item[0].min,item[0].max):
            y1=(ms-x*item[0].len)/item[1].len
            ty1=math.trunc(y1)
            z=y1-ty1
            if
        print(item[0].len,item[1].len)
        for x=item[0].min 
        if array_len >2:
            a = item[2:]
            return frem(a,max_size=ms)
    

l1 = item(52,1,100,1)
l2 = item(183,1,100,1)
l3 = item(44,1,100,1)

ar = [l1,l2,l3]

frem(ar,max_size=800)



l1_len=52
l2_len=183
l1_min=1
l2_min=1
max_len=800
rest=max_len


l1_max=math.trunc((max_len-l2_len*l2_min)/l1_len)
for x in range( l1_min, l1_max):
    # print(x)
    y1=(max_len-x*l1_len)/l2_len
    ty1=math.trunc(y1)
    z=y1-ty1
    if z<=rest:
        # print("!! x=",x,"y1=",ty1," z=",math.trunc(z*l2_len))
        print("!! x=%2d y1=%2d  z=%2d " % (x,ty1, math.trunc(z*l2_len)))

        rest=z
    else:
        # print("   x=",x,"y1=",ty1," z=",math.trunc(z*l2_len))
        print("   x=%2d y1=%2d  z=%2d " % (x,ty1, math.trunc(z*l2_len)))
