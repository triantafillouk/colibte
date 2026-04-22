
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

void bnf_function_args (int number_of_args)
{
	int i;
	int f_entry;
	// tok_struct *tok = current_token();
	f_entry=entry_mode;
	entry_mode=KNORMAL;
	// MESG("bnf_function_args: %d [%d %s]",number_of_args,tok->tnum,tok->tname);

	if(number_of_args) {
		for(i=0;i< number_of_args;i++) { 
			ntoken();
			bnf_expression();set_var_value();
		};
		ntoken();
	} else {;
		if(check_token(TOK_LPAR)) {
			// MESG("	bnf_function_args: TOK_LPAR");
				ntoken();
				ntoken();
		};
	};

	entry_mode=f_entry;
// 	return va;
}

// MVAR va[3];

void bnf_numeric_args (int number_of_args)
{
	int i;
	// MESG("bnf_numeric_args: %d",number_of_args);
	for(i=0;i< number_of_args;i++ ) { 
		ntoken();
		bnf_expression();set_var_value();
	};
	ntoken();
}

double bnf_numeric_arg ()
{
	// MESG("bnf_numeric_arg:");
	ntoken();
	bnf_expression();set_var_value();
	ntoken();
	return num_result();
}

double bnf_len()
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

	return bnf_var->dval;
}

double bnf_array_cols()
{
	bnf_function_args(1);
	MVAR *va=bnf_var;
	if(va->var_type==VTYPE_ARRAY||va->var_type==VTYPE_SARRAY) {
		return va->adat->cols;
	} else return 0;
}

double bnf_array_rows()
{
	bnf_function_args(1);
	MVAR *va=bnf_var;
	if(va->var_type==VTYPE_ARRAY||va->var_type==VTYPE_SARRAY) {
		va->dval = va->adat->rows;
		va->var_type=VTYPE_NUM;
	};
	return 0;
}

/* clear output buffer */
double bnf_cls()
{
	ntoken();
	if(check_token(TOK_RPAR)) ntoken();
	cls_fout("[out]");
	MESG("<-[%s: %s ------------------------------------------------------------->",cbfp->b_fname,VERSION);
	return 0;
}

double bnf_index()	// ??
{
	ntoken();
	MESG("new index!");
	set_vtype(VTYPE_TREE);
	return 0;
}

/* get array determinant */
double bnf_determinant()
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

	return value;
}

double bnf_inverse()
{
	double value=0;
	bnf_function_args(1);

	if(vtype_is(VTYPE_ARRAY)) {
		array_dat *arr = bnf_var->adat;

		if(arr->rows==arr->cols) {
			value=determinant(arr);
		} else {
			syntax_error("wrong dimensions for determinant",205);
		};
		if(value!=0) {
			array_dat *inverse;
			inverse=cofactor2(arr,value);
			set_array(inverse);
			ex_name="Inverse";
		};
	} else {
		syntax_error("Not an array!",206);
	}
	return value;
}

double bnf_transpose()
{
	bnf_function_args(1);
	double value=0;
	if(vtype_is(VTYPE_ARRAY)) {
		array_dat *arr  = bnf_var->adat;
		array_dat *tarray;
		tarray = transpose(arr);
		set_array(tarray);
		ex_name="Tranpose";
		value=1;
	} else {
		syntax_error("Not an array!",207);
	};
	// free(va);
	return value;
}

double bnf_left()
{
	bnf_function_args(2);
	MVAR *va=bnf_var-1;
	if(va[0].var_type==VTYPE_STRING && va[1].var_type==VTYPE_NUM ) {
		set_nsval(va[0].sval,(int)va[1].dval);
	} else set_sval("");
	return 0;
}

double bnf_right()
{
	bnf_function_args(2);
	MVAR *va=bnf_var-1;
	if(va[0].var_type==VTYPE_STRING && va[1].var_type==VTYPE_NUM ) {
		int r1=(int)va[1].dval;
		// MESG("right: r1=%d",r1);
		if(strlen(va[0].sval)<r1) r1=strlen(va[0].sval);
		set_sval(va[0].sval+(strlen(va[0].sval)-r1));
	} else {
		syntax_error("right: wrong type of args",206);
		set_sval("");
	};

	return 0;
}

