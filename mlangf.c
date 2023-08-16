	#include "xe.h"
#include "mlang.h"

extern char *saved_string;
extern int ex_vtype;
//extern tok_struct *tok;
extern array_dat *ex_array;
extern array_dat *main_args;
extern char *ex_name;
extern FILEBUF *cbfp;

tok_struct *current_token();

void ntoken();
int check_token(int type);
double determinant(array_dat *aa);
array_dat * cofactor2(array_dat *numa,double det);
array_dat *transpose(array_dat *array1);
void clean_saved_string(int new_size);
void print_array1(char *title,array_dat *adat);
void get_uppercase_string(char *upper, char *string);
void get_lowercase_string(char *lower, char *string);
int deq(double v1,double v2);

MVAR va[3];

void get_function_args (int number_of_args)
{
	int i;
	int f_entry;
	// tok_struct *tok = current_token();
	f_entry=entry_mode;
	entry_mode=KNORMAL;
	// MESG("get_function_args: %d [%d %s]",number_of_args,tok->tnum,tok->tname);
	ex_vtype=VTYPE_NUM;
	if(number_of_args) {
		/* if we have arguments, check for parenthesis, then get the arguments  */
		// va = (MVAR *)malloc(sizeof(MVAR)*number_of_args);
		for(i=0;i< number_of_args;i++) { 
			ntoken();
#if	0
			tok=current_token();
			MESG("get_function_args:%d: [%d %s]",i,tok->tnum,tok->tname);
#endif
			double value = num_expression();
			va[i].vtype=ex_vtype;
			if(ex_vtype==VTYPE_STRING) { 
				va[i].sval=saved_string;saved_string=NULL;
				// MESG("%d: string: [%s]",i,va[i].sval);
			} else if (ex_vtype==VTYPE_ARRAY||ex_vtype==VTYPE_SARRAY) {
				va[i].adat=ex_array;
				// MESG("%d: array",i);
			}
			else {
				va[i].dval=value;
				// MESG("%d: numeric %f",i,va[i].dval);
			};
		};
#if	0
		tok=current_token();
		MESG("	%d:after [%d %s]",i,tok->tnum,tok->tname);
#endif
		ntoken();
	} else {;
		if(check_token(TOK_LPAR)) {
			// MESG("	get_function_args: TOK_LPAR");
				ntoken();
				ntoken();
		};
	};

	entry_mode=f_entry;
// 	return va;
}

// MVAR va[3];

void get_numeric_args (int number_of_args)
{
	int i;
	// MESG("get_numeric_args: %d",number_of_args);
	for(i=0;i< number_of_args;i++ ) { 
		ntoken();
		double value = num_expression();
		va[i].vtype=ex_vtype;
		va[i].dval=value;
	};
	ntoken();
}

double get_numeric_arg ()
{
	// MESG("get_numeric_args:");
	ntoken();
	double value = num_expression();
	ntoken();
	return value;
}

double uf_len()
{
	get_function_args(1);
	double value=0;
	if(va->vtype==VTYPE_STRING) { value =strlen(va->sval);}
	else if(ex_vtype==VTYPE_ARRAY || ex_vtype==VTYPE_SARRAY) {
		array_dat *arr=va->adat;		
		value=arr->rows*arr->cols;
	};
	ex_vtype=VTYPE_NUM;

	return value;
}

double uf_array_cols()
{
	get_function_args(1);
	ex_vtype=VTYPE_NUM;
	if(va->vtype==VTYPE_ARRAY||va->vtype==VTYPE_SARRAY) {
		return va->adat->cols;
	} else return 0;
}

double uf_array_rows()
{
	get_function_args(1);
	ex_vtype=VTYPE_NUM;
	if(va->vtype==VTYPE_ARRAY||va->vtype==VTYPE_SARRAY) {
		return va->adat->rows;
	} else return 0;
}

/* clear output buffer */
double uf_cls()
{
	ntoken();
	cls_fout("[out]");
	MESG("<- %s ------------------------------------------------------------->",VERSION);
	return 0;
}

double uf_index()
{
	ntoken();
	MESG("new index!");
	ex_vtype=VTYPE_TREE;
	return 0;
}

/* get array determinant */
double uf_determinant()
{
	double value=0;
	get_function_args(1);
	if(va->vtype==VTYPE_ARRAY) {
		array_dat *arr = va->adat;

		if(arr->rows==arr->cols) value=determinant(arr);
		else {
			// MESG("wrong dimensions!");
			syntax_error("wrong dimensions for determinant",203);
		};
	} else {
		syntax_error("Not an array!",204);
	};
	ex_vtype=VTYPE_NUM;
	// free(va);
	return value;
}

double uf_inverse()
{
	double value=0;
	get_function_args(1);

	if(ex_vtype==VTYPE_ARRAY) {
		array_dat *arr = va->adat;

		if(arr->rows==arr->cols) {
			value=determinant(arr);
		} else {
			syntax_error("wrong dimensions for determinant",205);
		};
		if(value!=0) {
			array_dat *inverse;
			inverse=cofactor2(arr,value);
			ex_array=inverse;
			ex_name="Inverse";
		};
	} else {
		syntax_error("Not an array!",206);
	}
	return value;
}

double uf_transpose()
{
	get_function_args(1);
	double value=0;
	if(ex_vtype==VTYPE_ARRAY) {
		array_dat *arr  = va->adat;
		array_dat *tarray;
		tarray = transpose(arr);
		ex_array=tarray;
		ex_name="Tranpose";
		value=1;
	} else {
		syntax_error("Not an array!",207);
	};
	// free(va);
	return value;
}

double uf_left()
{
	get_function_args(2);

	if(va[0].vtype==VTYPE_STRING && va[1].vtype==VTYPE_NUM ) {
	set_sval(va[0].sval);
	} else set_sval("");
	ex_vtype=VTYPE_STRING;
	return 0;
}

double uf_right()
{
	get_function_args(2);

	if(va[0].vtype==VTYPE_STRING && va[1].vtype==VTYPE_NUM ) {
		int r1=(int)va[1].dval;
		// MESG("right: r1=%d",r1);
		if(strlen(va[0].sval)<r1) r1=strlen(va[0].sval);
		clean_saved_string(r1);
		memcpy(saved_string,va[0].sval+(strlen(va[0].sval)-r1),r1);
		saved_string[r1]=0;
	} else {
		syntax_error("right: wrong type of args",206);
		set_sval("");
	};

	ex_vtype=VTYPE_STRING;
	return 0;
}

double uf_mid()
{
	get_function_args(3);

		if(va[0].vtype==VTYPE_STRING && va[1].vtype==VTYPE_NUM && va[2].vtype==VTYPE_NUM) 
		{
		if((int)va[1].dval>strlen(va[0].sval) || va[2].dval==0) {
			set_sval("");
		} else {
			clean_saved_string((int)va[2].dval);
			memcpy(saved_string,va[0].sval+(int)va[1].dval,va[2].dval);
			saved_string[(int)va[2].dval]=0;
		};
		} else {
			syntax_error("mid: wrong_type of args",100);
			set_sval("");
		};
		ex_vtype=VTYPE_STRING;
	return 0;	
}


double uf_print()
{
	tok_struct *tok=current_token();
	int args=tok->number_of_args;
	// MESG("uf_print: ex_vtype=%d tnum=%d args=%d",ex_vtype,tok->tnum,tok->number_of_args);
	int i;
	double value=0;
	for(i=0;i<args;i++) {
		ntoken();
		tok=current_token();
		// MESG("	eval arg %d tnum=%d ttype=%d",i,tok->tnum,tok->ttype);
		value=num_expression();
		// MESG("		val=%f s='%s'",value,saved_string);
		switch(ex_vtype) {
			case VTYPE_ARRAY:
			case VTYPE_SARRAY:
			case VTYPE_AMIXED:
				if(i>0) out_print("",1);
				print_array1("",ex_array);break;
			case VTYPE_NUM:{
				char *p_out=(char *)malloc(128);
				long l0=value;
				if(l0==value) snprintf(p_out,80,"%ld",l0);
				else snprintf(p_out,128,"%f",value); 
				out_print(p_out,0);
				free(p_out);
				break;
			};
			case VTYPE_STRING:{
				char *p_out=strdup(saved_string);
				ex_vtype=VTYPE_STRING;
				out_print(p_out,0);
				free(p_out);
			};
		};
		// MESG("	if: after switch!");
		// tok=current_token();
		// MESG("	if: after switch tnum=%d ttype=%d",tok->tnum,tok->ttype);
	};
	out_print("",1);
	// MESG("uf_print: skip function right parenthesis");
	ntoken();
	// tok=current_token();
	// MESG("end of if_print: show current token!");
	// MESG("end of uf_print: num=%d",tok->tnum);
	// MESG("uf_print: >>");
	return value;
}

double uf_show_time()
{
	get_function_args(2);
	double value=0;
	if(va[0].vtype==VTYPE_STRING) {
		set_sval(va[0].sval);
		value=show_time(saved_string,va[1].dval);
	} else {
		syntax_error("error in stime",312);
	}

	return value;
}


double uf_upper()
{
	get_function_args(1);
	if(va[0].vtype==VTYPE_STRING) {
		clean_saved_string(strlen(va[0].sval));
		get_uppercase_string(saved_string,va[0].sval);
	} else {
		syntax_error("upper: wrong_type of args",100);
		set_sval("");	
	};
	ex_vtype=VTYPE_STRING;
	return 0;
}

double uf_lower()
{
	get_function_args(1);
	if(va[0].vtype==VTYPE_STRING) {
		clean_saved_string(strlen(va[0].sval));
		get_lowercase_string(saved_string,va[0].sval);
	} else {
		syntax_error("lower: wrong_type of args",100);
		set_sval("");
	};
	ex_vtype=VTYPE_STRING;
	return 0;
}

double uf_ascii()
{
	get_function_args(1);
	double value=0;
	if(va[0].vtype==VTYPE_STRING) {
	 value=va[0].sval[0];
	} else {
		syntax_error("ascii: wrong_type of args",100);
	};
	ex_vtype=VTYPE_NUM;
	// free(va);
	return value;
}

double uf_chr()
{
	get_function_args(1);
	clean_saved_string(1);
	saved_string[0] = (int)va[0].dval;
	saved_string[1] = 0;

	ex_vtype=VTYPE_STRING;
	// free(va);
	return 0;
}

double uf_getchar()
{
	ntoken();
	clean_saved_string(1);
	if(execmd) {
		saved_string[0]=getchar();
	} else {
		saved_string[0] = getcmd();
	};
	saved_string[1] = 0;

	ex_vtype=VTYPE_STRING;
	return 0;
}

/* returns big! int */
double uf_rand()
{
 // static long =
 long max=(long) get_numeric_arg();
 ex_vtype=VTYPE_NUM;
 long value=random() % max;
 // MESG("uf_rand:%ld",value);
 // ex_value=23.33;
 return (double)value;
}

double uf_seed()
{
	long seed=(long) get_numeric_arg();
	ex_vtype=VTYPE_NUM;
	srand(seed);
	return 0;
}


double uf_abs()
{
	get_function_args(1);
	double value= fabs(va[0].dval);
	return value;
}

/* string of a value */
double uf_sindex()
{
	double value=0;
	get_function_args(2);
	if(va[0].vtype==VTYPE_STRING) {
		value = sindex(va[0].sval, va[1].sval);
	} else {
		syntax_error("s_index: wrong_type of args",100);
	};
	ex_vtype=VTYPE_NUM;

	// free(va);
	return value;
}

/* string of a value */
double uf_string()
{
	get_function_args(1);
	if(va[0].vtype==VTYPE_NUM) {
		// MESG("uf_string: %f",va[0].dval);
		clean_saved_string(20);
		snprintf(saved_string,20,"%f",va[0].dval);
		// MESG("uf_string: [%s]",saved_string);
	} else {
		syntax_error("string: wrong_type of args",100);
		set_sval("");
	};
	ex_vtype=VTYPE_STRING;

	return 0;
}

double uf_message()
{
	get_function_args(1);
	if(va[0].vtype==VTYPE_STRING) {
		msg_line("[%s]",va[0].sval);
	} else msg_line("<%f>",va[0].dval);
	events_flush();
	return 0;
}

double uf_error()
{
	get_function_args(1);
	if(va[0].vtype==VTYPE_STRING) 
		error_line("[%s]",va[0].sval);
	else error_line("<%f>",va[0].dval);
	events_flush();
	return 0;
}

double uf_wait()
{
	int f_entry=entry_mode;
	entry_mode=KENTRY;
	get_function_args(1);
	if(va[0].vtype==VTYPE_STRING) msg_line("[%s] waiting.. ",va[0].sval);
	else msg_line("<%f> wait for key",va[0].dval);
	clean_saved_string(1);
	if(execmd) {
		saved_string[0]=getc(stdin);
	} else {
		events_flush();
		// entry_mode=KENTRY;
		saved_string[0] = getcmd();
		// entry_mode=KNORMAL;
	};
	entry_mode=f_entry;
	saved_string[1] = 0;
	double value=saved_string[0];
	ex_vtype=VTYPE_STRING;

	return value;
}

double uf_input()
{
	int f_entry=entry_mode;
	get_function_args(1);
	entry_mode=KENTRY;	/* get input from screen */
	clean_saved_string(80);
	if(va[0].vtype!=VTYPE_STRING) getstring("Input :",saved_string,80,true);
	else getstring(va[0].sval,saved_string,80,true);
	if(execmd) saved_string[strlen(saved_string)-1]=0;
	ex_vtype=VTYPE_STRING;
	entry_mode=f_entry;
	return 0;
}

