
char *ddot_string();
void update_ddot_line(char *ddot_out);
void skip_sentence1();

tok_struct* bnf_stack[100];
tok_struct *bnf_start, *bnf_var1, *bnf_var2, *bnf_op;

int	ind_var1=0;
int ind_var2=0;
int ind_op=0;
int ind_var=0;	// current index
double value=0;

MVAR bnf_vars[100];
MVAR *bnf_var=&bnf_vars[0];

inline MVAR *get_left_slot(int ind);
void bnf_expression();

#define NTOKEN22	ntoken2()

void bnf_refresh_ddot()
{
 MESG("refresh_ddot");
 int stat=0;
 TextPoint *tp = tok->ddot;
 FILEBUF *buf = tp->fp;
 // MESG("refresh_ddot: %d",get_vtype());

	bnf_var--;
	MVAR *var_show = bnf_var;
	int var_index = var_show-bnf_vars;
	int ori_type=var_show->var_type;
	if(bnf_var->var_type==VTYPE_POINTER) {
		var_show = bnf_var->var_pointer;
 	};

 MESG("refresh_ddot: ind=%3ld ori=%d type=%d",var_index,ori_type,var_show->var_type);
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
	 // lstoken=NULL;
	 NTOKEN2;
	 return;
 };

 int precision=bt_dval("print_precision");
 int show_hex=bt_dval("show_hex");
 // char *ddot_out = (char *)malloc(128);
 char ddot_out[128];

 // MESG("	ddot_pos=%d end=%d todel=%d",ddot_position,line_end,line_end-ddot_position);
 if(buf->b_state & FS_VIEW) {NTOKEN2;return;}; // no refresh in view mode

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
 NTOKEN2;
}

void prev_var(char *title)
{
 bnf_var--;
 MESG("	-var %3ld %s",bnf_var-bnf_vars,title);
 if(bnf_var < &bnf_vars[0]) {
	MESG("Negativ var!");
	exit(1);
 };
}

void next_var(char *title)
{
 bnf_var++;
 MESG("	+var %3ld %s",bnf_var-bnf_vars,title);
 if(bnf_var - &bnf_vars[0]>100) {
	MESG("MAX var exceeded!");
	exit(2);
 };
}

double bnf_result()
{
 int stack_num = bnf_var-bnf_vars;
 switch(bnf_var->var_type) {
 	case VTYPE_NUM:
		MESG("bnf result: %3d NUMERIC val=%f",stack_num,bnf_var->dval);
		return(bnf_var->dval);
		break;
	case VTYPE_STRING:
		MESG("bnf result: %3d STRING  val=[%s]",stack_num,bnf_var->sval);
		return 0.0;
	case VTYPE_POINTER:
	{ MVAR *var=bnf_var->var_pointer;
		if(var->var_type==VTYPE_NUM) {
			MESG("bnf result: %3d var NUMERIC val=%f",stack_num,var->dval);
			return var->dval;
		};
		if(var->var_type==VTYPE_STRING) {
			MESG("bnf result: %3d var STRING  val=[%s]",stack_num,var->sval);
			return 0.0;
		};
		if(var->var_type==VTYPE_NONE) {
			MESG("bnf result: %3d var value undefined",stack_num);
			return 0.0;
		};
	};break;
	default:
		MESG("bnf result: %3d  type %X",stack_num,bnf_var->var_type);
 };
 return 0.0;
}

void bnf_factor_var()
{
	MESG("bnf_factor_var:");
	bnf_var->var_pointer=get_left_slot(tok->tind);
	bnf_var->var_type=VTYPE_POINTER;
	bnf_var->var_index=tok->tind;
	bnf_result();
	next_var("var");
	NTOKEN2;
}

void bnf_factor_num()
{
	MESG("bnf_factor_num:");
	bnf_var->dval = tok->dval;
	bnf_var->var_type=VTYPE_NUM;
	next_var("num");
	NTOKEN2;
}

void bnf_factor_quote()
{
	MESG("bnf_factor_quote");
	bnf_var->sval = tok->tname;
	bnf_var->var_type=VTYPE_STRING;
	next_var("quote");
	NTOKEN2;
}

