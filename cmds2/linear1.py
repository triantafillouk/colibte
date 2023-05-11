
import math

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