double bnf_mid()
{
	bnf_function_args(3);
	MVAR *va=bnf_var-2;
		if(va[0].var_type==VTYPE_STRING && va[1].var_type==VTYPE_NUM && va[2].var_type==VTYPE_NUM) 
		{
		if((int)va[1].dval>strlen(va[0].sval) || va[2].dval==0) {
			set_sval("");
		} else {
			set_nsval(va[0].sval+(int)va[1].dval,(int)va[2].dval);
		};
		} else {
			syntax_error("mid: wrong_type of args",100);
			set_sval("");
		};
		set_vtype(VTYPE_STRING);
	return 0;	
}


double bnf_print()
{
	int args=tok->number_of_args;
	// MESG("bnf_print: tnum=%d args=%d var index=%d",tok->tnum,tok->number_of_args,(int)(bnf_var-bnf_vars));
	int i;
	for(i=0;i<args;i++) {
		// ntoken();
		NTOKEN2;
		// tok=current_token();
		bnf_expression();
		// MESG("	bnf_print: after %d expression ind=%d [%s]",i,(int)(bnf_var-bnf_vars),tok_info(tok));
		// show_result();
		switch(bnf_var->var_type) {
			case VTYPE_ARRAY:
			case VTYPE_SARRAY:
			case VTYPE_AMIXED:
				if(i>0) out_print("",1);
				print_array1("",get_array("102"));break;
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
		// MESG("	if: after switch!");
		// tok=current_token();
		// MESG("	if: after switch tnum=%d ttype=%d",tok->tnum,tok->ttype);
	};
	out_print("",1);
	// MESG("bnf_print: skip function right parenthesis");
	ntoken();
	// tok=current_token();
	// MESG("end of if_print: show current token!");
	// MESG("end of bnf_print: num=%d",tok->tnum);
	// MESG("bnf_print: >>");
	return 1;
}

double bnf_show_time()
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
	return value;
}


double bnf_upper()
{
	bnf_function_args(1);
	MVAR *va=bnf_var;
	if(va[0].var_type==VTYPE_STRING) {
		clean_saved_string(strlen(va[0].sval));
		get_uppercase_string(get_sval(),va[0].sval);
	} else {
		syntax_error("upper: wrong_type of args",100);
		set_sval("");	
	};
	set_vtype(VTYPE_STRING);
	return 0;
}

double bnf_lower()
{
	bnf_function_args(1);
	MVAR *va=bnf_var;
	if(va[0].var_type==VTYPE_STRING) {
		clean_saved_string(strlen(va[0].sval));
		get_lowercase_string(get_sval(),va[0].sval);
	} else {
		syntax_error("lower: needs a string argument",100);
		set_sval("");
	};
	set_vtype(VTYPE_STRING);
	return 0;
}

double bnf_ascii()
{
	bnf_function_args(1);
	MVAR *va=bnf_var;
	double value=0;
	if(va[0].var_type==VTYPE_STRING) {
	 value=va[0].sval[0];
	} else {
		syntax_error("s_acs: needs a string argument",100);
	};
	set_vtype(VTYPE_NUM);
	// free(va);
	return value;
}

double bnf_chr()
{
	bnf_function_args(1);
	MVAR *va=bnf_var;
	clean_saved_string(1);
	char *slocal=get_sval();
	slocal[0] = (int)va[0].dval;
	slocal[1] = 0;

	set_vtype(VTYPE_STRING);
	// free(va);
	return 0;
}

double bnf_getchar() // ??
{
	ntoken();
	clean_saved_string(1);
	char *slocal=get_sval();
	if(execmd) {
		slocal[0]=getchar();
	} else {
		slocal[0] = getcmd();
	};
	slocal[1] = 0;

	set_vtype(VTYPE_STRING);
	return 0;
}

/* returns big! int */
double bnf_rand()
{
 // static long =
 long max=(long) bnf_numeric_arg();
#if	DARWIN
 long value=arc4random_uniform(max);
#else
 long value=random() % max;
#endif
 // MESG("bnf_rand:%ld",value);
 return (double)value;
}

double bnf_seed()
{
	long seed=(long) bnf_numeric_arg();
	srand(seed);
	return 0;
}


