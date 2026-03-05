
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


double bnf_not()
{
	return 1.0;
}
#if	0
double bnf_op1()
{
	return 1.0;

}
#endif

double bnf_op2()
{
 double val2=bnf_stack[ind_var1--]->dval;
 double val1=bnf_stack[ind_var1--]->dval;
 
 return val1+val2;
}

double bnf_mul()
{
 double val2=bnf_stack[ind_var1--]->dval;
 double val1=bnf_stack[ind_var1]->dval;
 double val = val1*val2;
 bnf_stack[ind_var1]->dval = val;
 
 return val;
}

double bnf_plus()
{
 double val2=bnf_stack[ind_var1--]->dval;
 double val1=bnf_stack[ind_var1--]->dval;
 return val1+val2;
}

double bnf_assign()
{
 
	return 1.0;
}

double bnf_expression()
{
 double val=0;
 bnf_stack_init();

	while(tok->ttype != TOK_DIR) {
		switch(tok->ttype) {
			case TOK_VAR:
			case TOK_NUM:
			case TOK_QUOTE:
				bnf_stack[ind_var]=tok;
				ind_var1=ind_var++;
				break;
			case TOK_OR:
			case TOK_AND:
				bnf_op2();
				break;
			case TOK_ASSIGN:
				ind_op=ind_var;
				bnf_assign();
				break;
			case TOK_PLUS:
				ind_op=ind_var;
				bnf_plus();
				break;
			case TOK_MUL:
				ind_op=ind_var;
				bnf_plus();
				break;
		};
		NTOKEN2;
	}
 return val;
}

