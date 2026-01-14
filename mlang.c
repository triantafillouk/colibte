/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

 	An interpreter,embeded calculator by K.Triantafillou (2011,2020),
*/

#include	<math.h>
#include	<stdlib.h>
#include	"xe.h"
#include	"display_driver.h"
#include	"alist.h"
#include	"mlang.h"
#include	"func.h"
#include	"token_table.h"

#define	SYNTAX_DEBUG	0
#if	1
// #define NTOKEN2 ntoken2()
#define NTOKEN2 tok++
#else
#define	NTOKEN2	ntoken2()
#endif
#define FACTOR_FUNCTION tok->factor_function()
void mesg_out(const char *fmt, ...);
extern FILEBUF *cbfp;
// extern array_dat *main_args;
FILEBUF *exe_buffer=NULL;
FILEBUF *check_buffer=NULL;
void show_vars(MVAR *va, int size,char *title);
void set_term_function(tok_struct *tok, TFunction term_function);
void  skip_args1(int nargs);
int check_init(FILEBUF *bf);
double exec_function(FILEBUF *bp,int nargs);
MVAR * push_args_1(int nargs,int vars_num);

#if	SYNTAX_DEBUG

int stage_level=0;	/* stage level  */

#define INIT_STAGE	stage_level=0;

#define	SHOWTOK {if(discmd) { MESG("  [%2d][%*s-%s][%d]  %s",stage_level,stage_level,"",Tds,tok->tnum,tok_name[tok->ttype]);};}

#define	TDS(name)   char *Tds=name;\
	if(discmd){ stage_level++;\
		MESG("-- level %d type=%d",stage_level,tok->tnum);\
		if(tok!=NULL) {\
			MESG("#+[%2d][%*s-%s][%d]  %s",stage_level,stage_level,"",Tds,tok->tnum,token_table[tok->ttype].tok_name);\
		}\
	} 

#define RTRN(val)	{ stage_level--;return(val);}

#else	/* SYNTAX_DEBUG  */

#define INIT_STAGE	;

#define	SHOWTOK ;

#define	TDS(name)   ;

#define RTRN(val)	return(val)

#endif


int err_check_block1();
int check_skip_token1( int type);
void clean_saved_string(int new_size);
int deq(double v1,double v2);
char *str_mul(char *sval, double v1);
char *str_cat(char *sval, char *add);
void refresh_ddot_1(double value);
double factor_refresh_ddot();

double assign_val(double none);
double assign_env(double none);
void init_error();
void get_lowercase_string(char *lower, char *string);
void get_uppercase_string(char *lower, char *string);
double cexpression();
double exec_block1_break(FILEBUF *fp);
char * tok_info(tok_struct *tok);
void MESG_TOK_INFO(char *title,tok_struct *tok);

void set_vtype(int type);
int vtype_is(int type);
int get_vtype();
static inline double num_expression();

int bnf_debug();

TLIST ctoklist=NULL;
int is_break1=0;
int tok_mask[256];
int no_push=0;
#define	USE_VAR	1

static MVAR ex_var; 	/* value of previous expression */
array_dat *get_array(char *);
void set_array(array_dat *);

array_dat *ex_array=NULL;
char *saved_string=NULL;

int ex_edenv=0;	/* true after encount an editor env variable */
int ex_nvars=0;	/* true is there are variables in the array definition  */
int ex_nquote=0;	/* true if there are strings in the array definition  */
int ex_nums=0;	/* true if array is only numeric  */
char *ex_name=NULL;	/* variable name of the previous array  */

/* error control variables  */
int err_num=0;
static int err_line=0;
int last_correct_line=0;
static tok_struct *tok=NULL;	/* current token!!  */

char *err_str;
BTNODE *var_node=NULL;

tok_struct *lstoken=NULL;
MVAR *lsslot=NULL;
char **ls_psval=NULL;
double *ls_pdval=NULL;
MVAR *lmvar=NULL;
int firt_var=1;
MVAR *current_stable=NULL; 	/* current symbol table ...  */

// CALL_STACK variables
MVAR *call_stack;
MVAR *call_stack_used;
MVAR *max_call_stack_end;
MVAR *call_stack_available;

#if	TBNF
MVAR *exec_stack;
MVAR *exec_stack_current;
MVAR *exec_stack_max;
#endif

int show_stage=0;
int xpos=0;		/* stage position  */
int current_active_flag=1;	/* on to execute procedure!  */

int pnum=0;	/* parenthesis level */


#include "mlang_array.c"

extern char *fvars[];
extern VAR option_names[];

BTREE *bt_table;
BTREE *directiv_table;
#if	USE_TYPE_VARS
BTREE *global_types_tree;
#endif
int simple=0;

static inline double num_term1();
static inline double num_term2();
//static inline double factor();


term_type term_types[] = {
	{"if"  ,TOK_DIR_IF  ,TOK_DIR },
	{"else",TOK_DIR_ELSE,TOK_DIR },
	{"break",TOK_DIR_BREAK,TOK_DIR},
	{"return",TOK_DIR_RETURN,TOK_DIR},
	{"while",TOK_DIR_WHILE,TOK_DIR},
	{"for",TOK_DIR_FOR,TOK_DIR},
	{"function",TOK_PROC,TOK_DIR},
	{"fori", TOK_DIR_FORI,TOK_DIR},
	{"foreach", TOK_DIR_FOREACH,TOK_DIR},
	{"type", TOK_DIR_TYPE,TOK_DIR},
	{"xor" ,TOK_XOR, TOK_BOOL},
	{"or"  ,TOK_OR,  TOK_BOOL},
	{"and" ,TOK_AND, TOK_BOOL},
	{"nand",TOK_NAND,TOK_BOOL},
	{"nor",TOK_NOR,TOK_BOOL},
	{NULL,0,0}
};

char *vtype_names[] = {
	"UNDEF","NUM","ARRAY","SARRAY","LIST",
	"SLIST","ALIST","ASLIST","STRING","BUFFER",
	"ARRAY_EL1","ARRAY_EL2","TREE","TREE_EL",
	"AMIXED","POINTER","PROC","OPTION","FUNCTION",NULL
};

/* Function definitions */
#include "mlang_functions.c"
#if	TBNF
#include "bnf_expr.c"
#endif

void stack_push(char *title,tok_struct *tok)
{
#if	TBNF
 // if(no_push) { MESG("stack_push:%s skip %s",title,tok_info(tok));return;};
 if(tok!=NULL) {
 // memcpy((void *)check_buffer->tok_bnf,(void *)tok,sizeof(tok_struct));
 // check_buffer->tok_bnf++;
 if(tok->ttype==TOK_LPAR) { MESG("skip left paranthesis!");return;};
	MESG("P[%10s %3d %-15s|%s",check_buffer->b_fname,check_buffer->tok_bnf_index++,title,tok_info(tok));
 } else MESG("P [%s] null token!!!",title);
#endif
}

void delete_type_tree(BTREE *type_tree)
{
	MESG("delete_type_tree:");
	free_btnode(type_tree->root);
	free(type_tree);
}

array_dat *transpose(array_dat *array1);

void init_vars(MVAR *head,int size)
{
 // initialize as numeric
 // MESG("init_vars: %d",size);
#if	1
 MVAR *tdp,*tdp_end=head+size;
 for(tdp=head;tdp<tdp_end;tdp++) {
 	tdp->var_type=VTYPE_NUM;
	tdp->dval=0;
 };
#else
 int i=0;
 for(;i<size;i++) {
	// td[i].var_index=i;
 	head[i].var_type=VTYPE_NUM;
	head[i].dval=0;
 };
#endif
}

void initialize_call_stack(int initial_size)
{
	// MESG("Initialize call_stack with %d size",initial_size);
	call_stack=(MVAR *)malloc(sizeof(struct MVAR)*initial_size);
	call_stack_used=call_stack;
	max_call_stack_end=call_stack;
	call_stack_available=call_stack+initial_size;
	// fprintf(stderr,"	initial call_stack=%p\n",(void *)call_stack);
#if	TBNF
	exec_stack=(MVAR *)malloc(sizeof(struct MVAR)*512);
	exec_stack_current=exec_stack;
	exec_stack_max=exec_stack+512;
#endif
}

#if	USE_CALL_STACK0
// Not used, realloc_call_stack is difficult to implement. All previous positions must be reassigned!!!
MVAR *realloc_exec_stack(int additional_size)
{
	int required_size = exec_stack_current-call_stack;
	int new_size = required_size+additional_size;
	int initial_size = exec_stack_max-exec_stack;
	int alloc_size = new_size*sizeof(struct MVAR);
	// MESG("realloc_call_stack %p",call_stack);
	// MESG("call_stack realloc: initial=%d required=%d new=%d alloc=%d",initial_size,required_size,new_size,alloc_size);
		// show_vars(call_stack,initial_size,"reallo_call_stack");
		exec_stack=(MVAR *)realloc(exec_stack,alloc_size);
		if(exec_stack) {
			exec_stack_current=exec_stack+required_size;
			exec_stack_max=exec_stack+new_size;;
			
			// show_vars(call_stack,initial_size,"after reallocation");
			// MESG("call_stack reallocated from %d to %d ----------------",initial_size,new_size);
		} else {
			return NULL;
		};
	MESG("	new  exec_stack=%lX",(void *)call_stack);
	return exec_stack;
}
#endif

void init_btree_table()
{
	bt_table=new_btree("table",0);
	directiv_table=new_btree("directives",0);
#if	USE_TYPE_VARS
	global_types_tree=new_btree("types",0);
#endif
}


void clear_args(MVAR *va,int nargs)
{
#if	0
 int i;

 // MESG("clear_args: %d",nargs);

	 for(i=0;i<nargs;i++){
	 	// MESG("	%d %d %X",i,va[i].var_type,va[i].sval);
		if(va[i].var_type==VTYPE_STRING){
			// MESG("clear_args: free %X",va[i].sval);
	 		free(va[i].sval);
		}
	 };
#endif
	call_stack_used -= nargs;
	// MESG("clear_args: call_stack=%lld upto %lld",call_stack_used-call_stack,call_stack_used-call_stack+nargs);
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

curl_struct *new_curl(int level,int mline, struct _el *el)
{
 struct curl_struct *lcurl; // left curl
 lcurl=(curl_struct *)malloc(sizeof(struct curl_struct));
 lcurl->level=level;
 lcurl->ocurl=el;
 lcurl->num=0;
 return(lcurl);
}


static inline double factor_none()
{
	NTOKEN2;
	return 0.0;	/* continue  */
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
				if(lsslot==NULL) MESG("lsslot is NULL!");
				v0=lsslot->dval;
				lsslot->dval += tok->dval;
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

void sarray_mul1(array_dat *sarray, double factor);

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

tok_struct *new_tok()
{
 tok_struct *tok;
 tok=(struct tok_struct *) malloc(sizeof(struct tok_struct));
 tok->tind=0;
 tok->tline=0;
 tok->tnum=0;
 tok->tname="empty";
 tok->dval=0;
 tok->ttype=0;
 tok->tgroup=0;
 tok->factor_function=factor_none;
 tok->directive=lexpression;
 tok->tok_node=NULL;
 // MESG("new_tok:");
 return(tok);
}

#define	TNAME	tname(tok->ttype)

char *tname(int type)
{
 int ind=type%1000;
 	if(ind>TOK_OTHER) return("unknown!");
	// return(tok_name[ind]);
	return(token_table[ind].tok_name);
}

void init_token_mask()
{
 int ind;

 for(ind=0;ind<255;ind++) {
 	if((ind>='a' && ind<='z') ||(ind>='A' && ind<='Z') || ind=='_' || ind>=128) 
 	tok_mask[ind]=TOK_LETTER;
	else if(ind>='0'&&ind<='9') tok_mask[ind]=TOK_NUM;
	else if(ind==' '||ind==9) tok_mask[ind]=TOK_SPACE;
	else if(ind=='.') tok_mask[ind]=TOK_DOT;
	else if(ind=='{') tok_mask[ind]=TOK_LCURL;
	else if(ind=='}') tok_mask[ind]=TOK_RCURL;
	else if(ind==12||ind==10||ind==13) tok_mask[ind]=TOK_NL;
	else if(ind==';') tok_mask[ind]=TOK_SEP;
	else if(ind=='\'' || ind=='"') tok_mask[ind]=TOK_QUOTE;	/* double,single quotes are the same  */
	else if(ind=='(') tok_mask[ind]=TOK_LPAR;
	else if(ind==')') tok_mask[ind]=TOK_RPAR;
	else if(ind==CHR_LBRA) tok_mask[ind]=TOK_LBRAKET;
	else if(ind==CHR_RBRA) tok_mask[ind]=TOK_RBRAKET;
	else if(ind ==',') tok_mask[ind]=TOK_COMMA;
	else if(ind==':') tok_mask[ind]=TOK_SHOW;
	else if(ind=='#') tok_mask[ind]=TOK_COMMENT;
	else if(ind=='<') tok_mask[ind]=TOK_SMALLER;
	else if(ind=='>') tok_mask[ind]=TOK_BIGGER;
	else if(ind=='|') tok_mask[ind]=TOK_OR;
	else if(ind=='&') tok_mask[ind]=TOK_AND;
	else if(ind=='!') tok_mask[ind]=TOK_NOT;
	else if(ind=='+') tok_mask[ind]=TOK_PLUS;
	else if(ind=='-') tok_mask[ind]=TOK_MINUS;
	else if(ind=='%') tok_mask[ind]=TOK_MOD;	/* term2  */
	else if(ind=='^') tok_mask[ind]=TOK_XOR;	/* boolean/bitwise  */
	else if(ind=='*') tok_mask[ind]=TOK_MUL;
	else if(ind=='/') tok_mask[ind]=TOK_DIV;
	else if(ind=='=') tok_mask[ind]=TOK_ASSIGN;
	else if(ind=='@') tok_mask[ind]=TOK_AT;
	else if(ind=='`') tok_mask[ind]=TOK_BQUOTE;
	else if(ind=='$') tok_mask[ind]=TOK_DOLAR;
	else if(ind=='~') tok_mask[ind]=TOK_TILDA;
	else if(ind=='\\') tok_mask[ind]=TOK_BSLASH;
	else if(ind==0) tok_mask[ind]=0;
	else tok_mask[ind]=TOK_OTHER;
 };
}

void init_hash()
{
 int i;

 init_btree_table();
 init_token_mask();
	/* editor command */
  for(i=0;ftable[i].n_func !=NULL;i++) {
	insert_bt_element(bt_table,ftable[i].n_name,TOK_CMD,i);
  };
	/* editor function */
  for(i=0; m_functions[i].f_name != NULL;i++){ 
	insert_bt_element(bt_table,m_functions[i].f_name,TOK_FUNC,i);
  };

/* env variable */
  for(i=0;fvars[i]!=NULL;i++)
  {
	insert_bt_element(bt_table,fvars[i],TOK_ENV,i);
  };

/* option variables  */
 for(i=0;option_names[i].name!=NULL;i++) {
 	BTNODE *option_node;
	if(option_names[i].sval!=NULL) {
		option_node=set_btsval(bt_table,option_names[i].name,option_names[i].sval);
	} else {
		option_node=set_btdval(bt_table,option_names[i].name,option_names[i].dval);
	};
	option_node->node_type=TOK_OPTION;
    // printf("OPTION %15s ind=%2d type %d vtype=%d\n",option_names[i].name,option_node->node_index,option_node->node_type,option_node->node_vtype);
 };
	// eval_btree(bt_table->root,print_node);

 for(i=0;term_types[i].term_name!=NULL;i++)
 {
	BTNODE *term_node;
	term_node = set_btdval(directiv_table,term_types[i].term_name,i);
	term_node->node_index = term_types[i].term_type;
	term_node->node_type = term_types[i].term_group;
	term_node->node_vtype=VTYPE_NUM;	/* to show the index in value if needed!  */
 };
	// eval_btree(directiv_table->root,print_node);
 init_common();
}

/* test if a file is macro lanuage ...  */
int is_mlang(FILEBUF *fp)
{
 int bt=fp->b_type;
 // MESG("check macro file!");
 if(fp->b_flag & FSNLIST) {
 	msg_line("dir is not an mlang file! %X",fp->b_flag);
	return 0;
 };

 if(!file_type_is("NONE",bt) && 
    !file_type_is("CMD",bt) &&
    !file_type_is("MD",bt) &&
	!file_type_is("TEXT",bt) &&
	!file_type_is("DOT",bt) &&
	!file_type_is("GTEXT",bt)) 
 {
	MESG("this is NOT a macro file!!!!!!!!!!!!!!");
 	msg_line("Not an mlang file bt=%d",bt);
	return 0;	/* only allowed in selected types  */
 };

 if(!strcmp("[out]",fp->b_fname)) {
 	msg_line("cannot check [out] buffer!!");
	return 0;
 };
 // MESG("this is a macro file!");
 return 1; 
}

void set_symbol_table(MVAR *stable)
{
	current_stable=stable;
}


MVAR *new_symbol_table(int const size)
{
 // MESG("Initialize new_symbol_table: size %d",size);
 MVAR *td=call_stack_used;
 // MESG("		new symbol table starts at ind=%d",td-call_stack);
 // int size_call_stack_used = call_stack_used - call_stack;
 // MESG("		new_symbol_table: at %lld",td-call_stack);
 call_stack_used += size;
 if(max_call_stack_end<call_stack_used) {
 	max_call_stack_end=call_stack_used;

	if(call_stack_used>call_stack_available) {
		if(execmd) {
			printf("new_symbol_table: overflow: available=%ld required=%ld\n",call_stack_available-call_stack,call_stack_used-call_stack);
			// printf("execmd=%d\n",execmd);
			exit(0);
		} else {

			msg_line("new_symbol_table: overflow: available=%d required=%d",call_stack_available-call_stack,call_stack_used-call_stack);
		};
		set_error(tok,101,"call_stack overflow");
		return NULL;
	};
 }

 // MESG("Initialize new_symbol_table: size %d",size);
 init_vars(td,size);
 return td;
}

MVAR *realloc_symbol_table(MVAR *td,int size,int old_size)
{
  MESG("realloc_symbol_table: ---------------");
  call_stack_used += size-old_size;
 if(max_call_stack_end<call_stack_used) {
 	max_call_stack_end=call_stack_used;
	if(call_stack_used>call_stack_available) {
		fprintf(stderr,"call_stack limit overflow: %ld",call_stack_used-call_stack);
		quit(true);
	};
 };

 init_vars(td+old_size,size-old_size);
 return td;
}


/* free symbol table after execute */
void delete_symbol_table(MVAR *td, int size,int nargs)
{
 // MESG("delete_symbol_table: nargs=%d size=%d",nargs,size);
 MVAR *sslot=td;
 // int i=0;
 for(;sslot < td+nargs;sslot++)
 {
	// string arguments are dublicated, must be freed
 	if(sslot->var_type==VTYPE_STRING) {
		if(sslot->sval!=NULL){
			free(sslot->sval);
			sslot->sval=NULL;
		};
	};
 };
 // for(i=nargs;i<size;i++) 
 for(;sslot < td+size+nargs;sslot++) 
 {
	// MESG("dst vars %d",i);
	if(sslot->var_type==VTYPE_STRING) {
		// MESG("delete_symbol_table:%d [%s] free %X",i,sslot->sval,sslot->sval);
		 	if(sslot->sval!=NULL)  {
				free(sslot->sval);
				sslot->sval=NULL;
			};
	};
	if(sslot->var_type==VTYPE_ARRAY) {
		// MESG("delete_symbol_table:array %s",sslot->adat->array_name);
		if(sslot->adat!=NULL) {
			free_array_dat(sslot->adat);
			free(sslot->adat);
			sslot->var_type=VTYPE_NUM;
			sslot->dval=0;
		};
	};
 };
 // MESG("delete_symbol_table from: %lld",call_stack_used-call_stack);
 call_stack_used -= size+nargs;
 // MESG("delete_symbol_table: call_stack=%lld upto %lld",call_stack_used-call_stack,call_stack_used-call_stack+nargs+size-1);
 // MESG("                    at  : %lld",call_stack_used-call_stack);
}

void init_stack()
{
	delete_symbol_table(call_stack,call_stack_used-call_stack,0);
}

/* double equality */
int deq(double v1,double v2)
{
 if(v1==v2) return(1);
 if(v1>v2) {
	if(v1-v2<0.0000001) return(1);
 } else {
	if(v2-v1<0.0000001) return(1);
 };
 return(0);
}

void init_error()
{
 // MESG("init_error:");
 err_line=0;
 err_num=0;
 err_str=NULL;
}

void init_ex_var()
{
	// MESG("init_ex_var:");
	ex_var.var_type=VTYPE_NUM;
	ex_var.dval=0;
}

void init_exec_flags()
{
 // MESG("init_exec_flags:");
 init_error();
 is_break1=0;
 current_active_flag=1;
 set_vdval(0.0);
 init_ex_var();
 var_node=NULL;
 pnum=0;
 INIT_STAGE;
 ex_nvars=0;ex_nquote=0;ex_nums=0;	/* initialize table counters  */
 // MESG("init_exec_flags:end");
}

int exec_named_function(char *name)
{
    FILEBUF *bp;		/* ptr to buffer to execute */
    char bufn[MAXFLEN+2];		/* name of buffer to execute */

	// MESG("exec_named_function: %s",name);
	/* find out what buffer to execute */
	strlcpy(bufn+1,name,MAXFLEN);

	/* construct the buffer name */
	bufn[0] = CHR_LBRA;
	strlcat(bufn, "]",MAXFLEN);
	// MESG("exec_named_function: name=%s bufn=%s",name,bufn);	
	/* find the pointer to that buffer */
    if ((bp=get_filebuf(bufn,NULL,FSINVS)) == NULL) 
	{
		msg_line("No function named %s",bufn);
		return(FALSE);
    };
	bp->b_type=1;	/* set file type to cmd  */
	init_exec_flags();
	/* parse the block if not already parsed  */
	if(bp->m_mode != M_PARSED)
	parse_block1(bp,NULL,0);	/* do not init if already parsed!  */

	if((err_num=check_init(bp))>0) {
		return(0);
	};

	double value=exec_function(bp,0);

	return((int)value);
}

void show_error(char *from,char *name)
{
	ERROR("%s error %d file %s line %d: [%s]",from,err_num,name,err_line,err_str);
}

/* Check for any errors and initialize parsed list  */
int check_init(FILEBUF *bf)
{
 tok_struct *tok_table=bf->tok_table;
 FILEBUF *ori_buffer = check_buffer;
 check_buffer = bf;
 int err=0;
 INIT_STAGE;
 // MESG("-check_init: [%s] %d",bf->b_fname,bf->b_type);
 if(execmd) 
 {
 	fprintf(stderr,"exec [%s] ----------------\n",bf->b_fname);
 	fprintf(stdout,"exec [%s] ----------------\n",bf->b_fname);
 };
 if(tok_table==NULL) 
 {
 	// MESG("create token table [%s]",bf->b_fname);
	parse_block1(bf,NULL,1);
	// MESG("block parsed err = %d",err_num);
	if(err_num>0) {
		msg_line("found parsed errors: err_num=%d %s",err_num,err_str);
		check_buffer = ori_buffer;
		return(err_num);
	}
	tok_table=bf->tok_table;
	if(tok_table==NULL)	{
		ERROR("cannot parse file");
		check_buffer = ori_buffer;
		return(201);
	}
 };

 if(bf->err<1) 
 {
	tok=bf->tok_table;
	if(!(bf->m_mode & M_CHECKED))	/* not checked  */
	{
		err=err_check_block1();
		bf->err=err;
		bf->m_mode |= M_CHECKED;
		// MESG("Block checked! err=%d",err);
	};
 };

 tok=tok_table;
 // MESG("check_init:end [%s] %d",bf->b_fname,bf->b_type);

 if(bf->err>0) {
	return bf->err;
 };
 bf->m_mode |= M_CHECKED;
 check_buffer = ori_buffer;
#if	TBNF
 bf->tok_bnf_index = 0;
 bf->tok_bnf = bf->tok_table_bnf;
 MESG("! bnf table of [%s] initialized! ------------",bf->b_fname);
 if(bnf_debug() && check_buffer==NULL) exit(0);
#endif
 return(0);
}

void set_current_token(tok_struct *tnew)
{
	tok=tnew;
}

tok_struct *current_token()
{
	return(tok);
}

void skip_comma()
{
	if(check_skip_token_err(TOK_COMMA,"skip_comma",221)) {
		ERROR("no comma found!");
	};
}

int check_rparenthesis()
{
	if(check_token(TOK_RPAR)) return 1;
	else return 0;
}

int check_skip_token( int type)
{
 TDS("check_skip_token");

 	if(tok->ttype==type) 
 	{ 
		NTOKEN2;
 		RTRN(1);
 	} else {
		ERROR("check_skip_token: wrong type=%d [%s] found %d [%s]",type,tname(type), tok->ttype,TNAME);
		RTRN(0);
	};
}

int check_skip_token_err(int type,char *mesg,int err)
{
 TDS("check_skip_token_err");

 	if(tok->ttype==type) 
 	{ 
		NTOKEN2;
 		return(0);
 	} else {
		err_str=mesg;
		err_num=err;
		err_line=tok->tline;
	};

  return(err_num);
}

int check_token(int type)
{
 TDS("check_token");

 return(tok->ttype == type);
}


MVAR * push_args_1(int nargs,int vars_num)
{
 TDS("push_args");

 MVAR *va = new_symbol_table(nargs+vars_num);
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

 init_vars(va_i,vars_num);
 // MESG(">	push_args_1:end [%s]",tok_info(tok));
 return(va);
}


double exec_function(FILEBUF *proc_buffer,int nargs)
{
#if	1
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
#else
	// MESG("exec_function: bp=[%s] nargs=%d level=%d",bp->b_fname,nargs,level);
	tok=bp->tok_table;	/* start of function  */
#if	TBNF
	bp->tok_bnf_index=0;
#endif
	skip_args1(nargs);
	return tok->directive();
#endif
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

inline MVAR *get_left_slot(int ind)
{
	// MESG("get_left_slot: ind=%d",ind);
	return current_stable+ind;
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
 	lsslot = get_left_slot(tok->tind);
	lstoken = tok;
	NTOKEN2;
	return(lsslot->dval);
}

double factor_variable()
{
	// MESG("factor_variable:");
 	lsslot = get_left_slot(tok->tind);
	// lsslot = current_stable+tok->tind;
	int ptype=get_vtype();
	lstoken = tok;
	// MESG("factor_variable: %d",tok->tind);
	set_vtype(lsslot->var_type);
	// MESG("	factor_variable:[%s] tind=%d ,var ind=%d ex_vtype=%d ttype=%d vtype=%d",
		// tok->tname,tok->tnum,lsslot->var_index,get_vtype(),tok->ttype,lsslot->var_type);
	switch(get_vtype()) {
		case VTYPE_NUM:
		{
			if(ptype==VTYPE_NUM) tok->factor_function = factor_variable_num;
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


void eval_btree1(BTNODE *node,void do_func(BTNODE *n,void *p),void *p);

void node_to_mvar(BTNODE *node,void *p)
{
 int index=node->node_index;
 MVAR *mvar_array = (MVAR *)p;
	mvar_array[index].var_type=node->node_vtype;
	// mvar_array[index].var_index=node->node_index;
	// mvar_array[index].var_type=0;
	if(node->node_vtype==VTYPE_NUM) {
		mvar_array[index].var_type=VTYPE_NUM;
		mvar_array[index].dval=node->node_dval;
		// mvar_array[index].var_len=0;
		// MESG("![%10s] ind=%2d type=%d %f",node->node_name,index,node->node_vtype,node->node_dval);
	} else {
		mvar_array[index].sval=strdup(node->node_sval);
		// mvar_array[index].var_len=strlen(node->node_sval);
		// MESG("![%10s] ind=%2d type=%d %s",node->node_name,index,node->node_vtype,node->node_sval);
	};	
}

MVAR *btree_to_mvar(BTREE *bt)
{
 MVAR *mvar_array = malloc(sizeof(struct MVAR)*bt->items);
 // MESG("-btree_to_mvar: items=%d",bt->items);
 eval_btree1(bt->root,node_to_mvar,(void *)mvar_array);
 return mvar_array;
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
	MESG("factor_assign_type: [%s]",tok_info(tok));
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

	array_dat *adat=alloc_array();
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
	lstoken=tok;
	// MESG("factor_array2: type %d",adat->atype);
	
	if(adat==NULL) {
#if	1
		set_error(tok,209,"array indexes out of bound!");
		return(0);
#else
		/* No data in the array, allocate new ones!  */
		NTOKEN2;
		ind1=(int)num_expression();
		NTOKEN2;
		ind2=(int)num_expression();
		NTOKEN2;
		adat = new_array(ind1+1,ind2+1);

		array_slot->adat = adat;
		tok0->adat = adat;
		print_array1("new array created",adat);
		dval2 = adat->dval2;

		// MESG("new rows=%d cols=%d",adat->rows,adat->cols);
// 		NTOKEN2;
			// value=dval2[ind1][ind2];
#endif
	} else {
		NTOKEN2;
		ind1=(int)num_expression();
		NTOKEN2;
		ind2=(int)num_expression();
		NTOKEN2;

		if((ind1 >= adat->rows) || (ind2 >= adat->cols)) {
			// syntax_error("array indexes out of bound!",209);
			set_error(tok0,209,"array indexes out of bound!");
		} else {
			if(adat->atype==VTYPE_ARRAY) {
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
	};
	// MESG("end factor_array2: lsslot=%X",(void *)lsslot);
	return(value);
}

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

double factor_array1()
{
	int ind1;
	double *dval=NULL;
	double value=0;
	MVAR *array_slot;
	array_slot=&current_stable[tok->tind];
	array_dat *adat = array_slot->adat;
	lstoken=tok;
	MESG("factor_array1:----------- vtype=%d",array_slot->var_type);
	NTOKEN2;
	ind1 = (int)num_expression();

	if(adat==NULL) {	/* this must not happen!!!  */
		// MESG("array adat is NULL allocate new one !!!!!!!!!!!!");
		ex_nums=1;
		adat=new_array(ind1+1,1);
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
	// MESG("	factor_array1:ind1=%d lsslot ind=%d type=%d rows=%d cols=%d [%s]!",ind1,lsslot->var_index,lsslot->var_type,lsslot->adat->rows,lsslot->adat->cols,array_slot->psval[0]);
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
	// MESG("	factor_array1:ind1=%d lsslot ind=%d type=%d rows=%d cols=%d!",ind1,lsslot->var_index,lsslot->var_type,lsslot->adat->rows,lsslot->adat->cols);
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

/* user interupt on, set also break flag  */
void set_break()
{
	// MESG("set_break:");
	is_break1=1;
	// tok->tgroup=TOK_EOF;
	// tok->ttype=TOK_EOF;
	current_active_flag=0;
}

double factor_error()
{
	syntax_error("factor_error",2081);
	NTOKEN2;
	set_break();
	return 0.0;
}

static inline double factor_lpar()
{
 double value;
 NTOKEN2;
 // set_vdval(0);
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

double cexpr_notequal(double v1,double v2)
{
 return v1!=v2 ? 1.0:0.0;
}

static inline double cexpr_smaller(double v1,double v2)
{
 return v1<v2 ? 1.0: 0.0;
}

static inline double cexpr_bigger(double v1,double v2)
{
 return v1>v2 ? 1.0: 0.0;
}

static inline double cexpr_equal(double v1,double v2)
{
 // MESG("cexpr_equal: %f == %f",v1,v2);
 set_vtype(VTYPE_NUM);
 return v1==v2 ? 1.0: 0.0;
}

double cexpr_smallereq(double v1,double v2)
{
 return v1 <= v2 ? 1.0: 0.0;
}

double cexpr_biggereq(double v1,double v2)
{
 return v1 >= v2 ? 1.0: 0.0;
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

char *str_mul(char *sval, double v1)
{
 if(v1>0) {
	int mul_size=v1;
	int string_size=strlen(sval);
	char *new_string=malloc(mul_size*string_size+1);
	
	for(int i=0;i<mul_size;i++) {
		memcpy(new_string+string_size*(i),sval,string_size);
	};new_string[string_size*mul_size]=0;
 	return new_string;
 } else return sval;
}

char *str_cat(char *sval, char *add)
{
	int add_size=strlen(add);
	int string_size=strlen(sval);
	sval=realloc(sval,add_size+string_size+1);
	memcpy(sval+string_size,add,add_size);
 return sval;
}

static double term1_mul_num(double v1)
{
	NTOKEN2;
	return v1*num_term2();;
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
			array_dat *new_array = get_array("11");
			if(loc_array->rows==1 && new_array->cols==1) {
				if(loc_array->cols== new_array->rows) {
					int i;
					v1=0;
					for(i=0;i<loc_array->cols;i++){
						v1 += loc_array->dval[i]*new_array->dval[i];
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
	NTOKEN2;
	if(tok->ttype==TOK_VAR) lstoken=tok;
	else lstoken=NULL;
	return 0.0;
}

double factor_comma(){
	// MESG("factor_comma:");
	NTOKEN2;
	return 0.0;
}

double factor_eof(){
	// MESG("factor_eof!!!!");
	current_active_flag=0;
	return 0.0;
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
	(FFunction)cexpr_notequal,	// TOK_NOTEQUAL	,
	(FFunction)cexpr_smaller,	// TOK_SMALLER		,	/* <  */
	(FFunction)cexpr_bigger,	// TOK_BIGGER		,	/* >  */
	(FFunction)cexpr_equal,	// TOK_EQUAL		,	/* ==  */
	(FFunction)cexpr_smallereq,	// TOK_SMALLEREQ	,	/* <=  */
	(FFunction)cexpr_biggereq,	// TOK_BIGGEREQ	,	/* >=  */

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
	factor_none		// TOK_OTHER,
};

void set_tok_function(tok_struct *tok, int type)
{
	// MESG("set_tok_function: type=%d",type);
	// MESG("set_tok_function: %s",tok_info(tok));
	if(tok==NULL) MESG("set_tok_function: NULL! token");
	switch(type) {
		case 0:
			// MESG("	type 0");
			if(tok->ttype==TOK_FUNC) {
				// MESG("ttype is tok_func");
				if(tok->tok_node==NULL) { set_error(tok,3003,"tok_node is null!");
				return;};
				int findex = tok->tok_node->node_index;
				// MESG("	findex=%d",findex);
				// MESG(" F tok %2d: %s type [%d -s] set factor function %d",tok->tnum,tok->tname,tok->ttype,findex);
				tok->factor_function = m_functions[findex].ffunction;
			} else {
	 			tok->factor_function = factor_funcs[tok->ttype];
				// MESG(" f tok %2d: %s type [%d -s] set factor function",tok->tnum,tok->tname,tok->ttype);
			};
			break;
		case 1:
			// MESG("	type 1");
			tok->cexpr_function = (EFunction)factor_funcs[tok->ttype];
			// MESG(" c tok %2d: %s type [%d %s] set cepr function",tok->tnum,tok->tname,tok->ttype,tok_name[tok->ttype]);

	};
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
 if(vtype_is(VTYPE_NUM)) {
	NTOKEN2;
	d1=num_term1();
	// MESG("term_plus");
	if(vtype_is(VTYPE_NUM)) {
		set_term_function(ptok,term_plus_num);
		return value+d1;
	};
	if(vtype_is(VTYPE_STRING)) {
		char svalue[MAXLLEN];
		int stat;
		stat=snprintf(svalue,sizeof(svalue),"%f%s",value,get_sval());
		if(stat>MAXLLEN) MESG("truncated 2");
		set_sval(svalue);
		return 0;
	};
	if(vtype_is(VTYPE_ARRAY)) { // num + array
		array_dat *new_array = get_array("18");
		// MESG("term_plus: numeric + array");
		if(new_array->astat==ARRAY_LOCAL) {
			set_array(dup_array_add1(new_array,value));
			ex_name="New array,add to numeric";
		} else {
			array_add1(new_array,value);
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

 if(vtype_is(VTYPE_STRING)) {	// set local value
	char svalue[MAXLLEN];
	strlcpy(svalue,get_sval(),MAXLLEN);

		NTOKEN2;
		d1=num_term1();
		 /* catanate string */
			if(vtype_is(VTYPE_STRING)) {	/* string catenate  */
				strlcat(svalue,get_sval(),sizeof(svalue));
				value=atof(svalue);
				set_sval(svalue);
				return value;
			};
			if(vtype_is(VTYPE_NUM))	{	/* string, numeric catanate  */
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
			if(vtype_is(VTYPE_ARRAY)) {
				err_num=218;
				ERROR("Add array to string not supported err %d",err_num);
				RTRN(value);				
			};
			err_num=219;
			ERROR("operation not supported err %d",err_num);
			RTRN(value);				
 };

 if(vtype_is(VTYPE_ARRAY)) {
  	array_dat *loc_array=get_array("20");
		 		NTOKEN2;
				d1=num_term1();
				if(vtype_is(VTYPE_NUM)) { // add numeric to array
					MESG("	array + numeric");
					if(loc_array->astat==ARRAY_LOCAL) {
						array_dat *new_array;
						new_array=dup_array_add1(loc_array,d1);
						set_array(new_array);
						print_array1("after a+num",new_array);
					} else {
						array_add1(loc_array,d1);
					};
					set_vtype(VTYPE_ARRAY);
					ex_name="Add to numeric";
					MESG("	array + numeric: ok!");
					return 0;
				} else if(vtype_is(VTYPE_ARRAY)) {	// array addition
					array_dat *loc_array2;
					loc_array2=array_add2(loc_array,get_array("21"));
					set_vtype(VTYPE_ARRAY);
					set_array(loc_array2);
					ex_name="Add to array";
					RTRN(value);
				} else {
					err_num=220;
					ERROR("operation not_supported err %d",err_num);
					RTRN(value);
				};		
 };

#if	1
 if(vtype_is(VTYPE_AMIXED)) {
  	array_dat *loc_array=get_array("20");
		 		NTOKEN2;
				d1=num_term1();
				if(vtype_is(VTYPE_NUM)) { // add numeric to array
					MESG("	mixed array + numeric");
					if(loc_array->astat==ARRAY_LOCAL) {
						array_dat *new_array;
						new_array=dup_array_add1(loc_array,d1);
						set_array(new_array);
						print_array1("after a+num",new_array);
					} else {
						array_add1(loc_array,d1);
					};
					set_vtype(VTYPE_ARRAY);
					ex_name="Add to numeric";
					MESG("	array + numeric: ok!");
					return 0;
				} else if(vtype_is(VTYPE_ARRAY)) {	// array addition
					array_dat *loc_array2;
					loc_array2=array_add2(loc_array,get_array("21"));
					set_vtype(VTYPE_ARRAY);
					set_array(loc_array2);
					ex_name="Add to array";
					RTRN(value);
				} else {
					err_num=220;
					ERROR("operation not_supported err %d",err_num);
					RTRN(value);
				};		
 };
#endif

	// ERROR("term_plus: operation not supported!, vtype=%d line %d",ex_var.var_type,tok->tline);
	NTOKEN2;
	set_error(tok,221,"operation not supported");
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
		array_dat *new_array=get_array("22");
		if(new_array->astat==ARRAY_LOCAL) {
			set_array(dup_array_sub1(new_array,value));
			ex_name="New array,subtract from numeric";
		} else {
			array_sub1(new_array,value);
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

double expression(char *from)
{
	// set_vdval(0);
	return lexpression();
}

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

double lexpression()
{
 TDS("lexpression");
 // MESG("# lexpression:0 start [%s]",tok_info(tok));
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


double cexpression()
{
 TDS("cexpression");
 // MESG(";cexpression [%s]",tok_info(tok));
 double value = num_expression();

 if(tok->tgroup!=TOK_COMPARE) RTRN(value);
 tok_struct *tok0=tok;
 NTOKEN2;
 if(vtype_is(VTYPE_STRING)) {
	static char svalue[MAXLLEN];
	 
	strlcpy(svalue,get_sval(),MAXLLEN);
	num_expression();
	if(!vtype_is(VTYPE_STRING)) {
		syntax_error("string with no string comparison error",223);
		set_vtype(VTYPE_NUM);
		RTRN(0);	/* it is an error to compare string with number  */
	};
	set_vtype(VTYPE_NUM);
	int lresult=scmp(svalue,get_sval());

	// clean_saved_string(0);
	RTRN(tok0->cexpr_function(lresult,0));
 } else {
	double v2=num_expression();
	RTRN(tok0->cexpr_function(value,v2));
 }
}

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
	double v1=lexpression();
	sslot->dval=v1;
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
		return(v1);
	};

	int stype=sslot->var_type;;
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
					if(stype!=lmvar->var_type) MESG("	different subtype!!!");
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
			array_dat *new_array = get_array("32");
			if(new_array->anum != sslot->adat->anum) {
 				if(sslot->adat->dval) free_array("assign",sslot->adat);
				if(sslot->adat->sval) {
					// MESG("free string array!");
				};
				sslot->adat=new_array;
				if(new_array->astat==ARRAY_ALLOCATED) new_array->astat=ARRAY_LOCAL;	/* make it local to variable  */
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

void  skip_args1(int nargs)
{
#if    1
 if(nargs) tok +=1+2*nargs;
 else tok+=2;
#else
 // static int skips[]={2,3,5,7,9,11};
 // tok += skips[nargs];
 // MESG("skip_args1: va!=symbols! ++++++++++");
 NTOKEN2; /* skip name */
	int i;
	for(i=0;i<nargs;i++) {
		NTOKEN2;	/* arg */
		if(tok->ttype==TOK_RPAR) break;
		NTOKEN2;	/* skip separator  */
	};
 NTOKEN2;	/* skip right parenthesis  */
#endif
}

#include "mlang_parser.c"

// skip next sentence in a list
void skip_sentence1()
{
 TDS("skip_sentence1");
 // MESG("skip_sentence: ttype=%d",tok->ttype);
 if(tok->ttype==TOK_LCURL) {
		tok=tok->match_tok; 
		// NTOKEN2;
		// MESG("skip_sentence1");
		return; 
 };

 int plevel=0;
 for(;tok->ttype!=TOK_EOF;NTOKEN2)
 {
 	// MESG("	skip [%s]",tok_info(tok));
	switch(tok->ttype) {
		case TOK_DIR_ELSE:	/* this one starts a new sentence!!  */
		case TOK_SEP:
			set_tok_function(tok,1);
			NTOKEN2;
			return;
		case TOK_LPAR:
			plevel++;
			break;
		case TOK_RPAR:
			plevel--;
			if(plevel<0) {
				return;
			};
			break;
		case TOK_CMD:
		case TOK_FUNC:
		case TOK_DIR_IF:
		case TOK_DIR_FORI:
		case TOK_DIR_FOR:
		case TOK_DIR_WHILE:
			plevel++;
			break;
		case TOK_RCURL:
			NTOKEN2;
			return;
	};
 };
}


char *ddot_string()
{
 static char ddot_string[256];
 TextPoint *tp = tok->ddot;
 FILEBUF *buf = tp->fp;
 offs ddot_pos = tp_offset(tok->ddot);
 offs i,sl,el; // current offset, start,end of line

 el=FLineEnd(buf,ddot_pos);
 sl=FLineBegin(buf,ddot_pos);

 ddot_pos=el-sl;
	for(i=sl;i<el;i++) {
		int c=FCharAt(buf,i);
		if( c==':' || i-sl>sizeof(ddot_string)-1) { 
			ddot_string[i-sl]=0;
			break;
		};
		ddot_string[i-sl]=c;
	};
  return ddot_string;
}

void update_ddot_line(char *ddot_out)
{
 // FILEBUF *old_fp;
 TextPoint *tp = tok->ddot;
 FILEBUF *buf = tp->fp;
 int dsize=0;
 int is_ddot=0;
 offs ddot_pos=tp_offset(tp);
 offs i,sl,el; // current offset, start,end of line
 // old_fp=cbfp;
 // sfb(buf);
 el=FLineEnd(buf,ddot_pos);
 sl=FLineBegin(buf,ddot_pos);

 dsize=el-sl;
 ddot_pos=el-sl;
	for(i=sl;i<el;i++) {
		if(FCharAt(buf,i)==':') { 
			is_ddot=!(buf->b_state & FS_VIEW);
			ddot_pos=i-sl+1;break;};
	};
 
// replace text
 textpoint_set(buf->tp_current,sl+ddot_pos);
 if(is_ddot) DeleteBlock(buf,0,dsize-ddot_pos);

 if(err_num>0) {
 	insert_string(buf," ,err ",6);
	if(err_str!=NULL) insert_string(buf,err_str,strlen(err_str));
 } else {
	insert_string(buf,ddot_out,strlen(ddot_out));
 }
 // sfb(old_fp);
}

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

/* exec multiple sentences at the same level */
double exec_block1(FILEBUF *fp)
{
 INIT_STAGE;
 exe_buffer=fp;
 double val=0;
   // MESG("exec_block1:[%s] err_num= %d %d tok=[%s]",fp->b_fname,err_num,current_active_flag,tok_info(tok));
   if(!current_active_flag) {
		tok=fp->end_token;
		return(ex_var.dval);
   };
   while(tok->tgroup!=TOK_END) 
   {
	// MESG_TOK_INFO("- exec_block1 [%s]",tok);
#if	1
	if(tok->ttype==TOK_SEP){ NTOKEN2;
		// MESG("factor_sep: [%s %d]",tok->tname,tok->ttype);
		if(tok->ttype==TOK_VAR) lstoken=tok;
		else lstoken=NULL;
		continue;
	};
#endif
#if	0
	if(tok->ttype==TOK_RCURL) { NTOKEN2;lstoken=NULL;return(ex_var.dval);};
#endif
 	val=tok->directive();
	if(ex_var.var_type==VTYPE_NUM) ex_var.dval=val;
	if(!current_active_flag) break;
   };
	if(tok->ttype==TOK_RCURL) { NTOKEN2;lstoken=NULL;return(ex_var.dval);};

   // MESG("exec_block1: end!");
	return(val);
}

double exec_block1_break(FILEBUF *fp)
{
 // double val=0;
 INIT_STAGE;
 exe_buffer=fp;
 double val=0;
	// MESG("#exec_block_break:%d ttype=%d [%s]",tok->tnum,tok->ttype,tok_info(tok));
   if(!current_active_flag|| err_num>0) {
		tok=fp->end_token;
		return(ex_var.dval);
   };
   while(tok->tgroup!=TOK_END) 
   {
	// MESG(";exec_block:%d ttype=%d",tok->tnum,tok->ttype);
#if	1
	if(tok->ttype==TOK_SEP){ NTOKEN2;
		// MESG("factor_sep: [%s %d]",tok->tname,tok->ttype);
		if(tok->ttype==TOK_VAR) lstoken=tok;
		else lstoken=NULL;
		continue;
	};
#endif
#if	0
	if(tok->ttype==TOK_RCURL) { NTOKEN2;lstoken=NULL;return(ex_var.dval);};
#endif
 	val=tok->directive();
	if(ex_var.var_type==1) ex_var.dval=val;

	if(!current_active_flag) break;
	if(drv_check_break_key()) break;
	// MESG(" [%s]",tok_info(tok));
   };
	if(tok->ttype==TOK_RCURL) { NTOKEN2;lstoken=NULL;return(ex_var.dval);};
	return(val);
}

/* execute a block from a file  */
double compute_block(FILEBUF *bp,FILEBUF *use_fp,int start)
{
 double val=0;
 // int extra=0;
 MVAR *local_symbols;
 MVAR *old_symbol_table=current_stable;
 tok_struct *old_tok=tok;
	MESG("# [%-15s %s ---------------------------------------------",bp->b_fname,VERSION);
 if(show_tokens) {
	parse_buffer_show_tokens(1);
	return(0);	
 };

 if(use_fp->symbol_tree==NULL) {
	// MESG("create new symbol_tree for use_fp!");
 	use_fp->symbol_tree=new_btree(use_fp->b_fname,0);
 };
 int old_items=0;
	if(start==0 && current_stable!=NULL) {
		old_items=use_fp->symbol_tree->items;
	};
	parse_block1(bp,use_fp->symbol_tree,start);
	// MESG("parse_block: ended! err=%d start=%d items=%d",err_num,start,use_fp->symbol_tree->items);
	if(err_num) { execmd=0;return(0);};
	// MESG("	comput_block: start=%d",start);
	if(start || current_stable==NULL) {
		// MESG("new current_stable with %d items",use_fp->symbol_tree->items);
		local_symbols=new_symbol_table(use_fp->symbol_tree->items);
	} else {
		// MESG("use current_stable new items = %d",use_fp->symbol_tree->items);
		local_symbols=realloc_symbol_table(current_stable,use_fp->symbol_tree->items,old_items);
	}
	current_stable=local_symbols;

 if(bp->m_mode<2)	/* if not already checked!  */
 {
	err_num=check_init(bp);
	if(err_num>0) 
	{
		// mesg_out("Error %d %s line %d ex_vtype=%d ex_value=%f slval=[%s]!",err_num,err_str,err_line,get_vtype(),get_val(),get_sval());
		show_error("Check init",bp->b_fname);
		return(0);
	};
	init_exec_flags();
	tok=bp->tok_table;

	drv_start_checking_break();
	// MESG("	call exec_block1 ------");
	if(execmd) val=exec_block1(bp);
	else val=exec_block1_break(bp);
	// MESG("	after exec_block1 !!!!!");
	drv_stop_checking_break();

	/* cleaning  */
	// MESG("cleaning:");
	if(start) {
		if(local_symbols)
		if(bp->symbol_tree){
			delete_symbol_table(local_symbols,bp->symbol_tree->items,0);
			bp->symbol_tree=NULL;
		};
		current_stable=old_symbol_table;
	};

	if(vtype_is(VTYPE_STRING)) msg_line("Result is \"%s\"",get_sval());
	if(vtype_is(VTYPE_NUM)) msg_line("Result is [%f]",val);
 } else {
 	msg_line("parse error %d on %s ",err_num,bp->b_fname);
	val=0;
 };
 tok=old_tok;
 // MESG("compute_block return %f",val);
 return(val); 
}

int empty_tok_table(FILEBUF *fp)
{
 tok_struct *table= fp->tok_table;
 tok_struct *tokdel;
 // MESG("empty_tok_table:");
 if(table==NULL) {
 	// MESG("empty_tok_table: already clean!");
	return(0);
 };
 tokdel=table;
 for(tokdel=table;tokdel->ttype!=TOK_EOF;tokdel++){
	// MESG("delete token %d type=%d",tokdel->tnum,tokdel->ttype);
	if(tokdel->ttype==TOK_VAR || tokdel->ttype==TOK_QUOTE){
 		if(tokdel->tname!=NULL) free(tokdel->tname);
	};
 };
 if(fp->tok_table) {
	 // MESG("free the table");
	free(fp->tok_table);
	// MESG("empty_tok_table: set NULL!");
	fp->tok_table=NULL;
 };
 fp->err=0;
 fp->m_mode=0;
 return(1);
}

int refresh_current_buffer(num nused)
{
 double val=0;
 FILEBUF *fp=cbfp;
 exe_buffer=cbfp;
 num curline = tp_line(cwp->tp_current);

 if(!is_mlang(fp)) return 0;
 err_num=0;
 err_line=0;
 is_break1=0;
 show_stage=0;
 INIT_STAGE;

 /* clear parse list  */
 // MESG("refresh_current_buffer: call empty_tok_table: [%s]",fp->b_fname);
 empty_tok_table(fp);
 clean_saved_string(0);
 fp->err=-1;
 // MESG("refresh_current_buffer:1 [%s] %d",fp->b_fname,fp->b_type);
 parse_block1(fp,fp->symbol_tree,1);

 if(err_num<1){	/* if no errors  */
	current_stable=new_symbol_table(fp->symbol_tree->items);

 	msg_line("checking ...");
	drv_start_checking_break();
	if(check_init(fp)>0) {
		drv_stop_checking_break();
		show_error("refresh buffer",fp->b_fname);
		// msg_line("syntax error %d line %d [%s]",err_num,err_line,err_str);
		// mesg_out("syntax error %d line %d [%s]",err_num,err_line,err_str);
		return(0);
	};
	// MESG("refresh_current_buffer: after check_init");
 	msg_line("evaluating ...");
	init_exec_flags();
	tok=fp->tok_table;
	val=exec_block1_break(fp);
	drv_stop_checking_break();
	if(err_num>0) {
		macro_exec=0;
		show_error("refresh buffer",fp->b_fname);
		// msg_line("Error %d [%s] at line %d",err_num,err_str,err_line);
		// mesg_out("Error %d [%s] at line %d",err_num,err_str,err_line);
	} else {
		if(vtype_is(VTYPE_STRING)) msg_line("Result is \"%s\"",get_sval());
		else if(vtype_is(VTYPE_NUM)) msg_line("Result is [%f]",val);
		else if(get_sval()) msg_line("Result is [%s %f]",get_sval(),val);
	};
 } else {
 	msg_line("parse error %d line %d [%s]",err_num,err_line+1,err_str);
	return(0);
 };

 if(plot_on()) plot_redraw();
 textpoint_set_lc(cwp->tp_current,curline,0);
 msg_line("");
 set_update(cwp,UPD_EDIT);
 return(OK_CLRSL);
}

// parse, check current buffer
int parse_check_current_buffer(num n)
{
 FILEBUF *fp=cbfp;

 err_num=0;
 err_line=0;
 show_stage=1;
 // MESG("parse_check_current_buffer: %d",is_mlang(fp));
 if(!is_mlang(fp)) return 0;

 /* clear parse list  */
 empty_tok_table(fp);
 INIT_STAGE;
 // clear out buffer
 cls_fout("[out]");
 // MESG("clear output");
 err_num=check_init(fp);
 // MESG("parse_check_current_buffer: after check_init");
 if(err_num>0) {
	macro_exec=0;
	msg_line("syntax error %d line %d [%s]",err_num,err_line,err_str);
	show_error("parse_check",fp->b_fname);
	igotolinecol(err_line+1,1,1);
 	return(0);
 } else {
 	msg_line("No errors!");
 	return(1);
 };
}

int parse_buffer_show_tokens(num n)
{
 FILEBUF *fp=cbfp;
 tok_struct *tok_ind=NULL;
 int ind=0;
 err_num=0;
 err_line=0;
 show_stage=2;
 if(!is_mlang(fp)) return 0;

 /* clear parse list  */
 empty_tok_table(fp);

 INIT_STAGE;
 // clear out buffer
 cls_fout("[out]");
 err_num=check_init(fp);
 tok_ind=fp->tok_table;
 // MESG("Print token table to out buffer");
 if(tok_ind==NULL) {
	msg_line("parsing buffer produced no table!");
	return(0);
 };
 out_print("|-------- Token list -----------------------------------",1);
 // if(tok_ind==NULL) return 0;
 while(1)
 {
	if((ind%25)==0)  out_print("Num Line Ind Type               Val        Group",1);
	out_print(tok_info(tok_ind),1);
	if(tok_ind->ttype==TOK_EOF) break;
	tok_ind++;
	ind++;
 };
 
 if(err_num>0) {
	macro_exec=0;
	show_error("parse_buffer_show_tokens",fp->b_fname);
	// MESG("error go to line %d",err_line);
	// msg_line("syntax error %d line %d [%s]",err_num,err_line,err_str);
	if(!execmd) igotolinecol(err_line,1,1);
 	return(0);
 } else {
 	msg_line("No errors!");
 	return(1);
 };
}

void MESG_TOK_INFO(char *title,tok_struct *tok)
{
	static int prev_tok_num=-1;
	if(tok->tnum!=prev_tok_num) {
		MESG("%-25s : %s",title,tok_info(tok));
		prev_tok_num=tok->tnum;
	};
}

char * tok_info(tok_struct *tok)
{
 static char stok[MAXLLEN];
	if(tok==NULL) { MESG("tok_info: NULL token!");return "null token";};
	// MESG("tok_info: ttype=%d",tok->ttype);

	if(tok->tname!=NULL){
		// MESG("tok_info: %d %s %d",tok->tind,tok->tname,tok->tline);
		if(tok->ttype==TOK_ARRAY1 || tok->ttype==TOK_ARRAY2) {
			int rows=0;
			int cols=0;
			
			if(tok->tok_adat) {
				rows=tok->tok_adat->rows;
				cols=tok->tok_adat->cols;
			};
			snprintf(stok,sizeof(stok),"%3d:%4d %3d [%2d=%8s] [%5s] rows=%d cols=%d",
				tok->tnum,tok->tline,tok->tind,tok->ttype,TNAME,(char *)tok->tname,rows,cols);
		} else 
		if(tok->ttype==TOK_SHOW) { snprintf(stok,sizeof(stok),"%3d:%4d %3d [%2d=%8s] [:]",tok->tnum,tok->tline,tok->tind,tok->ttype,TNAME);
		} else
		if(tok->ttype==TOK_LCURL||tok->ttype==TOK_RCURL) {
				// snprintf(stok,sizeof(stok),"%3d:%4d CURL",tok->tnum,tok->tline);
				snprintf(stok,sizeof(stok),"%3d:%4d %3d [%2d=%8s] %s other is %d",tok->tnum,tok->tline,tok->tind,tok->ttype,TNAME,(char *)tok->tname,tok->match_tok->tnum);
		} else
		if(tok->tgroup>0) {
			// snprintf(stok,sizeof(stok),"%3d:%4d %s",tok->tnum,tok->tline,tok->tname);
			snprintf(stok,sizeof(stok),"%3d:%4d %3d [%2d=%8s] [%5s] [%2d:%5s]!!",tok->tnum,tok->tline,tok->tind,tok->ttype,TNAME,(char *)tok->tname,tok->tgroup,tname(tok->tgroup));
		} else 
			if(tok->ttype==TOK_NUM) { 
			// snprintf(stok,sizeof(stok),"%3d:%4d %s",tok->tnum,tok->tline,tok->tname);
			snprintf(stok,sizeof(stok),"%3d:%4d %3d [%2d=%8s] %5.1f",tok->tnum,tok->tline,tok->tind,tok->ttype,TNAME,tok->dval);
		} else if(tok->ttype==TOK_QUOTE) {
			// snprintf(stok,sizeof(stok),"%3d:%4d %s",tok->tnum,tok->tline,tok->tname);
			snprintf(stok,sizeof(stok),"%3d:%4d %3d [%2d=%8s] \"%s\"",tok->tnum,tok->tline,tok->tind,tok->ttype,TNAME,(char *)tok->tname);
		} else if(tok->ttype==TOK_VAR) {
			// MESG("TOK_VAR:");
			BTNODE *var_node = tok->tok_node;
			int size=0;
			int vtype=0;
#if	0
			char *var_name="unknown";
			if(var_node!=NULL) {
				vtype=var_node->node_vtype;
				var_name=var_node->node_name;
			};
#endif
			// MESG("TOK_VAR: vtype=%d",vtype);
			// MESG("tok_info var! ind=[%d] group=%d vtype=%d",tok->tind,tok->tgroup,vtype);
			if(vtype==VTYPE_TREE) {
				BTREE *type_tree=(BTREE *)var_node->node_dat;
				size = type_tree->items;
			};
			// snprintf(stok,sizeof(stok),"%3d:%4d %s",tok->tnum,tok->tline,tok->tname);

			snprintf(stok,sizeof(stok),"%3d:%4d %3d [%2d=%8s] [%5s] %8s %d size %d",
				tok->tnum,tok->tline,tok->tind,tok->ttype,TNAME,(char *)tok->tname,vtype_names[vtype] ,vtype,size);
		} else {
			// snprintf(stok,sizeof(stok),"%3d:%4d %s",tok->tnum,tok->tline,tok->tname);
			snprintf(stok,sizeof(stok),"%3d:%4d %3d [%2d=%8s] [%5s]",tok->tnum,tok->tline,tok->tind,tok->ttype,TNAME,(char *)tok->tname);
		};
// 			
	} else {
			return "null tok name !!!!!!!!!!!!!!!!!!!!!!!!!!!";
		     snprintf(stok,sizeof(stok),"%3d:%4d %3d [%2d=%8s] [%5.1f]",tok->tnum,tok->tline,tok->tind,tok->ttype,"null name",tok->dval);
	};
	// MESG("tok_info: end");
	return stok;
}

int show_parse_buffer(num n)
{
 FILEBUF *fp;
 TDS("show parse");

 char *parsebuf="[lex_out]";
 FILEBUF *outbuf;
 tok_struct *tok_table,*tok_ind;
 fp=cbfp;

 // MESG("show_parse_buffer:");
 if(!is_mlang(fp)) return 0;

 err_num=0;
 err_line=0;
 show_stage=0;
 /* clear parse list  */
 empty_tok_table(fp);
 parse_block1(fp,fp->symbol_tree,1);

 tok_table=fp->tok_table;
 tok_ind=tok_table;
 if(tok_ind==NULL) {
	msg_line("parsing buffer produced no table!");
	return(0);
 }
 cls_fout(parsebuf);
 while(1)
 {
	out_fprint(parsebuf,tok_info(tok_ind),1);
	if(tok_ind->ttype==TOK_EOF) break;
	tok_ind++;	
 };

 outbuf=get_filebuf(parsebuf,NULL,0);
 select_filebuf(outbuf);
 goto_bof(1);
 set_hmark(1,"select out buffer");
 return(1); 
}


/* return string results from an expression */
char * key_str1()
{
 // MESG("key_str1: tnum=%d ttype=%d",tok->tnum,tok->ttype);
 num_expression();
 // MESG("key_str1: ex_vtype=%d ex_value=%f [%s]",get_vtype(),ex_value,get_sval());
 return (get_sval());
}

inline double get_val()
{
	return ex_var.dval;
}

array_dat *get_array(char *pos)
{
	// MESG("get_array: %s",pos);
	if(ex_var.adat==NULL) MESG("ex_var.adat at %s is NULL!",pos);
	// else MESG("get_array:[%s] num=%d type=%d tok [%s]  ind=%d num=%d type=%d atype=%d",
		// pos,ex_var.adat->anum,ex_var.var_type,tok->tname,tok->tind,tok->tnum,tok->ttype,ex_var.adat->atype);
	// if(ex_var.var_type==1) return NULL;
	return ex_var.adat;
}

void set_array(array_dat *a)
{
	// MESG("set_array:num=%d type %d allocated=%d name=%s",a->anum,a->atype,a->astat,a->array_name);
	ex_var.adat=a;
	ex_var.var_type=a->atype;
}

char *get_sval()
{
// 	MESG("get_sval: tnum=%d type=%d %f ex_value=%f",tok->tnum,tok->ttype,tok->dval,ex_value);
	if(saved_string==NULL) {
		saved_string=(char *)malloc(16);
		snprintf(saved_string,16,"%f",get_val());
		// MESG("get_sval: new saved_string! [%s]",saved_string);
	};
	return(saved_string);
}

inline int get_vtype()
{
#if	0
	if(ex_var.var_type>=VTYPE_OTHER || ex_var.var_type<0) {
		MESG("get_vtype: error %d",ex_var.var_type);
		ex_var.var_type=VTYPE_NONE;
		return VTYPE_NONE;
	};
#endif
	return ex_var.var_type;
}

inline int vtype_is(int type)
{
	// MESG("vtype_is: type=%d ex type=%d",type,ex_var.var_type);
	return type==ex_var.var_type;
}

inline void set_vtype(int type)
{
	// MESG("set_vtype: %X",type);
	ex_var.var_type=type;
}

void set_nsval(char *s,int max)
{
 clean_saved_string(max);
 memcpy(saved_string,s,max);
 saved_string[max]=0;
 // MESG(";set_nsval:s=[%s] [%s] %d",s,saved_string,max);
 // MESG("set_nsval:");
 ex_var.var_type=VTYPE_STRING;
}

void set_sval(char *s)
{
 clean_saved_string(0);
 saved_string=strdup(s);
 // MESG("set_sval:");
 ex_var.var_type=VTYPE_STRING;
}

inline void set_vdval(double value){
	// MESG("set_vdval: %f",value);
	ex_var.var_type=VTYPE_NUM;
	ex_var.dval=value;
}

inline void set_dval(double value)
{
	// MESG("set_dval: %f",value);
	ex_var.dval=value;
}

#include "mlang_err.c"

/*	nextarg:	get the next argument from console or from macro */
int nextarg(char *prompt,char *buffer, int size,int show)
/* prompt to use if we must be interactive */
/* buffer to put token into */
/* size of the buffer */
{
	/* if we are interactive, go get it! */
	// MESG("nextarg:");
	if (macro_exec == FALSE) {
		// MESG("getstring: %s",prompt);
		if(getstring(prompt, buffer, size,show)!=FALSE) {
			// MESG("nextarg: buffer=[%s]",buffer);
			set_dval(atof(buffer));
		} else {
			set_update(cwp,UPD_MOVE);
			return(FALSE);
		};
	} else {
		/* slval has already the next argument */
		// MESG("nextarg: slval=%s",get_sval());	
		strlcpy(buffer,get_sval(),size);
	};
	return(TRUE);
}

/*
 To do
 matrix division ??
*/

// Some utility btree functions

/* get a double value from a node named name from main table  */
double bt_dval(char *name)
{
// MESG("bt_dval: [%s] %f",name,btndval(bt_table,name));
 return(btndval(bt_table,name));
}

/* get a string value from a node named name from main table */
char * bt_sval(char *name)
{
 return(btnsval(bt_table,name));
}

/* set type, numeric value for a node named name of main table  */
void set_bt_num_val(char *name,double val)
{
 BTNODE *node = set_btdval(bt_table,name,val);
 node->node_type=TOK_OPTION;
 node->node_vtype=VTYPE_NUM;
}

/* -- */
