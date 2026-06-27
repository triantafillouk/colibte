/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

	interpreter syntax/error analyzer
*/

int err_lexpression();
int err_num_expression();
int err_cexpression();
int err_check_block1();
int err_check_sentence1();
int err_assign_val();
int err_assign_env();

int assign_type_to=0;

void mesg_out(const char *fmt, ...)
{
 va_list args;
 static char mline[1024];
    if (fmt != NULL) {
		va_start(args,fmt);
		vsnprintf(mline,sizeof(mline),fmt,args);
		va_end(args);
		out_print(mline,1);
    }
}

char * tok_info2(tok_struct *tok)
{
 static char stok[MAXLLEN];
 // MESG("tok_info2: ");
	if(tok){
	// MESG("tok not null! %d",tok->ttype); 
	if(tok->ttype>TOK_OTHER || tok->ttype==0) {
		return("unknown type of token!");
	};
	if(tok->tname!=NULL){
		switch(tok->ttype){
			case TOK_SHOW:		
				snprintf(stok,sizeof(stok),"%d:%d %s :",tok->tline,tok->tnum,TNAME);
				break;
			case TOK_LCURL:
			case TOK_RCURL:
				snprintf(stok,sizeof(stok),"%d:%d %s other is %d",tok->tline,tok->tnum,TNAME,tok->match_tok->tnum);
				break;
			case TOK_SEP:
				snprintf(stok,sizeof(stok),"%d:%d %s",tok->tline,tok->tnum,TNAME);
				break;
			case TOK_DIR_IF:
			case TOK_DIR_ELSE:
			case TOK_DIR_FOR:
			case TOK_DIR_WHILE:
				snprintf(stok,sizeof(stok),"%d:%d %s",tok->tline,tok->tnum,TNAME);
				break;
			case TOK_QUOTE:
				snprintf(stok,sizeof(stok),"%d:%d %s \"%s\"",tok->tline,tok->tnum,TNAME,(char *)tok->tname);
				break;
			default:
				snprintf(stok,sizeof(stok),"%d:%d %s %s",tok->tline,tok->tnum,TNAME,(char *)tok->tname);
		};
	} else {
		// MESG("token name is null! line %d type %d",last_correct_line,tok->ttype);
		snprintf(stok,sizeof(stok),"type %d tname is null!",tok->ttype);
		};
	} else {
			snprintf(stok,sizeof(stok),"%2d:%3d %s [%f]",tok->tline,tok->tnum,TNAME,tok->dval);
	};
	return stok;
}

#define	SYNTAX_DEBUG1	0

#if	SYNTAX_DEBUG1	/* if debugging  */

#define TDSERR(description) char *Tds=description;MESG("#%*s",stage_level,Tds);

// Return with message
#define RT_MESG { \
		if(err_num>0) mesg_out(" >[%s][%d] end with error %d position %d line %d",Tds,tok->tnum,err_num,xpos,tok->tline);\
		else { if(show_stage==1) mesg_out(" >[%s][%d] end ok pos %d!",Tds,tok->tnum,xpos);};\
		stage_level--;return(err_num);\
}

#define RT_MESG1(pos) { \
	CHECK_TOK(pos);\
	stage_level--;return(err_num);\
}

#else	/* No debugging macros  */

#define TDSERR(description) {}

// Return with message
#define RT_MESG { \
	;return(err_num);\
}

#define RT_MESG1(pos) { \
	;return(err_num);\
}

#endif


#define SHOW_STAGE(pos)	{ show_type='#';CHECK_TOK(pos);}

#if	1
#define	CHECK_TOK(pos) { xpos=pos ;}
#define	NTOKEN_ERR(xxx)	{ tok++;show_type=';';CHECK_TOK(xxx);}
#else

#define	CHECK_TOK(pos) { xpos=pos;\
		if(err_num>0) {\
			show_type=' ';\
			mesg_out("!err [%2d][%4d]%*s-%s: [%s]",stage_level,xpos,stage_level,"",Tds,tok_info2(tok));\
			return(err_num);\
		} else {\
			if(show_stage==1) mesg_out(" %c[%2d][%4d][%4d]%*s-%s:[%s]",show_type,tok->tnum,stage_level,xpos,stage_level,"",Tds,tok_info2(tok));\
			show_type=' ';\
			MESG(" %c[%2d][%3d][%4d]%*s-%s:[%s]",show_type,stage_level,tok->tnum,xpos,stage_level,"",Tds,tok_info2(tok));\
		}\
}

#define	NTOKEN_ERR(xxx)	{ tok++;show_type=';';CHECK_TOK(xxx);}

#endif


int parse_level=0;
char show_type=' ';

int check_skip_token1( int type)
{
 	if(tok->ttype==type) 
 	{ 
		// MESG("skip_token1");
		NTOKEN2;
 		return(1);
 	} else {
		return(0);
	};
}

int check_skip_token_err1(int type,char *mesg,int err)
{
	// MESG("	check_skip_token_err: (%s)",tok_info(tok));
 	if(tok->ttype==type) 
 	{
		// MESG_TOK_INFO(" check_skip_token",tok);
		NTOKEN2;
 		return(0);
 	} else {
		syntax_error(mesg,err);
	};

  return(err_num);
}

/* skip token if input error */
void error_skip_token(int index,char *description)
{
	set_error(tok,index, "input error!!!");
	NTOKEN2;
}

void set_error(tok_struct *tok,int err,char *description)
{
 set_break(description);

 if(tok==NULL) {
 	fprintf(stderr,"error: tok is NULL, %d %s\n",err,description);
	err_num=err;
	err_str=description;
	return;
 };
 err_line=tok->tline;
 err_num=err;
 // MESG("set_error: [%s] line %d name %s",description,tok->tline,tok->tname);
 err_str=strdup(description);
#if	1
 if(execmd) {
	if(tok->tname==NULL) fprintf(stderr,"Error:%d tnum=%d ttype=%d %s line %d\n",err,tok->tnum,tok->ttype,err_str,err_line);
 	else fprintf(stderr,"Error:%d [%s] tnum=%d ttype=%d %s line %d\n",err,(char *)tok->tname,tok->tnum,tok->ttype,err_str,err_line);
 };
#endif
 tok=cbfp->end_token;
 if(tok==NULL) return;
 tok->ttype=TOK_EOF;
#if	TNORMAL
 set_tok_directive(tok,factor_eof);
#endif
 tok->tgroup=TOK_END;
 tok->ttype=TOK_EOF;
 current_active_flag=0;
}

void syntax_error(char *description,int err)
{
 if(err_num>0) return;	/* error already set  */
 set_error(tok,err,description);
}

#if	TBNF
int	err_eval_fun1(tok_struct *tok0,tok_struct *tok_bnf,int lpar)
#else
int	err_eval_fun1(tok_struct *tok0,int lpar)
#endif
{
	TDSERR("eval_fun1");
	int ia=0;
	int ia0=0;
	int f_entry;
	BTNODE 	*var_node = tok0->tok_node;
	int fnum = var_node->node_index;
	// MESG("err_eval_fun1: fnum=%d tok0=[%s]",fnum,tok_info(tok0));
#if	TBNF
	ia0=bnf_functions[fnum].f_args;
#else
	ia0=m_functions[fnum].f_args;
	// MESG("err_eval_fun1: fnum=%d [%s] args=%d",fnum,m_functions[fnum].f_name,ia);
#endif
	// MESG("err_eval_fun1:lpar=%d <<  f_args=%d",lpar,ia0);
	// MESG("err_eval_fun1: [%s] args=%d tnum=%d ttype=%d",m_functions[fnum].f_name,ia,tok->tnum,tok->ttype);

	f_entry=entry_mode;
	entry_mode=KNORMAL;

	SHOW_STAGE(401);

	ia=0;
	// MESG("	tok [%s]",tok_info(tok));
	while(1){
		// if(tok->ttype==TOK_LPAR) {plevel++; NTOKEN_ERR(402); continue;};
		// MESG("function arg tnum=%d ttype=%d ia=%d",tok->tnum,tok->ttype,ia);
		// if(tok->ttype==TOK_RPAR) {
			// if(plevel>0) { plevel--;NTOKEN_ERR(402); continue;};
		// };
		if(tok->ttype==TOK_RPAR||tok->ttype==TOK_SEP ||tok->ttype==TOK_EOF) {
			// MESG("end function parameters");
			if(tok->ttype==TOK_RPAR) stack_push("eval_function )",tok,-TOK_RPAR);
			break;
		}
		if(tok->ttype==TOK_COMMA) {
			// MESG("	found comma! %s",tok_info(tok));
			stack_push("eval_function ,",tok,0);
			NTOKEN_ERR(403);
		}
		ia++;
		// MESG("	ia=%d [%s]",ia,tok_info(tok));
		err_num=err_lexpression();
	};
	// MESG("	ia=%d ia0=%d",ia,ia0);
	// Check for correct number of args, this sould not happen!
	if(ia0>=0 && ia0!=ia) {
		MESG("Error in number of args: %d != %d",ia0,ia);
		syntax_error("function arguments error",4031);
		return(err_num);
	};
	// MESG("	err_eval_fun_1: 1");
	if(tok->ttype==TOK_RPAR) {
		// MESG("err_eval_fun1: skip RPAR!!");
#if	TBNF
		if(lpar) stack_push("eval_func )",tok,-TOK_RPAR);
#endif
		NTOKEN_ERR(404);
		// MESG("	eval_func1: after rpar: [%s]",tok_info(tok));
		// if(tok->ttype==TOK_SEP) stack_push("function end",tok,0);
	};
	// MESG("	err_eval_fun_1: 2");

	tok0->number_of_args=ia;
#if	TBNF
	tok_bnf->number_of_args=ia;
#endif
	entry_mode=f_entry;
	// MESG("err_eval_fun1: END >> args=%d %d",ia0,ia);
	RT_MESG1(408);
}

int err_skip_type_args(tok_struct *tok0)
{
 int stat=0;
 BTNODE *nod0=tok0->tok_node;
 BTREE *nb = (BTREE *)nod0->node_dat;
 int args0=nb->items;
 // MESG("err_skip_type_args: items=%d",args0);
 // MESG("	items=%d",args0);
 if(tok->ttype==TOK_LBRAKET) {
 	// MESG("	skip index");
	stack_push("type lbraket",tok,-TOK_LBRAKET);
	NTOKEN_ERR(661);
	stat=err_cexpression();
	stack_push("type_args sep",tok,0);
	NTOKEN_ERR(662);
 };
 if(tok->ttype==TOK_LPAR) {
 	// MESG("	there is an LPAR type=%d name=%s",tok->ttype,tok->tname);
	int nargs=0;
	// int rows=0;

	while(tok->ttype!=TOK_RPAR){
		// MESG("-- start of loop [%s]",tok->tname);
		nargs++;
		if(nargs>args0) {
			// MESG("	?? nargs?args0 %d>%d",nargs,args0);
			return 661;
		};
		// MESG("	nargs=%d type=%d name=[%s]",nargs,tok->ttype,tok->tname);
		if(tok->ttype!=TOK_COMMA && tok->ttype!=TOK_LPAR && tok->ttype!=TOK_SEP) { 
			return 662;
		};
		stack_push("skip args sep",tok,0);
		NTOKEN_ERR(668);
		stat=err_cexpression();
		// MESG("	next token in loop is [%s] tnum=%d",tok->tname,tok->tnum);
		if(tok->ttype==TOK_SEP) {
			// rows++;
			nargs=0;
		};	
	};
	stack_push("skip_args )",tok,0);
	NTOKEN_ERR(670);
	// MESG("err_skip_type_args: Data definition of [%s] at line %d rows=%d args=%d",tok0->tname,tok0->tline,rows+1,args0);
 };
 return stat;
}

int  err_push_args_1(int *nargs)
{
 // int i=0;
 int num_args=0;
 TDSERR("push_args");

 SHOW_STAGE(410);
 // MESG("err_push_args: [%s]",tok_info(tok));
 if(tok->ttype!=TOK_RPAR) {
	if(tok->ttype==TOK_SEP) {
		*nargs=0;
		set_error(tok,411,"no parenthesis on function call!");
		return(err_num);
	};
 
 while(1){
	xpos=412;
	
	if(tok->ttype==TOK_RPAR) {
		break;
	};
	err_num = err_lexpression();
	// MESG("	%2d	after expression [%s]",num_args,tok_info(tok));
	if(err_num) { ERROR("error in push_args_1 from lexpression %d",err_num);return err_num;};
	CHECK_TOK(413);
	num_args++;

	if(err_num) { 
		set_error(tok,err_num,"after push_args");
		*nargs=num_args;
		return(err_num);
	};
	// i++;
	xpos=415;
	
	if(tok->ttype==TOK_RPAR) {
		
		break;
	} else if (tok->ttype==TOK_COMMA) {
		stack_push("arg ,",tok,0);
		NTOKEN_ERR(416);
		continue;
	}
	else {
		*nargs=num_args;
		xpos=417;
		set_error(tok,xpos,"after push_args:2");
		return(err_num);
	}
 };
	*nargs=num_args;
	// MESG("err_push_args: end args=%d err=%d",num_args,err_num);
 	return(err_num);
 } else {	/* no arguments!  */
	xpos=418;
	*nargs=num_args;
	init_error();
 	return(0);
 }
}


int err_assign_args1(int nargs)
{
 int i=0;
 TDSERR("assign_args");

 SHOW_STAGE(420);
 stack_push("assign args",tok,0); // ????
 NTOKEN_ERR(4201); /* skip name */
 // MESG("err_assign_args1: nargs=%d",nargs); 
 if(nargs!=0) {
	xpos=421;
	for(i=0;i<nargs;i++) {
		xpos=422;
		if(tok->ttype==TOK_RPAR) {
			// stack_push("end of assign",tok,TOK_RPAR);
			break;	/* end of arguments!  */
		}
		xpos=423;	/* this should be a var token  */

		err_cexpression();
		// MESG("err_assign: after lexpression: [%s] %d",tok->tname,tok->ttype);
		if(tok->ttype==TOK_RPAR)  {
			// stack_push("end of assign1",tok,TOK_RPAR);
			break;
		}
		if(tok->ttype!=TOK_COMMA) { set_error(tok,xpos,"not separator between args"); RT_MESG1(426);};
		stack_push("assign_args sep",tok,0);
		NTOKEN_ERR(427);	/* skip separator  */
	};
 } else { // we send no arguments!
	// skip till end parenthesis setting default values for arguments!!??
	xpos=428;
	if(tok->ttype==TOK_SEP) return(0);

	i=0;
	while(1) {
		CHECK_TOK(430);
		if(tok->ttype==TOK_RPAR) {
			break;
		};
		xpos=431;
		stack_push("skip sem par",tok,0);
		NTOKEN_ERR(432);	/* skip semicolon or end parenthesis */

		if(tok->ttype==TOK_RPAR)  {
			break;
		};
		NTOKEN_ERR(433);
		i++;
	} ;
 };
 // MESG("err_assign_args1: end tok = [%s] %d",tok->tname,tok->ttype);
 NTOKEN_ERR(434);
 return(0);
}


int err_assign_val()
{
 TDSERR("assign_val");
 // MESG("# tok assign_val: %s",tok_info(tok));
 SHOW_STAGE(440);
	// MESG("err_assign_val");
	if(!simple){
		xpos=440;
		syntax_error("bogus assignement!!!!",xpos);
		RT_MESG1(440);
	};
	err_num=err_lexpression();

	RT_MESG1(442);
}

int err_increase_by()
{
 TDSERR("increase_by");
 SHOW_STAGE(443);
 // MESG("# increase_by: %s",tok_info(tok));
	if(!simple){
		xpos=443;
		syntax_error("bogus assignement!!!!",xpos);
		RT_MESG1(443);
	};
	err_num=err_lexpression();

	RT_MESG1(443);
}

int err_mul_by()
{
 TDSERR("mul_by");
 SHOW_STAGE(443);
 // MESG("# mul_by	: %s",tok_info(tok));

	if(!simple){
		xpos=443;
		syntax_error("bogus assignement!!!!",xpos);
		RT_MESG1(443);
	};
	err_num=err_num_expression();

	RT_MESG1(443);
}

int err_decrease_by()
{
 TDSERR("decrease_by");
 SHOW_STAGE(443);
 MESG_TOK_INFO("decrease_by",tok);

	if(!simple){
		xpos=443;
		syntax_error("bogus assignement!!!!",xpos);
		RT_MESG1(443);
	};
	err_num=err_num_expression();

	RT_MESG1(443);
}

int err_increase_val()
{
 TDSERR("increase_val");
 MESG_TOK_INFO("increase_val",tok);
 SHOW_STAGE(444);
 if(!vtype_is(VTYPE_NUM)) {
	set_error(tok,444,"increase_val not numeric!");
	NTOKEN_ERR(444);
	return(444);
 };
 // NTOKEN_ERR(444);
 RT_MESG1(444);
}

int err_decrease_val()
{
 TDSERR("decrease_val");
 // MESG_TOK_INFO("# decrease_val",tok);
 SHOW_STAGE(446);
 if(!vtype_is(VTYPE_NUM)) {
	set_error(tok,446,"decrease_val not numeric!");
	NTOKEN_ERR(446);
	return(446);
 };
 NTOKEN_ERR(446);
 RT_MESG1(447);
}

int err_assign_env()
{
 TDSERR("assign_env");
 // MESG_TOK_INFO("# assign_env",tok);

 SHOW_STAGE(450);
	if(!simple){
		xpos=451;
		syntax_error("bogus assignement!!!!",xpos);
		RT_MESG1(451);
	};xpos=452;
	err_num=err_cexpression();
	RT_MESG1(453);
}

int err_exec_function(char *name,int nargs,FILEBUF **bf)
{
 TDSERR("exec_function");
#if	DEBUG1
 int save_stage_level=stage_level;
#endif
 	// MESG("err_exec_funtion: << %s",name);
    FILEBUF *bp;		/* ptr to buffer to execute */
    char bufn[MAXFLEN+2];		/* name of buffer to execute */
	int parsed;

	xpos=460;
	
	SHOW_STAGE(462);

	/* find out what buffer to execute */
	strlcpy(bufn+1,name,MAXFLEN);

	/* construct the buffer name */
    bufn[0] = '[';
    strcat(bufn, "]");
	
	/* find the pointer to that buffer */
    if ((bp=get_filebuf(bufn,NULL,0)) == NULL) 
	{
		xpos=463;ERROR("No function named %s",bufn);
		syntax_error("function not found",xpos);
		RT_MESG;
    };
	*bf=bp;
	// tok_struct *tok1=tok;
	// MESG("err_exec_function: %s args=%d",bp->b_fname,nargs);
	parsed=parse_block1(bp,NULL,0);	/* do not init if already parsed!, returns 0 if parsed  */
	// MESG("err_exec_function: return from parse_block1");
	if(parsed==0) {	/* already parsed, no need to check again!  */
		// MESG("	already parsed!");
		// MESG("err_exec_function: already parsed! >>");
		RT_MESG1(463);
	};
	// if(tok!=tok1) {
		// MESG("tok changed from parse_block1!!!!!!!!!!!!!!");
	// };
	/* and now execute it as asked */
	// MESG("err_exec_function: call check_init");
	tok_struct *tok_latest=tok;
	if((check_init(bp))>0) {
		ERROR("found syntax errors in function!");
		RT_MESG1(464);
	};
#if	DEBUG1
	stage_level=save_stage_level;
#endif
	tok=tok_latest;

	tok=bp->tok_table;
	no_push=1;
	err_num=err_assign_args1(nargs);
	tok=bp->tok_table;
	no_push=0;
	CHECK_TOK(466);

	// tok=tok_latest;
	// MESG("err_exec_function: before check_sentence1");
	err_num=err_check_sentence1();
	// MESG("err_exec_function: end >>");
	// MESG("err_exec_func: err_num=%d",err_num);
	RT_MESG1(467);
}


int err_factor()
{
 static int pre_symbol=0;
 TDSERR("factor");
 int lpar=0;
 // MESG("-- err_factor %s",tok_info(tok));
#if	TNORMAL
 set_tok_function(tok,0);
#endif
#if	TBNF
 tok_struct *tok0_bnf=NULL;

 if(tok->ttype!=TOK_NOT && tok->ttype!=TOK_LPAR && tok->ttype!=TOK_MINUS && tok->ttype!=TOK_PLUS) {
	if(tok->ttype!=TOK_VAR)
 	tok0_bnf=stack_push("factor",tok,tok->ttype);	// ????
 };
#endif
 int save_macro_exec;
 tok_struct *tok0; 

 ex_edenv=0;

 SHOW_STAGE(470);
 tok0=tok;
 switch(tok0->ttype) {
 	case TOK_PLUS:
	case TOK_MINUS:
		pre_symbol++;
 };
 NTOKEN_ERR(473);
 // MESG("	>> : next tok (%s)",tok_info(tok));
 if(tok0->ttype > TOK_OTHER) {
 	// MESG("unknown token type %d line %d %d",tok0->ttype,tok0->tline,last_correct_line);
	err_num=4730;
	return(err_num);
 };

 if(tok0->ttype==TOK_VAR && tok->ttype != TOK_ASSIGN) {
 	tok0_bnf=stack_push("factor",tok0,tok0->ttype);
 };
 // set_tok_function(tok0,0);
 // MESG("switch  : tok0 %s",tok_info(tok0));
 // MESG("now     : tok  %s",tok_info(tok));
 switch(tok0->ttype) {
	/*  the following ends factor  */
#if	1
 	case TOK_SEP:
		xpos=476;
		// syntax_error("separator in factor!",xpos);
		RT_MESG1(xpos);
#endif
	case TOK_COMMA:
	case TOK_SHOW:
		// MESG("err_factor: TOK_SHOW ..............");
#if	TNORMAL
		set_tok_function(tok,0);
#endif
		RT_MESG1(xpos);
	case TOK_LBRAKET:{	/* array definition  */
		// MESG("err: TOK_LBRAKET, array definition for [%s]",tok0->tname);
		pre_symbol=0;
		int i=0;
		// int j=0;
		int cdim=0;
		int rows=1,cols=0;
		cdim=1;
		ex_nvars=0;ex_nquote=0;ex_nums=0;	/* initialize table counters  */
		ex_array=NULL;
		int ctype=VTYPE_ARRAY;
		while(cdim>0){
		while(1) {
			// MESG("	err: lbra: type=%d [%s] %f",tok->ttype,tok->tname,tok->dval);
			if(tok->ttype==TOK_EOF||tok->ttype==TOK_SHOW) {
				xpos=4777;
				syntax_error(" FAC1_err: No closing braket",xpos);
				RT_MESG1(xpos);
			};
			if(tok->ttype==TOK_SEP || tok->ttype==TOK_COMMA) {
				// MESG("	tok_sep [%s]",tok->tname);
				stack_push("lbra",tok,-tok->ttype);
				NTOKEN2;
				continue;
			};
			if(tok->ttype!=TOK_NUM && tok->ttype!=TOK_VAR && tok->ttype!=TOK_MINUS && tok->ttype!=TOK_LPAR && tok->ttype!=TOK_PLUS && tok->ttype!=TOK_QUOTE) {
				xpos=4778;
				syntax_error(" wrong separator in table definition!",xpos);
				RT_MESG1(xpos);
			};
			err_num=err_num_expression();
			// MESG("	ex_value=%f err_num=%d",ex_value,err_num);
			if(err_num) return err_num;
			i++;if(i>cols) cols=i;
			if(tok->ttype==TOK_RBRAKET) {
				// MESG("	tok_rbraket: end definition");
				cdim=0;break;
			};
			if(tok->ttype==TOK_SEP) {	/* Add tok->ttype==TOK_COMMA for using comma to separate array items  */
				i=0;
				// j++;
				cdim++;if(cdim>rows) rows=cdim;
				// MESG("		new row2");
				stack_push("lbra line",tok,-tok->ttype);
				NTOKEN2;
				continue;
			};
			// MESG("ttype=%d %s",tok->ttype,tname(tok->ttype));
		};
		cdim--;
		};
		// MESG("set array dat! [%s] rows=%d cols=%d",tok0->tname,rows,cols);
		// set array dat
		if(tok0->tok_adat) {	/* this should never hapen!  */
			// MESG("err_lb: free array!");
			free(tok0->tok_adat);
		};
		if(ex_nvars) ctype=VTYPE_AMIXED;
		else if(ex_nquote>0 && ex_nums>0) ctype=VTYPE_AMIXED;
		else if(ex_nquote) ctype=VTYPE_SARRAY;
		
		tok0->tok_adat = new_array(rows,cols,ctype);
#if	TBNF
		tok0_bnf->tok_adat = tok0->tok_adat;
		// MESG("	err: CREATED new array %d [%d %d]",tok0->tok_adat->anum,rows,cols);
		stack_push("4789",tok,-tok->ttype);
#endif
		NTOKEN_ERR(4789)
		// MESG("	token after array: %s",tok_info(tok));
		if(tok->ttype!=TOK_SEP && tok->ttype!=TOK_EOF && tok->ttype!=TOK_SHOW) {
			xpos=4789;
			set_error(tok,xpos,"missing separator after array declare");
		};
		RT_MESG1(4789);
		};
	case TOK_RPAR:
		xpos=479;
		pnum--;
		// syntax_error("wrong argument number",xpos);
		RT_MESG1(480);
	/* start of logic ---------  */
	case TOK_VAR:{	// 0 variable
#if	0
		if(current_stable)
		MESG("	TOK_VAR: var ind=%d type is %d [%s]",tok0->tind,
			current_stable[tok0->tind].var_type,tok_info(tok0));
#endif
		pre_symbol=0;
		ex_nvars++;

		if(tok->ttype==TOK_INCREASE) {
			tok->dval=1;
#if	TNORMAL
			set_tok_function(tok,0);
#endif
			stack_push("INC",tok,tok->ttype);
			NTOKEN_ERR(498);
		} else
		if(tok->ttype==TOK_DECREASE) {
			tok->dval=-1;
#if	TNORMAL
			set_tok_function(tok,0);
#endif
			stack_push("DEC",tok,tok->ttype);
			NTOKEN_ERR(4981);
		};
		if(tok->ttype==TOK_TYPE_ELEMENT) {
			// MESG("err tok_type_element: %s",tok->tname);
#if	TNORMAL
			set_tok_function(tok,0);
#endif
			// set_bnf_function1(tok,tok->ttype);
			tok->dval=-1;
			tok_struct *bnf_tok=stack_push("type_element",tok,tok->ttype);
			bnf_tok->dval=-1;
			NTOKEN_ERR(4982);
		};
		if(tok->ttype==TOK_ASSIGN) {
			// MESG("set normal assign [%s]",tok_info(tok));
			assign_type_to=0;
			tok->tname = tok0->tname;
			tok->tind = tok0->tind;
			// MESG("	var_name_to_assign: %s [%s]",tok0->tname,tok_info(tok));
		};
		// MESG("	TOK_VAR: return [%s]",tok_info(tok));
		RT_MESG1(493);}
	case TOK_ARRAY_L2:{
		// MESG("TOK_ARRAY_L2: [%s] type %d ind=%d",tok0->tname,tok0->ttype,tok0->tind);
		err_num=err_num_expression(); 
		if(tok->ttype==TOK_RBRAKET) {
			stack_push("array_l2",tok,tok->ttype);
			NTOKEN2;
		} else {
			MESG("	No rbracket found!");
		};
		// MESG("	tok_array_l2: after rbraket [%s]",tok_info(tok));
		stack_push("tok_array_l2",tok,tok->ttype);
		if(tok->ttype==TOK_TYPE_ELEMENT) {
			tok->dval=-1;
#if	TBNF
			tok0_bnf->bnf_factor_function=bnf_type_l2_result;
			tok0_bnf->tname="TL2";
#endif
			// MESG("	TOK_TYPE_ELEMENT before [%s]",tok_info(tok));
			NTOKEN2;
			// MESG("	TOK_TYPE_ELEMENT after [%s]",tok_info(tok));

			if(tok->ttype==TOK_INCREASE) {
				// MESG("	found TOK_INCREASE after type array_l2");
				tok->dval=1;
#if	TNORMAL
				set_tok_function(tok,0);
#endif
				tok->tgroup=0;
				stack_push("INC_AT2",tok,TOK_INCREASE);
				NTOKEN_ERR(4984);
			} else
			if(tok->ttype==TOK_DECREASE) {
				// MESG("	found TOK_DECREASE after type array_l2");
				tok->dval=-1;
#if	TNORMAL
				set_tok_function(tok,0);
#endif
				tok->tgroup=0;
				stack_push("DEC_AT2",tok,TOK_DECREASE);
				NTOKEN_ERR(4985);\
			};

			RT_MESG1(495);
		} else 
		{
		NTOKEN_ERR(444);
		if(tok->ttype==TOK_INCREASE) {
			// MESG("	found TOK_INCREASE after array_l2");
			tok->dval=1;
#if	TNORMAL
			set_tok_function(tok,0);
#endif
			tok->tgroup=0;
			stack_push("INC_AL2",tok,TOK_INCREASE_ARRAY2);
			NTOKEN_ERR(4984);
		} else
		if(tok->ttype==TOK_DECREASE) {
			// MESG("	found TOK_DECREASE after array_l2");
			tok->dval=-1;
#if	TNORMAL
			set_tok_function(tok,0);
#endif
			tok->tgroup=0;
			stack_push("DEC_AL2",tok,TOK_DECREASE_ARRAY2);
			NTOKEN_ERR(4985);
		};
		};
		RT_MESG1(4441);		
	};
	case TOK_ARRAY_L1:	
		// MESG("TOK_ARRAY_L1: [%s] type %d ind=%d",tok0->tname,tok0->ttype,tok0->tind);
		pre_symbol=0;
		ex_nvars++;
#if	TBNF
		tok_struct *dest=stack_push("499",tok,tok->ttype);
#endif
		NTOKEN_ERR(499);
#if	TBNF
		if(dest->ttype==TOK_TYPE_ELEMENT) {
			// MESG("	TOK_TYPE_ELEMENT before [%s]",tok_info(tok));
			if(tok->ttype==TOK_ASSIGN || tok->ttype==TOK_INCREASEBY || tok->ttype==TOK_MULBY || tok->ttype==TOK_DECREASEBY) {
				// MESG("	assign type_element1");
				tok0_bnf->bnf_factor_function=bnf_type_element_l1_tba;
				assign_type_to=TOK_ASSIGN_TYPE;
			} else {;
				tok0_bnf->bnf_factor_function=bnf_type_l1_result;
			};
			RT_MESG1(493);
		} else 
#endif
		{
		if(tok->ttype==TOK_INCREASE) {
			tok->dval=1;
			// tok->tgroup=TOK_TERM2;
#if	TNORMAL
			set_tok_function(tok,0);
#endif
#if	TBNF
			stack_push("INC_AL1",tok,TOK_INCREASE_ARRAY1);
#endif
			NTOKEN_ERR(4986);
		} else
		if(tok->ttype==TOK_DECREASE) {
			// tok->tgroup=TOK_TERM2;
#if	TBNF
			tok->bnf_group=tok->ttype;
#endif
			tok->dval=-1;
#if	TNORMAL
			set_tok_function(tok,0);
#endif
			stack_push("DEC_AL1",tok,TOK_DECREASE_ARRAY1);
			NTOKEN_ERR(4987);
		};
		}
		RT_MESG1(494);

	// case TOK_INCREASE:
	case TOK_ARRAY1:{
		// MESG("	TOK_ARRAY1: tok0=[%s]",tok_info(tok0));
#if	TBNF
		tok0->bnf_group=tok0->ttype;
#endif
		err_num=err_num_expression(); 
		// MESG("	err tok_array1: after [%s]",tok_info(tok));
		xpos=499;
		if(!check_skip_token_err1(TOK_RBRAKET,"array error",xpos)){
			tok_struct *braket=tok-1;
			// MESG("	RBRAKET found! [%s]",tok_info(braket));
			stack_push("RB",braket,-braket->ttype);
#if	TBNF
			tok0_bnf->ttype=TOK_ARRAY_L1;
			tok0_bnf->bnf_factor_function=bnf_factor_array_l1;
#endif
			if(tok->ttype==TOK_ASSIGN)	assign_type_to=TOK_ASSIGN_ARRAY1;
		} else {
			MESG("	No rbracket found!");
		};
		if(tok->ttype==TOK_INCREASE) {
			// MESG("		TOK_INCREASE: tok=[%s]",tok_info(tok));
			tok->dval=1;
#if	TBNF
			tok0_bnf->bnf_factor_function=bnf_factor_array_l1_tba;
#endif
			set_tok_function(tok,0);
#if	TBNF
			tok_struct *ts=stack_push("4988",tok,tok->ttype);
			ts->bnf_factor_function=bnf_update_array1;
#endif
			NTOKEN_ERR(4988);
		} else
		if(tok->ttype==TOK_DECREASE) {
			// MESG("		TOK_DECREASE: tok=[%s]",tok_info(tok));
			tok->dval=-1;
#if	TBNF
			tok0_bnf->bnf_factor_function=bnf_factor_array_l1_tba;
#endif
			set_tok_function(tok,0);
#if	TBNF
			tok_struct *ts=stack_push("4988",tok,tok->ttype);
			ts->bnf_factor_function=bnf_update_array1;
#endif
			NTOKEN_ERR(4989);
		};
#if	TBNF
		if(tok->ttype==TOK_ASSIGN || tok->ttype==TOK_INCREASEBY || tok->ttype==TOK_MULBY || tok->ttype==TOK_DECREASEBY) {
			tok0_bnf->bnf_factor_function=bnf_factor_array_l1_tba;
			// MESG("; for [%s] set  bnf_factor_array_l1_tba",tok_info(tok0));
			assign_type_to=TOK_ASSIGN_ARRAY1;
		};
#endif
		RT_MESG1(4931);
		};
#if	0
	case TOK_RBRAKET:
		// MESG("	err: rbracket");
		RT_MESG1(5931);
#endif
	case TOK_ARRAY2:{
		// MESG("	TOK_ARRAY2 tok0=[%s]",tok_info(tok0));
#if	TBNF
		tok0->bnf_group=tok0->ttype;
#endif
		err_num=err_num_expression(); 
		// MESG("	err_array2:1 [%s]",tok_info(tok));
		stack_push("500",tok,-tok->ttype);
		
		NTOKEN_ERR(500);
		// MESG("	err_array2:2 [%s]",tok_info(tok));
		if(tok->ttype==TOK_TYPE_ELEMENT) {
			tok->dval=-1;
			// MESG("	TOK_TYPE_ELEMENT before [%s]",tok_info(tok));
#if	TBNF
			tok0_bnf->bnf_factor_function=bnf_type_l2_result;
#endif
			RT_MESG1(501);
		};
		err_num=err_num_expression(); 
		// MESG("err_array2:3 t=%d",tok->ttype);
		stack_push("5001",tok,-tok->ttype);
		NTOKEN_ERR(5001);	// skip RB
		// MESG("err_array2:4 t=%d",tok->ttype);
		if(tok->ttype==TOK_INCREASE) {
			// MESG("		TOK_INCREASE set bnf_factor_array_l1_tba");
#if	TBNF
			tok0_bnf->bnf_factor_function=bnf_factor_array_l1_tba;
			assign_type_to=TOK_INCREASE_ARRAY2;
#endif
		};
		if(tok->ttype==TOK_DECREASE) {
			// MESG("		TOK_DECREASE set bnf_factor_array_l1_tba");
#if	TBNF
			tok0_bnf->bnf_factor_function=bnf_factor_array_l1_tba;
			assign_type_to=TOK_DECREASE_ARRAY2;
#endif
		};
#if	TBNF
		if(tok->ttype==TOK_ASSIGN||(tok->ttype>=TOK_INCREASEBY&&tok->ttype<=TOK_DECREASEBY)) {
			tok0_bnf->bnf_factor_function=bnf_factor_array_l2_tba;
			tok0_bnf->tname="AL2_TBA";
			assign_type_to=TOK_ASSIGN_ARRAY2;
			// MESG("; for [%s] set  bnf_factor_array_l2_tba",tok_info(tok0_bnf));
		};
#endif
		RT_MESG1(4932);
		};
	case TOK_LPAR:
		{
			lpar=1;
			pnum++;
			pre_symbol=0;
			CHECK_TOK(481);
			{
				if(tok->ttype==TOK_RPAR) {	/* null expression  */
					pnum--;
					stack_push("483",tok,-tok->ttype);
					NTOKEN_ERR(483);
					RT_MESG;	/* return right after right parenthesis  */
				};
			};
			err_num = err_lexpression();
			CHECK_TOK(484);
			if(tok->ttype==TOK_LBRAKET) {
				MESG("	(((((( err_tok_lpar: we have a left braket, continue!");
				RT_MESG;
			};
#if	0
			if(tok->ttype ==TOK_LPAR) {
				NTOKEN_ERR(1841);
				RT_MESG;
			};
#endif
			if(tok->ttype !=TOK_RPAR) {
				xpos=4850;
				set_error(tok0,xpos,"FAC1_err: No closing parenthesis");
				// syntax_error(" FAC1_err: No closing parenthesis",xpos);
				RT_MESG1(485);
			} else { 
				pnum--;
				// stack_push("4851",tok);
				NTOKEN_ERR(4851);
			};
			RT_MESG1(486);
	 	};
		break;
#if	0
	case TOK_EQUAL:
		tok0->bnf_group=tok0->ttype;
		NTOKEN_ERR(444);
		// err_cexpression();
		RT_MESG1(444);;	
#endif
	case TOK_NUM:
		pre_symbol=0;
		ex_nums++;
		xpos=4871;
#if	TBNF
		tok0->bnf_group=tok0->ttype;
#endif
		// MESG("tok_num: ");
		// MESG("	next is: %s",tok_info(tok));
		if(tok->ttype==TOK_INCREASE||tok->ttype==TOK_INCREASEBY ||tok->ttype==TOK_DECREASE||tok->ttype==TOK_DECREASEBY) {
			tok--;
			set_error(tok0,xpos,"cannot change constant value!");
			RT_MESG1(xpos);
		};
		tok0->tname="numeric";
		RT_MESG1(487);
	case TOK_QUOTE:	 { // string 
		xpos=488;
#if	TBNF
		tok0->bnf_group=tok0->ttype;
#endif
		if(pre_symbol) { syntax_error("symbol before string",xpos);RT_MESG1(4881);};
		ex_nquote++;
		RT_MESG1(4882);
	 	};
	case TOK_AT: {
		xpos=489;
		RT_MESG1(4891);
		};
	case TOK_MINUS:
		// MESG("tok_minus: %d",pre_symbol);
#if	TBNF
		tok0->bnf_group=tok0->ttype;
#endif
		if(pre_symbol>1) {
			xpos=490;
			syntax_error("too many symbols(+) infront of factor",xpos);
			RT_MESG1(490);
		};
		CHECK_TOK(491);
		if(pre_symbol) {
			err_factor();
			// MESG("	minus set to negate");
#if	TBNF
			tok0_bnf=stack_push("negate",tok0,TOK_NEGATE);
#endif
		} else {
#if	TBNF
			tok0_bnf=stack_push("minus",tok0,tok0->ttype);
#endif
			err_factor();
		};
		RT_MESG1(4911);
	case TOK_PLUS:
		// MESG("tok_plus: %d",pre_symbol);
#if	TBNF
		tok0->bnf_group=tok0->ttype;
#endif
		if(pre_symbol>1) {
			xpos=490;
			syntax_error("too many symbols(+) infront of factor",xpos);
			RT_MESG1(490);
		};
		CHECK_TOK(491);
		if(pre_symbol) {
			err_factor();
		} else {
#if	TBNF
			tok0_bnf=stack_push("plus",tok0,tok0->ttype);
#endif
			err_factor();
		};

		// stack_push("NOT",tok0,tok0->ttype);
		RT_MESG1(4911);
	case TOK_NOT:
		xpos=492;
		if(pre_symbol) { syntax_error("symbol before not",xpos);RT_MESG;};
		pre_symbol=0;
		CHECK_TOK(xpos);
		int stat=err_factor();
		stack_push("NOT",tok0,tok0->ttype);
		return(stat);
	case TOK_OPTION:// 5 editor option
		/* variable's name in tok0->tname */
		xpos=494;
		ex_nvars++;
		var_node=tok0->tok_node;
#if	TBNF
		// stack_push("TOK_OPTION",tok0,tok0->ttype);
#endif
		ex_edenv=0;
		pre_symbol=0;
		RT_MESG1(xpos);
	case TOK_ENV:	// 1 editor env var
		/* variable's name in tok0->tname */
		var_node=tok0->tok_node;
		// MESG("+ TOK_ENV tok0=[%s]",tok_info(tok0));
		// MESG("           tok=[%s]",tok_info(tok));
#if	TBNF
		tok0->bnf_group=tok0->ttype;
		tok0_bnf->tok_node=var_node;
#endif
		pre_symbol=0;
		ex_nvars++;
		if(tok->ttype==TOK_ASSIGNENV) {
			// MESG("set normal assign [%s]",tok_info(tok));
			assign_type_to=0;
			tok->tname = tok0->tname;
			tok->dval=var_node->node_index;
			// MESG("	env_name_to_assign: %s ind=%d [%s]",tok0->tname,var_node->node_index,tok_info(tok));
		};
		RT_MESG1(495);
	case TOK_FUNC:	// 2 editor function 
		/* variable's name in tok0->tname */
		pre_symbol=0;
#if	TBNF
		tok0->bnf_group=tok0->ttype;
		err_num=err_eval_fun1(tok0,tok0_bnf,lpar);
#else
		err_num=err_eval_fun1(tok0,lpar);
#endif
		RT_MESG1(497);
	case TOK_PROC: {	// 4 ex_proc (normal function)
		// MESG("------ define TOK_PROC ------ [%s]",tok_info(tok));
		int nargs=0;
#if	TBNF
		tok0->bnf_group=tok0->ttype;
#endif
		tok_struct *after_proc;	// this is needed for recursive functions
		xpos=501;
		// MESG("err TOK_PROC:");
		ex_nvars++;

		/* function */
		pre_symbol=0;
		// MESG("	err check:	TOK_PROC: ");
		err_num = err_push_args_1(&nargs);
		if(err_num) return(err_num);
		// MESG("	err check: TOK_PROC: args=%d",nargs);
		tok0->t_nargs=nargs;
#if	TBNF
		if(tok0_bnf!=NULL) {
			tok0_bnf->t_nargs=nargs;
			// MESG("	proc function %s pushed ok!",tok0_bnf->tname);
		} else {
			err_num=502;
			MESG("tok0_bnf is NULL!!");
			RT_MESG1(502);
		};
#endif
		CHECK_TOK(503);
		after_proc=tok;	// this must be RPAR token!
		if(tok->ttype!=TOK_RPAR) {
			xpos=503;
			MESG("503 no right_parenthesis after proc");
			RT_MESG1(503);
		};
		stack_push("proc ) ",tok,-TOK_RPAR);
		NTOKEN2;
		after_proc=tok;	// this must be RPAR token!
		// MESG("err TOK_PROC: set after_proc [%s] ttype=%d",tok->tname,tok->ttype);
		FILEBUF *proc_buffer=NULL;
		err_num=err_exec_function(tok0->tname,nargs,&proc_buffer);
		// MESG("		>>>>>>>>>> after exec_function!");
		if(err_num) {
			// MESG("	err_num=%d",err_num);
			return(err_num);
		};
		if(proc_buffer!=NULL) {
			tok0->proc_buffer=proc_buffer;
#if	TBNF
			tok0_bnf->proc_buffer=proc_buffer;
#endif
			proc_buffer->function_called=0;
			// MESG("	set function buffer");
		};
		CHECK_TOK(504);
		tok=after_proc;
		// MESG("	TOK_PROC: end function tnum=%d",tok->tnum);
		// stack_push("proc ) ",tok,0);
		// MESG("err TOK_PROC: end ttype=%d",tok->ttype);
		RT_MESG;
	};
	case TOK_CMD:	{ // 3 editor command
		int index;
		int check_par;

		/* variable's name in tok0->tname */
		xpos=506;
		var_node=tok0->tok_node;
		index = var_node->node_index;
		// MESG("err: TOK_CMD");
#if	TBNF
		stack_push("TOK_CMD",tok0,-tok0->ttype);
#endif
		pre_symbol=0;
		if(ftable[index].arg==0) check_par=0;else check_par=1;
		if(check_par) 
		{	xpos=507;
			pnum++;

			if(pnum>10) {
				CHECK_TOK(508);
				return xpos;	/* recursion should not be checked at this stage  */
			};
		};

		xpos=5152;
		save_macro_exec=macro_exec;
		macro_exec=MACRO_MODE2;
		{
			int args=ftable[index].arg;
			int check_end=1;
			int i;
			if(args<0) {	/* undefined number of arguments ...  */
				args=999999;
				check_end=0;
			};
			xpos=516;

			if(args>0){
			for(i=0;i<args;i++){
				err_num=err_num_expression();
				if(err_num) return err_num;
				CHECK_TOK(517);
				// MESG("arg %d tok=[%s %d]",i,tok->tname,tok->ttype);
#if	1
				if(!check_end) {
					if(tok->ttype==TOK_RPAR || tok->ttype==TOK_SEP) {
						// MESG_TOK_INFO("# check RPAR:1",tok);
						break;
					} else {
						syntax_error("argument not correct!",xpos);
					};
				}
#else
				if(!check_end && tok->ttype==TOK_RPAR) {
					CHECK_TOK(5171);
					break;
				};
#endif
				if(i<args-1) {	/* check for a comma  */
					CHECK_TOK(5172);
					if(tok->ttype==TOK_COMMA || check_end==0) {
						stack_push("518",tok,0);
						NTOKEN_ERR(518);
					} else {
						CHECK_TOK(519);
						if(!check_end) {
							CHECK_TOK(5191);
							break;
						};
						syntax_error("argument error,comma expected",xpos);
						macro_exec=save_macro_exec;
						RT_MESG1(5192);
					};
				} else {
					CHECK_TOK(520);
					if(tok->ttype==TOK_RPAR) {
						// MESG_TOK_INFO("# check RPAR:2",tok);
						break;
					};
				};
			};

			if(i!=(args-1) && check_end) {
				xpos=521; 
				syntax_error("wrong number of arguments",xpos);
				macro_exec=save_macro_exec;
				RT_MESG1(5211);
			};
			CHECK_TOK(5211);
			};
		};

		macro_exec=MACRO_MODE2;
		
		CHECK_TOK(522);
		macro_exec = save_macro_exec;
		if(check_par){ 
		// MESG(" ---- skip tok_rpar");
#if	TBNF
			if(tok->ttype==TOK_RPAR) {
				stack_push("cmd rpar",tok,-tok->ttype);
			};
#endif
		if(!check_skip_token_err1(TOK_RPAR,"editor command",xpos)) 
		{
			pnum--;
		} else {
			xpos=523;
			syntax_error("right parenthesis not found",xpos);
			RT_MESG1(5231);
		};
		};
		RT_MESG1(524);
	};
	case TOK_DIR_ELSE:
#if	0
		xpos=526;
		set_error(tok,xpos,"else without if error");
#endif
		RT_MESG1(5261);
	case TOK_RBRAKET:
		// MESG("tok_rbtacket");
		RT_MESG1(5262);
	case TOK_ASSIGN:
		// MESG("TOK_ASSIGN");
	case TOK_INCREASEBY:
	case TOK_MULBY:
	case TOK_DECREASEBY:
		tok0->tname="assign";
		// set_bnf_function1(tok0,tok0->ttype);
#if	TBNF
		tok0->bnf_group=tok0->ttype;
#endif
		RT_MESG1(527);
#if	USE_TYPE_VARS
	case TOK_ASSIGN_TYPE:{
		// MESG("TOK_ASSIGN_TYPE: tok0 [%s]",tok_info(tok));
		// MESG("		tok [%s]",tok_info(tok));
		if(tok->ttype==TOK_LPAR) stack_push("assign type",tok,-tok->ttype);
		if(err_skip_type_args(tok0)) {
			set_error(tok0,2000,"TOK_ASSIGN_TYPE: error in argument list");
		};
		// MESG("	TOK_ASSIGN_TYPE: end tnum=%d",tok->tnum);
		RT_MESG1(528);
		};
#endif
#if	1
	case TOK_TYPE_ELEMENT:
		// MESG("TOK_TYPE_ELEMENT: [%s]",tok0->tname);
#if	TBNF
		tok0_bnf->dval=-1;
		tok0->bnf_group=tok0->ttype;
#endif
		tok0->tgroup=TOK_TERM2;
		RT_MESG1(529);
#endif
	default:
		xpos=527;
		MESG(" default: error_factor0: %s tind=%d ttype=%d TOK_VAR=%d",tok_info(tok0),tok0->tind,tok0->ttype,TOK_VAR);
		MESG(" default: error_factor : %s",tok_info(tok));
		set_error(tok,3000+tok0->ttype,"factor :wrong character found:");
		RT_MESG1(5271);
 }
 xpos=529;
 set_error(tok,xpos,"FAC1_err: end, ERROR should not pass from here!");
 RT_MESG1(5291);
}

int err_num_term3(tok_struct *tok1)
{
 TDSERR("num_term3");

 SHOW_STAGE(541);
 // MESG("- num term2");
 // tok_struct *tok0=tok;
 // MESG("-- term2 start: %s",tok_info(tok));
 err_num = err_factor();
 stack_push("term3",tok1,tok1->ttype);
 if(err_num) RT_MESG1(err_num);

 if(tok->tgroup==TOK_TERM2) {
 	CHECK_TOK(543);
	// tok->term_function = factor_funcs[tok->ttype];
#if	TNORMAL
	set_term_function(tok,(TFunction)factor_funcs[tok->ttype]);
#endif
	tok_struct *tok0=tok;

	if(tok1->ttype==TOK_MOD||tok1->ttype==TOK_POWER) {
		NTOKEN_ERR(544);
		// MESG("# term3 function [%s]",tok_info(tok0));
		err_num = err_num_term3(tok0);
	} else {
		NTOKEN_ERR(544);
		stack_push("term3 function",tok1,tok1->ttype);
		err_num = err_num_term3(tok0);
	};
	// if(tok0->ttype==TOK_MOD) MESG("# term2 function after [%s]",tok_info(tok0));
	RT_MESG1(545);
 } else {
 	// MESG(" term2 end %s",tok_info(tok));
 };
 RT_MESG1(548);
}

int err_num_term2()
{
 TDSERR("num_term2");

 SHOW_STAGE(541);
 err_num = err_factor();
 if(err_num) RT_MESG1(err_num);

 if(tok->tgroup==TOK_TERM2) {
 	CHECK_TOK(543);
	// tok->term_function = factor_funcs[tok->ttype];
#if	TNORMAL
	set_term_function(tok,(TFunction)factor_funcs[tok->ttype]);
#endif
	tok_struct *tok0=tok;

	if(tok0->ttype==TOK_MOD||tok0->ttype==TOK_POWER) {
		NTOKEN_ERR(544);
		err_num = err_num_term3(tok0);
	} else {
		NTOKEN_ERR(544);
		// MESG_TOK_INFO("-- push term2 function",tok0);
		stack_push("num_term2",tok0,tok0->ttype);
		err_num = err_num_term2();
	};
	// if(tok0->ttype==TOK_MOD) MESG("# term2 function after [%s]",tok_info(tok0));
	RT_MESG1(545);
 } else {
 	// MESG(" term2 end %s",tok_info(tok));
 };
 RT_MESG1(548);
}

/* multiply & divide */
int err_num_term1()
{
 TDSERR("term1_1");
 
 SHOW_STAGE(551);
 err_num=err_num_term2();
 if(err_num) RT_MESG1(err_num);
 // MESG("term1: initial token %s",tok_info(tok));
 CHECK_TOK(552);
 tok_struct *tok0=NULL;
 while (tok->tgroup==TOK_TERM1) {
 	// tok1=tok;
 	// tok->term_function = factor_funcs[tok->ttype];
	tok0=tok;
#if	TNORMAL
	set_term_function(tok,(TFunction)factor_funcs[tok->ttype]);
#endif
	CHECK_TOK(553);
	NTOKEN_ERR(5531);
	err_num=err_num_term2();
	// tok1 = tok;
	// MESG_TOK_INFO("-- push term1 function",tok0);
	stack_push("num_term1",tok0,tok0->ttype);

	if(err_num) RT_MESG1(5531);
	CHECK_TOK(554);
 };
 // MESG("# end of term 1 function! %s", tok_info(tok1));
 RT_MESG1(558);
}

int err_num_expression()
{
 TDSERR("num_expression");
 int expression_type=VTYPE_NUM;

 // init_expression_stack();

 xpos=561;
 SHOW_STAGE(561);
 // MESG("# start expression!");
 err_num = err_num_term1();
 if(err_num) return(err_num);

 expression_type=get_vtype();	// set local value

 while (tok->tgroup==TOK_TERM) {
   CHECK_TOK(563);
    tok_struct *tok0=tok;
#if	TBNF
	// tok->bnf_group=tok->ttype;
#endif
    if(tok->ttype==TOK_PLUS) {	/* TOK_PLUS  */
		// tok->term_function = term_plus;
#if	TNORMAL
		set_term_function(tok,term_plus);
#endif
		NTOKEN_ERR(568);
		err_num=err_num_term1();
		stack_push("TOK_PLUS",tok0,tok0->ttype);

		if(err_num) return (err_num);
		CHECK_TOK(569);
		simple=0;
		/* check validity of operation combinations   */
		if(expression_type==VTYPE_STRING) { /* catanate string */
			if(get_vtype()) {
			} else {
				expression_type=get_vtype();
			}
		} else {
		}
	} else
	if(tok->ttype==TOK_MINUS) {	/* TOK_MINUS  */
#if	TNORMAL
		set_term_function(tok,term_minus);
#endif
		NTOKEN_ERR(571);
		if(expression_type==VTYPE_STRING) {	// operator on first chars of strings. numeric result
			err_num=err_num_term1();
			stack_push("TOK_MINUS string",tok0,tok0->ttype);
			CHECK_TOK(573);
			simple=0;
		} else 	{
			err_num=err_num_term1();
			stack_push("TOK_MINUS num",tok0,tok0->ttype);
			CHECK_TOK(574);
		}
	};
 };
 // MESG_TOK_INFO("! end expression",tok);
 RT_MESG1(599);
}

int err_cexpression();

int err_lexpression()
{
 TDSERR("lexpression");
 
	// slval[0]=0;
	simple=1;
	// MESG("err_lexpression: [%s]",tok_info(tok));
	SHOW_STAGE(701);

	// tok_struct *tok_l = tok;
	err_num = err_cexpression();
	if(err_num) return err_num;
	while(1)
	{
	CHECK_TOK(702);
	tok_struct *tok0=tok;
	switch(tok->ttype){
		case TOK_OR: {	/* TOK_OR  */
			// tok->term_function = logical_or;
			// MESG_TOK_INFO("# err_lexpression",tok);
#if	TNORMAL
			set_term_function(tok,logical_or);
#endif
			tok->tgroup=TOK_BOOL;
#if	TBNF
			// tok->bnf_group=tok->ttype;
#endif
			NTOKEN_ERR(704);
			err_num=err_lexpression();
			// MESG_TOK_INFO("#2 err_lexpression",tok0);
			stack_push("TOK_OR",tok0,tok0->ttype);
			if(err_num) return err_num;
			CHECK_TOK(706);
			simple=0;
			continue;
		};
		case TOK_XOR: {	
			// tok->term_function = logical_xor;
			// MESG_TOK_INFO("# err_lexpression",tok);
#if	TNORMAL
			set_term_function(tok,logical_xor);
#endif
#if	TBNF
			// tok->bnf_group=tok->ttype;
#endif
			tok->tgroup=TOK_BOOL;
			NTOKEN_ERR(709);
			err_num=err_lexpression();
			// MESG_TOK_INFO("#2 err_lexpression",tok0);
			stack_push(" TOK_XOR",tok0,tok0->ttype);
			if(err_num) return err_num;
			CHECK_TOK(712);
			simple=0;
			continue;
		};
		case TOK_NOR: {	
			// tok->term_function = logical_nor;
			// MESG_TOK_INFO("# err_lexpression",tok);
#if	TNORMAL
			set_term_function(tok,logical_nor);
#endif
			tok->tgroup=TOK_BOOL;
#if	TBNF
			// tok->bnf_group=tok->ttype;
#endif
			NTOKEN_ERR(7091);
			err_num=err_lexpression();
			// MESG_TOK_INFO("#2 err_lexpression",tok0);
			stack_push("TOK_NOR",tok0,tok0->ttype);
			if(err_num) return err_num;
			CHECK_TOK(712);
			simple=0;
			continue;
		};
		case TOK_EQUAL:
		case TOK_NOTEQUAL:
		case TOK_SMALLER:
		case TOK_BIGGER:
		case TOK_SMALLEREQ:
		case TOK_BIGGEREQ:
			tok->tgroup=TOK_BOOL;
#if	TBNF
			tok->bnf_group=tok->ttype;
#endif
			NTOKEN_ERR(708);
			err_num=err_cexpression();
			stack_push("compare",tok0,tok0->ttype);
			simple=0;
			continue;
		case TOK_AND: {	
			// tok->term_function = logical_and;
			// MESG_TOK_INFO("# err_lexpression",tok);
#if	TNORMAL
			set_term_function(tok,logical_and);
#endif
			tok->tgroup=TOK_BOOL;
#if	TBNF
			// tok->bnf_group=tok->ttype;
#endif
			NTOKEN_ERR(709);
			err_num=err_cexpression();
			// MESG_TOK_INFO("#2 err_lexpression",tok0);
			stack_push("TOK_AND",tok0,tok0->ttype);
			if(err_num) return err_num;
			CHECK_TOK(712);
			simple=0;
			continue;
		};
		case TOK_NAND: {	
			// tok->term_function = logical_nand;
			// MESG_TOK_INFO("# err_lexpression",tok);
#if	TNORMAL
			set_term_function(tok,logical_nand);
#endif
			tok->tgroup=TOK_BOOL;
#if	TBNF
			// tok->bnf_group=tok->ttype;
#endif
			NTOKEN_ERR(7092);
			err_num=err_cexpression();
			// MESG_TOK_INFO("#2 err_lexpression",tok0);
			stack_push("TOK_NAND",tok0,tok0->ttype);
			if(err_num) return err_num;
			CHECK_TOK(712);
			simple=0;
			continue;
		};
		case TOK_ASSIGN: {
			// tok->term_function = assign_val;
#if	TNORMAL
			set_term_function(tok,assign_val);
#endif
			// MESG_TOK_INFO("# err_lexpression",tok);
			NTOKEN_ERR(710);
			err_num=err_assign_val();
#if	TBNF
			tok_struct *tok0_bnf_assign=stack_push("TOK_ASSIGN 1",tok0,tok0->ttype);
			if(assign_type_to) {
				
				if(assign_type_to==TOK_ASSIGN_ARRAY2) {
					tok0_bnf_assign->ttype=TOK_ASSIGN_ARRAY2;
					tok0_bnf_assign->bnf_factor_function=bnf_assign_array2;
					// MESG("; for [%s] set assign to type %d,bnf_assign_array2",tok_info(tok0_bnf_assign));
				}; if(assign_type_to==TOK_ASSIGN_ARRAY1) {
					tok0_bnf_assign->ttype=TOK_ASSIGN_ARRAY1;
					tok0_bnf_assign->bnf_factor_function=bnf_assign_array1;
					// MESG("; for [%s] set assign to type %d,bnf_assign_array1",tok_info(tok0_bnf_assign));
				};
				if(assign_type_to==TOK_ASSIGN_TYPE) {
					tok0_bnf_assign->ttype=TOK_ASSIGN_ARRAY1;
					tok0_bnf_assign->bnf_factor_function=bnf_assign_type_el1;
					// MESG("; for [%s] set assign to type %d,bnf_assign_type_el1",tok_info(tok0_bnf_assign));
				};
				assign_type_to=0;
			};
#endif
			RT_MESG1(714);
		};
		case TOK_INCREASEBY: {
			// tok->term_function = increase_by;
			// MESG_TOK_INFO("# err_lexpression",tok);
#if	TNORMAL
			set_term_function(tok,(TFunction)increase_by);
#endif
			tok->tname = "+=";
			// tok0=tok;
			NTOKEN_ERR(710);
			err_num=err_increase_by();
#if	TBNF
			tok_struct *dest=stack_push(tok0->tname,tok0,tok0->ttype);
			if(assign_type_to) {
				if(assign_type_to==TOK_ASSIGN_ARRAY2) {
					dest->tname="A2+=";
					dest->bnf_factor_function=bnf_increaseby_array2;
					// MESG("; for [%s] set assign to type %d,bnf_increaseby_array2",tok_info(dest));
				}; if(assign_type_to==TOK_ASSIGN_ARRAY1) {
					// tok->ttype=TOK_ASSIGN_ARRAY1;
					dest->tname="A1+=";
					dest->bnf_factor_function=bnf_increaseby_array1;
					// MESG("; for [%s] set assign to type %d,bnf_increaseby_array1",tok_info(dest));
				};
				 if(assign_type_to==TOK_ASSIGN_TYPE) {
					// tok->ttype=TOK_ASSIGN_ARRAY1;
					
					dest->tname="A1+=";
					dest->bnf_factor_function=bnf_increaseby_type_el1;
					// MESG("; for [%s] set assign to type %d,bnf_increaseby_array1",tok_info(dest));
				};
				assign_type_to=0;
			} 
#endif

			RT_MESG1(714);
		};
		case TOK_MULBY: {
			// tok->term_function = mul_by;
			// MESG_TOK_INFO("# err_lexpression",tok);
#if	TNORMAL
			set_term_function(tok,(TFunction)mul_by);
#endif
			tok->tname = "*=";
			// tok0=tok;
			NTOKEN_ERR(710);
			err_num=err_mul_by();
#if	TBNF
			tok_struct *dest=stack_push("*=",tok0,tok0->ttype);
			// MESG("	TOK_MULBY: assign_type_to=%d",assign_type_to,tok_info(dest));
			if(assign_type_to) {
				if(assign_type_to==TOK_ASSIGN_ARRAY2) {
					dest->tname="A2*=";
					dest->bnf_factor_function=bnf_mulby_array2;
					// MESG("; for [%s] set assign to type %d,bnf_mulby_array2",tok_info(dest));
				} if(assign_type_to==TOK_ASSIGN_ARRAY1) {
					dest->tname="A1*=";
					dest->bnf_factor_function=bnf_mulby_array1;
					// MESG("; for [%s] set assign to type %d,bnf_mulby_array1",tok_info(dest));
				};
				 if(assign_type_to==TOK_ASSIGN_TYPE) {
					// tok->ttype=TOK_ASSIGN_ARRAY1;
					
					dest->tname="A1*=";
					dest->bnf_factor_function=bnf_mulby_type_el1;
					// MESG("; for [%s] set assign to type %d,bnf_increaseby_array1",tok_info(dest));
				};
				assign_type_to=0;
			};
#endif
			RT_MESG1(714);
		};
		case TOK_DECREASEBY: {
			// tok->term_function = decrease_by;
			// MESG_TOK_INFO("# err_lexpression",tok);
#if	TNORMAL
			set_term_function(tok,(TFunction)decrease_by);
#endif
			tok->tname = "-=";
			// tok0=tok;
			NTOKEN_ERR(710);
			err_num=err_decrease_by();
#if	TBNF
			tok_struct *dest=stack_push("-=",tok0,tok0->ttype);
			if(assign_type_to) {
				
				if(assign_type_to==TOK_ASSIGN_ARRAY2) {
					dest->tname="A2-=";
					dest->bnf_factor_function=bnf_decreaseby_array2;
					// MESG("; for [%s] set assign to type %d,bnf_decreaseby_array2",tok_info(dest));
				};if(assign_type_to==TOK_ASSIGN_ARRAY1) {
					dest->tname="A1-=";
					dest->bnf_factor_function=bnf_decreaseby_array1;
					// MESG("; for [%s] set assign to type %d,bnf_decreaseby_array1",tok_info(dest));
				};
				 if(assign_type_to==TOK_ASSIGN_TYPE) {
					dest->tname="A1-=";
					dest->bnf_factor_function=bnf_decreaseby_type_el1;
					// MESG("; for [%s] set assign to type %d,bnf_increaseby_array1",tok_info(dest));
				};
				assign_type_to=0;
			};
#endif
			RT_MESG1(714);
		};
		case TOK_ASSIGNENV:
			// tok->term_function = assign_env;
			// MESG_TOK_INFO("# err_lexpression",tok);
#if	TNORMAL
			set_term_function(tok,assign_env);
#endif
#if	TBNF
			// tok->bnf_group=tok->ttype;
#endif
			NTOKEN_ERR(710);
			err_num=err_assign_env();
			// stack_push_replace("tok l",tok_l);
			stack_push("assign env",tok0,tok0->ttype);
			RT_MESG1(7141);
		case TOK_ASSIGNOPT:
			// tok->term_function = assign_option;
			// MESG_TOK_INFO("# err_lexpression",tok);
#if	TNORMAL
			set_term_function(tok,assign_option);
#endif
#if	TBNF
			tok->bnf_group=tok->ttype;
#endif
			NTOKEN_ERR(710);
			err_num=err_assign_env();
			stack_push("assign option",tok0,tok0->ttype);
			RT_MESG1(7141);
		default:
			RT_MESG1(715);
	};
 };
 RT_MESG1(717);
}

