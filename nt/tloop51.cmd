# test loop with fori

cls
p=0, l=0; 
print(_time)	: "Wed Oct  8 15:13:47 2025"
show_time("Start fori test loop 1",0)

fori(i=1;101;1){ 
	fori(j=1;800001;1) { 
		l = i*j+32.1;
		p += l
	} 
	show_time("loop:"+i+" p="+p,1);
}
_time	: "Wed Oct  8 15:13:52 2025"
#message(_version+" i<"+i+" and j< "+j+" sets p="+p)
print(_time)	: "Wed Oct  8 15:13:52 2025"
tdif=show_time("End test",2)
tdif: 4.633262
print(_version+": p="+p+" Test concluded in "+tdif+" seconds")
