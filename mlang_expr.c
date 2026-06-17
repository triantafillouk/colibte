/*
	Curses,gtk editor,notes,directory bnfrowser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

 	An interpreter,embeded calculator by K.Triantafillou (2011,2020),
	Normal factor ops
*/


double factor_option();

#if	TBNF
double factor_option()
{
 BTNODE *bte; 
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
#else
double factor_option()
{
 BTNODE *bte; 
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
#endif


void refresh_ddot_1(double value)
{
 TDS("refresh_ddot_1");
 int stat=0;
 TextPoint *tp = tok->ddot;
 FILEBUF *buf = tp->fp;
 // MESG("refresh_ddot: %d",get_vtype());

 if(execmd) {
	 if(vtype_is(VTYPE_NUM)) {
		printf("%s	: %.3f\n",ddot_string(),value);
	 } else if(vtype_is(VTYPE_STRING)) {
		printf("%s	: %s\n",ddot_string(),get_sval());
	 } else if(vtype_is(VTYPE_ARRAY)||vtype_is(VTYPE_SARRAY)||vtype_is(VTYPE_AMIXED)) {
		// MESG("	show_array!");
		print_array1("array: ",get_array("36"));
	 	// print_array1(ddot_string(),get_array("36"));
	 };
	 lstoken=NULL;
	 NTOKEN2;
	 return;
 };

 int precision=bt_dval("print_precision");
 int show_hex=bt_dval("show_hex");
 // char *ddot_out = (char *)malloc(128);
 char ddot_out[128];

 // MESG("	ddot_pos=%d end=%d todel=%d",ddot_position,line_end,line_end-ddot_position);
 if(buf->b_state & FS_VIEW) {NTOKEN2;return;}; // no refresh in view mode

 if(vtype_is(VTYPE_STRING)) {	/* string value  */
	stat=snprintf(ddot_out,sizeof(ddot_out)," \"%s\"",get_sval());
 }  else if(vtype_is(VTYPE_NUM)) {	/* numeric value  */
	long int d = (long int)value;
	if(d==value) {	/* an integer/double value!  */
		if(show_hex) stat=snprintf(ddot_out,sizeof(ddot_out)," %5.0f | 0x%llX | 0o%llo",value,(unsigned long long)value,(unsigned long long)value);
		else stat=snprintf(ddot_out,sizeof(ddot_out)," %5.*f",1,value);
	} else {	/* a decimal value!  */
		stat=snprintf(ddot_out,sizeof(ddot_out)," %5.*f",precision,value);
	};

 } else if(vtype_is(VTYPE_ARRAY) || vtype_is(VTYPE_SARRAY) || vtype_is(VTYPE_AMIXED)) {
	// MESG("refresh_ddot:1");
	array_dat *adat = get_array("37");
	// MESG("refresh_ddot: array: type=%d name=(%s)",adat->atype,adat->array_name);

 	stat=snprintf(ddot_out,sizeof(ddot_out)," array %d:[%s] type [%s] , slot %ld type=%d rows %d,cols %d",adat->anum,
		adat->array_name,vtype_names[adat->atype],lsslot-current_stable,adat->atype,adat->rows,adat->cols);
	// print_array1(":",adat);
 };
 if(stat>MAXLLEN) MESG("truncated");

 update_ddot_line(ddot_out);
 NTOKEN2;
}

double factor_refresh_ddot()
{
 double value = get_val();
 // MESG("TOK_SHOW factor_refresh_ddot");
 // MESG("	val=%f",value);
 refresh_ddot_1(value);
 return value;
}



void init_ex_var()
{
	// MESG("init_ex_var:");
	ex_var.var_type=VTYPE_NUM;
	ex_var.dval=0;
}


void clean_saved_string(int new_size)
{
	if(saved_string) {
		// MESG("clean_saved_string:%s",saved_string);
		free(saved_string);
	};
	if(new_size>0) saved_string=(char *)malloc(new_size+1);
	else saved_string=NULL;
}

static inline double factor_none()
{
	// MESG("factor_none:");
	NTOKEN2;
	return 0.0;	/* continue  */
}

double update_val_num()
{
 double v0=lsslot->dval;
 lsslot->dval += tok->dval;
 NTOKEN2;
 return(v0);
}

/* Used in increase, decrease  */
double update_val()
{
	// MESG("update_val: tnum=%d vtype=%d group=%d",tok->tnum,get_vtype(),tok->tgroup);

	if(vtype_is(VTYPE_NUM)) {
		double v0=0;
		// MESG("	vtype is NUM");
		if(lsslot==NULL) MESG("lsslot is NULL!");
		if(lstoken==NULL) MESG("lstoken is NULL!");
		if(lstoken->ttype==TOK_ARRAY_L1 || lstoken->ttype==TOK_ARRAY1) {
			// MESG("	array1 element!");
			v0=*ls_pdval;
			*ls_pdval += tok->dval;
		} else {
			// MESG("	numeric lsslot type=%d",lsslot->var_type);
			if(lsslot->var_type==VTYPE_AMIXED) {
				if(ls_pdval) {
					v0=*ls_pdval;
					*ls_pdval += tok->dval;
				};
			} else {
				// MESG("update_val: numeric");
				if(lsslot==NULL) {
					MESG("lsslot is NULL!");
				} else {
					MESG("set term function to update_val_num!");
					set_term_function(tok,(TFunction)update_val_num);
					v0=lsslot->dval;
					lsslot->dval += tok->dval;
				};
				// MESG("update_val:1");
			};
		};
		NTOKEN2;
		return(v0);
	};
	if(vtype_is(VTYPE_ARRAY)) {
		// MESG("	vtype is array");
		array_add1(lsslot->adat,tok->dval);
		NTOKEN2;
		return(0);
	};
	set_error(tok,222,"cannot update non numeric value!");
	NTOKEN2;
	return(0);
}

static inline double decrease_by_num()
{
	MVAR *sslot=lsslot;
	sslot->dval -= num_expression();
	return(sslot->dval);
}


double decrease_by()
{
	double v1,v0;
	MVAR *sslot=lsslot;
	// MESG("decrease_by: slot_index=%d ",sslot->var_index);
	tok_struct *ptok=tok-1;
	// MESG("decrease_by: tok type=%d",ptok->ttype);
	int ori_type=lstoken->ttype;
	tok_struct *lstok=lstoken;
#if	0
	if(ori_type!=TOK_VAR && ori_type!=TOK_ARRAY_L1 && ori_type!=TOK_ARRAY1) {
		MESG("decrease_by: ori_type=%d ",ori_type);
		set_error(lstok,1022,"decrease constant not supported!");
		return(0);
	};
#endif
	v1=-num_expression();
	// MESG("		>>   : by %f",v1);

	if(sslot->var_type==VTYPE_NUM)
	if(vtype_is(VTYPE_NUM)) {
		set_term_function(ptok,(TFunction)decrease_by_num);
		sslot->dval += v1;
		return(sslot->dval);
	};

	if(sslot->var_type==VTYPE_ARRAY) {
		// MESG("		VTYPE_ARRAY:");
		if(ori_type!=TOK_VAR) {
			v0=*ls_pdval;
			*ls_pdval = v0+v1;
			return(v0);
		} else {
			array_add1(sslot->adat,v1);
			set_array(sslot->adat);
			return(v1);
		};
	};

	if(sslot->var_type==VTYPE_AMIXED) {
		if(lmvar->var_type==VTYPE_NUM) {
			double v0=lmvar->dval;
			lmvar->dval+=v1;
			return v0;
		}
	};

	set_error(lstok,1024,"decrease operation not supported!");
	return(v1);
}

static inline double increase_by_num()
{
	MVAR *sslot=lsslot;
	sslot->dval += num_expression();
	return(sslot->dval);
}

double increase_by()
{
	double v1,v0;
	MVAR *sslot=lsslot;
	// MESG("increase_by: slot_index=%d ",sslot->var_index);
	tok_struct *ptok=tok-1;
	// MESG("increase_by: tok type=%d",ptok->ttype);
	int ori_type=lstoken->ttype;
	tok_struct *lstok=lstoken;
#if	0
	if(ori_type!=TOK_VAR && ori_type!=TOK_ARRAY_L1 && ori_type!=TOK_ARRAY1) {
		MESG("increase_by: ori_type=%d ",ori_type);
		set_error(lstok,1022,"increase constant not supported!");
		return(0);
	};
#endif
	v1=num_expression();
	// MESG("		>>   : by %f",v1);

	if(sslot->var_type==VTYPE_NUM)
	if(vtype_is(VTYPE_NUM)) {
		set_term_function(ptok,(TFunction)increase_by_num);
		sslot->dval += v1;
		return(sslot->dval);
	};

	if(sslot->var_type==VTYPE_ARRAY) {
		// MESG("		VTYPE_ARRAY:");
		if(ori_type!=TOK_VAR) {
			v0=*ls_pdval;
			*ls_pdval = v0+v1;
			return(v0);
		} else {
			array_add1(sslot->adat,v1);
			set_array(sslot->adat);
			return(v1);
		};
	};

	if(sslot->var_type==VTYPE_STRING) {
		// MESG("increase by:");
		sslot->sval=(char *)realloc(sslot->sval,strlen(sslot->sval)+strlen(get_sval())+1);
		strcat(sslot->sval,get_sval());
		set_sval(sslot->sval);
		return(v1);
	};

	if(sslot->var_type==VTYPE_SARRAY) {
		if(vtype_is(VTYPE_STRING)) {
		if(ori_type!=TOK_VAR) {
			// MESG("	increase string val");
			char *stmp=malloc(strlen(get_sval())+strlen(ls_psval[0]));
			strcpy(stmp,ls_psval[0]);
			strcat(stmp,get_sval());
			free(ls_psval[0]);
			ls_psval[0]=stmp;
		} else {
			// MESG("	increase sarray! sslot ind=%d %d %s",sslot->var_index,get_vtype(),get_sval());
			sarray_add1(sslot->adat,get_sval());
		};
		};
		return(0);
	};

	// MESG("		return -1 sslot->var_type=%d vtype=%d",sslot->var_type,get_vtype());
	if(sslot->var_type==VTYPE_AMIXED) {
		if(lmvar->var_type==VTYPE_NUM) {
			double v0=lmvar->dval;
			lmvar->dval+=v1;
			return v0;
		} if(lmvar->var_type==VTYPE_STRING) {
			// MESG("	string cat");
			lmvar->sval=str_cat(lmvar->sval,get_sval());
			return 0;
		}
	};
	set_error(lstok,1024,"increase operation not supported!");
	return(v1);
}

double mul_by_num()
{
	MVAR *sslot=lsslot;
	sslot->dval *= num_expression();
	return(sslot->dval);
}

double mul_by()
{
	double v1,v0;
	// MESG("mul_by:");
	MVAR *sslot=lsslot;
	tok_struct *ptok=tok;
	// TDS("mul_by");
	int ori_type=lstoken->ttype;
	tok_struct *ltok = tok;
#if	0
	if(ori_type!=TOK_VAR) {
		set_error(lstoken,1022,"mul by constant not supported!");
		return(0);
	};
#endif
	v1=num_expression();

	if(sslot->var_type==VTYPE_NUM) {
		if(ori_type==VTYPE_NUM)
			set_term_function(ptok,(TFunction)mul_by_num);
		sslot->dval *= v1;
		return(sslot->dval);
	};
	if(sslot->var_type==VTYPE_ARRAY) {
		if(ori_type!=TOK_VAR) {
			v0=*ls_pdval;
			*ls_pdval = v0*v1;
			return(v0*v1);
		} else {
			array_mul1(sslot->adat,v1);
			set_array(sslot->adat);
			return(v1);
		};
	};
	if(sslot->var_type==VTYPE_STRING) {
		// MESG("mul by:");
		set_sval(str_mul(sslot->sval,v1));
		return(0);
	};

	if(sslot->var_type==VTYPE_SARRAY) {
		if(vtype_is(VTYPE_NUM) && v1>0) {
		if(ori_type!=TOK_VAR) {
			// MESG("	mul string val");
			char *stmp=str_mul(ls_psval[0],v1);
			free(ls_psval[0]);
			ls_psval[0]=stmp;
		} else {
			// MESG("	mul sarray! sslot ind=%d %d %s",sslot->var_index,get_vtype(),get_sval());
			sarray_mul1(sslot->adat,v1);
		};
		};
		return(0);
	};

	if(sslot->var_type==VTYPE_AMIXED) {
		if(vtype_is(VTYPE_NUM)) {
			// MESG("	VTYPE_NUM sslot vtype=%d %d",VTYPE_AMIXED,VTYPE_NUM);
			// MESG("	lmvar type=%d",lmvar->var_type);

			if(ori_type!=TOK_VAR) {
				if(lmvar->var_type==VTYPE_STRING) {
					char *tmp_s= str_mul(lmvar->sval,v1);
					// MESG("mul_by: mixed string1 %s",tmp_s);
					lmvar->sval = tmp_s;
				} else {
					lmvar->dval *= v1;
				};
			return(v1);
		};	
		if(vtype_is(VTYPE_STRING)) {
			char *tmp_s= str_mul(lmvar->sval,v1);
			// MESG("mul_by: mixed string2 %s",tmp_s);
			lmvar->sval = tmp_s;
			// MESG("	result [%s]",lmvar->sval);
			return v1;
		};
	 };
	};
	MESG("error token [%s] ind=%d line=%d type=%d",ltok->tname,ltok->tind,ltok->tline,ltok->ttype);
	set_error(ltok,1023,"multiply operation not supported!");
	return(-1);
}





/* ---------------------- Factor functions ------------------------------------ */

double factor_line_array()
{
 double value=0.0;
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
		value=cexpression();
		// MESG("	factor_line_array: set type=%d",get_vtype());
		if(vtype_is(VTYPE_STRING) && adat->atype!=VTYPE_AMIXED) adat->atype=VTYPE_SARRAY;
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
	};cdim--;
	};
	adat->astat=ARRAY_LOCAL;
	// print_array1("",adat);
	set_array(adat);
	NTOKEN2;
	RTRN(1.2);
}

