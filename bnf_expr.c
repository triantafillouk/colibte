
char *ddot_string();
void update_ddot_line(char *ddot_out);
void skip_sentence1();

static MVAR bnf_vars[500];
static MVAR *bnf_var=bnf_vars;

#if	TPROFILE
static long max_var=0;
static long var_index=0;
#endif

inline MVAR *get_left_slot(int ind);
static double bnf_expression();
int set_option_val(int vnum,char *svalue);
int set_option_bnf(int vnum,int ival);

#if	1
#if	1
#define	prev_var(x)	bnf_var--
#define	next_var(x)	bnf_var++

#else
inline static void prev_var(char *title)
{
	bnf_var--;
#if	TPROFILE
	var_index--;
#endif
}

inline static void next_var(char *title)
{
	bnf_var++;
#if	TPROFILE
	var_index++;
	if(max_var<var_index) max_var=var_index;
	if(bnf_var - &bnf_vars[0]>500) {
		MESG("MAX var exceeded!");
		exit(2);
	};
	if(bnf_var->var_type==VTYPE_STRING) {
		if(bnf_var->var_alloced) {
			MESG("next_var free [%s] -- [%s] alloced",tok_info(tok),bnf_var->sval);
			free(bnf_var->sval);
		};
		bnf_var->var_alloced=0;
		bnf_var->var_type=0;
	};
#endif
}
#endif
#else

void prev_var(char *title)
{
 MESG(" - var %d %s",(int)(bnf_var-bnf_vars),title);
 // MESG("	%3d -var %3ld -> %3ld %s",tok->tnum,(long)(bnf_var-bnf_vars),(long)(bnf_var-bnf_vars-1),title);
 if(bnf_var>bnf_vars) bnf_var--;
 else {
	MESG("prev_var (%s) from 0 at [%s]",title,tok_info(tok));
 };
#if	1
 if(bnf_var < &bnf_vars[0]) {
	// MESG("Negativ var!");
	exit(1);
 };
#endif
}

void next_var(char *title)
{
 MESG(" + var %d %s",(int)(bnf_var-bnf_vars),title);
 // MESG("	%3d +var %3ld -> %3ld %s",tok->tnum,(long)(bnf_var-bnf_vars),(long)(bnf_var-bnf_vars+1),title);
 bnf_var++;
	if(bnf_var->var_type==VTYPE_STRING) {
		if(bnf_var->var_alloced) free(bnf_var->sval);
		bnf_var->var_alloced=0;
	};
 if(bnf_var - &bnf_vars[0]>100) {
	MESG("MAX var exceeded!");
	exit(2);
 };
}
#endif


inline static void set_var_value()
{
#if	1
	if(bnf_var->var_type==VTYPE_POINTER)
		memmove(bnf_var,bnf_var->var_pointer,sizeof(struct MVAR));
#else
 if(bnf_var->var_type==VTYPE_POINTER) {
 	bnf_var->var_type=bnf_var->var_pointer->var_type;
	if(bnf_var->var_type==VTYPE_NUM) {
		bnf_var->dval=bnf_var->var_pointer->dval;
		// MESG("	set_var_value num %f",bnf_var->dval);
	} else if(bnf_var->var_type==VTYPE_STRING) {
		bnf_var->sval=strdup(bnf_var->var_pointer->sval);
		bnf_var->var_alloced=1;
	} else if (bnf_var->var_type==VTYPE_ARRAY) {
		bnf_var->adat=bnf_var->var_pointer->adat;
		bnf_var->var_alloced=0;
	};
 };
#endif
}

void bnf_refresh_ddot()
{
 // MESG("refresh_ddot: var type=%d",bnf_var->var_type);
 int stat=0;
 double value=0;
 TextPoint *tp = tok->ddot;
 FILEBUF *buf = tp->fp;

 MVAR *var_show = (bnf_var->var_type==VTYPE_POINTER) ? bnf_var->var_pointer: bnf_var;

 if(execmd) {
	 if(var_show->var_type==VTYPE_NUM) {
		printf("%s	: %.3f\n",ddot_string(),var_show->dval);
	 } else if(var_show->var_type==VTYPE_STRING) {
		printf("%s	: %s\n",ddot_string(),var_show->sval);
	 } else if(var_show->var_type==VTYPE_ARRAY || var_show->var_type==VTYPE_SARRAY ||var_show->var_type==VTYPE_AMIXED) {
		// MESG("	show_array!");
		print_array1("array: ",var_show->adat);
	 	// print_array1(ddot_string(),var_show->adat);
	 };

	 // NTOKEN2;
	 if(tok->ttype != TOK_END) prev_var("ddot");
	 return;
 };

 int precision=bt_dval("print_precision");
 int show_hex=bt_dval("show_hex");
 char ddot_out[128];

 // MESG("	ddot_pos=%d end=%d todel=%d",ddot_position,line_end,line_end-ddot_position);
 if(buf->b_state & FS_VIEW) {
 	// NTOKEN2;
	return;
 }; // no refresh in view mode

 if(var_show->var_type==VTYPE_STRING) {	/* string value  */
	stat=snprintf(ddot_out,sizeof(ddot_out)," \"%s\"",var_show->sval);
 }  else if(var_show->var_type==VTYPE_NUM) {	/* numeric value  */
	long int d = (long int)value;
	if(d==value) {	/* an integer/double value!  */
		if(show_hex) stat=snprintf(ddot_out,sizeof(ddot_out)," %5.0f | 0x%llX | 0o%llo",value,(unsigned long long)value,(unsigned long long)value);
		else stat=snprintf(ddot_out,sizeof(ddot_out)," %5.*f",1,value);
	} else {	/* a decimal value!  */
		stat=snprintf(ddot_out,sizeof(ddot_out)," %5.*f",precision,value);
	};

 } else if(var_show->var_type==VTYPE_ARRAY || var_show->var_type==VTYPE_SARRAY || var_show->var_type==VTYPE_AMIXED) {
	// MESG("refresh_ddot:1");
	array_dat *adat = var_show->adat;
	// MESG("refresh_ddot: array: type=%d name=(%s)",adat->atype,adat->array_name);

 	stat=snprintf(ddot_out,sizeof(ddot_out)," array %d:[%s] type [%s] , slot %ld type=%d rows %d,cols %d",adat->anum,
		adat->array_name,vtype_names[adat->atype],lsslot-current_stable,adat->atype,adat->rows,adat->cols);
	// print_array1(":",adat);
 };
 if(stat>MAXLLEN) MESG("truncated");

 update_ddot_line(ddot_out);
 // NTOKEN2;
}

inline static double num_result()
{
  // MESG("num_result: %s",tok_info(tok));
  MVAR *num_var = (bnf_var->var_type==VTYPE_POINTER) ? bnf_var->var_pointer:bnf_var;
  if(num_var->var_type==VTYPE_NUM) {
 	return num_var->dval;
  };
  return 0.0;
}

inline static char * string_result()
{
 MVAR *s_var = (bnf_var->var_type==VTYPE_POINTER) ? bnf_var->var_pointer:bnf_var;
 if(s_var->var_type==VTYPE_STRING) {
 	return s_var->sval;
 };
 return "";
}

void show_result()
{
 int stack_num = (int)(bnf_var-bnf_vars);
 // MESG("# show_result: stack_num=%d type=%d\n",stack_num,bnf_var->var_type);
 switch(bnf_var->var_type) {
 	case VTYPE_NUM:
		MESG("##: @%3d NUMERIC val=%f",stack_num,bnf_var->dval);
		return;
	case VTYPE_STRING:
		MESG("##: @%3d STRING  val=[%s]",stack_num,bnf_var->sval);
		return;
	case VTYPE_POINTER:
	{ MVAR *var=bnf_var->var_pointer;
		if(var->var_type==VTYPE_NUM) {
			MESG("##: @%3d var NUMERIC val=%f",stack_num,var->dval);
			return;
		};
		if(var->var_type==VTYPE_STRING) {
			MESG("##: @%3d var STRING  val=[%s]",stack_num,var->sval);
			return;
		};
		if(var->var_type==VTYPE_NONE) {
			MESG("##: @%3d var other undefined",stack_num);
			return;
		};
	};break;
	default:
		MESG("##: @%3d  type %X",stack_num,bnf_var->var_type);
 };
}

void show_results()
{
 while(bnf_var>=bnf_vars) {
	show_result();
	bnf_var--;
 };
}

#if	NUSE
inline static double return_result()
{
 int stack_num = bnf_var-bnf_vars;
 printf("return_result: stack_num=%d type=%d\n",stack_num,bnf_var->var_type);
 switch(bnf_var->var_type) {
 	case VTYPE_NUM:
		MESG("bnf result: @%3d NUMERIC val=%f",stack_num,bnf_var->dval);
		return(bnf_var->dval);
	case VTYPE_STRING:
		MESG("bnf result: @%3d STRING  val=[%s]",stack_num,bnf_var->sval);
		return 0.0;
	case VTYPE_POINTER:
	{ MVAR *var=bnf_var->var_pointer;
		if(var->var_type==VTYPE_NUM) {
			MESG("bnf1 result: @%3d var NUMERIC val=%f",stack_num,var->dval);
			return var->dval;
		};
		if(var->var_type==VTYPE_STRING) {
			MESG("bnf result: @%3d var STRING  val=[%s]",stack_num,var->sval);
			return 0.0;
		};
		if(var->var_type==VTYPE_NONE) {
			MESG("bnf result: @%3d var value undefined",stack_num);
			return 0.0;
		};
	};break;
	default:
		MESG("bnf result: @%3d  type %X",stack_num,bnf_var->var_type);
 };
 return 0.0;
}
#endif

void bnf_factor_var()
{
	next_var("var");
	bnf_var->var_pointer=get_left_slot(tok->tind);
	bnf_var->var_type=VTYPE_POINTER;
	// MESG("## factor_var: put var %s ind=%d %p at pos %ld",tok->tname,tok->tind,bnf_var->var_pointer,bnf_var-bnf_vars);
	// bnf_var->var_alloced=0;
}

void bnf_factor_num()
{
	next_var("num");
	bnf_var->dval = tok->dval;
	bnf_var->var_type=VTYPE_NUM;
	// MESG("bnf_factor_num: put numeric %f at pos %ld type=%d",bnf_var->dval,bnf_var-bnf_vars,bnf_var->var_type);
}

void bnf_factor_quote()
{
	next_var("quote");
	bnf_var->var_type=VTYPE_STRING;
	bnf_var->sval = tok->tname;
	bnf_var->var_alloced=0;
	// MESG("bnf_factor_quote: put quote '%s' at pos %d type %d",bnf_var->sval,(int)(bnf_var-bnf_vars),bnf_var->var_type);
}

