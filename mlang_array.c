/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

	interpreter array operations support
*/

double determinant(array_dat *a);
array_dat * cofactor2(array_dat *a,double det);
array_dat * transpose2(array_dat *a,array_dat *b,double det);
array_dat * cofactor(array_dat *numa,double det);
array_dat *cofactor2inverse(array_dat *faca, double det);
void print_array1(char *title,array_dat *adat);
struct array_dat *new_array(int rows,int cols);
struct array_dat *new_array_similar(array_dat *a);
void allocate_array(struct array_dat *adat);

void init_array(struct array_dat *array, int rows,int cols)
{
	int ctype=VTYPE_ARRAY;	/* default is numeric!!  */
	if(ex_nums) ctype=VTYPE_ARRAY;
	else if(ex_nquote) ctype=VTYPE_SARRAY;
	else if(ex_nquote>0 && ex_nums>0) ctype=VTYPE_AMIXED;
	else if(ex_nvars) ctype=VTYPE_DYNAMIC;
	array->rows=rows;
	array->cols=cols;
	array->atype=ctype;
	array->astat=0;
	array->dat=NULL;
	ex_vtype=ctype;
	/* init again after use the ex values!  */
	ex_nums=0;ex_nquote=0;ex_nvars=0;
	ex_vtype=VTYPE_ARRAY;
}


void free_array_dat(struct array_dat *adat)
{
 if(adat==NULL) {
	err_num=251;
 	err_str="free_array_dat: array is already NULL!!";
	return;
 };
 if(adat->astat==0) { 
 	err_num=252;
	err_str="free_array_dat: astat is already zero !!!!!!!";
 };
 if(adat->rows==0 || adat->cols==0) {
 	err_num=253;
	err_str="error array with zero dims";
 	ERROR("error array with zero dims %d %d",adat->rows,adat->cols);
	return;
 };
 if(adat->dval==NULL) return;
 if(adat->rows==1 || adat->cols==1) {
 	if(adat->dval!=NULL) free(adat->dval);
	adat->dval=NULL;
 } else {
 	int j;
 	for(j=0;j<adat->rows;j++) {
		free(adat->dval2[j]);
	};
	free(adat->dval2);
 };
 adat->astat=0;
 return;
}

void free_array(char *spos,struct array_dat *adat)
{
 if(adat==NULL) return;
 free_array_dat(adat);
 free(adat);
}

void allocate_array(struct array_dat *adat)
{
 int i;
 if(adat->astat==0 || adat->atype==VTYPE_DYNAMIC) {	/* new/renew  */
 	if(adat->atype==VTYPE_ARRAY) {	/* allocate num array  */

 if(adat->rows >1 && adat->cols>1) {
 	int i,j;
 	if(adat->dval2 != NULL) { 
//		free first the rows
		free(adat->dval2);
	};
	adat->dval2=(double **)malloc(sizeof(double)*adat->rows);
	for(j=0;j<adat->rows;j++) {
		adat->dval2[j]=(double *)malloc(sizeof(double)*adat->cols);
		for(i=0;i<adat->cols;i++) adat->dval2[j][i]=0;
	};
	adat->astat=3;
 } else {
	int dim=1;
	int i;

 	if(adat->dval != NULL) free(adat->dval);
	if(adat->rows > 1) dim=adat->rows; else dim=adat->cols;
 	adat->dval=(double *)malloc(sizeof(double)*dim);
	for(i=0;i<dim;i++) adat->dval[i]=0;
	adat->astat=3;
 };

	};
	if(adat->atype==VTYPE_SARRAY) {	/* allocate string array  */
	 	if(adat->dat != NULL) free(adat->dat);	/* free string array  */
		adat->sval=(char **)malloc(sizeof(char *)*adat->rows*adat->cols);
		for(i=0;i< adat->rows*adat->cols;i++) adat->sval[i]=NULL;
	};
	if(adat->atype==VTYPE_AMIXED || adat->atype==VTYPE_DYNAMIC) {	/* new mixed  */
		if(adat->mval!=NULL) free(adat->mval);
		adat->mval=(struct MVAR *)malloc(sizeof(struct MVAR)*adat->rows*adat->cols);
		for(i=0;i< adat->rows*adat->cols;i++) {adat->mval[i].dval=0;adat->mval[i].vtype=VTYPE_NUM;};
	};
 }
}

