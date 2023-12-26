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
 // MESG("tok_info2: ");
	if(tok){
	// MESG("tok not null! %d",tok->ttype); 
	if(tok->ttype>TOK_OTHER || tok->ttype==0) {
		return("unknown type of token!");
	};
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
		// MESG("token name is null! line %d type %d",last_correct_line,tok->ttype);
		snprintf(stok,MAXLLEN,"type %d tname is null!",tok->ttype);
		};
	} else {
			snprintf(stok,MAXLLEN,"%2d:%3d %s [%f]",tok->tline,tok->tnum,TNAME,tok->dval);
	};
	return stok;
}

#define	SYNTAX_DEBUG1	0

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
	stage_level--;return(err_num);\
}

#define RT_MESG1(pos) { \
	stage_level--;return(err_num);\
}

#endif


#define SHOW_STAGE(pos)	{ stage_level++;show_type='#';CHECK_TOK(pos);}

#if	1
#define	CHECK_TOK(pos) { xpos=pos ;}
#define	NTOKEN_ERR(xxx)	{ tok++;show_type=';';CHECK_TOK(xxx);}
#define TDSERR(description) {} 
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

#define TDSERR(description) char *Tds=description;
#endif


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
		syntax_error(mesg,err);
		// ERROR("err %d [%s] token wrong type %d != %d",err,mesg,type,tok->ttype);
		// err_str=mesg;
		// err_num=err;
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
 set_break();

 if(tok==NULL) {
 	fprintf(stderr,"error: tok is NULL, %d %s\n",err,description);
	err_num=err;
	err_str=description;
	return;
 };
 // MESG("set_error:----------------");
 err_line=tok->tline;
 err_num=err;
 MESG("set_error: [%s] line %d name %s",description,tok->tline,tok->tname);
 err_str=strdup(description);
#if	0
 if(execmd) {
	if(tok->tname==NULL) fprintf(stderr,"Error:%d tnum=%d ttype=%d %s line %d\n",err,tok->tnum,tok->ttype,err_str,err_line);
 	else fprintf(stderr,"Error:%d [%s] tnum=%d ttype=%d %s line %d\n",err,(char *)tok->tname,tok->tnum,tok->ttype,err_str,err_line);
 };
#endif
 tok=cbfp->end_token;
 if(tok==NULL) return;
 tok->ttype=TOK_EOF;
 set_tok_directive(tok,factor_eof);
 tok->tgroup=TOK_EOF;
 tok->ttype=TOK_END;
}

void syntax_error(char *description,int err)
{
 if(err_num>0) return;	/* error already set  */
 set_error(tok,err,description);
}

int	err_eval_fun1(tok_struct *tok0)
{
	TDSERR("eval_fun1");
	int ia;
	int ia0;
	int f_entry;
	BTNODE 	*var_node = tok0->tok_node;
	int fnum = var_node->node_index;
	ia0=m_functions[fnum].f_args;
	// MESG("err_eval_fun1: <<  args=%d",ia0);
	// MESG("err_eval_fun1: [%s] args=%d tnum=%d ttype=%d",m_functions[fnum].f_name,ia,tok->tnum,tok->ttype);

	f_entry=entry_mode;
	entry_mode=KNORMAL;

	SHOW_STAGE(401);

	ia=0;
	while(1){
		// MESG("function arg tnum=%d ttype=%d ia=%d",tok->tnum,tok->ttype,ia);
		if(tok->ttype==TOK_RPAR||tok->ttype==TOK_SEP) break;
		if(tok->ttype==TOK_COMMA) NTOKEN_ERR(403);
		ia++;
		err_num=err_num_expression();
	};

	// Check for correct number of args, this sould not happen!
	if(ia0>=0 && ia0!=ia) {
		MESG("Error in number of args: %d != %d",ia0,ia);
		syntax_error("function arguments error",4031);
		return(err_num);
	};

	if(tok->ttype==TOK_RPAR) {
		// MESG("err_eval_fun1: skip RPAR!!");
		NTOKEN_ERR(404);
	};
	tok0->number_of_args=ia;

	entry_mode=f_entry;
	// MESG("err_eval_fun1: END >> args=%d %d",ia0,ia);
	RT_MESG1(408);
}