double uf_dinput()
{
	get_function_args(1);
	clean_saved_string(80);
	if(va[0].vtype!=VTYPE_STRING) getstring("DInput :",saved_string,80,true);
	getstring(va[0].sval,saved_string,80,true);
	double value=atof(saved_string);
	clean_saved_string(0);
	ex_vtype=VTYPE_NUM;

	return value;
}

double uf_init()
{
	ntoken();
	initialize_vars();
	return 0;
}

double uf_val()
{
	double value=0;
	get_function_args(1);
	if(va[0].vtype==VTYPE_STRING) 
		value=atof(va[0].sval);
	else syntax_error("val: wrong_type of args",100);
	ex_vtype=VTYPE_NUM;
	return value;
}

double uf_sqrt()
{
	double value=get_numeric_arg();
	if(ex_vtype==VTYPE_NUM) value=sqrt(va[0].dval);
	else syntax_error("aqrt: wrong argument",305);

	return value;
}

double uf_dbg_message()
{
	get_function_args(1);
	if(va[0].vtype==VTYPE_STRING) MESG(":%s",va[0].sval);
	ex_vtype=VTYPE_NUM;
	return 0;
}

double uf_sin()
{
	double value=get_numeric_arg();
	if(ex_vtype==VTYPE_NUM)	
		value=sin(value);
	else {
		syntax_error("sin: wrong argument",305);
	};
	return value;
}

double uf_cos()
{
	double value=get_numeric_arg();
	if(ex_vtype==VTYPE_NUM)	value=cos(va[0].dval);
	else syntax_error("cos: wrong argument",305);
	return value;
}

double uf_tan()
{
	double value=get_numeric_arg();
	if(ex_vtype==VTYPE_NUM) value=tan(value);
	else syntax_error("tan: wrong arguments",100);
	return value;
}

double uf_log10()
{
	double value=get_numeric_arg();
	if(ex_vtype==VTYPE_NUM)	value=log10(va[0].dval);
	else syntax_error("log10: wrong argument",305);
	return value;
}

double uf_atan()
{
	double value=get_numeric_arg();
	if(ex_vtype==VTYPE_NUM)	value=atan(va[0].dval);
	else syntax_error("atan: wrong argument",305);
	return value;
}

double uf_log()
{
	double value=get_numeric_arg();
	if(ex_vtype==VTYPE_NUM)	value=log(va[0].dval);
	else syntax_error("log: wrong argument",305);
	return value;
}

double uf_trunc()
{
	double value=get_numeric_arg();
	if(ex_vtype==VTYPE_NUM)	value=trunc(va[0].dval);
	else syntax_error("trunc: wrong argument",305);
	return value;
}

double uf_round()
{
	double value=get_numeric_arg();
	if(ex_vtype==VTYPE_NUM)	value=round(va[0].dval);
	else syntax_error("round: wrong argument",305);
	return value;
}

double uf_getpoint()
{
	ntoken();
	return 0.0;
}

double uf_time()
{
	get_function_args(2);
	double value=0;
	if(va[0].vtype==VTYPE_STRING) {
		set_sval(va[0].sval);
		value=show_time(saved_string,va[1].dval);
	} else {
		syntax_error("error in stime",312);
	}

	ex_vtype=VTYPE_NUM;
	return value;
}

double uf_deq()
{
	get_numeric_args(2);
	double value = deq(va[0].dval,va[1].dval);
	ex_vtype=VTYPE_NUM;
	return value;
}

double uf_atbof()
{
	ntoken();
	ex_vtype=VTYPE_NUM;
	return (FBof(cbfp));
}

double uf_ateof()
{
	ntoken();
	ex_vtype=VTYPE_NUM;
	return (FEof(cbfp));
}

double uf_atbol()
{
	ntoken();
	ex_vtype=VTYPE_NUM;
	return (FBolAt(cbfp,Offset()));
}

double uf_ateol()
{
	ntoken();
	ex_vtype=VTYPE_NUM;
	return (FEolAt(cbfp,Offset()));
}

double uf_mainargsize()
{
	ntoken();
	if(main_args) {
	// MESG("argument size: rows=%d cols=%d",main_args->rows,main_args->cols);
		ex_vtype=VTYPE_NUM;
		return main_args->cols;
	} else return 0;
}

double uf_mainarg()
{
	if(!main_args) { ntoken();return 0;};

	get_function_args(1);
	int ind;
	double value=0;
	ind=(int)va[0].dval;
	if(ind<main_args->cols) {
		set_sval(main_args->sval[(int)va[0].dval]);
		ex_vtype=VTYPE_STRING;
		value=atof(saved_string);
	} else {
		ex_vtype=VTYPE_NUM;
	};
	return value;
}
