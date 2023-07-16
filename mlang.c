/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

 	An interpreter,embeded calulator by K.Triantafillou (2011,2020),
*/

#include	<math.h>
#include	<stdlib.h>
#include	"xe.h"

#include	"alist.h"
#include	"mlang.h"
#include	"func.h"

#define	SYNTAX_DEBUG	0
#define	NTOKEN2	tok++
#define FACTOR_FUNCTION tok->factor_function()

void mesg_out(const char *fmt, ...);
extern FILEBUF *cbfp;
extern array_dat *main_args;

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


int err_check_block1(int level);
int check_skip_token1( int type);
void clean_saved_string(int new_size);
int deq(double v1,double v2);

double assign_val(double none);
double assign_env(double none);
void init_error();
void get_lowercase_string(char *lower, char *string);
void get_uppercase_string(char *lower, char *string);
double cexpression();
double exec_block1_break();
char * tok_info(tok_struct *tok);

TLIST ctoklist=NULL;
int is_break1=0;
int tok_mask[256];
int ex_vtype=0; 	/* type of previous expression */
int ex_edenv=0;	/* true after encount an editor env variable */
static double ex_value;	// saved double value
array_dat *ex_array=NULL;
int ex_nvars=0;	/* true is there are variables in the array definition  */
int ex_nquote=0;	/* true if there are strings in the array definition  */
int ex_nums=0;	/* true if array is only numeric  */
char *ex_name=NULL;	/* variable name of the previous array  */
char *saved_string=NULL;

/* error control variables  */
int err_num=0;
static int err_line=0;
int last_correct_line=0;
static tok_struct *tok;	/* current token!!  */

char *err_str;
BTNODE *var_node=NULL;

tok_data *lsslot=NULL;
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
	{"xor" ,TOK_XOR, TOK_BOOL},
	{"or"  ,TOK_OR,  TOK_BOOL},
	{"and" ,TOK_AND, TOK_BOOL},
	{"nand",TOK_NAND,TOK_BOOL},
	{NULL,0,0}
};

// token type names
char *tok_name[] = {
	"none",
	"separator","space","letter","left curl","right curl",
	"dquote",
	"left par","right par","show","comment","variable",
	"ed option","ed command","ed function","macro proc","ed env",
	"term0","term","term1","term2",
	"assingment","eof","numeric",
	"directive","dir if","dir else","dir break","dir return",
	"dir while","dir for","comma",
	"for i",
	"compare","not equal",
	"smaller","bigger","equals","smaller or equal","bigger or equal",
	"bool",
	"bool and","bool or","bool not","bool nand","bool nor","bool xor",
	"plus","minus","power","modulo","multiply","divide",
	"left braket","right braket","squote","at","range","back quote","dolar","tilda",
	"increase","decrease","increase by","mul_by", "decrease by","backslash",
	"new line",
	"continue",
	"foreach",
	"array","array2","array3","assign env","assign option",
//	"end arg",
	"other",
	NULL
};

/* Function definitions */
#include "mlang_functions.c"

/* directives */
char *directives[] = {
 "{","}",
 "if","else",
 "break",
 "return",
 "while",
 "for","function","fori",NULL
};

array_dat *transpose(array_dat *array1);

void init_btree_table()
{
	bt_table=new_btree("table",0);
	directiv_table=new_btree("directives",0);
}

void clear_args(MVAR *va,int nargs)
{
 int i;
 // MESG("clear_args: %d",nargs);
 if(va) {
	 for(i=0;i<nargs;i++){
	 	// MESG("	%d %d %X",i,va[i].vtype,va[i].sval);
		if(va[i].vtype==VTYPE_STRING){
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
 lcurl->mline=mline;
 lcurl->ocurl=el;
 lcurl->num=0;
 lcurl->active=1;
 return(lcurl);
}


static inline double factor_none()
{
	NTOKEN2;
	return 0.0;	/* continue  */
}

double add_value(double v1)
{
	tok_data *sslot;
	double v0=0;
	TDS("add_value");
	sslot=lsslot;
	// MESG("add_val: ind=%d type=%d ex_vtype=%d",sslot->ind,sslot->vtype,ex_vtype);

	if(sslot->vtype==VTYPE_NUM ) {
		v0=sslot->dval;
		sslot->dval+=v1;
		return(v0);
	};
	if(sslot->vtype==VTYPE_ARRAY) {
		v0=*sslot->pdval;
		*sslot->pdval+=v1;
		return(v0);
	}
#if	USE_SARRAYS
	if(sslot->vtype==VTYPE_SARRAY) {
		char *stmp=malloc(strlen(saved_string)+strlen(sslot->psval[0]));
		sprintf(stmp,"%s%s",sslot->psval[0],saved_string);
		strcpy(stmp,sslot->psval[0]);
		strcat(stmp,saved_string);
		saved_string=stmp;
		return(0);
	};
#endif

	return(v0);
}

double increase_val()
{
 double v0;
	TDS("increase_val");
	// MESG("increase_val:");
	ex_vtype=VTYPE_NUM;
	v0=add_value(1.0);
	// NTOKEN2;
	return(v0);
}

double decrease_val()
{
	TDS("decrease_val");
	// MESG("decrease_val:");
	ex_vtype=VTYPE_NUM;
	add_value(-1.0);
	NTOKEN2;
	return(lsslot->dval);
}

double decrease_by()
{
	double v1,v0;
	tok_data *sslot;
	TDS("decrease_val");
	sslot=lsslot;
	// MESG("decrease_by: of [%s]",tok_info(lsslot));
	v1=lexpression();

	if(sslot->vtype==VTYPE_NUM) {
		v0=sslot->dval;
		sslot->dval =v0-v1;
		return(sslot->dval);
	};
	if(sslot->vtype==VTYPE_ARRAY) {
		v0=*sslot->pdval;
		
		*sslot->pdval = v0-v1;
		// MESG("array: %f -> %f",v0,*sslot->pdval);
		return(v0-v1);
	};
	return(-1);
}

double increase_by()
{
	double v1,v0;
	tok_data *sslot;
	// TDS("increase_by");
	sslot=lsslot;
	// if(sslot->vtype==VTYPE_STRING) MESG("increase_by string [%s]",sslot->sval);
	
	v1=lexpression();

	if(sslot->vtype==VTYPE_NUM) {
		v0=sslot->dval;
		sslot->dval = v0+v1;
		return(sslot->dval);
	};
	if(sslot->vtype==VTYPE_ARRAY) {
		v0=*sslot->pdval;
		
		*sslot->pdval = v0+v1;
		// MESG("array: %f -> %f",v0,*sslot->pdval);
		return(v0+v1);
	};
	if(sslot->vtype==VTYPE_STRING) {
		// MESG("increase by:");
		sslot->sval=(char *)realloc(sslot->sval,strlen(sslot->sval)+strlen(saved_string)+1);
		strcat(sslot->sval,saved_string);
		set_sval(sslot->sval);
	};
#if	USE_SARRAYS
	if(sslot->vtype==VTYPE_SARRAY) {
		char *stmp=malloc(strlen(saved_string)+strlen(sslot->psval[0]));
		strcpy(stmp,sslot->psval[0]);
		strcat(stmp,saved_string);	/* check!! TODO  */
		free(saved_string);
		// MESG("caoncatanated[%s]",stmp);
		free(sslot->psval[0]);
		sslot->psval[0]=stmp;

		return(0);
	};
#endif
	return(-1);
}

double mul_by()
{
	double v1,v0;
	tok_data *sslot;
	// TDS("increase_by");
	sslot=lsslot;
	// if(sslot->vtype==VTYPE_STRING) MESG("increase_by string [%s]",sslot->sval);
	
	v1=num_expression();

	if(sslot->vtype==VTYPE_NUM) {
		v0=sslot->dval;
		sslot->dval = v0*v1;
		return(sslot->dval);
	};
#if	0
	if(sslot->vtype==VTYPE_ARRAY) {
		v0=*sslot->pdval;
		
		*sslot->pdval = v0+v1;
		// MESG("array: %f -> %f",v0,*sslot->pdval);
		return(v0+v1);
	};
#endif
#if	0
	if(sslot->vtype==VTYPE_STRING) {
		// MESG("increase by:");
		sslot->sval=(char *)realloc(sslot->sval,strlen(sslot->sval)+strlen(saved_string)+1);
		strcat(sslot->sval,saved_string);
		set_sval(sslot->sval);
	};
#endif
	return(-1);
}

tok_struct *new_tok()
{
 tok_struct *tok;
 tok=(struct tok_struct *) malloc(sizeof(struct tok_struct));
 tok->level=0;
 tok->tind=0;
 tok->tline=0;
 tok->tnum=0;
 tok->tname="empty";
 tok->dval=0;
 tok->ttype=0;
 tok->tgroup=0;
 // tok->tatype=0;
 tok->factor_function=factor_none;
 tok->directive=lexpression;
 tok->tnode=NULL;
 // MESG("new_tok:");
 return(tok);
}

#define	TNAME	tname(tok->ttype)

char *tname(int type)
{
 int ind=type%1000;
	return(tok_name[ind]);
}

void init_token_mask()
{
 int ind;

 for(ind=0;ind<255;ind++) {
 	if((ind>='a' && ind<='z') ||(ind>='A' && ind<='Z') || ind=='_' || ind>=128 || ind=='.' ) 
 	tok_mask[ind]=TOK_LETTER;
	else if(ind>='0'&&ind<='9') tok_mask[ind]=TOK_NUM;
	else if(ind==' '||ind==9) tok_mask[ind]=TOK_SPACE;
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
	set_btval(option_names[i].name,TOK_OPTION,option_names[i].sval,option_names[i].dval);
 };

 // set_btval("args",TOK_VAR,"",0);

 for(i=0;term_types[i].term_name!=NULL;i++)
 {
	BTNODE *tnode;
	tnode = set_btdval(directiv_table,term_types[i].term_name,i);
	tnode->node_index = term_types[i].term_type;
	tnode->node_type = term_types[i].term_group;
//	tnode->val = i+1;
 };
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
 tok_data *td=malloc(sizeof(struct tok_data)*(size+1));
 if(td==NULL) { err_num=101;return NULL;};
 for(i=0;i<size;i++) {
	td[i].ind=i;
 	td[i].vtype=VTYPE_NUM;
	td[i].pdval=NULL;
#if	USE_SARRAYS
	td[i].psval=NULL;
#endif
	td[i].dval=0;
	td[i].sval=NULL;
 };
 return td;
}

/* free symbol table after execute */
void delete_symbol_table(tok_data *td, int size,int level)
{
 int i;
 // tok_data *sslot;
 // MESG("delete_symbol_table: size=%d level=%d",size,level);
 if(td) {
 for(i=0;i<size;i++) {
	tok_data *sslot;
 	sslot=&td[i];

	if(sslot->vtype==VTYPE_STRING) {
		// MESG("delete_symbol_table:%d [%s] free %X",i,sslot->sval,sslot->sval);
		 	if(sslot->sval!=NULL)  free(sslot->sval);
	};
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

void init_exec_flags()
{
 // MESG("init_exec_flags:");
 init_error();
 is_break1=0;
 current_active_flag=1;
 ex_vtype=0;
 ex_value=0.0;
 var_node=NULL;
 pnum=0;
 stage_level=0;
 ex_nvars=0;ex_nquote=0;ex_nums=0;	/* initialize table counters  */
 // MESG("init_exec_flags:end");
}

void show_error(char *from,char *name)
{
 int var_index=-1;
 if(tok) if(tok->tnode) var_index=tok->tnode->node_index;
 if(var_index>=0)
	ERROR("%s error %d file %s after function [%s] after line %d: [%s]",from,err_num,name,ftable[var_index].n_name,last_correct_line,err_str);
 else {
	ERROR("%s error %d file %s before line %d: [%s]",from,err_num,name,last_correct_line,err_str);
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
	// MESG("block parsed ok");
	if(err_num>0) {
		msg_line("found parsed errors: err_num=%d",err_num);
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
		err=err_check_block1(0);
		bf->err=err;
		bf->m_mode |= M_CHECKED;
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

 err_num=-1;
 if(tok->ttype!=TOK_RPAR && nargs!=0)
 {

 va=(MVAR *) malloc(sizeof(MVAR)*(nargs));

 if(va){
 MVAR *va_i=va;
 for(i=0;i<nargs;i++,va_i++){
	
	value = num_expression();
	va_i->vtype=ex_vtype;

	if(ex_vtype==VTYPE_NUM) {
			va_i->dval=value;
	} else
	if(ex_vtype==VTYPE_STRING) {
		va_i->sval=saved_string;
		clean_saved_string(0);
	} else 
	if(ex_vtype==VTYPE_ARRAY||ex_vtype==VTYPE_SARRAY) {
			va_i->adat=ex_array;
			va_i->vtype=ex_array->atype;
	} else {
			ERROR("error: wrong type arg %d",ex_vtype);
			err_num=202;
			clear_args(va,i); return(NULL);
	}

	NTOKEN2; // skip separator or right parenthesis!
 };
 return(va);
 }
 };
 	return(NULL);
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
	current_stable=new_symbol_table(bp->symbol_tree->items);	/* create new symbol table  */

	if(current_stable==NULL) { 
		err_num=208;
		ERROR("cannot create new symbol table! proc level is %ld",level);
		is_break1=1;
		level=0;
		return 0;
	};

	assign_args1(vargs,current_stable,nargs);
	// MESG("exec_function: after assign_args1 [%s] tnum=%d ttype=%d",tok->tname,tok->tnum,tok->ttype);
	value=tok->directive();
	// MESG("exec_function: before delete_symbol_table, ex_value=%f",ex_value);
	/* remove local variable tree and restore the old one  */
	delete_symbol_table(current_stable,bp->symbol_tree->items,level);
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
	array_dat *adat=tok->adat;
	cdim=1;
	ex_array=adat;
	ex_name="Definition";
	// MESG("factor_line_array:");
	allocate_array(ex_array);
	NTOKEN2;
	while(cdim>0){
	while(tok->ttype!=TOK_EOF) {
		if(tok->ttype==TOK_SEP) { 
			NTOKEN2;
			continue;
		};
		value=cexpression();
		if(ex_vtype==VTYPE_STRING) adat->atype=VTYPE_SARRAY;
		// MESG("	[%d %d]: value=%f [%s]",j,i,value,saved_string);
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
			// MESG("	add row %d col=%d -> %d [%s]",j,i,ind1,saved_string);
			adat->sval[ind1]=strdup(saved_string);
		};
		i++;if(i>cols) cols=i;
		if(tok->ttype==TOK_SHOW || tok->ttype==TOK_RBRAKET) {
			cdim=0;break;
		};
		if(tok->ttype==TOK_SEP) {
			i=0;
			j++;
			cdim++;if(cdim>rows) rows=cdim;
			NTOKEN2;};
	};cdim--;
	};
	ex_array->astat=ARRAY_LOCAL;
	ex_vtype=adat->atype;
	// print_array1("",adat);
	NTOKEN2;
	RTRN(1.2);
}

tok_data *get_left_slot(int ind)
{
	// MESG("get_left_slot: ind=%d",ind);
	return &current_stable[ind];
}

double factor_variable()
{
	// lsslot=&current_stable[tok->tind];	/* symbol table variable's slot  */
	lsslot= get_left_slot(tok->tind);
	ex_vtype=lsslot->vtype;
	// MESG("	factor_variable: ind=%d type=%d",lsslot->ind,lsslot->vtype);
	switch(lsslot->vtype) {
		case VTYPE_NUM:{
			double val=lsslot->dval; 
			NTOKEN2;
			if(tok->tgroup==TOK_INCREASE) {
				lsslot->dval += tok->dval;
				NTOKEN2;
			};
			RTRN(val);
			};
		case VTYPE_STRING:
			set_sval(lsslot->sval);
			NTOKEN2;
			RTRN(lsslot->dval);
		case VTYPE_ARRAY:
		case VTYPE_SARRAY:
			// MESG("factor_variable: [%s] array",tok->tname);
			ex_array=lsslot->adat;
			ex_name=tok->tname;
			NTOKEN2;
			RTRN(lsslot->dval);
		default:
		// error !!
		syntax_error("no data in the array!!",2081);
		NTOKEN2;
		RTRN(lsslot->dval);
	};
}

double factor_array2()
{
	int ind1=0;
	int ind2=0;
	double **dval2;
	double value=0;
	tok_data *array_slot;
	array_dat *adat;
	tok_struct *tok0 = tok;
	// MESG("factor_array2: %s",tok->tname);
	// array_slot=&current_stable[tok->tind];
	array_slot=get_left_slot(tok->tind);
	adat=array_slot->adat;
	ex_vtype=VTYPE_NUM;
	if(adat==NULL) {
#if	1
		set_error(tok0,209,"array indexes out of bound!");
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
			// array_slot->pdval=&dval2[ind1][ind2];
#endif
	} else {
		NTOKEN2;

		ind1=(int)num_expression();
		NTOKEN2;
		ind2=(int)num_expression();
		NTOKEN2;
		dval2 = adat->dval2;

		if((ind1 >= adat->rows) || (ind2 >= adat->cols)) {
			// syntax_error("array indexes out of bound!",209);
			set_error(tok0,209,"array indexes out of bound!");
		} else {
			value=dval2[ind1][ind2];
			array_slot->pdval=&dval2[ind1][ind2];
		};
	};
	// MESG("end factor_array2: lsslot=%X",(void *)lsslot);
	return(value);
}

double factor_option()
{
 BTNODE *bte; 
/* variable's name in tok0->tname */
	bte=tok->tnode;
	var_node=bte;
	NTOKEN2;
	if(bte->sval!=NULL) { /* there is a valid string value */
		clean_saved_string(strlen(bte->sval));
		strcpy(saved_string,bte->sval);
		ex_vtype=VTYPE_STRING;
	} else ex_vtype=VTYPE_NUM;
	RTRN(bte->val);
}

double factor_array1()
{
	int ind1;
	double *dval=NULL;
	double value=0;
	tok_data *array_slot;
	// MESG("factor_array1: ttype=%d %d",tok->ttype,TOK_LBRAKET);
	array_slot=&current_stable[tok->tind];
	// MESG("	array1: vtype=%d",array_slot->vtype);
	NTOKEN2;
	// ind1=(int)FACTOR_FUNCTION;
	ind1 = (int)num_expression();
	// MESG("factor_array1: ind1=%d",ind1);
	if(tok->ttype==TOK_RBRAKET) { 
		NTOKEN2;
		// MESG("ends with rbracket!!");
	};
	// MESG("factor_array1:ind=%d ind1=%d type=%d",array_slot->ind,ind1,array_slot->vtype);
	if(array_slot->adat == NULL) {
		ex_nums=1;
		array_slot->adat=new_array(ind1+1,1);
		array_slot->vtype=VTYPE_ARRAY;
		allocate_array(array_slot->adat);	/*   */
		// MESG("	array allocated:%X",array_slot->adat->dval);
	} else {
		// MESG("	2 vtype=%d %d",array_slot->vtype,VTYPE_ARRAY);
		if(array_slot->vtype==VTYPE_ARRAY)
		if(array_slot->adat->rows<ind1 && array_slot->adat->cols<ind1) {
			 {
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
			};
			// MESG("	array reallocated:%X",array_slot->adat->dval);
		};
	};
#if	USE_SARRAYS
	if(array_slot->vtype==VTYPE_SARRAY) {
		char **sval = array_slot->adat->sval;
		clean_saved_string(strlen(array_slot->adat->sval[ind1]));	/* Check!! TODO  */
		strcpy(saved_string,array_slot->adat->sval[ind1]);
		// MESG("	show string value![%s]",saved_string);
		// array_slot->psval = &sval[ind1];
		array_slot->psval=&sval[ind1];
		value=0;
		ex_vtype=VTYPE_STRING;
	} else 
#endif
	{
		dval = array_slot->adat->dval;
		value=dval[ind1];
		array_slot->pdval=&dval[ind1];
		ex_vtype=VTYPE_NUM;
	};
	lsslot=array_slot;
	// MESG("factor_array1: end");
	// MESG("	factor_array1:ind1=%d lsslot ind=%d type=%d rows=%d cols=%d [%s]!",ind1,lsslot->ind,lsslot->vtype,lsslot->adat->rows,lsslot->adat->cols,array_slot->psval[0]);
	return(value);
}

double factor_cmd()
{ // 3 editor command
	int var_index;
	int check_par=0;
	int save_macro_exec;
	int status=1;
	double value=1;
	FUNCS *ed_command;

	ex_vtype=VTYPE_NUM;
	// MESG(";factor_cmd: ttype=%d command=%d",tok->ttype,tok->tnode->node_index);
	var_index = tok->tnode->node_index;
	ed_command = ftable+var_index;

	NTOKEN2;
	save_macro_exec=macro_exec;
	macro_exec=MACRO_MODE2;
	// MESG(";ed_command: [%s] args=%d",ed_command->n_name,ed_command->arg);
	if(ed_command->arg) {
#if	!NO_LPAR
		if(ed_command->arg>2 || ed_command->arg<0) {
			NTOKEN2;	/* skip parenthesis  */
			return ed_command->n_func(1);
		};
#endif
		check_par=1;	/* we need parenthesis if arguments.  */
#if	!NO_LPAR
		NTOKEN2;
#endif
		value=num_expression();
		// MESG(";	ed_command: value=%f ex_vtype=%d s=[%s]",value,ex_vtype,saved_string);
		ex_value=value;
		switch(ed_command->arg) {
			case 1:{ /* one argument */
				if(ex_vtype==VTYPE_STRING) { value=1;};
				break;
			};
			case 2:	/* two arguments  */
			{
				NTOKEN2;
#if	0
				value=num_expression();
				// MESG(";	ed_command:arg2 value=%f ex_vtype=%d s=[%s]",value,ex_vtype,saved_string);
#endif
				// MESG(";ed_command: second token! type=%d ",tok->ttype);
				break;
			};
		};

	} else {	/* no argument  */

#if	0
		if(tok->ttype==TOK_LPAR) {	/* we can have parenthesis or not. */
			NTOKEN2;
			check_par=1;
		};
		value=1;
#endif
	};
	macro_exec = MACRO_MODE2;

	err_num=0;
	err_line=tok->tline;
	err_str=NULL;
	// tok_struct *tok1=tok;
	// MESG(";factor_cmd: execute function! current token is [%s] tnum=%d",tok->tname,tok->tnum);
	status=ed_command->n_func((int)value);
	// MESG("toknum1 = %d",tok1->tnum);
	// tok=tok1;
	// MESG(";TOC_CMD: tnum=%d status=%d check_par=%d",tok->tnum,status,check_par);
	ex_value=status;
//	editor command returns a numeric value
	ex_vtype=VTYPE_NUM;

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
	RTRN(ex_value);
}

double factor_env()
{
	BTNODE *bte;
	double value=0;
	ex_vtype=VTYPE_NUM;
	bte=tok->tnode;
	var_node=bte;
	value = get_env(bte->node_index);
	ex_value = value;
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
	ex_vtype=VTYPE_NUM;
	NTOKEN2;
	RTRN(val);
}

/* string  */
double factor_quote()
{
	// MESG("factor_quote: tnum=%d tname=%s",tok->tnum,tok->tname);
	ex_vtype=VTYPE_STRING;
	set_sval(tok->tname);
	NTOKEN2;
	RTRN(0);		/* 0 value for string variables  */
}

double factor_at()
{
	int bval=macro_exec;
	double value;
	macro_exec=MACRO_TRUE;
	ex_vtype=VTYPE_NUM;
	value=sysexec((char *)tok->tname);
	NTOKEN2;
	macro_exec=bval;
	RTRN(value);
}

static inline double factor_plus()
{
	NTOKEN2;
	return FACTOR_FUNCTION;
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
	// MESG("factor_proc:");
	NTOKEN2;
#if	!NO_LPAR
	NTOKEN2;	/* this is left parenthesis */
#endif
	/* function */
	MVAR *vargs = NULL;
	// MESG("factor_proc: tok0 [%d %s] args=%d",tok0->tnum,tok0->tname,tok0->tind);
	vargs = push_args_1(tok0->tind);
	// MESG("factor_proc: vargs=%d",vargs);
	after_proc=tok;
	// MESG("factor_proc: tok after push [%d %s]",tok->tnum,tok->tname);
	value=exec_function(tok0->tbuf,vargs,tok0->tind);
	// MESG("factor_proc: return val=%f",value);
	tok=after_proc;

	current_active_flag=1;	/* start checking again  */
	free(vargs);
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
	if(ex_vtype==VTYPE_ARRAY){
		array_dat *loc_array = ex_array;
		v2 = FACTOR_FUNCTION;
		if(ex_vtype==VTYPE_NUM) {
			if(loc_array->astat==ARRAY_LOCAL) loc_array=dup_array_power(loc_array,v2);
			else array_power(loc_array,v2);
			ex_array=loc_array;
			ex_vtype=VTYPE_ARRAY;
			ex_name="array power numeric";
		} else {
			syntax_error("wrong power on table",2101);
			set_break();
		};
		return(1);
	} else {
		v2=FACTOR_FUNCTION;
		v1=pow(v1,v2);
		ex_vtype=VTYPE_NUM;
	}
 return v1; 	
}


static double term2_modulo(double v1)
{
double v2;
	NTOKEN2;
	if(ex_vtype==VTYPE_ARRAY){
		array_dat *loc_array = ex_array;
		v2 = FACTOR_FUNCTION;
		if(v2>0) 
		{
			if(loc_array->astat==ARRAY_LOCAL) loc_array=dup_array_mod1(loc_array,v2);
			else array_mod1(loc_array,v2);
			ex_array=loc_array;
			ex_vtype=VTYPE_ARRAY;
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
		ex_vtype=VTYPE_NUM;
	} else {
		/* RT error  */
		syntax_error("wrong modulo value value=0",2103);
		set_break();
	};
	}
	RTRN(v1);
}

static double term1_mul(double v1)
{
 double v2;
 TDS("term1_mul");
 // MESG("term1_mul: ex_vtype=%d",ex_vtype);
	if(ex_vtype==VTYPE_NUM){
		NTOKEN2;
		v2=num_term2();
		switch(ex_vtype)
		{
			case VTYPE_NUM:	// numeric * numeric
				v1=v1*v2;
				return v1;
			case VTYPE_ARRAY:	// numeric * array
				ex_array = dup_array_mul1(ex_array,v1);
				ex_name = "numeric * array";
				ex_vtype=VTYPE_ARRAY;
				return 1;
		};
	};
	if(ex_vtype==VTYPE_ARRAY){
		array_dat *loc_array = ex_array;
		NTOKEN2;
		v2 = num_term2();
		if(ex_vtype==VTYPE_NUM) {
			if(loc_array->astat==ARRAY_LOCAL) loc_array=dup_array_mul1(loc_array,v2);
			else array_mul1(loc_array,v2);
			ex_array=loc_array;
			ex_vtype=VTYPE_ARRAY;
			ex_name="array * numeric";
			return 1;
		};
		if(ex_vtype==VTYPE_ARRAY) {
			if(loc_array->rows==1 && ex_array->cols==1) {
				if(loc_array->cols==ex_array->rows) {
					int i;
					v1=0;
					for(i=0;i<loc_array->cols;i++){
						v1 += loc_array->dval[i]*ex_array->dval[i];
					};
					ex_vtype=VTYPE_NUM;
					// free old ex_array ???
					ex_array=NULL;
					ex_name="array * array";
					RTRN(v1);
				};
				syntax_error("array multiply error",213);
				ex_vtype=VTYPE_NUM;
				RTRN(v1);
			} else {
				array_dat *loc_array2;
				loc_array2=array_mul2(loc_array,ex_array);
				ex_vtype=VTYPE_ARRAY;
				
				if(loc_array->astat==ARRAY_ALLOCATED) {	/* free this one!!  */
				};
				ex_array=loc_array2;
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
	if(ex_vtype==VTYPE_NUM){
		NTOKEN2;
		v2=num_term2();
			switch(ex_vtype)
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
					ex_array = dup_array_mul1(ex_array,1/v1);
					ex_name = "numeric / array";
					ex_vtype=VTYPE_ARRAY;
					return 1;
			};
	};
	if(ex_vtype==VTYPE_ARRAY){
		array_dat *loc_array = ex_array;
		NTOKEN2;
		v2 = num_term2();
		if(v2==0) {
			/* RT error  */
			syntax_error("Division by zero",215);
			v1 = 9999999999.9;
			RTRN(v1);
		};

		if(ex_vtype==VTYPE_NUM) {
			if(loc_array->astat==ARRAY_LOCAL) loc_array=dup_array_mul1(loc_array,1/v2);
			else array_mul1(loc_array,1/v2);
			ex_array=loc_array;
			ex_vtype=VTYPE_ARRAY;
			ex_name="array / numeric";
			return 1;
		};
		if(ex_vtype==VTYPE_ARRAY) {
				ERROR("div arrays not implemmented yet!");
				RTRN(v1);
		};
	};
	set_error(tok,216,"division op not supported");
	set_break();
	RTRN(v1);
}

double factor_rcurl(){
	NTOKEN2;
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
	increase_val,	// TOK_INCREASE	,
	decrease_val,	// TOK_DECREASE	,
	factor_none,	// TOK_INCREASEBY
	mul_by,	// TOK_MULBY
	decrease_by,	// TOK_DECREASEBY
	factor_none,	// TOK_BSLASH		,

	factor_none,	// TOK_NL				,
	factor_none,	// TOK_DIR_CONTINUE	,
	factor_none,	// TOK_DIR_FOREACH		,

	factor_array1,	// TOK_ARRAY1
	factor_array2,	// TOK_ARRAY2
	factor_none,	// TOK_ARRAY3
	factor_none,	// TOK_ASSIGNENV	,
	factor_none,	// TOK_ASSIGNOPT	,
	factor_none,	// TOK_START,
	factor_none,	// TOK_END,
	factor_none		// TOK_OTHER,
};

void set_tok_function(tok_struct *tok, int type)
{
	switch(type) {
		case 0:
			if(tok->ttype==TOK_FUNC) {
				int findex = tok->tnode->node_index;
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
	return exec_block1();
}

static double inline dir_lcurl_break()
{
	NTOKEN2;
	return exec_block1_break();
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
	if(tok->ttype==TOK_SEP || tok->ttype==TOK_RPAR) { val=0;ex_vtype=VTYPE_NUM;}
	else val=lexpression();
	current_active_flag=0;	/* skip rest of function  */
	return(val);
}

double term_plus(double value)
{
 double d1;
 if(ex_vtype==VTYPE_NUM) {
	NTOKEN2;
	d1=num_term1();
	// MESG("term_plus");
	if(ex_vtype==VTYPE_NUM) return value+d1;
	if(ex_vtype==VTYPE_STRING) {
		char svalue[MAXLLEN];
		int stat;
		stat=snprintf(svalue,MAXLLEN,"%f%s",value,saved_string);
		if(stat>MAXLLEN) MESG("truncated 2");
		set_sval(svalue);
		return 0;
	};
	if(ex_vtype==VTYPE_ARRAY) { // num + array
		if(ex_array->astat==ARRAY_LOCAL) {
			ex_array=dup_array_add1(ex_array,value);
			ex_name="New array,add to numeric";
		} else {
			array_add1(ex_array,value);
			ex_name=tok->tname;
		};
		ex_vtype = VTYPE_ARRAY;
		ex_name="Add to numeric";
		return 0;
	};
	err_num=217;
	err_line=tok->tline;
	ERROR("operation not supported err %d",err_num);
	RTRN(0);
 };

 if(ex_vtype==VTYPE_STRING) {	// set local value
	char svalue[MAXLLEN];
	strlcpy(svalue,saved_string,MAXLLEN);

		NTOKEN2;
		d1=num_term1();
		 /* catanate string */
			if(ex_vtype==VTYPE_STRING) {	/* string catanate  */
				strlcat(svalue,saved_string,sizeof(svalue));
				value=atof(svalue);
				set_sval(svalue);
				return value;
			};
			if(ex_vtype==VTYPE_NUM)	{	/* string, numeric catanate  */
				long l0;
				ex_vtype=VTYPE_STRING;
				l0=d1;
				clean_saved_string(80);
				if(l0==d1)snprintf(saved_string,80,"%ld",l0);
				else snprintf(saved_string,80,"%f",d1);
				strlcat(svalue,saved_string,sizeof(svalue));
				value=0;
				set_sval(svalue);
				return value;
			};
			if(ex_vtype==VTYPE_ARRAY) {
				err_num=218;
				ERROR("Subtrsct array from string not supported err %d",err_num);
				RTRN(value);				
			};
			err_num=219;
			ERROR("operation not supported err %d",err_num);
			RTRN(value);				
 };

 if(ex_vtype==VTYPE_ARRAY) {
  	array_dat *loc_array=ex_array;
		 		NTOKEN2;
				d1=num_term1();
				if(ex_vtype==VTYPE_NUM) { // add numeric to array
					if(loc_array->astat==ARRAY_LOCAL) {
						loc_array=dup_array_add1(loc_array,d1);
					} else {
						array_add1(loc_array,d1);
					};
					ex_vtype=VTYPE_ARRAY;
					ex_array=loc_array;
					ex_name="Add to numeric";
					return 0;
				} else if(ex_vtype==VTYPE_ARRAY) {	// array addition
					array_dat *loc_array2;
					loc_array2=array_add2(loc_array,ex_array);
					ex_vtype=VTYPE_ARRAY;
					ex_array=loc_array2;
					ex_name="Add to array";
					RTRN(value);
				} else {
					err_num=220;
					ERROR("operation not_supported err %d",err_num);
					RTRN(value);
				};		
 };
 return 0;
}

double term_minus(double value)
{
 double d1;
 if(ex_vtype==VTYPE_NUM) {	/* numeric  !!  */
	NTOKEN2;
	d1=num_term1();

	if(ex_vtype==VTYPE_NUM) return value-d1;
	if(ex_vtype==VTYPE_ARRAY) {
		if(ex_array->astat==ARRAY_LOCAL) {
			ex_array=dup_array_sub1(ex_array,value);
			ex_name="New array,subtract from numeric";
		} else {
			array_sub1(ex_array,value);
			ex_name="Subtract from numeric";
		};
		ex_vtype = VTYPE_ARRAY;
		return 0;
	};
	err_num=215;
	err_line=tok->tline;
	ERROR("operation not supported err %d",err_num);
	RTRN(value);
 };
 if(ex_vtype==VTYPE_STRING) {	// set local value
	char svalue[MAXLLEN];
	strlcpy(svalue,saved_string,MAXLLEN);
		NTOKEN2;
		// operator on first chars of strings. numeric result
			d1=num_term1();
			if(ex_vtype==VTYPE_STRING) {
				value=svalue[0]-saved_string[0];
			} else {
				value=svalue[0]-d1;
			};
			ex_vtype=VTYPE_NUM;
			return value;
 };
 if(ex_vtype==VTYPE_ARRAY) {
  	array_dat *loc_array=ex_array;
	NTOKEN2;
	d1=num_term1();
	if(ex_vtype==VTYPE_NUM) {	// subtruct numeric from array
		if(loc_array->astat==ARRAY_LOCAL) {
			loc_array=dup_array_add1(loc_array,-d1);
			ex_name="new subtruct numeric";
		} else {
			array_add1(loc_array,-d1);
			ex_name="subtruct numeric";
		};
		ex_vtype=VTYPE_ARRAY;
		ex_array=loc_array;
		return(value);
	} else if(ex_vtype==VTYPE_ARRAY) {	// array subtruction
		array_dat *loc_array2;
		loc_array2=array_sub2(loc_array,ex_array);
		ex_vtype=VTYPE_ARRAY;
		ex_array=loc_array2;
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
 // MESG("num_term2: [%s]",tok_info(tok));
 double v1 = FACTOR_FUNCTION;
 // MESG("factor function executed!");
	 while(tok->tgroup==TOK_TERM2)
	 {
		// MESG("while: TERM2");
		v1 = tok->term_function(v1);
	 };
 // MESG("term2 end");
 RTRN(v1);
}

/* multiply & divide */
double num_term1()
{
 TDS("num_term1");
 // MESG("num_term1: [%s]",tok_info(tok));
 double v1 = num_term2();
	 while(tok->tgroup==TOK_TERM1)
	 {
		// MESG("while: TERM1");
		v1 = tok->term_function(v1);
		if(err_num) break;
	 };
 RTRN(v1);
}

/* numerical only expression  */
double num_expression()
{
 double value;
 TDS("num_expression");
 // MESG(";num_expression: [%s]",tok_info(tok));
 ex_vtype=VTYPE_NUM;
 ex_value=0;
 value = num_term1();
 while(tok->tgroup==TOK_TERM) {
	// MESG("while: TERM");
	value = tok->term_function(value);
 };
 // MESG(";num_expression:end ttnum=%d return value=%f slval=[%s]",tok->tnum,value,saved_string);
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
	return ((int)value)^v2;
}

double logical_nor(double value)
{
 int v2;
	v2=(int) lexpression();
//	MESG("%d XOR %d = %d",value,v2,value^v2);
	return !(((int)value)^v2);
}

double logical_and(double value)
{
 int v2;
	v2=(int) lexpression();
	ex_vtype = VTYPE_NUM;
	return v2 && (int)value;
}

double logical_nand(double value)
{
 int v2;
	v2=(int) lexpression();
	ex_vtype = VTYPE_NUM;
	return !(v2 && (int)value);
}

double assign_option(double none)
{
double	value=lexpression();
		if(var_node->sval!=NULL) {free(var_node->sval);};
		if(ex_vtype==VTYPE_STRING){
			if(saved_string) var_node->sval=strdup(saved_string);
			else {
				err_num=2221;
				ERROR("error: Null string ",err_num);
				var_node->sval=NULL;
			};
		} else {
			if(saved_string) 
			{
				err_num=2222;
				ERROR("error: no string and not free!");
			};
			var_node->sval=NULL;
		}
		var_node->val=value;
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
	ex_vtype = VTYPE_NUM;
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
 if(ex_vtype==VTYPE_STRING) {
	static char svalue[MAXLLEN];
	 
	strlcpy(svalue,saved_string,MAXLLEN);
	num_expression();
	if(ex_vtype!=VTYPE_STRING) {
		syntax_error("string comparison error",223);
		ex_vtype=VTYPE_NUM;
		RTRN(0);	/* it is an error to compare string with number  */
	};
	ex_vtype=VTYPE_NUM;
	int lresult=scmp(svalue,saved_string);

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
	set_env(left_index,saved_string,v1);
	var_node=NULL;
	return(v1);
}


double assign_val(double none)
{
	double v1;
	tok_data *sslot;
	TDS("assign_val");
	// MESG("assign_val: lsslot=%X",(void *)lsslot);
	sslot=lsslot;
	// MESG("assign_val: ind=%d type=%d",sslot->ind,sslot->vtype);
	v1=lexpression();
	// MESG("assign_val: after lexpression!\n");
	if(sslot->vtype!=ex_vtype){ /* we should ?? consider is as an error ? */
		if(sslot->vtype==VTYPE_STRING) {
			if(sslot->sval) free(sslot->sval);
			sslot->sval=NULL;
			if(ex_vtype==VTYPE_NUM) {
				// MESG("set new as num");
				sslot->dval=v1;
				sslot->vtype=VTYPE_NUM;
				return(v1);
			};
		} else {
			if(sslot->vtype!=VTYPE_ARRAY && sslot->vtype!=VTYPE_SARRAY)	{/* added to handle arrays (v698l) but CHECK!!!!  */
				// if(is_break1) return(0);
				sslot->vtype=ex_vtype;
			};
			if(ex_vtype==VTYPE_NUM) {
				// MESG("set new as num");
				*sslot->pdval=v1;
				return(v1);
			};
			if(ex_vtype==VTYPE_STRING) {
				sslot->sval=strdup(saved_string);
				return(0);
			};
			if(ex_vtype==VTYPE_ARRAY || ex_vtype==VTYPE_SARRAY) {
				// MESG("assign array to var!");
				sslot->adat=ex_array;
				sslot->vtype=ex_array->atype;
				if(ex_array->astat==ARRAY_ALLOCATED) ex_array->astat=ARRAY_LOCAL;	/* make it local to variable  */
			};
		};
		return(v1);		
	} else {
		if(sslot->vtype==VTYPE_NUM ) {
			sslot->dval=v1;
			return(v1);
		};
		if(ex_vtype==VTYPE_ARRAY || ex_vtype==VTYPE_SARRAY) {
//				print_array1("assign array ",ex_array);
//				print_array1("to array",sslot->adat);
			if(ex_array->anum != sslot->adat->anum) {
 				if(sslot->adat->dval) free_array("assign",sslot->adat);
				if(sslot->adat->sval) {
					// MESG("free string array!");
				};
				sslot->adat=ex_array;
				if(ex_array->astat==ARRAY_ALLOCATED) ex_array->astat=ARRAY_LOCAL;	/* make it local to variable  */
			};
			return(v1);
		};
		if(sslot->vtype==VTYPE_STRING) {
			free(sslot->sval);
			sslot->sval=strdup(saved_string);
			return(v1);
		};
		return(v1);
	};
}

int assign_args1(MVAR *va,tok_data *symbols,int nargs)
{
 TDS("assign_args1");

 NTOKEN2; /* skip name */
 // MESG("assign_args1: nargs=%d",nargs);
 if(va) {
	int i;
	MESG("assign_args1: pos1 tok=[%d %s] %d",tok->tnum,tok->tname,tok->ttype);
#if	1 	/* 1 works array5, 0 works nr/f1  */
	NTOKEN2;
#endif
	// MESG("assign_args1: pos2 tok=[%d %s] %d",tok->tnum,tok->tname,tok->ttype);
	for(i=0;i<nargs;i++,va++) {
		tok_data *arg_dat=&symbols[tok->tind];
		arg_dat->vtype=va->vtype;
		MESG("assign_args1:arg %d: pos2 tok=[%d %s] %d",i,tok->tnum,tok->tname,tok->ttype);
		switch(va->vtype) {
			case VTYPE_NUM:
				arg_dat->dval=va->dval;
				// MESG("	assign_args1: dval=%f",va->dval);
				break;
			case VTYPE_STRING:
				// MESG("assign %d [%s] %X",i,va->sval,va->sval);
				arg_dat->sval=va->sval;break;
			case VTYPE_ARRAY:
			case VTYPE_SARRAY:
				arg_dat->adat=va->adat;break;
			default:
				ERROR("assign_args:[%d] type is wrong! (%d)",i,va->vtype);
				arg_dat->sval="";
				arg_dat->dval=0;
		};
		NTOKEN2;	/* skip separator or end parenthesis */
		MESG("assign_args1:arg %d: pos3 after args tok=[%s] %d",i,tok->tname,tok->ttype);
#if	!NO_LPAR
		NTOKEN2;
		if(tok->ttype==TOK_RPAR) break;
#endif
	};
 } else { // we send no arguments!
	// skip till end parenthesis setting default values for arguments!!??
	while(tok->ttype!=TOK_RPAR && tok->ttype!=TOK_END) NTOKEN2;
#if	NO_LPAR
 	// NTOKEN2;
#endif
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
 // MESG("skip_sentence:");
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
			// tok->cexpr_function=factor_funcs[tok->ttype];
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
		case TOK_RCURL:
			return;
	};
 };
}


void refresh_ddot_1(double value)
{
 offs line_end,ddot_position=0;
 int dsize=0;
 int stat=0;
 FILEBUF *old_fp; 
 FILEBUF *buf;
 TextPoint *tp;

 TDS("refresh_ddot_1");
 // MESG("refresh_ddot_1:");
 if(execmd) {
	// MESG("vtype: %d",ex_vtype);
	 if(ex_vtype==VTYPE_NUM) printf(";num: %f\n",value);
	 else if(ex_vtype==VTYPE_STRING) printf(";string: [%s]\n",saved_string);
	 else if(ex_vtype==VTYPE_ARRAY) print_array1(":",ex_array);
	 return;
 };

 int precision=bt_dval("print_precision");
 int show_hex=bt_dval("show_hex");
 char *ddot_out = (char *)malloc(128);
 tp=(TextPoint *)tok->tname;
 buf=tp->fp;

 ddot_position=tp_offset(tp);
 
 line_end=FLineEnd(buf,ddot_position);

 if(buf->b_state & FS_VIEW) return; // no refresh in view mode

 old_fp=cbfp; // keep the old buffer just in case

 sfb(buf);
 dsize=line_end-ddot_position-1;

 textpoint_set(buf->tp_current,ddot_position+1);

 if(ex_vtype==VTYPE_STRING) {	/* string value  */
// 	if(value!=0) stat=snprintf(ddot_out,128," <%5.5f>[%s]",value,saved_string);
	// else 
	stat=snprintf(ddot_out,128," \"%s\"",saved_string);
 }  else if(ex_vtype==VTYPE_NUM) {	/* numeric value  */
	long int d = (long int)value;
	if(d==value) {	/* an integer/double value!  */
		if(show_hex) stat=snprintf(ddot_out,128," %5.0f | 0x%llX | 0o%llo",value,(unsigned long long)value,(unsigned long long)value);
		else stat=snprintf(ddot_out,128," %5.*f",1,value);
	} else {	/* a decimal value!  */
		stat=snprintf(ddot_out,128," %5.*f",precision,value);
	};

 } else if(ex_vtype==VTYPE_ARRAY || ex_vtype==VTYPE_SARRAY) {
	array_dat *adat = ex_array;
 	stat=snprintf(ddot_out,128,"array %d, slot %ld type=%d rows %d,cols %d",adat->anum,lsslot-current_stable,adat->atype,adat->rows,adat->cols);
	print_array1(":",adat);
 };
 if(stat>MAXLLEN) MESG("truncated");
// replace text
 DeleteBlock(0,dsize);
 insert_string(cbfp,ddot_out,strlen(ddot_out));
 if(err_num>0) {
 	insert_string(cbfp," ,err ",6);
	if(err_str!=NULL) insert_string(cbfp,err_str,strlen(err_str));
 };
 free(ddot_out);
 sfb(old_fp);
}

double tok_dir_if()
{
 double val=0;
	int exec_else=0;

	NTOKEN2;	/* go to next token after if */
#if	!NO_LPAR
	NTOKEN2;	/* skip left parenthesis  */
#endif
	val=lexpression();

	NTOKEN2;	/* skip right parenthesis  */
	if(val) {
		val=tok->directive();
	} else {
		skip_sentence1();	/* at the begin of next blocl/sentence  */
		exec_else=1;
	}
	// check for else statement!
	if(check_skip_token1(TOK_DIR_ELSE))
	{
		if(exec_else)	{
			val=tok->directive();	/* eval else statement */
		} else {
			skip_sentence1();	/* skip else statement  */
		};
	} else {
//			NTOKEN2;
	};
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
#if	!NO_LPAR
	NTOKEN2;	/* skip left parenthesis  */
#endif
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

	if(tok->ttype==TOK_LCURL) {
		end_block=tok->match_tok; 
		end_block++;
	} else {
		skip_sentence1();
		end_block=tok;
	};

	while(!is_break1) {
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
	double *pdval;

	NTOKEN2;	/* go to next token after for */
#if	!NO_LPAR
	NTOKEN2;	/* skip left parenthesis  */
#endif
	if(tok->ttype==TOK_VAR) {
		index=&current_stable[tok->tind];
		if(index->vtype!=VTYPE_NUM) {err_num=224;ERROR("for i syntax error %d",err_num);};
		ex_vtype=index->vtype;
		NTOKEN2;
		if(tok->ttype!=TOK_ASSIGN) { err_num=225; ERROR("for i error %d",err_num);};
	} else { err_num=226;ERROR("for i syntax error %d",err_num);};

	NTOKEN2;
	dinit=num_expression();	/* initial   */

	pdval=&index->dval;
	*pdval=dinit;
	NTOKEN2;	/* skip separator! */

	dmax=num_expression();
	NTOKEN2;

	dstep=num_expression();
	// MESG("fori: from %f to %f step %f",dinit,dmax,dstep);
	if(tok->ttype!=TOK_RPAR) { err_num=226;ERROR("for i: error %d",err_num);};
	NTOKEN2;	/* skip right parenthesis  */
	// set block start
	start_block=tok;	/* this is a block start or a simple sentence  */

	if(tok->ttype==TOK_LCURL) {
		end_block=tok->match_tok; 
		end_block++;
	} else {
		skip_sentence1();
		end_block=tok;
	};
	if(dinit==dmax) {
		tok=end_block;
		current_active_flag=old_active_flag;
		return 1;
	};
	if(dstep>0 && dmax > *pdval) {
		for(;*pdval < dmax; *pdval +=dstep) {

			tok=start_block;
			tok->directive();
			if(current_active_flag==0) {
				if(is_break1) { tok=cbfp->end_token;return(0);};
				break;
			};
		};
	} else if(dstep<0 && dmax< *pdval) {
		for(; *pdval > dmax; *pdval +=dstep) {

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
#if	!NO_LPAR
	NTOKEN2;	/* skip left parenthesis  */
#endif
	check_element=tok;	/* this is the check element!  */
	skip_sentence1();	/* for now skip it  */

	NTOKEN2;	/* skip right parenthesis  */

	// this is the start of a simple sentence or a curl
	start_block=tok;
	// find the start and end of block
	if(tok->ttype==TOK_LCURL) {
		end_block=tok->match_tok;
		end_block++;
	} else {
		skip_sentence1();
		end_block=tok;
	};

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
double exec_block1()
{
 double val=0;
 stage_level=0;
 TDS("exec_block1");
 // MESG("exec_block1: starting at tok %d type=%d",tok->tnum,tok->ttype);
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
#if	0
	if(drv_check_break_key()){
		syntax_error("user interruption",100);
		if(is_break1) return 0;
	};
#endif
	// if(!current_active_flag) return(val);
 	val=tok->directive();
   };
   // MESG("exec_block1: end!");
	return(val);
}

double exec_block1_break()
{
 double val=0;
 stage_level=0;
 TDS("exec_block1");
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
 // MESG(";compute_block: %s",bp->b_fname);

 if(show_tokens) {
	parse_buffer_show_tokens(1);
	return(0);	
 };

 if(use_fp->symbol_tree==NULL) {
	// MESG("create new symbol_tree for use_fp!");
 	use_fp->symbol_tree=new_btree(use_fp->b_fname,0);
	extra=100;
	use_fp->symbol_tree->max_items=extra;
 };
	if(current_stable==NULL) {
		current_stable=new_symbol_table(use_fp->symbol_tree->max_items);
	};
	parse_block1(bp,use_fp->symbol_tree,start,extra);

	if(start) {
		local_symbols=new_symbol_table(bp->symbol_tree->max_items);
		current_stable=local_symbols;
	} else {
		local_symbols=current_stable;	
	}
 // MESG("compute_block: call check_init");
 if(bp->m_mode<2)	/* if not already checked!  */
 {
	err_num=check_init(bp);
#if	0
	if(err_num>0) 
	{
		mesg_out("Error %d %s line %d ex_vtype=%d ex_value=%f slval=[%s]!",err_num,err_str,err_line,ex_vtype,ex_value,saved_string);
		show_error("Check init",bp->b_fname);
		return(0);
	};
#endif
	// MESG("compute_block: call init_exec_flags");
	init_exec_flags();
	tok=bp->tok_table;
	// MESG("compute_block: call drv_start_checking_break");
	drv_start_checking_break();
	// MESG("exec block->");
	if(execmd) val=exec_block1();
	else val=exec_block1_break();
	drv_stop_checking_break();
	// MESG("--- start=%d",start);

	if(start) {
		if(local_symbols)
		if(bp->symbol_tree)
		delete_symbol_table(local_symbols,bp->symbol_tree->items,0);
		current_stable=old_symbol_table;
	};

	if(ex_vtype==VTYPE_STRING) msg_line("Result is \"%s\"",saved_string);
	if(ex_vtype==VTYPE_NUM) msg_line("Result is [%f]",val);
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
 if(table==NULL) return(0);
 // MESG("empty_tok_table:");
 tokdel=table;
 for(tokdel=table;tokdel->ttype!=TOK_EOF;tokdel++){
	if(tokdel->ttype==TOK_VAR || tokdel->ttype==TOK_QUOTE){
 		if(tokdel->tname!=NULL) {
			if(tokdel->ttype==TOK_QUOTE
			||tokdel->ttype==TOK_AT
			||tokdel->ttype==TOK_LETTER
			||tokdel->ttype==TOK_SHOW
			) {
			// MESG("free tname %s",tokdel->tname);
			free(tokdel->tname);
			};
		};
	};
 };
 // MESG("free the table");
 if(fp->tok_table) {
	free(table);
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
 empty_tok_table(fp);
 fp->err=-1;
 // MESG("refresh_current_buffer:[%s] %d",fp->b_fname,fp->b_type);
 parse_block1(fp,fp->symbol_tree,1,100);	/* init tree,extra 100 symbols  */

 if(err_num<1){	/* if no errors  */
	fp->symbol_table=new_symbol_table(fp->symbol_tree->max_items);
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
	val=exec_block1_break();
	drv_stop_checking_break();
	if(err_num>0) {
		show_error("refresh buffer",fp->b_fname);
		// msg_line("Error %d [%s] at line %d",err_num,err_str,err_line);
		// mesg_out("Error %d [%s] at line %d",err_num,err_str,err_line);
	} else {
		if(ex_vtype==VTYPE_STRING) msg_line("Result is \"%s\"",saved_string);
		if(ex_vtype==VTYPE_NUM) msg_line("Result is [%f]",val);
		if(saved_string) msg_line("Result is [%s %f]",saved_string,val);
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
 // MESG("parse_check_current_buffer: after chek_init");
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
	if((ind%25)==0)  out_print("Num Line Ind Level  Type               Val        Group",1);
	out_print(tok_info(tok_ind),1);
	if(tok_ind->ttype==TOK_EOF) break;
	tok_ind++;
	ind++;
 };
 
 if(err_num>0) {
	macro_exec=0;
	show_error("parse_buffer_show_token",fp->b_fname);
	// msg_line("syntax error %d line %d [%s]",err_num,err_line,err_str);
	igotolinecol(err_line+1,1,1);
 	return(0);
 } else {
 	msg_line("No errors!");
 	return(1);
 };
}

#if	1
char * tok_info(tok_struct *tok)
{
 static char stok[MAXLLEN];
	if(tok->tname!=NULL){
		if(tok->ttype==TOK_ARRAY1 || tok->ttype==TOK_ARRAY2) {
			int rows=0;
			int cols=0;
			if(tok->adat) {
				rows=tok->adat->rows;
				cols=tok->adat->cols;
			};
			snprintf(stok,MAXLLEN,"%3d:%4d %3d  %3d   [%2d=%12s] [%s] rows=%d cols=%d",tok->tnum,tok->tline,tok->tind,tok->level,tok->ttype,TNAME,(char *)tok->tname,rows,cols);
		} else 
		if(tok->ttype==TOK_SHOW) { snprintf(stok,MAXLLEN,"%3d:%4d %3d  %3d   [%2d=%12s] [:]",tok->tnum,tok->tline,tok->tind,tok->level,tok->ttype,TNAME);
		} else
		if(tok->ttype==TOK_LCURL||tok->ttype==TOK_RCURL) {
				snprintf(stok,MAXLLEN,"%3d:%4d %3d  %3d   [%2d=%12s] %s other is %d",tok->tnum,tok->tline,tok->tind,tok->level,tok->ttype,TNAME,(char *)tok->tname,tok->match_tok->tnum);
		} else
		if(tok->tgroup>0)
			snprintf(stok,MAXLLEN,"%3d:%4d %3d  %3d   [%2d=%12s] [%s] group [%d:%s]",tok->tnum,tok->tline,tok->tind,tok->level,tok->ttype,TNAME,(char *)tok->tname,tok->tgroup,tname(tok->tgroup));
		else snprintf(stok,MAXLLEN,"%3d:%4d %3d  %3d   [%2d=%12s] [%s] %f",tok->tnum,tok->tline,tok->tind,tok->level,tok->ttype,TNAME,(char *)tok->tname,tok->dval);
	} else {
		     snprintf(stok,MAXLLEN,"%3d:%4d %3d  %3d   [%2d=%12s] [%f]",tok->tnum,tok->tline,tok->tind,tok->level,tok->ttype,TNAME,tok->dval);
	};
	return stok;
}
#else
char * tok_info(tok_struct *tok)
{
 static char stok[MAXLLEN];
	if(tok->tname!=NULL){
		if(tok->ttype==TOK_ARRAY1 || tok->ttype==TOK_ARRAY2) {
			int rows=0;
			int cols=0;
			if(tok->adat) {
				rows=tok->adat->rows;
				cols=tok->adat->cols;
			};
			snprintf(stok,MAXLLEN,"%lX %3d: l=%4d i%3d  %3d   [%2d=%12s] [%s] rows=%d cols=%d",(long)tok,tok->tnum,tok->tline,tok->tind,tok->level,tok->ttype,TNAME,(char *)tok->tname,rows,cols);
		} else 
		if(tok->ttype==TOK_SHOW) { snprintf(stok,MAXLLEN,"%lX %3d: l=%4d i%3d  %3d   [%2d=%12s] [:]",(long)tok,tok->tnum,tok->tline,tok->tind,tok->level,tok->ttype,TNAME);
		} else
		if(tok->ttype==TOK_LCURL||tok->ttype==TOK_RCURL) {
				snprintf(stok,MAXLLEN,"%lX %3d: l=%4d i%3d  %3d   [%2d=%12s] %s other is %d",(long)tok,tok->tnum,tok->tline,tok->tind,tok->level,tok->ttype,TNAME,(char *)tok->tname,tok->match_tok->tnum);
		} else
		if(tok->tgroup>0)
			snprintf(stok,MAXLLEN,"%lX %3d: l=%4d i%3d  %3d   [%2d=%12s] [%s] group [%d:%s]",(long)tok,tok->tnum,tok->tline,tok->tind,tok->level,tok->ttype,TNAME,(char *)tok->tname,tok->tgroup,tname(tok->tgroup));
		else snprintf(stok,MAXLLEN,"%lX %3d: l=%4d i%3d  %3d   [%2d=%12s] [%s] %f",(long)tok,tok->tnum,tok->tline,tok->tind,tok->level,tok->ttype,TNAME,(char *)tok->tname,tok->dval);
	} else {
		     snprintf(stok,MAXLLEN,"%lX %3d: l=%4d i%3d  %3d   [%2d=%12s] [%f]",(long)tok,tok->tnum,tok->tline,tok->tind,tok->level,tok->ttype,TNAME,tok->dval);
	};
	return stok;
}
#endif

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
 ex_value=num_expression();
 // MESG("key_str1: ex_vtype=%d ex_value=%f [%s]",ex_vtype,ex_value,saved_string);
 return (saved_string);
}

double get_val()
{
	return (ex_value);
}


char *get_sval()
{
// 	MESG("get_sval: tnum=%d type=%d %f ex_value=%f",tok->tnum,tok->ttype,tok->dval,ex_value);
	if(saved_string==NULL) {
		saved_string=(char *)malloc(16);
		snprintf(saved_string,16,"%f",ex_value);
		// MESG("get_sval: new saved_string! [%s]",saved_string);
	};
	return(saved_string);
}

double next_value()
{
 double v;
 v=atof(saved_string);
 return(v);
}

int vtype_is(int type)
{
	return type==ex_vtype;
}

void set_vtype(int type)
{
	ex_vtype=type;
}

void set_sval(char *s)
{
 clean_saved_string(0);
 saved_string=strdup(s);
}

void set_dval(double value)
{
 ex_value = value;
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
			ex_value = atof(buffer);
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

/* set type,double and string values for a node named name of main table  */
void set_btval(char *name,int type,char *sval,double val)
{
 set_btsval(bt_table,type,name,sval,val);
}

/* -- */
