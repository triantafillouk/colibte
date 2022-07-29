# interest calculations
# months = years * 12
# a = current value == amount
# monthly payment
# p = a * ( ( r*(1+r)^m ) / ( (1+r)^m -1 ) )
# total amount t = p*m
# Insurance
as=3000/12	: 250.0
a=70000
drx = a*340.75	: 23852500.0
fr=0
m=12*6
int=3.9
r=(int/100)/12	: 0.003250
r*(1+r)^m	: 0.004105
(1+r)^m*r	: 0.004105
p=((a-fr) * ((r*(1+r)^m))) / ((1+r)^m -1)	: 1091.976262
p+as	: 1341.976262
# Maximum monthly payment
maxm = 1300
#--------------------------------------------------
p*12	: 13103.715144
(p+as)*12	: 16103.715144
# should give 422.24
# Total amount
t=m*p	: 78622.290864
# Year salary
1500*14	: 21000.0
# Remain montly amount
remain=(1500*14 - (p+as)*12) /12	: 408.023738
print("test end, remain="+remain)
remain
# 66000 6 years 74345
# 60000 5.5 y 66941