double bnf_abs()
{
	double value=bnf_numeric_arg();
	value= fabs(value);
	return value;
}

/* string of a value */
double bnf_sindex()
{
	double value=0;
	bnf_function_args(2);
	MVAR *va=bnf_var-1;
	if(va[0].var_type==VTYPE_STRING) {
		value = sindex(va[0].sval, va[1].sval);
		bnf_var->var_type=VTYPE_NUM;
	} else {
		syntax_error("s_index: wrong_type of args",100);
	};

	return value;
}

/* string of a value */
double bnf_string()
{
	bnf_function_args(1);
	MVAR *va=bnf_var;
	if(va[0].var_type==VTYPE_NUM) {
		// MESG("bnf_string: %f",va[0].dval);
		clean_saved_string(20);
		char *slocal=get_sval();
		snprintf(slocal,20,"%f",va[0].dval);
		va[0].sval=slocal;
		// MESG("bnf_string: [%s]",get_sval());
	} else {
		syntax_error("string: wrong_type of args",100);
		set_sval("");
		va[0].sval="";
	};
		va[0].var_type=VTYPE_STRING;

	return 0;
}

double bnf_message()
{
	bnf_function_args(1);
	MVAR *va=bnf_var;
	if(va[0].var_type==VTYPE_STRING) {
		msg_line("[%s]",va[0].sval);
	} else msg_line("<%f>",va[0].dval);
	events_flush();
	return 0;
}

double bnf_error()
{
	bnf_function_args(1);
	MVAR *va=bnf_var;
	if(va[0].var_type==VTYPE_STRING) 
		error_line("[%s]",va[0].sval);
	else error_line("<%f>",va[0].dval);
	events_flush();
	return 0;
}

double bnf_wait()
{
	int f_entry=entry_mode;
	entry_mode=KENTRY;
	bnf_function_args(1);
	MVAR *va=bnf_var;
	if(va[0].var_type==VTYPE_STRING) msg_line("[%s] waiting.. ",va[0].sval);
	else msg_line("<%f> wait for key",va[0].dval);
	clean_saved_string(1);
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
	double value=slocal[0];
	set_vtype(VTYPE_STRING);

	return value;
}

double bnf_input()
{
	int f_entry=entry_mode;
	bnf_function_args(1);
	MVAR *va=bnf_var;
	entry_mode=KENTRY;	/* get input from screen */
	clean_saved_string(80);
	char *slocal=get_sval();
	if(va[0].var_type!=VTYPE_STRING) getstring("Input :",get_sval(),80,true);
	else getstring(va[0].sval,get_sval(),80,true);
	if(execmd) strtok(slocal, "\n");
	set_vtype(VTYPE_STRING);
	entry_mode=f_entry;
	return 0;
}

double bnf_dinput()
{
	bnf_function_args(1);
	MVAR *va=bnf_var;
	clean_saved_string(80);
	if(va[0].var_type!=VTYPE_STRING) getstring("DInput :",get_sval(),80,true);
	getstring(va[0].sval,get_sval(),80,true);
	double value=atof(get_sval());
	clean_saved_string(0);
	set_vtype(VTYPE_NUM);

	return value;
}

double bnf_init()
{
	MESG("initialize: bnf_init");
	init_stack();
	ntoken();
	return 0;
}

double bnf_val()
{
	double value=0;
	bnf_function_args(1);
	MVAR *va=bnf_var;
	if(va[0].var_type==VTYPE_STRING) 
		value=atof(va[0].sval);
	else syntax_error("val: wrong_type of args",100);
	set_vtype(VTYPE_NUM);
	return value;
}

double bnf_sqrt()
{
	double value=bnf_numeric_arg();
	// MESG("sqrt(%f)",value);
	value=sqrt(value);
	return value;
}

double bnf_cbrt()
{
	double value=bnf_numeric_arg();
	// MESG("cbrt(%f)",value);
	value=cbrt(value);

	return value;
}

double bnf_dbg_message()
{
	bnf_function_args(1);
	MVAR *va=bnf_var;
	if(va[0].var_type==VTYPE_STRING) MESG(":%s",va[0].sval);
	set_vtype(VTYPE_NUM);
	return 0;
}

