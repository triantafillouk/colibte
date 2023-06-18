# test loop with fori

cls

p=0; l=0; 
print(_time)	:
show_time("Start fori test loop 1",0)

fori(i=1;101;1){ 
	fori(j=1;800001;1) { 
		l = i*j+32.1;
		p += l
	} 
#	show_time("loop:"+i+" p="+p,1);
}
_time	: "Wed Feb 24 19:48:43 2016"
#message(_version+" i<"+i+" and j< "+j+" sets p="+p)
print(_time)	: "Wed Feb 24 19:48:43 2016"
tdif=show_time("End test",2)
print(_version+": p="+p+" Test concluded in "+tdif+" seconds")
