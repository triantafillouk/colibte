# test loop with fori

cls

p=0; l=0; 
print(_time)	: "Wed Feb  1 13:47:48 2023"
show_time("Start fori test loop 1",0)

fori(i=1;101;1){ 
	for(j=1;j<800001;j +=2) { 
		l = i*j+32.1;
		p += l
	} 
	show_time("loop:"+i+" p="+p,1);
}
_time	: "Wed Feb  1 10:56:09 2023"
#message(_version+" i<"+i+" and j< "+j+" sets p="+p)
print(_time)	: "Wed Feb  1 10:56:09 2023"
tdif=show_time("End test",2)
print(_version+": p="+p+" Test concluded in "+tdif+" seconds")