#if	USE_TYPE_VARS
double factor_type_element()
{
 double value=0;
 array_dat *adat = lsslot->adat;
 // MESG("	factor_type_element: lsslot %d [%s] array name=[%s] vtype=%d (%s)",lsslot->var_index,lstoken->tname,adat->array_name,lsslot->var_type,vtype_names[lsslot->var_type]);
 // MESG("	type_element [%s]",tok->tname);
 if(adat->var_tree) {
 	int ind;
 	// MESG("	var tree name [%s]",adat->var_tree->tree_name);
	if(tok->dval<0) {
		BTNODE *el_node = find_btnode(adat->var_tree,tok->tname);
		if(el_node) {
			ind = (int)(el_node->node_index);
			tok->dval=ind;
		} else {
			set_error(tok,501,"element not found!");
			return(0);
		};
	} else {
		ind=(int)tok->dval;
		// MESG(" ind from cache %d",ind);
	};
	// MESG("	ind = %d",ind);
	if(adat->mval[ind].var_type==VTYPE_NUM) {
		value=adat->mval[ind].dval;
		set_vdval(value);
		// MESG("	return type dval %f",value);
	} else {
		set_vtype(VTYPE_STRING);
		set_sval(adat->mval[ind].sval);
		// MESG("	return type sval [%s]",adat->mval[ind].sval);
	};

 } else {
 	set_error(tok,502,"no tree in type element!");
	return(0);
 };
 
 NTOKEN2;
 return value;
}
#endif

double factor_variable_num()
{
	// MESG("factor_variable_num:");
 	lsslot = get_left_slot(tok->tind);
	lstoken = tok;
	set_vdval(lsslot->dval);
	NTOKEN2;
	return(lsslot->dval);
}

double factor_variable()
{
 	lsslot = get_left_slot(tok->tind);
	// MESG("factor_variable:--[%s] ind=%d type=%d",tok->tname,tok->tind,tok->ttype);
	// lsslot = current_stable+tok->tind;
	int ptype=get_vtype();
	if(lstoken==NULL) { 
		lstoken = tok;
	};
	// MESG("factor_variable: %d type %d",tok->tind,lsslot->var_type);
	set_vtype(lsslot->var_type);
	// MESG("	factor_variable:[%s] tind=%d ,var ind=%d ex_vtype=%d ttype=%d vtype=%d",
		// tok->tname,tok->tnum,lsslot->var_index,get_vtype(),tok->ttype,lsslot->var_type);
	switch(get_vtype()) {
		case VTYPE_NUM:
		{
			if(ptype==VTYPE_NUM) tok->factor_function = factor_variable_num;
			set_vdval(lsslot->dval);
			NTOKEN2;
			return(lsslot->dval);
			};
		case VTYPE_STRING:
			set_sval(lsslot->sval);
			NTOKEN2;
			RTRN(lsslot->dval);
		case VTYPE_ARRAY:
		case VTYPE_SARRAY:
			// MESG("	factor_variable: array num=%d atype=%d slot_index=%d",lsslot->adat->anum,lsslot->adat->atype,lsslot->var_index);
			set_array(lsslot->adat);
			lsslot->adat->atype=get_vtype();
			ex_name=tok->tname;
			// MESG("	factor_variable: [%s] array atype=%d vtype=%d",tok->tname,lsslot->adat->atype,get_vtype());
			NTOKEN2;
			RTRN(lsslot->dval);
		case VTYPE_AMIXED:
			set_array(lsslot->adat);
			lsslot->adat->atype=get_vtype();
			ex_name=tok->tname;
			// MESG("factor_variable:AMIXED  [%s] array type=%d",tok->tname,lsslot->adat->atype);
			NTOKEN2;
			RTRN(lsslot->dval);
#if	GTYPE
		case VTYPE_TREE:
			// MESG("found BTREE variable");
			set_vtype(1);
			NTOKEN2;
			RTRN(0);
#endif
		default:
		// error !!
		syntax_error("no data in the array!!",2081);
		NTOKEN2;
		RTRN(lsslot->dval);
	};
}

#if	USE_TYPE_VARS
double factor_assign_type()
{
	double value=0;
	tok_struct *tok0 = tok;
	BTREE *var_tree = tok->tok_node->node_dat;
	MVAR *var_slot=lsslot;
	int columns = var_tree->items;
	MVAR *svar = btree_to_mvar(var_tree);
	int size2=1;
	MESG("factor_assign_type: [%s] items=%d",tok_info(tok),columns);
	// if(lsslot) MESG("	lsslot ind=%d type=%d",lsslot->var_index,lsslot->var_type);
	// MESG("factor_assign_type: -- var_slot ind=%d vtype=%d",var_slot->var_index,var_slot->var_type);
	// MESG(" factor_assign_type: $$$$$ name=[%s] type %d vtype=%d line=%d size=%d",tok->tname,tok->ttype,var_slot->var_type,tok->tline,size);

	NTOKEN2;

	if(check_token(TOK_LBRAKET)) {
		// MESG("	type double array!");
		NTOKEN2;
		size2 = (int)num_expression();
		NTOKEN2;
		// MESG("	end: size2=%d [%s]",size2,tok_info(tok));
	};

	array_dat *adat=alloc_array_header();
	adat->rows=size2;
	adat->cols=columns;
	adat->atype=VTYPE_AMIXED;
	if(size2==1) adat->mval=svar;
	else {
		int asize=adat->rows*adat->cols;
		adat->mval=(struct MVAR *)malloc(sizeof(struct MVAR)*asize);
		// MESG("	allocate %d elements in mval",asize);
	};
	adat->astat=ARRAY_ALLOCATED;
	adat->array_name=strdup(tok0->tname);
	adat->var_tree = var_tree;
	// MESG("	>> items %d",size2);

	
	if(check_token(TOK_LPAR)) {
		// MESG("	assign with parenthessis!");
		int i;
		int row=0;
		NTOKEN2;
#if	0
		MVAR *avars=adat->mval;
		MESG("	avars addr=%Xl",(long)avars);
		for(row=0;row<size2;row++) {
		for(i=0;i<columns;i++) {
			value=lexpression();
			int ind=columns*row+i;
			if(ex_var.var_type!=svar[i].var_type) {
				MESG("type mismatch %d !=%d ",ex_var.var_type,svar[ind].var_type);
				set_error(tok,1022,"type mismatch!");
				return(0);
			};
			avars[ind].var_type=ex_var.var_type;
			if(ex_var.var_type==VTYPE_STRING) {
				MESG("	%2d: type=%d val=[%s]",ind,ex_var.var_type,get_sval());
				avars[ind].sval=strdup(get_sval());
			} else {
				MESG("	%2d: type=%d val=%f %f",ind,ex_var.var_type,ex_var.dval,value);
				avars[ind].dval=value;
			};
			// MESG("next toke type is %s",tok_info(tok));
			NTOKEN2;	// skip end parenthesis or separator
		};
		};
#else
		for(row=0;row<size2;row++) {
		MVAR *row_var=&adat->mval[row*columns];
		for(i=0;i<columns;i++) {
			value=lexpression();
			if(ex_var.var_type!=svar[i].var_type) {
				set_error(tok,1022,"type mismatch!");
				return(0);
			};
			row_var[i].var_type=ex_var.var_type;
			if(ex_var.var_type==VTYPE_STRING) {
				// MESG("	%2d: type=%d val=[%s]",i,ex_var.var_type,get_sval());
				row_var[i].sval=strdup(get_sval());
			} else {
				MESG("	%2d: type=%d val=%f %f",i,ex_var.var_type,ex_var.dval,value);
				row_var[i].dval=value;
			};
			// MESG("next toke type is %s",tok_info(tok));
			NTOKEN2;	// skip end parenthesis or separator
		};
		};
#endif
	};

	if(var_slot) {
		// MESG("	add var_type to array!");
		var_slot->var_type=VTYPE_AMIXED;
		var_slot->adat = adat;
	};
	set_array(adat);
	// print_array1("type array",adat);
	return 1;
}
#endif

