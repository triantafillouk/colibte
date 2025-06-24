	/* test file comparing rline,getline,fgets  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLLEN 1024

int rline(char *string,int max_len,FILE *f)
{
 char *s=string;
 int c;
 // int size=0;
 max_len--;
 char *end_string=string+max_len-1;

 while(s<end_string) {
	c=fgetc(f);
	if(c==0x0D) continue;
	if(c=='\n') { *s=0; return (s-string);};
	if(c==EOF) {
		*s=0;
		if(s==string) return EOF;
		return (s-string);
	};
	*s++=c;
 };*s=0;
 return(s-string);
}

int color_scheme_read(char *fname)
{
 FILE *f1;
 // char *fname="/mnt/c/D/src/colibte/.colors";

 char *rb=malloc(MAXLLEN);
 // char bline[MAXLLEN];
 // size_t line_len=MAXLLEN;
 // if((fname=find_file("",".colors",1,0))==NULL) return FALSE;
 fprintf(stderr,"file found [%s]\n",fname);
 f1=fopen(fname,"r");
 if(f1!=NULL) {
 int ind=0;
#if	0
 int len;
 while(rline(rb,MAXLLEN,f1)!=EOF)
 // while((len=fgets(rb,MAXLLEN,f1)!=NULL))
 {
	 // strtok(rb, "\n");
	fprintf(stdout,"%2d -- [%s] \n",ind++,rb);
	ind++;
 };
#else
 size_t nread;
 size_t len=MAXLLEN;
 while ((nread = getline(&rb, &len, f1)) != -1) {
	// if(rb[strlen(rb)-1]=='\n') rb[strlen(rb)-1]=0;
 	strtok(rb, "\n");
	fprintf(stdout,"%2d -- [%s] linelen=%ld\n",ind,rb,nread);
	ind++;
 }
#endif
 printf("read %d lines\n",ind);
 } else fclose(f1);
 free(rb);
 return 1;
}

int main(int argc,char **arg)
{
 if(argc<1) printf("%d usage is rl fname\n",argc);
 else color_scheme_read(arg[1]);
}