double bnf_sin()
{
	bnf_numeric_arg();
	double value=sin(num_result());
	MESG("bnf_sin: %f",value);
	return value;
}

double bnf_cos()
{
	bnf_function_args(1);
	double value=cos(num_result());
	return value;
}

double bnf_tan()
{
	double value=tan(num_result());
	return value;
}

double bnf_log10()
{
	double value=log10(num_result());
	return value;
}

double bnf_atan()
{
	double value=atan(num_result());
	return value;
}

double bnf_log()
{
	double value=log(num_result());
	return value;
}

double bnf_trunc()
{
	double value=trunc(num_result());
	return value;
}

double bnf_round()
{
	double value=bnf_numeric_arg();
	value=round(value);
	return value;
}

double bnf_getpoint()
{
	ntoken();
	return 0.0;
}

double bnf_time()	// ??
{
	bnf_function_args(2);
	MVAR *va=bnf_var-1;
	double value=0;
	if(va[0].var_type==VTYPE_STRING) {
		set_sval(va[0].sval);
		value=show_time(get_sval(),va[1].dval);
	} else {
		syntax_error("error in stime",312);
	}
	prev_var("time");
	bnf_var->var_type=VTYPE_NUM;
	return value;
}

double bnf_deq()
{
	bnf_numeric_args(2);
	MVAR *va=bnf_var-1;
	double value = deq(va[0].dval,va[1].dval);
	va[0].dval=value;
	prev_var("deq");
	return value;
}

double bnf_atbof()
{
	next_var("atbof");

	bnf_var->dval = FBof(cbfp);
	bnf_var->var_type=VTYPE_NUM;
	ntoken();
	
	return bnf_var->dval;
}

double bnf_ateof()
{
	ntoken();
	next_var("ateof");
	bnf_var->var_type=VTYPE_NUM;
	bnf_var->dval = FEof(cbfp);
	return bnf_var->dval;
}

double bnf_atbol()
{
	ntoken();
	next_var("atbol");
	bnf_var->var_type=VTYPE_NUM;
	bnf_var->dval = FBolAt(cbfp,Offset());
	return bnf_var->dval;
}

double bnf_ateol()
{
	ntoken();
	set_vtype(VTYPE_NUM);
	return (FEolAt(cbfp,Offset()));
}

double bnf_mainargsize()
{
	ntoken();
	next_var("mainargsize");
	bnf_var->var_type=VTYPE_NUM;
	bnf_var->dval = main_args->cols;
	return bnf_var->dval;
}

double bnf_mainarg()	// ??
{
	if(!main_args) { ntoken();return 0;};

	bnf_function_args(1);
	MVAR *va=bnf_var;
	int ind;
	double value=0;
	ind=(int)va[0].dval;
	if(ind<main_args->cols) {
		set_sval(main_args->sval[(int)va[0].dval]);
		set_vtype(VTYPE_STRING);
		value=atof(get_sval());
	} else {
		set_vtype(VTYPE_NUM);
	};
	return value;
}

extern MVAR *current_stable;
extern FILEBUF *exe_buffer;


double bnf_show_vars()
{
	ntoken();

	mesg_out("Ind Name       Type             Value      local vars",exe_buffer->symbol_tree->items);
	eval_btree(exe_buffer->symbol_tree->root,show_var_node);
#if	USE_TYPE_VARS
	if(global_types_tree->root) {
	mesg_out("Ind Name       Type             Value      global vars/types",exe_buffer->symbol_tree->items);
		eval_btree(global_types_tree->root,show_var_node);
	} else mesg_out("global_types_tree: empty!");
#endif
	return 0;
}

double bnf_list_tokens()
{
 ntoken();

 tok_struct *ltok=exe_buffer->tok_table;
 out_print("Num: line ind [type     name ] value ------- ",1);
 while(ltok->ttype!=TOK_EOF) {
 	out_print(tok_info(ltok),1);
	// mesg_out("	show token info",1);
	ltok++;
 };	
 out_print("|-------------------------------------------|",1);

 return 0;
}

double bnf_test_loop()
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
	return p;
}


m_function bnf_functions[] = {
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
