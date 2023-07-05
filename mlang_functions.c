#if	SEP_FUNCTIONS

double value;
MVAR va[3];

MVAR * get_function_args (int number_of_args)
{
	int i;
	int f_entry;
	MVAR *va=NULL;

	f_entry=entry_mode;
	entry_mode=KNORMAL;
	// MESG("get_function_args: %d",number_of_args);
	NTOKEN2;
	if(number_of_args) {
		/* if we have arguments, check for parenthesis, then get the arguments  */
		va = (MVAR *)malloc(sizeof(MVAR)*number_of_args);
		MVAR *vai = va;
		for(i=0;i< number_of_args;i++,vai++ ) { 
			NTOKEN2;
			value = num_expression();
			vai->vtype=ex_vtype;
			if(ex_vtype==VTYPE_STRING) { 
				vai->sval=saved_string;saved_string=NULL;
				// MESG("%d: string: [%s]",i,va[i].sval);
			} else if (ex_vtype==VTYPE_ARRAY||ex_vtype==VTYPE_SARRAY) {
				vai->adat=ex_array;
				// MESG("%d: array",i);
			}
			else {
				vai->dval=value;
				// MESG("%d: numeric %f",i,va[i].dval);
			};
		};
		NTOKEN2;
		// array_dat *arr=vai->adat;
		ex_vtype = vai->vtype;
	} else {;
		if(tok->ttype==TOK_LPAR) {
				NTOKEN2;
				NTOKEN2;
		};
	};

	entry_mode=f_entry;
	return va;
}

double uf_len()
{
	MVAR *va=get_function_args(1);

	if(va->vtype==VTYPE_STRING) { value =strlen(va->sval);}
	else if(ex_vtype==VTYPE_ARRAY || ex_vtype==VTYPE_SARRAY) {
		array_dat *arr=va->adat;		
		value=arr->rows*arr->cols;
	} else value=0;
	ex_vtype=VTYPE_NUM;

	if(va) free(va);
	return value;
}

double uf_cls()
{
	// MVAR *va=get_function_args(0);
	NTOKEN2;
	cls_fout("[out]");
	MESG("clear screen!");
	return 0;
}

double uf_determinant()
{
	MVAR *va=get_function_args(1);
	if(va->vtype==VTYPE_ARRAY) {
		array_dat *arr = va->adat;

		if(arr->rows==arr->cols) value=determinant(arr);
		else {
			// MESG("wrong dimensions!");
			syntax_error("wrong dimensions for determinant",203);
			value=0;
		};
	} else {
		syntax_error("Not an array!",204);
		value=0;
	};
	ex_vtype=VTYPE_NUM;
	return value;
}

double uf_inverse()
{
	MVAR *va=get_function_args(1);

	if(ex_vtype==VTYPE_ARRAY) {
		array_dat *arr = va->adat;

		if(arr->rows==arr->cols) {
			value=determinant(arr);
		} else {
			syntax_error("wrong dimensions for determinant",205);
			value=0;
		};
		if(value!=0) {
			array_dat *inverse;
			inverse=cofactor2(arr,value);
			ex_array=inverse;
			ex_name="Inverse";
		};
	} else {
		syntax_error("Not an array!",206);
		value=0;
	}
	return value;
}

double uf_transpose()
{
	MVAR *va=get_function_args(1);

	if(ex_vtype==VTYPE_ARRAY) {
		array_dat *arr  = va->adat;
		array_dat *tarray;
		tarray = transpose(arr);
		ex_array=tarray;
		ex_name="Tranpose";
		return 1;
	} else {
		syntax_error("Not an array!",207);
		value=0;
		return 0;
	};
}

double uf_left()
{
	MVAR *va=get_function_args(2);

	if(va[0].vtype==VTYPE_STRING && va[1].vtype==VTYPE_NUM ) {
	set_sval(va[0].sval);
	} else set_sval("");
	value = 0;
	ex_vtype=VTYPE_STRING;
	return 0;
}

double uf_right()
{
	MVAR *va=get_function_args(2);

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


#if	0
double eval_fun1(int fnum)
{
	static double value;
	MVAR va[3];
	int i,ia;
	int f_entry;
	// int stat=0;

	TDS("eval_fun1");
	ia=m_functions[fnum].f_args;
	
	f_entry=entry_mode;
	entry_mode=KNORMAL;
	// MESG(";eval_fun1: fnum=%d %d ia=%d entry_mode=%d",fnum,UFMAINARGLEN,ia,entry_mode);

	if(ia) {
		/* if we have arguments, check for parenthesis, then get the arguments  */
		for(i=0;i< ia;i++) { 
			NTOKEN2;
			value = num_expression();
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

	array_dat *arr=va[0].adat;
	ex_vtype = va[0].vtype;

	entry_mode=f_entry;
	// MESG(";eval_fun1: go eval! fnum=%d",fnum);
	/* and now evaluate it! */
	switch (fnum) {
		case UFDETERMINANT:
				if(ex_vtype==VTYPE_ARRAY) {
					if(arr->rows==arr->cols) value=determinant(arr);
					else {
						// MESG("wrong dimensions!");
						syntax_error("wrong dimensions for determinant",203);
						value=0;
					};
				} else {
					syntax_error("Not an array!",204);
					value=0;
				};
				ex_vtype=VTYPE_NUM;
				break;
		case UFINVERSE:{
				if(ex_vtype==VTYPE_ARRAY) {
					if(arr->rows==arr->cols) {
						value=determinant(arr);
					} else {
						syntax_error("wrong dimensions for determinant",205);
						value=0;
					};
					if(value!=0) {
						array_dat *inverse;
						inverse=cofactor2(arr,value);
						ex_array=inverse;
						ex_name="Inverse";
					};
				} else {
					syntax_error("Not an array!",206);
					value=0;
				}
			};break;
		case UFTRANSPOSE:
				if(ex_vtype==VTYPE_ARRAY) {
					array_dat *tarray;
					tarray = transpose(arr);
					ex_array=tarray;
					ex_name="Tranpose";
				} else {
					syntax_error("Not an array!",207);
					value=0;
				};
				break;
		case UFLENGTH:	
			if(va[0].vtype==VTYPE_STRING) { value =strlen(va[0].sval);}
			else if(ex_vtype==VTYPE_ARRAY || ex_vtype==VTYPE_SARRAY) {
				value=arr->rows*arr->cols;
			} else value=0;
			ex_vtype=VTYPE_NUM;
			break;			
		case UFLEFT:
			if(va[0].vtype==VTYPE_STRING && va[1].vtype==VTYPE_NUM ) {
			set_sval(va[0].sval);
			} else set_sval("");
			value = 0;
			ex_vtype=VTYPE_STRING;
			break;
		case UFRIGHT:{
			if(va[0].vtype==VTYPE_STRING && va[1].vtype==VTYPE_NUM ) {
				int r1=(int)va[1].dval;
				// MESG("right: r1=%d",r1);
				if(strlen(va[0].sval)<r1) r1=strlen(va[0].sval);
				clean_saved_string(r1);
				memcpy(saved_string,va[0].sval+(strlen(va[0].sval)-r1),r1);
				saved_string[r1]=0;
			} else set_sval("");

			ex_vtype=VTYPE_STRING;
			};break;
		case UFMID:	
			if(va[0].vtype==VTYPE_STRING && va[1].vtype==VTYPE_NUM && va[2].vtype==VTYPE_NUM) {
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
			value = 0;
			ex_vtype=VTYPE_STRING;
			break;
		case UFUPPER:
			if(va[0].vtype==VTYPE_STRING) {
				clean_saved_string(strlen(va[0].sval));
				get_uppercase_string(saved_string,va[0].sval);
			} else set_sval("");
			ex_vtype=VTYPE_STRING;
			
			break;
		case UFLOWER:
			if(va[0].vtype==VTYPE_STRING) {
				clean_saved_string(strlen(va[0].sval));
				get_lowercase_string(saved_string,va[0].sval);
			} else set_sval("");
			ex_vtype=VTYPE_STRING;
			break;
		case UFASCII:	
			if(va[0].vtype==VTYPE_STRING) {
			 value=va[0].sval[0];
			} else value=0;
			ex_vtype=VTYPE_NUM;
			break;
		case UFCHR:
			clean_saved_string(1);
			saved_string[0] = (int)va[0].dval;
			saved_string[1] = 0;
			value = va[0].dval;
			ex_vtype=VTYPE_STRING;
			break;
		case UFGTKEY:
			clean_saved_string(1);
			if(execmd) {
				saved_string[0]=getchar();
			} else {
				saved_string[0] = getcmd();
			};
			saved_string[1] = 0;
			ex_vtype=VTYPE_STRING;
			break;
		case UFRND:	/* returns big! int */
			value = rand();ex_vtype=VTYPE_NUM;break;
		case UFABS:	value = fabs(va[0].dval);ex_vtype=VTYPE_NUM;break;
		case UFSINDEX:	/* segmentation */
			if(va[0].vtype==VTYPE_STRING) {
				value = sindex(va[0].sval, va[1].sval);
			} else value=0;
			ex_vtype=VTYPE_NUM;
			break;
		case UFSTRING:	/* string of a value */
			if(va[0].vtype==VTYPE_NUM) {
				clean_saved_string(20);
				snprintf(saved_string,20,"%f",va[0].dval);
				value = va[0].dval;
			} else set_sval("");
			ex_vtype=VTYPE_STRING;
			break;
		case UFMESSAGE: if(va[0].vtype==VTYPE_STRING) msg_line("[%s]",va[0].sval);
				else msg_line("<%f>",va[0].dval);
				events_flush();
				break;
		case UFERROR: if(va[0].vtype==VTYPE_STRING) error_line("[%s]",va[0].sval);
				else error_line("<%f>",va[0].dval);
				events_flush();
				break;
		case UFWAIT: 
				if(va[0].vtype==VTYPE_STRING) msg_line("[%s] waiting.. ",va[0].sval);
				else msg_line("<%f> wait for key",va[0].dval);
				clean_saved_string(1);
				if(execmd) {
					saved_string[0]=getc(stdin);
				} else {
					events_flush();
					entry_mode=KENTRY;
					saved_string[0] = getcmd();
					entry_mode=KNORMAL;
				};
				saved_string[1] = 0;
				ex_value=saved_string[0];
				ex_vtype=VTYPE_STRING;
				break;
		case UFINPUT:   
				entry_mode=KENTRY;	/* get input from screen */
				clean_saved_string(80);
				if(va[0].vtype!=VTYPE_STRING) getstring("Input :",saved_string,80,true);
				else getstring(va[0].sval,saved_string,80,true);
				if(execmd) saved_string[strlen(saved_string)-1]=0;
				ex_vtype=VTYPE_STRING;
				value=0;
				break;
		case UFDINPUT:
				clean_saved_string(80);
				if(va[0].vtype!=VTYPE_STRING) getstring("DInput :",saved_string,80,true);
				getstring(va[0].sval,saved_string,80,true);
				value=atof(saved_string);
				clean_saved_string(0);
				ex_vtype=VTYPE_NUM;
				break;
		case UFINIT:	initialize_vars();
				break;
		case UFVAL:	
			if(va[0].vtype==VTYPE_STRING) 
				value=atof(va[0].sval);
			else value=0;
			ex_vtype=VTYPE_NUM;
			break;
		case UFSQRT: 
			if(va[0].vtype==VTYPE_NUM) 
				value=sqrt(va[0].dval);
			else value=0;
			ex_vtype=VTYPE_NUM;
			break;
		case UFSIN: 
			if(va[0].vtype==VTYPE_NUM)	
				value=sin(va[0].dval);
			else {
				syntax_error("math error in sin",305);
				value=0;
			};
			ex_vtype=VTYPE_NUM;
			break;
		case UFCOS: 
			if(va[0].vtype==VTYPE_NUM)	
			value=cos(va[0].dval);
			else {
				syntax_error("math error in cos",306);
				value=1;
			}
			ex_vtype=VTYPE_NUM;
			break;
		case UFTAN: 
			if(va[0].vtype==VTYPE_NUM)	
				value=tan(va[0].dval);
			else {
				syntax_error("math error in tan",307);
				value=0;
			};
				ex_vtype=VTYPE_NUM;
				break;
		case UFLOG10: 
			if(va[0].vtype==VTYPE_NUM)
				value=log10(va[0].dval);
			else {
				syntax_error("math error in log10",308);
				value=0;
			};
			ex_vtype=VTYPE_NUM;
			break;

		case UFATAN:
			if(va[0].vtype==VTYPE_NUM) 
				 value=atan(va[0].dval);
			else {
				syntax_error("math error in atan",309);
				value=0;
			};
			 ex_vtype=VTYPE_NUM;
			break;
		case UFLOGNAT:
			if(va[0].vtype==VTYPE_NUM) 
				value=log(va[0].dval);
			else {
				syntax_error("math error in lognat",310);
				value=0;
			};
			ex_vtype=VTYPE_NUM;
			break;
		case UFTRUNC:
			if(va[0].vtype==VTYPE_NUM) 
				value=trunc(va[0].dval);
			else {
				syntax_error("math error in trunc",311);
				value=0;
			};
			ex_vtype=VTYPE_NUM;
			break;
		case UFROUND:
			if(va[0].vtype==VTYPE_NUM) 
				value=round(va[0].dval);
			else {
				syntax_error("math error in round",312);
				value=0;
			};
				ex_vtype=VTYPE_NUM;
				break;
		case UFPRINT:	// to stdio, be carefull CHECK !!!!
				if(va[0].vtype==VTYPE_ARRAY) print_array1("",arr);
				else {
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
				break;
		case UFGETPOINT:
				break;
		case UFSTIME:
			if(va[0].vtype==VTYPE_STRING) {
			set_sval(va[0].sval);
			value=show_time(saved_string,va[1].dval);
			} else {
				syntax_error("error in stime",312);
			}
			ex_vtype=VTYPE_NUM;
			break;
		case UFDEQ:
			value=deq(va[0].dval,va[1].dval);
			ex_vtype=VTYPE_NUM;
			break;
		case UFCLS:
			cls_fout("[out]");
			break;
		case UFATBOF:
			return(FBof(cbfp));
		case UFATEOF:
			return(FEof(cbfp));
		case UFATBOL:
			return(FBolAt(cbfp,Offset()));
		case UFATEOL:
			return(FEolAt(cbfp,Offset()));
		case UFMAINARGLEN:
			if(main_args) {
			// MESG("argument size: rows=%d cols=%d",main_args->rows,main_args->cols);
			ex_vtype=VTYPE_NUM;
			return main_args->cols;
			} else return 0;
		case UFMAINARG: {
			if(!main_args) return 0;
			int ind;
			ind=(int)va[0].dval;
			if(ind<main_args->cols) {
				set_sval(main_args->sval[(int)va[0].dval]);
				ex_vtype=VTYPE_STRING;
				return atof(saved_string);
			} else {
				ex_vtype=VTYPE_NUM;
				return 0.0;
			};
		};
		default:
			value=0.0;
			ex_vtype=VTYPE_NUM;
	};
	// if(stat>MAXLLEN) MESG("truncated string eval function");
	RTRN(value);
}
#endif

m_function m_functions[] = {
	{"len",1,uf_len},        /* STRING LENGTH */
	{"cls",0,uf_cls},	/* clear output buffer  */
#if	0
	{"upper",1},        /* UPPERCASE STRING */
    {"lower",1},        /* LOWER CASE STRING */
	{"left",2},
	{"right",2},
	{"mid",3},
    {"s_asc",1},	/* CHAR TO INTEGER CONVERSION */
    {"chr",1},		/* INTEGER TO CHAR CONVERSION */
    {"getchar",0},	/* GET 1 CHARACTER */
    {"rand",0},        	/* GET A RANDOM NUMBER */
    {"abs",1},        	/* ABSOLUTE VALUE OF A NUMBER */
    {"s_index",2},      /* FIND THE INDEX OF ONE STRING IN ANOTHER */
	{"str",1},		/* string of a value */
	{"message",1},	/* show message on screen  */
	{"error_log",1},	/* show message error  */
	{"input",1},	/* string input from screen  */
	{"initialize",0},	/* variables init */
	{"s_val",1},	/* numerical string value  */
	{"sqrt",1},
	{"sin",1},
	{"cos",1},
	{"tan",1},
	{"atan",1},
	{"log10",1},
	{"log",1},
	{"trunc",1},
	{"round",1},
	{"print",1},	/* out_print, print on out buffer, and stdout if in Xwindows mode  */
	{"getpoint",0},
	{"message_wait",1},
	{"dinput",1},
	{"show_time",2},	/* show elapsed time in nanoseconds  */
	{"deq",2},	/* compare double with limited precission */
	{"cls",0},	/* clear output buffer  */
	{"DET",1},	/* determinant  */
	{"INV",1},	/* inverse  */
	{"T",1},	/* transpose  */
	{"at_bof",0},	/* if at begin of file  */
	{"at_eof",0},	/* if at end of file  */
	{"at_bol",0},	/* if at begin of line  */
	{"at_eol",0},	/* if at end of line  */
	{"args_size",0},	/* main arguments list size  */
	{"args",1},	/* main  argument at position */
#endif
	{NULL,0}
};

#endif