void bnf_factor_not()
{

  MVAR *num_var = (bnf_var->var_type==VTYPE_POINTER) ? bnf_var->var_pointer:bnf_var;
 // MESG("bnf_factor_not:");
 if(num_var->var_type == VTYPE_NUM) {
 	bnf_var->dval = num_var->dval==0 ? 1:0;
	bnf_var->var_type=VTYPE_NUM;
	return;
 };
 set_error(tok,1027,"factor not error!");
}

inline static void  bnf_factor_pp_plus()
{
 // MESG("bnf_factor_pp_plus : var ind=%d tok ind=%d var type=%d",bnf_var-&bnf_vars[0],tok->tnum,bnf_var->var_type);
 double valb = bnf_var->var_pointer->dval;
	prev_var("pp_plus");
	bnf_var->dval = bnf_var->var_pointer->dval + valb;
	bnf_var->var_type=VTYPE_NUM;
}

inline static void  bnf_factor_pp_minus()
{
 // MESG("bnf_factor_pp_plus : var ind=%d tok ind=%d var type=%d",bnf_var-&bnf_vars[0],tok->tnum,bnf_var->var_type);
 double valb = bnf_var->var_pointer->dval;
	prev_var("pp_minus");
	bnf_var->dval = bnf_var->var_pointer->dval - valb;
	bnf_var->var_type=VTYPE_NUM;
}

inline static void  bnf_factor_nn_plus()
{
 // MESG("bnf_factor_plus : var ind=%d tok ind=%d var type=%d",bnf_var-&bnf_vars[0],tok->tnum,bnf_var->var_type);
 double valb = bnf_var->dval;
	prev_var("nn_plus");
	bnf_var->dval += valb;
}

inline static void  bnf_factor_pn_plus()
{
 // MESG("bnf_factor_pn_plus : var ind=%d tok ind=%d var type=%d",bnf_var-&bnf_vars[0],tok->tnum,bnf_var->var_type);
 double valb = bnf_var->dval;
	prev_var("pn_plus");
	bnf_var->dval = bnf_var->var_pointer->dval + valb;
	bnf_var->var_type=VTYPE_NUM;
}

inline static void  bnf_factor_np_plus()
{
 // MESG("bnf_factor_plus : var ind=%d tok ind=%d var type=%d",bnf_var-&bnf_vars[0],tok->tnum,bnf_var->var_type);
 double valb = bnf_var->var_pointer->dval;
	prev_var("np_plus");
	bnf_var->dval += valb;
}

inline static void  bnf_factor_np_minus()
{
 // MESG("bnf_factor_minus : var ind=%d tok ind=%d var type=%d",bnf_var-&bnf_vars[0],tok->tnum,bnf_var->var_type);
 double valb = bnf_var->var_pointer->dval;
	prev_var("np_minus");
	bnf_var->dval -= valb;
}

void set_bnf_function(tok_struct *tok, char *label, VFunction function)
{
	tok->bnf_factor_function=function;
	// MESG("- set bnf function: to %s [%s]",label,tok_info(tok));
}

static void bnf_factor_spn_plus()
{
 double valb=bnf_var->var_pointer->dval;
 prev_var("spn_plus");
 char *svala=bnf_var->sval;
 char svalue[MAXLLEN];
	// MESG("	varb pointer num = %f",varb->var_pointer->dval);
	int stat;
	double l0 = trunc(valb);

	if(l0 == valb) stat=snprintf(svalue,sizeof(svalue),"%s%.0f",svala,l0);
	else stat=snprintf(svalue,sizeof(svalue),"%s%f",svala,valb);

	if(stat>MAXLLEN) MESG("truncated spn_plus");
	if(bnf_var->var_alloced) free(svala);
	bnf_var->sval=strdup(svalue);
	bnf_var->var_alloced=1;
	// MESG("var string + varb pointer num! [%s]",vara->sval);
	return;
}

void bnf_factor_plus()
{
 MVAR *varb = bnf_var;
	prev_var("plus");
 MVAR *vara = bnf_var;
 int typea=vara->var_type;
 // int typeb=varb->var_type;

 // MESG("bnf_factor_plus [%2d]: var ind=%d  va=%d vb=%d",tok->tnum,(int)(bnf_var-bnf_vars),vara->var_type,varb->var_type);
 if(varb->var_type==VTYPE_POINTER) {
 	if(varb->var_pointer->var_type==VTYPE_NUM) {
		if(vara->var_type==VTYPE_POINTER) {
			if(vara->var_pointer->var_type==VTYPE_NUM) {
				// MESG("-plus vara num = %f",vara->var_pointer->dval);
				bnf_var->dval=vara->var_pointer->dval + varb->var_pointer->dval;
				bnf_var->var_type=VTYPE_NUM;
				// tok->bnf_factor_function=bnf_factor_pp_plus;
				// MESG("set factor_function to pp_plus [%s]",tok_info(tok));
				set_bnf_function(tok,"pp_plus",bnf_factor_pp_plus);
				return;
			} else if(vara->var_pointer->var_type==VTYPE_STRING) {
				// MESG("-plus vara pointer string is [%s] ....TBD",vara->var_pointer->sval);
				return;
			};
		} else if (vara->var_type==VTYPE_NUM) {
			bnf_var->dval += varb->var_pointer->dval;
			// tok->bnf_factor_function=bnf_factor_np_plus;
			// MESG("set factor_function to np_plus [%s]",tok_info(tok));
			set_bnf_function(tok,"np_plus",bnf_factor_np_plus);
			return;
		} else if(vara->var_type==VTYPE_STRING) {
			// MESG("-plus vara string is [%s]",vara->sval);
			char svalue[MAXLLEN];
			if(varb->var_pointer->var_type==VTYPE_NUM) { // string + num
				// MESG("	varb pointer num = %f",varb->var_pointer->dval);
				double l0 = trunc(varb->var_pointer->dval);
				int stat; 

				if(l0 == varb->var_pointer->dval) stat=snprintf(svalue,sizeof(svalue),"%s%.0f",vara->sval,l0);
				else stat=snprintf(svalue,sizeof(svalue),"%s%f",vara->sval,varb->var_pointer->dval);

				if(stat>MAXLLEN) MESG("truncated 2");
				if(vara->var_alloced) free(vara->sval);
				vara->sval=strdup(svalue);
				// MESG("var string + varb pointer num! [%s]",vara->sval);
				// MESG("set factor_function to spn_plus [%s]",tok_info(tok));
				// tok->bnf_factor_function=bnf_factor_spn_plus;
				set_bnf_function(tok,"spn_plus",bnf_factor_spn_plus);
				return;
			} else if(varb->var_pointer->var_type==VTYPE_STRING) { // string + string
				// MESG("	varb pointer string = %s",varb->var_pointer->sval);
				int stat = snprintf(svalue,sizeof(svalue),"%s%s",vara->sval,varb->var_pointer->sval);
				if(stat>MAXLLEN) MESG("truncated 2");
				// MESG("-plus string + var string = [%s]",svalue);
				if(vara->var_alloced) free(vara->sval);
				vara->sval=strdup(svalue);
				return;
			};
		}; 
	} else if (varb->var_pointer->var_type==VTYPE_STRING) {
		MVAR *var1=vara;
		if(var1->var_type==VTYPE_POINTER) var1=vara->var_pointer;
		if(var1->var_type==VTYPE_STRING) {
			char svalue[MAXLLEN];
			unsigned long stat;
			stat = snprintf(svalue,sizeof(svalue),"%s%s",var1->sval,varb->var_pointer->sval);
			if(stat>=sizeof(svalue)) MESG("truncated s+s");
			if(var1->var_type==VTYPE_STRING && vara->var_alloced) free(vara->sval);
			vara->sval=strdup(svalue);
			vara->var_alloced=1;
		} else if(var1->var_type==VTYPE_NUM) {
			char svalue[MAXLLEN];
			unsigned long stat;
			double l0 = trunc(var1->dval);
			if(l0 == var1->dval) stat=snprintf(svalue,sizeof(svalue),"%.0f%s",l0,varb->var_pointer->sval);
			else stat=snprintf(svalue,sizeof(svalue),"%f%s",var1->dval,varb->var_pointer->sval);

			if(stat>=sizeof(svalue)) MESG("truncated s+s");
			vara->sval=strdup(svalue);
			vara->var_alloced=1;
			vara->var_type=VTYPE_STRING;
			// MESG("- plus varb pointer string %s",varb->var_pointer->sval);
		};
		return;
	};
 } else if(varb->var_type==VTYPE_NUM) {
 	if(vara->var_type==VTYPE_POINTER) {
		if(vara->var_pointer->var_type==VTYPE_NUM) {
			bnf_var->dval = vara->var_pointer->dval + varb->dval;
			bnf_var->var_type=VTYPE_NUM;
			// tok->bnf_factor_function=bnf_factor_pn_plus;
			// MESG("set factor_function to pn_plus [%s]",tok_info(tok));
			set_bnf_function(tok,"pn_plus",bnf_factor_pn_plus);
			return;
		} else if(vara->var_pointer->var_type==VTYPE_STRING) {
			char svalue[MAXLLEN];
			int stat = snprintf(svalue,sizeof(svalue),"%s%f",vara->var_pointer->sval,varb->dval);
			if(stat>MAXLLEN) MESG("truncated 2");
			vara->sval=strdup(svalue);
			vara->var_type=VTYPE_STRING;
			return;
		};
	} else if (vara->var_type==VTYPE_NUM) {
 			bnf_var->dval += varb->dval;
			set_bnf_function(tok,"nn_plus",bnf_factor_nn_plus);
			return;
	} else if(vara->var_type==VTYPE_STRING) {
			char svalue[MAXLLEN];
			int stat = snprintf(svalue,sizeof(svalue),"%s%f",vara->sval,varb->dval);
			if(stat>=MAXLLEN) MESG("truncated 2");
			free(vara->sval);
			vara->sval=strdup(svalue);
			return;
	};
	// MESG("plus varb=num!");
 } else if(varb->var_type==VTYPE_STRING) {
	char svalue[MAXLLEN];
	int stat;
 	// MESG("plus varb=string!");
	if(typea==VTYPE_STRING) {
		stat=snprintf(svalue,sizeof(svalue),"%s%s",vara->sval,varb->sval);
		if(stat>MAXLLEN) MESG("truncated 2");
	} else if (typea==VTYPE_NUM) {
		stat=snprintf(svalue,sizeof(svalue),"%f%s",vara->dval,varb->sval);
		if(stat>MAXLLEN) MESG("truncated 2");
	};
	bnf_var->sval=strdup(svalue);
	bnf_var->var_alloced=1;
	if(varb->var_alloced) free(varb->sval);
	varb->var_alloced=0;
	// MESG("string plus ind=%d [%s]",(int)(bnf_var-bnf_vars),bnf_var->sval);
	return;
 };
 	set_error(tok,1028,"plus error");
 	// MESG("plus error! atype=%d btype=%d [%s]",typea,typeb,tok_info(tok));
}

