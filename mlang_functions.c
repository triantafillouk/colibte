/* editor functions */
MVAR va[3];

void get_function_args (int number_of_args)
{
	int i;
	int f_entry;
	// MVAR *va=NULL;

	f_entry=entry_mode;
	entry_mode=KNORMAL;
	// MESG("get_function_args: %d",number_of_args);
	ex_vtype=VTYPE_NUM;
	NTOKEN2;
	if(number_of_args) {
		/* if we have arguments, check for parenthesis, then get the arguments  */
		// va = (MVAR *)malloc(sizeof(MVAR)*number_of_args);
		for(i=0;i< number_of_args;i++) { 
			NTOKEN2;
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
		NTOKEN2;
	} else {;
		if(tok->ttype==TOK_LPAR) {
				NTOKEN2;
				NTOKEN2;
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
	NTOKEN2;
	for(i=0;i< number_of_args;i++ ) { 
		NTOKEN2;
		double value = num_expression();
		va[i].vtype=ex_vtype;
		va[i].dval=value;
	};
	NTOKEN2;
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

	// free(va);
	return value;
}

/* clear output buffer */
double uf_cls()
{
	NTOKEN2;
	cls_fout("[out]");
	// MESG("clear screen!");
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
	// free(va);
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
	} else set_sval("");

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
			set_sval("");
		};
		ex_vtype=VTYPE_STRING;
	return 0;	
}

double uf_print()
{
	// MESG("uf_print:");
	get_function_args(1);
	double value=0;
	if(va[0].vtype==VTYPE_ARRAY) {
		print_array1("",va[0].adat);
	} else {
		char *p_out;
		if(va[0].vtype==VTYPE_STRING) {
			p_out=strdup(va[0].sval);
			ex_vtype=VTYPE_STRING;
			set_sval(va[0].sval);
			// if(xwin && !execmd) MESG(saved_string);
		} else {
			p_out=(char *)malloc(128);
			snprintf(p_out,128,": %f",va[0].dval); 
			ex_vtype=VTYPE_NUM;
			value=va[0].dval;
		};
		out_print(p_out,1);
		free(p_out);
	};

	// free(va);
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

	// free(va);
	return value;
}


double uf_upper()
{
	get_function_args(1);
	if(va[0].vtype==VTYPE_STRING) {
		clean_saved_string(strlen(va[0].sval));
		get_uppercase_string(saved_string,va[0].sval);
	} else set_sval("");
	ex_vtype=VTYPE_STRING;
	// free(va);
	return 0;
}

double uf_lower()
{
	get_function_args(1);
	if(va[0].vtype==VTYPE_STRING) {
		clean_saved_string(strlen(va[0].sval));
		get_lowercase_string(saved_string,va[0].sval);
	} else set_sval("");
	ex_vtype=VTYPE_STRING;
	// free(va);
	return 0;
}

double uf_ascii()
{
	get_function_args(1);
	double value=0;
	if(va[0].vtype==VTYPE_STRING) {
	 value=va[0].sval[0];
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
 ex_vtype=VTYPE_NUM;
 return rand();
}

double uf_abs()
{
	get_function_args(1);
	double value= fabs(va[0].dval);
	// free(va);
	return value;
}

/* string of a value */
double uf_sindex()
{
	double value=0;
	get_function_args(2);
	if(va[0].vtype==VTYPE_STRING) {
		value = sindex(va[0].sval, va[1].sval);
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
	} else set_sval("");
	ex_vtype=VTYPE_STRING;

	// free(va);
	return 0;
}

double uf_message()
{
	get_function_args(1);
	if(va[0].vtype==VTYPE_STRING) {
		msg_line("[%s]",va[0].sval);
	} else msg_line("<%f>",va[0].dval);
	events_flush();
	// free(va);
	return 0;
}

double uf_error()
{
	get_function_args(1);
	if(va[0].vtype==VTYPE_STRING) 
		error_line("[%s]",va[0].sval);
	else error_line("<%f>",va[0].dval);
	events_flush();
	// free(va);
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

	// free(va);
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
	// free(va);
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

	// free(va);
	return value;
}

double uf_init()
{
	initialize_vars();
	return 0;
}

double uf_val()
{
	double value=0;
	get_function_args(1);
	if(va[0].vtype==VTYPE_STRING) 
		value=atof(va[0].sval);

	ex_vtype=VTYPE_NUM;
	// free(va);
	return value;
}

double uf_sqrt()
{
	double value=0;

	get_numeric_args(1);
	if(va[0].vtype==VTYPE_STRING) 
		value=atof(va[0].sval);
	ex_vtype=VTYPE_NUM;

	// free(va);
	return value;
}

double uf_sin()
{
	get_numeric_args(1);
	double value=0;
	if(va[0].vtype==VTYPE_NUM)	
		value=sin(va[0].dval);
	else {
		syntax_error("math error in sin",305);
	};
	ex_vtype=VTYPE_NUM;
	// free(va);
	return value;
}

double uf_cos()
{
	get_numeric_args(1);
	double value=0;
	if(va[0].vtype==VTYPE_NUM)	
		value=cos(va[0].dval);
	else {
		syntax_error("math error in cos",305);
	};
	ex_vtype=VTYPE_NUM;
	// free(va);
	return value;
}

double uf_tan()
{
	get_numeric_args(1);
	double value=0;
	if(va[0].vtype==VTYPE_NUM)	
		value=tan(va[0].dval);
	else {
		syntax_error("math error in tan",305);
	};

	ex_vtype=VTYPE_NUM;
	// free(va);
	return value;
}

double uf_log10()
{
	get_numeric_args(1);
	double value=0;
	if(va[0].vtype==VTYPE_NUM)	
		value=log10(va[0].dval);
	else {
		syntax_error("math error in log10",305);
	};
	ex_vtype=VTYPE_NUM;
	// free(va);
	return value;
}

double uf_atan()
{
	get_numeric_args(1);
	double value=0;
	if(va[0].vtype==VTYPE_NUM)	
		value=atan(va[0].dval);
	else {
		syntax_error("math error in atan",305);
	};
	ex_vtype=VTYPE_NUM;
	// free(va);
	return value;
}

double uf_log()
{
	get_numeric_args(1);
	double value=0;
	if(va[0].vtype==VTYPE_NUM)	
		value=log(va[0].dval);
	else {
		syntax_error("math error in lognat",305);
	};
	ex_vtype=VTYPE_NUM;
	// free(va);
	return value;
}

double uf_trunc()
{
	get_numeric_args(1);
	double value=0;
	if(va[0].vtype==VTYPE_NUM)	
		value=trunc(va[0].dval);
	else {
		syntax_error("math error in trunc",305);
	};
	ex_vtype=VTYPE_NUM;
	// free(va);
	return value;
}

double uf_round()
{
	get_numeric_args(1);
	double value=0;
	if(va[0].vtype==VTYPE_NUM)	
		value=round(va[0].dval);
	else {
		syntax_error("math error in round",305);
	};
	ex_vtype=VTYPE_NUM;
	// free(va);
	return value;
}

double uf_getpoint()
{
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
	// free(va);
	return value;
}

double uf_deq()
{
	get_numeric_args(2);
	double value = deq(va[0].dval,va[1].dval);
	ex_vtype=VTYPE_NUM;
	// free(va);
	return value;
}

double uf_atbof()
{
	return (FBof(cbfp));
}

double uf_ateof()
{
	return (FEof(cbfp));
}

double uf_atbol()
{
	return (FBolAt(cbfp,Offset()));
}

double uf_ateol()
{
	return (FEolAt(cbfp,Offset()));
}

double uf_mainargsize()
{
	if(main_args) {
	// MESG("argument size: rows=%d cols=%d",main_args->rows,main_args->cols);
	ex_vtype=VTYPE_NUM;
	return main_args->cols;
	} else return 0;
}

double uf_mainarg()
{
	if(!main_args) return 0;

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
	// free(va);
	return value;
}

m_function m_functions[] = {
	{"len",1,uf_len},        /* STRING LENGTH */
	{"upper",1,uf_upper},        /* UPPERCASE STRING */
    {"lower",1,uf_lower},        /* LOWER CASE STRING */
	{"left",2,uf_left},
	{"right",2,uf_right},
	{"mid",3,uf_mid},
    {"s_asc",1,uf_ascii},	/* CHAR TO INTEGER CONVERSION */
    {"chr",1,uf_chr},		/* INTEGER TO CHAR CONVERSION */
    {"getchar",0,uf_getchar},	/* GET 1 CHARACTER */
    {"rand",0,uf_rand},        	/* GET A RANDOM NUMBER */
    {"abs",1,uf_abs},        	/* ABSOLUTE VALUE OF A NUMBER */
    {"s_index",2,uf_sindex},     /* FIND THE INDEX OF ONE STRING IN ANOTHER */
	{"str",1,uf_string},		/* string of a value */
	{"message",1,uf_message},	/* show message on screen  */
	{"error_log",1,uf_error},	/* show message error  */
	{"input",1,uf_input},		/* string input from screen  */
	{"initialize",0,uf_init},	/* variables init */
	{"s_val",1,uf_val},			/* numerical string value  */
	{"sqrt",1,uf_sqrt},
	{"sin",1,uf_sin},
	{"cos",1,uf_cos},
	{"tan",1,uf_tan},
	{"atan",1,uf_atan},
	{"log10",1,uf_log10},
	{"log",1,uf_log},
	{"trunc",1,uf_trunc},
	{"round",1,uf_round},
	{"print",1,uf_print},	/* out_print, print on out buffer, and stdout if in Xwindows mode  */
	{"getpoint",0,uf_getpoint},
	{"message_wait",1,uf_wait},
	{"dinput",1,uf_dinput},
	{"show_time",2,uf_show_time},	/* show elapsed time in nanoseconds  */
	{"deq",2,uf_deq},	/* compare double with limited precission */
	{"cls",0,uf_cls},	/* clear output buffer  */
	{"DET",1,uf_determinant},	/* determinant  */
	{"INV",1,uf_inverse},	/* inverse  */
	{"T",1,uf_transpose},	/* transpose  */
	{"at_bof",0,uf_atbof},	/* if at begin of file  */
	{"at_eof",0,uf_ateof},	/* if at end of file  */
	{"at_bol",0,uf_atbol},	/* if at begin of line  */
	{"at_eol",0,uf_ateol},	/* if at end of line  */
	{"args_size",0,uf_mainargsize},	/* main arguments list size  */
	{"args",1,uf_mainarg},	/* main  argument at position */
	{NULL,0}
};

