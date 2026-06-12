# test loop with fori
print("--- e5.cmd")

p=0, l=0; 

fori(i=1;101;1){ 
	fori(j=1;800001;1) {
		l = i*j+32.1
		p += l
	} 
}

p: