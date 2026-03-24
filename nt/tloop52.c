// test loop with proc inside

#include <stdio.h>

double fact(double num)
{
 if(num<1) 
 {
 	return(1);
 } else {
	double f=num*fact(num-1);
	return(f);
 }
}

int main(int argc,char **argp)
{
	printf("fact(30)=%f\n",fact(3));

	double p=0; 
	double l=0; 
	printf("Start loop\n");

	for(int i=1;i<11;i++){ 
		for(int j=1;j<800001;j++) { 
			l = i*j+32;
			p = p+l+fact(10);
		} 
		printf("loop:%d p=%.0f\n",i,p);	
	}
	return 0;
}