int err_cexpression()
{
 tok_struct *tok0;
 TDSERR("cexpression");

	// slval[0]=0;
	simple=1;

	SHOW_STAGE(611);

	err_num = err_num_expression();
	if(err_num) return(err_num);

	tok0=tok;
	CHECK_TOK(612);

	if(tok0->tgroup!=TOK_COMPARE) {
		RT_MESG1(617);
	};
	// MESG(" in cexpression compare: %s",tok_info(tok));
	NTOKEN_ERR(615);
	err_num=err_num_expression();
	stack_push("compare",tok0,tok0->ttype);
	RT_MESG1(616);
}

int err_check_sentence1()
{
 TDSERR("sentence");
 SHOW_STAGE(621);
 
 // MESG("err_check_sentence: ttype=%d",tok->ttype);
 switch(tok->ttype) {
	case TOK_EOF:
		MESG_TOK_INFO("# err_check_sentence",tok);
		err_num=0;
#if	TBNF
		stack_push("EOF",tok,0);
#endif
		RT_MESG1(625);

	case TOK_RCURL:
		// MESG_TOK_INFO("# err_check_sentence",tok);
#if	TBNF
		stack_push("sentence RCURL",tok,-tok->ttype);
#endif
		CHECK_TOK(626);
		break;
 	case TOK_LCURL:
	{	
		// MESG("# err_check_sentence: %s",tok_info(tok));
#if	TNORMAL
		if(execmd) {
			set_tok_directive(tok,dir_lcurl);
		}else {
			set_tok_directive(tok,dir_lcurl_break);
		};
#endif
#if	TBNF
		stack_push("sentence LCURL",tok,-tok->ttype);
#endif
		NTOKEN_ERR(627);
		CHECK_TOK(628);
		err_num=err_check_block1();
		RT_MESG1(629);	/* at the start of next sentence  */
	};

	case TOK_DIR_IF:
		{
		tok_struct *tok0=tok;
		// MESG("# err_check_sentence: %s",tok_info(tok));
#if	TNORMAL
		set_tok_directive(tok,tok_dir_if);
#endif
#if	TBNF
		tok_struct *if_token=stack_push("DIR_IF",tok,-tok->ttype);
#endif
		NTOKEN_ERR(631);	/* go to next token after if */
		xpos=632;

		err_num=err_lexpression();
		// MESG("err: TOK_DIR_IF! after lexpr");
		CHECK_TOK(633);
		if(err_num) { 
			// MESG("err: TOK_DIR_IF! error!!");
			return err_num;
		};
		// MESG("err: TOK_DIR_IF! check rpar! of %d",tok->tnum);
#if	TBNF
		stack_push("if )",tok,-tok->ttype);
#endif
		check_skip_token_err1(TOK_RPAR,"tok_dir_if",xpos);

		CHECK_TOK(634);
//		if(tok->ttype==TOK_LCURL) is_block=1; 
		err_num=err_check_sentence1();	/* body of if  */
		// MESG("	after body of if [%s]",tok_info(tok));
		// check_skip_token1(TOK_RPAR);	/* ???????????  */
		// NTOKEN_ERR(635);	/* go after sentence  */
		CHECK_TOK(635);
#if	TBNF
		tok_struct *end_if_tok=stack_push("end of if",tok,-tok->ttype);
		if_token->next_tok=end_if_tok;
#endif
		// MESG("err tok_dir_if: end next_token [%s]",tok_info(end_if_tok));

		tok0->next_tok=tok;
		// MUST: No separator before else, handle this in parser !!!!! CHECK
		// check for else statement!
		
		if(tok->ttype==TOK_DIR_ELSE) {
			// MESG("we have an else statement!");
			tok0=tok;
			NTOKEN2;
			err_num=err_check_sentence1();	/* body of else  */
			xpos=638;
			tok0->next_tok=tok;
#if	TBNF
			end_if_tok->next_tok = stack_push("end of else",tok,-tok->ttype);
			end_if_tok->next_tok--;
#endif
		} else {
			xpos=639;
		};
		RT_MESG1(xpos);
		};
	case TOK_DIR_FORI:
		{
		tok_struct *start_block;	// element at block start
		tok_struct *end_block;	/* at the block end  */
		// MESG("# err_check_sentence: %s",tok_info(tok));
#if	TNORMAL
		set_tok_directive(tok,tok_dir_fori);
#endif
#if	TBNF
		stack_push("DIR_FORI",tok,-tok->ttype);
#endif
		NTOKEN_ERR(640);	/* go to next token after for */
#if	TBNF
		tok_struct *tok0=NULL;
		tok_struct *tok_var=tok;
		tok_struct *tok0_bnf=NULL;
#endif
		if(tok->ttype==TOK_VAR) {
			// MESG_TOK_INFO(" loop var",tok);
#if	TBNF
			tok->bnf_group=TOK_BOOL;
			tok0_bnf=stack_push("loop var",tok,tok->ttype);
#endif
			NTOKEN_ERR(6403);
			if(tok->ttype!=TOK_ASSIGN) {
				ERROR("6404:for i error");
			} else {
#if	TBNF
				tok0=tok;
#endif
				// MESG_TOK_INFO(" equal to",tok);
			};
		} else ERROR("6405:for i syntax error");
		NTOKEN_ERR(64053);
		err_num=err_num_expression();	/* initial   */
#if	TBNF
		tok0_bnf=stack_push("for i assign",tok0,tok0->ttype);
		tok0_bnf->tname = tok_var->tname;
		tok0_bnf->tind = tok_var->tind;
		tok0_bnf->bnf_factor_function=bnf_factor_assign_iterator;
#endif
		if(err_num) return(err_num);
		CHECK_TOK(6406);
#if	TBNF
		stack_push("fori sep 1",tok,0);
#endif
		NTOKEN_ERR(6407);	/* skip separator! */
		err_num=err_num_expression();
		if(err_num) return(err_num);
#if	TBNF
		stack_push("fori sep 2",tok,-tok->ttype);
#endif
		NTOKEN_ERR(6408);
		err_num=err_num_expression();
		if(err_num) return(err_num);
		CHECK_TOK(6409);
#if	TBNF
		stack_push("fori )",tok,-tok->ttype);
#endif
		if(tok->ttype!=TOK_RPAR) ERROR("6409:for i: error ");
		NTOKEN_ERR(64010);	/* skip right parenthesis  */
		// set block start
		start_block=tok;
		CHECK_TOK(64011);
#if	1
		skip_sentence1();
		end_block=tok;
#else
		if(tok->ttype==TOK_LCURL) {
			end_block=tok->match_tok; 
			end_block++;
			is_block=1;
		} else {
			skip_sentence1();
			end_block=tok;
		};
#endif
		tok=start_block;
		err_num=err_check_sentence1();
		CHECK_TOK(64015);
#if	1
		tok=end_block;
#else
		if(is_block) tok=end_block;
		else skip_sentence1();
#endif
		CHECK_TOK(64020);
		}; 
		break;
	case TOK_DIR_FOR:
		{
		tok_struct *start_block;	// element at block start
		// MESG("# err_check_sentence: %s",tok_info(tok));
#if	TNORMAL
		set_tok_directive(tok,tok_dir_for);
#endif
#if	TBNF
		stack_push("dir for",tok,-tok->ttype);
#endif
		NTOKEN_ERR(641);	/* go to next token after for */

		err_num=err_lexpression();	/* check initial  */
#if	TBNF
		stack_push("sep",tok,0);
#endif
		NTOKEN_ERR(643);	/* skip separator! */
			
		// set check_list
		err_num=err_lexpression();
#if	TBNF
		stack_push("sep",tok,0);
#endif
		NTOKEN_ERR(645);

		err_num=err_lexpression();	/* check loop statement  */
		CHECK_TOK(647);
#if	TBNF
		stack_push("for )",tok,-tok->ttype);
#endif
		if(!check_skip_token1(TOK_RPAR)) { set_error(tok,xpos,"RPAR"); RT_MESG1(6471);};

		start_block=tok;

		// set tok pointer here and check the loop body
		{
			CHECK_TOK(651);
			tok=start_block;
			err_num=err_check_sentence1();	/* check syntax for block  */
			CHECK_TOK(652);
		};
		}; 
		break;
	case TOK_DIR_WHILE:
		{
			tok_struct *check_element; // check element pointer
			tok_struct *start_block;	// element at block start
			// MESG("# err_check_sentence: %s",tok_info(tok));
#if	TNORMAL
			set_tok_directive(tok,tok_dir_while);
#endif
#if	TBNF
			stack_push("while",tok,-tok->ttype);
#endif
			NTOKEN_ERR(654);	/* go to next toke after while */

			// set check_list
			check_element=tok;
			err_num=err_check_sentence1();
			CHECK_TOK(657);
#if	TBNF
			stack_push("while )",tok,-tok->ttype);
#endif
			check_skip_token_err1(TOK_RPAR,"tok_dir_while:",xpos);
			CHECK_TOK(658);
			start_block=tok;
	
			// set check while body
			tok=check_element;
			err_num=err_lexpression();

			CHECK_TOK(661);
			tok=start_block;
			err_num=err_check_sentence1();
			CHECK_TOK(662);
		}
		break;

	case TOK_DIR_BREAK:
		// MESG("# err_check_sentence: %s",tok_info(tok));
#if	TNORMAL
		set_tok_directive(tok,dir_break);
#endif
#if	TBNF
		stack_push("dir_break",tok,-tok->ttype);
#endif
		NTOKEN_ERR(663);
		RT_MESG;
	case TOK_DIR_RETURN:{
		// MESG("# err_check_sentence: %s",tok_info(tok));
#if	TNORMAL
		set_tok_directive(tok,dir_return);
#endif

		// MESG(" err TOK_DIR_RetURN");
#if	TBNF
		tok_struct *dest=stack_push("dir_return",tok,-tok->ttype);
#endif
		NTOKEN_ERR(664);
		if(tok->ttype!=TOK_SEP&&tok->ttype!=TOK_RPAR&&tok->ttype!=TOK_RCURL) {	
			err_num=err_lexpression();
#if	TBNF
			dest->bnf_factor_function = bnf_dir_return_value;
#endif
		} else {
#if	TBNF
			dest->bnf_factor_function = bnf_dir_return_novalue;
#endif
		};
#if	TBNF
		stack_push("dir_return )",tok,-tok->ttype);
#endif
		check_skip_token1(TOK_RPAR);
		// MESG(" err TOK_DIR_RETURN: after lexpression: tname=[%s] tnum=%d ttype=%d",tok->tname,tok->tnum,tok->ttype); 
		};RT_MESG1(666);
	case TOK_SEP:
#if	TBNF
		stack_push("sep ",tok,-tok->ttype);
#endif
		MESG_TOK_INFO("# err_check_sentence",tok);
		RT_MESG;
	case TOK_DIR_TYPE:
		{
		// MESG("# err_check_sentence: %s",tok_info(tok));
#if	TNORMAL
		set_tok_directive(tok,tok_dir_type);
#endif
#if	TBNF
		stack_push("dir type",tok,-tok->ttype);
#endif
		// MESG("--> err: TOK_DIR_TYPE: %s",tok->tname);
		NTOKEN_ERR(671);	// this is the type name
		xpos=632;
	};break;
	default:
		CHECK_TOK(623);
		err_num=err_lexpression();
		CHECK_TOK(668);
 };
 RT_MESG1(669); 
}