inline static void  bnf_factor_pn_minus()
{
 // MESG("bnf_factor_pn_minus : var ind=%d tok ind=%d var type=%d",bnf_var-&bnf_vars[0],tok->tnum,bnf_var->var_type);
 double valb = bnf_var->dval;
	prev_var("pn_minus");
	bnf_var->dval = bnf_var->var_pointer->dval - valb;
	bnf_var->var_type=VTYPE_NUM;
}

void bnf_factor_minus()
{
 // MESG("bnf_factor_minus : var ind=%d tok ind=%d var type=%d",bnf_var-&bnf_vars[0],tok->tnum,bnf_var->var_type);
 MVAR *varb = bnf_var;
 prev_var("minus");
 MVAR *vara = bnf_var;
 int typea=vara->var_type;
 int typeb=varb->var_type;

 // MESG("bnf_factor_minus [%2d]: var ind=%d  va=%d vb=%d",tok->tnum,(int)(bnf_var-bnf_vars),vara->var_type,varb->var_type);
 if(varb->var_type==VTYPE_POINTER) {
 	if(varb->var_pointer->var_type==VTYPE_NUM) {
		if(vara->var_type==VTYPE_POINTER) {
			if(vara->var_pointer->var_type==VTYPE_NUM) {
				// MESG("- minus vara nump = %f - %f",vara->var_pointer->dval,varb->var_pointer->dval);
				bnf_var->dval=vara->var_pointer->dval - varb->var_pointer->dval;
				bnf_var->var_type=VTYPE_NUM;
				// tok->bnf_factor_function=bnf_factor_pp_minus;
				// MESG("set factor_function to pp_minus [%s]",tok_info(tok));
				set_bnf_function(tok,"pp_minus",bnf_factor_pp_minus);
				return;
			}
		} else if (vara->var_type==VTYPE_NUM) {
			// MESG(" - minus var num = %f - %f",vara->dval,varb->var_pointer->dval);
			bnf_var->dval -= varb->var_pointer->dval;
			set_bnf_function(tok,"np_minus",bnf_factor_np_minus);
			return;
		} 
	};
 } else if(varb->var_type==VTYPE_NUM) {
 	if(vara->var_type==VTYPE_POINTER) {
		if(vara->var_pointer->var_type==VTYPE_NUM) {
			// MESG(" - minus var num = %f - %f",vara->var_pointer->dval,varb->dval);
			bnf_var->dval = vara->var_pointer->dval - varb->dval;
			bnf_var->var_type=VTYPE_NUM;
			set_bnf_function(tok,"pn_minus",bnf_factor_pn_minus);
			return;
		} else {
			MESG(" - minus vara->var_pointer->var_type NOT VTYPE_NUM! %d",vara->var_pointer->var_type);
		};
	} else if (vara->var_type==VTYPE_NUM) {
			// MESG(" - minus var num = %f - %f",vara->dval,varb->dval);
 			bnf_var->dval -= varb->dval;
			// MESG("set factor_function (%f) to nn_plus [%s]",bnf_var->dval,tok_info(tok));
			// tok->bnf_factor_function=bnf_factor_nn_minus;
			// set_bnf_function(tok,"nn_minus",bnf_factor_nn_minus);
			return;
	}
	// MESG("plus varb=num!");
 } 
 	MESG("minus error! atype=%d btype=%d [%s]",typea,typeb,tok_info(tok));
}

inline static void bnf_factor_np_num_mul()
{
 double valb = bnf_var->var_pointer->dval;
 prev_var("np_mul");
 bnf_var->dval *= valb;
}

inline static void bnf_factor_nn_num_mul()
{
 double valb = bnf_var->dval;
 prev_var("nn_mul");
 bnf_var->dval *= valb;
}

inline static void bnf_factor_pp_num_mul()
{
 double valb = bnf_var->var_pointer->dval;
 prev_var("pp_mul");
 // bnf_var->var_pointer->dval *= valb;
 double vala = bnf_var->var_pointer->dval;
 bnf_var->dval=vala * valb;
 bnf_var->var_type=VTYPE_NUM;
 // MESG("bnf_factor_pp_num_mul: %f",bnf_var->dval);
}

inline static void bnf_factor_mul()
{
 // MESG("bnf_factor_mul : var ind=%d tok ind=%d var type=%d",bnf_var-&bnf_vars[0],tok->tnum,bnf_var->var_type);
 MVAR *varb = bnf_var;
 int vb=varb->var_type;
 	if(varb->var_type==VTYPE_POINTER){ varb=varb->var_pointer;vb=VTYPE_POINTER;};
	prev_var("mul");
	MVAR *vara = bnf_var;
	int va=vara->var_type;
	if(vara->var_type==VTYPE_POINTER) {vara=vara->var_pointer;va=VTYPE_POINTER;};
	if(vara->var_type==VTYPE_NUM) {
		if(varb->var_type==VTYPE_NUM) {
			bnf_var->dval=vara->dval*varb->dval;
			bnf_var->var_type=VTYPE_NUM;
			if(va==VTYPE_POINTER && vb==VTYPE_POINTER) {
				set_bnf_function(tok,"pp_num_mul",bnf_factor_pp_num_mul);
			};
			if(va==VTYPE_NUM && vb==VTYPE_NUM) {
				set_bnf_function(tok,"nn_num_mul",bnf_factor_nn_num_mul);
			};
			if(va==VTYPE_NUM && vb==VTYPE_POINTER) {
				set_bnf_function(tok,"np_num_mul",bnf_factor_np_num_mul);
			}
			return;
		};
	};
 	MESG("num error! [%s]",tok_info(tok));
	set_error(tok,1029,"mul error");
}


inline static void bnf_factor_div()
{
 // MESG("bnf_factor_div : var ind=%d tok ind=%d var type=%d",bnf_var-&bnf_vars[0],tok->tnum,bnf_var->var_type);
 MVAR *varb = bnf_var;
 	if(varb->var_type==VTYPE_POINTER) varb=varb->var_pointer;
	prev_var("div21");
	MVAR *vara = bnf_var;
	if(vara->var_type==VTYPE_POINTER) vara=vara->var_pointer;
	if(vara->var_type==VTYPE_NUM) {
		if(varb->var_type==VTYPE_NUM) {
			bnf_var->dval=vara->dval/varb->dval;
			bnf_var->var_type=VTYPE_NUM;
			return;
		};
	};
	set_error(tok,1030,"div error");
 	// MESG("div error!");
}

inline static void bnf_factor_modulo()
{
 // MESG("bnf_factor_modulo : var ind=%d tok ind=%d var type=%d",bnf_var-&bnf_vars[0],tok->tnum,bnf_var->var_type);
 MVAR *varb = bnf_var;
 	if(varb->var_type==VTYPE_POINTER) varb=varb->var_pointer;
	prev_var("mod21");
	MVAR *vara = bnf_var;
	if(vara->var_type==VTYPE_POINTER) vara=vara->var_pointer;
	if(vara->var_type==VTYPE_NUM) {
		if(varb->var_type==VTYPE_NUM) {
			bnf_var->dval=(int)vara->dval%(int)varb->dval;
			bnf_var->var_type=VTYPE_NUM;
			// NTOKEN2;
			return;
		};
	};
	set_error(tok,1031,"mod error");
 	// MESG("mod error!");
}

inline static void bnf_factor_power()
{
 // MESG("bnf_factor_power : var ind=%d tok ind=%d var type=%d",bnf_var-&bnf_vars[0],tok->tnum,bnf_var->var_type);
 MVAR *varb = bnf_var;
 	if(varb->var_type==VTYPE_POINTER) varb=varb->var_pointer;
	prev_var("power");
	MVAR *vara = bnf_var;
	if(vara->var_type==VTYPE_POINTER) vara=vara->var_pointer;
	if(vara->var_type==VTYPE_NUM) {
		if(varb->var_type==VTYPE_NUM) {
			bnf_var->dval=pow(vara->dval,varb->dval);
			bnf_var->var_type=VTYPE_NUM;
			return;
		};
	};
 	set_error(tok,1032,"power error");
	// MESG("power error!");
}

inline static void bnf_factor_smaller_pn()
{
 MVAR *varb = bnf_var;
	prev_var("smaller_pn");
	bnf_var->dval = bnf_var->var_pointer->dval < varb->dval;
	bnf_var->var_type=VTYPE_NUM;
}

inline static void bnf_factor_smaller()
{
 // MESG("bnf_factor_smaller : var ind=%d tok ind=%d var type=%d",bnf_var-&bnf_vars[0],tok->tnum,bnf_var->var_type);
 MVAR *varb = bnf_var;
 int varb_type=varb->var_type;
 	if(varb->var_type==VTYPE_POINTER) varb=varb->var_pointer;
	prev_var("smaller");
	MVAR *vara = bnf_var;
	int vara_type=vara->var_type;
	if(vara->var_type==VTYPE_POINTER) vara=vara->var_pointer;
	if(vara->var_type==VTYPE_NUM) {
		if(varb->var_type==VTYPE_NUM) {
			// MESG("< numeric!");
			bnf_var->dval=vara->dval < varb->dval;
			bnf_var->var_type=VTYPE_NUM;
			if(vara_type==VTYPE_POINTER && varb_type==VTYPE_NUM) {
				// tok->bnf_factor_function=bnf_factor_smaller_pn;
				// MESG("set bnf_function to smaller_pn [%s]",tok_info(tok));
				set_bnf_function(tok,"smaller_pn",bnf_factor_smaller_pn);
			};
			return;
		};
	};
	set_error(tok,1033,"smaller error");
 	// MESG("smaller error!");
}


inline static void bnf_factor_bigger()
{
 // MESG("bnf_factor_bigger : var ind=%d tok ind=%d var type=%d",bnf_var-&bnf_vars[0],tok->tnum,bnf_var->var_type);
 MVAR *varb = bnf_var;
 	if(varb->var_type==VTYPE_POINTER) varb=varb->var_pointer;
	prev_var("bigger");
	MVAR *vara = bnf_var;
	if(vara->var_type==VTYPE_POINTER) vara=vara->var_pointer;
	if(vara->var_type==VTYPE_NUM) {
		if(varb->var_type==VTYPE_NUM) {
			bnf_var->dval=vara->dval > varb->dval;
			bnf_var->var_type=VTYPE_NUM;
			return;
		};
	};
	set_error(tok,1034,"bigger error!");
}

inline static void bnf_factor_smallereq()
{
 // MESG("bnf_factor_smallereq : var ind=%d tok ind=%d var type=%d",bnf_var-&bnf_vars[0],tok->tnum,bnf_var->var_type);
 MVAR *varb = bnf_var;
 	if(varb->var_type==VTYPE_POINTER) varb=varb->var_pointer;
	prev_var("<=");
	MVAR *vara = bnf_var;
	if(vara->var_type==VTYPE_POINTER) vara=vara->var_pointer;
	if(vara->var_type==VTYPE_NUM) {
		if(varb->var_type==VTYPE_NUM) {
			bnf_var->dval=vara->dval <= varb->dval;
			bnf_var->var_type=VTYPE_NUM;
			return;
		};
	};
 	set_error(tok,1035,"<= error!");
}

