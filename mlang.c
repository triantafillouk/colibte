/*
	Curses,gtk editor,notes,directory bnfrowser
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
inline static void  skip_args1(int nargs);
inline static MVAR *get_left_slot(int ind);
inline static int check_token(int type);

#if	TNORMAL
static inline double num_expression();
static inline double num_term1();
static inline double num_term2();
double cexpression();
#endif
int check_init(FILEBUF *bf);
double exec_function(FILEBUF *bp,int nargs);
MVAR * push_args_1(int nargs,int vars_num);
double compare_notequal(double value);
double compare_smaller(double value);
double compare_bigger(double value);
double compare_smallereq(double value);
double compare_biggereq(double value);
double compare_equal(double v1);
void set_bnf_function1(tok_struct *tok, int type);
#if	TBNF
static inline void bnf_factor_dummy();
#endif
void delete_symbol_table(MVAR *td, int size,int nargs);
MVAR *new_symbol_table(int const size);
void init_vars(MVAR *head,int size);
void set_tok_function(tok_struct *tok, int type);
void set_tok_directive(tok_struct *tok, FFunction directive);
MVAR *btree_to_mvar(BTREE *bt);
void skip_sentence1();
inline MVAR *get_left_slot(int ind);
void show_error(char *from,char *name);
char *ddot_string();
void update_ddot_line(char *ddot_out);

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
double factor_refresh_ddot();

double assign_val(double none);
double assign_env(double none);
void init_error();
void get_lowercase_string(char *lower, char *string);
void get_uppercase_string(char *lower, char *string);
double exec_block1_break(FILEBUF *fp);
char * tok_info(tok_struct *tok);
void MESG_TOK_INFO(char *title,tok_struct *tok);

void set_vtype(int type);
int vtype_is(int type);
int get_vtype();
#if	TBNF
int bnf_debug();
#endif
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
static int current_active_flag=1;	/* on to execute procedure!  */

void set_active_flag(int flag)
{
	current_active_flag=flag;
}

int get_active_flag()
{
	return current_active_flag;
}

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
#if	TNORMAL
#include "mlang_functions.c"
#endif
#if	TBNF
#include "bnf_expr.c"
#include "bnf_mlangf.c"
#endif
#if	TNORMAL
#include	"mlang_expr.c"
#endif

void eval_curl_match(tok_struct *tok)
{
 static tok_struct *curl_stack[100];
 static int left_curl_index=0;
	
 	if(tok==NULL) {
		left_curl_index=0;
		// MESG("initialize left_curl_index");
		return;
	};
	if(tok->ttype==TOK_LCURL) {// push left curl
		curl_stack[left_curl_index] = tok;
		// MESG("set left_curl_index left: %d [%s]",left_curl_index,tok_info(tok));
		left_curl_index++;
		return;
	};
	if(tok->ttype==TOK_RCURL) { // pull left curl and set match_token
		left_curl_index--;
		// MESG("set left_curl_inde right: %d [%s]",left_curl_index,tok_info(tok));
		tok_struct *left_curl_tok = curl_stack[left_curl_index];
		tok->match_tok = left_curl_tok;
		left_curl_tok->match_tok = tok; 
		// MESG("set curl match left=%d right=%d",left_curl_tok->tnum,tok->tnum);
	};
}

tok_struct * stack_push(char *title,tok_struct *tok,int exp_type)
{
  // MESG("--[%s] try push %d [%s] exp=%d [%s]",check_buffer->b_fname,tok->pushed,title,exp_type,tok_info(tok));
#if	TBNF
 // if(no_push) { MESG("stack_push:%s skip %s",title,tok_info(tok));return;};
 if(tok!=NULL) {
 	// MESG("stack_push! at %p",check_buffer->tok_table_bnf+check_buffer->tok_bnf_index);
	if(tok->pushed>=0) {
		if(check_buffer)
		// MESG("P[%10s already pushed at %3d %-15s|%s %p",check_buffer->b_fname,tok->pushed,title,tok_info(tok),tok);
		// else MESG("P[ check buffer is NULL!!!!! %s",title);
		return check_buffer->tok_table_bnf+tok->pushed;
	} else {
		if(tok->ttype==TOK_LPAR && exp_type>0) { 
			MESG("skip left paranthesis!");
			return NULL;
		};
		tok_struct *dest = check_buffer->tok_table_bnf+check_buffer->tok_bnf_index;
		memcpy((void *)dest,(void *)tok,sizeof(tok_struct));
    	tok->pushed=check_buffer->tok_bnf_index;
		// MESG("! set pushed! as %d [%s] exp_type=%d",tok->pushed,tok_info(tok),exp_type);
		// dest->ttype=exp_type;
		set_bnf_function1(dest,exp_type);

		if(dest->ttype==TOK_LCURL||dest->ttype==TOK_RCURL) {
			eval_curl_match(dest);
			if(dest->ttype==TOK_LCURL) dest->bnf_factor_function = bnf_dir_lcurl;
		};
		if(dest->ttype==TOK_RCURL) {
			tok_struct *p=dest-1;
			if(p->ttype==TOK_SHOW || p->ttype==TOK_RCURL) dest->bnf_factor_function=bnf_factor_rcurl_no;
			else
			if(p->ttype!=TOK_SEP) dest->bnf_factor_function=bnf_factor_rcurl0;
			else dest->bnf_factor_function=bnf_factor_rcurl;
		};
 		// MESG("P[%10s %3d %-15s|%s",check_buffer->b_fname,check_buffer->tok_bnf_index,title,tok_info(dest));
		if(dest->ttype==TOK_FUNC) {
			// MESG("	set bnf function! index=%d for [%s]",tok->tok_node->node_index,tok_info(dest));
			dest->bnf_factor_function=bnf_functions[tok->tok_node->node_index].vfunction;
		};
		check_buffer->tok_bnf_index++;
		return dest;
   }
 } else {
 	MESG("P [%s] null token!!!",title);
	return NULL;
 }
#endif
 return NULL;
}

void delete_type_tree(BTREE *type_tree)
{
	MESG("delete_type_tree:");
	free_btnode(type_tree->root);
	free(type_tree);
}

array_dat *transpose(array_dat *array1);

inline void init_vars(MVAR *head,int size)
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

void init_btree_table()
{
	bt_table=new_btree("table",0);
	directiv_table=new_btree("directives",0);
#if	USE_TYPE_VARS
	global_types_tree=new_btree("types",0);
#endif
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

inline static void  skip_args1(int nargs)
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




void sarray_mul1(array_dat *sarray, double factor);


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
#if	TBNF
 tok->pushed=-1;
 tok->bnf_group=-1;
 tok->bnf_factor_function=bnf_factor_dummy;
#endif
#if	TNORMAL
 tok->factor_function=factor_none;
 tok->directive=lexpression;
#endif
 tok->tok_node=NULL;
 // MESG("new_tok:");
 return(tok);
}

#define	TNAME	tname(tok->ttype)

void ntoken()
{
	tok++;
}

int tok_type()
{
	return tok->ttype;
}

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
#if	TBNF
  for(i=0; bnf_functions[i].f_name != NULL;i++){ 
	insert_bt_element(bt_table,bnf_functions[i].f_name,TOK_FUNC,i);
  };
#else
  for(i=0; m_functions[i].f_name != NULL;i++){ 
	insert_bt_element(bt_table,m_functions[i].f_name,TOK_FUNC,i);
  };
#endif

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
			fprintf(stderr,"new_symbol_table: overflow: available=%ld required=%ld\n",call_stack_available-call_stack,call_stack_used-call_stack);
			exit(0);
		} else {
			msg_line("new_symbol_table: overflow: available=%d required=%d",call_stack_available-call_stack,call_stack_used-call_stack);
		};
		set_error(tok,301,"call_stack overflow");
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
 // MESG("delete_symbol_table: << nargs=%d size=%d",nargs,size);
 MVAR *sslot=td;
 int i=0;
 for(;i<nargs;i++,sslot++)
 {
	// string arguments are dublicated, must be freed
 	if(sslot->var_type==VTYPE_STRING) {
		if(sslot->var_alloced)
		{
			free(sslot->sval);
			sslot->sval=NULL;
		};
	};
 };

 // for(i=nargs;i<size;i++) 
 for(;i<size;i++,sslot++) 
 {
	// MESG("dst vars %d",i);
	if(sslot->var_type==VTYPE_STRING) {
		// MESG("delete_symbol_table:%d [%s] free %X",i,sslot->sval,sslot->sval);
		 	if(sslot->var_alloced)  {
				free(sslot->sval);
				sslot->sval=NULL;
			};
	} else
	if(sslot->var_type==VTYPE_ARRAY) {
		// MESG("delete_symbol_table:array %s",sslot->adat->array_name);
		if(sslot->adat!=NULL) {
			if(sslot->adat->astat==ARRAY_ALLOCATED
				||sslot->adat->astat==ARRAY_LOCAL
			){
				free_array_dat(sslot->adat);
				free(sslot->adat);
			};
			sslot->var_type=VTYPE_NUM;
			sslot->dval=0;
		};
	};
 };

 // MESG("delete_symbol_table from: >> %lld",call_stack_used-call_stack);
 call_stack_used -= size;
 // MESG("delete_symbol_table: call_stack=%lld upto %lld",call_stack_used-call_stack,call_stack_used-call_stack+nargs+size-1);
 // MESG("                    at  : %lld",call_stack_used-call_stack);
}