int  err_push_args_1(int *nargs)
{
 int i=0;
 int num_args=0;
 TDSERR("push_args");

 SHOW_STAGE(410);
 // MESG("err_push_args:");
 if(tok->ttype!=TOK_RPAR) {
	if(tok->ttype==TOK_SEP) {
		*nargs=0;
		set_error(tok,411,"no parenthesis on function call!");
		return(err_num);
	};
 
 while(1){
	set_vtype(VTYPE_NUM);
	// slval[0]=0;
	xpos=412;
	
	if(tok->ttype==TOK_RPAR) {
		break;
	};
	err_num = err_lexpression();
	// MESG("err after expression t_type=%d err=%d",tok->ttype,err_num);
	if(err_num) { ERROR("error in push_args_1 from lexpression %d",err_num);return err_num;};
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
	// MESG("err_push+args: args=%d err=%d",num_args,err_num);
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
 // MESG("err_assign_args1: nargs=%d",nargs); 
 if(nargs!=0) {
	xpos=421;
	for(i=0;i<nargs;i++) {
		xpos=422;
		if(tok->ttype==TOK_RPAR) {
			break;	/* end of arguments!  */
		}
		xpos=423;	/* this should be a var token  */
#if	1
		err_num_expression();
#else
		NTOKEN_ERR(424);	/* skip semicolon or end parenthesis */
#endif
		// MESG("err_assign: after lexpression: [%s] %d",tok->tname,tok->ttype);
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
 // MESG("err_assign_args1: end tok = [%s] %d",tok->tname,tok->ttype);
 NTOKEN_ERR(434);
 return(0);
}


int err_assign_val()
{
 TDSERR("assign_val");
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

	if(!simple){
		xpos=443;
		syntax_error("bogus assignement!!!!",xpos);
		RT_MESG1(443);
	};
	err_num=err_cexpression();

	RT_MESG1(443);
}

int err_mul_by()
{
 TDSERR("mul_by");
 SHOW_STAGE(443);

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
 int save_stage_level=stage_level;
 	// MESG("err_exec_funtion: %s",name);
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
	// MESG("err_exec_function: call parse_block1");
	parsed=parse_block1(bp,NULL,0,0);	/* do not init if already parsed!, returns 0 if parsed  */
	// MESG("err_exec_function: return from parse_block1");
	if(parsed==0) {	/* already parsed, no need to check again!  */
		// MESG("	already parsed!");
		RT_MESG1(463);
	};
	/* and now execute it as asked */
	// MESG("err_exec_function: call check_init");
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
	// MESG("err_exec_func: err_num=%d",err_num);
	RT_MESG1(467);
}


int err_factor()
{
 static int pre_symbol=0;
 TDSERR("factor");
 // MESG("# err_factor: tname=[%s] tnum=%d ttype=%d",tok->tname,tok->tnum,tok->ttype);
 int save_macro_exec;
 tok_struct *tok0; 

 set_vtype(VTYPE_NUM);
 ex_edenv=0;

 SHOW_STAGE(470);
 tok0=tok;
 switch(tok0->ttype) {
 	// case TOK_PLUS:
	case TOK_MINUS:
		pre_symbol++;
 };
 NTOKEN_ERR(473);
 // MESG("set factor function:");
 // MESG("	err_factor: [%s]",tok_info(tok0));
 if(tok0->ttype > TOK_OTHER) {
 	// MESG("unknown token type %d line %d %d",tok0->ttype,tok0->tline,last_correct_line);
	err_num=4730;
	return(err_num);
 };
 set_tok_function(tok0,0);
 // set_tok_function(tok,0);
 // MESG("switch: tok0 type=%d err=%d %s %LX",tok0->ttype,err_num,tok0->tname,tok0->factor_function);
 switch(tok0->ttype) {
	/*  the following ends factor  */
#if	1
 	case TOK_SEP:
		xpos=476;syntax_error("separator in factor!",xpos);
		RT_MESG1(xpos);
#endif
	case TOK_SHOW:
		// xpos=477;syntax_error(": in factor",xpos);
		RT_MESG1(xpos);
	case TOK_LBRAKET:{	/* array definition  */
		// MESG("err: TOK_LBRAKET, array definition");
		pre_symbol=0;
		int i=0,j=0;
		int cdim=0;
		int rows=1,cols=0;
		cdim=1;
		ex_nvars=0;ex_nquote=0;ex_nums=0;	/* initialize table counters  */
		ex_array=NULL;
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
				i=0;j++;
				cdim++;if(cdim>rows) rows=cdim;
				// MESG("		new row2");
				NTOKEN2;
				continue;
			};
			// MESG("ttype=%d %s",tok->ttype,tname(tok->ttype));
		};cdim--;
		};
		// MESG("set array dat! [%s] rows=%d cols=%d",tok0->tname,rows,cols);
		// set array dat
		if(tok0->adat) {
			free(tok0->adat);
		};
		// MESG("	err: CREATE new array [%d %d]",rows,cols);
		tok0->adat = new_array(rows,cols);
		// MESG("	array type is %d ?= %d",tok0->ttype,TOK_ARRAY2);
		// set end
		NTOKEN_ERR(4789)
		RT_MESG1(4789);
		};
	case TOK_RPAR:
		xpos=479;
		pnum--;
		// syntax_error("wrong argument number",xpos);
		RT_MESG1(480);
	/* start of logic ---------  */
	case TOK_VAR:{	// 0 variable
		// MESG("TOK_VAR: [%s] type %d ind=%d",tok0->tname,tok0->ttype,tok0->tind);
		pre_symbol=0;
		ex_nvars++;
		if(tok->ttype==TOK_INCREASE) {
			tok->dval=1;
			// tok->tgroup=TOK_TERM2;
			set_tok_function(tok,0);
			NTOKEN_ERR(498);
		} else
		if(tok->ttype==TOK_DECREASE) {
			// tok->tgroup=TOK_TERM2;
			tok->dval=-1;
			set_tok_function(tok,0);
			NTOKEN_ERR(498);
		};
		// MESG("	TOK_VAR: return [%s]",tok_info(tok));
		RT_MESG1(493);}
	case TOK_ARRAY1:{
		MESG("	err use of tok_array1 [%s]",tok_info(tok0));
		// err_num=err_factor();
		err_num=err_num_expression(); 
		// MESG("	err tok_array1: after tok=%d",tok->ttype);
		xpos=499;
		check_skip_token_err1(TOK_RBRAKET,"array error",xpos);
		// NTOKEN_ERR(499);
#if	0
		tok_struct *save_tok = tok;
		// check for second index
		err_num=err_num_expression();
		if(tok->ttype==TOK_RBRAKET) {
			tok0->ttype=TOK_ARRAY2;
			tok0->factor_function=factor_array2;
			// MESG("	set 2 dimensional array!!!!!!!!");
			NTOKEN_ERR(500);
		} else {
			tok = save_tok;
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
		// MESG("	err use of tok_array2 [%s]",tok_info(tok0));
		err_num=err_num_expression(); 
		// MESG("	err_array2:1 t=%d",tok->ttype);
		NTOKEN_ERR(500);
		// MESG("	err_array2:2 t=%d",tok->ttype);
		// if(err_num) return(err_num);
		err_num=err_num_expression(); 
		// MESG("err_array2:3 t=%d",tok->ttype);
		NTOKEN_ERR(500);
		// MESG("err_array2:4 t=%d",tok->ttype);
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
			if(tok->ttype==TOK_LBRAKET) {
				// MESG("	err_tok_lpar: we have a left braket, continue!");
				RT_MESG;
			};
			if(tok->ttype !=TOK_RPAR) {
				xpos=4850;
				MESG(" +++++++++++ lpar: %d -> %d type=%d",tok0->tnum,tok->tnum,tok->ttype);

				set_error(tok0,xpos,"FAC1_err: No closing parenthesis");
				// syntax_error(" FAC1_err: No closing parenthesis",xpos);
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
		tok0->tname="numeric";
		RT_MESG1(487);
	case TOK_QUOTE:	 { // string 
		xpos=488;
		if(pre_symbol) { syntax_error("symbol before string",xpos);RT_MESG1(4881);};
		set_vtype(VTYPE_STRING);
		ex_nquote++;
		RT_MESG1(4882);
	 	};
	case TOK_AT: {
		xpos=489;
		set_vtype(VTYPE_NUM);
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
		var_node=tok0->tok_node;

		ex_edenv=0;
#if	1
		set_vtype(var_node->node_vtype);
#else
		if(var_node->sval!=NULL) { /* there is a valid string value CHECK !!*/
			set_vtype(VTYPE_STRING);
		} else set_vtype(VTYPE_NUM);
#endif
		pre_symbol=0;
		RT_MESG1(xpos);
	case TOK_ENV:	// 1 editor env var
		/* variable's name in tok0->tname */
		var_node=tok0->tok_node;
		pre_symbol=0;
		ex_nvars++;
		RT_MESG1(495);
	case TOK_FUNC:	// 2 editor function 
		/* variable's name in tok0->tname */
		pre_symbol=0;
		err_num=err_eval_fun1(tok0);
		RT_MESG1(497);
	case TOK_PROC: {	// 4 ex_proc (normal function)
		int nargs=0;
		FILEBUF *bp;
		tok_struct *after_proc;	// this is needed for recursive functions
		xpos=501;
		// MESG("err TOK_PROC:");
		ex_nvars++;

		/* function */
		pre_symbol=0;

		err_num = err_push_args_1(&nargs);
		if(err_num) return(err_num);
		// MESG("err TOK_PROC: args=%d",nargs);
		tok0->t_nargs=nargs;
		CHECK_TOK(503);
		after_proc=tok;
		// MESG("err TOK_PROC: set after_proc [%s] ttype=%d",tok->tname,tok->ttype);
		err_num=err_exec_function(tok0->tname,nargs,&bp);
		if(err_num) {
			// MESG("	err_num=%d",err_num);
			return(err_num);
		};
		if(bp!=NULL) {
			tok0->tbuf=bp;
			// MESG("	set function buffer");
		};
		CHECK_TOK(504);
		tok=after_proc;
		// MESG("	TOK_PROC: end function tnum=%d",tok->tnum);
		check_skip_token_err1(TOK_RPAR,"no right parenthesis",505);
		// MESG("err TOK_PROC: end ttype=%d",tok->ttype);
		RT_MESG;
	};
	case TOK_CMD:	{ // 3 editor command
		int var_index;
		int check_par;

		/* variable's name in tok0->tname */
		xpos=506;
		var_node=tok0->tok_node;
		var_index = var_node->node_index;
		// MESG("err: TOK_CMD");
		pre_symbol=0;
		if(ftable[var_index].arg==0) check_par=0;else check_par=1;
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
				if(err_num) return err_num;
				CHECK_TOK(517);
				// MESG("arg %d tok=[%s %d]",i,tok->tname,tok->ttype);
#if	1
				if(!check_end) {
					if(tok->ttype==TOK_RPAR || tok->ttype==TOK_SEP) break;
					else {
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
#if	0
		xpos=526;
		set_error(tok,xpos,"else without if error");
#endif
		RT_MESG1(5261);
	case TOK_RBRAKET:
		// MESG("tok_rbtacket");
		RT_MESG1(5262);
	case TOK_ASSIGN:
		MESG("TOK_ASSIGN");
	case TOK_INCREASEBY:
	case TOK_MULBY:
	case TOK_DECREASEBY:
		tok0->tname="assign";
		RT_MESG1(527);
	case TOK_ASSIGN_TYPE:
		MESG("TOK_ASSIGN_TYPE");
		RT_MESG1(528);
	default:
		xpos=527;
		MESG(" default: error_factor0: %s tind=%d ttype=%d TOK_VAR=%d",tok_info(tok0),tok0->tind,tok0->ttype,TOK_VAR);
		MESG(" default: error_factor : %s",tok_info(tok));
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
 if(err_num) RT_MESG1(err_num);

 if(tok->tgroup==TOK_TERM2) {
 	CHECK_TOK(543);
	// tok->term_function = factor_funcs[tok->ttype];
	set_term_function(tok,factor_funcs[tok->ttype]);
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
 if(err_num) RT_MESG1(err_num);
 CHECK_TOK(552);

 while (tok->tgroup==TOK_TERM1) {
 	// tok->term_function = factor_funcs[tok->ttype];
	set_term_function(tok,factor_funcs[tok->ttype]);
	CHECK_TOK(553);
	NTOKEN_ERR(5531);
	err_num=err_num_term2();
	if(err_num) RT_MESG1(5531);
	CHECK_TOK(554);
 };
 RT_MESG1(558);
}

int err_num_expression()
{
 TDSERR("num_expression");
 int expression_type=VTYPE_NUM;

 set_vtype(VTYPE_NUM);
 // slval[0]=0;
 xpos=561;

 SHOW_STAGE(561);
 err_num = err_num_term1();
 if(err_num) return(err_num);

 expression_type=get_vtype();	// set local value

 while (tok->tgroup==TOK_TERM) {
   CHECK_TOK(563);
    if(tok->ttype==TOK_PLUS) {	/* TOK_PLUS  */
		// tok->term_function = term_plus;
		set_term_function(tok,term_plus);
		NTOKEN_ERR(568);
		err_num=err_num_term1();
		if(err_num) return (err_num);
		CHECK_TOK(569);
		simple=0;
		/* check validity of operation combinations   */
		if(expression_type==VTYPE_STRING) { /* catanate string */
			if(get_vtype()) {
			} else {
				set_vtype(VTYPE_STRING);
				expression_type=get_vtype();
			}
		} else {
		}
	};
	if(tok->ttype==TOK_MINUS) {	/* TOK_MINUS  */
		// tok->term_function = term_minus;
		set_term_function(tok,term_minus);
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
 // MESG("end expression: ttype=%d",tok->ttype);
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
	err_num = err_cexpression();
	if(err_num) return err_num;
	while(1)
	{
	CHECK_TOK(702);
	switch(tok->ttype){
		case TOK_OR: {	/* TOK_OR  */
			// tok->term_function = logical_or;
			set_term_function(tok,logical_or);
			NTOKEN_ERR(704);
			err_num=err_lexpression();
			if(err_num) return err_num;
			CHECK_TOK(706);
			simple=0;
			continue;
		};
		case TOK_XOR: {	
			// tok->term_function = logical_xor;
			set_term_function(tok,logical_xor);
			NTOKEN_ERR(709);
			err_num=err_lexpression();
			if(err_num) return err_num;
			CHECK_TOK(712);
			simple=0;
			continue;
		};
		case TOK_NOR: {	
			// tok->term_function = logical_nor;
			set_term_function(tok,logical_nor);
			NTOKEN_ERR(7091);
			err_num=err_lexpression();
			if(err_num) return err_num;
			CHECK_TOK(712);
			simple=0;
			continue;
		};
		case TOK_AND: {	
			// tok->term_function = logical_and;
			set_term_function(tok,logical_and);
			NTOKEN_ERR(709);
			err_num=err_lexpression();
			if(err_num) return err_num;
			CHECK_TOK(712);
			simple=0;
			continue;
		};
		case TOK_NAND: {	
			// tok->term_function = logical_nand;
			set_term_function(tok,logical_nand);
			NTOKEN_ERR(7092);
			err_num=err_lexpression();
			if(err_num) return err_num;
			CHECK_TOK(712);
			simple=0;
			continue;
		};
		case TOK_ASSIGN: {
			// tok->term_function = assign_val;
			set_term_function(tok,assign_val);
			NTOKEN_ERR(710);
			err_num=err_assign_val();
			RT_MESG1(714);
		};
		case TOK_INCREASEBY: {
			// tok->term_function = increase_by;
			set_term_function(tok,increase_by);
			tok->tname = "+=";
			NTOKEN_ERR(710);
			err_num=err_increase_by();
			RT_MESG1(714);
		};
		case TOK_MULBY: {
			// tok->term_function = mul_by;
			set_term_function(tok,mul_by);
			tok->tname = "*=";
			NTOKEN_ERR(710);
			err_num=err_mul_by();
			RT_MESG1(714);
		};
		case TOK_DECREASEBY: {
			// tok->term_function = decrease_by;
			set_term_function(tok,decrease_by);
			tok->tname = "-=";
			NTOKEN_ERR(710);
			err_num=err_decrease_by();
			RT_MESG1(714);
		};
		case TOK_ASSIGNENV:
			// tok->term_function = assign_env;
			set_term_function(tok,assign_env);
			NTOKEN_ERR(710);
			err_num=err_assign_env();
			RT_MESG1(7141);
		case TOK_ASSIGNOPT:
			// tok->term_function = assign_option;
			set_term_function(tok,assign_option);
			NTOKEN_ERR(710);
			err_num=err_assign_env();
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
		// tok->directive = dir_lcurl;
		if(execmd) set_tok_directive(tok,dir_lcurl);
		else set_tok_directive(tok,dir_lcurl_break);
		NTOKEN_ERR(627);
		CHECK_TOK(628);
		err_num=err_check_block1();
		RT_MESG1(629);	/* at the start of next sentence  */
	};

	case TOK_DIR_IF:
		{
		tok_struct *tok0=tok;
		// MESG("err: TOK_DIR_IF!");
		set_tok_directive(tok,tok_dir_if);
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
		check_skip_token_err1(TOK_RPAR,"tok_dir_if",xpos);
		CHECK_TOK(634);
//		if(tok->ttype==TOK_LCURL) is_block=1; 
		err_num=err_check_sentence1();	/* body of if  */
		// MESG("	after body of if [%s]",tok_info(tok));
		// check_skip_token1(TOK_RPAR);	/* ???????????  */
		// NTOKEN_ERR(635);	/* go after sentence  */
		CHECK_TOK(635);
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
		} else {
			xpos=639;
		};
		RT_MESG1(xpos);
		};
	case TOK_DIR_FORI:
		{
		tok_struct *start_block;	// element at block start
		tok_struct *end_block;	/* at the block end  */
		int is_block=0;
		// tok->directive = tok_dir_fori;
		set_tok_directive(tok,tok_dir_fori);
		NTOKEN_ERR(640);	/* go to next token after for */

		if(tok->ttype==TOK_VAR) {
			NTOKEN_ERR(6403);
			if(tok->ttype!=TOK_ASSIGN) ERROR("6404:for i error");
		} else ERROR("6405:for i syntax error");
		NTOKEN_ERR(64053);
		err_num=err_num_expression();	/* initial   */
		if(err_num) return(err_num);
		CHECK_TOK(6406);
		NTOKEN_ERR(6407);	/* skip separator! */
		err_num=err_num_expression();
		if(err_num) return(err_num);
		NTOKEN_ERR(6408);
		err_num=err_num_expression();
		if(err_num) return(err_num);
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
		// tok->directive=tok_dir_for;
		set_tok_directive(tok,tok_dir_for);
		NTOKEN_ERR(641);	/* go to next token after for */

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
			// tok->directive=tok_dir_while;
			set_tok_directive(tok,tok_dir_while);
			NTOKEN_ERR(654);	/* go to next toke after while */

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
		// tok->directive = dir_break;
		set_tok_directive(tok,dir_break);
		NTOKEN_ERR(663);
		RT_MESG;
	case TOK_DIR_RETURN:
		// tok->directive = dir_return;
		set_tok_directive(tok,dir_return);
		// MESG(" err TOK_DIR_RetURN");

		NTOKEN_ERR(664);
		if(tok->ttype!=TOK_SEP&&tok->ttype!=TOK_RPAR) 	
			err_num=err_lexpression();
		check_skip_token1(TOK_RPAR);
		// MESG(" err TOK_DIR_RETURN: after lexpression: tname=[%s] tnum=%d ttype=%d",tok->tname,tok->tnum,tok->ttype); 
		RT_MESG1(666);
	case TOK_SEP:
		RT_MESG;
	case TOK_DIR_TYPE:
		{
		// tok_struct *tok0=tok;
		set_tok_directive(tok,tok_dir_type);
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
   MESG("err_check_block1: --------------------------------");
   while(1) {
	CHECK_TOK(672);
	if(tok->ttype==11)  {
		BTNODE *var_node=tok->tok_node;
		if(tok->tvtype==32) MESG(" - %3d: var %s node_name=%s type=%d",tok->tnum,tok->tname,var_node->node_name,var_node->node_type);
		else MESG(" - %3d: %3d %s",tok->tnum,tok->ttype,tok->tname);
	} else MESG(" - %3d: %3d %s",tok->tnum,tok->ttype,tok->tname);
	switch(tok->ttype) {
		case TOK_EOF: 
			MESG(">>>>>>>>>>>>>>> end error_check!");
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