void bnf_factor_not()
{
 MESG("bnf_factor_not:");
 prev_var("not");
 if(bnf_var->var_type == VTYPE_NUM) {
 	bnf_var->dval = bnf_var->dval==0 ? 1:0;
	next_var("not");
	NTOKEN2;
	return;
 };
 if(bnf_var->var_type == VTYPE_POINTER) {
	MVAR *var = bnf_var->var_pointer;
	if(var->var_type == VTYPE_NUM) {
 		double val = var->dval ==0? 1:0;
		bnf_var->var_type = VTYPE_NUM;
		bnf_var->dval = val;
		next_var("not2");NTOKEN2;
		return;
	};
	// ARRAY op
 };
 // set error!!!!
 next_var("not error");NTOKEN2;
}

void bnf_factor_add()
{
 prev_var("add");
 MESG("bnf_factor_add:");
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	prev_var("add2");
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval += val;
		next_var("add");NTOKEN2;
		return;
	};
 }; 
}

void bnf_factor_plus()
{
 MESG("bnf_factor_plus : var ind=%d tok ind=%d var type=%d",bnf_var-&bnf_vars[0],tok->tnum,bnf_var->var_type);
 prev_var("plus");
	if(bnf_var->var_type == VTYPE_NUM) {
		double val=bnf_var->dval;
		prev_var("plus21");
		if(bnf_var->var_type == VTYPE_NUM) {
			bnf_var->dval += val;
			bnf_result();
			next_var("plus");
		};
		if(bnf_var->var_type == VTYPE_POINTER) {
			MVAR *vara =bnf_var->var_pointer;
			bnf_var->var_type=VTYPE_NUM;
			bnf_var->dval = vara->dval + val;
			next_var("plus2");
		};
		NTOKEN2;
		return;
	};
	if(bnf_var->var_type==VTYPE_POINTER) {
		MVAR *var = bnf_var->var_pointer;
		prev_var("plus22");
		if(var->var_type==VTYPE_NUM) {
			if(bnf_var->var_type == VTYPE_NUM) {
				bnf_var->dval += var->dval;
				next_var("plus22");
			};
			if(bnf_var->var_type == VTYPE_POINTER) {
				double val = bnf_var->var_pointer->dval;
				bnf_var->var_type = VTYPE_NUM;
				bnf_var->dval = val + var->dval;
				next_var("plus23");
			};
		};
		NTOKEN2;
		return;					
	};
 	MESG("plus error!");
	NTOKEN2;
}

void bnf_factor_minus()
{
 MESG("bnf_factor_minus : var ind=%d tok ind=%d var type=%d",bnf_var-&bnf_vars[0],tok->tnum,bnf_var->var_type);
 prev_var("minus");
	if(bnf_var->var_type == VTYPE_NUM) {
		double val=bnf_var->dval;
		prev_var("plus21");
		if(bnf_var->var_type == VTYPE_NUM) {
			bnf_var->dval -= val;
			bnf_result();
			next_var("minus");
		};
		if(bnf_var->var_type == VTYPE_POINTER) {
			MVAR *vara =bnf_var->var_pointer;
			bnf_var->var_type=VTYPE_NUM;
			bnf_var->dval = vara->dval - val;
			next_var("minus");
		};
		NTOKEN2;
		return;
	};
	if(bnf_var->var_type==VTYPE_POINTER) {
		MVAR *var = bnf_var->var_pointer;
		prev_var("minus22");
		if(var->var_type==VTYPE_NUM) {
			if(bnf_var->var_type == VTYPE_NUM) {
				bnf_var->dval -= var->dval;
				next_var("minus22");
			};
			if(bnf_var->var_type == VTYPE_POINTER) {
				double val = bnf_var->var_pointer->dval;
				bnf_var->var_type = VTYPE_NUM;
				bnf_var->dval = var->dval - val;
				next_var("minus23");
			};
		};
		NTOKEN2;
		return;					
	};
 	MESG("minus error!");
	NTOKEN2;
}

