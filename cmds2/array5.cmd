
function scale_perc1(num)
{
 scale=[14000 4; 3000 20; 3000 35; 5000 37; 100000 20]
 ptotal=0
 ind=0
 val=0
 while(ptotal<num) {
	pptotal=ptotal
	ptotal += scale[ind][0]
	print("index ="+ind+" up to "+ptotal)
	perc = scale[ind][1]
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
 while(ptotal<num) {
	pptotal=ptotal
	ptotal += scale[ind][0]
	print("index ="+ind+" up to "+ptotal)
	perc = scale[ind][1]
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

cls

auto_scale=[14000 4; 3000 20; 3000 35; 5000 37; 100000 20]

b=scale_perc2(auto_scale,10302)
b:

a=scale_perc1(10301):

a:
