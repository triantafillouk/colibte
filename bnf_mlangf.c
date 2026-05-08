
extern array_dat *main_args;
extern char *ex_name;
extern FILEBUF *cbfp;
extern BTREE *global_types_tree;
extern char *vtype_names[];

void mesg_out(const char *fmt, ...);
void init_stack();
tok_struct *current_token();
void set_vtype(int type);
int vtype_is(int type);
int get_vtype();
void set_array(array_dat *a);
array_dat *get_array(char *);
void set_nsval(char *,int);
char * tok_info(tok_struct *tok);

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
void show_var_node(BTNODE *node);

inline static void bnf_function_args (int number_of_args)
{
	int i;
	int f_entry=entry_mode;
	entry_mode=KNORMAL;
	// MESG("bnf_function_args: %d [%d %s]",number_of_args,tok->tnum,tok->tname);

	for(i=0;i<number_of_args;i++) {
		NTOKEN2;
		bnf_expression();
		// show_result();
	};
	entry_mode=f_entry;
}

void bnf_numeric_args (int number_of_args)
{
	int i;
	// MESG("bnf_numeric_args: %d",number_of_args);
	for(i=0;i< number_of_args;i++ ) { 
		ntoken();
		bnf_expression();set_var_value();
	};
// 	ntoken();
}

double bnf_numeric_arg ()
{
	// MESG("bnf_numeric_arg:");
	ntoken();
	bnf_expression();set_var_value();
	// ntoken();
	return num_result();
}

void bnf_len()
{
	bnf_function_args(1);

	if(bnf_var->var_type==VTYPE_STRING) { 
		bnf_var->dval=strlen(bnf_var->sval);
	}
	else if(bnf_var->var_type==VTYPE_ARRAY || bnf_var->var_type==VTYPE_SARRAY) {
		array_dat *arr=bnf_var->adat;		
		bnf_var->dval=arr->rows*arr->cols;
	};
	bnf_var->var_type=VTYPE_NUM;
}

void bnf_array_cols()
{
	bnf_function_args(1);
	MVAR *va=bnf_var;
	if(va->var_type==VTYPE_ARRAY||va->var_type==VTYPE_SARRAY) {
		va->dval=va->adat->cols;
	} va->dval=0;
	va->var_type = VTYPE_NUM;
}

void bnf_array_rows()
{
	bnf_function_args(1);
	MVAR *va=bnf_var;
	if(va->var_type==VTYPE_ARRAY||va->var_type==VTYPE_SARRAY) {
		va->dval = va->adat->rows;
	} else va->dval=0;
	va->var_type=VTYPE_NUM;
}

/* clear output buffer */
void bnf_cls()
{
	// ntoken();
	if(check_token(TOK_RPAR)) ntoken();
	cls_fout("[out]");
	MESG("<-[%s: %s ------------------------------------------------------------->",cbfp->b_fname,VERSION);
}

void bnf_index()	// ?? TBD!!
{
	// ntoken();
	MESG("new index!");
	// bnf_var->dval=VTYPE_TREE;
	bnf_var->var_type=VTYPE_TREE;
}

/* get array determinant */
void bnf_determinant()	/* TBD  */
{
	double value=0;
	bnf_function_args(1);
	if(bnf_var->var_type==VTYPE_ARRAY) {
		array_dat *arr = bnf_var->adat;

		if(arr->rows==arr->cols) value=determinant(arr);
		else {
			// MESG("wrong dimensions!");
			syntax_error("wrong dimensions for determinant",203);
		};
	} else {
		syntax_error("Not an array!",204);
	};
	bnf_var->var_type=VTYPE_NUM;
	bnf_var->dval=value;
	MESG("	determinant: %f var@=%d",value,(int)(bnf_var-bnf_vars));
}

void bnf_inverse()	/* TBC  */
{
	double value=0;
	bnf_function_args(1);
	MVAR *va = bnf_var;
	if(va->var_type==VTYPE_POINTER) va=va->var_pointer;
	if(va->var_type==VTYPE_ARRAY) {
		array_dat *arr = va->adat;

		if(arr->rows==arr->cols) {
			value=determinant(arr);
		} else {
			syntax_error("wrong dimensions for determinant",205);
		};
		if(value!=0) {
			array_dat *inverse;
			inverse=cofactor2(arr,value);
		// free array ??
			bnf_var->adat = inverse;
			bnf_var->var_type=va->var_type;
			// set_array(inverse);
			ex_name="Inverse";
		};
	} else {
		syntax_error("Not an array!",206);
	}
}

void bnf_transpose()	/* TBC  */
{
	bnf_function_args(1);

	MVAR *va = bnf_var;
	if(va->var_type==VTYPE_POINTER) va=va->var_pointer;

	if(va->var_type==VTYPE_ARRAY) {
		array_dat *arr  = va->adat;
		array_dat *tarray;
		tarray = transpose(arr);
		// free array ??
		bnf_var->adat = tarray;
		bnf_var->var_type=va->var_type;
		// set_array(tarray);
		ex_name="Tranpose";
	} else {
		syntax_error("Not an array!",207);
	};
	// free(va);
}

void bnf_left()	/* TBC  */
{
	bnf_function_args(2);
	MVAR *va=bnf_var-1;
	if(va[0].var_type==VTYPE_STRING && va[1].var_type==VTYPE_NUM ) {
		int r1=(int)va[1].dval;
		if(r1<strlen(va[0].sval)) {
			char *s=(char *)malloc(r1+1);
			memcpy(s,va[0].sval,r1);s[r1]=0;
			va[0].sval=s;
			va[0].var_alloced=1;
		} else {
			// va[0].sval[(int)va[1].dval]=0;
			// char *s=strdup(va[0].sval);
			// if(va[0].var_alloced) free(va[0].sval);
		};
	} else {
		va[0].sval="error string left!";
		va[0].var_type=VTYPE_NUM;
		va[0].var_alloced=0;
	};
	prev_var("left");
	MESG("- left: next is [%s]",tok_info(tok));
	// NTOKEN2;
}

void bnf_right()	/* TBC  */
{
	bnf_function_args(2);
	MVAR *va=bnf_var-1;
	// MESG("bnf_right: %d %d",va[0].var_type,va[1].var_type);
	if(va[0].var_type==VTYPE_STRING && va[1].var_type==VTYPE_NUM ) {
		unsigned long r1=(unsigned long)va[1].dval;
		// MESG("	right: [%s] r1=%d",va[0].sval,r1);
		if(strlen(va[0].sval)<r1) r1=strlen(va[0].sval);
		char *s=strdup(va[0].sval+(strlen(va[0].sval)-r1));
		if(va[0].var_alloced) free(va[0].sval);
		va[0].sval=s;
		va[0].var_alloced=1;
		// set_sval(va[0].sval+(strlen(va[0].sval)-r1));
	} else {
		syntax_error("right: wrong type of args",206);
		// set_sval("");
	};
	prev_var("right");
	MESG("- right: next is [%s]",tok_info(tok));
	// NTOKEN2;
}

void bnf_mid()	/* TBC  */
{
	bnf_function_args(3);
	MVAR *va=bnf_var-2;
		if(va[0].var_type==VTYPE_STRING && va[1].var_type==VTYPE_NUM && va[2].var_type==VTYPE_NUM) 
		{
		if((unsigned long)va[1].dval>strlen(va[0].sval) || va[2].dval==0) {
			// set_sval("");
		} else {
			char *s=(char *)malloc((int)va[2].dval+1);
			memcpy(s,va[0].sval+(int)va[1].dval,(int)va[2].dval);
			s[(int)va[2].dval]=0;
			// set_nsval(va[0].sval+(int)va[1].dval,(int)va[2].dval);
			va[0].sval=s;
			va[0].var_type=VTYPE_STRING;
			va[0].var_alloced=1;
		};
		} else {
			syntax_error("mid: wrong_type of args",100);
			set_sval("");
		};
		prev_var("mid1");
		prev_var("mid2");
		bnf_var->var_type=VTYPE_STRING;
		MESG("- mid : next is [%s]",tok_info(tok));
}


void bnf_print()
{
	int args=tok->number_of_args;
	// MESG("bnf_print: tnum=%d args=%d var index=%d",tok->tnum,tok->number_of_args,(int)(bnf_var-bnf_vars));
	int i;
	for(i=0;i<args;i++) {
		// ntoken();
		NTOKEN2;
		
		bnf_expression();
		// MESG("	bnf_print: after %d expression ind=%d [%s]",i,(int)(bnf_var-bnf_vars),tok_info(tok));
		// show_result();
		switch(bnf_var->var_type) {
			case VTYPE_ARRAY:
			case VTYPE_SARRAY:
			case VTYPE_AMIXED:
				if(i>0) out_print("",1);
				// print_array1("",get_array("102"));break;
				print_array1("",bnf_var->adat);break;
			case VTYPE_NUM:{
				char p_out[128];
				long l0=bnf_var->dval;
				if(l0==bnf_var->dval) snprintf(p_out,sizeof(p_out),"%ld",l0);
				else snprintf(p_out,sizeof(p_out),"%f",bnf_var->dval); 
				out_print(p_out,0);
				// free(p_out);
				break;
			};
			case VTYPE_STRING:{
				// MESG("-- bnf_print: string result '%s'",bnf_var->sval);
				out_print(bnf_var->sval,0);
			};
		};
		prev_var("print");
		// MESG("	if: after switch tnum=%d ttype=%d",tok->tnum,tok->ttype);
	};
	out_print("",1);
	// MESG("bnf_print: skip function right parenthesis");
	// ntoken();
	// MESG("end of bnf_print: num=%d",tok->tnum);
	// MESG("bnf_print: >>");
}

void bnf_show_time()
{
	bnf_function_args(2);
	MVAR *va=bnf_var-1;
	double value=0;
	if(va[0].var_type==VTYPE_STRING) {
		value=show_time(va[0].sval,va[1].dval);
	} else {
		syntax_error("error in stime",312);
	}
	// MESG("show_time: [%s]",va[0].sval);
	prev_var("show_time");
	if(bnf_var->var_type==VTYPE_STRING && bnf_var->var_alloced==1) {
		// MESG("show time, free sval!");
		free(bnf_var->sval);
	};
	bnf_var->dval=value;
	bnf_var->var_type=VTYPE_NUM;
}


void bnf_upper()	/* TBC  */
{
	bnf_function_args(1);
	MVAR *va=bnf_var;
	if(va->var_type==VTYPE_POINTER) va=va->var_pointer;
	if(va->var_type==VTYPE_STRING) {
		// clean_saved_string(strlen(va[0].sval));
		char *s=(char *)malloc(strlen(va[0].sval+1));
		get_uppercase_string(s,va[0].sval);
		if(bnf_var->var_alloced) free(bnf_var->sval);
		bnf_var->sval=s;
		// MESG("upper: [%s]",va[0].sval);
		// MESG("upper: [%s]",bnf_var->sval);
		bnf_var->var_type=VTYPE_STRING;
	} else {
		syntax_error("upper: wrong_type of args",100);
		// set_sval("");	
	};
	MESG("- upper: next is [%s]",tok_info(tok));
}

void bnf_lower()	/* TBC  */
{
	bnf_function_args(1);
	MVAR *va=bnf_var;
	if(va->var_type==VTYPE_POINTER) va=va->var_pointer;
	if(va->var_type==VTYPE_STRING) {
		// clean_saved_string(strlen(va[0].sval));
		char *s=(char *)malloc(strlen(va[0].sval+1));
		get_lowercase_string(s,va[0].sval);
		if(bnf_var->var_alloced) free(bnf_var->sval);
		bnf_var->sval=s;
		// MESG("upper: [%s]",va[0].sval);
		// MESG("upper: [%s]",bnf_var->sval);
		bnf_var->var_type=VTYPE_STRING;
	} else {
		syntax_error("lower: wrong_type of args",100);
		// set_sval("");	
	};
	// MESG("- lower: next is [%s]",tok_info(tok));
}

void bnf_ascii()	/* TBC  */
{
	bnf_function_args(1);
	MVAR *va=bnf_var;
	if(bnf_var->var_type==VTYPE_POINTER) va=bnf_var->var_pointer;
	if(va->var_type==VTYPE_STRING) {
	 	int ascii=va->sval[0];
		if(bnf_var->var_alloced) free(bnf_var->sval);
		bnf_var->dval=ascii;
	} else {
		syntax_error("s_acs: needs a string argument",100);
	};
	bnf_var->var_type=VTYPE_NUM;
	// MESG("- ascii: next is [%s]",tok_info(tok));
}

void bnf_chr()	/* TBC  */
{
	bnf_function_args(1);
	MVAR *va=bnf_var;
	clean_saved_string(1);
	char *slocal=get_sval();
	slocal[0] = (int)va[0].dval;
	slocal[1] = 0;

	bnf_var->var_type=VTYPE_STRING;
	// free(va);
}

void bnf_getchar() /* TBC  */
{
	ntoken();
	next_var("getchar");
	char *slocal=(char *)malloc(2);
	if(execmd) {
		slocal[0]=getchar();	/* get a utf8 char! TBD !! */
	} else {
		slocal[0] = getcmd();
	};
	slocal[1] = 0;
	bnf_var->sval=slocal;bnf_var->var_alloced=1;
	bnf_var->var_type=VTYPE_STRING;
}

/* returns big! int */
void bnf_rand()	/* TBC  */
{
 // static long =
 long max=(long) bnf_numeric_arg();
#if	DARWIN
 long value=arc4random_uniform(max);
#else
 long value=random() % max;
#endif
 // MESG("bnf_rand:%ld",value);
	bnf_var->dval=value;
	bnf_var->var_type=VTYPE_NUM;
}

void bnf_seed()
{
	long seed=(long) bnf_numeric_arg();
	srand(seed);
}


void bnf_abs()
{
	double value=bnf_var->dval;// numeric_arg();
	value= fabs(value);
	bnf_var->dval=value;
	bnf_var->var_type=VTYPE_NUM;
}

/* string of a value */
void bnf_sindex()	/* TBC  */
{
	bnf_function_args(2);
	MVAR *vb=bnf_var;
	if(vb->var_type==VTYPE_POINTER) vb=vb->var_pointer;
	prev_var("sindex");
	MVAR *va=bnf_var;
	if(va->var_type==VTYPE_POINTER) va=va->var_pointer;
	if(va->var_type==VTYPE_STRING && vb->var_type==VTYPE_STRING) {
		bnf_var->dval = sindex(va->sval, vb->sval);
		bnf_var->var_type=VTYPE_NUM;
	} else {
		syntax_error("s_index: wrong_type of args",100);
	};
}

/* string of a value */
void bnf_string()	/* TBC  */
{
	bnf_function_args(1);
	MVAR *va=bnf_var;
	if(va[0].var_type==VTYPE_NUM) {
		// MESG("bnf_string: %f",va[0].dval);
		char *slocal=(char *)malloc(20);
		snprintf(slocal,20,"%f",va[0].dval);
		va->sval=slocal;
		va->var_alloced=1;
		// MESG("- bnf_string: next is [%s]",tok_info(tok));
		// show_result();
	} else {
		syntax_error("string: wrong_type of args",100);
		va->sval="";
		va->var_alloced=0;
	};
	va->var_type=VTYPE_STRING;
}

void bnf_message()	/* TBC  */
{
	bnf_function_args(1);
	MVAR *va=bnf_var;
	if(va[0].var_type==VTYPE_STRING) {
		msg_line("[%s]",va[0].sval);
	} else msg_line("<%f>",va[0].dval);
	events_flush();
}

void bnf_error()	/* TBC  */
{
	bnf_function_args(1);
	MVAR *va=bnf_var;
	if(va[0].var_type==VTYPE_STRING) 
		error_line("[%s]",va[0].sval);
	else error_line("<%f>",va[0].dval);
	events_flush();
}

void bnf_wait()	/* TBC  */
{
	int f_entry=entry_mode;
	entry_mode=KENTRY;
	bnf_function_args(1);
	MVAR *va=bnf_var;
	if(va[0].var_type==VTYPE_STRING) msg_line("[%s] waiting.. ",va[0].sval);
	else msg_line("<%f> wait for key",va[0].dval);

	char *slocal=get_sval();
	if(execmd) {
		slocal[0]=getc(stdin);
	} else {
		events_flush();
		// entry_mode=KENTRY;
		slocal[0] = getcmd();
		// entry_mode=KNORMAL;
	};
	entry_mode=f_entry;
	slocal[1] = 0;
	bnf_var->sval = strdup(slocal);
	bnf_var->var_type=VTYPE_STRING;
}

void bnf_input()	/* TBC  */
{
	int f_entry=entry_mode;
	bnf_function_args(1);
	MVAR *va=bnf_var;
	entry_mode=KENTRY;	/* get input from screen */
	// clean_saved_string(80);
	char *slocal=(char *)malloc(80);
	if(va[0].var_type!=VTYPE_STRING) getstring("Input :",slocal,80,true);
	else getstring(va->sval,slocal,80,true);
	if(execmd) strtok(slocal, "\n");
	bnf_var->var_type=VTYPE_STRING;
	if(bnf_var->var_alloced) free(bnf_var->sval);
	bnf_var->sval=slocal;bnf_var->var_alloced=1;
	entry_mode=f_entry;
}

// prompt and get a numeric value
void bnf_dinput()	/* TBD  */
{
	bnf_function_args(1);
	MVAR *va=bnf_var;
	clean_saved_string(80);
	if(va[0].var_type!=VTYPE_STRING) getstring("DInput :",get_sval(),80,true);
	getstring(va[0].sval,get_sval(),80,true);
	bnf_var->dval=atof(get_sval());
	clean_saved_string(0);
	bnf_var->var_type=VTYPE_NUM;
}

void bnf_init()	/* TBC  */
{
	MESG("initialize: bnf_init");
	init_stack();
	ntoken();
}

void bnf_val()	/* TBC  */
{
	bnf_function_args(1);
	MVAR *va=bnf_var;
	if(va[0].var_type==VTYPE_STRING) {
		bnf_var->var_type=VTYPE_NUM;
		double val=atof(va[0].sval);
		if(bnf_var->var_alloced) { free(bnf_var->sval);bnf_var->var_alloced=0;};
		bnf_var->dval=val;
		bnf_var->var_type=VTYPE_NUM;
	} else syntax_error("val: wrong_type of args",100);
}

void bnf_sqrt()	/* OK?  */
{
	NTOKEN2;
	bnf_expression();
	bnf_var->dval=sqrt(bnf_var->dval);
	NTOKEN2;
}

void bnf_cbrt()	/* OK?  */
{
	NTOKEN2;
	bnf_expression();
	bnf_var->dval=cbrt(bnf_var->dval);
	NTOKEN2;
}

void bnf_dbg_message()	/* TBC  */
{
	bnf_function_args(1);
	MVAR *va=bnf_var;
	if(va[0].var_type==VTYPE_STRING) MESG(":%s",va[0].sval);
	NTOKEN2;
}

void bnf_sin()	/* OK?  */
{
	NTOKEN2;
	bnf_expression();
	bnf_var->dval=sin(bnf_var->dval);
	// NTOKEN2;
}

void bnf_cos()	/* OK?  */
{
	NTOKEN2;
	bnf_expression();
	bnf_var->dval=cos(bnf_var->dval);
	// NTOKEN2;
}

void bnf_tan()	/* OK?  */
{
	NTOKEN2;
	bnf_expression();
	bnf_var->dval=tan(bnf_var->dval);
// 	prev_var("tan");
	// NTOKEN2;
}

void bnf_log10()	/* OK?  */
{
	NTOKEN2;
	bnf_expression();
	bnf_var->dval=log10(bnf_var->dval);
	// NTOKEN2;
}

void bnf_atan()	/* OK?  */
{
	NTOKEN2;
	bnf_expression();
	bnf_var->dval=atan(bnf_var->dval);
	// NTOKEN2;
}

void bnf_log()	/* OK?  */
{
	NTOKEN2;
	bnf_expression();
	bnf_var->dval=log(bnf_var->dval);
	// NTOKEN2;
}

void bnf_trunc()	/* OK?  */
{
	NTOKEN2;
	bnf_expression();
	bnf_var->dval=trunc(bnf_var->dval);
	// NTOKEN2;
}

void bnf_round()	/* OK?  */
{
	NTOKEN2;
	bnf_expression();
	bnf_var->dval=round(bnf_var->dval);
	// NTOKEN2;
}

void bnf_getpoint()	/* dummy ?? only for plot!  */
{
	NTOKEN2;
	// bnf_expression();
	// bnf_var->dval=tan(bnf_var->dval);
	// NTOKEN2;
}

void bnf_time()	/* OK????  */
{
	MESG("-- bnf_time");
	bnf_function_args(2);
	MVAR *va=bnf_var-1;

	if(va[0].var_type==VTYPE_STRING) {
		// set_sval(va[0].sval);
		double dval=show_time(va[0].sval,va[1].dval);
		if(va[0].var_alloced) free(va[0].sval);
		va[0].dval=dval;
		va[0].var_type=VTYPE_NUM;
	} else {
		syntax_error("error in stime",312);
	}
	prev_var("time");
}

void bnf_deq()	/* OK?  */
{
	bnf_function_args(2);
	MVAR *va=bnf_var-1;
	va[0].dval = deq(va[0].dval,va[1].dval);
	prev_var("deq");
	// NTOKEN2;
}

void bnf_atbof()	/* TBC  */
{
	// ntoken();
	next_var("atbof");

	bnf_var->dval = FBof(cbfp);
	bnf_var->var_type=VTYPE_NUM;
}

void bnf_ateof()	/* TBC  */
{
	ntoken();
	next_var("ateof");
	bnf_var->var_type=VTYPE_NUM;
	bnf_var->dval = FEof(cbfp);
}

void bnf_atbol()	/* OK?  */
{
	ntoken();
	next_var("atbol");
	bnf_var->var_type=VTYPE_NUM;
	bnf_var->dval = FBolAt(cbfp,Offset());
}

void bnf_ateol()	/* OK?  */
{
	ntoken();
	next_var("ateol");
	bnf_var->var_type=VTYPE_NUM;
	bnf_var->dval=FEolAt(cbfp,Offset());
}

void bnf_mainargsize()	/* TBC  */
{
	ntoken();
	next_var("mainargsize");
	bnf_var->var_type=VTYPE_NUM;
	bnf_var->dval = main_args->cols;
}

void bnf_mainarg()	/* TBD  */
{
	if(!main_args) { ntoken();};

	bnf_function_args(1);
	MVAR *va=bnf_var;
	int ind;
	// double value=0;
	ind=(int)va[0].dval;
	if(ind<main_args->cols) {
		// set_sval(main_args->sval[(int)va[0].dval]);
		bnf_var->sval=main_args->sval[(int)va[0].dval];
		bnf_var->var_type=VTYPE_STRING;
		// value=atof(get_sval());
	} else {
		set_vtype(VTYPE_NUM);
	};
}

extern MVAR *current_stable;
extern FILEBUF *exe_buffer;


void bnf_show_vars()	/* ok?  */
{
	ntoken();

	mesg_out("Ind Name        Type             Value      local vars %d",exe_buffer->symbol_tree->items);
	eval_btree(exe_buffer->symbol_tree->root,show_var_node);
#if	USE_TYPE_VARS
	if(global_types_tree->root) {
	mesg_out("Ind Name        Type             Value      global vars/types %d",exe_buffer->symbol_tree->items);
		eval_btree(global_types_tree->root,show_var_node);
	} else mesg_out("global_types_tree: empty!");
#endif
}

void bnf_list_tokens()	/* ok?  */
{
 ntoken();
 // MESG("bnf_list_tokens:");
 if(exe_buffer==NULL) { MESG("	exec buffer is null!");return;};

 tok_struct *ltok=exe_buffer->tok_table_bnf;

 cbfp=exe_buffer;
 out_print("Num: line ind[type   name] [value] ------------------------------- ",1);
 while(ltok->ttype!=TOK_EOF) {
 	out_print(tok_info(ltok),1);
	// mesg_out("	show token info",1);
	ltok++;
 };	
 out_print("|-----------------------------------------------------|",1);
}

void bnf_test_loop()
{
	long int lmax= (long int) bnf_numeric_arg();
	double p=0;
	double l=1;
	int j=1;
	int i=1;
	for(j=1;j<lmax;j++) {
		l = i*j+32;
		p = p + l + 6;
	};
}

v_function bnf_functions[] = {
	{"len",1,bnf_len},        /* STRING LENGTH */
	{"upper",1,bnf_upper},        /* UPPERCASE STRING */
    {"lower",1,bnf_lower},        /* LOWER CASE STRING */
	{"left",2,bnf_left},
	{"right",2,bnf_right},
	{"mid",3,bnf_mid},
    {"s_asc",1,bnf_ascii},	/* CHAR TO INTEGER CONVERSION */
    {"chr",1,bnf_chr},		/* INTEGER TO CHAR CONVERSION */
    {"getchar",0,bnf_getchar},	/* GET 1 CHARACTER */
    {"rand",1,bnf_rand},        	/* GET A RANDOM NUMBER */
	{"seed",1,bnf_seed},			/* set sedd for random function  */
    {"abs",1,bnf_abs},        	/* ABSOLUTE VALUE OF A NUMBER */
    {"s_index",2,bnf_sindex},     /* FIND THE INDEX OF ONE STRING IN ANOTHER */
	{"str",1,bnf_string},		/* string of a value */
	{"message",1,bnf_message},	/* show message on screen  */
	{"error_log",1,bnf_error},	/* show message error  */
	{"input",1,bnf_input},		/* string input from screen  */
	{"initialize",0,bnf_init},	/* variables init */
	{"s_val",1,bnf_val},			/* numerical string value  */
	{"sqrt",1,bnf_sqrt},
	{"cbrt",1,bnf_cbrt},
	{"sin",1,bnf_sin},
	{"cos",1,bnf_cos},
	{"tan",1,bnf_tan},
	{"atan",1,bnf_atan},
	{"log10",1,bnf_log10},
	{"log",1,bnf_log},
	{"trunc",1,bnf_trunc},
	{"round",1,bnf_round},
	{"print",-1,bnf_print},	/* out_print, print on out buffer, and stdout if in Xwindows mode  */
	{"getpoint",0,bnf_getpoint},
	{"message_wait",1,bnf_wait},
	{"dinput",1,bnf_dinput},
	{"show_time",2,bnf_show_time},	/* show elapsed time in nanoseconds  */
	{"deq",2,bnf_deq},	/* compare double with limited precission */
	{"cls",0,bnf_cls},	/* clear output buffer  */
	{"DET",1,bnf_determinant},	/* determinant  */
	{"INV",1,bnf_inverse},	/* inverse  */
	{"T",1,bnf_transpose},	/* transpose  */
	{"at_bof",0,bnf_atbof},	/* if at begin of file  */
	{"at_eof",0,bnf_ateof},	/* if at end of file  */
	{"at_bol",0,bnf_atbol},	/* if at begin of line  */
	{"at_eol",0,bnf_ateol},	/* if at end of line  */
	{"args_size",0,bnf_mainargsize},	/* main arguments list size  */
	{"args",1,bnf_mainarg},	/* main  argument at position */
	{"array_cols",1,bnf_array_cols},	/* columns of an an array  */
	{"array_rows",1,bnf_array_rows},	/* rows of an an array  */
	{"dbg_message",1,bnf_dbg_message},	/* show debug message  */
	{"index",0,bnf_index},	/* new index  */
	{"show_vars",0,bnf_show_vars},	/* show local vars  */
	{"list_tokens",0,bnf_list_tokens},	/* list_tokens  */
	{"test_loop",1,bnf_test_loop},	/* test computational loop  */
	{NULL,0,NULL}
};
