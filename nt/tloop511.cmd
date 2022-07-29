# test loop with fori

cls

p=0; l=0; 
print(_time)	: "Thu May 23 11:38:23 2019"
show_time("Start fori test loop 1",0)

fori(i=1;10;1){ 
	fori(j=1;800001;1) { 
		l = i*j+32.1; 
		p = p+l
	} 
	show_time("loop:"+i+" p="+p,1);
}
_time	: "Thu May 23 11:38:23 2019" ,err 
message("i<"+i+" and j< "+j+" sets p="+p)
print(_time)	: "Thu May 23 11:38:23 2019" ,err 
tdif=show_time("End test",2)
print("Test concluded in "+tdif+" seconds")