struct array_dat *alloc_array()
{
 static int array_num=0;
 array_dat *na=(array_dat *)malloc(sizeof(struct array_dat));
 na->anum = array_num++;
 return(na);
}

struct array_dat *new_array_similar(array_dat *a)
{
 array_dat *na;
 na=alloc_array();
 na->rows=a->rows;
 na->cols=a->cols;
 na->atype=a->atype;
 na->astat=0;
 na->dat=NULL;
 allocate_array(na);
 return(na); 
}

struct array_dat *new_array(int rows,int cols)
{
	struct array_dat *array;
	array=alloc_array();
	init_array(array,rows,cols);
	return(array);
}

array_dat * dup_array_add1(array_dat *a,double plus)
{
 array_dat *na;	/* new array  */
 na=new_array_similar(a);
 	if(na->atype==VTYPE_ARRAY) {	/* allocate num array  */
		int i,j,dim=1;
		if(na->rows>1 && na->cols>1) {
			for(j=0;j<na->rows;j++) for(i=0;i<na->cols;i++) 
				na->dval2[j][i]=a->dval2[j][i]+plus;
		} else {
			if(na->rows > 1) dim=na->rows; else dim=na->cols;
			for(i=0;i<dim;i++) na->dval[i]= a->dval[i]+plus;
		};
	};
 na->astat=3;
 return(na);
}
	/* subtruct array from variable  */
array_dat * dup_array_sub1(array_dat *a,double plus)
{
 array_dat *na;	/* new array  */
 na=new_array_similar(a);
 	if(na->atype==VTYPE_ARRAY) {	/* allocate num array  */
		int i,j,dim=1;
		if(na->rows>1 && na->cols>1) {
			for(j=0;j<na->rows;j++) for(i=0;i<na->cols;i++) 
				na->dval2[j][i]=plus - a->dval2[j][i];
		} else {
			if(na->rows > 1) dim=na->rows; else dim=na->cols;
			for(i=0;i<dim;i++) na->dval[i]=plus - a->dval[i];
		};
	};
 na->astat=3;
 return(na);
}

array_dat * dup_array_mul1(array_dat *a,double num)
{
 array_dat *na;	/* new array  */
 na=new_array_similar(a);
 	if(na->atype==VTYPE_ARRAY) {	/* allocate num array  */
		int i,j,dim=1;
		if(na->rows>1 && na->cols>1) {
			for(j=0;j<na->rows;j++) for(i=0;i<na->cols;i++) 
				na->dval2[j][i]=a->dval2[j][i]*num;
		} else {
			if(na->rows > 1) dim=na->rows; else dim=na->cols;
			for(i=0;i<dim;i++) na->dval[i]=a->dval[i]*num;
		};
	};
 na->astat=3;
 return(na);
}

array_dat * dup_array_mod1(array_dat *a,double num)
{
 array_dat *na;	/* new array  */
 na=new_array_similar(a);
 	if(na->atype==VTYPE_ARRAY) {	/* allocate num array  */
		int i,j,dim=1;
		if(na->rows>1 && na->cols>1) {
			for(j=0;j<na->rows;j++) for(i=0;i<na->cols;i++) 
				na->dval2[j][i]=modulo(a->dval2[j][i],num);
		} else {
			if(na->rows > 1) dim=na->rows; else dim=na->cols;
			for(i=0;i<dim;i++) na->dval[i]=modulo(a->dval[i],num);
		};
	};
 na->astat=3;
 return(na);
}