void init_stack()
{
	// MESG("init_stack:");
	delete_symbol_table(call_stack,call_stack_used-call_stack,0);
	// MESG("init_stack:end ok!");
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


void show_error(char *from,char *name)
{
	ERROR("%s error %d file %s line %d: [%s]",from,err_num,name,err_line,err_str);
}

void show_token_table(char *title, FILEBUF *bf,tok_struct *token_start,int size)
{
 int i=0;

 tok_struct *tokp=token_start;
 MESG("!---------- %s token table of %s size %d -------------",title,bf->b_fname,size);
 for(i=0;i<size;i++) {
	MESG("!T %3d %s",i,tok_info(tokp));
	tokp++;
 };
 MESG("------------------------------------------------------");
}

/* Check for any errors and initialize parsed list  */
int check_init(FILEBUF *bf)
{
 tok_struct *tok_table=bf->tok_table;
 FILEBUF *ori_buffer = check_buffer;
 check_buffer = bf;
 int err=0;
 INIT_STAGE;
 int checked = (bf->tok_table != NULL);
 MESG("---- check_init: [%s] %d checked=%d err=%d",bf->b_fname,bf->b_type,checked,bf->err);
 // eval_curl_match(NULL);
#if	0
 if(execmd) 
 {
 	fprintf(stderr,"exec [%s] ----------------\n",bf->b_fname);
 	fprintf(stdout,"exec [%s] ----------------\n",bf->b_fname);
 };
#endif
 if(tok_table==NULL) 
 {
 	MESG("create token table [%s] err=%d",bf->b_fname,bf->err);
	parse_block1(bf,NULL,1);
	// MESG("block parsed err = %d",err_num);
	if(err_num>0) {
		msg_line("found parsed errors: err_num=%d %s",err_num,err_str);
		check_buffer = ori_buffer;
		return(err_num);
	};
	tok_table=bf->tok_table;
	if(tok_table==NULL)	{
		ERROR("cannot parse file");
		check_buffer = ori_buffer;
		return(201);
	}
 } else {
 	// MESG("	already checked!");
 };
 // MESG("check_init:2 err=%d %d",bf->err,bf->tok_table==NULL);
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
#if	TBNFNORMAL
 if(!usebnf)
#endif
#if	TNORMAL
 show_token_table("Token table ",bf,bf->tok_table,bf->end_token - bf->tok_table+1);
#endif
 if(bf->err>0) {
	return bf->err;
 };
 bf->m_mode |= M_CHECKED;
 check_buffer = ori_buffer;
 // bf->tok_bnf_index = 0;
 // bf->tok_bnf = bf->tok_table_bnf;
#if	TBNFNORMAL
 if(usebnf)
#endif
#if	TBNF
 show_token_table("BNF ",bf,bf->tok_table_bnf,bf->tok_bnf_index);
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

#if	TNORMAL
int is_token(int type)
{
 return(tok->ttype == type);
}
#endif

inline static int check_token(int type)
{
 TDS("check_token");

 return(tok->ttype == type);
}




inline static MVAR *get_left_slot(int ind)
{
	// MESG("get_left_slot: ind=%d",ind);
	return &current_stable[ind];
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


/* user interupt on, set also break flag  */
void set_break(char *from)
{
	MESG("set_break: from %s",from);
	is_break1=1;
	// tok->tgroup=TOK_EOF;
	// tok->ttype=TOK_EOF;
	current_active_flag=0;
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


void set_bnf_function1(tok_struct *tok, int type)
{
#if	TBNF
 if(type==0) {
	int exp_type = factor_bnf_type[type];
	tok->bnf_group=exp_type;
	tok->bnf_factor_function = factor_bnf_funcs[exp_type];
	tok->function_index=exp_type;
	// MESG("-- set_bnf_function1 to none!!!: num=%2d exp type=%3d",tok->tnum,exp_type);
 } else if(type>0) {
	int exp_type = factor_bnf_type[type];
	tok->bnf_group=exp_type;
	tok->bnf_factor_function = factor_bnf_funcs[exp_type];
	tok->function_index=exp_type;
	tok->ttype=type;
	// MESG("-- set_bnf_function1: ind=%2d exp num=%3d",tok->tnum,exp_type);
 } else {
	tok->bnf_group=0;
	tok->bnf_factor_function = factor_bnf_funcs[-type];
	tok->function_index=type;
	// MESG("-- set_bnf_function1: ind=%2d exp num=%3d",tok->tnum,type);
 };
#endif
}

#include "mlang_parser.c"

void set_tok_function(tok_struct *tok, int type)
{
#if	TNORMAL
	// MESG("set_tok_function: type=%d ttype=%d %s",type,tok->ttype,tok_info(tok));
	if(tok==NULL) MESG("set_tok_function: NULL! token");
	switch(type) {
		case 0:
			if(tok->ttype==TOK_FUNC) {
				// MESG("ttype is tok_func");
				if(tok->tok_node==NULL) { set_error(tok,303,"tok_node is null!");
				return;};
				int findex = tok->tok_node->node_index;
				// MESG(" F tok %2d: %s type [%d -s] set factor function %d",tok->tnum,tok->tname,tok->ttype,findex);
				tok->factor_function = m_functions[findex].ffunction;
			} else {
	 			tok->factor_function = factor_funcs[tok->ttype];
				// MESG(" f tok %2d: %s type [%d -s] set factor function",tok->tnum,tok->tname,tok->ttype);
			};
			break;
		case 1:
			// MESG("	1 set factor function to %d",tok->ttype);
			tok->cexpr_function = (EFunction)factor_funcs[tok->ttype];
			// tok->bnf_factor_function = factor_bnf_funcs[tok->ttype];
			// MESG(" c tok %2d: %s type [%d %s] set cepr function",tok->tnum,tok->tname,tok->ttype,tok_name[tok->ttype]);
	};
#endif
}

// skip next sentence in a list
void skip_sentence1()
{
 TDS("skip_sentence1");
 // MESG("skip_sentence: ttype=%d",tok->ttype);
 if(tok->ttype==TOK_LCURL) {
	// MESG("skip_sentence rcurl [%s]",tok_info(tok));
	tok=tok->match_tok; 
		// tok is after the corresponding rcurl!
	// MESG("skip_sentence end [%s]",tok_info(tok));
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

#if	TBNF
void msg_result(char *name,int show_no_time)
{
	// set_result();
	MVAR *result = (bnf_var->var_type==VTYPE_POINTER) ? bnf_var->var_pointer: bnf_var;
	if(show_no_time) {
		if(result->var_type==VTYPE_NUM) msg_line("%s Result (%f)",name,num_result());
		else if(result->var_type==VTYPE_STRING) msg_line("%s Result \"%s\"",name,string_result());
		else msg_line("%s Result @%d type %d",name,VARIND,result->var_type);
	} else {
		if(result->var_type==VTYPE_NUM) msg_line("[%s Result @%d (%f)",name,VARIND,num_result());
		else if(result->var_type==VTYPE_STRING) msg_line("[%s Result @%d \"%s\"",name,VARIND,string_result());
		else msg_line("[%s Result @%d type %d",name,VARIND,result->var_type);
	};
}
#else
void msg_result(char *name, int show_no_time)
{
	if(show_no_time) {
		if(vtype_is(VTYPE_NUM)) msg_line("%s Result (%f)",name,get_val());
		else if(vtype_is(VTYPE_STRING)) msg_line("%s Result \"%s\"",name,get_sval());
		else msg_line("%s Result type %d",name,get_vtype());
	} else {
		if(vtype_is(VTYPE_STRING)) msg_line("%s Result n is \"%s\"",name,get_sval());
		else if(vtype_is(VTYPE_NUM)) msg_line("%s Result n is (%f)",name,get_val());
		else msg_line("%s Result n type %d",name,get_vtype());
	};
}
#endif


/* execute a block from a file  */
double compute_block(FILEBUF *bp,FILEBUF *use_fp,int start)
{
#if	TNORMAL
 double val=0;
#endif
 MVAR *local_symbols;
 MVAR *old_symbol_table=current_stable;
 tok_struct *old_tok=tok;
	MESG("# compute_block1: [%s] use [%s] start=%d",bp->b_fname,use_fp->b_fname,start);
	if(show_no_time) MESG("# [%-15s %d %s --------------------------------------------- %d",bp->b_fname,usebnf,"Version",bp->err);
	else MESG("# [%-15s %d %s --------------------------------------------- %d",bp->b_fname,usebnf,VERSION,bp->err);
	eval_curl_match(NULL);
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
 // MESG("compute_block:2");
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

	drv_start_checking_break();
	// MESG("	call exec_block1 ------");
#if	TBNFNORMAL
	if(!usebnf) {
#endif
#if	TNORMAL
	tok=bp->tok_table;
	if(execmd) val=exec_block1(bp);
	else val=exec_block1_break(bp);
#endif
#if TBNFNORMAL
	};
#endif
	// MESG("	after exec_block1 !!!!!");

#if	TBNFNORMAL
	if(usebnf) {
#endif
#if	TBNF
		MESG("## execute bnf program block! --------------------");
		exe_buffer=bp;
		tok=bp->tok_table_bnf;
		if(execmd) bnf_block1();
		else bnf_block1_break();
		// next_var("res1");
		// next_var("res2");
		set_result();
		// MESG("end of program1 var@=%d type %d",VARIND,bnf_var->var_type);
		// if(bnf_var->var_type==VTYPE_NUM) MESG("	dval=%f",bnf_var->dval);
		MESG("end of program2 var@=%d type %d",VARIND,bnf_var->var_type);
		// if(bnf_var->var_type==VTYPE_NUM) MESG("	dval=%f",bnf_var->dval);
		// show_results();
#endif
#if	TBNFNORMAL
	};
#endif
	drv_stop_checking_break();

	/* cleaning  */
	// MESG("cleaning:");
	if(start) {
		if(local_symbols){
		// MESG("	cleaning local symbols of [%s]",bp->b_fname);
		if(bp->symbol_tree){
			delete_symbol_table(local_symbols,bp->symbol_tree->items,0);
			bp->symbol_tree=NULL;
		};
		};
		current_stable=old_symbol_table;
	};
#if	TBNFNORMAL
	if(usebnf) {
#endif
#if	TBNF
		show_var_stats();
		// next_var("result");
		// prev_var("r");
		// MESG("show result executing buffer [%s]!",bp->b_fname);
		msg_result(bp->b_fname,show_no_time);
#endif
#if	TBNFNORMAL
	} else
#endif
#if	TNORMAL 
	{
		msg_result(bp->b_fname,show_no_time);
	};
#endif
 } else {
 	msg_line("parse error %d on %s ",err_num,bp->b_fname);
#if	TNORMAL
	val=0;
#endif
 };
 tok=old_tok;
 // MESG("compute_block return %f %f",val,num_result());
#if	TBNFNORMAL
 if(usebnf) return(num_result()); 
 else return(val);
#endif
#if	TBNF
 return(num_result()); 
#else
 return(val);
#endif
}

int empty_tok_table(FILEBUF *fp)
{
 tok_struct *table= fp->tok_table;
 tok_struct *tokdel;
 // MESG("empty_tok_table: of [%s]",fp->b_fname);
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
#if	TBNF
// Initialize var stack
 bnf_var=bnf_vars;
#endif
 return(1);
}

int refresh_current_buffer(num nused)
{
#if	TNORMAL
 // double val=0;
#endif
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
 MESG("refresh_current_buffer: call empty_tok_table: [%s]",fp->b_fname);
 empty_tok_table(fp);

#if	TBNFNORMAL
 if(!usebnf)
#endif
#if	TNORMAL
 clean_saved_string(0);
#endif
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
 	msg_line("evaluating %s",fp->b_fname);
	init_exec_flags();
	exe_buffer=fp;

#if	TBNFNORMAL
	if(usebnf) {
#endif
#if	TBNF
		tok=fp->tok_table_bnf;
		bnf_block1_break();
#endif
#if	TBNFNORMAL
	} else {
#endif
#if	TNORMAL
		tok=fp->tok_table;
		exec_block1_break(fp);
#endif
#if	TBNFNORMAL
	};
#endif

	drv_stop_checking_break();
	if(err_num>0) {
		macro_exec=0;
		show_error("refresh buffer",fp->b_fname);
		// msg_line("Error %d [%s] at line %d",err_num,err_str,err_line);
		// mesg_out("Error %d [%s] at line %d",err_num,err_str,err_line);
	} else {
		MESG("refresh_current_buffer: set result!");
		msg_result("",0);
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
 MESG("parse_check_current_buffer: %d",is_mlang(fp));
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
#if	TBNF
 tok_ind=fp->tok_table_bnf;
#else
 tok_ind=fp->tok_table;
#endif
 // MESG("Print token table to out buffer");
 if(tok_ind==NULL) {
	msg_line("parsing buffer produced no table!");
	return(0);
 };
#if	TBNF
 out_print("|-------- Token list bnf -----------------------------------",1);
#else
 out_print("|-------- Token list ---------------------------------------",1);
#endif
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

#if	TBNF
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
			snprintf(stok,sizeof(stok),"%3d:%4d %3d [%2d=%8s] [%5s] rows=%d cols=%d bnf=%2d fi=%d",
				tok->tnum,tok->tline,tok->tind,tok->ttype,TNAME,(char *)tok->tname,rows,cols,tok->bnf_group,tok->function_index);
		} else 
		if(tok->ttype==TOK_SHOW) { snprintf(stok,sizeof(stok),"%3d:%4d %3d [%2d=%8s] [:] bnf=%2d fi=%d",tok->tnum,tok->tline,tok->tind,tok->ttype,TNAME,tok->bnf_group,tok->function_index);
		} else
		if(tok->ttype==TOK_LCURL||tok->ttype==TOK_RCURL) {
				// snprintf(stok,sizeof(stok),"%3d:%4d CURL",tok->tnum,tok->tline);
				snprintf(stok,sizeof(stok),"%3d:%4d %3d [%2d=%8s] %s other is %d bnf=%2d fi=%d",tok->tnum,tok->tline,tok->tind,tok->ttype,TNAME,(char *)tok->tname,tok->match_tok->tnum,tok->bnf_group,tok->function_index);
		} else	if(tok->tgroup>0) {
			// snprintf(stok,sizeof(stok),"%3d:%4d %s",tok->tnum,tok->tline,tok->tname);
			snprintf(stok,sizeof(stok),"%3d:%4d %3d [%2d=%8s] [%5s] [%2d:%5s]!! bnf=%2d fi=%d",tok->tnum,tok->tline,tok->tind,tok->ttype,TNAME,(char *)tok->tname,tok->tgroup,tname(tok->tgroup),tok->bnf_group,tok->function_index);
		} else	if(tok->ttype==TOK_NUM) { 
			// snprintf(stok,sizeof(stok),"%3d:%4d %s",tok->tnum,tok->tline,tok->tname);
			snprintf(stok,sizeof(stok),"%3d:%4d %3d [%2d=%8s] %5.1f bnf=%2d fi=%d",tok->tnum,tok->tline,tok->tind,tok->ttype,TNAME,tok->dval,tok->bnf_group,tok->function_index);
		} else if(tok->ttype==TOK_QUOTE) {
			// snprintf(stok,sizeof(stok),"%3d:%4d %s",tok->tnum,tok->tline,tok->tname);
			snprintf(stok,sizeof(stok),"%3d:%4d %3d [%2d=%8s] \"%s\" bnf=%2d fi=%d",tok->tnum,tok->tline,tok->tind,tok->ttype,TNAME,(char *)tok->tname,tok->bnf_group,tok->function_index);
		} else if(tok->ttype==TOK_PROC) { 
			if(tok->proc_buffer == NULL) 
				snprintf(stok,sizeof(stok),"%3d:%4d %3d [%2d=%8s] \"%s\" NULL proc bnf=%2d fi=%d",tok->tnum,tok->tline,tok->tind,tok->ttype,TNAME,(char *)tok->tname,tok->bnf_group,tok->function_index);
			else {
				int len=snprintf(stok,sizeof(stok),"%3d:%4d %3d [%2d=%8s] \"%s\" [%20s] proc bnf=%2d fi=%d",tok->tnum,tok->tline,tok->tind,tok->ttype,TNAME,(char *)tok->tname,tok->proc_buffer->b_fname,tok->bnf_group,tok->function_index);
				if(len>=sizeof(stok))  MESG("	truncated");
			};
		} else if(tok->ttype==TOK_VAR) {
			// MESG("TOK_VAR:");
			int vtype=0;
#if	0
			BTNODE *var_node = tok->tok_node;
			char *var_name="unknown";
			if(var_node!=NULL) {
				vtype=var_node->node_vtype;
				var_name=var_node->node_name;
			};
#endif
			// MESG("TOK_VAR: vtype=%d",vtype);
			// MESG("tok_info var! ind=[%d] group=%d vtype=%d",tok->tind,tok->tgroup,vtype);
			MVAR *var=NULL;
			if(current_stable) 
				var = &current_stable[tok->tind];
			if(var!=NULL) vtype=var->var_type;
#if	0
			int size=0;
			if(vtype==VTYPE_TREE) {
				BTREE *type_tree=(BTREE *)var_node->node_dat;
				size = type_tree->items;
			};
			snprintf(stok,sizeof(stok),"%3d:%4d %3d [%2d=%8s] [%5s] %8s %d size %d [bnf=%2d] fi=%d",
				tok->tnum,tok->tline,tok->tind,tok->ttype,TNAME,(char *)tok->tname,vtype_names[vtype] ,vtype,size,tok->bnf_group,tok->function_index);
#else
			snprintf(stok,sizeof(stok),"%3d:%4d %3d [%2d=%8s] [%5s] %8s %d [bnf=%2d] fi=%d",
				tok->tnum,tok->tline,tok->tind,tok->ttype,TNAME,(char *)tok->tname,vtype_names[vtype] ,vtype,tok->bnf_group,tok->function_index);
#endif
		} else {
			// snprintf(stok,sizeof(stok),"%3d:%4d %s",tok->tnum,tok->tline,tok->tname);
			snprintf(stok,sizeof(stok),"%3d:%4d %3d [%2d=%8s] [%5s] bnf=%2d fi=%d",tok->tnum,tok->tline,tok->tind,tok->ttype,TNAME,(char *)tok->tname,tok->bnf_group,tok->function_index);
		};
// 			
	} else {
			return "null tok name !!!!!!!!!!!!!!!!!!!!!!!!!!!";
		     snprintf(stok,sizeof(stok),"%3d:%4d %3d [%2d=%8s] [%5.1f] bnf=%2d fi=%d",tok->tnum,tok->tline,tok->tind,tok->ttype,"null name",tok->dval,tok->bnf_group,tok->function_index);
	};
	// MESG("tok_info: end");
	return stok;
}
#else
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
#endif

int show_parse_buffer(num n)
{
 FILEBUF *fp;
 TDS("show parse");

 char *parsebuf="[lex_out]";
 FILEBUF *outbuf;
 tok_struct *tok_table,*tok_ind;
 fp=cbfp;

 MESG("show_parse_buffer:");
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
 char *s;
#if	TBNFNORMAL
 // MESG("key_str1: var@=%d [%s]",VARIND,tok_info(tok));
 if(usebnf){
#endif
#if	TBNF
	bnf_expression();
	// MESG("	key_str1: var@=%d type=%d [%s]",VARIND,bnf_var->var_type,tok_info(tok));
	if(bnf_var->var_type==VTYPE_STRING) s=bnf_var->sval;
	else s="";
#endif
#if	TBNFNORMAL
	} else {
#endif
#if	TNORMAL
 num_expression();
 s = get_sval();
#endif
#if	TBNFNORMAL
 };
#endif
 // MESG("key_str1: ex_vtype=%d ex_value=%f [%s]",get_vtype(),ex_value,get_sval());
 return (s);
}

void init_exec_flags()
{
 // MESG("init_exec_flags:");
 init_error();
 is_break1=0;
 current_active_flag=1;
 var_node=NULL;
 pnum=0;
 INIT_STAGE;
#if	TNORMAL
 init_ex_var();
 set_vdval(0.0);
 ex_nvars=0;ex_nquote=0;ex_nums=0;	/* initialize table counters  */
#endif
 // MESG("init_exec_flags:ok!");
}



int exec_named_function(char *name)
{
    FILEBUF *bp;		/* ptr to buffer to execute */
    char bufn[MAXFLEN+2];		/* name of buffer to execute */

	// MESG("exec_named_function: %s",name);
	msg_line("exec_name_function: %s",name);
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
	double value;
	// MESG("exec_named_function: active_flag=%d",current_active_flag);

	#if	TBNFNORMAL
		if(!exebnf) 
	#endif
	#if	TNORMAL
		value=exec_function(bp,0);
	#endif
	#if	TBNFNORMAL
		else {
	#endif
	# if TBNF
			bnf_exec_function(bp,0);
			if(bnf_var->var_type==VTYPE_NUM) 
				value=bnf_var->dval;
			else value=0;
	#endif
	#if	TBNFNORMAL
		};
	#endif

#if	TBNF
	MESG("end of exec_named_function var@=%d value=%f",VARIND,value);
#endif

	return((int)value);
}

double expression(char *from)
{
	MESG("expression from: %s",from);
#if	TBNFNORMAL
	if(usebnf) 
#endif
#if	TBNF
		return bnf_expression();
#endif
#if	TBNFNORMAL
	else  
#endif
#if	TNORMAL
		return lexpression();
#endif
}

inline double get_val()
{
#if	TBNFNORMAL
	if(usebnf) 
#endif
#if	TBNF
		return bnf_var->dval;
#endif
#if	TBNFNORMAL
	else
#endif
#if	TNORMAL
	return ex_var.dval;
#endif
}


char *get_sval()
{
#if	TBNFNORMAL
	// MESG("get_sval:");
	if(usebnf) {
#endif
#if	TBNF
	if(bnf_var->var_type==VTYPE_STRING) return bnf_var->sval;
	else if(bnf_var->var_type==VTYPE_NUM) {
		char *s=malloc(16);
		snprintf(saved_string,16,"%f",bnf_var->dval);
		bnf_var->sval=s;
		bnf_var->var_type=VTYPE_STRING;
		bnf_var->var_alloced=1;
		return s;
	} else return "";
#endif
#if	TNORMAL & TBNF
	} else {
#endif
#if	TNORMAL
	if(saved_string==NULL) {
		saved_string=(char *)malloc(16);
		snprintf(saved_string,16,"%f",get_val());
		// MESG("get_sval: new saved_string! [%s]",saved_string);
	};
	return(saved_string);
#if	TNORMAL & TBNF
	};
#endif
#endif
}

inline int get_vtype()
{
#if	TBNFNORMAL
	// MESG("get_vtype:");
	if(usebnf) {
#endif
#if	TBNF
		return bnf_var->var_type;
#endif
#if	TBNFNORMAL
	} else {
#endif
#if	TNORMAL
		return ex_var.var_type;
#endif
#if	TNORMAL & TBNF
	};
#endif
}

inline int vtype_is(int type)
{
	// MESG("vtype_is: type=%d ex type=%d",type,ex_var.var_type);
#if	TBNFNORMAL
	// MESG("vtype_is:");
	if(usebnf) {
#endif
#if	TBNF
		return bnf_var->var_type==type;
#endif
#if	TBNFNORMAL
	} else {
#endif
#if	TNORMAL
		return type==ex_var.var_type;
#endif
#if	TBNFNORMAL
	};
#endif
}

inline void set_vtype(int type)
{
	// MESG("set_vtype: %X",type);
	ex_var.var_type=type;
}

#if	TNORMAL
void set_nsval(char *s,int max)
{
 clean_saved_string(max);
 memcpy(saved_string,s,max);
 saved_string[max]=0;
 // MESG(";set_nsval:s=[%s] [%s] %d",s,saved_string,max);
 // MESG("set_nsval:");
 ex_var.var_type=VTYPE_STRING;
}
#endif

void set_sval(char *s)
{
#if	TBNFNORMAL
	// MESG("set_sval:");
	if(usebnf) {
#endif
#if	TBNF
		if(bnf_var->var_type==VTYPE_STRING && bnf_var->var_alloced)
			free(bnf_var->sval);
		bnf_var->sval=strdup(s);
		bnf_var->var_type=VTYPE_STRING;
		bnf_var->var_alloced=1;
#endif
#if	TBNFNORMAL
	} else {
#endif
#if	TNORMAL
		clean_saved_string(0);
		saved_string=strdup(s);
		ex_var.var_type=VTYPE_STRING;
#endif
#if	TBNFNORMAL
	};
#endif
}

inline void set_vdval(double value)
{
#if	TBNFNORMAL
	// MESG("set_vdval:");
	if(usebnf) {
#endif
#if	TBNF
		bnf_var->dval=value;
		bnf_var->var_type=VTYPE_NUM;
#endif
#if	TBNFNORMAL
	} else {
#endif
#if	TNORMAL
		ex_var.var_type=VTYPE_NUM;
		ex_var.dval=value;
#endif
#if	TBNFNORMAL
	};
#endif

}

inline void set_dval(double value)
{
#if	TBNFNORMAL
	// MESG("set_dval:");
	if(usebnf) {
#endif
#if	TBNF
		bnf_var->dval=value;
#endif
#if	TBNFNORMAL
	} else 	
#endif
#if	TNORMAL
	ex_var.dval=value;
#endif
}

#include "mlang_err.c"

/*	nextarg:	get the next argument from console or from macro */
int nextarg(char *prompt,char *buffer, int size,int show)
/* prompt to use if we must be interactive */
/* buffer to put token into */
/* size of the buffer */
{
	/* if we are interactive, go get it! */
	// MESG("nextarg: macro_exec=%d var@=%d",macro_exec,VARIND);
	if (macro_exec == FALSE) {
		// MESG("getstring: %s",prompt);
		if(getstring(prompt, buffer, size,show)!=FALSE) {
			// MESG("nextarg: buffer=[%s]",buffer);
#if	TBNFNORMAL
			if(exebnf) {
#endif
#if	TBNF
				bnf_var->dval=atof(buffer);
				bnf_var->var_type=VTYPE_NUM;
#endif
#if	TBNFNORMAL
			}
			else
#endif
#if	TNORMAL
			set_dval(atof(buffer));
#endif
		} else {
			set_update(cwp,UPD_MOVE);
			return(FALSE);
		};
	} else {
		// MESG("	get the next argument! exebnf=%d execmd=%d",exebnf,execmd);
		/* slval has already the next argument */
		// MESG("nextarg: slval=%s",get_sval());	
		// MESG("nextarg:2 var_type=%d",bnf_var->var_type);
#if	TBNFNORMAL
		if(usebnf) {
#endif
#if	TBNF
			if(bnf_var->var_type==VTYPE_STRING) strlcpy(buffer,bnf_var->sval,size);
#endif
#if	TBNFNORMAL
		} else
#endif
#if	TNORMAL
		strlcpy(buffer,get_sval(),size);
#endif
	};
	return(TRUE);
}

/*
 TODO  matrix division ??
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