void bnf_factor_mul()
{
 MESG("bnf_factor_mul : var ind=%d tok ind=%d var type=%d",bnf_var-&bnf_vars[0],tok->tnum,bnf_var->var_type);
 prev_var("mul");
	if(bnf_var->var_type == VTYPE_NUM) {
		double val=bnf_var->dval;
		prev_var("mul21");
		if(bnf_var->var_type == VTYPE_NUM) {
			bnf_var->dval *= val;
			bnf_result();
			next_var("mul");
		};
		if(bnf_var->var_type == VTYPE_POINTER) {
			MVAR *vara =bnf_var->var_pointer;
			bnf_var->var_type=VTYPE_NUM;
			bnf_var->dval = vara->dval * val;
			next_var("mul2");
		};
		NTOKEN2;
		return;
	};
	if(bnf_var->var_type==VTYPE_POINTER) {
		MVAR *var = bnf_var->var_pointer;
		prev_var("plus22");
		if(var->var_type==VTYPE_NUM) {
			if(bnf_var->var_type == VTYPE_NUM) {
				bnf_var->dval *= var->dval;
				next_var("mul22");
			};
			if(bnf_var->var_type == VTYPE_POINTER) {
				double val = bnf_var->var_pointer->dval;
				bnf_var->var_type = VTYPE_NUM;
				bnf_var->dval = val * var->dval;
				next_var("mul23");
			};
		};
		NTOKEN2;
		return;					
	};
 	MESG("mul error!");
	NTOKEN2;
}

void bnf_factor_div()
{
 MESG("bnf_factor_div : var ind=%d tok ind=%d var type=%d",bnf_var-&bnf_vars[0],tok->tnum,bnf_var->var_type);
 // check for 0 on denominator
 prev_var("div");
	if(bnf_var->var_type == VTYPE_NUM) {
		double val=bnf_var->dval;
		prev_var("div21");
		if(bnf_var->var_type == VTYPE_NUM) {
			bnf_var->dval /= val;
			bnf_result();
			next_var("plus");
		};
		if(bnf_var->var_type == VTYPE_POINTER) {
			MVAR *vara =bnf_var->var_pointer;
			bnf_var->var_type=VTYPE_NUM;
			bnf_var->dval = vara->dval / val;
			next_var("div");
		};
		NTOKEN2;
		return;
	};
	if(bnf_var->var_type==VTYPE_POINTER) {
		MVAR *var = bnf_var->var_pointer;
		prev_var("div22");
		if(var->var_type==VTYPE_NUM) {
			if(bnf_var->var_type == VTYPE_NUM) {
				bnf_var->dval /= var->dval;
				next_var("div22");
			};
			if(bnf_var->var_type == VTYPE_POINTER) {
				double val = bnf_var->var_pointer->dval;
				bnf_var->var_type = VTYPE_NUM;
				bnf_var->dval = var->dval / val;
				next_var("div23");
			};
		};
		NTOKEN2;
		return;					
	};
 	MESG("div error!");
	NTOKEN2;
}

void bnf_factor_modulo()
{
 prev_var("mod");
 // MESG("bnf_factor_modulo:");

 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	prev_var("mod2");
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = ((int)(bnf_var->dval))%(int)val;
		next_var("mod");NTOKEN2;
		return;
	};
	if(bnf_var->var_type == VTYPE_POINTER) {
		double vala = ((int)(bnf_var->var_pointer->dval))%(int)val;
		bnf_var->var_type = VTYPE_NUM;
		bnf_var->dval = vala;
		next_var("mod2");NTOKEN2;
		return;
	};
	// mod on array!! TBD
 };
 MESG("mod2 error");
 NTOKEN2;
}

void bnf_factor_power()
{
 prev_var("power");

 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	prev_var("power2");
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = pow(bnf_var->dval,val);
		next_var("power");NTOKEN2;
		return;
	};
	if(bnf_var->var_type == VTYPE_POINTER) {
		double vala = pow(bnf_var->var_pointer->dval,val);
		bnf_var->var_type = VTYPE_NUM;
		bnf_var->dval = vala;
		next_var("power2");NTOKEN2;
		return;
	};
	// power on array!! TBD
 };
 MESG("power error");
 NTOKEN2;
}

void bnf_factor_smaller()
{
 prev_var("smaller");

 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	prev_var("smaller2");
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = bnf_var->dval < val;
		next_var("smaller");NTOKEN2;
		return;
	};
	if(bnf_var->var_type == VTYPE_POINTER) {
		double vala = bnf_var->var_pointer->dval<val;
		bnf_var->var_type = VTYPE_NUM;
		bnf_var->dval = vala;
		next_var("smaller2");NTOKEN2;
		return;
	};
 };
 MESG("smaller error");
 NTOKEN2;
}

void bnf_factor_bigger()
{
 prev_var("bigger");

 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	prev_var("bigger2");
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = bnf_var->dval > val;
		next_var("bigger");NTOKEN2;
		return;
	};
	if(bnf_var->var_type == VTYPE_POINTER) {
		double vala = bnf_var->var_pointer->dval > val;
		bnf_var->var_type = VTYPE_NUM;
		bnf_var->dval = vala;
		next_var("bigger2");NTOKEN2;
		return;
	};
	// power on array!! TBD
 };
 MESG("bigger error");
 NTOKEN2;
}

void bnf_factor_smallereq()
{
 prev_var("<=");
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	prev_var("smallereq2");
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = bnf_var->dval <= val;
		next_var("smallereq");NTOKEN2;
		return;
	};
	if(bnf_var->var_type == VTYPE_POINTER) {
		double vala = bnf_var->var_pointer->dval<=val;
		bnf_var->var_type = VTYPE_NUM;
		bnf_var->dval = vala;
		next_var("smallereq2");NTOKEN2;
		return;
	};
 };
 MESG("smallereq error");
 NTOKEN2;

}

void bnf_factor_biggereq()
{
 prev_var(">=");
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	prev_var("biggereq2");
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = bnf_var->dval >= val;
		next_var("biggereq");NTOKEN2;
		return;
	};
	if(bnf_var->var_type == VTYPE_POINTER) {
		double vala = bnf_var->var_pointer->dval >=val;
		bnf_var->var_type = VTYPE_NUM;
		bnf_var->dval = vala;
		next_var("biggereq2");NTOKEN2;
		return;
	};
 };
 MESG("biggereq error");
 NTOKEN2;
}

void bnf_factor_equal()
{
 prev_var("==");
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	prev_var("equal2");
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = bnf_var->dval == val;
		next_var("equal");NTOKEN2;
		return;
	};
	if(bnf_var->var_type == VTYPE_POINTER) {
		double vala = bnf_var->var_pointer->dval == val;
		bnf_var->var_type = VTYPE_NUM;
		bnf_var->dval = vala;
		next_var("equal2");NTOKEN2;
		return;
	};
 };
 MESG("equal error");
 NTOKEN2;
}

void bnf_factor_notequal()
{
 prev_var("!=");
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	prev_var("notequal");
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = bnf_var->dval != val;
		next_var("notequal");NTOKEN2;
		return;
	};
	if(bnf_var->var_type == VTYPE_POINTER) {
		double vala = bnf_var->var_pointer->dval != val;
		bnf_var->var_type = VTYPE_NUM;
		bnf_var->dval = vala;
		next_var("noequal2");NTOKEN2;
		return;
	};
 };
 MESG("notequal error");
 NTOKEN2;
}

void bnf_factor_and()
{
 prev_var("and");
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	prev_var("and2");
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = (int)bnf_var->dval && (int)val;
		next_var("and");NTOKEN2;
		return;
	};
	if(bnf_var->var_type == VTYPE_POINTER) {
		double vala = bnf_var->var_pointer->dval && val;
		bnf_var->var_type = VTYPE_NUM;
		bnf_var->dval = vala;
		next_var("and2");NTOKEN2;
		return;
	};
 };
 MESG("and error");
 NTOKEN2;
}

void bnf_factor_nand()
{
 prev_var("nand");
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	prev_var("nand2");
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = !((int)bnf_var->dval && (int)val);
		next_var("nand");NTOKEN2;
		return;
	};
	if(bnf_var->var_type == VTYPE_POINTER) {
		double vala = !(bnf_var->var_pointer->dval && val);
		bnf_var->var_type = VTYPE_NUM;
		bnf_var->dval = vala;
		next_var("nand2");NTOKEN2;
		return;
	};
 };
 MESG("nand error");
 NTOKEN2;
}

void bnf_factor_or()
{
 prev_var("or");
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	prev_var("or2");
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = (int)bnf_var->dval || (int)val;
		next_var("or");NTOKEN2;
		return;
	};
	if(bnf_var->var_type == VTYPE_POINTER) {
		double vala = (int)bnf_var->var_pointer->dval || (int)val;
		bnf_var->var_type = VTYPE_NUM;
		bnf_var->dval = vala;
		next_var("or2");NTOKEN2;
		return;
	};
 };
 MESG("or error");
 NTOKEN2;
}

void bnf_factor_nor()
{
 prev_var("nor");
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	prev_var("nor2");
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = !((int)bnf_var->dval || (int)val);
		next_var("nor");NTOKEN2;
		return;
	};
	if(bnf_var->var_type == VTYPE_POINTER) {
		double vala = !((int)bnf_var->var_pointer->dval || (int)val);
		bnf_var->var_type = VTYPE_NUM;
		bnf_var->dval = vala;
		next_var("nor2");NTOKEN2;
		return;
	};
 };
 MESG("nor error");
 NTOKEN2;
}

void bnf_factor_xor()
{
 prev_var("xor");
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	prev_var("xor2");
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = (bnf_var->dval!=0 && val!=0) || ((bnf_var->dval==0 && val==0));
		next_var("xor");NTOKEN2;
		return;
	};
	if(bnf_var->var_type == VTYPE_POINTER) {
		double vala = (bnf_var->var_pointer->dval!=0  && val!=0) || ((bnf_var->var_pointer->dval==0 && val==0));
		bnf_var->var_type = VTYPE_NUM;
		bnf_var->dval = vala;
		next_var("xor2");NTOKEN2;
		return;
	};
 };
 MESG("xor error");
 NTOKEN2;
}

void bnf_stack_init()
{
 MESG("# bnf_stack_init:");
	bnf_var1 = NULL;
	bnf_var2 = NULL;
	bnf_op   = NULL;
	ind_var1=0;
	ind_var =0;
	value=0;
}

static inline void bnf_factor_none()
{
	MESG("bnf_factor_none!");
	NTOKEN2;
}

void bnf_factor_dummy()
{
	MESG("bnf_factor_dummy!");
	NTOKEN2;
}

void bnf_update_val()
{
	MESG("bnf_update_val:");
	prev_var("update");
	MVAR *aval=bnf_var;
	if(bnf_var->var_type != VTYPE_POINTER) {
		// error
		next_var("update error!");
		NTOKEN2;
		return;
	};
	if(aval->var_type==VTYPE_NUM) { 
		aval->dval += tok->dval;
		next_var("update num");
		NTOKEN2;
		return;
	};
	if(aval->var_type==VTYPE_ARRAY) {
		array_add1(aval->adat,tok->dval);
		next_var("update num");
		NTOKEN2;
		return;
	};
	set_error(tok,222,"cannot update non numeric value!");
	next_var("update type error!");
	NTOKEN2;
	// set for any different type!
}

// aval+=bval
void bnf_increase_by()
{
	MESG("bnf_factor_increase_by:");
	prev_var("inc by");
	MVAR *bval=bnf_var;
	prev_var("inc by2");
	if(bnf_var->var_type != VTYPE_POINTER) {
		next_var("incr by error1");
		NTOKEN2;
		return;
	};

	MVAR *aval=bnf_var->var_pointer;
	if(bval->var_type==VTYPE_POINTER) {
		bval = bval->var_pointer;
	};
	//aval->var_type = bval->var_type;
	if(bval->var_type==VTYPE_NUM) { 
		if(aval->var_type==VTYPE_NUM) {
			aval->dval += bval->dval;
			double val = aval->dval;
			bnf_var->var_type = VTYPE_NUM;
			bnf_var->dval = val;
			next_var("inc by num");
			NTOKEN2;
			return;
		};
		if(aval->var_type==VTYPE_ARRAY) {
			// copy array
			array_add1(aval->adat,bval->dval);
			// set the copied array to bnf_var position
			next_var("inc by array");
			NTOKEN2;
			return;
		};
	};
	
/*
	if(bval->var_type==VTYPE_STRING) {
		aval->sval = strdup(bval->sval);
		NTOKEN2;
		return;
	};
*/
	set_error(tok,1024,"increase operation not supported!");
	NTOKEN2;
}

// aval-=bval
void bnf_decrease_by()
{
	MESG("bnf_factor_decrease_by:");
	prev_var("dec by");
	MVAR *bval=bnf_var;
	prev_var("dec by2");
	if(bnf_var->var_type != VTYPE_POINTER) {
		next_var("dec by error1");
		NTOKEN2;
		return;
	};

	MVAR *aval=bnf_var->var_pointer;
	if(bval->var_type==VTYPE_POINTER) {
		bval = bval->var_pointer;
	};
	//aval->var_type = bval->var_type;
	if(bval->var_type==VTYPE_NUM) { 
		if(aval->var_type==VTYPE_NUM) {
			aval->dval -= bval->dval;
			double val = aval->dval;
			bnf_var->var_type = VTYPE_NUM;
			bnf_var->dval = val;
			next_var("dec by num");
			NTOKEN2;
			return;
		};
		if(aval->var_type==VTYPE_ARRAY) {
			// copy array
			array_add1(aval->adat,-bval->dval);
			// set the copied array to bnf_var position
			next_var("dev by array");
			NTOKEN2;
			return;
		};
	};
/*
	if(bval->var_type==VTYPE_STRING) {
		aval->sval = strdup(bval->sval);
		NTOKEN2;
		return;
	};
*/
	set_error(tok,1024,"decrease operation not supported!");
	NTOKEN2;
}

// aval*=bval
void bnf_mul_by()
{
	MESG("bnf_factor_mul_by:");
	prev_var("mul by");
	MVAR *bval=bnf_var;
	prev_var("mul by2");
	if(bnf_var->var_type != VTYPE_POINTER) {
		next_var("mul by error1");
		NTOKEN2;
		return;
	};

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
			next_var("mul by num");
			NTOKEN2;
			return;
		};
		if(aval->var_type==VTYPE_ARRAY) {
			// copy array
			array_mul1(aval->adat,bval->dval);
			// set the copied array to bnf_var position
			next_var("mul by array");
			NTOKEN2;
			return;
		};
	};
/*
	if(bval->var_type==VTYPE_STRING) {
		aval->sval = strdup(bval->sval);
		NTOKEN2;
		return;
	};
*/
	set_error(tok,1023,"multiply operation not supported!");
	NTOKEN2;
}

// aval /= bval
void bnf_div_by()
{
	MESG("bnf_factor_div_by:");
	prev_var("div by");
	MVAR *bval=bnf_var;
	prev_var("div by2");
	if(bnf_var->var_type != VTYPE_POINTER) {
		// error
		return;
	};
	MVAR *aval=bnf_var->var_pointer;
	if(bval->var_type==VTYPE_POINTER) {
		bval = bval->var_pointer;
	};
	//aval->var_type = bval->var_type;
	if(bval->var_type==VTYPE_NUM) { 
		if(aval->var_type==VTYPE_NUM) {
			aval->dval /= bval->dval;
			NTOKEN2;
			return;
		};
		if(aval->var_type==VTYPE_ARRAY) {
			array_mul1(aval->adat,(1/bval->dval));
			NTOKEN2;
			return;
		};
	};
	
/*
	if(bval->var_type==VTYPE_STRING) {
		aval->sval = strdup(bval->sval);
		NTOKEN2;
		return;
	};
*/
	set_error(tok,1023,"divide operation not supported!");
	NTOKEN2;
}

void bnf_factor_comma()
{
 MESG("bnf_factor_comma:");
	NTOKEN2;
}

void bnf_factor_eof()
{
 	MESG("bnf_factor_EOF:");
	current_active_flag=0;
}

void bnf_factor_sep()
{
 MESG("bnf_factor_sep:");
	NTOKEN2;
	// initialize and/or check expression stack!
}

void bnf_factor_rcurl()
{
 MESG("bnf_factor_rcurl:");
	NTOKEN2;
	// return what ??
}

void bnf_factor_error()
{
 MESG("bnf_factor_error:");
	current_active_flag=0;
	NTOKEN2;
}

void bnf_factor_line_array()
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
			if((bnf_var->var_type==VTYPE_NUM)) adat->mval[ind1].dval=bnf_var->dval;
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
	NTOKEN2;
}

void bnf_factor_assign_var()
{
	// MESG("bnf_factor_assign_var:");
	prev_var("assign var");
	MVAR *bval=bnf_var;
	prev_var("assign var2");
	if(bnf_var->var_type != VTYPE_POINTER) {
		next_var("cannot assign to non var!");
		current_active_flag=0;
		NTOKEN2;
		return;
	};
	MVAR *aval=bnf_var->var_pointer;
	int stack_index=bnf_var-bnf_vars;
	if(bval->var_type==VTYPE_POINTER) {
		bval = bval->var_pointer;
	};
	aval->var_type = bval->var_type;
	if(bval->var_type==VTYPE_NUM) { 
		aval->dval = bval->dval;
		bnf_var->var_type = aval->var_type;
		bnf_var->dval = aval->dval;
		MESG("stack ind=%2d set var to %f",stack_index,aval->dval);
		next_var("assign_var num");
		NTOKEN2;
		return;
	};
	if(bval->var_type==VTYPE_STRING) {
		aval->sval = strdup(bval->sval);
		next_var("assign_var string");
		NTOKEN2;
		return;
	};
	// set for any different type!
	next_var("assign_var undefined");
	NTOKEN2;
}

void bnf_assign_env()
{
	MESG("bnf_assign_env:");
	NTOKEN2;
}

void bnf_assign_opt()
{
	MESG("bnf_assign_opt:");
	NTOKEN2;
}

double bnf_factor_option()
{
 BTNODE *bte; 
 MESG("bnf_factor_option:");
/* variable's name in tok0->tname */
	bte=tok->tok_node;
	// MESG("factor_option: set var_node [%s]",tok_info(tok));
	var_node=bte;
	NTOKEN2;

	set_vtype(bte->node_vtype);
	if(bte->node_vtype==VTYPE_STRING) { /* there is a valid string value */
		clean_saved_string(strlen(bte->node_sval));
		strcpy(saved_string,bte->node_sval);
		return 0;
	} else {
		return(bte->node_dval);
	};
}

// dummy one!
void bnf_factor_lpar()
{
	MESG("bnf_factor_lpar:");
	NTOKEN2; 
}

void bnf_block1(FILEBUF *fp)
{
 INIT_STAGE;
 exe_buffer=fp;
 // double val=0;
   MESG("# bnf_block1:[%s] err_num= %d %d tok=[%s]",fp->b_fname,err_num,current_active_flag,tok_info(tok));
   if(!current_active_flag) {
		tok=fp->end_token;
		// return(ex_var.dval);
   };
   while(tok->tgroup!=TOK_END) 
   {
	MESG("!__exec_block1 [%s]",tok_info(tok));
 	tok->bnf_factor_function();
	// lstoken=NULL;MESG("exec_block1: reset lstoken");
	// if(ex_var.var_type==VTYPE_NUM) ex_var.dval=val;
	if(!current_active_flag) break;
   };
	if(tok->ttype==TOK_RCURL) { 
		NTOKEN2;
		// lstoken=NULL;
		// return(ex_var.dval);
	};
}

void bnf_block1_break(FILEBUF *fp)
{
 INIT_STAGE;
 exe_buffer=fp;
   // MESG("exec_block1:[%s] err_num= %d %d tok=[%s]",fp->b_fname,err_num,current_active_flag,tok_info(tok));
   if(!current_active_flag) {
		tok=fp->end_token;
		// return(ex_var.dval);
   };
   while(tok->tgroup!=TOK_END) 
   {
	// MESG_TOK_INFO("- exec_block1 [%s]",tok);
 	tok->bnf_directive();
	// lstoken=NULL;MESG("exec_block1: reset lstoken");
	// if(ex_var.var_type==VTYPE_NUM) ex_var.dval=val;
	if(!current_active_flag) break;
	if(drv_check_break_key()) break;
   };
	if(tok->ttype==TOK_RCURL) { 
		NTOKEN2;
		// lstoken=NULL;
		// return(ex_var.dval);
	};
}

void bnf_dir_lcurl()
{
	MESG("bnf_dir_lcurl:");
	NTOKEN2;
	bnf_block1(exe_buffer);
}

void bnf_dir_lcurl_break()
{
	NTOKEN2;
	bnf_block1_break(exe_buffer);
}

void bnf_dir_break()
{
	MESG("bnf_dir_break:");
	NTOKEN2;
	current_active_flag=0;
}

void bnf_dir_fori()
{
	MESG("bnf_dir_fori: var_stack_index=%d",bnf_var-bnf_vars);
	tok_struct *start_block;	// element at block start
	tok_struct *end_block=NULL;	/* at the block end  */
	int old_active_flag=current_active_flag;
	MVAR *index=NULL;
	double dinit,dmax,dstep;
	double *iterrator_val;

	NTOKEN2;	/* go to next token after for */

	index=&current_stable[tok->tind];
	MESG("	fori start initialization!");
	bnf_expression();
	MESG("	fori: after index expression tok=%s",tok_info(tok));
#if	0
	if(index->var_type!=VTYPE_NUM) {err_num=224;ERROR("for i syntax error %d",err_num);};
	set_vtype(index->var_type);

	NTOKEN2; // next
	NTOKEN2; // skip equal sign

	dinit=num_expression();	/* initial   */
	iterrator_val=&index->dval;
	*iterrator_val=dinit;
#else
	bnf_var--;
	if(bnf_var->var_type!=VTYPE_NUM) { 
		MESG("	fori: error in fori index! var index=%ld var_type=%d",bnf_var-bnf_vars,bnf_var->var_type);
		exit(1);
	}
	else iterrator_val = &bnf_var->dval;
	dinit = bnf_var->dval;
	// check if ok!
	if(dinit != index->dval) { MESG("	fori: ERROR1 IN FORI index! %f %f",dinit ,&index->dval);};
#endif
	NTOKEN2;	/* skip separator! */

	bnf_expression();
	bnf_var--;
	dmax = bnf_var->dval;
	MESG("	fori max=%f",dmax);
	NTOKEN2;

	bnf_expression();
	bnf_var--;
	dstep = bnf_var->dval;

	MESG("fori: from %f to %f step %f",dinit,dmax,dstep);
	//if(tok->ttype!=TOK_RPAR) { err_num=226;ERROR("for i: error %d",err_num);};
	NTOKEN2;	/* skip right parenthesis  */
	// set block start
	start_block=tok;	/* this is a block start or a simple sentence  */
	
	MESG("	fori start of block token [%s]",tok_info(tok));
	// find token after the end of block
	skip_sentence1();
	end_block=tok;
	MESG("	fori end if block token [%s]",tok_info(tok));

	if(dinit==dmax) {
		tok=end_block;
		current_active_flag=old_active_flag;
		return;
	};
	if(dstep>0 && dmax > *iterrator_val) {

		for(;*iterrator_val < dmax; *iterrator_val +=dstep) {
			tok=start_block;
			tok->bnf_factor_function();
			if(current_active_flag==0) {
				if(is_break1) { tok=exe_buffer->end_token;return;};
				break;
			};
		};
	} else if(dstep<0 && dmax< *iterrator_val) {
		for(; *iterrator_val > dmax; *iterrator_val +=dstep) {
			tok=start_block;
			tok->bnf_factor_function();
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
	current_active_flag=old_active_flag;
}

void bnf_dir_for()
{
// to be defined
	MESG("bnf_dir_for TBD");
	NTOKEN2;
}

void bnf_dir_while()
{
// to be defined
	MESG("bnf_dir_while TBD");
	NTOKEN2;
}

void bnf_expression()
{
	MESG("	bnf_expression: ------ tok ind=%3d ttype=%d tgroup=%d bnf_group=%d",tok->tind,tok->ttype,tok->tgroup,tok->bnf_group);
	while(tok->bnf_group>0) {
		MESG("	bnf_group=%d [%s]",tok->bnf_group,tok_info(tok));
		if(tok->bnf_factor_function==NULL) {
			MESG("bnf_factor_function is NULL!!");
		} else {
			tok->bnf_factor_function();	
		};
	};
	bnf_result();
}

void bnf_dir_return()
{
	MESG("bnf_dir_return:");
	NTOKEN2;
	if(tok->ttype!=TOK_SEP && tok->ttype!=TOK_RPAR) { 
		bnf_expression();
	}
	current_active_flag=0;	/* skip rest of function  */
}

void bnf_dir_type()
{
	MESG("bnf_dir_type TBD");
	NTOKEN2;
}

