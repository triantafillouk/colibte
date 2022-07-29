/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

	interpreter syntax/error analyzer
*/

int err_lexpression();
int err_num_expression();
int err_check_block1(int level);
int err_check_sentence1();
int err_assign_val();
int err_assign_env();

void mesg_out(const char *fmt, ...)
{
 va_list args;
 static char mline[1024];
    if (fmt != NULL) {
		va_start(args,fmt);
		vsnprintf(mline,511,fmt,args);
		va_end(args);
		out_print(mline,1);
    }
}

char * tok_info2(tok_struct *tok)
{
 static char stok[MAXLLEN];

	if(tok->tname!=NULL){
		switch(tok->ttype){
			case TOK_SHOW:		
				snprintf(stok,MAXLLEN,"%d:%d %s :",tok->tline,tok->tnum,TNAME);
				break;
			case TOK_LCURL:
			case TOK_RCURL:
				snprintf(stok,MAXLLEN,"%d:%d %s other is %d",tok->tline,tok->tnum,TNAME,tok->match_tok->tnum);
				break;
			case TOK_SEP:
				snprintf(stok,MAXLLEN,"%d:%d %s",tok->tline,tok->tnum,TNAME);
				break;
			case TOK_DIR_IF:
			case TOK_DIR_ELSE:
			case TOK_DIR_FOR:
			case TOK_DIR_WHILE:
				snprintf(stok,MAXLLEN,"%d:%d %s",tok->tline,tok->tnum,TNAME);
				break;
			case TOK_QUOTE:
				snprintf(stok,MAXLLEN,"%d:%d %s \"%s\"",tok->tline,tok->tnum,TNAME,(char *)tok->tname);
				break;
			default:
				snprintf(stok,MAXLLEN,"%d:%d %s %s",tok->tline,tok->tnum,TNAME,(char *)tok->tname);
		};
	} else {
			snprintf(stok,MAXLLEN,"%2d:%3d %s [%f]",tok->tline,tok->tnum,TNAME,tok->dval);
	};
	return stok;
}

#define	SYNTAX_DEBUG1	1

#if	SYNTAX_DEBUG1	/* if debugging  */
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

// Return with message
#define RT_MESG { \
	if(err_num>0) mesg_out(" -[%s][%d] end with error %d position %d line %d",Tds,tok->tnum,err_num,xpos,tok->tline);\
	stage_level--;return(err_num);\
}

#define RT_MESG1(pos) { \
	CHECK_TOK(pos);\
	stage_level--;return(err_num);\
}

#endif


#define SHOW_STAGE(pos)	{ stage_level++;show_type='#';CHECK_TOK(pos);}

#define	CHECK_TOK(pos) { xpos=pos;\
		if(err_num>0) {\
			show_type=' ';\
			mesg_out("!err [%2d][%4d]%*s-%s: [%s]",stage_level,xpos,stage_level,"",Tds,tok_info2(tok));\
			return(err_num);\
		} else {\
			if(show_stage==1) mesg_out(" %c[%2d][%4d]%*s-%s:[%s]",show_type,stage_level,xpos,stage_level,"",Tds,tok_info2(tok));\
			show_type=' ';\
		}\
}

#define	NTOKEN_ERR(xxx)	{ tok++;show_type=';';CHECK_TOK(xxx);}

#define TDSERR(description) char *Tds=description;

int parse_level=0;
char show_type=' ';

int check_skip_token1( int type)
{
 	if(tok->ttype==type) 
 	{ 
		NTOKEN2;
 		return(1);
 	} else {
		return(0);
	};
}

int check_skip_token_err1(int type,char *mesg,int err)
{
 	if(tok->ttype==type) 
 	{ 
		NTOKEN2;
 		return(0);
 	} else {
		err_str=mesg;
		err_num=err;
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
 if(tok==NULL) {
	err_num=err;
	err_str=description;
	return;
 };
 err_line=tok->tline;
 err_num=err;
 err_str=strdup(description);
}

void syntax_error(char *description,int err)
{
 if(err_num>0) return;	/* error already set  */
 set_error(tok,err,description);
}

int  err_eval_fun1(int fnum)
{
	TDSERR("eval_fun1");
	int i,ia;
	int f_entry;
	char err_message[512];

	ia=m_functions[fnum].f_args;

	f_entry=entry_mode;
	entry_mode=KNORMAL;

	SHOW_STAGE(401);

	if(ia) {
		/* if we have arguments, check for parenthesis, then get the arguments  */
		if(tok->ttype!=TOK_LPAR) {
			snprintf(err_message,512,"function [%s] with %d arguments without left parenthesis!",m_functions[fnum].f_name,m_functions[fnum].f_args);
			xpos=402;
			syntax_error(err_message,xpos);
			RT_MESG;
		} ;

		for(i=0;i< ia;i++) { 
			NTOKEN_ERR(403);
			err_num = err_lexpression();
			CHECK_TOK(405);
		};
		xpos=406;
		check_skip_token_err1(TOK_RPAR,"eval_fun1: error closing parenthesis",xpos);
		CHECK_TOK(406);
	} else {;
		CHECK_TOK(407);
		if(tok->ttype==TOK_LPAR){
			NTOKEN_ERR(4071);
			if(tok->ttype!=TOK_RPAR) syntax_error("missing right parenthesis",xpos);
			else NTOKEN_ERR(4072);
		};
	}
	entry_mode=f_entry;
	/* and now evaluate it! */
	RT_MESG1(408);
}

int  err_push_args_1(int *nargs)
{
 int i=0;
 int num_args=0;
 TDSERR("push_args");

 SHOW_STAGE(410);
 if(tok->ttype!=TOK_RPAR) {
#if	1
 if(tok->ttype==TOK_SEP) {
		*nargs=0;
		set_error(tok,411,"no parenthesis on function call!");
//		NTOKEN_ERR(411);
		return(err_num);
 };
#endif
 
 while(1){
	ex_vtype=VTYPE_NUM;
	slval[0]=0;
	xpos=412;
	
	if(tok->ttype==TOK_RPAR) {
		break;
	};
	err_num = err_lexpression();
	CHECK_TOK(413);
	num_args++;

	if(err_num) { 
		set_error(tok,err_num,"after push_args");
		*nargs=num_args;
		return(err_num);
	};
	i++;
	xpos=415;
	if(tok->ttype==TOK_RPAR) {
		break;
	} else if (tok->ttype==TOK_COMMA) {
		NTOKEN_ERR(416);
		continue;
	}
	else {
		*nargs=num_args;
		xpos=417;
		set_error(tok,xpos,"after push_args:2");
//		clear_args(va); 
		return(err_num);
	}
 };
	*nargs=num_args;
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
 
 NTOKEN_ERR(4201); /* skip name */
 
 if(nargs!=0) {
	xpos=421;
	if(tok->ttype!=TOK_LPAR) err_num=xpos;

	NTOKEN_ERR(4211);	/* skip left parenthesis  */
	for(i=0;i<nargs;i++) {
		xpos=422;
		if(tok->ttype==TOK_RPAR) {
			break;	/* end of arguments!  */
		}
		xpos=423;	/* this should be a var token  */

		NTOKEN_ERR(424);	/* skip semicolon or end parenthesis */

		if(tok->ttype==TOK_RPAR)  {
			break;
		}
		if(tok->ttype!=TOK_COMMA) { set_error(tok,xpos,"not separator between args"); RT_MESG1(426);};
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

		NTOKEN_ERR(432);	/* skip semicolon or end parenthesis */

		if(tok->ttype==TOK_RPAR)  {
			break;
		};
		NTOKEN_ERR(433);
		i++;
	} ;
 };
 NTOKEN_ERR(434);
 return(0);
}


int err_assign_val()
{
 TDSERR("assign_val");
 SHOW_STAGE(440);

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

	if(!simple){
		xpos=443;
		syntax_error("bogus assignement!!!!",xpos);
		RT_MESG1(443);
	};
	err_num=err_lexpression();

	RT_MESG1(443);
}

int err_decrease_by()
{
 TDSERR("decrease_by");
 SHOW_STAGE(443);

	if(!simple){
		xpos=443;
		syntax_error("bogus assignement!!!!",xpos);
		RT_MESG1(443);
	};
	err_num=err_lexpression();

	RT_MESG1(443);
}

int err_increase_val()
{
 TDSERR("increase_val");
 SHOW_STAGE(444);
 if(ex_vtype!=VTYPE_NUM) {
	set_error(tok,444,"increase_val not numeric!");
	NTOKEN_ERR(444);
	return(444);
 };
 NTOKEN_ERR(444);
 RT_MESG1(444);
}

int err_decrease_val()
{
 TDSERR("decrease_val");
 SHOW_STAGE(446);
 if(ex_vtype!=VTYPE_NUM) {
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

 SHOW_STAGE(450);
	if(!simple){
		xpos=451;
		syntax_error("bogus assignement!!!!",xpos);
		RT_MESG1(451);
	};xpos=452;
	err_num=err_lexpression();
	RT_MESG1(453);
}

int err_exec_function(char *name,int nargs,FILEBUF **bf)
{
 TDSERR("exec_function");
 int save_stage_level=stage_level;

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

	parsed=parse_block1(bp,NULL,0,0);	/* do not init if already parsed!, returns 0 if parsed  */
	if(parsed==0) {	/* already parsed, no need to check again!  */
		RT_MESG1(463);
	};
	/* and now execute it as asked */
	if((check_init(bp))>0) {
		ERROR("found syntax errors in function!");
		RT_MESG1(464);
	};

	stage_level=save_stage_level;
	
	CHECK_TOK(465);
	tok=bp->tok_table;
	err_num=err_assign_args1(nargs);
	tok=bp->tok_table;
	CHECK_TOK(466);
	err_num=err_check_sentence1();

	RT_MESG1(467);
}


int err_factor()
{
 static int pre_symbol=0;
 TDSERR("factor");
 int save_macro_exec;
 tok_struct *tok0; 

 ex_vtype=VTYPE_NUM;
 ex_edenv=0;

 SHOW_STAGE(470);
 tok0=tok;
 switch(tok0->ttype) {
 	case TOK_PLUS:
	case TOK_MINUS:
		pre_symbol++;
 };
 if(tok0->ttype==TOK_INCREASE) {
 	MESG("increase %d ",tok0->tnum);
	xpos=471;
	RT_MESG1(xpos);
 };
 if(tok0->ttype==TOK_DECREASE) {
 	MESG("decrease %d ",tok0->tnum);
	xpos=472;
	RT_MESG1(xpos);
 };
 NTOKEN_ERR(473);
 tok0->factor_function = factor_funcs[tok0->ttype];
 switch(tok0->ttype) {
	/*  the following ends factor  */
 	case TOK_SEP:
		xpos=476;syntax_error("separator in factor!",xpos);
		RT_MESG1(xpos);
	case TOK_SHOW:
		xpos=477;syntax_error(": in factor",xpos);
		RT_MESG1(xpos);
	case TOK_LBRAKET:{	/* array definition  */
		pre_symbol=0;
		int i=0,j=0;
		int cdim=0;
		int rows=1,cols=0;
		cdim=1;
		ex_nvars=0;ex_nquote=0;ex_nums=0;	/* initialize table counters  */
		ex_array=NULL;
		while(cdim>0){
		while(1) {
			if(tok->ttype==TOK_EOF||tok->ttype==TOK_SHOW) {
				xpos=4777;
				syntax_error(" FAC1_err: No closing braket",xpos);
				RT_MESG1(xpos);
			};
			if(tok->ttype==TOK_SEP) {
				NTOKEN2;
				continue;
			};
			err_num=err_num_expression();
			i++;if(i>cols) cols=i;
			if(tok->ttype==TOK_RBRAKET) {
				cdim=0;break;
			};
			if(tok->ttype==TOK_COMMA) {
				i=0;j++;
				cdim++;if(cdim>rows) rows=cdim;
				NTOKEN2;
				continue;
			};
		};cdim--;
		};
		// set array dat
		if(tok0->adat) {
			free(tok0->adat);
		};
		tok0->adat = new_array(rows,cols);
		// set end
		NTOKEN_ERR(4789)
		RT_MESG1(4789);
		};
	case TOK_RPAR:
		xpos=479;
		pnum--;
		syntax_error("wrong argument number",xpos);
		RT_MESG1(480);
	/* start of logic ---------  */
	case TOK_VAR:{	// 0 variable
		pre_symbol=0;
		ex_nvars++;
		RT_MESG1(493);}
	case TOK_ARRAY1:{
		err_num=err_factor(); 
		RT_MESG1(4931);
		};
	case TOK_ARRAY2:{
		err_num=err_factor(); 
		err_num=err_factor(); 
		RT_MESG1(4932);
		};
	case TOK_LPAR:
		{
			pnum++;
			pre_symbol=0;
			CHECK_TOK(481);
			{
				if(tok->ttype==TOK_RPAR) {	/* null expression  */
					pnum--;
					NTOKEN_ERR(483);
					RT_MESG;	/* return right after right parenthesis  */
				};
			};
			err_num = err_lexpression();
			CHECK_TOK(484);
			if(tok->ttype !=TOK_RPAR) {
				xpos=485;
				syntax_error(" FAC1_err: No closing parenthesis",xpos);
				RT_MESG1(485);
			} else { 
				pnum--;
				NTOKEN_ERR(4851);
			};
			RT_MESG1(486);
	 	};
		break;	
	case TOK_NUM:
		pre_symbol=0;
		ex_nums++;
		RT_MESG1(487);
	case TOK_QUOTE:	 { // string 
		xpos=488;
		if(pre_symbol) { syntax_error("symbol before string",xpos);RT_MESG1(4881);};
		ex_vtype=VTYPE_STRING;
		ex_nquote++;
		RT_MESG1(4882);
	 	};
	case TOK_BQUOTE: {
		xpos=489;
		ex_vtype=VTYPE_NUM;
		RT_MESG1(4891);
	};
	case TOK_MINUS:
	case TOK_PLUS:
		if(pre_symbol>1) {
			xpos=490;
			syntax_error("too many symbols(+) infront of factor",xpos);
			RT_MESG1(490);
		};
		CHECK_TOK(491);
		err_factor();
		RT_MESG1(4911);
	case TOK_NOT:
		xpos=492;
		if(pre_symbol) { syntax_error("symbol before not",xpos);RT_MESG;};
		pre_symbol=0;
		CHECK_TOK(xpos);
		return(err_factor());
	case TOK_OPTION:// 5 editor option
		/* variable's name in tok0->tname */
		xpos=494;
		ex_nvars++;
		var_node=tok0->tnode;

		ex_edenv=0;
		if(var_node->sval!=NULL) { /* there is a valid string value CHECK !!*/
			ex_vtype=VTYPE_STRING;
		} else ex_vtype=VTYPE_NUM;
		pre_symbol=0;
		RT_MESG1(xpos);
	case TOK_ENV:	// 1 editor env var
		/* variable's name in tok0->tname */
		var_node=tok0->tnode;
		pre_symbol=0;
		ex_nvars++;
		RT_MESG1(495);
	case TOK_FUNC:	// 2 editor function 
		/* variable's name in tok0->tname */
		var_node=tok0->tnode;
		pre_symbol=0;
		CHECK_TOK(496);
		err_num= err_eval_fun1(var_node->node_index);
		RT_MESG1(497);
	case TOK_PROC: {	// 4 ex_proc (normal function)
		int nargs=0;
		FILEBUF *bp;
		tok_struct *after_proc;	// this is needed for recursive functions
		xpos=501;
		ex_nvars++;

		if(tok->ttype!=TOK_SEP)
		NTOKEN_ERR(502);	/* this is parenthesis or separator */
		/* function */
		pre_symbol=0;

		err_num = err_push_args_1(&nargs);

		tok0->tind=nargs;
		CHECK_TOK(503);
		after_proc=tok;
		err_num=err_exec_function(tok0->tname,nargs,&bp);
		if(bp!=NULL) {
			tok0->tbuf=bp;
		};
		CHECK_TOK(504);
		tok=after_proc;

		NTOKEN_ERR(505);	/* skip parenthesis */
		RT_MESG;
	};
	case TOK_CMD:	{ // 3 editor command
		int var_index;
		int check_par;

		/* variable's name in tok0->tname */
		xpos=506;
		var_node=tok0->tnode;
		var_index = var_node->node_index;

		pre_symbol=0;
		if(ftable[var_index].arg==0) check_par=0;else check_par=1;
		if(check_par) 
		{	xpos=507;
			check_skip_token_err1(TOK_LPAR,"tok_cmd:0",xpos);
			CHECK_TOK(xpos);
			pnum++;

			if(pnum>10) {
				CHECK_TOK(508);
				return xpos;	/* recursion should not be checked at this stage  */
			};
		} else {
			CHECK_TOK(509);
			if(tok->ttype==TOK_LPAR) { /* no arguments but still LPAR */
				NTOKEN_ERR(510);
			}
		};
		xpos=5152;
		save_macro_exec=macro_exec;
		macro_exec=MACRO_MODE2;
		{
			int args=ftable[var_index].arg;
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
				CHECK_TOK(517);

				if(!check_end && tok->ttype==TOK_RPAR) {
					CHECK_TOK(5171);
					break;
				};
				if(i<args-1) {	/* check for a comma  */
					CHECK_TOK(5172);
					if(tok->ttype==TOK_COMMA || check_end==0) {
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
		xpos=526;
		set_error(tok,xpos,"else without if error");
		RT_MESG1(5261);
	case TOK_ASSIGN:
	case TOK_INCREASEBY:
	case TOK_DECREASEBY:
	case TOK_INCREASE:
	case TOK_DECREASE:
		RT_MESG1(527);
	default:
		xpos=527;
		set_error(tok,3000+tok->ttype,"factor :wrong character found:");
		RT_MESG1(5271);
 }
 xpos=529;
 set_error(tok,xpos,"FAC1_err: end, ERROR should not pass from here!");
 RT_MESG1(5291);
}

int err_num_term2()
{
 TDSERR("num_term2");

 SHOW_STAGE(541);
 err_num = err_factor();
 
 if(tok->tgroup==TOK_TERM2) {
 	CHECK_TOK(543);
	tok->term_function = factor_funcs[tok->ttype];
	NTOKEN_ERR(544);
	err_num = err_num_term2();
	RT_MESG1(545);
 };
 RT_MESG1(548);
}

/* multiply & divide */
int err_num_term1()
{
 TDSERR("term1_1");
 
 SHOW_STAGE(551);
 err_num=err_num_term2();
 CHECK_TOK(552);

 while (tok->tgroup==TOK_TERM1) {
 	tok->term_function = factor_funcs[tok->ttype];
	CHECK_TOK(553);
	NTOKEN_ERR(5531);
	err_num=err_num_term2();
	CHECK_TOK(554);
 };
 RT_MESG1(558);
}

int err_num_expression()
{
 TDSERR("num_expression");
 int expression_type=VTYPE_NUM;

 ex_vtype=VTYPE_NUM;
 slval[0]=0;
 xpos=561;

 SHOW_STAGE(561);
 err_num = err_num_term1();

 expression_type=ex_vtype;	// set local value

 while (tok->tgroup==TOK_TERM) {
   CHECK_TOK(563);
    if(tok->ttype==TOK_PLUS) {	/* TOK_PLUS  */
		tok->term_function = term_plus;
		NTOKEN_ERR(568);
		err_num=err_num_term1();
		CHECK_TOK(569);
		simple=0;
		/* check validity of operation combinations   */
		if(expression_type==VTYPE_STRING) { /* catanate string */
			if(ex_vtype) {
			} else {
				expression_type=ex_vtype=VTYPE_STRING;
			}
		} else {
		}
	};
	if(tok->ttype==TOK_MINUS) {	/* TOK_MINUS  */
		tok->term_function = term_minus;
		NTOKEN_ERR(571);
		if(expression_type==VTYPE_STRING) {	// operator on first chars of strings. numeric result
			err_num=err_num_term1();
			CHECK_TOK(573);
			simple=0;
		} else 	{
			err_num=err_num_term1();
			CHECK_TOK(574);
		}
	};
 };
 RT_MESG1(599);
}

int err_cexpression();

int err_lexpression()
{
 TDSERR("lexpression");

	slval[0]=0;
	simple=1;

	SHOW_STAGE(701);
	err_num = err_cexpression();
	while(1)
	{
	CHECK_TOK(702);
	switch(tok->ttype){
		case TOK_OR: {	/* TOK_OR  */
			tok->term_function = logical_or;
			NTOKEN_ERR(704);
			err_num=err_lexpression();
			CHECK_TOK(706);
			simple=0;
			continue;
		};
		case TOK_XOR: {	
			tok->term_function = logical_xor;
			NTOKEN_ERR(709);
			err_num=err_lexpression();
			CHECK_TOK(712);
			simple=0;
			continue;
		};
		case TOK_NOR: {	
			tok->term_function = logical_nor;
			NTOKEN_ERR(7091);
			err_num=err_lexpression();
			CHECK_TOK(712);
			simple=0;
			continue;
		};
		case TOK_AND: {	
			tok->term_function = logical_and;
			NTOKEN_ERR(709);
			err_num=err_lexpression();
			CHECK_TOK(712);
			simple=0;
			continue;
		};
		case TOK_NAND: {	
			tok->term_function = logical_nand;
			NTOKEN_ERR(7092);
			err_num=err_lexpression();
			CHECK_TOK(712);
			simple=0;
			continue;
		};
		case TOK_ASSIGN: {
			tok->term_function = assign_val;
			NTOKEN_ERR(710);
			err_num=err_assign_val();
			RT_MESG1(714);
		};
		case TOK_INCREASEBY: {
			tok->term_function = increase_by;
			NTOKEN_ERR(710);
			err_num=err_increase_by();
			RT_MESG1(714);
		};
		case TOK_DECREASEBY: {
			tok->term_function = decrease_by;
			NTOKEN_ERR(710);
			err_num=err_decrease_by();
			RT_MESG1(714);
		};
		case TOK_ASSIGNENV:
			tok->term_function = assign_env;
			NTOKEN_ERR(710);
			err_num=err_assign_env();
			RT_MESG1(7141);
		case TOK_ASSIGNOPT:
			tok->term_function = assign_option;
			NTOKEN_ERR(710);
			err_num=err_assign_env();
			RT_MESG1(7141);
		case TOK_INCREASE:
			// tok->factor_function = increase_val; 
			tok->factor_function = factor_funcs[tok->ttype];
			NTOKEN_ERR(710);
			err_num=err_increase_val();
			RT_MESG1(7142);
		case TOK_DECREASE:
			// tok->factor_function = increase_val; 
			tok->factor_function = factor_funcs[tok->ttype];
			NTOKEN_ERR(710);
			err_num=err_decrease_val();
			RT_MESG1(7142);
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

	slval[0]=0;
	simple=1;

	SHOW_STAGE(611);

	err_num = err_num_expression();
	tok0=tok;
	CHECK_TOK(612);

	if(tok0->tgroup!=TOK_COMPARE) {
		RT_MESG1(617);
	};

	NTOKEN_ERR(615);
	err_num=err_num_expression();
	RT_MESG1(616);
}

int err_check_sentence1()
{
 TDSERR("sentence");
 SHOW_STAGE(621);

 switch(tok->ttype) {
	case TOK_EOF:
		err_num=0;
		RT_MESG1(625);

	case TOK_RCURL:
		CHECK_TOK(626);
		break;
 	case TOK_LCURL:
	{	
		NTOKEN_ERR(627);
		CHECK_TOK(628);
		err_num=err_check_block1(tok->level);
		RT_MESG1(629);	/* at the start of next sentence  */
	};

	case TOK_DIR_IF:
		{
//		int is_block=0;
		NTOKEN_ERR(631);	/* go to next token after if */
		xpos=632;
		check_skip_token_err1(TOK_LPAR,"tok_dir_if",xpos);
		CHECK_TOK(6321);
		err_num=err_lexpression();
		CHECK_TOK(633);

		check_skip_token_err1(TOK_RPAR,"tok_dir_if",xpos);

		CHECK_TOK(634);
//		if(tok->ttype==TOK_LCURL) is_block=1; 
		err_num=err_check_sentence1();	/* body of if  */
		CHECK_TOK(635);

		// MUST: No separator before else, handle this in parser !!!!! CHECK
		// check for else statement!
		if(check_skip_token1(TOK_DIR_ELSE))
		{
			err_num=err_check_sentence1();	/* body of else  */
			xpos=638;
		} else xpos=639;

		RT_MESG1(xpos);
		};
	case TOK_DIR_FORI:
		{
		tok_struct *start_block;	// element at block start
		tok_struct *end_block;	/* at the block end  */
		int is_block=0;
		NTOKEN_ERR(640);	/* go to next token after for */
		NTOKEN_ERR(6401);	/* skip left parenthesis  */
		if(tok->ttype==TOK_VAR) {
			NTOKEN_ERR(6403);
			if(tok->ttype!=TOK_ASSIGN) ERROR("6404:for i error");
		} else ERROR("6405:for i syntax error");
		NTOKEN_ERR(64053);
		err_num=err_num_expression();	/* initial   */
		CHECK_TOK(6406);
		NTOKEN_ERR(6407);	/* skip separator! */
		err_num=err_num_expression();
		NTOKEN_ERR(6408);
		err_num=err_num_expression();
		CHECK_TOK(6409);
		if(tok->ttype!=TOK_RPAR) ERROR("6409:for i: error ");
		NTOKEN_ERR(64010);	/* skip right parenthesis  */
		// set block start
		start_block=tok;
		CHECK_TOK(64011);
		if(tok->ttype==TOK_LCURL) {
			end_block=tok->match_tok; 
			end_block++;
			is_block=1;
		} else {
			end_block=NULL;
		};
//		for(;index->dval < dmax;index->dval +=dstep) 
		{
			tok=start_block;
			err_num=err_check_sentence1();
			CHECK_TOK(64015);
			if(is_block) tok=end_block;
			else skip_sentence1();
		};
		CHECK_TOK(64020);
		}; 
		break;
	case TOK_DIR_FOR:
		{
		tok_struct *start_block;	// element at block start
		NTOKEN_ERR(641);	/* go to next token after for */
		check_skip_token_err1(TOK_LPAR,"tok_dir_for",xpos);
		CHECK_TOK(6411);
		err_num=err_lexpression();	/* check initial  */
		NTOKEN_ERR(643);	/* skip separator! */
		
		// set check_list
		err_num=err_lexpression();
		NTOKEN_ERR(645);

		err_num=err_lexpression();	/* check loop statement  */
		CHECK_TOK(647);
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

			NTOKEN_ERR(654);	/* go to next toke after while */
			check_skip_token_err1(TOK_LPAR,"tok_dir_while",xpos);
			CHECK_TOK(656);
			// set check_list
			check_element=tok;
			err_num=err_check_sentence1();
			CHECK_TOK(657);

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
		NTOKEN_ERR(663);
		RT_MESG;
	case TOK_DIR_RETURN:
		NTOKEN_ERR(664);	/* skip left par, CHECK if no par!!, remove from parser!!  */
		err_num=err_lexpression();
		RT_MESG1(666);
	case TOK_SEP:
		RT_MESG;
	default:
		CHECK_TOK(623);
		err_num=err_lexpression();
		CHECK_TOK(668);
 };
 RT_MESG1(669); 
}

int err_check_block1(int level)
{
 TDSERR("block");
   SHOW_STAGE(671);

   while(1) {
	CHECK_TOK(672);
	switch(tok->ttype) {
		case TOK_EOF: 
			RT_MESG1(673);
		case TOK_SEP:
		case TOK_COMMA:
		case TOK_RPAR:	/* problem if removed  */
			NTOKEN_ERR(6741);
			continue;
		case TOK_SHOW:
			NTOKEN_ERR(674);
			continue;
		case TOK_RCURL:
			NTOKEN_ERR(675);
			RT_MESG1(6751);
	};
 	err_num=err_check_sentence1();
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
