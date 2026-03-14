
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

void bnf_factor_var()
{
	bnf_var->var_pointer=get_left_slot(tok->tind);
	bnf_var->var_type=VTYPE_POINTER;
	bnf_var->var_index=tok->tind;
	bnf_var++;
	NTOKEN2;
}

void bnf_factor_num()
{
	bnf_var->dval = tok->dval;
	bnf_var->var_type=VTYPE_NUM;
	bnf_var++;
	NTOKEN2;
}

void bnf_factor_quote()
{
	bnf_var->sval = tok->tname;
	bnf_var->var_type=VTYPE_STRING;
	bnf_var++;
	NTOKEN2;
}

void bnf_factor_not()
{
 bnf_var--;
 if(bnf_var->var_type == VTYPE_NUM) {
 	bnf_var->dval = bnf_var->dval==0 ? 1:0;
	bnf_var++;NTOKEN2;
	return;
 };
 if(bnf_var->var_type == VTYPE_POINTER) {
	MVAR *var = bnf_var->var_pointer;
	if(var->var_type == VTYPE_NUM) {
 		double val = var->dval ==0? 1:0;
		bnf_var->var_type = VTYPE_NUM;
		bnf_var->dval = val;
		bnf_var++;NTOKEN2;
		return;
	};
	// ARRAY op
 };
 // set error!!!!
 bnf_var++;NTOKEN2;
}

void bnf_factor_add()
{
 bnf_var--;
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	bnf_var--;
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval += val;
		bnf_var++;NTOKEN2;
		return;
	};
 }; 
}

void bnf_factor_plus()
{
 bnf_var--;
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	bnf_var--;
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval += val;
		bnf_var++;NTOKEN2;
		return;
	};
 }; 
}

void bnf_factor_minus()
{
 bnf_var--;
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	bnf_var--;
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval -= val;
		bnf_var++;NTOKEN2;
		return;
	};
 }; 
}

void bnf_factor_mul()
{
 bnf_var--;
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	bnf_var--;
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval *= val;
		bnf_var++;NTOKEN2;
		return;
	};
 }; 
}

void bnf_factor_div()
{
 bnf_var--;
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	bnf_var--;
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = bnf_var->dval/val;
		bnf_var++;NTOKEN2;
		return;
	};
 }; 
}

void bnf_factor_modulo()
{
 bnf_var--;
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	bnf_var--;
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = ((int)(bnf_var->dval))%(int)val;
		bnf_var++;NTOKEN2;
		return;
	};
 }; 
}

void bnf_factor_power()
{
 bnf_var--;
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	bnf_var--;
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = pow(bnf_var->dval,val);
		bnf_var++;NTOKEN2;
		return;
	};
 }; 
}

void bnf_factor_smaller()
{
 bnf_var--;
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	bnf_var--;
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = bnf_var->dval < val;
		bnf_var++;NTOKEN2;
		return;
	};
 }; 
}

void bnf_factor_bigger()
{
 bnf_var--;
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	bnf_var--;
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = bnf_var->dval > val;
		bnf_var++;NTOKEN2;
		return;
	};
 }; 
}

void bnf_factor_smallereq()
{
 bnf_var--;
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	bnf_var--;
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = bnf_var->dval <= val;
		bnf_var++;NTOKEN2;
		return;
	};
 }; 
}

void bnf_factor_biggereq()
{
 bnf_var--;
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	bnf_var--;
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = bnf_var->dval >= val;
		bnf_var++;NTOKEN2;
		return;
	};
 }; 
}

void bnf_factor_equal()
{
 bnf_var--;
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	bnf_var--;
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = bnf_var->dval == val;
		bnf_var++;NTOKEN2;
		return;
	};
 }; 
}

void bnf_factor_notequal()
{
 bnf_var--;
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	bnf_var--;
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = bnf_var->dval == val;
		bnf_var++;NTOKEN2;
		return;
	};
 }; 
}

void bnf_factor_and()
{
 bnf_var--;
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	bnf_var--;
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = (int)bnf_var->dval && (int)val;
		bnf_var++;NTOKEN2;
		return;
	};
 }; 
}

void bnf_factor_nand()
{
 bnf_var--;
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	bnf_var--;
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = !((int)bnf_var->dval && (int)val);
		bnf_var++;NTOKEN2;
		return;
	};
 }; 
}

void bnf_factor_or()
{
 bnf_var--;
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	bnf_var--;
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = (int)bnf_var->dval || (int)val;
		bnf_var++;NTOKEN2;
		return;
	};
 }; 
}

void bnf_factor_nor()
{
 bnf_var--;
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	bnf_var--;
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = !((int)bnf_var->dval && (int)val);
		bnf_var++;NTOKEN2;
		return;
	};
 }; 
}

void bnf_factor_xor()
{
 bnf_var--;
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	bnf_var--;
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = (bnf_var->dval!=0 && val!=0) || ((bnf_var->dval==0 && val==0));
		bnf_var++;NTOKEN2;
		return;
	};
 }; 
}

void bnf_stack_init()
{
	// bnf_start= bnf_stack;
	bnf_var1 = NULL;
	bnf_var2 = NULL;
	bnf_op   = NULL;
	ind_var1=0;
	ind_var =0;
	value=0;
}

void bnf_factor_none()
{
	NTOKEN2;
}

void bnf_factor_comma()
{
	NTOKEN2;
}

void bnf_factor_eof()
{
	current_active_flag=0;
}

void bnf_factor_sep()
{
	NTOKEN2;
	lstoken=NULL;
	// initialize and/or check expression stack!
}

void bnf_factor_rcurl()
{
	NTOKEN2;
	lstoken=NULL;
	// return what ??
}

void bnf_factor_error()
{
}

void bnf_factor_assign_var()
{
	bnf_var--;
	MVAR *bval=bnf_var;
	bnf_var--;
	if(bnf_var->var_type != VTYPE_POINTER) {
		// error
		return;
	};
	MVAR *aval=bnf_var->var_pointer;
	if(bval->var_type==VTYPE_POINTER) {
		bval = bval->var_pointer;
	};
	aval->var_type = bval->var_type;
	if(bval->var_type==VTYPE_NUM) { 
		aval->dval = bval->dval;
		NTOKEN2;
		return;
	};
	if(bval->var_type==VTYPE_STRING) {
		aval->sval = strdup(bval->sval);
		NTOKEN2;
		return;
	};
	// set for any different type!
}

void bnf_factor_assign_option()
{
	
}


// dummy one!
void bnf_factor_lpar()
{
 
}

void bnf_block1(FILEBUF *bp)
{

}

void bnf_block1_break(FILEBUF *bp)
{

}

void bnf_dir_lcurl()
{
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
	NTOKEN2;
	current_active_flag=0;
}

double bnf_expression()
{
 double val=0;
 bnf_stack_init();

	while(tok->bnf_group) {
		tok->bnf_factor_function();	
	}
 return val;
}

void bnf_dir_return()
{
	NTOKEN2;
	if(tok->ttype!=TOK_SEP && tok->ttype!=TOK_RPAR) { 
		bnf_expression();
	}
	current_active_flag=0;	/* skip rest of function  */
}