int err_check_block1()
{
 TDSERR("block");
   SHOW_STAGE(671);
	
   // MESG("err_check_block1: [%s] <-- [%s]",check_buffer->b_fname,tok_info(tok));
   while(1) {
	CHECK_TOK(672);
	// MESG(" - %3d: %-15s %3d %3d",tok->tnum,tok->tname,tok->ttype,tok->tind);
	switch(tok->ttype) {
		case TOK_EOF: 
			// MESG("err_check_block1: [%s] --> [%s]", check_buffer->b_fname,tok_info(tok));
#if	TBNF
			stack_push("EOF ",tok,-tok->ttype);
#endif
			RT_MESG1(673);
		case TOK_SEP:
			// MESG_TOK_INFO("# err_check_block1",tok);
#if	TBNF
			stack_push("--------------",tok,-tok->ttype);
#endif
			NTOKEN_ERR(6741);
			continue;
		case TOK_RPAR:	/* problem if removed  */
			// MESG_TOK_INFO("# err_check_block1",tok);
#if	TBNF
			stack_push("RPAR ",tok,-tok->ttype);
#endif
			NTOKEN_ERR(6741);
			continue;
		case TOK_COMMA:
		case TOK_SHOW:
			// MESG_TOK_INFO("# err_check_block1",tok);
#if	TBNF
			stack_push("Show or comma",tok,-tok->ttype);
#endif
#if	TNORMAL
			tok->factor_function = factor_funcs[tok->ttype];
#endif
			NTOKEN_ERR(674);
#if	1
			if(tok->ttype==TOK_SEP) {
#if	TBNF
				tok_struct *dest=stack_push("sep after ddot",tok,-tok->ttype);
				dest->bnf_factor_function=bnf_factor_sep0;
#endif
				NTOKEN_ERR(674);
			};
#endif
			continue;
		case TOK_RCURL:
#if	TBNF
			stack_push("RCURL ",tok,-tok->ttype);
#endif
			// MESG("err_check_block1: [%s] --> [%s]", check_buffer->b_fname,tok_info(tok));
			NTOKEN_ERR(675);
			RT_MESG1(6751);
		// default:
			// MESG("check_block: other %s",tok_info(tok));
	};
	// MESG("-- err_check_block: end of switch: at [%s]",tok_info(tok));
 	err_num=err_check_sentence1();
	// MESG("--- err_check_block: [%s] after sequence: [%s]",check_buffer->b_fname,tok_info(tok));
	if(err_num) return(err_num);
   };
 xpos=692;syntax_error("Null in block",xpos);
 RT_MESG1(6921);
}


/*
 Syntax of mlang

 push_args
 assign_args
 assign_val
 assign_env
 exec_function

 exec_block:
	TOK_SEP 
	| TOK_EOF
	| sentence TOK_SEP
	| sentence TOK_COMMA
	| sentence TOK_SHOW TOK_SEP 
	| TOK_RPAR
	| TOK_RCURL

 sentence:
	TOK_LCURL TOK_BLOCK TOK_RCURL
	|command
	|TOK_DIR_IF TOK_LPAR lsentence TOK_RPAR sentence TOK_ELSE sentence
	|TOK_DIR_IF TOK_LPAR lsentence TOK_RPAR sentence
	|TOK_DIR_FOR TOK_LPAR command TOK_SEP lexpression TOK_SEP command TOK_RPAR 
	|TOK_DIR_WHILE TOK_LPAR lexpression TOK_RPAR sentence
	|TOK_DIR_BREAK
	|TOK_CMD	(??) or in factor?
	|TOK_FUNC	(??) or in factor?
	

 lexpression: // command
	cexpression
	| cexpression TOK_OR lexpression ..
	| cexpression TOK_AND lexpression
	| env TOK_ASSIGN cexpression 
	| var TOK_ASSIGN cexpression
	| option TOK_ASSIGN cexpression

 cexpression:
	num_expression
	| num_expression COMP_OP num_expression	

 num_expression:
	num_term1
	|num_term1 TOK_PLUS num_term1 ..
	|num_term1 TOK_MINUS num_term1 ..
	|num_term1 TOK_OR lexpression ..

 num_term1:
	num_term2
	| num_term2 TOK_MUL num_term2 ..
	| num_term2 TOK_DIV num_term2 ..

 num_term2:
	factor
	factor TOK_MOD num_term2 ..
	factor TOK_POWER num_term2 ..

 factor:
	TOK_VAR
	| TOK_LPAR command TOK_RPAR
	| TOK_NUM
	| TOK_QUOTE
	| TOK_MINUS factor
	| TOK_PLUS factor
	| TOK_NOT factor
	| TOK_OPTION
	| TOK_ENV
	| TOK_FUNC (editor function)
	| TOK_PROC (function)
	| TOK_CMD (??) or in cmd


*/

/* -- */
