
import math
import array

class item:
 def __init__(self,lentgh,min,max,number):
    self.len = lentgh
    self.min = min
    self.max = max
    self.number = number
    self.test = 0
    self.current = 0

def show_array(ar=[]):
    ind=1
    print("ind  len  min   max")
    for x in ar:
        print("%d:   %3d  %3d   %3d" % (ind,x.len,x.min,x.max))
        ind=ind+1


def show_results(total0,ar=[]):
    total=0
    print("%d - ("%(total0),end='')
    ind=0
    number_of_items=0
    for i in ar:
        total = total + i.len*(i.current+i.min)
        number_of_items = number_of_items + i.current+i.min
        if ind==0:
            print("(%d x %d) "%(i.len,i.current+i.min),end='')
        else:
            print("+ (%d x %d) "%(i.len,i.current+i.min),end='')
        ind = ind+1
    print (")=%d items %d"%(total0-total,number_of_items))

def show_final(total,ar=[]):
    ind=1
    print("ind  len  min   max   num   total")
    total=0
    for x in ar:
        total0 = (x.number+x.min)*x.len
        total = total + total0
        print("%d:  %3d  %3d   %3d   %3d    %4d   %4d" % (ind,x.len,x.min,x.max,x.number+x.min,total0,total))
        ind=ind+1
    print("   remain is %3d - %3d = %3d" %(tsize,total,tsize-total)) 


def frem(level,min_goal,m_remain,max_size,total, ar=[]):
    ms = max_size
    array_len=len(ar)
    # print("array size %d level %d"%(array_len,level))
    max1 = math.trunc(ms/ar[level].len)
    if max1>ar[level].max:
        max1 = ar[level].max
    remain = max_size
    # print("%s :%d, r= %d, max %d, n=%d m=%d" % ("+".rjust(2*level,' '),level,m_remain,max_size,ar[level].number,max1))

    if level<array_len-1:
        # print("     check level",level+1,"array remaning size is",len(a))
        for x0 in range(0, max1):
            ar[level].current=x0;

            for l in range(level+1,array_len):
                ar[l].current=0
            remain = frem(level+1,min_goal,m_remain,ms - ar[level].len*x0,total,ar)
            # print("%s %d: %3d - %d x %d = %d %d" % ("<".rjust(2*level,' '), level,ms,x0,ar[level].len,remain,m_remain))
            # if remain==0:
                # print("remain is 0!!!!!!!!!!!!!")
                # return remain
            if remain < m_remain:
                m_remain = remain
            # print("%s %d: %3d - %d x %d = %d %d" % (">".rjust(2*level,' '), level,ms,x0,ar[level].len,remain,min_remain))
        return m_remain
    else:
        # ar[level].current=0
        item_number=0
        if ms>ar[level].len:
            item_number = math.trunc(ms/ar[level].len)
            # check if bigger thatn maximum
            if item_number > ar[level].max - ar[level].min:
                item_number = ar[level].max - ar[level].min
            ar[level].current = item_number
            remain = ms - item_number*ar[level].len
            
        if remain < m_remain:
            m_remain=remain
            for l in range(0,level):
                ar[l].number = ar[l].current
            # print("%s :%d, set %2d x %3d = %3d remain %3d %d" % ("".rjust(2*level,' '),level,ar[level].number,ar[level].len,ar[level].number*ar[level].len,remain,m_remain))
            ar[level].number = item_number
        if remain==min_goal:
            # print ("remain is again minimum!")
            show_results(total,ar)
        return m_remain

def find_minimum_remain(tsize, ar=[]):
# remove known minimum requirements
    initial_min=0
    for x in ar:
        initial_min = initial_min+x.min*x.len
    
    if initial_min > tsize:
        print("Minimum values over total size: %d > %d",initial_min,tsize)
    else:
        print("Total size = %d, initial min = %d array size %d" % (tsize,initial_min,len(ar)))
        remain=frem(0,0,tsize-initial_min,tsize-initial_min,tsize,ar)
        if remain >0:
            remain=frem(0,remain,tsize-initial_min,tsize-initial_min,tsize,ar)
    

# define array with initial info (size, minimum, maximum, result number)
l1 = item(250,1,100,0)
l2 = item(219,0,100,0)
l3 = item(189,0,100,0)
l4 = item(158,0,100,0)
l5 = item(128,0,100,0)
l6 = item(98,0,100,0)
#tsize = 658
tsize = 763

ar = [l1,l2,l3,l4,l5,l6]

# show array
show_array(ar)

find_minimum_remain(tsize,ar)

# show results
#print("   remain is %3d - %3d = %3d" %(tsize,total,tsize-total)) 

show_final(tsize,ar)

