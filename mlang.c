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

#include	"alist.h"
#include	"mlang.h"
#include	"func.h"
#include	"token_table.h"

#define	SYNTAX_DEBUG	0
#define	NTOKEN2	tok++
#define FACTOR_FUNCTION tok->factor_function()
#define GTYPES 0

void mesg_out(const char *fmt, ...);
extern FILEBUF *cbfp;
extern array_dat *main_args;
FILEBUF *exe_buffer=NULL;

#if	SYNTAX_DEBUG
#define	SHOWTOK {if(discmd) { MESG("  [%2d][%*s-%s][%d]  %s",stage_level,stage_level,"",Tds,tok->tnum,tok_name[tok->ttype]);};}

#define	TDS(name)   char *Tds=name;\
	if(discmd){ stage_level++;\
		MESG("-- level %d type=%d",stage_level,tok->tnum);\
		if(tok!=NULL) {\
			MESG("#+[%2d][%*s-%s][%d]  %s",stage_level,stage_level,"",Tds,tok->tnum,tok_name[tok->ttype]);\
		}\
	} 

#define RTRN(val)	{ stage_level--;return(val);}

#else	/* SYNTAX_DEBUG  */

#define	SHOWTOK ;

#define	TDS(name)   ;

#define RTRN(val)	return(val)

#endif


int err_check_block1();
int check_skip_token1( int type);
void clean_saved_string(int new_size);
int deq(double v1,double v2);
void set_vtype(int type);
int vtype_is(int type);
char *str_mul(char *sval, double v1);

double assign_val(double none);
double assign_env(double none);
void init_error();
void get_lowercase_string(char *lower, char *string);
void get_uppercase_string(char *lower, char *string);
double cexpression();
double exec_block1_break(FILEBUF *fp);
char * tok_info(tok_struct *tok);
void set_vtype(int type);
int vtype_is(int type);
int get_vtype();

TLIST ctoklist=NULL;
int is_break1=0;
int tok_mask[256];
#define	USE_VAR	1

#if	USE_VAR
static tok_data ex_var; 	/* value of previous expression */
#else
int ex_vtype=0; 	/* type of previous expression */
double ex_value;	// saved double value
#endif
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
static tok_struct *tok;	/* current token!!  */

char *err_str;
BTNODE *var_node=NULL;

tok_struct *lstoken=NULL;
tok_data *lsslot=NULL;

MVAR *lmvar=NULL;
int firt_var=1;
tok_data *current_stable=NULL; 	/* current symbol table ...  */
int stage_level=0;	/* stage level  */

int show_stage=0;
int xpos=0;		/* stage position  */
int current_active_flag=1;	/* on to execute procedure!  */

int pnum=0;	/* parenthesis level */


#include "mlang_array.c"

extern char *fvars[];
extern VAR option_names[];

BTREE *bt_table;
BTREE *directiv_table;
BTREE *global_types_tree;

int simple=0;

double num_expression();
double num_term1();
double num_term2();
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
	"UNDEFINED","NUMERIC","ARRAY","SARRAY","LIST",
	"SLIST","ALIST","ASLIST","STRING","BUFFER",
	"ARRAY_EL1","ARRAY_EL2","TREE","TREE_EL",
	"AMIXED","MIXEDEL",NULL
};

/* Function definitions */
#include "mlang_functions.c"


void delete_type_tree(BTREE *type_tree)
{
	free_btnode(type_tree->root);
	free(type_tree);
}

array_dat *transpose(array_dat *array1);

void init_btree_table()
{
	bt_table=new_btree("table",0);
	directiv_table=new_btree("directives",0);
	global_types_tree=new_btree("types",0);
}

void clear_args(MVAR *va,int nargs)
{
 int i;
 // MESG("clear_args: %d",nargs);
 if(va) {
	 for(i=0;i<nargs;i++){
	 	// MESG("	%d %d %X",i,va[i].var_type,va[i].sval);
		if(va[i].var_type==VTYPE_STRING){
			// MESG("clear_args: free %X",va[i].sval);
	 		free(va[i].sval);
		}
	 };
	 free(va);
 };
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

double update_val()
{
 double v0=lsslot->dval;
	// MESG("update_val: from %f by %f",v0,tok->dval);
	// set_vtype(VTYPE_NUM);
	if(vtype_is(VTYPE_NUM)) {
		lsslot->dval += tok->dval;
		NTOKEN2;
		return(v0);
	};
	if(vtype_is(VTYPE_ARRAY)) {
		array_add1(lsslot->adat,tok->dval);
		NTOKEN2;
		return(v0);
	};
	set_error(tok,222,"cannot update non numeric value!");NTOKEN2;return(0);
	NTOKEN2;
	return(v0);
}

double decrease_by()
{
	double v1,v0;
	tok_data *sslot=lsslot;
	TDS("decrease_val");
	// MESG("decrease_by: ");
	v1=lexpression();
	// MESG("	after lexpression: v1=%f",v1);
	if(sslot->vtype==VTYPE_NUM) {
		v0=sslot->dval;
		sslot->dval =v0-v1;
		return(sslot->dval);
	};
	if(sslot->vtype==VTYPE_ARRAY) {
		// MESG("	array decrease_by");
		array_add1(sslot->adat,-v1);
		return(-v1);
	};
	return(-1);
}

double increase_by()
{
	double v1,v0;
	tok_data *sslot=lsslot;
	// MESG("increase_by: slot_index=%d",sslot->ind);
	int ori_type=lstoken->ttype;
	v1=lexpression();

	if(sslot->vtype==VTYPE_NUM) {
		v0=sslot->dval;
		sslot->dval = v0+v1;
		return(sslot->dval);
	};

	if(sslot->vtype==VTYPE_ARRAY) {
		if(ori_type!=TOK_VAR) {
			v0=*sslot->pdval;
			*sslot->pdval = v0+v1;
			return(v0+v1);
		} else {
			array_add1(sslot->adat,v1);
			set_array(sslot->adat);
			return(v1);
		};
	};

	if(sslot->vtype==VTYPE_STRING) {
		// MESG("increase by:");
		sslot->sval=(char *)realloc(sslot->sval,strlen(sslot->sval)+strlen(get_sval())+1);
		strcat(sslot->sval,get_sval());
		set_sval(sslot->sval);
	};
#if	USE_SARRAYS
	if(sslot->vtype==VTYPE_SARRAY) {
		if(vtype_is(VTYPE_STRING)) {
		if(ori_type!=TOK_VAR) {
			// MESG("	increase string val");
			char *stmp=malloc(strlen(get_sval())+strlen(sslot->psval[0]));
			strcpy(stmp,sslot->psval[0]);
			strcat(stmp,get_sval());	/* check!! TODO  */
			free(sslot->psval[0]);
			sslot->psval[0]=stmp;
		} else {
			// MESG("	increase sarray! sslot ind=%d %d %s",sslot->ind,get_vtype(),get_sval());
			sarray_add1(sslot->adat,get_sval());
		};
		};
		return(0);
	};
#endif
	return(-1);
}

void sarray_mul1(array_dat *sarray, double factor);

double mul_by()
{
	double v1,v0;
	// MESG("mul_by:");
	tok_data *sslot=lsslot;
	// TDS("mul_by");
	int ori_type=lstoken->ttype;
	tok_struct *ltok = tok;
	// MESG("mul_by: ori_type=%d vtype=%d",ori_type,sslot->vtype);
	v1=num_expression();

	if(sslot->vtype==VTYPE_NUM) {
		v0=sslot->dval;
		sslot->dval = v0*v1;
		return(sslot->dval);
	};
	if(sslot->vtype==VTYPE_ARRAY) {
		if(ori_type!=TOK_VAR) {
			v0=*sslot->pdval;
			*sslot->pdval = v0*v1;
			return(v0*v1);
		} else {
			array_mul1(sslot->adat,v1);
			set_array(sslot->adat);
			return(v1);
		};
	};
	if(sslot->vtype==VTYPE_STRING) {
		// MESG("increase by:");
		set_sval(str_mul(sslot->sval,v1));
		return(0);
	};
#if	USE_SARRAYS
	if(sslot->vtype==VTYPE_SARRAY) {
		if(vtype_is(VTYPE_NUM) && v1>0) {
		if(ori_type!=TOK_VAR) {
			MESG("	increase string val");
			char *stmp=str_mul(sslot->psval[0],v1);

			free(sslot->psval[0]);
			sslot->psval[0]=stmp;
		} else {
			// MESG("	increase sarray! sslot ind=%d %d %s",sslot->ind,get_vtype(),get_sval());
			sarray_mul1(sslot->adat,v1);
		};
		};
		return(0);
	};
#endif
	if(sslot->vtype==VTYPE_AMIXED) {
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

void initialize_vars()
{
 // MESG("initialize_vars");
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
	// MESG("this is NOT a macro file!!!!!!!!!!!!!!");
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

tok_data *new_symbol_table(int size)
{
 int i;
 // MESG("Initialize new_symbol_table: size %d",size);
 tok_data *td=malloc(sizeof(struct tok_data)*(size+1));
 if(td==NULL) { err_num=101;return NULL;};
 for(i=0;i<size;i++) {
	td[i].ind=i;
 	td[i].vtype=VTYPE_NUM;
	td[i].dval=0;
 };
 return td;
}

tok_data *realloc_symbol_table(tok_data *td,int size,int old_size)
{
 int i;
 // MESG("realloc_symbol_table: size %d",size);
 td=realloc(td,sizeof(struct tok_data)*(size+1));
 if(td==NULL) { err_num=101;return NULL;};
 for(i=old_size;i<size;i++) {
	td[i].ind=i;
 	td[i].vtype=VTYPE_NUM;
	td[i].pdval=NULL;
	td[i].dval=0;
 };
 return td;
}

/* free symbol table after execute */
void delete_symbol_table(tok_data *td, int size,int level,int nargs)
{
 int i;
 // tok_data *sslot;
 // MESG("delete_symbol_table: size=%d level=%d",size,level);
 if(td) {
 for(i=nargs;i<size;i++) {
	tok_data *sslot;
 	sslot=&td[i];

	if(sslot->vtype==VTYPE_STRING) {
		// MESG("delete_symbol_table:%d [%s] free %X",i,sslot->sval,sslot->sval);
		 	if(sslot->sval!=NULL)  free(sslot->sval);
	};
#if	1
	if(sslot->vtype==VTYPE_ARRAY) {
		if(sslot->adat!=NULL) {
			free_array_dat(sslot->adat);
			free(sslot->adat);
		};
	};
#endif
 };
 free(td);
 };
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

#if	USE_VAR
void init_ex_var()
{
	ex_var.vtype=VTYPE_NUM;
	ex_var.pval = &ex_var.dval;
	ex_var.dval=0;
}
#endif

void init_exec_flags()
{
 // MESG("init_exec_flags:");
 init_error();
 is_break1=0;
 current_active_flag=1;
 set_vdval(0.0);
#if	EX_VAR
 init_ex_var();
#endif
 var_node=NULL;
 pnum=0;
 stage_level=0;
 ex_nvars=0;ex_nquote=0;ex_nums=0;	/* initialize table counters  */
 // MESG("init_exec_flags:end");
}

void show_error(char *from,char *name)
{
 int var_index=-1;
 if(tok) if(tok->tok_node) var_index=tok->tok_node->node_index;
 if(var_index>=0)
	ERROR("%s error %d file %s after function [%s] after line %d: [%s]",
		from,err_num,name,ftable[var_index].n_name,err_line,err_str);
 else {
	ERROR("%s error %d file %s line %d: [%s]",from,err_num,name,err_line,err_str);
 };
}

/* Check for any errors and initialize parsed list  */
int check_init(FILEBUF *bf)
{
 tok_struct *tok_table=bf->tok_table;
 int err=0;
 stage_level=0;
 // MESG("check_init: [%s] %d",bf->b_fname,bf->b_type);
 if(tok_table==NULL) 
 {
 	// MESG("create token table [%s]",bf->b_fname);
	parse_block1(bf,NULL,1,0);
	// MESG("block parsed err = %d",err_num);
	if(err_num>0) {
		msg_line("found parsed errors: err_num=%d %s",err_num,err_str);
		return(err_num);
	}
	tok_table=bf->tok_table;
	if(tok_table==NULL)	{
		ERROR("cannot parse file");
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
		MESG("Block checked! err=%d",err);
	};
 };

 tok=tok_table;
 // MESG("check_init:end [%s] %d",bf->b_fname,bf->b_type);
 if(bf->err>0) {
	return bf->err;
 };
 bf->m_mode |= M_CHECKED;
 return(0);
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


MVAR * push_args_1(int nargs)
{
 int i=0;

 double value=0;
 MVAR *va=NULL;

 TDS("push_args");

 err_num=0;
 // MESG("push_args: args=%d",nargs);

 // if(tok->ttype!=TOK_RPAR && nargs!=0)
 if(nargs>0)
 {

 va=(MVAR *) malloc(sizeof(MVAR)*(nargs));

 if(va){
 MVAR *va_i=va;
 for(i=0;i<nargs;i++,va_i++){
	value = num_expression();
	va_i->var_type=get_vtype();
	// MESG("	push_args_1: arg %d, tok=[%d %s] value=%f type=%d",i,tok->tnum,tok->tname,value,va_i->var_type);
	if(vtype_is(VTYPE_NUM)) {
			// MESG("	arg:%d numeric %f",value);
			va_i->dval=value;
	} else
	if(vtype_is(VTYPE_STRING)) {
		// MESG("	arg:%d string [%s]",i,get_sval());
		va_i->sval=strdup(get_sval());
		clean_saved_string(0);
	} else 
	if(vtype_is(VTYPE_ARRAY)||vtype_is(VTYPE_SARRAY)||vtype_is(VTYPE_AMIXED)) {
			va_i->adat=get_array("1");
			// MESG("	arg:%d array type %d",i,va_i->adat->atype);
			// va_i->var_type=va_i->adat->atype;
			va_i->var_type=get_vtype();
			// MESG("	pushed array type=%d stat=%d",va_i->var_type,va_i->adat->astat);
	} else {
			ERROR("error: wrong type arg %d",get_vtype());
			err_num=202;
			clear_args(va,i); return(NULL);
	}
	// MESG("		vtype is %d",va_i->var_type);
	NTOKEN2; // skip separator or right parenthesis!
 };
 }
 };
 return(va);
}


double exec_function(FILEBUF *bp,MVAR *vargs,int nargs)
{
	double value=0;
	static long level=0;
	level++;
	// MESG("exec_function: bp=[%s] nargs=%d level=%d",bp->b_fname,nargs,level);
	tok_data *old_symbol_table=current_stable;
	// MESG("exec_function:2");
	tok=bp->tok_table;	/* start of function  */
	// MESG("exec_function: first token is [%s] type=%d",tok->tname,tok->ttype);
	current_stable=new_symbol_table(bp->symbol_tree->items+nargs);	/* create new symbol table  */

	if(current_stable==NULL) { 
		err_num=208;
		ERROR("cannot create new symbol table! proc level is %ld",level);
		set_break();
		level=0;
		return 0;
	};

	assign_args1(vargs,current_stable,nargs);
	// MESG("exec_function: after assign_args1 [%s] tnum=%d ttype=%d",tok->tname,tok->tnum,tok->ttype);
	value=tok->directive();
	// MESG("exec_function: before delete_symbol_table, ex_value=%f",ex_value);
	/* remove local variable tree and restore the old one  */
	delete_symbol_table(current_stable,bp->symbol_tree->items,level,nargs);
	current_stable=old_symbol_table;

	// MESG("exec_function: before clear_args");
	// clear_args(vargs,nargs);	/* allocated args already cleared above in delete_symbol_table! */
	level--;
	return(value);
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
	MESG("factor_line_array:%d Array definition ------------",adat->anum);
#if	1
	MESG("	tok info: %s array cols=%d rows=%d",tok_info(tok),adat->cols,adat->rows);
#endif
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
	// set_vtype(adat->atype);
	// print_array1("",adat);
	set_array(adat);
	NTOKEN2;
	RTRN(1.2);
}

inline tok_data *get_left_slot(int ind)
{
	// MESG("get_left_slot: ind=%d",ind);
	return &current_stable[ind];
}

double factor_type_element()
{
 double value=0;
 // MESG("factor_type_element:----------");
 array_dat *adat = lsslot->adat;
 MESG("	lsslot %d [%s] array name=[%s] vtype=%d (%s)",lsslot->ind,lstoken->tname,adat->array_name,lsslot->vtype,vtype_names[lsslot->vtype]);
 // MESG(" token value = %f",tok->dval);
 MESG("	type_element [%s]",tok->tname);
 if(adat->var_tree) {
 	int ind;
 	MESG("	var tree name [%s]",adat->var_tree->tree_name);
	if(tok->dval<0) {
		BTNODE *el_node = find_btnode(adat->var_tree,tok->tname);
		if(el_node) {
			ind = (int)(el_node->node_index);
			tok->dval=ind;
		} else {
			MESG("error element not found!");
			set_error(tok,501,"element not found!");
			return(0);
		};
	} else {
		ind=(int)tok->dval;
		// MESG(" ind from cache %d",ind);
	};
	MESG("	ind = %d",ind);
	if(adat->mval[ind].var_type==VTYPE_NUM) {
		set_vtype(VTYPE_NUM);
		value=adat->mval[ind].dval;
		set_dval(value);
		MESG("	return type dval %f",value);
	} else {
		set_vtype(VTYPE_STRING);
		set_sval(adat->mval[ind].sval);
		MESG("	return type sval [%s]",adat->mval[ind].sval);
	};

 } else {
 	set_error(tok,502,"no tree in type element!");
	return(0);
 };
 
 NTOKEN2;
 return value;
}

double factor_variable()
{	
	// MESG("factor_variable:");
 	lsslot = get_left_slot(tok->tind);
	lstoken = tok;

	set_vtype(lsslot->vtype);
	// MESG("	factor_variable:[%s] tind=%d ,var ind=%d ex_vtype=%d ttype=%d vtype=%d",
		// tok->tname,tok->tnum,lsslot->ind,get_vtype(),tok->ttype,lsslot->vtype);
	switch(get_vtype()) {
		case VTYPE_NUM:{
			double val=lsslot->dval; 
			// MESG("		>> val=%f",val);
			NTOKEN2;
			RTRN(val);
			};
		case VTYPE_STRING:
			set_sval(lsslot->sval);
			NTOKEN2;
			RTRN(lsslot->dval);
		case VTYPE_ARRAY:
		case VTYPE_SARRAY:
			// MESG("	factor_variable: array num=%d atype=%d slot_index=%d",lsslot->adat->anum,lsslot->adat->atype,lsslot->ind);
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
			// MESG("factor_variable: [%s] array type=%d",tok->tname,lsslot->adat->atype);
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

#if	1
MVAR *slot_var=NULL;
// void eval_btree(BTNODE *node,void do_func(BTNODE *n));

void type_init_vals(BTNODE *node)
{
	int index=node->node_index;
	slot_var[index].var_type=node->node_vtype;
	slot_var[index].var_index=node->node_index;
	if(node->node_vtype==VTYPE_NUM) {
		slot_var[index].dval=node->node_dval;
		slot_var[index].var_len=0;
		printf("[%10s] ind=%2d type=%d %f\n",node->node_name,index,node->node_vtype,node->node_dval);
	} else {
		slot_var[index].sval=strdup(node->node_sval);
		slot_var[index].var_len=strlen(node->node_sval);
		printf("[%10s] ind=%2d type=%d %s\n",node->node_name,index,node->node_vtype,node->node_sval);
	};	
}
#endif
void eval_btree1(BTNODE *node,void do_func(BTNODE *n,void *p),void *p);

void node_to_mvar(BTNODE *node,void *p)
{
 int index=node->node_index;
 MVAR *mvar_array = (MVAR *)p;
	mvar_array[index].var_type=node->node_vtype;
	mvar_array[index].var_index=node->node_index;
	if(node->node_vtype==VTYPE_NUM) {
		mvar_array[index].dval=node->node_dval;
		mvar_array[index].var_len=0;
		// MESG("![%10s] ind=%2d type=%d %f",node->node_name,index,node->node_vtype,node->node_dval);
	} else {
		mvar_array[index].sval=strdup(node->node_sval);
		mvar_array[index].var_len=strlen(node->node_sval);
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

double factor_assign_type()
{
	double value=0;

	BTREE *var_tree = tok->tok_node->node_dat;

	int size = var_tree->items;
	MVAR *svar = btree_to_mvar(var_tree);
	tok_data *var_slot=get_left_slot(tok->tind);
	MESG("factor_assign_type: -- var_slot in=%d vtype=%d",var_slot->ind,var_slot->vtype);
	MESG(" factor_assign_type: $$$$$ name=[%s] type %d vtype=%d line=%d size=%d",tok->tname,tok->ttype,var_slot->vtype,tok->tline,size);

	array_dat *adat=alloc_array();
	adat->rows=1;
	adat->cols=size;
	adat->atype=VTYPE_AMIXED;
	adat->mval=svar;
	adat->astat=ARRAY_ALLOCATED;
	adat->array_name=strdup(tok->tname);
	adat->var_tree = var_tree;
	// MESG("	>> items %d",size);
	NTOKEN2;
	
	if(check_token(TOK_LPAR)) {
		// MESG("	assign with parenthessis!");
		int i;
		NTOKEN2;
		for(i=0;i<size;i++) {
			value=lexpression();
			if(ex_var.vtype!=svar[i].var_type) {
				set_error(tok,1022,"type mismatch!");
				return(0);
			};
			if(ex_var.vtype==VTYPE_STRING) {
				// MESG("%2d: type=%d val=[%s]",i,ex_var.vtype,get_sval());
				svar[i].sval=strdup(get_sval());
			} else {
				// MESG("%2d: type=%d val=%f %f",i,ex_var.vtype,ex_var.dval,value);
				svar[i].dval=value;
			};
			// MESG("next toke type is %s",tok_info(tok));
			NTOKEN2;
		};
	};
	set_array(adat);
	
	return 1;
}

double factor_array2()
{
	int ind1=0;
	int ind2=0;
	double value=0;
	tok_data *array_slot;
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
				array_slot->pdval=&dval2[ind1][ind2];
				return(value);
			} else if(adat->atype==VTYPE_SARRAY) {
				char **sval = adat->sval;
				int ind=adat->cols*ind1+ind2;
				clean_saved_string(strlen(adat->sval[ind]));	/* Check!! TODO  */
				strcpy(saved_string,adat->sval[ind]);
				// MESG("	show string value![%s]",saved_string);
				array_slot->psval=&sval[ind];
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
					array_slot->pdval=&array_slot->adat->mval[ind].dval;
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
	tok_data *array_slot;
	array_slot=&current_stable[tok->tind];
	array_dat *adat = array_slot->adat;
	lstoken=tok;
	// MESG("factor_array1:----------- vtype=%d",array_slot->vtype);
	NTOKEN2;
	ind1 = (int)num_expression();

	if(adat==NULL) {	/* this must not happen!!!  */
		// MESG("array adat is NULL allocate new one !!!!!!!!!!!!");
		ex_nums=1;
		adat=new_array(ind1+1,1);
		array_slot->adat=adat;
		array_slot->vtype=VTYPE_ARRAY;
		allocate_array(array_slot->adat);	/*   */
	};

	if(tok->ttype==TOK_RBRAKET) { 
		NTOKEN2;
		// MESG("ends with rbracket!!");
	};
	// MESG("factor_array1:ind=%d ind1=%d type=%d",array_slot->ind,ind1,array_slot->vtype);

		// MESG("	2 vtype=%d %d",array_slot->vtype,VTYPE_ARRAY);
		if(array_slot->vtype==VTYPE_ARRAY)
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
		};
		if(array_slot->vtype==VTYPE_AMIXED) {
			set_vtype(array_slot->adat->mval[ind1].var_type);
			lmvar = &array_slot->adat->mval[ind1];
			// MESG("get indexed value from mixed array ind1=%d type=%d",ind1,get_vtype());
			if(vtype_is(VTYPE_NUM)) {
				value=array_slot->adat->mval[ind1].dval;
				// MESG("	value=%f",value);
				array_slot->pdval=&array_slot->adat->mval[ind1].dval;
			} else {
				value=0;
				set_sval(array_slot->adat->mval[ind1].sval);

				// MESG("	show string value![%s]",saved_string);
				// array_slot->psval = &sval[ind1];
				array_slot->psval=&array_slot->adat->mval[ind1].sval;
			};
			
		}
		if(array_slot->vtype==VTYPE_SARRAY) {
			char **sval = array_slot->adat->sval;
			clean_saved_string(strlen(array_slot->adat->sval[ind1]));	/* Check!! TODO  */
			strcpy(saved_string,array_slot->adat->sval[ind1]);
			// MESG("	show string value![%s]",saved_string);
			// array_slot->psval = &sval[ind1];
			array_slot->psval=&sval[ind1];
			value=0;
			set_vtype(VTYPE_STRING);
		};

	if(array_slot->vtype==VTYPE_ARRAY) {
		dval = array_slot->adat->dval;

		value=dval[ind1];
		// MESG("	value [%d]=%f",ind1,value);
		array_slot->pdval=&dval[ind1];
		set_vtype(VTYPE_NUM);
	};
	lsslot=array_slot;
	// MESG("        : >>>> end");
	// MESG("	factor_array1:ind1=%d lsslot ind=%d type=%d rows=%d cols=%d [%s]!",ind1,lsslot->ind,lsslot->vtype,lsslot->adat->rows,lsslot->adat->cols,array_slot->psval[0]);
	return(value);
}

double factor_cmd()
{ // 3 editor command
	int var_index;
	int check_par=0;
	int save_macro_exec;
	double status=1;
	double value=1;
	FUNCS *ed_command;

	// set_vtype(VTYPE_NUM);
	// MESG(";factor_cmd: ttype=%d command=%d",tok->ttype,tok->tok_node->node_index);
	var_index = tok->tok_node->node_index;
	ed_command = ftable+var_index;

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
				// MESG(";	ed_command:arg2 value=%f ex_var.vtype=%d s=[%s]",value,ex_var.vtype,get_sval());
#endif
				// MESG(";ed_command: second token! type=%d ",tok->ttype);
				break;
			};
		};

	};

	macro_exec = MACRO_MODE2;

	err_num=0;
	err_line=tok->tline;
	err_str=NULL;
	// tok_struct *tok1=tok;
	// MESG(";factor_cmd: execute function! current token is [%s] tnum=%d value=%d",tok->tname,tok->tnum,(int)value);
	status=ed_command->n_func((int)value);
	// MESG("toknum1 = %d",tok1->tnum);
	// tok=tok1;
	// MESG(";TOC_CMD: tnum=%d status=%d check_par=%d",tok->tnum,status,check_par);
	set_vdval(status);

	macro_exec = save_macro_exec;

	if(check_par) { 
		if(check_rparenthesis()) {
			NTOKEN2;
			// MESG("right parenthesis skipped!");
		};
	};

	if(err_num>0) {
		// ERROR("error %d after function [%s] at line %d: %s",err_num,ftable[var_index].n_name,err_line,err_str);
		show_error("Factor","factor_cmd");
		RTRN(status);
	};
	// MESG(";factor_cmd:end tnum=%d value=%f ex_value=%f",tok->tnum,value,ex_value);
	RTRN(status);
}

double factor_env()
{
	BTNODE *bte;
	double value=0;
	bte=tok->tok_node;
	var_node=bte;
	value = get_env(bte->node_index);
	NTOKEN2;
	RTRN(value);
}

/* user interupt on, set also break flag  */
void set_break()
{
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
 value = lexpression();
 NTOKEN2;	/* skip corresponding right parenthesis!  */
 return value;
}

static inline double factor_num()
{
	double val=tok->dval;
	// MESG("factor_num:");
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
	tok_struct *after_proc;
	tok_struct *tok0=tok;
	double value;
	// MESG("factor_proc:----------------");
	FILEBUF *cbuf=exe_buffer;
	exe_buffer=tok0->tbuf;
	// MESG("factor_proc: cbuf=%s ----------------",cbuf);
	// MESG("factor_proc: filebuf=%s",exe_buffer->b_fname);
	NTOKEN2;
	/* function */
	MVAR *vargs = NULL;
	// MESG("factor_proc: tok0 [%d %s] args=%d",tok0->tnum,tok0->tname,tok0->tind);
	// MESG("factor_proc: tok  [%d %s] %d ",tok->tnum,tok->tname,tok->tind);
	vargs = push_args_1(tok0->t_nargs);
	after_proc=tok;
	// MESG("factor_proc: tok after push [%d %s]",tok->tnum,tok->tname);
	value=exec_function(exe_buffer,vargs,tok0->t_nargs);
	// MESG("factor_proc: return val=%f",value);
	tok=after_proc;
	current_active_flag=1;	/* start checking again  */
	free(vargs);
	exe_buffer=cbuf;
	RTRN(value);
}

double cexpr_notequal(double v1,double v2)
{
 return v1!=v2 ? 1.0:0.0;
}

double cexpr_smaller(double v1,double v2)
{
 return v1<v2 ? 1.0: 0.0;
}

double cexpr_bigger(double v1,double v2)
{
 return v1>v2 ? 1.0: 0.0;
}

double cexpr_equal(double v1,double v2)
{
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

static double term1_mul(double v1)
{
 double v2;
 TDS("term1_mul");
 // MESG("term1_mul: ex_vtype=%d",ex_vtype);
	if(vtype_is(VTYPE_NUM)){
		NTOKEN2;
		v2=num_term2();
		// MESG("term1_mul: second type=%d",ex_vtype);
		switch(get_vtype())
		{
			case VTYPE_NUM:	// numeric * numeric
				v1=v1*v2;
				return v1;

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
		MESG(";; sval %s",sval);
		if(vtype_is(VTYPE_NUM)) {
			char *news=str_mul(sval,v2);
			MESG(";; news=%s",news);
			set_sval(news);
			MESG("ok!");
			return v2;
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

static double term1_div(double v1)
{
 double v2;
	if(vtype_is(VTYPE_NUM)){
		NTOKEN2;
		v2=num_term2();
			switch(get_vtype())
			{
				case VTYPE_NUM:	// numeric * numeric
					if(v2!=0){
					v1=v1/v2;
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
	return -1;
}

double factor_sep(){
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
	factor_none,	// TOK_SHOW
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
	factor_none,	// TOK_ASSIGN	,	// assignment
	factor_eof,		// TOK_EOF		,	// end of file token
	factor_num,		// TOK_NUM, numeric value

	factor_none,	// TOK_DIR		,	// directive
	factor_none,	// TOK_DIR_IF	,	// dir if
	factor_none,	// TOK_DIR_ELSE	,	// dir else
	factor_none,	// TOK_DIR_BREAK	,
	factor_none,	// TOK_DIR_RETURN	,
	factor_none,	// TOK_DIR_WHILE	,
	factor_none,	// TOK_DIR_FOR		,
	factor_none,	// TOK_COMMA		,
	factor_none,	// TOK_DIR_FORI	,

	/* bool operators  */
	factor_none,	// TOK_COMPARE		,
	cexpr_notequal,	// TOK_NOTEQUAL	,
	cexpr_smaller,	// TOK_SMALLER		,	/* <  */
	cexpr_bigger,	// TOK_BIGGER		,	/* >  */
	cexpr_equal,	// TOK_EQUAL		,	/* ==  */
	cexpr_smallereq,	// TOK_SMALLEREQ	,	/* <=  */
	cexpr_biggereq,	// TOK_BIGGEREQ	,	/* >=  */

	factor_none,	// TOK_BOOL		,
	factor_none,	// TOK_AND			,	/* &  */
	factor_none,	// TOK_OR			,	/* |  */
	factor_not,		// TOK_NOT	/* !  */
	factor_none,	// TOK_NAND		,	/* !&  */
	factor_none,	// TOK_NOR			,	/* !|  */
	factor_none,	// TOK_XOR			,	/* ^  */
	/* term operators  */
	factor_plus,	// TOK_PLUS		,
	factor_minus,	// TOK_MINUS		,
	term2_power,	// TOK_POWER		,	/* ** */
	term2_modulo,	// TOK_MOD			,	/* %  */
	term1_mul,	// TOK_MUL			,
	term1_div,	// TOK_DIV			,

	factor_line_array,	// TOK_LBRAKET		,
	factor_error,	// TOK_RBRAKET		,
	factor_none,	// TOK_SQUOTE		,
	factor_at,		// TOK_AT			,
	factor_none,	// TOK_RANGE		,
	factor_none,	// TOK_BQUOTE
	factor_none,	// TOK_DOLAR		,
	factor_none,	// TOK_TILDA		,
	update_val,	// TOK_INCREASE	,
	update_val,	// TOK_DECREASE	,
	factor_none,	// TOK_INCREASEBY
	mul_by,	// TOK_MULBY
	decrease_by,	// TOK_DECREASEBY
	factor_none,	// TOK_BSLASH		,

	factor_none,	// TOK_NL				,
	factor_none,	// TOK_DIR_CONTINUE	,
	factor_none,	// TOK_DIR_FOREACH		,
	factor_none,	// TOK_DIR_TYPE,
	factor_array1,	// TOK_ARRAY1
	factor_array2,	// TOK_ARRAY2
	factor_none,	// TOK_ARRAY3
	factor_none,	// TOK_ASSIGNENV	,
	factor_none,	// TOK_ASSIGNOPT	,
	factor_none,	// TOK_END,
	factor_none,	// TOK_DEFINE_TYPE,
	factor_assign_type,	// TOK_ASSIGN_TYPE,
	factor_type_element,	// TOK_TYPE_ELEMENT
	factor_none,	// TOK_DOT,
	factor_none		// TOK_OTHER,
};

void set_tok_function(tok_struct *tok, int type)
{
	// MESG("set_tok_function: type=%d ttype=%d",type,tok->ttype);
	switch(type) {
		case 0:
			if(tok->ttype==TOK_FUNC) {
				int findex = tok->tok_node->node_index;
				// MESG(" F tok %2d: %s type [%d %s] set factor function %d",tok->tnum,tok->tname,tok->ttype,tok_name[tok->ttype],findex);
				tok->factor_function = m_functions[findex].ffunction;
			} else {
	 			tok->factor_function = factor_funcs[tok->ttype];
				// MESG(" f tok %2d: %s type [%d %s] set factor function",tok->tnum,tok->tname,tok->ttype,tok_name[tok->ttype]);
			};
			break;
		case 1:
			tok->cexpr_function = factor_funcs[tok->ttype];
			// MESG(" c tok %2d: %s type [%d %s] set cepr function",tok->tnum,tok->tname,tok->ttype,tok_name[tok->ttype]);

	};
}

void set_tok_directive(tok_struct *tok, FFunction directive)
{
	tok->directive = directive;
	// MESG(" d tok %2d: %s set directive function",tok->tnum,tok->tname);
}

void set_term_function(tok_struct *tok, TFunction term_function)
{
	tok->term_function = term_function;
	// MESG(" t tok %2d: %s set term function",tok->tnum,tok->tname);
}

// Directove functions

static double inline dir_lcurl()
{
	NTOKEN2;
	return exec_block1(exe_buffer);
}

static double inline dir_lcurl_break()
{
	NTOKEN2;
	return exec_block1_break(exe_buffer);
}

static double inline dir_break()
{
	NTOKEN2;
	current_active_flag=0;
	return 0;
}

static double inline dir_return()
{	double val;
	NTOKEN2;
	if(tok->ttype==TOK_SEP || tok->ttype==TOK_RPAR) { val=0;set_vtype(VTYPE_NUM);}
	else val=lexpression();
	current_active_flag=0;	/* skip rest of function  */
	return(val);
}

double term_plus(double value)
{
 double d1;
 if(vtype_is(VTYPE_NUM)) {
	NTOKEN2;
	d1=num_term1();
	// MESG("term_plus");
	if(vtype_is(VTYPE_NUM)) return value+d1;
	if(vtype_is(VTYPE_STRING)) {
		char svalue[MAXLLEN];
		int stat;
		stat=snprintf(svalue,MAXLLEN,"%f%s",value,get_sval());
		if(stat>MAXLLEN) MESG("truncated 2");
		set_sval(svalue);
		return 0;
	};
	if(vtype_is(VTYPE_ARRAY)) { // num + array
		array_dat *new_array = get_array("18");
		if(new_array->astat==ARRAY_LOCAL) {
			set_array(dup_array_add1(new_array,value));
			ex_name="New array,add to numeric";
		} else {
			array_add1(new_array,value);
			ex_name=tok->tname;
		};
		set_vtype(VTYPE_ARRAY);
		ex_name="Add to numeric";
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
			if(vtype_is(VTYPE_STRING)) {	/* string catanate  */
				strlcat(svalue,get_sval(),sizeof(svalue));
				value=atof(svalue);
				set_sval(svalue);
				return value;
			};
			if(vtype_is(VTYPE_NUM))	{	/* string, numeric catanate  */
				long l0;
				set_vtype(VTYPE_STRING);
				l0=d1;
				clean_saved_string(80);
				if(l0==d1)snprintf(saved_string,80,"%ld",l0);
				else snprintf(saved_string,80,"%f",d1);
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
					if(loc_array->astat==ARRAY_LOCAL) {
						loc_array=dup_array_add1(loc_array,d1);
					} else {
						array_add1(loc_array,d1);
					};
					set_vtype(VTYPE_ARRAY);
					set_array(loc_array);
					ex_name="Add to numeric";
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
	// ERROR("term_plus: operation not supported!, vtype=%d line %d",ex_var.vtype,tok->tline);
	NTOKEN2;
	set_error(tok,221,"operation not supported");
 return 0;
}

double term_minus(double value)
{
 double d1;
 if(vtype_is(VTYPE_NUM)) {	/* numeric  !!  */
	NTOKEN2;
	d1=num_term1();

	if(vtype_is(VTYPE_NUM)) return value-d1;
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
double num_term2()
{
 TDS("num_term2");
 // MESG("		num_term2: [%s]",tok_info(tok));
 double v1 = FACTOR_FUNCTION;
 // MESG("		factor function executed! v1=%f",v1);
	 while(tok->tgroup==TOK_TERM2)
	 {
		// MESG("while: TERM2");
		v1 = tok->term_function(v1);
	 };
 // MESG("		term2 end %f",v1);
 RTRN(v1);
}

/* multiply & divide */
double num_term1()
{
 TDS("num_term1");
 // MESG("	num_term1: [%s]",tok_info(tok));
 double v1 = num_term2();
	 while(tok->tgroup==TOK_TERM1)
	 {
		// MESG("while: TERM1");
		v1 = tok->term_function(v1);
		if(err_num) break;
	 };
	// MESG("	num_term1: end %f",v1);
 RTRN(v1);
}

/* numerical only expression  */
double num_expression()
{
 double value;
 TDS("num_expression");
 // MESG(";num_expression: [%s]",tok_info(tok));
 // set_vdval(0);
 value = num_term1();
 while(tok->tgroup==TOK_TERM) {
	// MESG("while: TERM");
	value = tok->term_function(value);
 };
 // MESG(";num_expression:end ttnum=%d type=%d return value=%f slval=[%s]",tok->tnum,get_vtype(),value,get_sval());
 return value;
}

double logical_or(double value)
{
 int v2;
	v2=(int) lexpression();
//	MESG("logical_or: v2=%d || value=%f",v2,value);
	return v2 || value;
}

double logical_xor(double value)
{
 int v2;
	v2=(int) lexpression();
//	MESG("%d XOR %d = %d",value,v2,value^v2);
	// set_vtype(VTYPE_NUM);
	return ((int)value)^v2;
}

double logical_nor(double value)
{
 int v2=(int) lexpression() >0;;
	// set_vtype(VTYPE_NUM);
	return (!((value>0) || v2));
}

double logical_and(double value)
{
 int v2=(int) lexpression() >0;
	// set_vtype(VTYPE_NUM);
	return v2 && (int)value;
}

double logical_nand(double value)
{
 int v2;
	v2=(int) lexpression();
	// set_vtype(VTYPE_NUM);
	return !(v2 && (int)value);
}

double assign_option(double none)
{
double	value=lexpression();
#if	1
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
		
#else
		if(var_node->sval!=NULL) {free(var_node->sval);};
		if(vtype_is(VTYPE_STRING)){
			if(get_sval()) var_node->sval=strdup(get_sval());
			else {
				err_num=2221;
				ERROR("error: Null string ",err_num);
				var_node->sval=NULL;
			};
		} else {
			if(get_sval()) 
			{
				err_num=2222;
				ERROR("error: no string and not free!");
			};
			var_node->sval=NULL;
		}
		var_node->node_val=value;
#endif
	RTRN(value);
}

double lexpression()
{
 double value;
 TDS("lexpression");
 // MESG(";lexpression: [%s]",tok_info(tok));
 value = cexpression();
 // MESG("lexpression : [%s] cexpression result = %f",tok_info(tok),value);
 if(tok->tgroup == TOK_TERM0){
	tok_struct *tok0=tok;
 	NTOKEN2;
	RTRN(tok0->term_function(value));
 };
 while(tok->tgroup==TOK_BOOL){
	// MESG("while: BOOL");
	tok_struct *tok0=tok;
//	MESG("lexpression in loop!: [%s] value=%f",tok_info(tok),value);
 	NTOKEN2;
	value =tok0->term_function(value);
//	MESG("lexression result is %f",value);
	// set_vtype(VTYPE_NUM);
	set_vdval(value);
 };
 
	// MESG("lexpression return value %f [%s]",value,tok_info(tok));	
	RTRN(value);
}

double cexpression()
{
 double value;
 tok_struct *tok0;
 TDS("cexpression");
 // MESG(";cexpression [%s]",tok_info(tok));
 value = num_expression();

 if(tok->tgroup!=TOK_COMPARE) RTRN(value);
 tok0=tok;
 NTOKEN2;
 if(vtype_is(VTYPE_STRING)) {
	static char svalue[MAXLLEN];
	 
	strlcpy(svalue,get_sval(),MAXLLEN);
	num_expression();
	if(!vtype_is(VTYPE_STRING)) {
		syntax_error("string comparison error",223);
		set_vtype(VTYPE_NUM);
		RTRN(0);	/* it is an error to compare string with number  */
	};
	set_vtype(VTYPE_NUM);
	int lresult=scmp(svalue,get_sval());

	// clean_saved_string(0);
	RTRN(tok0->cexpr_function(lresult,0));
 } else {
	double v2;
	v2=num_expression();
	RTRN(tok0->cexpr_function(value,v2));
 }
}

double expression(char *title)
{
	return num_expression();
}

double assign_env(double none)
{
	double v1;
	int left_index;
	TDS("assign_env");
	left_index=var_node->node_index;
	v1=lexpression();
	set_env(left_index,get_sval(),v1);
	var_node=NULL;
	return(v1);
}

double assign_val(double none)
{
	double v1;
	TDS("assign_val");
	// MESG("assign_val: ind=%d [%s] vtype=%d",lsslot->ind,lstoken->tname,lsslot->vtype);
	MESG("assign_val: [%s] = [%s] ",lstoken->tname,tok->tname);
	tok_struct *lstok=lstoken;
	tok_data *sslot=lsslot;
	v1=lexpression();
	MESG("assign_val: after lexpression! slot vtype=%d ex_vtype=%d\n",sslot->vtype,get_vtype());
	if(vtype_is(sslot->vtype) && vtype_is(VTYPE_NUM)) {
			sslot->dval=v1;
			return(v1);
	};
	MESG("assign_val: check diffs!");
	if(!vtype_is(sslot->vtype)){
		MESG("assign_val: different vtype %d != sslot_vtype %d",get_vtype(),sslot->vtype);
		if(vtype_is(VTYPE_AMIXED)) {
			MESG("	mixed array!!!!");
			array_dat *adat = get_array("type");
			if(adat) {
				MESG("		named [%s]",adat->array_name);
				if(adat->var_tree) {
					MESG("	var_tree: %s",adat->var_tree->tree_name);
					if(tok->ttype==TOK_TYPE_ELEMENT) {
						MESG("  next token is %d",tok->ttype);
						// lsslot=sslot;
						// lstoken=lstok;
						v1=factor_type_element();
						lsslot->vtype=get_vtype();
						if(vtype_is(VTYPE_NUM)) lsslot->dval=v1;
						else lsslot->sval=strdup(get_sval());
						return v1;
					};
				};
			};
		};
		if(sslot->vtype==VTYPE_STRING) {
			if(sslot->sval) free(sslot->sval);
			sslot->sval=NULL;
			if(vtype_is(VTYPE_NUM)) {
				// MESG("set new as num");
				sslot->dval=v1;
				sslot->vtype=VTYPE_NUM;
				return(v1);
			};
		} else {
			if(sslot->vtype!=VTYPE_ARRAY && sslot->vtype!=VTYPE_SARRAY)	{/* added to handle arrays (v698l) but CHECK!!!!  */
				if(sslot->vtype==VTYPE_AMIXED) {
					MESG("	AMIXED we are here! %f",*sslot->pdval);
					// MESG("-- t=%d",lmvar->vtype);

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
				// sslot->vtype=get_vtype();
			};
			if(vtype_is(VTYPE_NUM)) {
				// MESG("set new as num");
				*sslot->pdval=v1;
				// sslot->vtype=VTYPE_NUM;
				return(v1);
			};
			if(vtype_is(VTYPE_STRING)) {
				sslot->sval=strdup(get_sval());
				/* !!!!! to change type beware!!!!!!  */
				if(sslot->vtype!=VTYPE_SARRAY)
					sslot->vtype=VTYPE_STRING;
				// MESG("set string!");
				return(0);
			};
		};
			if(vtype_is(VTYPE_ARRAY) || vtype_is(VTYPE_SARRAY)||vtype_is(VTYPE_AMIXED)) {
				// MESG("assign array to var!");
				sslot->adat=get_array("27");
				sslot->vtype=sslot->adat->atype;
				sslot->adat->array_name=strdup(lstok->tname);
				if(sslot->adat->astat==ARRAY_ALLOCATED) sslot->adat->astat=ARRAY_LOCAL;
			};
		return(v1);		
	} else {
		if(vtype_is(VTYPE_ARRAY) || vtype_is(VTYPE_SARRAY)) {
//				print_array1("assign array ",get_array("31"));
//				print_array1("to array",sslot->adat);
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

int assign_args1(MVAR *va,tok_data *symbols,int nargs)
{
 TDS("assign_args1");
 // MESG("\n# assign_args1: tok=[%d %s] %d nargs=%d",tok->tnum,tok->tname,tok->ttype,nargs);
 NTOKEN2; /* skip name */
 if(va) 
 {
	int i;
	// MESG("assign_args1: pos1 tok=[%d %s] %d",tok->tnum,tok->tname,tok->ttype);
	for(i=0;i<nargs;i++,va++) {
		tok_data *arg_dat=&symbols[tok->tind];
		arg_dat->vtype=va->var_type;
		// MESG("assign_args1:arg %d: pos2 tok=[%d %s] ttype=%d tind=%d",i,tok->tnum,tok->tname,tok->ttype,tok->tind);

		switch(va->var_type) {
			case VTYPE_NUM:
				arg_dat->dval=va->dval;
				// MESG("		nuneric arg: dval=%f",va->dval);
				break;
			case VTYPE_STRING:
				// MESG("		string arg: %d [%s] %X",i,va->sval,va->sval);
				arg_dat->sval=va->sval;break;
			case VTYPE_ARRAY:
			case VTYPE_SARRAY:
			case VTYPE_AMIXED:
				arg_dat->adat=va->adat;break;
			default:
				ERROR("		array argn_args:[%d] type is wrong! (%d)",i,va->var_type);
				arg_dat->sval="";
				arg_dat->dval=0;
		};

		// MESG("assign_args1:arg %d: pos3 after args tok=[%s] %d",i,tok->tname,tok->ttype);
		NTOKEN2;	/* skip separator or end parenthesis */
		if(tok->ttype==TOK_RPAR) break;
		// MESG("		ntoken");
		if(nargs>0) NTOKEN2;
	};
 } else { // we send no arguments!
	// skip till end parenthesis setting default values for arguments!!??
	while(tok->ttype!=TOK_RPAR && tok->ttype!=TOK_END) NTOKEN2;
 };
 NTOKEN2;
 // MESG("assign_args1: end! pos5 after args tok=[%s] %d",tok->tname,tok->ttype);
 return(1);
}

#include "mlang_parser.c"

// skip next sentence in a list
void skip_sentence1()
{
 int plevel=0;
 TDS("skip_sentence1");
 // MESG("skip_sentence: ttype=%d",tok->ttype);
 if(tok->ttype==TOK_LCURL) {
		tok=tok->match_tok; 
		NTOKEN2;
		return; 
 };

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
 FILEBUF *old_fp;
 TextPoint *tp = tok->ddot;
 FILEBUF *buf = tp->fp;
 int dsize=0;
 int is_ddot=0;
 offs ddot_pos=tp_offset(tp);
 offs i,sl,el; // current offset, start,end of line
 old_fp=cbfp;
 
 sfb(buf);
 el=FLineEnd(buf,ddot_pos);
 sl=FLineBegin(buf,ddot_pos);

 dsize=el-sl;
 ddot_pos=el-sl;
	for(i=sl;i<el;i++) {
		if(CharAt(i)==':') { 
			is_ddot=!(cbfp->b_state & FS_VIEW);
			ddot_pos=i-sl+1;break;};
	};
  
// replace text
 textpoint_set(buf->tp_current,sl+ddot_pos);
 if(is_ddot) DeleteBlock(0,dsize-ddot_pos);

 insert_string(cbfp,ddot_out,strlen(ddot_out));
 if(err_num>0) {
 	insert_string(cbfp," ,err ",6);
	if(err_str!=NULL) insert_string(cbfp,err_str,strlen(err_str));
 };
 free(ddot_out);
 sfb(old_fp);
}

void refresh_ddot_1(double value)
{
 TDS("refresh_ddot_1");
 int stat=0;
 TextPoint *tp = tok->ddot;
 FILEBUF *buf = tp->fp;
 // MESG("refresh_ddot:");
 if(execmd) {
	 if(vtype_is(VTYPE_NUM)) {
		if(lstoken) {
			printf(";%s	: %.3f\n",lstoken->tname,value);
		} else printf(";	: %.3f\n",value);
	 } else if(vtype_is(VTYPE_STRING)) {
	 	if(lstoken) {
			printf(";%s	: %s\n",lstoken->tname,get_sval());
		} else printf(";	: '%s'\n",get_sval());
	 } else if(vtype_is(VTYPE_ARRAY)||vtype_is(VTYPE_SARRAY)||vtype_is(VTYPE_AMIXED)) print_array1(";",get_array("36"));
	 lstoken=NULL;
	 return;
 };

 int precision=bt_dval("print_precision");
 int show_hex=bt_dval("show_hex");
 char *ddot_out = (char *)malloc(128);

 // MESG("	ddot_pos=%d end=%d todel=%d",ddot_position,line_end,line_end-ddot_position);
 if(buf->b_state & FS_VIEW) return; // no refresh in view mode

 if(vtype_is(VTYPE_STRING)) {	/* string value  */
	stat=snprintf(ddot_out,128," \"%s\"",get_sval());
 }  else if(vtype_is(VTYPE_NUM)) {	/* numeric value  */
	long int d = (long int)value;
	if(d==value) {	/* an integer/double value!  */
		if(show_hex) stat=snprintf(ddot_out,128," %5.0f | 0x%llX | 0o%llo",value,(unsigned long long)value,(unsigned long long)value);
		else stat=snprintf(ddot_out,128," %5.*f",1,value);
	} else {	/* a decimal value!  */
		stat=snprintf(ddot_out,128," %5.*f",precision,value);
	};

 } else if(vtype_is(VTYPE_ARRAY) || vtype_is(VTYPE_SARRAY) || vtype_is(VTYPE_AMIXED)) {
	array_dat *adat = get_array("37");
	// MESG("refresh_ddot: array: type=%d name=(%s)",adat->atype,adat->array_name);

 	stat=snprintf(ddot_out,128,"array %d:[%s] type [%s] , slot %ld type=%d rows %d,cols %d",adat->anum,
	adat->array_name,vtype_names[adat->atype],lsslot-current_stable,adat->atype,adat->rows,adat->cols);
	print_array1(":",adat);
 };
 if(stat>MAXLLEN) MESG("truncated");

 update_ddot_line(ddot_out);
}

double tok_dir_type()
{
	// MESG("dir_type");
	NTOKEN2;
	return 0;
}

double tok_dir_if()
{
 double val=0;
	tok_struct *tok0=tok;
	// MESG("tok_dir_if: n=%d",tok->tnum);
	NTOKEN2;	/* go to next token after if */

	val=lexpression();
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
	if(is_break1) { tok=cbfp->end_token;return(0);};
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
	tok_data *index=NULL;
	double dinit,dmax,dstep;
	double *iterrator_val;

	NTOKEN2;	/* go to next token after for */

	index=&current_stable[tok->tind];
	if(index->vtype!=VTYPE_NUM) {err_num=224;ERROR("for i syntax error %d",err_num);};
	set_vtype(index->vtype);

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
	if(tok->ttype!=TOK_RPAR) { err_num=226;ERROR("for i: error %d",err_num);};
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
				if(is_break1) { tok=cbfp->end_token;return(0);};
				break;
			};
		};
	} else if(dstep<0 && dmax< *iterrator_val) {
		for(; *iterrator_val > dmax; *iterrator_val +=dstep) {
			tok=start_block;
			tok->directive();
			if(current_active_flag==0) {
				if(is_break1) { tok=cbfp->end_token;return(0);};
				break;
			};
		};
	} else {
		err_num=226;
		ERROR("error: infinite fori loop %d",err_num);
		tok=end_block;
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
	do {
		// set tlist to tok pointer
		tok=check_element;

		if(lexpression()) {
			// on the block start
			tok=start_block;
			tok->directive();
			if(current_active_flag==0) {	/* only after break  */
				if(is_break1) { tok=cbfp->end_token;return(0);};
				break;
			};
		} else {
			break;
		};

	} while(1); 
	tok=end_block;	/* to the end of executable block  */
	current_active_flag=old_active_flag;
	return 1;
}

/* exec multiple sentences at the same level */
double exec_block1(FILEBUF *fp)
{
 double val=0;
 stage_level=0;
 TDS("exec_block1");
 // MESG("exec_block1: starting at tok %d type=%d err=%d",tok->tnum,tok->ttype,err_num);
	exe_buffer=fp;
	// MESG("exec_block1:[%s] size of tok_struct is %d",fp->b_fname,sizeof(tok_struct));
   while(tok->ttype!=TOK_EOF && current_active_flag) 
   {
	// MESG(";exec_block:%d ttype=%d",tok->tnum,tok->ttype);
	// if(tok->ttype==TOK_RPAR) { exit(1);};
	if(tok->ttype==TOK_SEP){ NTOKEN2;
		// MESG("factor_sep: [%s %d]",tok->tname,tok->ttype);
		if(tok->ttype==TOK_VAR) lstoken=tok;
		else lstoken=NULL;
		continue;
	};
	if(tok->ttype==TOK_RCURL) { NTOKEN2;lstoken=NULL;return(val);};
	if(tok->ttype==TOK_COMMA) { NTOKEN2;};
	if(tok->ttype==TOK_SHOW) {
		refresh_ddot_1(val);NTOKEN2;continue;
	};
 	val=tok->directive();
   };
   // MESG("exec_block1: end!");
	return(val);
}

double exec_block1_break(FILEBUF *fp)
{
 double val=0;
 stage_level=0;
 TDS("exec_block1");
 // MESG("exec_block1_break:");
 exe_buffer=fp;
   while(tok->ttype!=TOK_EOF && current_active_flag) 
   {
	// MESG(";exec_block:%d ttype=%d",tok->tnum,tok->ttype);
	// if(tok->ttype==TOK_RPAR) { exit(1);};
	if(tok->ttype==TOK_SEP){ NTOKEN2;continue;	};
	if(tok->ttype==TOK_RCURL) { NTOKEN2;return(val);};
	if(tok->ttype==TOK_COMMA) { NTOKEN2;};
	if(tok->ttype==TOK_SHOW) {
		refresh_ddot_1(val);NTOKEN2;continue;
	};
	if(drv_check_break_key()){
		syntax_error("user interruption",100);
		if(is_break1) return 0;
	};
 	val=tok->directive();
   };
   // MESG("exec_block1: end!");
	return(val);
}

/* execute a block from a file  */
double compute_block(FILEBUF *bp,FILEBUF *use_fp,int start)
{
 double val=0;
 int extra=0;
 tok_data *local_symbols;
 tok_data *old_symbol_table=current_stable;
 tok_struct *old_tok=tok;
	MESG("<- [%-15s %s ------------------------------------------------------------->",bp->b_fname,VERSION);
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
	parse_block1(bp,use_fp->symbol_tree,start,extra);
	MESG("parse_block: ended! err=%d start=%d",err_num,start);
	if(err_num) return(0);
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
		// mesg_out("Error %d %s line %d ex_vtype=%d ex_value=%f slval=[%s]!",err_num,err_str,err_line,get_vtype(),ex_value,get_sval());
		show_error("Check init",bp->b_fname);
		return(0);
	};
	init_exec_flags();
	tok=bp->tok_table;

	drv_start_checking_break();
	MESG("	call exec_block1");
	if(execmd) val=exec_block1(bp);
	else val=exec_block1_break(bp);

	drv_stop_checking_break();

	/* cleaning  */
	if(start) {
		if(local_symbols)
		if(bp->symbol_tree){
			delete_symbol_table(local_symbols,bp->symbol_tree->items,0,0);
			bp->symbol_tree=NULL;
#if	TEST_TYPE0
			/* delete the type tree  */
			delete_type_tree(bp->type_tree);
			bp->type_tree=NULL;
#endif
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

int refresh_current_buffer(int nused)
{
 double val=0;
 FILEBUF *fp=cbfp;
 num curline = tp_line(cwp->tp_current);

 if(!is_mlang(fp)) return 0;
 err_num=0;
 err_line=0;
 is_break1=0;
 show_stage=0;
 stage_level=0;

 /* clear parse list  */
 // MESG("refresh_current_buffer: call empty_tok_table: [%s]",fp->b_fname);
 empty_tok_table(fp);
 fp->err=-1;
 // MESG("refresh_current_buffer:1 [%s] %d",fp->b_fname,fp->b_type);
 parse_block1(fp,fp->symbol_tree,1,100);	/* init tree,extra 100 symbols  */

 if(err_num<1){	/* if no errors  */
	fp->symbol_table=new_symbol_table(fp->symbol_tree->items);
	current_stable=fp->symbol_table;

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
		show_error("refresh buffer",fp->b_fname);
		// msg_line("Error %d [%s] at line %d",err_num,err_str,err_line);
		// mesg_out("Error %d [%s] at line %d",err_num,err_str,err_line);
	} else {
		if(vtype_is(VTYPE_STRING)) msg_line("Result is \"%s\"",get_sval());
		if(vtype_is(VTYPE_NUM)) msg_line("Result is [%f]",val);
		if(get_sval()) msg_line("Result is [%s %f]",get_sval(),val);
	};
 } else {
 	msg_line("parse error %d line %d level %d [%s]",err_num,err_line+1,stage_level,err_str);
	return(0);
 };

 if(plot_on()) plot_redraw();
 textpoint_set_lc(cwp->tp_current,curline,0);
 msg_line("");
 set_update(cwp,UPD_EDIT);
 return(OK_CLRSL);
}

// parse, check current buffer
int parse_check_current_buffer(int n)
{
 FILEBUF *fp=cbfp;

 err_num=0;
 err_line=0;
 show_stage=1;
 // MESG("parse_check_current_buffer: %d",is_mlang(fp));
 if(!is_mlang(fp)) return 0;

 /* clear parse list  */
 empty_tok_table(fp);
 stage_level=0;
 // clear out buffer
 cls_fout("[out]");
 // MESG("clear output");
 err_num=check_init(fp);
 MESG("parse_check_current_buffer: after check_init");
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

int parse_buffer_show_tokens(int n)
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
#if	TEST_TYPE0
 fp->type_tree=new_btree("type_tree",0);
#endif
 stage_level=0;
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

char * tok_info(tok_struct *tok)
{
 static char stok[MAXLLEN];
	// MESG("tok_info: start");
	// MESG("tok_info: ttype=%d",tok->ttype);
	if(tok->tname!=NULL){
		if(tok->ttype==TOK_ARRAY1 || tok->ttype==TOK_ARRAY2) {
			int rows=0;
			int cols=0;
			
			if(tok->tok_adat) {
				rows=tok->tok_adat->rows;
				cols=tok->tok_adat->cols;
			};
			snprintf(stok,MAXLLEN,"%3d:%4d %3d [%2d=%12s] [%s] rows=%d cols=%d",
				tok->tnum,tok->tline,tok->tind,tok->ttype,TNAME,(char *)tok->tname,rows,cols);
		} else 
		if(tok->ttype==TOK_SHOW) { snprintf(stok,MAXLLEN,"%3d:%4d %3d [%2d=%12s] [:]",tok->tnum,tok->tline,tok->tind,tok->ttype,TNAME);
		} else
		if(tok->ttype==TOK_LCURL||tok->ttype==TOK_RCURL) {
				snprintf(stok,MAXLLEN,"%3d:%4d %3d [%2d=%12s] %s other is %d",tok->tnum,tok->tline,tok->tind,tok->ttype,TNAME,(char *)tok->tname,tok->match_tok->tnum);
		} else
		if(tok->tgroup>0)
			snprintf(stok,MAXLLEN,"%3d:%4d %3d [%2d=%12s] [%s] group [%d:%s]",tok->tnum,tok->tline,tok->tind,tok->ttype,TNAME,(char *)tok->tname,tok->tgroup,tname(tok->tgroup));
		else 
		if(tok->ttype==TOK_NUM) snprintf(stok,MAXLLEN,"%3d:%4d %3d [%2d=%12s] %f",tok->tnum,tok->tline,tok->tind,tok->ttype,TNAME,tok->dval);
		else if(tok->ttype==TOK_QUOTE) snprintf(stok,MAXLLEN,"%3d:%4d %3d [%2d=%12s] \"%s\"",tok->tnum,tok->tline,tok->tind,tok->ttype,TNAME,(char *)tok->tname);
		else if(tok->ttype==TOK_VAR) {
			// MESG("TOK_VAR:");
			int size=0;
			int vtype=VTYPE_NONE;
			if(current_stable) {
				struct tok_data *var_slot=get_left_slot(tok->tind);
				if(var_slot) vtype=var_slot->vtype;
			};
			// MESG("tok_info var! ind=[%d] group=%d vtype=%d",tok->tind,tok->tgroup,vtype);
			if(vtype==VTYPE_TREE) {
				BTNODE *tok_node=tok->tok_node;
				BTREE *type_tree=(BTREE *)tok_node->node_dat;
				size = type_tree->items;
				// var_type="btree type";
			};

			snprintf(stok,MAXLLEN,"%3d:%4d %3d [%2d=%12s] [%s] type %s %d size %d",tok->tnum,tok->tline,tok->tind,tok->ttype,TNAME,(char *)tok->tname,vtype_names[vtype],vtype,size);
		} else snprintf(stok,MAXLLEN,"%3d:%4d %3d [%2d=%12s] [%s]",tok->tnum,tok->tline,tok->tind,tok->ttype,TNAME,(char *)tok->tname);
// 			
	} else {
		     snprintf(stok,MAXLLEN,"%3d:%4d %3d [%2d=%12s] [%f]",tok->tnum,tok->tline,tok->tind,tok->ttype,TNAME,tok->dval);
	};
	// MESG("tok_info: end");
	return stok;
}

int show_parse_buffer(int n)
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
 parse_block1(fp,fp->symbol_tree,1,0);

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

double get_val()
{
#if	USE_VAR
	return ex_var.dval;
#else
	return (ex_value);
#endif
}

array_dat *get_array(char *pos)
{
	// MESG("get_array:[%s] num=%d type=%d tok [%s]  ind=%d num=%d type=%d atype=%d",
		// pos,ex_var.adat->anum,ex_var.vtype,tok->tname,tok->tind,tok->tnum,tok->ttype,ex_var.adat->atype);
#if	USE_VAR
	// if(ex_var.vtype==1) return NULL;
	return ex_var.adat;
#else
	if(ex_array==NULL) MESG("	return null!");	
	return ex_array;
#endif
}

void set_array(array_dat *a)
{
#if	USE_VAR
	// MESG("set_array:num=%d type %d allocated=%d name=%s",a->anum,a->atype,a->astat,a->array_name);
	ex_var.adat=a;
	ex_var.vtype=a->atype;
#else
	ex_array=a;
#endif
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

double next_value()
{
 double v;
 v=atof(get_sval());
 return(v);
}

int get_vtype()
{
#if	USE_VAR
	return ex_var.vtype;
#else
	return ex_vtype;
#endif
}

int vtype_is(int type)
{
#if	USE_VAR
	return type==ex_var.vtype;
#else
	return type==ex_vtype;
#endif
}

void set_vtype(int type)
{
#if	USE_VAR
	// MESG("set_vtype:");
	ex_var.vtype=type;
#else
	ex_vtype=type;
#endif
}

void set_nsval(char *s,int max)
{
 clean_saved_string(max);
 memcpy(saved_string,s,max);
 saved_string[max]=0;
 // MESG(";set_nsval:s=[%s] [%s] %d",s,saved_string,max);
#if	USE_VAR
 // MESG("set_nsval:");
 ex_var.vtype=VTYPE_STRING;
#else
 ex_vtype=VTYPE_STRING;
#endif
}

void set_sval(char *s)
{
 clean_saved_string(0);
 saved_string=strdup(s);
#if	USE_VAR
 // MESG("set_sval:");
 ex_var.vtype=VTYPE_STRING;
#else
 ex_vtype=VTYPE_STRING;
#endif
}

void set_vdval(double value){
#if	USE_VAR
	// MESG("set_vdval: %f",value);
	ex_var.vtype=VTYPE_NUM;
	ex_var.dval=value;
#else
	ex_vtype = VTYPE_NUM;
	ex_value = value;
#endif
}

void set_dval(double value)
{
#if	USE_VAR
	// MESG("set_dval: %f",value);
	ex_var.dval=value;
#else
	ex_value = value;
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
	// MESG("nextarg:");
	if (macro_exec == FALSE) {
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
 node->node_type=-1;
 node->node_vtype=VTYPE_NUM;
}

/* -- */
