#include <stdio.h>
#include <stdlib.h>

unsigned long fib2_inner(unsigned long a,unsigned long b,unsigned long n)
{
 if(n==0) return (a);
 return (fib2_inner(b,a+b,n-1));
}


int main(int argc, char **argp)
{
 long fib_num=5;
// printf("0=%s 1=%s\n",argp[0],argp[1]);
 if(argc>1) fib_num=atoi(argp[1]);
 else {
 	printf("usage is fib2 num\n");
	return 1;
 };
 printf("fibonachi num %ld = %lu\n",fib_num,fib2_inner(0,1,fib_num));
 return 0;
}