inline static void bnf_factor_biggereq()
{
 // MESG("bnf_factor_biggereq : var ind=%d tok ind=%d var type=%d",bnf_var-&bnf_vars[0],tok->tnum,bnf_var->var_type);
 MVAR *varb = bnf_var;
 	if(varb->var_type==VTYPE_POINTER) varb=varb->var_pointer;
	prev_var("biggereq");
	MVAR *vara = bnf_var;
	if(vara->var_type==VTYPE_POINTER) vara=vara->var_pointer;
	if(vara->var_type==VTYPE_NUM) {
		if(varb->var_type==VTYPE_NUM) {
			bnf_var->dval=vara->dval >= varb->dval;
			bnf_var->var_type=VTYPE_NUM;
			return;
		};
	};
 	set_error(tok,1036,"biggereq error!");
}


inline static void bnf_factor_equal()
{
 // MESG("bnf_factor_equal : var ind=%d tok ind=%d var type=%d",bnf_var-&bnf_vars[0],tok->tnum,bnf_var->var_type);
 MVAR *varb = bnf_var;
 	if(varb->var_type==VTYPE_POINTER) varb=varb->var_pointer;
	prev_var("equal");
	MVAR *vara = bnf_var;
	if(vara->var_type==VTYPE_POINTER) vara=vara->var_pointer;
	if(vara->var_type==VTYPE_NUM) {
		if(varb->var_type==VTYPE_NUM) {
			bnf_var->dval=vara->dval == varb->dval;
			bnf_var->var_type=VTYPE_NUM;
			return;
		};
	};
 	set_error(tok,1037,"equal error!");
}

inline static void bnf_factor_notequal()
{
 MESG("bnf_factor_notequal : var ind=%d tok ind=%d var type=%d",bnf_var-&bnf_vars[0],tok->tnum,bnf_var->var_type);
 MVAR *varb = bnf_var;
 	if(varb->var_type==VTYPE_POINTER) varb=varb->var_pointer;
	prev_var("notequal");
	MVAR *vara = bnf_var;
	if(vara->var_type==VTYPE_POINTER) vara=vara->var_pointer;
	if(vara->var_type==VTYPE_NUM) {
		if(varb->var_type==VTYPE_NUM) {
			bnf_var->dval=vara->dval != varb->dval;
			bnf_var->var_type=VTYPE_NUM;
			return;
		};
	};
 	set_error(tok,1038,"notequal error!");
}

inline static void bnf_factor_and()
{
 MVAR *bvar = (bnf_var->var_type==VTYPE_POINTER) ? bnf_var->var_pointer: bnf_var;
 if(bvar->var_type == VTYPE_NUM) {
 	int val=bvar->dval>0;
	prev_var("xor2");

	MVAR *avar = (bnf_var->var_type==VTYPE_POINTER) ? bnf_var->var_pointer: bnf_var;
	if(avar->var_type == VTYPE_NUM) {
		bnf_var->dval = (avar->dval>0) && val;
		bnf_var->var_type=VTYPE_NUM;
		return;
	};
 };
 set_error(tok,1039,"and error");
}

inline static void bnf_factor_nand()
{
 MVAR *bvar = (bnf_var->var_type==VTYPE_POINTER) ? bnf_var->var_pointer: bnf_var;
 if(bvar->var_type == VTYPE_NUM) {
 	int val=bvar->dval>0;
	prev_var("xor2");

	MVAR *avar = (bnf_var->var_type==VTYPE_POINTER) ? bnf_var->var_pointer: bnf_var;
	if(avar->var_type == VTYPE_NUM) {
		bnf_var->dval = !((avar->dval>0) && val);
		bnf_var->var_type=VTYPE_NUM;
		return;
	};
 };
 set_error(tok,1040,"and error");
}

inline static void bnf_factor_or()
{
 MVAR *bvar = (bnf_var->var_type==VTYPE_POINTER) ? bnf_var->var_pointer: bnf_var;
 if(bvar->var_type == VTYPE_NUM) {
 	int val=bvar->dval>0;
	prev_var("xor2");

	MVAR *avar = (bnf_var->var_type==VTYPE_POINTER) ? bnf_var->var_pointer: bnf_var;
	if(avar->var_type == VTYPE_NUM) {
		bnf_var->dval = (avar->dval>0) || val;
		bnf_var->var_type=VTYPE_NUM;
		// NTOKEN2;
		return;
	};
 };
 set_error(tok,1041,"or error");
}

inline static void bnf_factor_nor()
{
 MVAR *bvar = (bnf_var->var_type==VTYPE_POINTER) ? bnf_var->var_pointer: bnf_var;
 if(bvar->var_type == VTYPE_NUM) {
 	int val=bvar->dval>0;
	prev_var("xor2");

	MVAR *avar = (bnf_var->var_type==VTYPE_POINTER) ? bnf_var->var_pointer: bnf_var;
	if(avar->var_type == VTYPE_NUM) {
		bnf_var->dval = !((avar->dval>0) || val);
		bnf_var->var_type=VTYPE_NUM;
		return;
	};
 };
 set_error(tok,1042,"nor error");
}

inline static void bnf_factor_xor()
{
 MVAR *bvar = (bnf_var->var_type==VTYPE_POINTER) ? bnf_var->var_pointer: bnf_var;
 if(bvar->var_type == VTYPE_NUM) {
 	int val=bvar->dval>0;
	prev_var("xor2");

	MVAR *avar = (bnf_var->var_type==VTYPE_POINTER) ? bnf_var->var_pointer: bnf_var;
	if(avar->var_type == VTYPE_NUM) {
		bnf_var->dval = (avar->dval>0) ^ val;
		bnf_var->var_type=VTYPE_NUM;
		return;
	};
 };
 set_error(tok,1043,"xor error");
}

inline static void bnf_factor_none()
{
	MESG("bnf_factor_none! [%s]",tok_info(tok));
}

inline static void bnf_factor_dummy()
{
	MESG("bnf_factor_dummy! [%s]",tok_info(tok));
}

// var++,var--
inline static void bnf_update_val()
{
	// MESG("bnf_update_val:");
	MVAR *avar=bnf_var->var_pointer;

	if(avar->var_type==VTYPE_NUM) { 
		bnf_var->dval=avar->dval;
		bnf_var->var_type=VTYPE_NUM;
		
		avar->dval += tok->dval;
		return;
	};
	if(avar->var_type==VTYPE_ARRAY) {
		bnf_var->adat=dup_array_add1(avar->adat,0);
		array_add1(avar->adat,tok->dval);
		bnf_var->var_type=VTYPE_ARRAY;
		return;
	};
	set_error(tok,1026,"cannot update non numeric value!");
	// set for any different type!
}

inline static void bnf_increase_by_pp_num()
{
	double bval=bnf_var->var_pointer->dval;
	prev_var("increase_pp_num2");
	MVAR *aval=bnf_var->var_pointer;
	aval->dval += bval;
}

inline static void bnf_increase_by_pn_num()
{
	double bval=bnf_var->dval;
	prev_var("increase_pp_num2");
	MVAR *aval=bnf_var->var_pointer;
	aval->dval += bval;
}

// aval+=bval
inline static void bnf_increase_by()
{
	// MESG("bnf_factor_increase_by: [%s]",tok_info(tok));
	MVAR *bvar=bnf_var;
	int btype=bvar->var_type;
	prev_var("inc by");

	int atype=bnf_var->var_type;
	MVAR *avar=bnf_var->var_pointer;
	if(bvar->var_type==VTYPE_POINTER) {
		bvar = bvar->var_pointer;
	};
	if(bvar->var_type==VTYPE_NUM) {
		if(avar->var_type==VTYPE_NUM) {
			avar->dval += bvar->dval;
			double val = avar->dval;
			bnf_var->var_type = VTYPE_NUM;
			bnf_var->dval = val;
#if	1
			if(atype==btype && atype==VTYPE_POINTER) {
				set_bnf_function(tok,"increase_by_pp_num",bnf_increase_by_pp_num);
			};
			if(atype==VTYPE_POINTER && btype==VTYPE_NUM) {
				set_bnf_function(tok,"increase_by_pn_num",bnf_increase_by_pn_num);
			};
#endif
			return;
		};
		if(avar->var_type==VTYPE_ARRAY) {
			// copy array
			array_add1(avar->adat,bvar->dval);
			// set the copied array to bnf_var position
			return;
		};
	};
	

	if(bvar->var_type==VTYPE_STRING && avar->var_type==VTYPE_STRING) {
		char *s=(char*)malloc(strlen(avar->sval)+strlen(bvar->sval)+1);
		memcpy(s,avar->sval,strlen(avar->sval));
		memcpy(s+strlen(avar->sval),bvar->sval,strlen(bvar->sval));
		s[strlen(avar->sval)+strlen(bvar->sval)]=0;
		bnf_var->sval=s; bnf_var->var_alloced=0;bnf_var->var_type=VTYPE_STRING;
		if(avar->var_alloced) free(avar->sval);
		avar->sval=s; avar->var_alloced=1;
		return;
	};

	set_error(tok,1024,"increase operation not supported!");
}


// aval-=bval
inline static void bnf_decrease_by()
{
	MESG("bnf_factor_decrease_by:");

	MVAR *bval=bnf_var;
	prev_var("decrease by");
	MVAR *aval=bnf_var->var_pointer;

	if(bval->var_type==VTYPE_POINTER) {
		bval = bval->var_pointer;
	};
	if(bval->var_type==VTYPE_NUM) { 
		if(aval->var_type==VTYPE_NUM) {
			aval->dval -= bval->dval;
			double val = aval->dval;
			bnf_var->var_type = VTYPE_NUM;
			bnf_var->dval = val;
			return;
		};
		if(aval->var_type==VTYPE_ARRAY) {
			// copy array
			array_add1(aval->adat,-bval->dval);
			// set the copied array to bnf_var position
			return;
		};
	};
/*
	if(bval->var_type==VTYPE_STRING) {
		aval->sval = strdup(bval->sval);
		// NTOKEN2;
		return;
	};
*/
	set_error(tok,1026,"decrease operation not supported!");
}

// aval*=bval
inline static void bnf_mul_by()
{
	MESG("bnf_factor_mul_by:");
	MVAR *bval=bnf_var;

	prev_var("mul by");

	MVAR *aval=bnf_var->var_pointer;
	if(bval->var_type==VTYPE_POINTER) {
		bval = bval->var_pointer;
	};
	//aval->var_type = bval->var_type;
	if(bval->var_type==VTYPE_NUM) { 
		if(aval->var_type==VTYPE_NUM) {
			aval->dval *= bval->dval;
			double val = aval->dval;
			bnf_var->var_type = VTYPE_NUM;
			bnf_var->dval = val;
			return;
		};
		if(aval->var_type==VTYPE_ARRAY) {
			// copy array
			array_mul1(aval->adat,bval->dval);
			// set the copied array to bnf_var position
			return;
		};
	};
/*
	if(bval->var_type==VTYPE_STRING) {
		aval->sval = strdup(bval->sval);
		return;
	};
*/
	set_error(tok,1023,"multiply operation not supported!");
}

// aval /= bval
inline static void bnf_div_by()
{
	MESG("bnf_factor_div_by:");
	MVAR *bval=bnf_var;
	prev_var("div by2");

	MVAR *aval=bnf_var->var_pointer;
	if(bval->var_type==VTYPE_POINTER) {
		bval = bval->var_pointer;
	};
	//aval->var_type = bval->var_type;
	if(bval->var_type==VTYPE_NUM) { 
		if(aval->var_type==VTYPE_NUM) {
			aval->dval /= bval->dval;
			return;
		};
		if(aval->var_type==VTYPE_ARRAY) {
			array_mul1(aval->adat,(1/bval->dval));
			return;
		};
	};
	
/*
	if(bval->var_type==VTYPE_STRING) {
		aval->sval = strdup(bval->sval);
		return;
	};
*/
	set_error(tok,1025,"divide operation not supported!");
}

inline static void bnf_factor_sep0()
{
}

inline static void bnf_factor_end()
{
	current_active_flag=0;
}

inline static void bnf_factor_sep1()
{
	prev_var("sep1");
}

inline static void bnf_factor_comma()
{
 // MESG("bnf_factor_comma:");
 if(bnf_var-bnf_vars>1) { 
	prev_var("comma");
	set_bnf_function(tok,"comma->sep1",bnf_factor_sep1);
 } else {
	set_bnf_function(tok,"comma->sep0",bnf_factor_sep0);
};
}

inline static void bnf_factor_eof()
{
	// int ind = bnf_var - bnf_vars;
 	// MESG("bnf_factor_EOF: var index = %d type=%d",ind,bnf_var->var_type);
	current_active_flag=0;
}


inline static void bnf_factor_sep()
{
 if(bnf_var-bnf_vars>1) 
 { 
	 prev_var("sep");
	set_bnf_function(tok,"sep->sep1",bnf_factor_sep1);
 } else {
	set_bnf_function(tok,"sep->sep0",bnf_factor_sep0);

 };
	// initialize and/or check expression stack!
}

inline static void bnf_factor_rcurl0()
{
	// MESG("	rcurl go prev!");
	prev_var("rcurl0");
}

inline static void bnf_factor_rcurl_no()
{
	// MESG("	rcurl no move");
}


inline static void bnf_factor_rcurl()
{
 // long ind=bnf_var-bnf_vars;
 // MESG("	rcurl: ind=%ld",ind);
}

void bnf_factor_error()
{
 	MESG("bnf_factor_error: [%s]",tok_info(tok));
	current_active_flag=0;
}

/* Array definition  */
void bnf_factor_line_array1()
{
	int i=0,j=0;
	int cdim=0;
	int rows=0,cols=0;
	array_dat *adat=tok->tok_adat;
	cdim=1;
	ex_name="Definition";

	MESG("bnf_factor_line_array:%d Array definition ------------",adat->anum);
	// MESG("	tok info: %s array cols=%d rows=%d",tok_info(tok),adat->cols,adat->rows);
	allocate_array(adat);
	NTOKEN2;
	while(cdim>0){
	while(tok->ttype!=TOK_EOF) {
		if(tok->ttype==TOK_SEP) { 
			NTOKEN2;
			continue;
		};
		// MESG("	[%d][%d] tok_type=%d",j,i,tok->ttype);
		bnf_expression();
		// MESG("	factor_line_array: set type=%d",get_vtype());
		if((bnf_var->var_type==VTYPE_STRING) && adat->atype!=VTYPE_AMIXED) adat->atype=VTYPE_SARRAY;
		// MESG("	[%d %d]: value=%f [%s] %d",j,i,value,get_sval(),get_vtype());

		if(tok->ttype == TOK_COMMA) { NTOKEN2;};

		if(adat->atype==VTYPE_ARRAY){
			if(adat->rows>1 && adat->cols>1) {
				adat->dval2[j][i]=bnf_var->dval;
			} else {
				if(adat->cols>1) adat->dval[i]=bnf_var->dval;
				else adat->dval[j]=bnf_var->dval;
			};
		};
		if(adat->atype==VTYPE_SARRAY) {
			int ind1=cols*j+i;
			// MESG("	add row %d col=%d -> %d [%s]",j,i,ind1,get_sval());
			adat->sval[ind1]=strdup(bnf_var->sval);
		};
		if(adat->atype==VTYPE_AMIXED) {
			int ind1=cols*j+i;
			// MESG("factor_line_array: set [%d] ex_vtype=%d",ind1,get_vtype());
			adat->mval[ind1].var_type=bnf_var->var_type;
			if(bnf_var->var_type==VTYPE_NUM) adat->mval[ind1].dval=bnf_var->dval;
			else if(vtype_is(VTYPE_STRING)) adat->mval[ind1].sval=strdup(bnf_var->sval);
		};
		i++;if(i>cols) cols=i;
		if(tok->ttype==TOK_SHOW || tok->ttype==TOK_RBRAKET) {
			cdim=0;break;
		};
		if(tok->ttype==TOK_SEP/* ||tok->ttype==TOK_COMMA */) {
			i=0;
			j++;
			cdim++;if(cdim>rows) rows=cdim;
			NTOKEN2;};
	};cdim--;
	};
	adat->astat=ARRAY_LOCAL;
	// print_array1("",adat);
	// set_bnf_array!
	if(bnf_var->var_type==VTYPE_STRING) free(bnf_var->sval);
	bnf_var->var_type=VTYPE_ARRAY;
	bnf_var->adat=adat;
	next_var("line array");
}

inline static void bnf_factor_assign_var_num()
{
	double bval=bnf_var->dval;
	prev_var("assign_var_num");
	bnf_var->var_pointer->dval=bval;
	// MESG("assign_var numeric %f",bval);
	// bnf_var->dval=bval;
	// bnf_var->var_type=VTYPE_NUM;
}

inline static void bnf_factor_assign_var_nump()
{
	double bval=bnf_var->var_pointer->dval;
	prev_var("assign_var_nump");
	// MESG("assign_var to var %f",bval);
	bnf_var->var_pointer->dval=bval;
	bnf_var->dval=bval;
	bnf_var->var_type=VTYPE_NUM;
}

void bnf_factor_assign_var()
{
	MVAR *bvar=bnf_var;
	// show_result();
	int btype=bvar->var_type;
	prev_var("assign var");
	MESG("bnf_factor_assign_var: atype=%d btype %d",bnf_var->var_type,btype);

	MVAR *aval=bnf_var->var_pointer;

	if(btype==VTYPE_POINTER) {
		// MESG("bvar is pointer");
		bvar = bvar->var_pointer;
	};

	aval->var_type = bvar->var_type;

	if(bvar->var_type==VTYPE_NUM) { 
		aval->dval = bvar->dval;
		bnf_var->var_type = aval->var_type;
		bnf_var->dval = aval->dval;
		// long ind=bnf_var-bnf_vars;
		// MESG("	assign %p ind=%2ld set var to %f tok [%s]",aval,ind,aval->dval,tok_info(tok));
		if(btype==VTYPE_POINTER) {
			// tok->bnf_factor_function=bnf_factor_assign_var_nump;
			// MESG("set factor_function to assign_var_nump [%s]",tok_info(tok));
			set_bnf_function(tok,"assign_var_nump",bnf_factor_assign_var_nump);
		};
		if(btype==VTYPE_NUM) {
			// tok->bnf_factor_function=bnf_factor_assign_var_num;
			// MESG("set factor_function to assign_var_num [%s]",tok_info(tok));
			set_bnf_function(tok,"assign_var_num",bnf_factor_assign_var_num);
		};
		return;
	};
	if(bvar->var_type==VTYPE_STRING) {
		if(aval->var_type==VTYPE_STRING) {
			if(aval->var_alloced) free(aval->sval);
		} else aval->var_type=VTYPE_STRING;
		aval->sval = strdup(bvar->sval);
		// bnf_var->sval = aval->sval;
		return;
	};
	
	// set for any different type!
#if	1
	MESG("	-- assign_var: copy data");
	memcpy(aval,bvar,sizeof(MVAR));
#else
	// MESG("	set array");
	aval->var_type=bvar->var_type;
	aval->adat = bvar->adat;
#endif
	bnf_var->var_alloced=0;
}

void set_bnf_string(char *s)
{
	bnf_var->sval=strdup(s);
	bnf_var->var_type=VTYPE_STRING;
	bnf_var->var_alloced=1;
}

void set_bnf_num(double v)
{
	bnf_var->dval=v;
	bnf_var->var_type=VTYPE_NUM;
}

void bnf_factor_env()
{
	BTNODE *bte;
	// MESG("bnf_factor_env");
	next_var("env");

	bte=tok->tok_node;
	// var_node=bte;
	// MESG("factor_env: set var_node [%s]",tok_info(tok));
	// MESG("	env node name [%s] vtype=%d index=%d",bte->node_name,bte->node_vtype,bte->node_index);
#if	1
	get_env(bte->node_index);
#else
	if(bte->node_vtype==VTYPE_STRING) {
		bnf_var->sval = strdup(bte->node_sval);
		bnf_var->var_alloced=1;
		bnf_var->var_type=VTYPE_STRING;
	} else {
		bnf_var->dval = get_env(bte->node_index);
		bnf_var->var_type=VTYPE_NUM;
	};
#endif
}

void bnf_assign_env()
{
	MESG("bnf_assign_env:");
	int left_index;
	// MESG("assign_env:");
	left_index=var_node->node_index;
	// MESG("assign_env: left_index=%d",left_index);
	MVAR *var=bnf_var;
	prev_var("assign_env");
	if(var->var_type==VTYPE_POINTER) var=bnf_var->var_pointer;
	if(var->var_type==VTYPE_NUM) set_env(left_index,"",var->dval);
	else if(var->var_type==VTYPE_STRING) set_env(left_index,var->sval,0);
	else MESG("bnf_assign_env type=%d not correct!",var->var_type);
	var_node=NULL;
}

void bnf_assign_opt()
{
	MESG("bnf_assign_opt:");
	MVAR *var=bnf_var;
	prev_var("assign var");

	// MESG("assign_option:");
	if(var_node->node_vtype==VTYPE_STRING) { free(var_node->node_sval);};

	if(var->var_type==VTYPE_POINTER) var=bnf_var->var_pointer;
	if(var->var_type==VTYPE_STRING){
		var_node->node_vtype=VTYPE_STRING;
		var_node->node_sval=strdup(var->sval);
	} else {
		// CHECK !!!!!!!!
		
		var_node->node_vtype=VTYPE_NUM;
		var_node->node_dval=var->dval;
	};
}

