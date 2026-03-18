
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
 int var_ind = bnf_var-&bnf_vars[0];
 switch(bnf_var->var_type) {
 	case VTYPE_NUM:
		MESG("bnf result: %3d NUMERIC val=%f",var_ind,bnf_var->dval);
		return(bnf_var->dval);
		break;
	case VTYPE_STRING:
		MESG("bnf result: %3d STRING  val=[%s]",var_ind,bnf_var->sval);
		return 0.0;
	case VTYPE_POINTER:
	{ MVAR *var=bnf_var->var_pointer;
		if(var->var_type==VTYPE_NUM) {
			MESG("bnf result: %3d var NUMERIC val=%f",var_ind,var->dval);
			return var->dval;
		};
		if(var->var_type==VTYPE_STRING) {
			MESG("bnf result: %3d var STRING  val=[%s]",var_ind,var->sval);
			return 0.0;
		};
	};break;
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
 prev_var("plus");
 MESG("bnf_factor_plus : var ind=%d tok ind=%d var type=%d",bnf_var-&bnf_vars[0],tok->tnum,bnf_var->var_type);
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	prev_var("plus21");
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval += val;
		bnf_result();
		next_var("plus");
		NTOKEN2;
		return;
	};
 } else
	if(bnf_var->var_type==VTYPE_POINTER) {
		MVAR *var = bnf_var->var_pointer;
		prev_var("plus22");
		if(var->var_type==VTYPE_NUM) {
			if(bnf_var->var_type == VTYPE_NUM) {
				bnf_var->dval += var->dval;
				next_var("plus22");
				NTOKEN2;
				return;
			};
			if(bnf_var->var_type == VTYPE_POINTER) {
				double val = bnf_var->var_pointer->dval;
				bnf_var->var_type = VTYPE_NUM;
				bnf_var->dval = val + var->dval;
				next_var("plus23");
				NTOKEN2;
				return;
			};
								
		};
	};
 
}

void bnf_factor_minus()
{
 prev_var("minus");
 // MESG("bnf_factor_minus:");
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	prev_var("minus2");
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval -= val;
		next_var("minus");NTOKEN2;
		return;
	};
 }; 
}

void bnf_factor_mul()
{
 prev_var("mul");
 MESG("bnf_factor_mul : var ind=%d tok ind=%d var type=%d",bnf_var-&bnf_vars[0],tok->tnum,bnf_var->var_type);
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	prev_var("mul2");
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval *= val;
		bnf_result();
		next_var("mul");NTOKEN2;
		return;
	};
 };
 next_var("mul other");
 NTOKEN2; 
}

void bnf_factor_div()
{
 prev_var("div");
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	prev_var("div2");
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = bnf_var->dval/val;
		next_var("div");NTOKEN2;
		return;
	};
 }; 
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
 }; 
}

void bnf_factor_power()
{
 prev_var("power");
 // MESG("bnf_factor_power:");

 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	prev_var("power2");
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = pow(bnf_var->dval,val);
		next_var("power");NTOKEN2;
		return;
	};
 }; 
}

void bnf_factor_smaller()
{
 prev_var("smaller");
 // MESG("bnf_factor_smaller:");

 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	prev_var("smaller2");
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = bnf_var->dval < val;
		next_var("smaller");NTOKEN2;
		return;
	};
 }; 
}

void bnf_factor_bigger()
{
 prev_var("bigger");
 // MESG("bnf_factor_bigger:");

 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	prev_var("bigger2");
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = bnf_var->dval > val;
		next_var("bigger");NTOKEN2;
		return;
	};
 }; 
}

void bnf_factor_smallereq()
{
 prev_var("<=");
 // MESG("bnf_factor_smallereq:");

 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	prev_var("<=2");
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = bnf_var->dval <= val;
		next_var("<=");NTOKEN2;
		return;
	};
 }; 
}

void bnf_factor_biggereq()
{
 prev_var(">=");
 // MESG("bnf_factor_biggereq:");

 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	prev_var(">=2");
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = bnf_var->dval >= val;
		next_var(">=");NTOKEN2;
		return;
	};
 }; 
}

void bnf_factor_equal()
{
 prev_var("==");
 // MESG("bnf_factor_equal:");

 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	prev_var("==2");
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = bnf_var->dval == val;
		next_var("==");NTOKEN2;
		return;
	};
 }; 
}

void bnf_factor_notequal()
{
 prev_var("!=");
 // MESG("bnf_factor_notequal:");

 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	prev_var("!=2");
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = bnf_var->dval == val;
		next_var("!=");NTOKEN2;
		return;
	};
 }; 
}

void bnf_factor_and()
{
 prev_var("and");
 // MESG("bnf_factor_and:");

 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	prev_var("and2");
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = (int)bnf_var->dval && (int)val;
		next_var("and");NTOKEN2;
		return;
	};
 }; 
}

void bnf_factor_nand()
{
 prev_var("nand");
 // MESG("bnf_factor_nand:");

 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	prev_var("nand2");
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = !((int)bnf_var->dval && (int)val);
		next_var("nand");NTOKEN2;
		return;
	};
 }; 
}

void bnf_factor_or()
{
 prev_var("or");
 // MESG("bnf_factor_or:");

 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	prev_var("or2");
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = (int)bnf_var->dval || (int)val;
		next_var("or");NTOKEN2;
		return;
	};
 }; 
}

void bnf_factor_nor()
{
 prev_var("nor");
 // MESG("bnf_factor_nor:");
 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	prev_var("nor2");
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = !((int)bnf_var->dval && (int)val);
		next_var("nor");NTOKEN2;
		return;
	};
 }; 
}

void bnf_factor_xor()
{
 prev_var("xor");
 // MESG("bnf_factor_xor:");

 if(bnf_var->var_type == VTYPE_NUM) {
 	double val=bnf_var->dval;
	prev_var("xor2");
	if(bnf_var->var_type == VTYPE_NUM) {
		bnf_var->dval = (bnf_var->dval!=0 && val!=0) || ((bnf_var->dval==0 && val==0));
		next_var("xor");NTOKEN2;
		return;
	};
 }; 
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
	prev_var("inc by");
	MESG("bnf_factor_increase_by:");
	MVAR *bval=bnf_var;
	prev_var("inc by2");
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
	prev_var("dec by");
	// MESG("bnf_factor_decrease_by:");
	MVAR *bval=bnf_var;
	prev_var("dec by2");
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
	prev_var("mul by");
	// MESG("bnf_factor_mul_by:");
	MVAR *bval=bnf_var;
	prev_var("mul by2");
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
	prev_var("div by");
	// MESG("bnf_factor_div_by:");
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
		MESG("ind %2d set var to %f",aval->dval);
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
   MESG("bnf_block1:[%s] err_num= %d %d tok=[%s]",fp->b_fname,err_num,current_active_flag,tok_info(tok));
   if(!current_active_flag) {
		tok=fp->end_token;
		// return(ex_var.dval);
   };
   while(tok->tgroup!=TOK_END) 
   {
	// MESG_TOK_INFO("- exec_block1 [%s]",tok);
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
	MESG("bnf_dir_fori: to be defined!");
	NTOKEN2;
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

