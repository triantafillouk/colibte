
function scale_perc1(num)
{
 scale=[14000 4; 3000 20; 3000 35; 5000 37; 100000 20]
 ptotal=0
 ind=0
 val=0
 while(ptotal<num) {
	pptotal=ptotal
	ptotal += scale[ind][0]
	perc = scale[ind][1]
	l1 = scale[ind][0]
	
	print("ind="+ind+" len="+l1+" up to "+ptotal)
	if(num>ptotal) {
		val += scale[ind][0]*perc/100
		print("1 val="+val)
	} else {
		val += (num-pptotal) * perc/100
		print("2 val="+val)
		break
	}
	print(">>>>")
	ind++
 } 
 print("return "+val)
 return val
}

function scale_perc2(scale,num)
{
 ptotal=0
 ind=0
 val=0
 print("scale_perc2: num=",num)
 while(ptotal<num) {
	pptotal=ptotal
	ptotal += scale[ind][0]
	print(" ptotal="+ptotal+" pptotal="+pprotal)
	perc = scale[ind][1]
	l1=scale[ind][0]
	print("	ind=",ind," perc=",perc," len=",l1)
	#print("	ind="+ind+" perc="+perc+" len="+len+" up to "+ptotal+" num=",num)
	if(num>ptotal) {
		print("	val=",val)
		val += scale[ind][0]*perc/100
		print("	1 val="+val)
	} else {
		print("	num-pptotal=",num-pptotal)
		val += (num-pptotal) * perc/100
		print("	2 val="+val)
		break
	}
	print(">>>>")
	ind++
 } 
 print("return "+val)
 return val
}

cls
print("find scales")

auto_scale=[14000 4; 3000 20; 3000 35; 5000 37; 100000 20]
print(auto_scale)
b=scale_perc2(auto_scale,10302)
print("b="+b)
b: 412.080000

a=scale_perc1(10301): 412.040000

#a: 41204.0
