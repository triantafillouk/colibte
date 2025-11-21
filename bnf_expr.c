
tok_struct* bnf_stack[100];
tok_struct *bnf_start, *bnf_var1, *bnf_var2, *bnf_op;
int	ind_var1=0;
int ind_var2=0;
int ind_op=0;
int ind_var=0;	// current index
double value=0;

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

void stack_push(char *title,tok_struct *tok)
{
 static int ind=0;
 if(no_push) { MESG("stack_push: skip %s",tok_info(tok));return;};
 // check_buffer->tok_table_bnf[check_buffer->tok_bnf_index]=tok;
 memcpy((void *)check_buffer->tok_bnf,(void *)tok,sizeof(tok_struct));
 check_buffer->tok_bnf++;
 check_buffer->tok_bnf_index++;
 MESG("P[%10s] %3d %-15s : %s",check_buffer->b_fname,ind++,tok->tname,tok_info(tok));
}

double bnf_not()
{
	return 1.0;
}

double bnf_op1()
{
	return 1.0;

}

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