void bnf_factor_option()
{
 BTNODE *bte; 
 MESG("bnf_factor_option:");
 next_var("option");
/* variable's name in tok0->tname */
	bte=tok->tok_node;
	// MESG("factor_option: set var_node [%s]",tok_info(tok));
	var_node=bte;

	set_vtype(bte->node_vtype);

	if(bte->node_vtype==VTYPE_STRING) { /* there is a valid string value */
		// clean_saved_string(strlen(bte->node_sval));
		bnf_var->sval=strdup(bte->node_sval);
		bnf_var->var_type=VTYPE_STRING;
		bnf_var->var_alloced=1;
	} else {
		bnf_var->dval=bte->node_dval;
		bnf_var->var_type=VTYPE_NUM;
		// return(bte->node_dval);
	};
}

// dummy one!
void bnf_factor_lpar()
{
	MESG("bnf_factor_lpar: bnf_group=%d",tok->bnf_group);
}

void bnf_factor_rpar()
{
	MESG("bnf_factor_rpar: bnf_group=%d",tok->bnf_group);
}

static void bnf_block1()
{
	// MESG("-------- bnf_block1 start![%s] { [%s]",fp->b_fname,tok_info(tok));
	// show_token_table("block  ",fp,fp->tok_table_bnf,fp->tok_bnf_index);
	// MESG("-- block1: start at [%s]",tok_info(tok));
	do {
		// tok_struct *tok0=tok;
	 	tok->bnf_factor_function();
		NTOKEN2;
		if(!current_active_flag) {
			// MESG("bnf_block1:[%s] stop: ind=%d type=%d [%s]",fp->b_fname,(int)(bnf_var-bnf_vars),bnf_var->var_type,tok_info(tok));
			return;
		};
	} while(tok->tgroup!=TOK_END);
	tok->bnf_factor_function();

	// MESG("-- block end  ! } [%s]",tok_info(tok));
}

void bnf_block1_break(/*FILEBUF *fp*/)
{
#if	0
 exe_buffer=fp;
   // MESG("exec_block1:[%s] err_num= %d %d tok=[%s]",fp->b_fname,err_num,current_active_flag,tok_info(tok));
   if(!current_active_flag) {
		tok=fp->end_token;
		// return(ex_var.dval);
   };
#endif
	// MESG("-- block start! { [%s]",tok_info(tok));
	do {
		// MESG("	- [%s]",tok_info(tok));
	 	tok->bnf_factor_function();
		if(drv_check_break_key()) break;
	} while(tok->tgroup!=TOK_END);
	tok->bnf_factor_function();
	// MESG("-- block end  ! } [%s] %d",tok_info(tok),ind);
}

inline static void bnf_dir_lcurl()
{
	// MESG("bnf_dir_lcurl:<<<");
	NTOKEN2;
	bnf_block1();
	// MESG("end block! [%s]>>>",tok_info(tok));
}

inline static void bnf_dir_lcurl_break()
{
	NTOKEN2;
	bnf_block1_break();
}

void bnf_dir_break()
{
	MESG("bnf_dir_break:");
	// NTOKEN2;
	current_active_flag=0;
}

void bnf_dir_fori()
{
	// MESG("bnf_dir_fori: var_stack_index=%d",bnf_var-bnf_vars);
	tok_struct *start_block;	// element at block start
	tok_struct *end_block=NULL;	/* at the block end  */
	int old_active_flag=current_active_flag;
	MVAR *index=NULL;
	double dinit,dmax,dstep;
	double *iterrator_val;

	// long ind=bnf_var-bnf_vars;
	// MESG("	fori: ---  ind=%ld tok=[%s]",ind,tok_info(tok));
	NTOKEN2;	/* go to next token after for */

	index=&current_stable[tok->tind];

	// MESG("	fori: tok [%s]",tok_info(tok));
	// MESG("	fori: after index expression tok=%s",tok_info(tok));
	dinit = bnf_expression();
	prev_var("fori index");

	iterrator_val=&index->dval;
	*iterrator_val=dinit;

	// check if ok!
	// if(dinit != index->dval) { MESG("	fori: ERROR1 IN FORI index! %f %f",dinit ,&index->dval);};
	// MESG("	fori init iterrator to %f",dinit);
	NTOKEN2;	/* skip separator! */

	dmax = bnf_expression();
	prev_var("	get dmax val");

	// MESG("	fori max=%f",dmax);
	NTOKEN2;

	dstep = bnf_expression();
	prev_var("	get step val");

	NTOKEN2;	/* skip right parenthesis  */
	// ind=bnf_var-bnf_vars;
	// MESG("	fori: before block execute ind=%ld tok=[%s]",ind,tok_info(tok));
	// set block start
	start_block=tok;	/* this is a block start or a simple sentence  */
	start_block++;	
	// MESG("	fori start of block token [%s]",tok_info(start_block));
	// find token after the end of block
	skip_sentence1();
	end_block=tok;
	// MESG("	fori end of block token [%s]",tok_info(tok));

	if(dinit==dmax) {
		tok=end_block;
		current_active_flag=old_active_flag;
		return;
	};
	if(dstep>0 && dmax > *iterrator_val) {

		for(;*iterrator_val < dmax; *iterrator_val +=dstep) {
			tok=start_block;
			// MESG("		fori: iterrator_val=%3f [%s]",*iterrator_val,tok_info(tok));
			bnf_block1();
			if(current_active_flag==0) {
				// MESG("end loop!:");
				if(is_break1) { tok=exe_buffer->end_token;return;};
				break;
			};
		};
	} else if(dstep<0 && dmax< *iterrator_val) {
		for(; *iterrator_val > dmax; *iterrator_val +=dstep) {
			tok=start_block;
			bnf_block1();
			if(current_active_flag==0) {
				if(is_break1) { tok=exe_buffer->end_token;return;};
				break;
			};
		};
	} else {
		err_num=226;
		ERROR("error: infinite fori loop %d",err_num);
	};
	tok=end_block;
	// ind=bnf_var-bnf_vars;
	// MESG("	fori: at end of block ind=%ld tok=[%s]",ind,tok_info(tok));

	current_active_flag=old_active_flag;
}

void bnf_dir_for()
{
// to be defined
	MESG("bnf_dir_for TBD");
}

void bnf_dir_while()
{
// to be defined
	MESG("bnf_dir_while TBD");
}

inline static double bnf_expression()
{
	// MESG("	bnf_expression: ------ tok ind=%3d ttype=%d tgroup=%d bnf_group=%d",tok->tind,tok->ttype,tok->tgroup,tok->bnf_group);
	while(tok->bnf_group>0) {
		// MESG("	bnf_group=%d [%s]",tok->bnf_group,tok_info(tok));
		tok->bnf_factor_function();	
		NTOKEN2;
	};
#if	1
	if(bnf_var->var_type==VTYPE_POINTER) {
		memmove(bnf_var,bnf_var->var_pointer,sizeof(struct MVAR));
		if(bnf_var->var_type==VTYPE_STRING) bnf_var->var_alloced=0;
	};
	if(bnf_var->var_type==VTYPE_NUM) return bnf_var->dval;
	else return 0;
#endif
}

void bnf_dir_return()
{
	// MESG("bnf_dir_return: at [%s]",tok_info(tok));
	NTOKEN2;
	// MESG("	return : start at [%s]",tok_info(tok));
	if(tok->ttype!=TOK_SEP && tok->ttype!=TOK_RPAR) 
	{ 
		bnf_expression();
		// show_result();
	};
	// MESG("	return : end at [%s]",tok_info(tok));
	current_active_flag=0;	/* skip rest of function  */
}

void bnf_dir_type()
{
	MESG("bnf_dir_type TBD");
}

inline static MVAR * push_args_bnf(int nargs,int vars_num)
{
 // MESG("push_args_bnf: nargs=%d vars_num=%d",nargs,vars_num);

//  MVAR *va = new_symbol_table(nargs+vars_num);
 MVAR *va = new_symbol_table(vars_num);
 if(va==NULL) return NULL;

 MVAR *va_i=va;

 int i=0;
 for(;i<nargs;i++,va_i++)
 {
	// MESG("	push_args_1: arg %d, tok=[%d %s] type=%d",(int)(va_i-va),tok->tnum,tok->tname,va_i->var_type);
	// MESG("	push_args_1: arg %d, tok=[%d %s] value=%f type=%d",i,tok->tnum,tok->tname,value,va_i->var_type);
	// MESG(";		i=%d, var_type = %d",i,va_i->var_type);
	NTOKEN2;	/* skip proc name of separator  */
	bnf_expression();

	memmove(va_i,bnf_var,sizeof(MVAR));

	// MESG("	push_args_1: arg %d, tok=[%d %s] value=%f type=%d",i,tok->tnum,tok->tname,value,va_i->var_type);
	// NTOKEN2; /* skip separator or right parenthesis! */
 };
#if	0	/* no need to initialize ??  */
  // while(i<vars_num+nargs)
  for(;i<vars_num;i++,va_i++)
  // while(va_i<va+vars_num+nargs) 
  {
 	va_i->var_type=VTYPE_NUM;
	va_i->dval=0;
	// va_i++; i++;
  };
#endif

 // MESG(">	push_args_1:end [%s]",tok_info(tok));
 return(va);
}


inline static void bnf_exec_function(FILEBUF *proc_buffer,int nargs)
{
	MVAR *old_symbol_table=current_stable;
#if	0
	if(proc_buffer==NULL) MESG("	buffer is NULL!");
	else MESG("## bnf_exec_function:[%s] args=%d",proc_buffer->b_fname,nargs);
#endif
	current_stable = push_args_bnf(nargs,proc_buffer->symbol_tree->items);
	tok_struct *after_proc=tok; 

	tok=proc_buffer->tok_table_bnf;	/* start of function  */
	// MESG("bnf_exec_function:start [%s]",tok_info(tok));

	skip_args1(nargs);

	NTOKEN2;
	bnf_block1();
	// show_result();
	delete_symbol_table(current_stable,proc_buffer->symbol_tree->items,nargs);
	current_stable=old_symbol_table;
		// exe_buffer=ori_buf;
	tok=after_proc;
	// MESG("	continue after function to [%s]",tok_info(tok));
}