array_dat * dup_array_power(array_dat *a,double num)
{
 array_dat *na;	/* new array  */
 na=new_array_similar(a);
 	if(na->atype==VTYPE_ARRAY) {	/* allocate num array  */
		int i,j,dim=1;
		if(na->rows>1 && na->cols>1) {
			for(j=0;j<na->rows;j++) for(i=0;i<na->cols;i++) 
				na->dval2[j][i]=pow(a->dval2[j][i],num);
		} else {
			if(na->rows > 1) dim=na->rows; else dim=na->cols;
			for(i=0;i<dim;i++) na->dval[i]=pow(a->dval[i],num);
		};
	};
 na->astat=3;
 return(na);
}


array_dat * array_mul2(array_dat *aa,array_dat *ba)
{
 array_dat *na;	/* new array  */
 if(aa->cols==ba->rows){
   int result_cols=ba->cols;
   int result_rows=aa->rows;
   int i,j,k;
   // MESG("array_mul2: a (%d %d) b (%d %d)",aa->rows,aa->cols,ba->rows,ba->cols);
	na=new_array(result_rows,result_cols);
	allocate_array(na);
			double **n=na->dval2;
			if(aa->cols==1) { 
				double *a;
				double *b;
				a=aa->dval;
				b=ba->dval;
				for(i=0;i<aa->rows;i++) n[i][i]=a[i]*b[i];
			} else if(ba->cols==1) {
				double *a;
				double *b;
				double v=0;
				a=aa->dval;
				b=ba->dval;
				for(i=0;i<aa->rows;i++) v += a[i]*b[i];
				ex_value=v;
				n[0][0]=v;
			} else {
			double **a;
			double **b;
				a=aa->dval2;
				b=ba->dval2;
				for(j=0;j< result_rows;j++) {
					for(i=0;i< result_cols;i++){
						double v=0;
						for(k=0;k<aa->rows;k++) v += a[j][k]*b[k][i];
						n[j][i]=v;
					};
				}
			}
			na->astat=3;
		//	print_array1("mul2 result is ",na);
			return(na);
		} else {
			/* this is an error!!  */
		 	err_num=261;
			err_str="To multiply arrays, number of columns of the first must be equal to the number of rows of the second!!";
			return(aa);
		}
}

array_dat * array_add2(array_dat *aa,array_dat *ba)
{
 array_dat *na;	/* new array  */
 if(aa->rows==ba->rows && aa->cols==ba->cols){
 na=new_array_similar(aa);
 	if(na->atype==VTYPE_ARRAY) {	/* allocate num array  */
		int i,j;
		if(aa->cols==1 || aa->rows==1) {
			for(i=0;i < aa->cols*aa->rows;i++) na->dval[i]=aa->dval[i]+ba->dval[i];
		} else {
			double **a=aa->dval2;
			double **b=ba->dval2;
			double **n=na->dval2;
			for(j=0;j<aa->rows;j++) for(i=0;i<aa->cols;i++){
				n[j][i] = a[j][i]+b[j][i];
			};
		}
	};
	na->astat=3;
	return(na);
 } else {
 	err_num=254;
	err_str="To add arrays they must have the same dimensions";
	return(NULL);
 };
}

array_dat * array_sub2(array_dat *aa,array_dat *ba)
{
 array_dat *na;	/* new array  */
 if(aa->rows==ba->rows && aa->cols==ba->cols){
 na=new_array_similar(aa);
 	if(na->atype==VTYPE_ARRAY) {	/* allocate num array  */
		int i,j;
		if(aa->cols==1 || aa->rows==1) {
			for(i=0;i < aa->cols*aa->rows;i++) na->dval[i]=aa->dval[i]-ba->dval[i];
		} else {
			double **a=aa->dval2;
			double **b=ba->dval2;
			double **n=na->dval2;
			for(j=0;j<aa->rows;j++) for(i=0;i<aa->cols;i++){
				n[j][i] = a[j][i]-b[j][i];
			};
		}
	};
	na->astat=3;
	return(na);
 } else {
 	err_num=255;
	err_str="To substract arrays they must have the same dimensions";
	return(NULL);
 };
}