double factor_array2()
{
	int ind1=0;
	int ind2=0;
	double value=0;
	MVAR *array_slot;
	array_dat *adat;
	tok_struct *tok0 = tok;
	// MESG("factor_array2: %s",tok->tname);
	array_slot=get_left_slot(tok->tind);
	adat=array_slot->adat;
	char *array_name=tok->tname;
	lstoken=tok;
	// MESG("factor_array2: type %d",adat->atype);
	
	if(adat==NULL) {
		/* No data in the array, allocate new ones!  */
		NTOKEN2;
		ind1=(int)num_expression();
		NTOKEN2;
		ind2=(int)num_expression();
		NTOKEN2;
		adat = new_array(ind1+1,ind2+1,VTYPE_ARRAY);
		allocate_array(adat);
		adat->atype=VTYPE_ARRAY;
		adat->array_name = array_name;
		array_slot->adat = adat;
		// MESG("created a 2 dim array!");
		// MESG("	array name=%s",array_name);
		print_array1("new array created",adat);

		// MESG("new rows=%d cols=%d",adat->rows,adat->cols);
		NTOKEN2;
			// value=dval2[ind1][ind2];
	} else
	{
		NTOKEN2;
		ind1=(int)num_expression();
		NTOKEN2;
		ind2=(int)num_expression();
		NTOKEN2;
	};

		if((ind1 >= adat->rows) || (ind2 >= adat->cols)) {
			// syntax_error("array indexes out of bound!",209);
			set_error(tok0,209,"array indexes out of bound!");
		} else {
			if(adat->atype==VTYPE_ARRAY) {
				// MESG("- 2 dim num array!");
				double **dval2 = adat->dval2;
				set_vtype(VTYPE_NUM);
				value=dval2[ind1][ind2];

				ls_pdval=&dval2[ind1][ind2];
				return(value);
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
					return(value);
				} else {
					// MESG("	string: at %d",ind);
					// MESG("	string: val=[%s]",adat->mval[ind].sval);
					set_sval(adat->mval[ind].sval);
					// MESG("	string: val=[%s]",adat->mval[ind].sval);
					value=0;
					return(0);
				};
			};
		};

	// MESG("end factor_array2: lsslot=%X",(void *)lsslot);
	return(value);
}

double factor_array1()
{
	int ind1;
	double *dval=NULL;
	double value=0;
	MVAR *array_slot;
	array_slot=&current_stable[tok->tind];
	array_dat *adat = array_slot->adat;
	lstoken=tok;
	// MESG("factor_array1:----------- vtype=%d",array_slot->var_type);
	NTOKEN2;
	ind1 = (int)num_expression();

	if(adat==NULL) {	/* this must not happen!!!  */
		// MESG("array adat is NULL allocate new one !!!!!!!!!!!!");
		ex_nums=1;
		adat=new_array(ind1+1,1,VTYPE_ARRAY);
		array_slot->adat=adat;
		array_slot->var_type=VTYPE_ARRAY;
		allocate_array(array_slot->adat);	/*   */
	};

	if(tok->ttype==TOK_RBRAKET) { 
		NTOKEN2;
		// MESG("ends with rbracket!!");
	};
	// MESG("factor_array1:ind=%d ind1=%d type=%d",array_slot->var_index,ind1,array_slot->var_type);

		// MESG("	2 vtype=%d %d",array_slot->var_type,VTYPE_ARRAY);
		if(array_slot->var_type==VTYPE_ARRAY) {
		if(array_slot->adat->rows<ind1 && array_slot->adat->cols<ind1) {
			double *dval_old = array_slot->adat->dval;
			// MESG("+++ reallocate ind1=%d x %d %X",ind1,sizeof(double),dval_old);
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
				return 0;
			};
			array_slot->adat->dval = dval_new; 
			// MESG("	array reallocated:%X",array_slot->adat->dval);
		} else { 
			dval = array_slot->adat->dval;
	
			value=dval[ind1];
			// MESG("	pdval value [%d]=%f",ind1,value);
			ls_pdval=&dval[ind1];
			// MESG("	array_slot %d: pdal=%f",array_slot->var_index, *array_slot->pdval);
			set_vtype(VTYPE_NUM);
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
	// MESG("        : >>>> end");
	// MESG("	factor_array1:ind1=%d lsslot ind=%d type=%d rows=%d cols=%d [%s]!",ind1,lsslot->var_index,lsslot->var_type,lsslot->adat->rows,lsslot->adat->cols,array_slot->psval[0]);
	return(value);
}

double factor_array_l1()
{
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
			return(0);
		};
	} else {
		MESG("No var_tree!");
		set_error(lstoken,506,"No var_tree");
		return(0);
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
	return(value);
}

double factor_array_l2()
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
	if(adat==NULL) { set_error(tok,505,"var not defined as array");return(0);};
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
			return(0);
		};
	} else {
		// MESG("No var_tree!");
		set_error(lstoken,503,"No var_tree");
		return(0);
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
	return(value);
}

double factor_cmd()
{ // 3 editor command returns an int (??)
	int function_index;
	int check_par=0;
	int save_macro_exec;
	double value=1;
	FUNCS *ed_command;

	// MESG(";factor_cmd: ttype=%d command=%d",tok->ttype,tok->tok_node->node_index);
	function_index = tok->tok_node->node_index;
	ed_command = ftable+function_index;

	NTOKEN2;
	save_macro_exec=macro_exec;
	macro_exec=MACRO_MODE2;
	// MESG(";ed_command: [%s] args=%d",ed_command->n_name,ed_command->arg);
	if(ed_command->arg) {
		check_par=1;	/* we need parenthesis if arguments.  */

		value=num_expression();
		// MESG(";	ed_command: value=%f %f ex_vtype=%d s=[%s] arg=%d",value,get_val(),get_vtype(),get_sval(),ed_command->arg);
		set_dval(value);
		switch(ed_command->arg) {
			case 1:{ /* one argument */
				// MESG("	one argument type=%d",get_vtype());
				if(vtype_is(VTYPE_STRING)) { value=1;};
				break;
			};
			case 2:	/* two arguments  */
			{
				NTOKEN2;
#if	0
				value=num_expression();
				// MESG(";	ed_command:arg2 value=%f ex_var.var_type=%d s=[%s]",value,ex_var.var_type,get_sval());
#endif
				// MESG(";ed_command: second token! type=%d ",tok->ttype);
				break;
			};
		};

	};

	macro_exec = MACRO_MODE2;

	// err_num=0;
	err_line=tok->tline;
	err_str=NULL;
	// MESG(";factor_cmd: execute function! current token is [%s] tnum=%d value=%d",tok->tname,tok->tnum,(int)value);
	value=ed_command->n_func((int)value);

	set_vdval(value);
	// value=get_val();
	// MESG("TOC_CMD: result %f",get_val());
	macro_exec = save_macro_exec;

	if(check_par) { 
		if(check_rparenthesis()) {
			NTOKEN2;
			// MESG("right parenthesis skipped!");
		};
	};

	if(err_num>0) {
		// ERROR("error %d after function [%s] at line %d: %s",err_num,ftable[function_index].n_name,err_line,err_str);
		set_error(tok,105,"factor_cmd");
		show_error("Factor","factor_cmd");
	};

	// MESG(";factor_cmd:end value=%f status=%d err=%d",value,status,err_num);
	RTRN(value);
}

// Editor env variable
double factor_env()
{
	BTNODE *bte;
	double value=0;
	bte=tok->tok_node;
	var_node=bte;
	// MESG("factor_env: set var_node [%s]",tok_info(tok));
	value = get_env(bte->node_index);
	NTOKEN2;
	RTRN(value);
}

static inline double factor_lpar()
{
 double value;
 // MESG("factor_lpar");
 NTOKEN2;
 value = lexpression();
 NTOKEN2;	/* skip corresponding right parenthesis!  */
 return value;
}

static inline double factor_num()
{
	double val=tok->dval;
	set_vtype(VTYPE_NUM);
	NTOKEN2;
	RTRN(val);
}

/* string  */
double factor_quote()
{
	// MESG("factor_quote: tnum=%d tname=%s",tok->tnum,tok->tname);
	set_sval(tok->tname);
	NTOKEN2;
	RTRN(0);		/* 0 value for string variables  */
}

double factor_at()
{
	int bval=macro_exec;
	double value;
	macro_exec=MACRO_TRUE;
	set_vtype(VTYPE_NUM);
	value=sysexec((char *)tok->tname);
	NTOKEN2;
	macro_exec=bval;
	RTRN(value);
}

static inline double factor_plus()
{
	NTOKEN2;
	return ( FACTOR_FUNCTION );
}

static inline double factor_minus()
{
	// MESG("factor_minus:");
	NTOKEN2;
	return (- FACTOR_FUNCTION);
}
static inline double factor_not()
{
 double value;
	NTOKEN2;
	value=FACTOR_FUNCTION;
	RTRN(value==0.0 ? 1.0: 0.0);
}

double factor_proc()
{
	tok_struct *tok0=tok;
	FILEBUF *cbuf=exe_buffer;
	// MESG("factor_proc:");
	exe_buffer=tok0->proc_buffer;
#if	TPROFILE
	tok0->proc_buffer->function_called++;
#endif
	// MESG("factor_proc: cbuf=%s ---------------- args=%d",cbuf,tok0->t_nargs);
	NTOKEN2;	// skip proc name
	// MESG("factor_proc: tok0 [%d %s] args=%d",tok0->tnum,tok0->tname,tok0->tind);
	// MESG("factor_proc: tok  [%d %s] %d ",tok->tnum,tok->tname,tok->tind);

	double value = exec_function(tok0->proc_buffer,tok0->t_nargs);

	// MESG("factor_proc: return val=%f",value);

	current_active_flag=1;	/* start checking again  */

	exe_buffer=cbuf;
	RTRN(value);
}

static double term2_power(double v1)
{
 double v2;
	NTOKEN2;
	if(vtype_is(VTYPE_ARRAY)){
		array_dat *loc_array = get_array("7");
		v2 = FACTOR_FUNCTION;
		if(vtype_is(VTYPE_NUM)) {
			if(loc_array->astat==ARRAY_LOCAL) loc_array=dup_array_power(loc_array,v2);
			else array_power(loc_array,v2);
			set_array(loc_array);
			// set_vtype(VTYPE_ARRAY);
			ex_name="array power numeric";
		} else {
			syntax_error("wrong power on table",2101);
			set_break();
		};
		return(1);
	} else {
		v2=FACTOR_FUNCTION;
		v1=pow(v1,v2);
		set_vtype(VTYPE_NUM);
	}
 return v1; 	
}


static double term2_modulo(double v1)
{
double v2;
	NTOKEN2;
	if(vtype_is(VTYPE_ARRAY)){
		array_dat *loc_array = get_array("8");
		v2 = FACTOR_FUNCTION;
		if(v2>0) 
		{
			if(loc_array->astat==ARRAY_LOCAL) loc_array=dup_array_mod1(loc_array,v2);
			else array_mod1(loc_array,v2);
			set_array(loc_array);
			// set_vtype(VTYPE_ARRAY);
			ex_name="array modulo numeric";
		} else {
			syntax_error("wrong modulo on table",2102);
			set_break();
		};
		return(1);
	} else {
	v2 = FACTOR_FUNCTION;
	if(v2>0) {
		v1 = modulo(v1,v2);
		set_vtype(VTYPE_NUM);
	} else {
		/* RT error  */
		syntax_error("wrong modulo value value=0",2103);
		set_break();
	};
	}
	RTRN(v1);
}

inline static double term1_mul_num(double v1)
{
	NTOKEN2;
	return v1*num_term2();
}


static double term1_mul(double v1)
{
 double v2;
 TDS("term1_mul");
 tok_struct *ptok=tok;
 // MESG("term1_mul: tok type=%d ind=%d",ptok->ttype,ptok->tind);
	if(vtype_is(VTYPE_NUM)){
		NTOKEN2;
		v2=num_term2();
		// MESG("term1_mul: second type=%d",ex_vtype);
		switch(get_vtype())
		{
			case VTYPE_NUM:	// numeric * numeric
				set_term_function(ptok,term1_mul_num);
				return v1*v2;

			case VTYPE_ARRAY:	// numeric * array
				set_array(dup_array_mul1(get_array("9"),v1));
				ex_name = "numeric * array";
				// set_vtype(VTYPE_ARRAY);
				return 1;
		};
	};
	if(vtype_is(VTYPE_STRING)) {
		char *sval=strdup(get_sval());
		NTOKEN2;
		v2=num_term2();
		// MESG(";; sval %s",sval);
		if(vtype_is(VTYPE_NUM)) {
			char *news=str_mul(sval,v2);
			// MESG(";; news=%s",news);
			set_sval(news);
			// MESG("ok!");
			return v2;
		};
		if(vtype_is(VTYPE_STRING)) {
			syntax_error("cannot multiply str * str!",2102);
			set_break();
			return(0);
		};
	};
	if(vtype_is(VTYPE_ARRAY)){
		array_dat *loc_array = get_array("10");
		NTOKEN2;
		v2 = num_term2();
		if(vtype_is(VTYPE_NUM)) {
			if(loc_array->astat==ARRAY_LOCAL) loc_array=dup_array_mul1(loc_array,v2);
			else array_mul1(loc_array,v2);
			set_array(loc_array);
			// set_vtype(VTYPE_ARRAY);
			ex_name="array * numeric";
			return 1;
		};
		if(vtype_is(VTYPE_ARRAY)) {
			array_dat *array1 = get_array("11");
			if(loc_array->rows==1 && array1->cols==1) {
				if(loc_array->cols== array1->rows) {
					int i;
					v1=0;
					for(i=0;i<loc_array->cols;i++){
						v1 += loc_array->dval[i]*array1->dval[i];
					};
					set_vtype(VTYPE_NUM);
					// free old ex_array ???
					set_array(NULL);
					ex_name="array * array";
					RTRN(v1);
				};
				syntax_error("array multiply error",213);
				set_vtype(VTYPE_NUM);
				RTRN(v1);
			} else {
				array_dat *loc_array2;
				loc_array2=array_mul2(loc_array,get_array("14"));
				set_vtype(VTYPE_ARRAY);
				
				if(loc_array->astat==ARRAY_ALLOCATED) {	/* free this one!!  */
				};
				set_array(loc_array2);
				ex_name="Multiply to array";
				RTRN(v1);
			};
			return v1;
		};
	};
	syntax_error("wrong multiply type",2101);
	set_break();
	return(1);
}

static double term1_div_num(double v1)
{
 NTOKEN2;
 double v2=num_term2();
	if(v2!=0){
		v1=v1/v2;
		ex_var.dval=v1;
		return v1;
	};
	/* RT error  */
		syntax_error("Division by zero",212);
		v1 = 9999999999.9;
		RTRN(v1);
}

static double term1_div(double v1)
{
 double v2;
 tok_struct *ptok=tok;
	if(vtype_is(VTYPE_NUM)){
		NTOKEN2;
		v2=num_term2();
			switch(get_vtype())
			{
				case VTYPE_NUM:	// numeric * numeric
					set_term_function(ptok,term1_div_num);
					if(v2!=0){
					v1=v1/v2;
					ex_var.dval=v1;
					return v1;
					};
					/* RT error  */
					syntax_error("Division by zero",212);
					v1 = 9999999999.9;
					RTRN(v1);
				case VTYPE_ARRAY:	// numeric * array
					set_array(dup_array_mul1(get_array("15"),1/v1));
					ex_name = "numeric / array";
					return 1;
			};
	};
	if(vtype_is(VTYPE_ARRAY)){
		array_dat *loc_array = get_array("16");
		NTOKEN2;
		v2 = num_term2();
		if(v2==0) {
			/* RT error  */
			syntax_error("Division by zero",215);
			v1 = 9999999999.9;
			RTRN(v1);
		};

		if(vtype_is(VTYPE_NUM)) {
			if(loc_array->astat==ARRAY_LOCAL) loc_array=dup_array_mul1(loc_array,1/v2);
			else array_mul1(loc_array,1/v2);
			set_array(loc_array);
			// set_vtype(VTYPE_ARRAY);
			ex_name="array / numeric";
			return 1;
		};
		if(vtype_is(VTYPE_ARRAY)) {
				ERROR("div arrays not implemmented yet!");
				RTRN(v1);
		};
	};
	set_error(tok,216,"division op not supported");
	set_break();
	RTRN(v1);
}


double factor_rcurl(){
	// MESG("RCURL");
	NTOKEN2;
	lstoken=NULL;
	// current_active_flag=0;
	return ex_var.dval;
}

double factor_sep(){
	MESG("factor_sep:");
	NTOKEN2;
	// if(tok->ttype==TOK_VAR) lstoken=tok;
	// else 
	lstoken=NULL;
	return 0.0;
}

double factor_comma(){
	// MESG("factor_comma:");
	NTOKEN2;
	lstoken=NULL;
	return 0.0;
}

double factor_eof(){
	// MESG("factor_eof!!!!");
	current_active_flag=0;
	return 0.0;
}

double factor_error()
{
	syntax_error("factor_error",2081);
	NTOKEN2;
	set_break();
	return 0.0;
}

static inline double term_plus_num(double value)
{
	NTOKEN2;
	return value+num_term1();
}

double term_plus(double value)
{
 double d1;
 tok_struct *ptok=tok;
 // MESG("term_plus: value=%f tok type=%d",value,ptok->ttype);
 int vtype1=get_vtype();
 int vtype2=-1;
 if(vtype1==VTYPE_NUM) {
	NTOKEN2;
	d1=num_term1();
	vtype2=get_vtype();
	// MESG("term_plus");
	if(vtype2==VTYPE_NUM) {
		set_term_function(ptok,term_plus_num);
		return value+d1;
	};
	if(vtype2==VTYPE_STRING) {
		char svalue[MAXLLEN];
		int stat;
		stat=snprintf(svalue,sizeof(svalue),"%f%s",value,get_sval());
		if(stat>MAXLLEN) MESG("truncated 2");
		set_sval(svalue);
		return 0;
	};
	if(vtype2==VTYPE_ARRAY) { // num + array
		array_dat *array1 = get_array("18");
		// MESG("term_plus: numeric + array");
		if(array1->astat==ARRAY_LOCAL) {
			set_array(dup_array_add1(array1,value));
			ex_name="New array,add to numeric";
		} else {
			array_add1(array1,value);
			ex_name=tok->tname;
		};
		set_vtype(VTYPE_ARRAY);
		ex_name="Add to numeric";
		// MESG("tem_plus: end ok!");
		return 0;
	};
	err_num=217;
	err_line=tok->tline;
	ERROR("operation not supported err %d",err_num);
	RTRN(0);
 };

 if(vtype1==VTYPE_STRING) {	// set local value
	char svalue[MAXLLEN];
	strlcpy(svalue,get_sval(),MAXLLEN);

		NTOKEN2;
		d1=num_term1();
		vtype2=get_vtype();
		 /* catanate string */
			if(vtype2==VTYPE_STRING) {	/* string catenate  */
				strlcat(svalue,get_sval(),sizeof(svalue));
				value=atof(svalue);
				set_sval(svalue);
				return value;
			};
			if(vtype2==VTYPE_NUM)	{	/* string, numeric catanate  */
				long l0;
				set_vtype(VTYPE_STRING);
				l0=d1;
				// MESG("s+n %s ",get_sval());
				clean_saved_string(80);
				if(l0==d1)snprintf(saved_string,80,"%ld",l0);
				else snprintf(saved_string,80,"%f",d1);
				// MESG("s+n saved_string [%s] svalue=[%s]",saved_string,svalue);
				strlcat(svalue,get_sval(),sizeof(svalue));
				value=0;
				set_sval(svalue);
				return value;
			};
			if(vtype2==VTYPE_ARRAY) {
				err_num=218;
				ERROR("Add array to string not supported err %d",err_num);
				RTRN(value);				
			};
			err_num=219;
			ERROR("operation not supported err %d",err_num);
			RTRN(value);				
 };

 if(vtype1==VTYPE_ARRAY) {
  	array_dat *loc_array=get_array("20");
		 		NTOKEN2;
				d1=num_term1(); vtype2=get_vtype();
				if(vtype2==VTYPE_NUM) { // add numeric to array
					MESG("	array + numeric");
					if(loc_array->astat==ARRAY_LOCAL) {
						array_dat *array1;
						array1=dup_array_add1(loc_array,d1);
						set_array(array1);
						print_array1("after a+num",array1);
					} else {
						array_add1(loc_array,d1);
					};
					set_vtype(VTYPE_ARRAY);
					ex_name="Add to numeric";
					MESG("	array + numeric: ok!");
					return 0;
				} else if(vtype2==VTYPE_ARRAY) {	// array addition
					array_dat *loc_array2;
					loc_array2=array_add2(loc_array,get_array("21"));
					set_vtype(VTYPE_ARRAY);
					set_array(loc_array2);
					ex_name="Add to array";
					RTRN(value);
				}
#if	1
				 else if(vtype2==VTYPE_STRING) {
				 	MESG("add string [%s] to an array",get_sval());
					ERROR("operation not_supported err %d vtype1=%d vtype2=%d",err_num,vtype1,vtype2);
					return value;
				}
#endif 
				else {
					err_num=220;
					ERROR("operation not_supported err %d vtype1=%d vtype2=%d",err_num,vtype1,vtype2);
					RTRN(value);
				};		
 };

#if	1
 if(vtype1==VTYPE_AMIXED) {
  	array_dat *loc_array=get_array("20");
		 		NTOKEN2;
				d1=num_term1();vtype2=get_vtype();
				if(vtype2==VTYPE_NUM) { // add numeric to array
					MESG("	mixed array + numeric");
					if(loc_array->astat==ARRAY_LOCAL) {
						array_dat *array1;
						array1=dup_array_add1(loc_array,d1);
						set_array(array1);
						print_array1("after a+num",array1);
					} else {
						array_add1(loc_array,d1);
					};
					set_vtype(VTYPE_ARRAY);
					ex_name="Add to numeric";
					MESG("	array + numeric: ok!");
					return 0;
				} else if(vtype2==VTYPE_ARRAY) {	// array addition
					array_dat *loc_array2;
					loc_array2=array_add2(loc_array,get_array("21"));
					set_vtype(VTYPE_ARRAY);
					set_array(loc_array2);
					ex_name="Add to array";
					RTRN(value);
				} else {
					err_num=221;
					ERROR("operation not_supported err %d vtype1=%d vtype2=%d",err_num,vtype1,vtype2);
					RTRN(value);
				};		
 };
#endif

	// ERROR("term_plus: operation not supported!, vtype=%d line %d",ex_var.var_type,tok->tline);
	NTOKEN2;
	ERROR("operation not_supported err %d vtype1=%d vtype2=%d",err_num,vtype1,vtype2);
	set_error(tok,222,"operation not supported");
 return 0;
}

double term_minus_num(double value)
{
	NTOKEN2;
	return value-num_term1();
}

double term_minus(double value)
{
 double d1;
 if(vtype_is(VTYPE_NUM)) {	/* numeric  !!  */
	tok_struct *ptok=tok;
	NTOKEN2;
	d1=num_term1();

	if(vtype_is(VTYPE_NUM)) {
		set_term_function(ptok,term_minus_num);
		return value-d1;
	};
	if(vtype_is(VTYPE_ARRAY)) {
		array_dat *array1=get_array("22");
		if(array1->astat==ARRAY_LOCAL) {
			set_array(dup_array_sub1(array1,value));
			ex_name="New array,subtract from numeric";
		} else {
			array_sub1(array1,value);
			ex_name="Subtract from numeric";
		};
		set_vtype(VTYPE_ARRAY);
		return 0;
	};
	err_num=215;
	err_line=tok->tline;
	ERROR("operation not supported err %d",err_num);
	RTRN(value);
 };
 if(vtype_is(VTYPE_STRING)) {	// set local value
	char svalue[MAXLLEN];
	strlcpy(svalue,get_sval(),MAXLLEN);
		NTOKEN2;
		// operator on first chars of strings. numeric result
			d1=num_term1();
			if(vtype_is(VTYPE_STRING)) {
				value=svalue[0]-saved_string[0];
			} else {
				value=svalue[0]-d1;
			};
			set_vtype(VTYPE_NUM);
			return value;
 };
 if(vtype_is(VTYPE_ARRAY)) {
  	array_dat *loc_array=get_array("25");
	NTOKEN2;
	d1=num_term1();
	if(vtype_is(VTYPE_NUM)) {	// subtruct numeric from array
		if(loc_array->astat==ARRAY_LOCAL) {
			loc_array=dup_array_add1(loc_array,-d1);
			ex_name="new subtruct numeric";
		} else {
			array_add1(loc_array,-d1);
			ex_name="subtruct numeric";
		};
		// set_vtype(VTYPE_ARRAY);
		set_array(loc_array);
		return(value);
	} else if(vtype_is(VTYPE_ARRAY)) {	// array subtruction
		array_dat *loc_array2;
		loc_array2=array_sub2(loc_array,get_array("26"));
		// set_vtype(VTYPE_ARRAY);
		set_array(loc_array2);
		ex_name="Subtruct from array";
		RTRN(value);
	} else {
		err_num=221;
		ERROR("Subtract strings not_supported err %d",err_num);
		RTRN(value);
	};		
 };
 return 0;
}

/* power and modulo */
static inline double num_term2()
{
 TDS("num_term2");
 // MESG("			num_term2: [%s]",tok_info(tok));
 double v1 = FACTOR_FUNCTION;
 // MESG("				factor function executed! v1=%f",v1);
	 while(tok->tgroup==TOK_TERM2)
	 {
		// MESG("while: TERM2");
		v1 = tok->term_function(v1);
	 };
 // MESG("			term2 end %f",v1);
 RTRN(v1);
}

/* multiply & divide */
static inline double num_term1()
{
 TDS("num_term1");
 // MESG("		num_term1: [%s]",tok_info(tok));
 double v1 = num_term2();
	 while(tok->tgroup==TOK_TERM1)
	 {
		// MESG("while: TERM1");
		v1 = tok->term_function(v1);
		// if(err_num) break;
	 };
	 // ex_var.dval=v1;
 	// MESG("		num_term1: end %f",v1);
 RTRN(v1);
}

// ------ bool functions
double logical_term2()
{
 double v1 = FACTOR_FUNCTION > 0;
	while(tok->ttype==TOK_AND||tok->ttype==TOK_NAND) {
		v1 = tok->term_function(v1);
	};
	return v1;
}

double logical_term1()
{
 double v1 = logical_term2();
 while(tok->ttype==TOK_OR || tok->ttype==TOK_NOR) {
 	v1 = tok->term_function(v1);
 };
 return v1;
}

double logical_or(double value)
{
 // MESG("<< logical or: %s",tok_info(tok));
 NTOKEN2;
 int v2 = logical_term1() > 0;
 int ires=(value>0) | v2;

 set_dval((double) ires);
 return ires;
}

double logical_xor(double value)
{
 NTOKEN2;
 int v2 = logical_term1() > 0;
 int ires=(value>0) ^ v2;

 set_dval((double) ires);
 return ires;
}

double logical_nor(double value)
{
 NTOKEN2;
 int v2 = logical_term1() > 0;
 int ires=!((value>0) || v2);

 set_dval((double) ires);
 return ires;
}

double logical_and(double value)
{
 // MESG("<< logical and: %s",tok_info(tok));
 NTOKEN2;
 int v2 = FACTOR_FUNCTION > 0;
 // NTOKEN2;
 // MESG("-- logical and: next is [%s] res=%d",tok_info(tok),v2);
 int ires=(value>0) & v2;
 set_dval((double) ires);
 MESG(">> logical_and: %f & %d -> %d",value,v2,ires);
 	return ires;
}

double logical_nand(double value)
{
 NTOKEN2;
 int v2 = FACTOR_FUNCTION > 0;
 int ires=!((value>0) && v2);

 set_dval((double) ires);
 return ires;
}

double assign_option(double none)
{
double	value=lexpression();
	// MESG("assign_option:");
	if(var_node->node_vtype==VTYPE_STRING) {free(var_node->node_sval);};
	if(vtype_is(VTYPE_STRING)){
		var_node->node_vtype=VTYPE_STRING;
		if(get_sval()) var_node->node_sval=strdup(get_sval());
		else {
			err_num=2221;
			ERROR("error: saved string is Null string!! ",err_num);
			var_node->node_sval=strdup("");
		};
	} else {
		// CHECK !!!!!!!!
		var_node->node_vtype=VTYPE_NUM;
		var_node->node_dval=value;
	}
		
	RTRN(value);
}

// ----- compare functions

static inline double num_smaller(double v1)
{
 NTOKEN2;
 double v2=num_expression();
 return v1<v2 ? 1.0: 0.0;
}

static inline double num_notequal(double v1)
{
 NTOKEN2;
 double v2=num_expression();
 return v1!=v2 ? 1.0: 0.0;
}

static inline double num_equal(double v1)
{
 NTOKEN2;
 double v2=num_expression();
 return v1==v2 ? 1.0: 0.0;
}

static inline double num_smallereq(double v1)
{
 NTOKEN2;
 double v2=num_expression();
 return v1<=v2 ? 1.0: 0.0;
}

static inline double num_biggereq(double v1)
{
 NTOKEN2;
 double v2=num_expression();
 return v1>=v2 ? 1.0: 0.0;
}

static inline double num_bigger(double v1)
{
 NTOKEN2;
 double v2=num_expression();
 return v1>v2 ? 1.0: 0.0;
}

double compare_smaller(double v1)
{
 tok_struct *tok0=tok;
 int vtype1=get_vtype();
 static char svalue[MAXLLEN];
 NTOKEN2;
 if(vtype1==VTYPE_STRING) {
	strlcpy(svalue,get_sval(),MAXLLEN);
 };
 double v2=num_expression();
 int vtype2=get_vtype();
 if(vtype1==VTYPE_STRING && vtype2==VTYPE_STRING) {
		set_vtype(VTYPE_NUM);
		int lresult=scmp(svalue,get_sval());
		return (lresult < 0 ? 1.0: 0.0);
 } else 
 if(vtype1==VTYPE_NUM && vtype2==VTYPE_NUM) {
		MESG("change smaller function!");
		tok0->term_function = num_smaller;
		return v1 < v2 ? 1.0: 0.0;
 };
	syntax_error("comparison error",223);
	MESG("comparison error type1=%d type2=%d",vtype1,vtype2);
	set_vdval(0);
	return 0;
}

double compare_notequal(double v1)
{
 tok_struct *tok0=tok;
 int vtype1=get_vtype();
 static char svalue[MAXLLEN];
 NTOKEN2;
 if(vtype1==VTYPE_STRING) {
	strlcpy(svalue,get_sval(),MAXLLEN);
 };
 double v2=num_expression();
 int vtype2=get_vtype();
 if(vtype1==VTYPE_STRING && vtype2==VTYPE_STRING) {
		set_vtype(VTYPE_NUM);
		int lresult=scmp(svalue,get_sval());
		return (lresult != 0 ? 1.0: 0.0);
 } else 
 if(vtype1==VTYPE_NUM && vtype2==VTYPE_NUM) {
		MESG("change notequal function!");
		tok0->term_function = num_notequal;
		return v1 != v2 ? 1.0: 0.0;
 };
	syntax_error("comparison error",223);
	MESG("comparison error type1=%d type2=%d",vtype1,vtype2);
	set_vdval(0);
	return 0;
}


double compare_bigger(double v1)
{
 tok_struct *tok0=tok;
 int vtype1=get_vtype();
 static char svalue[MAXLLEN];
 NTOKEN2;
 if(vtype1==VTYPE_STRING) {
	strlcpy(svalue,get_sval(),MAXLLEN);
 };
 double v2=num_expression();
 int vtype2=get_vtype();
 if(vtype1==VTYPE_STRING && vtype2==VTYPE_STRING) {
		set_vtype(VTYPE_NUM);
		int lresult=scmp(svalue,get_sval());
		return (lresult > 0 ? 1.0: 0.0);
 } else 
 if(vtype1==VTYPE_NUM && vtype2==VTYPE_NUM) {
		MESG("change bigger function!");
		tok0->term_function = num_bigger;
		return v1 > v2 ? 1.0: 0.0;
 };
	syntax_error("comparison error",223);
	MESG("comparison error type1=%d type2=%d",vtype1,vtype2);
	set_vdval(0);
	return 0;
}

double compare_equal(double v1)
{
 tok_struct *tok0=tok;
 int vtype1=get_vtype();
 static char svalue[MAXLLEN];
 NTOKEN2;
 if(vtype1==VTYPE_STRING) {
	strlcpy(svalue,get_sval(),MAXLLEN);
 };
 double v2=num_expression();
 int vtype2=get_vtype();
 if(vtype1==VTYPE_STRING && vtype2==VTYPE_STRING) {
		set_vtype(VTYPE_NUM);
		int lresult=scmp(svalue,get_sval());
		return (lresult == 0 ? 1.0: 0.0);
 } else 
 if(vtype1==VTYPE_NUM && vtype2==VTYPE_NUM) {
		MESG("change equal function! v1=%f v2=%f",v1,v2);
		tok0->term_function = num_equal;
		return v1 == v2 ? 1.0: 0.0;
 };
	syntax_error("comparison error",223);
	MESG("comparison error type1=%d type2=%d",vtype1,vtype2);
	set_vdval(0);
	return 0;
}

double compare_smallereq(double v1)
{
 tok_struct *tok0=tok;
 int vtype1=get_vtype();
 static char svalue[MAXLLEN];
 NTOKEN2;
 if(vtype1==VTYPE_STRING) {
	strlcpy(svalue,get_sval(),MAXLLEN);
 };
 double v2=num_expression();
 int vtype2=get_vtype();
 if(vtype1==VTYPE_STRING && vtype2==VTYPE_STRING) {
		set_vtype(VTYPE_NUM);
		int lresult=scmp(svalue,get_sval());
		return (lresult <= 0 ? 1.0: 0.0);
 } else 
 if(vtype1==VTYPE_NUM && vtype2==VTYPE_NUM) {
		MESG("change smaller equal function!");
		tok0->term_function = num_smallereq;
		return v1 <= v2 ? 1.0: 0.0;
 };
	syntax_error("comparison error",223);
	MESG("comparison error type1=%d type2=%d",vtype1,vtype2);
	set_vdval(0);
	return 0;
}

double compare_biggereq(double v1)
{
 tok_struct *tok0=tok;
 int vtype1=get_vtype();
 static char svalue[MAXLLEN];
 NTOKEN2;
 if(vtype1==VTYPE_STRING) {
	strlcpy(svalue,get_sval(),MAXLLEN);
 };
 double v2=num_expression();
 int vtype2=get_vtype();
 if(vtype1==VTYPE_STRING && vtype2==VTYPE_STRING) {
		set_vtype(VTYPE_NUM);
		int lresult=scmp(svalue,get_sval());
		return (lresult >= 0 ? 1.0: 0.0);
 } else 
 if(vtype1==VTYPE_NUM && vtype2==VTYPE_NUM) {
		MESG("change bigger equal function!");
		tok0->term_function = num_biggereq;
		return v1 >= v2 ? 1.0: 0.0;
 };
	syntax_error("comparison error",223);
	MESG("comparison error type1=%d type2=%d",vtype1,vtype2);
	set_vdval(0);
	return 0;
}

// ---- assignment functions

double assign_env(double none)
{
	int left_index;
	// MESG("assign_env:");
	left_index=var_node->node_index;
	// MESG("assign_env: left_index=%d",left_index);
	double v1=lexpression();
	set_env(left_index,get_sval(),v1);
	var_node=NULL;
	return(v1);
}

double assign_val_num(double none)
{
	MVAR *sslot=lsslot;
	// tok_struct *tok0=tok;
	double v1=lexpression();
	sslot->dval=v1;
	set_dval(v1);
	// MESG("assign_val_num: %s to %f",tok0->tname,v1);
	return(v1);
}

double assign_val(double none)
{
	// int stype=get_vtype();
	TDS("assign_val");
	
	// MESG("assign_val: lsslot  [%s] vtype=%d ex_vtype=%d",lstoken->tname,lsslot->var_type);
	// MESG("assign_val: tok type=%d ind=%d",tok->ttype,tok->tind);
	tok_struct *ptok = tok-1;
	// MESG("assign_val: tok type=%d ind=%d",ptok->ttype,ptok->tind);
	
	tok_struct *lstok=lstoken;
	MVAR *sslot=lsslot;
	double v1=lexpression();
	// MESG("assign_val: after lexpression! slot vtype=%d ex_vtype=%d\n",sslot->var_type,get_vtype());
	if(vtype_is(sslot->var_type) && vtype_is(VTYPE_NUM)) 
	{
		set_term_function(ptok,assign_val_num);
		sslot->dval=v1;
		set_vdval(v1);
		// MESG("assign_val: %s to %f",lstok->tname,v1);
		return(v1);
	};

	// int stype=sslot->var_type;;
	// MESG("assign_val: check diffs! %d %d %d",get_vtype(),stype,sslot->var_type);
	if(!vtype_is(sslot->var_type)){
		// MESG("assign_val: different vtype %d != sslot_vtype %d",get_vtype(),sslot->var_type);
		if(vtype_is(VTYPE_AMIXED)) {
			// MESG("	mixed array!!!!");
			array_dat *adat = get_array("type");
			if(adat) {
				// MESG("		named [%s]",adat->array_name);
				if(adat->var_tree) {
					// MESG("	var_tree: %s",adat->var_tree->tree_name);
					if(tok->ttype==TOK_TYPE_ELEMENT) {
						// MESG("  next token is %d",tok->ttype);
						v1=factor_type_element();
						lsslot->var_type=get_vtype();
						if(vtype_is(VTYPE_NUM)) lsslot->dval=v1;
						else lsslot->sval=strdup(get_sval());
						return v1;
					};
				};
			};
		};
		if(sslot->var_type==VTYPE_STRING) {
			if(sslot->sval) free(sslot->sval);
			sslot->sval=NULL;
			if(vtype_is(VTYPE_NUM)) {
				// MESG("set new as num");
				sslot->dval=v1;
				sslot->var_type=VTYPE_NUM;
				return(v1);
			};
		} else {
			if(sslot->var_type!=VTYPE_ARRAY && sslot->var_type!=VTYPE_SARRAY)	{/* added to handle arrays (v698l) but CHECK!!!!  */
				if(sslot->var_type==VTYPE_AMIXED) {
					// MESG("	AMIXED !");
					// MESG("	AMIXED we are here! %f",*ls_pdval);
					// if(stype!=lmvar->var_type) MESG("	different subtype!!!");
					if(lmvar->var_type==VTYPE_STRING) free(lmvar->sval);
					lmvar->var_type=get_vtype();
					if(vtype_is(VTYPE_STRING)) {
						lmvar->sval=strdup(get_sval());					
					};
					if(vtype_is(VTYPE_NUM)) {
						lmvar->dval=v1;
					}
					return (v1);
				};
				// sslot->var_type=get_vtype();
			};
			if(vtype_is(VTYPE_NUM)) {
				// MESG("set new as num");
				*ls_pdval=v1;
				// sslot->var_type=VTYPE_NUM;
				return(v1);
			};
			if(vtype_is(VTYPE_STRING)) {
				sslot->sval=strdup(get_sval());
				/* !!!!! to change type beware!!!!!!  */
				if(sslot->var_type!=VTYPE_SARRAY)
					sslot->var_type=VTYPE_STRING;
				// MESG("set string!");
				return(0);
			};
		};
			if(vtype_is(VTYPE_ARRAY) || vtype_is(VTYPE_SARRAY)||vtype_is(VTYPE_AMIXED)) {
				// MESG("assign array to var!");
				sslot->adat=get_array("27");
				sslot->var_type=sslot->adat->atype;
				sslot->adat->array_name=strdup(lstok->tname);
				if(sslot->adat->astat==ARRAY_ALLOCATED) sslot->adat->astat=ARRAY_LOCAL;
			};
		return(v1);		
	} else {
		// MESG("	same type!");
		if(vtype_is(VTYPE_ARRAY) || vtype_is(VTYPE_SARRAY)) {
			array_dat *array1 = get_array("32");
			if(array1->anum != sslot->adat->anum) {
 				if(sslot->adat->dval) free_array("assign",sslot->adat);
				if(sslot->adat->sval) {
					// MESG("free string array!");
				};
				sslot->adat=array1;
				if(array1->astat==ARRAY_ALLOCATED) array1->astat=ARRAY_LOCAL;	/* make it local to variable  */
			};
			return(v1);
		};
#if	0
		if(vtype_is(VTYPE_AMIXED)) {
			MESG("deep copy mixed array!");
			return(v1);
		};
#endif
		if(vtype_is(VTYPE_STRING)) {
			free(sslot->sval);
			sslot->sval=strdup(get_sval());
			return(v1);
		};
		return(v1);
	};
}


double lexpression()
{
 TDS("lexpression");
 //MESG("# lexpression:0 start [%s]",tok_info(tok));
 double value = cexpression();
 // MESG("  lexpression :1 		[%s] cexpression result = %f",tok_info(tok),value);
 if(tok->tgroup == TOK_TERM0){
 	// MESG("  lexpression:1a term0! group=%d %d %d",tok->tgroup,TOK_BOOL,TOK_TERM0);
	tok_struct *tok0=tok;
 	NTOKEN2;
	RTRN(tok0->term_function(value));
 };
 // MESG("  lexpression:2 start loop --- %s",tok_info(tok));
 while(tok->tgroup==TOK_BOOL){
	// MESG("while: BOOL");
	tok_struct *tok0=tok;
	// NTOKEN2
	// MESG(" -lexpression:3 in loop! [%s] value=%f",tok_info(tok),value);
	value =tok0->term_function(value);
	// MESG(" -lexression:4 in loop, result is %f -> %f",value,value1);
	// NTOKEN2;
	// set_vtype(VTYPE_NUM);
	// value=value1;
	// set_vdval(value);
 };
 
	// MESG("! lexpression:4 return value %f [%s]",value,tok_info(tok));	
	RTRN(value);
}

// Directive functions

static double inline dir_lcurl()
{
	NTOKEN2;
	return exec_block1(exe_buffer);
}

static double inline dir_lcurl_break()
{
	NTOKEN2;
	// MESG("dir_lcurl_break:");
	return exec_block1_break(exe_buffer);
}

static double inline dir_break()
{
	NTOKEN2;
	current_active_flag=0;
	return 0;
}

static double inline dir_return()
{	double val=0;
	NTOKEN2;
	if(tok->ttype==TOK_SEP || tok->ttype==TOK_RPAR) { val=0;set_vtype(VTYPE_NUM);}
	else val=lexpression();
	current_active_flag=0;	/* skip rest of function  */
	return(val);
}

void set_tok_directive(tok_struct *tok, FFunction directive)
{
	tok->directive = directive;
	// MESG_TOK_INFO("# tok_directive",tok);
}

void set_term_function(tok_struct *tok, TFunction term_function)
{
	tok->term_function = term_function;
	// MESG_TOK_INFO(" term",tok);
}

double exec_function(FILEBUF *proc_buffer,int nargs)
{
	// MESG("exec_function: [%s]",proc_buffer->b_fname);
#if	TBNF
	if(usebnf) {
		bnf_exec_function(proc_buffer,nargs);
		// MESG("after bnf_exec_function: var@=%d type=%d",VARIND,bnf_var->var_type);
		if(bnf_var->var_type==VTYPE_NUM) {
			// MESG("	numeric val=%f",bnf_var->dval);
			return bnf_var->dval;
		};
		return 1;
	};
#endif
	MVAR *old_symbol_table=current_stable;
	current_stable = push_args_1(nargs,proc_buffer->symbol_tree->items);
	tok_struct *after_proc=tok;
	
	tok=proc_buffer->tok_table;	/* start of function  */
	skip_args1(nargs);
	double value=tok->directive();
	delete_symbol_table(current_stable,proc_buffer->symbol_tree->items,nargs);
	current_stable=old_symbol_table;
	tok=after_proc;
	return value;
}


/* numerical only expression  */
static inline double num_expression()
{
 double value;
 TDS("num_expression");
 // MESG(";	num_expression: [%s]",tok_info(tok));
 // set_vdval(0);
 value = num_term1();
 while(tok->tgroup==TOK_TERM) {
	// MESG("	num_expression: while: TERM [%s]",tok_info(tok));
	value = tok->term_function(value);
 };
 if(vtype_is(VTYPE_NUM)) ex_var.dval=value;	// this is needed!!
 // MESG(";num_expression:end ttnum=%d type=%d return value=%f slval=[%s]",tok->tnum,get_vtype(),value,get_sval());
 return value;
}


MVAR * push_args_1(int nargs,int vars_num)
{
 TDS("push_args");

 MVAR *va = new_symbol_table(vars_num);
 if(va==NULL) return NULL;

 MVAR *va_i=va;
 for(;va_i<va+nargs;va_i++)
 {
	// MESG("	push_args_1: arg %d, tok=[%d %s] value=%f type=%d",i,tok->tnum,tok->tname,value,va_i->var_type);
	// MESG("	push_args_1: arg %d, tok=[%d %s] value=%f type=%d",i,tok->tnum,tok->tname,value,va_i->var_type);
	// MESG(";		i=%d, var_type = %d",i,va_i->var_type);
	double value = num_expression();
	va_i->var_type=get_vtype();
	if(vtype_is(VTYPE_NUM)) {
			// MESG("	arg:%d numeric %f",i,value);
			// va_i->dval=ex_var.dval;
			va_i->dval=value;
	} else
	if(vtype_is(VTYPE_STRING)) {
		// MESG("	arg:%d string [%s]",i,get_sval());
		va_i->sval=strdup(get_sval());
		clean_saved_string(0);
	} 
#if	0
	else 
	if(vtype_is(VTYPE_ARRAY)||vtype_is(VTYPE_SARRAY)||vtype_is(VTYPE_AMIXED)) {
			va_i->adat=get_array("1");
			// MESG("	arg:%d array type %d",i,va_i->adat->atype);
			// MESG("	pushed array type=%d stat=%d",va_i->var_type,va_i->adat->astat);
	}
#endif
	else {
		// just copy the pointer to real data
		memcpy(va_i,&ex_var,sizeof(MVAR));
	}
	// MESG("	push_args_1: arg %d, tok=[%d %s] value=%f type=%d",i,tok->tnum,tok->tname,value,va_i->var_type);
	NTOKEN2; // skip separator or right parenthesis!
 };

 // init_vars(va_i,vars_num);	/* need to initialize ??  */
 // MESG(">	push_args_1:end [%s]",tok_info(tok));
 return(va);
}

double cexpression()
{
 TDS("cexpression");
 // MESG(";cexpression [%s]",tok_info(tok));
 double value = num_expression();

 if(tok->tgroup!=TOK_COMPARE) RTRN(value);
 else return 
	tok->term_function(value);	// compare function
}



double tok_dir_type()
{
	// MESG("dir_type");
	NTOKEN2;
	return 0;
}

double tok_dir_if()
{
	tok_struct *tok0=tok;
	// MESG("tok_dir_if: n=%d",tok->tnum);
	NTOKEN2;	/* go to next token after if */

	double val=lexpression();
	if(val) {
		// MESG("err_if: ttype=%d tnum=%f",tok->ttype,tok->tnum);
		NTOKEN2;	/* skip right parenthesis  */
		// MESG("	execute if at %d",tok->tnum);
		val=tok->directive();
		// MESG("tok_dir_if: num=%d",tok->tnum);
		if(tok->ttype==TOK_DIR_ELSE) {
			tok=tok->next_tok;
			// MESG("skip else up to %d",tok->tnum);
		};
		return val;
	} else {
		tok=tok0->next_tok;
		// MESG("	execute else at %d",tok->tnum);
		if(check_skip_token1(TOK_DIR_ELSE)) {
			val=tok->directive();
		};
	}
	return(val);
}

double tok_dir_for()
{
	tok_struct *check_element; // check element pointer
	tok_struct *loop_element;
	tok_struct *start_block;	// element at block start
	tok_struct *end_block=NULL;	/* at the block end  */
	int old_active_flag=current_active_flag;

//	MESG("-- start for loop: active = %d",current_active_flag);	
	NTOKEN2;	/* go to next token after for */

	lexpression();	/* initial   */
	NTOKEN2;	/* skip separator! */
	// set check_list
	check_element=tok;

	skip_sentence1();	/* skip check element  */
	// set loop_list
	loop_element=tok;
	// MESG("for loop element  [%s]",tok_info(loop_element));
	skip_sentence1();	/* skip loop element  */

	NTOKEN2;	/* skip right parenthesis  */
	// set block start
	start_block=tok;

	// find token after the end of block
	skip_sentence1();
	end_block=tok;

	// while(!is_break1) 
	while(current_active_flag)
	{
		double val;
		// check expression
		tok=check_element;
		val=lexpression();

		if(val) {
			tok=start_block;
			tok->directive();
			tok=loop_element;
			val=lexpression();	/* exec for loop  */
		} else {
			break;
		};
//		MESG("before next loop: val=%f",val);		
	};
	if(is_break1) { tok=exe_buffer->end_token;return(0);};
	tok=end_block;
	current_active_flag=old_active_flag;
//	MESG("-- end for loop: active = %d",current_active_flag);	
	return 1;
}

double tok_dir_fori()
{
	tok_struct *start_block;	// element at block start
	tok_struct *end_block=NULL;	/* at the block end  */
	int old_active_flag=current_active_flag;
	MVAR *index=NULL;
	double dinit,dmax,dstep;
	double *iterrator_val;

	NTOKEN2;	/* go to next token after for */

	index=&current_stable[tok->tind];
	if(index->var_type!=VTYPE_NUM) {err_num=224;ERROR("for i syntax error %d",err_num);};
	set_vtype(index->var_type);

	NTOKEN2; // next
	NTOKEN2; // skip equal sign

	dinit=num_expression();	/* initial   */
	iterrator_val=&index->dval;
	*iterrator_val=dinit;
	NTOKEN2;	/* skip separator! */

	dmax=num_expression();
	NTOKEN2;

	dstep=num_expression();
	// MESG("fori: from %f to %f step %f",dinit,dmax,dstep);
	//if(tok->ttype!=TOK_RPAR) { err_num=226;ERROR("for i: error %d",err_num);};
	NTOKEN2;	/* skip right parenthesis  */
	// set block start
	start_block=tok;	/* this is a block start or a simple sentence  */

	// find token after the end of block
	skip_sentence1();
	end_block=tok;

	if(dinit==dmax) {
		tok=end_block;
		current_active_flag=old_active_flag;
		return 1;
	};
	if(dstep>0 && dmax > *iterrator_val) {

		for(;*iterrator_val < dmax; *iterrator_val +=dstep) {
			tok=start_block;
			tok->directive();
			if(current_active_flag==0) {
				if(is_break1) { tok=exe_buffer->end_token;return(0);};
				break;
			};
		};
	} else if(dstep<0 && dmax< *iterrator_val) {
		for(; *iterrator_val > dmax; *iterrator_val +=dstep) {
			tok=start_block;
			tok->directive();
			if(current_active_flag==0) {
				if(is_break1) { tok=exe_buffer->end_token;return(0);};
				break;
			};
		};
	} else {
		err_num=226;
		ERROR("error: infinite fori loop %d",err_num);
	};
	tok=end_block;
	current_active_flag=old_active_flag;
	return 1;
}

double tok_dir_while()
{
	tok_struct *check_element; // check element pointer
	tok_struct *start_block;	// element at block start
	tok_struct *end_block=NULL;	/* at the block end  */
	int old_active_flag=current_active_flag;
	// MESG("tok_dir_while:");
	NTOKEN2;	/* go to next token after while */

	check_element=tok;	/* this is the check element!  */
	skip_sentence1();	/* for now skip it  */

	NTOKEN2;	/* skip right parenthesis  */

	// this is the start of a simple sentence or a curl
	start_block=tok;

	// find token after the end of block
	skip_sentence1();
	end_block=tok;

	// set tok pointer here
	drv_start_checking_break();
	do {
		// set tlist to tok pointer
		tok=check_element;

		if(lexpression()) {
			// on the block start
			tok=start_block;
			tok->directive();
			if(current_active_flag==0) {	/* only after break  */
				// if(is_break1) { tok=tok->fp->end_token;return(0);};
				break;
			};
		} else {
			break;
		};

	} while(1); 
	drv_stop_checking_break();
	tok=end_block;	/* to the end of executable block  */
	current_active_flag=old_active_flag;
	return 1;
}

FFunction factor_funcs[] = {
	factor_none,	// TOK_NONE
	factor_sep,		// TOK_SEP
	factor_none,	// TOK_SPACE
	factor_none,	// TOK_LETTER
	factor_none,	// TOK_LCURL	,
	factor_rcurl,	// TOK_RCURL	,
	factor_quote,	// TOK_QUOTE
	factor_lpar,	// TOK_LPAR
	factor_error,	// TOK_RPAR	
	factor_refresh_ddot,	// TOK_SHOW
	factor_none,	// TOK_COMMENT	,
	factor_variable,	// TOK_VAR	level 0 variable
	factor_option,	// TOK_OPTION	,	// editor option
	factor_cmd,		// TOK_CMD		,	// editor commands
	factor_none,	// TOK_FUNC	,	// function
	factor_proc,	// TOK_PROC	,
	factor_env,		// TOK_ENV		,	// editor environment function
	factor_none,	// TOK_TERM0	term0 group
	factor_none,	// TOK_TERM	,	// term operators (+,-)
	factor_none,	// TOK_TERM1	,	// term1 operators (%,^)
	factor_none,	// TOK_TERM2	,	// term2 operators (*,/)
	factor_none,		// TOK_ASSIGN	,	// assignment
	factor_eof,		// TOK_EOF		,	// end of file token
	factor_num,		// TOK_NUM, numeric value

	factor_none,	// TOK_DIR		,	// directive
	factor_none,	// TOK_DIR_IF	,	// dir if
	factor_none,	// TOK_DIR_ELSE	,	// dir else
	factor_none,	// TOK_DIR_BREAK	,
	factor_none,	// TOK_DIR_RETURN	,
	factor_none,	// TOK_DIR_WHILE	,
	factor_none,	// TOK_DIR_FOR		,
	factor_comma,	// TOK_COMMA		,
	factor_none,	// TOK_DIR_FORI	,

	/* bool operators  */
	factor_none,	// TOK_COMPARE		,33

	(FFunction) compare_notequal,	// TOK_NOTEQUAL	,
	(FFunction) compare_smaller,	// TOK_SMALLER		,	/* <  */
	(FFunction) compare_bigger,	// TOK_BIGGER		,	/* >  */
	(FFunction) compare_equal,	// TOK_EQUAL		,	/* ==  */
	(FFunction) compare_smallereq,	// TOK_SMALLEREQ	,	/* <=  */
	(FFunction) compare_biggereq,	// TOK_BIGGEREQ	,	/* >=  */

	factor_none,	// TOK_BOOL		,40
	factor_none,	// TOK_AND			,	/* &  */
	factor_none,	// TOK_OR			,	/* |  */
	factor_not,		// TOK_NOT	/* !  */
	factor_none,	// TOK_NAND		,	/* !&  */
	factor_none,	// TOK_NOR			,	/* !|  */
	factor_none,	// TOK_XOR			,	/* ^  */
	/* term operators  */
	factor_plus,	// TOK_PLUS		,47
	factor_minus,	// TOK_MINUS		,
	(FFunction)term2_power,	// TOK_POWER		,	/* ** */
	(FFunction)term2_modulo,	// TOK_MOD			,	/* %  */
	(FFunction)term1_mul,	// TOK_MUL			,
	(FFunction)term1_div,	// TOK_DIV			,

	factor_line_array,	// TOK_LBRAKET		,53
	factor_error,	// TOK_RBRAKET		,
	factor_none,	// TOK_SQUOTE		,
	factor_at,		// TOK_AT			,
	factor_none,	// TOK_RANGE		,
	factor_none,	// TOK_BQUOTE
	factor_none,	// TOK_DOLAR		,
	factor_none,	// TOK_TILDA		,
	update_val,		// TOK_INCREASE	,61
	update_val,		// TOK_DECREASE	,62
	factor_none,	// TOK_INCREASEBY 63
	mul_by,			// TOK_MULBY
	decrease_by,	// TOK_DECREASEBY
	factor_none,	// TOK_BSLASH		,

	factor_none,	// TOK_NL				,
	factor_none,	// TOK_DIR_CONTINUE	,
	factor_none,	// TOK_DIR_FOREACH		,
	factor_none,	// TOK_DIR_TYPE,
	factor_array1,	// TOK_ARRAY1
	factor_array2,	// TOK_ARRAY2
	factor_array_l1,// TOK_ARRAY_L1
	factor_array_l2,// TOK_ARRAY_L2
	factor_none,	// TOK_ASSIGNENV	,
	factor_none,	// TOK_ASSIGNOPT	,
	factor_none,	// TOK_END,
	factor_none,	// TOK_DEFINE_TYPE,
#if	USE_TYPE_VARS
	factor_assign_type,	// TOK_ASSIGN_TYPE,
	factor_type_element,	// TOK_TYPE_ELEMENT
#else
	factor_none,	// TOK_ASSIGN_TYPE,
	factor_none,	// TOK_TYPE_ELEMENT
#endif
	factor_none,	// TOK_DOT,
	factor_none,	// TOK_INCBEFORE
	factor_none,	// TOK_DECBEFORE
	factor_none,
	factor_none,
	factor_none,	// TOK_INCREASE_ARRAY1,
	factor_none,	// TOK_INCREASE_ARRAY2,
	factor_none,	// TOK_DECREASE_ARRAY1,
	factor_none,	// TOK_DECREASE_ARRAY2,
	factor_none,	// TOK_NEGATE,
	factor_none		// TOK_OTHER,
};