void bnf_factor_proc()
{
	tok_struct *tok0=tok;
	FILEBUF *cbuf=exe_buffer;
	// MESG("bnf_factor_proc:[%s] << ind=%d",tok0->tname,(int)(bnf_var-bnf_vars));
	next_var("proc");		/* to save proc result  */
	MVAR *result_var=bnf_var;
	// MESG("	tname [%s]",tok0->tname);
	// MESG("bnf_factor_proc: current_buffer [%s]",cbuf->b_fname);
	// MESG("	token buffer [%s]",tok0->proc_buffer->b_fname);
	exe_buffer=tok0->proc_buffer;
	// MESG("	exe_buffer [%s]",exe_buffer->b_fname);
#if	TPROFILE
	tok0->proc_buffer->function_called++;
#endif
	// MESG("factor_proc: cbuf=%s ---------------- args=%d",cbuf,tok0->t_nargs);
	// NTOKEN2;	/* skip proc name */
	// MESG("factor_proc: tok0 [%d %s] args=%d",tok0->tnum,tok0->tname,tok0->tind);
	// MESG("factor_proc: tok  [%d %s] %d ",tok->tnum,tok->tname,tok->tind);

	bnf_exec_function(tok0->proc_buffer,tok0->t_nargs);

	memmove(result_var,bnf_var,sizeof(MVAR));
#if	TPROFILE
	var_index -= (int)(bnf_var-result_var);
#endif
#if	0
//	clean used strings
	MVAR *mv_i;
	for(mv_i=result_var+1;mv_i<=bnf_var;mv_i++){
		if(mv_i->var_type==VTYPE_STRING) {
			if(mv_i->var_alloced) {
				free(mv_i->sval);mv_i->var_alloced=0;
			};
		};
	};
#endif
	bnf_var = result_var;
	current_active_flag=1;	/* start checking again  */

	exe_buffer=cbuf;
}

void bnf_dir_if()
{
	tok_struct *tok0=tok;
	// MESG("tok_dir_if: n=%d",tok->tnum);
	NTOKEN2;	/* go to next token after if */

	int ival = bnf_expression();
	prev_var("if result");
	// MESG("tok_dir_if: res=%d after expression [%s]",ival,tok_info(tok));
	if(ival) {
		// MESG("	true: start of [%s]",tok_info(tok));
		NTOKEN2;
		// MESG("	true:2 start of [%s]",tok_info(tok));
		if(tok->ttype==TOK_LCURL) { NTOKEN2;bnf_block1();NTOKEN2;}
		else { bnf_expression();
			// NTOKEN2;
		};
		// NTOKEN2;
		// MESG("	true:3 after if execution! %s",tok_info(tok));
		if(tok->ttype==TOK_DIR_ELSE) {
			tok=tok->next_tok;
			// tok--;
			// MESG("skip else up to %s",tok_info(tok));
		};
		// MESG("## tok_dir_if: true: end [%s]",tok_info(tok));
		return;
	} else {
		tok=tok0->next_tok;
		// MESG("	false: start of [%s]",tok_info(tok));
		if(check_skip_token1(TOK_DIR_ELSE)) {
			// MESG("	execute else at [%s]",tok_info(tok));
			// NTOKEN2;
			if(tok->ttype==TOK_LCURL) { 
				NTOKEN2;
				bnf_block1();
				// MESG("## tok_dir_if: else: end block [%s]",tok_info(tok));
			} else {
				bnf_expression();
				// MESG("## tok_dir_if: else: end expression [%s]",tok_info(tok));
			};
		};
		// MESG("## tok_dir_if: else: end [%s]",tok_info(tok));
	}
}

void bnf_dir_else()
{
	MESG("bnf_dir_else:");
	NTOKEN2;
}


void show_var_stats()
{
#if	TPROFILE
	fprintf(stderr,"MVAR index=%ld max=%ld\n",var_index,max_var);
#endif
}

void bnf_factor_line_array()
{
	int i=0,j=0;
	int cdim=0;
	int rows=0,cols=0;
	MESG("bnf_factor_line_array:");
	array_dat *adat=tok->tok_adat;
	cdim=1;
	next_var("line_array");
	ex_name="Definition";
	// MESG("bnf_factor_line_array:%d Array definition ------------",adat->anum);
	// MESG("	tok info: %s array cols=%d rows=%d",tok_info(tok),adat->cols,adat->rows);
	allocate_array(adat);
	NTOKEN2;
	while(cdim>0){
	while(tok->ttype!=TOK_EOF) {
		if(tok->ttype==TOK_SEP) { 
			NTOKEN2;
			continue;
		};
		// MESG("	[%d][%d] tok_type=%d",j,i,tok->ttype);
		double value=bnf_expression();
		// MESG("	factor_line_array: set type=%d",get_vtype());
		if(bnf_var->var_type==VTYPE_STRING && adat->atype!=VTYPE_AMIXED) adat->atype=VTYPE_SARRAY;
		// MESG("	[%d %d]: value=%f [%s] %d",j,i,value,get_sval(),get_vtype());

		if(tok->ttype == TOK_COMMA) { NTOKEN2;};

		if(adat->atype==VTYPE_ARRAY){
			if(adat->rows>1 && adat->cols>1) {
				adat->dval2[j][i]=value;
			} else {
				if(adat->cols>1) adat->dval[i]=value;
				else adat->dval[j]=value;
			};
		};
		// MESG("	ok! [%s]",tok_info(tok)s);		
		if(adat->atype==VTYPE_SARRAY) {
			int ind1=cols*j+i;
			// MESG("	add row %d col=%d -> %d [%s]",j,i,ind1,get_sval());
			adat->sval[ind1]=strdup(get_sval());
		};
		if(adat->atype==VTYPE_AMIXED) {
			int ind1=cols*j+i;
			// MESG("factor_line_array: set [%d] ex_vtype=%d",ind1,get_vtype());
			adat->mval[ind1].var_type=get_vtype();
			if(vtype_is(VTYPE_NUM)) adat->mval[ind1].dval=value;
			else if(vtype_is(VTYPE_STRING)) adat->mval[ind1].sval=strdup(get_sval());
		};
		i++;if(i>cols) cols=i;
		if(tok->ttype==TOK_SHOW || tok->ttype==TOK_RBRAKET) {
			cdim=0;break;
		};
		if(tok->ttype==TOK_SEP/* ||tok->ttype==TOK_COMMA */) {
			i=0;
			j++;
			cdim++;if(cdim>rows) rows=cdim;
			NTOKEN2;};
		prev_var("array next item");
	};cdim--;
		prev_var("array end");
	};
	// MESG("bnf_factor_line_array: end");
	adat->astat=ARRAY_LOCAL;
	// print_array1("end of array definition",adat);
	bnf_var->adat=adat;
	bnf_var->var_type=adat->atype;
	MESG("-> array definition: @%d  end [%s]",(int)(bnf_var-bnf_vars),tok_info(tok));
	// set_array(adat);
	// NTOKEN2;
}

void bnf_assign_array1()
{
	int ind1;
	double *dval=NULL;
	double value=0;
	next_var("array1");
	MVAR *array_slot;
	array_slot=&current_stable[tok->tind];
	array_dat *adat = array_slot->adat;
	lstoken=tok;
	MESG("assign_array1:----------- vtype=%d",array_slot->var_type);
	NTOKEN2;
	ind1 = (int)bnf_expression();
	MESG("	array index: %d [%s]",ind1,tok_info(tok));
	if(adat==NULL) {	/* this happens if array is not defined yet!!!  */
		MESG("array adat is NULL allocate new one %d x 1 !!!!!!!!!!!!",ind1);
		ex_nums=1;
		adat=new_array(ind1+1,1);
		array_slot->adat=adat;
		array_slot->var_type=VTYPE_ARRAY;
		allocate_array(array_slot->adat);	/*   */
	};

	if(tok->ttype==TOK_RBRAKET) { 
		// NTOKEN2;
		MESG("ends with rbracket!!");
	};
	MESG("assign_array1:ind=%d ind1=%d type=%d",array_slot->var_index,ind1,array_slot->var_type);

		MESG("	2 vtype=%d %d",array_slot->var_type,VTYPE_ARRAY);
		if(array_slot->var_type==VTYPE_ARRAY) {
		if(array_slot->adat->rows<ind1 && array_slot->adat->cols<ind1) {
			double *dval_old = array_slot->adat->dval;
			MESG("+++ reallocate ind1=%d x %d %X",ind1,sizeof(double),dval_old);
			if(array_slot->adat->cols > array_slot->adat->rows) 
				array_slot->adat->cols=ind1;
			else
				array_slot->adat->rows=ind1;
			double *dval_new = (double *)realloc((void *)(dval_old),(ind1+1)*sizeof(double));
			if(dval_new==NULL) {
				err_num=214;
				err_line=tok->tline;
				// ERROR("	array cannot allocate dval at %d",err_line);
				set_break();
				return;
			};
			array_slot->adat->dval = dval_new; 
			MESG("	array reallocated:%X",array_slot->adat->dval);
		} else { 
			dval = array_slot->adat->dval;
	
			value=dval[ind1];
			MESG("	pdval value [%d]=%f",ind1,value);
			ls_pdval=&dval[ind1];
			bnf_var->var_index=ind1;
			bnf_var->adat = array_slot->adat;
			bnf_var->var_type=VTYPE_ARRAYEL1;

			MESG("	array_slot %d: ",array_slot->var_index);
			// set_vtype(VTYPE_NUM);
		};
		};

		if(array_slot->var_type==VTYPE_AMIXED) {
			set_vtype(array_slot->adat->mval[ind1].var_type);
			lmvar = &array_slot->adat->mval[ind1];
			// MESG("get indexed value from mixed array ind1=%d type=%d",ind1,get_vtype());
			if(vtype_is(VTYPE_NUM)) {
				value=array_slot->adat->mval[ind1].dval;
				// MESG("	value=%f",value);
				ls_pdval=&array_slot->adat->mval[ind1].dval;
			} else {
				value=0;
				set_sval(array_slot->adat->mval[ind1].sval);

				// MESG("	show string value![%s]",saved_string);
				ls_psval=&array_slot->adat->mval[ind1].sval;
			};
			
		}
		if(array_slot->var_type==VTYPE_SARRAY) {
			char **sval = array_slot->adat->sval;
			clean_saved_string(strlen(array_slot->adat->sval[ind1]));	/* Check!! TODO  */
			strcpy(saved_string,array_slot->adat->sval[ind1]);
			// MESG("	show string value![%s]",saved_string);
			ls_psval=&sval[ind1];
			value=0;
			set_vtype(VTYPE_STRING);
		};

	lsslot=array_slot;
	MESG("        : >>>> end");
	// MESG("	factor_array1:ind1=%d lsslot ind=%d type=%d rows=%d cols=%d [%s]!",ind1,lsslot->var_index,lsslot->var_type,lsslot->adat->rows,lsslot->adat->cols,array_slot->psval[0]);
	// return(value);
}

void bnf_assign_array2()
{
	MESG("bnf_assign_array2");
}