void array_add1(array_dat *na,double plus)
{
 if(na->astat!=3) {
 	err_num=256;
 	err_str="error: cannot add to non defined array!";
	return;
 };
 	if(na->atype==VTYPE_ARRAY) {	/* allocate num array  */
		int i,j,dim=1;
		if(na->rows>1 && na->cols>1) {
			for(j=0;j<na->rows;j++) for(i=0;i<na->cols;i++) 
				na->dval2[j][i] += plus;
		} else {
			if(na->rows > 1) dim=na->rows; else dim=na->cols;
			for(i=0;i<dim;i++) na->dval[i] += plus;
		};
	};
}


void array_sub1(array_dat *na,double plus)
{
 if(na->astat!=3) {
 	err_num=257;
	err_str="error: cannot sub to non defined array!";
	return;
 };
 	if(na->atype==VTYPE_ARRAY) {	/* allocate num array  */
		int i,j,dim=1;
		if(na->rows>1 && na->cols>1) {
			for(j=0;j<na->rows;j++) for(i=0;i<na->cols;i++) 
				na->dval2[j][i] = plus - na->dval2[j][i];
		} else {
			if(na->rows > 1) dim=na->rows; else dim=na->cols;
			for(i=0;i<dim;i++)  na->dval[i] = plus - na->dval[i];
		};
	};
}

void array_mul1(array_dat *na,double num)
{
 if(na==NULL) { err_num=258;ERROR("array_mul1: NULL array!");return;};
 if(na->astat!=3) {
 	err_num=259;
 	ERROR("error: cannot mul to non defined array! astat=%d",na->astat);
	return;
 };
 	if(na->atype==VTYPE_ARRAY) {	/* allocate num array  */
		int i,j,dim=1;
		if(na->rows>1 && na->cols>1) {
			for(j=0;j<na->rows;j++) for(i=0;i<na->cols;i++) 
				na->dval2[j][i] *= num;
		} else {
			if(na->rows > 1) dim=na->rows; else dim=na->cols;
			for(i=0;i<dim;i++)  na->dval[i] *= num;
		};
	};
}

void array_mod1(array_dat *na,double num)
{
 if(na==NULL) { err_num=258;ERROR("array_mul1: NULL array!");return;};
 if(na->astat!=3) {
 	err_num=259;
 	ERROR("error: cannot mul to non defined array! astat=%d",na->astat);
	return;
 };
 	if(na->atype==VTYPE_ARRAY) {	/* allocate num array  */
		int i,j,dim=1;
		if(na->rows>1 && na->cols>1) {
			for(j=0;j<na->rows;j++) for(i=0;i<na->cols;i++) 
				na->dval2[j][i] = modulo(na->dval2[j][i],num);
		} else {
			if(na->rows > 1) dim=na->rows; else dim=na->cols;
			for(i=0;i<dim;i++)  na->dval[i] = modulo(na->dval[i],num);
		};
	};
}

void array_power(array_dat *na,double num)
{
 if(na==NULL) { err_num=258;ERROR("array_power: NULL array!");return;};
 if(na->astat!=3) {
 	err_num=259;
 	ERROR("error: cannot power to non defined array! astat=%d",na->astat);
	return;
 };
 	if(na->atype==VTYPE_ARRAY) {	/* allocate num array  */
		int i,j,dim=1;
		if(na->rows>1 && na->cols>1) {
			for(j=0;j<na->rows;j++) for(i=0;i<na->cols;i++) 
				na->dval2[j][i] = pow(na->dval2[j][i],num);
		} else {
			if(na->rows > 1) dim=na->rows; else dim=na->cols;
			for(i=0;i<dim;i++)  na->dval[i] = pow(na->dval[i],num);
		};
	};
}

