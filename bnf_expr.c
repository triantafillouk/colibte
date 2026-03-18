
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

double bnf_result()
{
 int var_ind = bnf_var-&bnf_vars[0];
 switch(bnf_var->var_type) {
 	case VTYPE_NUM:
		MESG("bnf result: %3d NUMERIC val=%f",var_ind,bnf_var->dval);
		return(bnf_var->dval);
		break;
	case VTYPE_STRING:
		MESG("bnf result: %3d STRING  val=[%s]",var_ind,bnf_var->sval);
		return 0.0;
	default:
		MESG("bnf result: %3d  type %X",var_ind,bnf_var->var_type);
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
	bnf_var++;
	NTOKEN2;
}

void bnf_factor_num()
{
	MESG("bnf_factor_num:");
	bnf_var->dval = tok->dval;
	bnf_var->var_type=VTYPE_NUM;
	bnf_var++;
	NTOKEN2;
}

void bnf_factor_quote()
{
	MESG("bnf_factor_quote");
	bnf_var->sval = tok->tname;
	bnf_var->var_type=VTYPE_STRING;
	bnf_var++;
	NTOKEN2;
}

void bnf_factor_not()
{
 MESG("bnf_factor_not:");
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
	bnf_var--;
	MVAR *aval=bnf_var;
	if(bnf_var->var_type != VTYPE_POINTER) {
		// error
		return;
	};
	if(aval->var_type==VTYPE_NUM) { 
		aval->dval += tok->dval;
		NTOKEN2;
		return;
	};
	if(aval->var_type==VTYPE_ARRAY) {
		array_add1(aval->adat,tok->dval);
		NTOKEN2;
		return;
	};
	set_error(tok,222,"cannot update non numeric value!");
	NTOKEN2;
	// set for any different type!
}

// aval+=bval
void bnf_increase_by()
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
	//aval->var_type = bval->var_type;
	if(bval->var_type==VTYPE_NUM) { 
		if(aval->var_type==VTYPE_NUM) {
			aval->dval += bval->dval;
			NTOKEN2;
			return;
		};
		if(aval->var_type==VTYPE_ARRAY) {
			array_add1(aval->adat,bval->dval);
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
	//aval->var_type = bval->var_type;
	if(bval->var_type==VTYPE_NUM) { 
		if(aval->var_type==VTYPE_NUM) {
			aval->dval -= bval->dval;
			NTOKEN2;
			return;
		};
		if(aval->var_type==VTYPE_ARRAY) {
			array_add1(aval->adat,-bval->dval);
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
	//aval->var_type = bval->var_type;
	if(bval->var_type==VTYPE_NUM) { 
		if(aval->var_type==VTYPE_NUM) {
			aval->dval *= bval->dval;
			NTOKEN2;
			return;
		};
		if(aval->var_type==VTYPE_ARRAY) {
			array_mul1(aval->adat,bval->dval);
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

void bnf_factor_line_array()
{
	int i=0,j=0;
	int cdim=0;
	int rows=0,cols=0;
	array_dat *adat=tok->tok_adat;
	cdim=1;
	ex_name="Definition";
	// MESG("factor_line_array:%d Array definition ------------",adat->anum);
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
	bnf_var++;
	NTOKEN2;
}

void bnf_factor_assign_var()
{
	MESG("bnf_factor_assign_var:");
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
		MESG("set var to %f",aval->dval);
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

void bnf_assign_env()
{
	MESG("bnf_assign_env:");
	NTOKEN2;
}

void bnf_assign_opt()
{
	MESG("bnf_assign_opt:");
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
   MESG("exec_block1:[%s] err_num= %d %d tok=[%s]",fp->b_fname,err_num,current_active_flag,tok_info(tok));
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
	MESG("bnf_dir_fori: to be defined!");
	
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

void bnf_expression()
{
	MESG("	bnf_expression:");
	while(tok->bnf_group>0) {
		MESG("	bnf_group=%d [%s]",tok->bnf_group,tok_info(tok));
		if(tok->bnf_factor_function==NULL) {
			MESG("bnf_factor_function is NULL!!");
		} else
		tok->bnf_factor_function();	
	};
	bnf_result();
}

void bnf_dir_return()
{
	NTOKEN2;
	if(tok->ttype!=TOK_SEP && tok->ttype!=TOK_RPAR) { 
		bnf_expression();
	}
	current_active_flag=0;	/* skip rest of function  */
}

void bnf_dir_type()
{
	MESG("bnf_dir_type TBD");
}

