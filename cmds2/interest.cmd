# interest calculations
# months = years * 12
# a = current value == amount
# monthly payment
# p = a * ( ( r*(1+r)^m ) / ( (1+r)^m -1 ) )
# total amount t = p*m
# Insurance
as=3000/12	: 
a=70000
drx = a*340.75	: 
fr=0
m=12*6
int=3.9
r=(int/100)/12	: 
r*(1+r)**m	: 
(1+r)**m*r	: 
p=((a-fr) * ((r*(1+r)**m))) / ((1+r)**m -1)	: 
p+as	: 
# Maximum monthly payment
maxm = 1300
#--------------------------------------------------
p*12	: 
(p+as)*12	: 
# should give 422.24
# Total amount
t=m*p	: 
# Year salary
1500*14	: 
# Remain montly amount
remain=(1500*14 - (p+as)*12) /12	: 
print("test end, remain="+remain)
remain
# 66000 6 years 74345
# 60000 5.5 y 66941