/*For calculating Determinant of the Matrix */
double determinant(array_dat *aa)
{
  array_dat *ba;
  double s=1,det=0,**a,**b;
  int i,j,m,n,c,k;
  a=aa->dval2;
  k=aa->rows-1;
  ex_nvars=0;
  ex_nums=1;	/* this is a numerical array!  */
//  print_array1("find determinant",aa);
  if (k==1)
    {
	det=a[0][0]*a[1][1]-a[0][1]*a[1][0];
    }
  else
    {
	ba=new_array(k,k);allocate_array(ba);
  	b=ba->dval2;
    det=0;

	 for(c=0;c<k+1;c++) {
	 	n=1;
	 	for(j=0;j<k;j++) {
			m=0;
			for(i=0;i<k;i++){
				if(i==c) m++;
				b[j][i]=a[n][m];	
				m++;
			};
			n++;
		};
	 	det=det + s*a[0][c] * determinant(ba);
		s=s*(-1);
	 };

    };
    return (det);
}

array_dat * cofactor2(array_dat *numa,double det)
{
 double **b,**fac,**num;
 array_dat *ba,*faca,*inverse;

 int p,q,m,n,i,j,dim;
 dim=numa->rows;
 num=numa->dval2;
 ex_nums=1;	/* this is a numerical array!  */
  faca=new_array(dim,dim);allocate_array(faca);
  fac=faca->dval2;

 if(dim>2) {
  ba=new_array(dim-1,dim-1);allocate_array(ba);
  b=ba->dval2;
 for (q=0;q<dim;q++)
 {
   for (p=0;p<dim;p++)
    {
     m=0;
     n=0;
     for (i=0;i<dim;i++)
     {
       for (j=0;j<dim;j++)
        {
          if (i != q && j != p)
          {
            b[m][n]=num[i][j];
            if (n<(dim-2))
             n++;
            else
             {
               n=0;
               m++;
               }
            }
        }
      }
      fac[q][p]=pow(-1,q + p) * determinant(ba);
    }
  };
 } else {
 double b=0;
 for (q=0;q<dim;q++)
 {
   for (p=0;p<dim;p++)
    {
     m=0;
     n=0;
     for (i=0;i<dim;i++)
     {
       for (j=0;j<dim;j++)
        {
          if (i != q && j != p)
          {
	         b=num[i][j];
            if (n<(dim-2))
             n++;
            else
             {
               n=0;
               m++;
               }
            }
        }
      };
      fac[q][p]=pow(-1,q + p) * b;
    };
  };
 };
//  print_array1("fac",faca);
  inverse=transpose2(numa,faca,det);
//  print_array1("Inverse",inverse);
  return(inverse);
}

array_dat * cofactor(array_dat *numa,double det)
{
 double **b,**fac,**num;
 array_dat *ba,*faca;

 int p,q,m,n,i,j,dim;
 dim=numa->rows;
 num=numa->dval2;
 ex_nums=1;	/* a numerical array!  */
  faca=new_array(dim,dim);allocate_array(faca);
  fac=faca->dval2;

 if(dim>2) {
  ba=new_array(dim-1,dim-1);allocate_array(ba);
  b=ba->dval2;
 for (q=0;q<dim;q++)
 {
   for (p=0;p<dim;p++)
    {
     m=0;
     n=0;
     for (i=0;i<dim;i++)
     {
       for (j=0;j<dim;j++)
        {
          if (i != q && j != p)
          {
            b[m][n]=num[i][j];
            if (n<(dim-2))
             n++;
            else
             {
               n=0;
               m++;
               }
            }
        }
      }
      fac[q][p]=pow(-1,q + p) * determinant(ba);
    }
  };
 } else {
 double b=0;
 for (q=0;q<dim;q++)
 {
   for (p=0;p<dim;p++)
    {
     m=0;
     n=0;
     for (i=0;i<dim;i++)
     {
       for (j=0;j<dim;j++)
        {
          if (i != q && j != p)
          {
	         b=num[i][j];
            if (n<(dim-2))
             n++;
            else
             {
               n=0;
               m++;
               }
            }
        }
      };
      fac[q][p]=pow(-1,q + p) * b;
    };
  };
 }

 return(faca);
}