void bnf_factor_array1()
{
	int ind1;
	double *dval=NULL;
	double value=0;
	next_var("array1");
	MVAR *array_slot;
	array_slot=&current_stable[tok->tind];
	array_dat *adat = array_slot->adat;
	lstoken=tok;
	MESG("factor_array1:----------- vtype=%d",array_slot->var_type);
	NTOKEN2;
	ind1 = (int)bnf_expression();
	MESG("	array index: %d [%s]",ind1,tok_info(tok));
	if(adat==NULL) {	/* this happens if array is not defined yet!!!  */
		MESG("array adat is NULL allocate new one %d x 1 !!!!!!!!!!!!",ind1);
		ex_nums=1;
		adat=new_array(ind1+1,1);
		array_slot->adat=adat;
		array_slot->var_type=VTYPE_ARRAY;
		allocate_array(array_slot->adat);	/*   */
	};

	if(tok->ttype==TOK_RBRAKET) { 
		// NTOKEN2;
		MESG("ends with rbracket!!");
	};
	MESG("factor_array1:ind=%d ind1=%d type=%d",array_slot->var_index,ind1,array_slot->var_type);

		MESG("	2 vtype=%d %d",array_slot->var_type,VTYPE_ARRAY);
		if(array_slot->var_type==VTYPE_ARRAY) {
		if(array_slot->adat->rows<ind1 && array_slot->adat->cols<ind1) {
			double *dval_old = array_slot->adat->dval;
			MESG("+++ reallocate ind1=%d x %d %X",ind1,sizeof(double),dval_old);
			if(array_slot->adat->cols > array_slot->adat->rows) 
				array_slot->adat->cols=ind1;
			else
				array_slot->adat->rows=ind1;
			double *dval_new = (double *)realloc((void *)(dval_old),(ind1+1)*sizeof(double));
			if(dval_new==NULL) {
				err_num=214;
				err_line=tok->tline;
				// ERROR("	array cannot allocate dval at %d",err_line);
				set_break();
				return;
			};
			array_slot->adat->dval = dval_new; 
			MESG("	array reallocated:%X",array_slot->adat->dval);
		} else { 
			dval = array_slot->adat->dval;
	
			value=dval[ind1];
			MESG("	pdval value [%d]=%f",ind1,value);
			ls_pdval=&dval[ind1];
			// bnf_var->var_index=tok->tind;
			bnf_var->dval = value;
			bnf_var->var_type=VTYPE_NUM;
			MESG("	array_slot %d: ",array_slot->var_index);
			// set_vtype(VTYPE_NUM);
		};
		};

		if(array_slot->var_type==VTYPE_AMIXED) {
			set_vtype(array_slot->adat->mval[ind1].var_type);
			lmvar = &array_slot->adat->mval[ind1];
			// MESG("get indexed value from mixed array ind1=%d type=%d",ind1,get_vtype());
			if(vtype_is(VTYPE_NUM)) {
				value=array_slot->adat->mval[ind1].dval;
				// MESG("	value=%f",value);
				ls_pdval=&array_slot->adat->mval[ind1].dval;
			} else {
				value=0;
				set_sval(array_slot->adat->mval[ind1].sval);

				// MESG("	show string value![%s]",saved_string);
				ls_psval=&array_slot->adat->mval[ind1].sval;
			};
			
		}
		if(array_slot->var_type==VTYPE_SARRAY) {
			char **sval = array_slot->adat->sval;
			clean_saved_string(strlen(array_slot->adat->sval[ind1]));	/* Check!! TODO  */
			strcpy(saved_string,array_slot->adat->sval[ind1]);
			// MESG("	show string value![%s]",saved_string);
			ls_psval=&sval[ind1];
			value=0;
			set_vtype(VTYPE_STRING);
		};

	lsslot=array_slot;
	MESG("        : >>>> end");
	// MESG("	factor_array1:ind1=%d lsslot ind=%d type=%d rows=%d cols=%d [%s]!",ind1,lsslot->var_index,lsslot->var_type,lsslot->adat->rows,lsslot->adat->cols,array_slot->psval[0]);
	// return(value);
}

void bnf_factor_array2()
{
	int ind1=0;
	int ind2=0;
	double value=0;
	next_var("array2");
	MVAR *array_slot;
	array_dat *adat;
	tok_struct *tok0 = tok;
	// MESG("factor_array2: %s",tok->tname);
	array_slot=get_left_slot(tok->tind);
	adat=array_slot->adat;
	// lstoken=tok;
	// MESG("factor_array2: type %d",adat->atype);
	
	if(adat==NULL) {
#if	1
		set_error(tok,209,"array indexes out of bound!");
		return;
#else
		/* No data in the array, allocate new ones!  */
		NTOKEN2;
		ind1=(int)bnf_expression();
		NTOKEN2;
		ind2=(int)bnf_expression();
		NTOKEN2;
		adat = new_array(ind1+1,ind2+1);

		array_slot->adat = adat;
		tok0->adat = adat;
		print_array1("new array created",adat);
		dval2 = adat->dval2;

		// MESG("new rows=%d cols=%d",adat->rows,adat->cols);
			// value=dval2[ind1][ind2];
#endif
	} else {
		NTOKEN2;
		ind1=(int)bnf_expression();
		NTOKEN2;prev_var("ind1");
		ind2=(int)bnf_expression();
		// NTOKEN2;
		prev_var("ind2");
		MESG("	ind1=%d ind2=%d",ind1,ind2);
		if((ind1 >= adat->rows) || (ind2 >= adat->cols)) {
			set_error(tok0,210,"array indexes out of bound!");
		} else {
			if(adat->atype==VTYPE_ARRAY) {
				// MESG("	array type!");
				double **dval2 = adat->dval2;
				value=dval2[ind1][ind2];

				ls_pdval=&dval2[ind1][ind2];
				bnf_var->dval=value;
				bnf_var->var_type=VTYPE_NUM;
				MESG("		value %f",value);
				return;
			} else if(adat->atype==VTYPE_SARRAY) {
				char **sval = adat->sval;
				int ind=adat->cols*ind1+ind2;
				clean_saved_string(strlen(adat->sval[ind]));	/* Check!! TODO  */
				strcpy(saved_string,adat->sval[ind]);
				// MESG("	show string value![%s]",saved_string);
				ls_psval=&sval[ind];
				value=0;
				set_vtype(VTYPE_STRING);
			} else if(adat->atype==VTYPE_AMIXED) {
				// MESG("factor_array2: AMIXED v1=%d v2=%d",ind1,ind2);
				int ind=adat->cols*ind1+ind2;
				set_vtype(adat->mval[ind].var_type);
				lmvar = &adat->mval[ind];
				if(adat->mval[ind].var_type==VTYPE_NUM) {
					value=array_slot->adat->mval[ind].dval;
					// MESG("	num: val=%f",value);
					ls_pdval=&array_slot->adat->mval[ind].dval;
					return;
				} else {
					// MESG("	string: at %d",ind);
					// MESG("	string: val=[%s]",adat->mval[ind].sval);
					set_sval(adat->mval[ind].sval);
					// MESG("	string: val=[%s]",adat->mval[ind].sval);
					// value=0;
					// return(0);
				};
			};
		};
	};
	// MESG("end factor_array2: lsslot=%X",(void *)lsslot);
}

void bnf_factor_array_l1()
{
	next_var("array_l1");
	int ind1;
	double value=0;

	MVAR *array_slot;
	array_slot=&current_stable[tok->tind];
	array_dat *adat = array_slot->adat;
	lstoken=tok;
	NTOKEN2;
	// MESG("factor_array_l1: ind=%d num=%d --------> %s . %s",tok->tind,tok->tnum,lstoken->tname,tok->tname);

	if(adat->var_tree) {
		BTNODE *el_node = find_btnode(adat->var_tree,tok->tname);
		if(el_node) {
			ind1 = (int)(el_node->node_index);
			tok->dval=ind1;
		} else {
			MESG("error element not found!");
			set_error(tok,504,"element not found!");
			return;
		};
	} else {
		MESG("No var_tree!");
		set_error(lstoken,506,"No var_tree");
		return;
	};

	// MESG("	ind = %d",ind1);
	lmvar=&adat->mval[ind1];
	if(adat->mval[ind1].var_type==VTYPE_NUM) {
		value=adat->mval[ind1].dval;
		ls_pdval = &adat->mval[ind1].dval;
		set_vdval(value);
		// MESG("	return type dval %f",value);
	} else {
		set_vtype(VTYPE_STRING);
		set_sval(adat->mval[ind1].sval);
		ls_psval = &adat->mval[ind1].sval;
		// MESG("	return type sval [%s]",adat->mval[ind1].sval);
	};
	NTOKEN2;
	lsslot=array_slot;
	// MESG("        : >>>> end");
	// MESG("	factor_array_l1:ind1=%d lsslot ind=%d type=%d rows=%d cols=%d [%s]!",ind1,lsslot->var_index,lsslot->var_type,lsslot->adat->rows,lsslot->adat->cols,array_slot->psval[0]);
	// return(value);
}

void bnf_factor_array_l2()
{
	int ind1;
	double value=0;
	MVAR *array_slot;
	// MESG("factor_array_l2: [%s]",tok_info(tok));
	// MESG("		var index=%d",tok->tind);
	array_slot=&current_stable[tok->tind];
	// MESG("	---1 array_slot %lX",(long)array_slot);
	array_dat *adat = array_slot->adat;
	// MESG("	---1 adat %lX",(long)adat);
	if(adat==NULL) { set_error(tok,505,"var not defined as array");return;};
	// MESG("	adat num=%d",adat->anum);
	// if(adat->var_tree) MESG("	there is a var_tree!");
	lstoken=tok;
	NTOKEN2;
	value=cexpression();
	// MESG("	index1=%f",value);
	NTOKEN2;	/* skip rbraket  */
	// MESG("factor_array_l2:ind=%d tnum %d --------> %s . %s",tok->tind,tok->tnum,lstoken->tname,tok->tname);
	// MESG("	---2 %lX",(long)adat);
	// if(adat==NULL) MESG("	no adat!");
	if(adat->var_tree) {
		// MESG("	found var_tree");
		BTNODE *el_node = find_btnode(adat->var_tree,tok->tname);
		if(el_node) {
			// MESG("	element found!");
			ind1 = (int)(el_node->node_index);
			tok->dval=ind1;
		} else {
			// MESG("	error element not found!");
			set_error(tok,507,"element not found!");
			return;
		};
	} else {
		// MESG("No var_tree!");
		set_error(lstoken,503,"No var_tree");
		return;
	};

	// MESG("	ind = %d",ind1);
	lmvar=&adat->mval[ind1];
	if(adat->mval[ind1].var_type==VTYPE_NUM) {
		value=adat->mval[ind1].dval;
		ls_pdval = &adat->mval[ind1].dval;
		set_vdval(value);
		// MESG("	return type dval %f",value);
	} else {
		set_vtype(VTYPE_STRING);
		set_sval(adat->mval[ind1].sval);
		ls_psval = &adat->mval[ind1].sval;
		// MESG("	return type sval [%s]",adat->mval[ind1].sval);
	};
	NTOKEN2;
	lsslot=array_slot;
	// MESG("        : >>>> end");
	// MESG("	factor_array_l2:ind1=%d lsslot ind=%d type=%d rows=%d cols=%d!",ind1,lsslot->var_index,lsslot->var_type,lsslot->adat->rows,lsslot->adat->cols);
	// return(value);
}
