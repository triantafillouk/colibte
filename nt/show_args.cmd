# show arguments
cls
l1 = args_size()
a= args(0)
print("args size is "+l1+" arg0="+a)
fori(i=0;l1;1){
	a=args(i)
	print("- "+i+": ["+a+"]")
}