/*Finding transpose of matrix*/  
array_dat *transpose2(array_dat *numa,array_dat *faca, double det)
{
  int i,j;
  array_dat *inversea;
  double **inverse,**fac;
  int dim=numa->rows;
  fac=faca->dval2;
  ex_nums=1;	/* a numerical array  */
  inversea=new_array(dim,dim); allocate_array(inversea);
  inverse = inversea->dval2;

//  d=determinant(numa);
	/* use the adjoint and divide with determinant!  */
  for (i=0;i<dim;i++)
    {
     for (j=0;j<dim;j++)
       {
        inverse[i][j]=fac[j][i] / det;
        }
    }
 return(inversea);
}

array_dat *transpose(array_dat *array1)
{
  array_dat *tarray;
  ex_nums=1;	/* a numerical array  */
  tarray=new_array(array1->cols,array1->rows);
  allocate_array(tarray);
  if(array1->rows==1 || array1->cols==1) {
	int dim=1;
	int i;
	if(array1->rows>1) dim=array1->rows;
	else dim=array1->cols;
	for (i=0;i<dim;i++) tarray->dval[i]=array1->dval[i];
  } else {
	  int i,j;
	  for (i=0;i<array1->rows;i++) {
	     for (j=0;j<array1->cols;j++) {
	        tarray->dval2[j][i]=array1->dval2[i][j];
	     };
	  };
  };
 return(tarray);
}

array_dat *cofactor2inverse(array_dat *faca, double det)
{
  int i,j;
  array_dat *inversea;
  double **inverse,**fac;
  int dim=faca->rows;
  fac=faca->dval2;
  ex_nums=1;	/* a numerical array  */
  inversea=new_array(dim,dim); allocate_array(inversea);
  inverse = inversea->dval2;

  /* use the adjoint of coofactor and divide with determinant!  */
  for (i=0;i<dim;i++)
    {
     for (j=0;j<dim;j++)
       {
        inverse[i][j]=fac[j][i] / det;
        }
    }
 return(inversea);
}

void print_array1(char *title,array_dat *adat)
{
	char so[MAXLLEN];
	char s2[128];
	int i,j;
	so[0]=0;
	if(adat==NULL) {
		err_num=260;
		err_str="%s: NULL array!!!!!!!!!!!!!!!!!!";
		ERROR("%s: NULL array!!!!!!!!!!!!!!!!!!",title);
		out_print("Null array!",1);
		return;
	};
	
	snprintf(so,MAXLLEN,"# %s Array %s, %d rows=%d cols=%d astat=%d",title,ex_name,adat->anum,adat->rows,adat->cols,adat->astat);
	out_print(so,1);
	strcpy(so,"");
	if(adat->astat!=0) {
	if(adat->rows>1 && adat->cols>1) {
		snprintf(so,MAXLLEN,"#   :");
		for(i=0;i< adat->cols;i++) { snprintf(s2,128,"%8d",i);strcat(so,s2);};
		out_print(so,1);
		for(j=0;j<adat->rows;j++){
			snprintf(so,128," %3d: ",j);
			for(i=0;i<adat->cols;i++) {
				snprintf(s2,128,"%7.3f ",adat->dval2[j][i]);
				strcat(so,s2);
			};
			out_print(so,1);
		};
	} else {
		if(adat->cols==1) {
			for(i=0;i<adat->rows;i++) {
				snprintf(so,128," %3d: %f",i,adat->dval[i]);
				out_print(so,1);
			};
		} else {
			for(i=0;i<adat->cols;i++) {
				snprintf(s2,128,"%05.3f(%d) ",adat->dval[i],i);
				strcat(so,s2);
			};
			out_print(so,1);
		};		
	};};
	out_print("---------------------------------------------",1);
}

#if	0
/* Array TODO */

int new_array0(int n)
{
	/* get rows.cols  */
	return(1.0);
}

int new_array1(int n)
{
	/* get rows.cols  */
	return(1.0);
}

int load_array(int n)
{
	/* get filename  */
	return(1.0);
}

int save_array(int n)
{
	/* get filename  */
	return(1.0);
}

int load_csv(int n)
{
	/* get filename  */
	return(1.0);
}

int save_csv(int n)
{
	/* get filename  */
	return(1.0);
}

#endif
/* -- */
