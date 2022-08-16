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
#define	USE_SARRAYS	1

void mesg_out(const char *fmt, ...);
extern FILEBUF *cbfp;

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

double assign_val(double none);
double assign_env(double none);
void init_error();
void get_lowercase_string(char *lower, char *string);
void get_uppercase_string(char *lower, char *string);

char * tok_info(tok_struct *tok);

char sout[MAXLLEN];	// used for messaging (msg_line), output print

TLIST ctoklist=NULL;
int is_break1=0;
int tok_mask[256];
static tok_struct *tok;	/* current token!!  */

int ex_vtype=0; 	/* type of previous expression */
int ex_edenv=0;	/* true after encount an editor env variable */
double ex_value;	// saved double value
array_dat *ex_array=NULL;
int ex_nvars=0;	/* true is there are variables in the array definition  */
int ex_nquote=0;	/* true if there are strings in the array definition  */
int ex_nums=0;	/* true if array is only numeric  */
char *ex_name=NULL;	/* variable name of the previous array  */
char slval[MAXLLEN];// saved string value
FILEBUF *ex_file=NULL;

/* error control variables  */
int err_num=0;
int drv_max_colors=8;
static int err_line=0;

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
	"increase","decrease","increase by","decrease by","backslash",
	"new line",
	"continue",
	"foreach",
	"array","array2","array3","assign env","assign option",
//	"end arg",
	"other",
	NULL
};

// function names with number of arguments
m_function m_functions[] = {
	{"strlen",1},        /* STRING LENGTH */
	{"upper$",1},        /* UPPERCASE STRING */
    {"lower$",1},        /* LOWER CASE STRING */
	{"left$",2},
	{"right$",2},
	{"mid$",3},
    {"s_asc",1},	/* CHAR TO INTEGER CONVERSION */
    {"chr$",1},		/* INTEGER TO CHAR CONVERSION */
    {"getchar",0},	/* GET 1 CHARACTER */
    {"rand",0},        	/* GET A RANDOM NUMBER */
    {"abs",1},        	/* ABSOLUTE VALUE OF A NUMBER */
    {"s_index",2},      /* FIND THE INDEX OF ONE STRING IN ANOTHER */
	{"str$",1},		/* string of a value */
	{"message",1},	/* show message on screen  */
	{"error_log",1},	/* show message error  */
	{"input",1},	/* string input from screen  */
	{"initialize",0},	/* variables init */
	{"s_val",1},	/* numerical string value  */
	{"sqrt",1},
	{"sin",1},
	{"cos",1},
	{"tan",1},
	{"atan",1},
	{"log10",1},
	{"log",1},
	{"print",1},	/* out_print, print on out buffer, and stdout if in Xwindows mode  */
	{"getpoint",0},
	{"message_wait",1},
	{"dinput",1},
	{"show_time",2},	/* show elapsed time in nanoseconds  */
	{"deq",2},	/* compare double with limited precission */
	{"cls",0},	/* clear output buffer  */
	{"DET",1},	/* determinant  */
	{"INV",1},	/* inverse  */
	{"T",1},	/* transpose  */
	{"at_bof",0},	/* if at begin of file  */
	{"at_eof",0},	/* if at end of file  */
	{"at_bol",0},	/* if at begin of line  */
	{"at_eol",0},	/* if at end of line  */
	{"at_bop",0},	/* if at begin of paragraph  */
	{"at_eop",0},	/* if at end of paragraph  */
	{NULL,0}
};

/* Function enumerator definitions */
enum {
UFLENGTH,	/* strlen */
UFUPPER,	/* upper */
UFLOWER,	/* lower */
UFLEFT,		/* left */
UFRIGHT,
UFMID,
UFASCII,
UFCHR,
UFGTKEY,
UFRND,
UFABS,
UFSINDEX,	/* s_index  */
UFSTRING,
UFMESSAGE,
UFERROR,	/* show message error  */
UFINPUT,
UFINIT,
UFVAL,
UFSQRT,
UFSIN,
UFCOS,
UFTAN,
UFATAN,
UFLOG10,
UFLOGNAT,
UFPRINT,
UFGETPOINT,
UFWAIT,
UFDINPUT,
UFSTIME,
UFDEQ,
UFCLS,
UFDETERMINANT,
UFINVERSE,
UFTRANSPOSE,
UFATBOF,
UFATEOF,
UFATBOL,
UFATEOL,
UFATBOP,
UFATEOP
};

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
 if(va) {
	 for(i=0;i<nargs;i++){
		if(va[i].vtype>VTYPE_NUM){
	 		free(va[i].sval);
		}
	 };
	 free(va);
 };
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
	return 0.0;
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
	return(v0);
}

double increase_val()
{
 double v0;
	TDS("increase_val");
	ex_vtype=VTYPE_NUM;
	v0=add_value(1.0);
	NTOKEN2;
	return(v0);
}

double decrease_val()
{
	TDS("decrease_val");
	ex_vtype=VTYPE_NUM;
	add_value(-1.0);
	NTOKEN2;
	return(lsslot->dval);
}

double decrease_by()
{
	double v1,v0;
	tok_data *sslot;
	TDS("increase_val");
	sslot=lsslot;
	// MESG("increase_val: ind=%d type=%d",sslot->ind,sslot->vtype);
	NTOKEN2;
	v1=lexpression();
	// MESG("increase by %f",v1);
	if(sslot->vtype==VTYPE_NUM) {
		v0=sslot->dval;
		sslot->dval =v0-v1;
		// MESG("num: %f -> %f",v0,v1);
		return(v0);
	};
	if(sslot->vtype==VTYPE_ARRAY) {
		v0=*sslot->pdval;
		
		*sslot->pdval = v0-v1;
		// MESG("array: %f -> %f",v0,*sslot->pdval);
		return(v0);
	};
	return(-1);
}

double increase_by()
{
	double v1,v0;
	tok_data *sslot;
	TDS("increase_val");
	sslot=lsslot;
	// MESG("increase_val: ind=%d type=%d",sslot->ind,sslot->vtype);
	NTOKEN2;
	v1=lexpression();
	// MESG("increase by %f",v1);
	if(sslot->vtype==VTYPE_NUM) {
		v0=sslot->dval;
		sslot->dval = v0+v1;
		// MESG("num: %f -> %f",v0,v1);
		return(v0);
	};
	if(sslot->vtype==VTYPE_ARRAY) {
		v0=*sslot->pdval;
		
		*sslot->pdval = v0+v1;
		// MESG("array: %f -> %f",v0,*sslot->pdval);
		return(v0);
	};
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
 tok->tname=NULL;
 tok->dval=0;
 tok->ttype=0;
 tok->tgroup=0;
 tok->factor_function=factor_none;
 tok->tnode=NULL;
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
 	if((ind>='a' && ind<='z') ||(ind>='A' && ind<='Z') || ind=='_' || ind>=128 || ind=='$' || ind=='.' ) 
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
 }

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
}


/* test if a file is macro lanuage ...  */
int is_mlang(FILEBUF *fp)
{
 int bt=fp->b_type;
 char st[256];
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
 	snprintf(st,256,"Not an mlang file bt=%d",bt);
 	msg_line(st);
	return 0;	/* only allowed in selected types  */
 };

 if(!strcmp("[out]",fp->b_fname)) {
 	msg_line("cannot check [out] buffer!!");
	return 0;
 };

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
	td[i].dval=0;
	td[i].sval=NULL;
 };
 return td;
}

/* free symbol table after execute */
void delete_symbol_table(tok_data *td, int size)
{
 int i;
 tok_data *sslot;

 if(td) {
 for(i=0;i<size;i++) {
 	sslot=&td[i];
	if(sslot->vtype>VTYPE_NUM)
 	if(sslot->sval!=NULL) {
		free(sslot->sval);
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
 err_line=0;
 err_num=0;
 err_str=NULL;
}

void init_exec_flags()
{
 init_error();
 is_break1=0;
 current_active_flag=1;
 ex_vtype=0;
 ex_value=0.0;
 var_node=NULL;
 pnum=0;
 stage_level=0;
 ex_nvars=0;ex_nquote=0;ex_nums=0;	/* initialize table counters  */
}

void show_error(char *from)
{
 int var_index=-1;
 if(tok) if(tok->tnode) var_index=tok->tnode->node_index;
 if(var_index>=0)
	ERROR("%s error %d after function [%s] at line %d: [%s]",from,err_num,ftable[var_index].n_name,err_line,err_str);
 else {
	// MESG("var_index=%d",var_index);
	ERROR("%s error %d at line %d: [%s]",from,err_num,err_line,err_str);
 };
}

/* Check for any errors and initialize parsed list  */
int check_init(FILEBUF *bf)
{
 tok_struct *tok_table=bf->tok_table;
 int err=0;
 stage_level=0;
 if(tok_table==NULL) 
 {
	parse_block1(bf,NULL,1,0);
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

	switch(ex_vtype) {
		case VTYPE_NUM:
			va_i->dval=value;
			va_i->vtype=VTYPE_NUM;
			break;
		case VTYPE_STRING:
			va_i->sval=strdup(slval);
			break;
		case VTYPE_ARRAY:
		case VTYPE_SARRAY:
			va_i->adat=ex_array;
			va_i->vtype=ex_array->atype;
			break;
		default:
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

/*	evaluate an internal function 
	tlist is at the start of token list
*/
double eval_fun1(int fnum)
{
	static double value;
	double vv[3];
	char arg[3][MAXLLEN];
	int i,ia;
	int f_entry;
	int stat=0;
	array_dat *arr=NULL;
	TDS("eval_fun1");

	ia=m_functions[fnum].f_args;
	
	f_entry=entry_mode;
	entry_mode=KNORMAL;

	if(ia) {
		/* if we have arguments, check for parenthesis, then get the arguments  */
		for(i=0;i< ia;i++) { 
			NTOKEN2;
			vv[i] = num_expression();
			if(ex_vtype==VTYPE_STRING) strlcpy(arg[i],slval,MAXLLEN);
			else if (ex_vtype==VTYPE_ARRAY||ex_vtype==VTYPE_SARRAY) arr=ex_array;
			else arg[i][0]=0;
		};
		NTOKEN2;
	} else {;
		arg[0][0]=arg[1][0]=arg[2][0]=0;
		if(tok->ttype==TOK_LPAR) {
				NTOKEN2;
				NTOKEN2;
		};
	};

	slval[0]=0;
	value=0.0;
	entry_mode=f_entry;
	/* and now evaluate it! */
	switch (fnum) {
		case UFDETERMINANT:
				if(ex_vtype==VTYPE_ARRAY) {
					if(arr->rows==arr->cols) value=determinant(arr);
					else {
						syntax_error("wrong dimensions for determinant",203);
						value=0;
					};
				} else {
					syntax_error("Not an array!",204);
					value=0;
				};
				ex_vtype=VTYPE_NUM;
				break;
		case UFINVERSE:{
				if(ex_vtype==VTYPE_ARRAY) {
					if(arr->rows==arr->cols) {
						value=determinant(arr);
					} else {
						syntax_error("wrong dimensions for determinant",205);
						value=0;
					};
					if(value!=0) {
						array_dat *inverse;
						inverse=cofactor2(arr,value);
						ex_array=inverse;
						ex_name="Inverse";
					};
				} else {
					syntax_error("Not an array!",206);
					value=0;
				}
			};break;
		case UFTRANSPOSE:
				if(ex_vtype==VTYPE_ARRAY) {
					array_dat *tarray;
					tarray = transpose(ex_array);
					ex_array=tarray;
					ex_name="Tranpose";
				} else {
					syntax_error("Not an array!",207);
					value=0;
				};
				break;
		case UFLENGTH:	value =strlen(arg[0]);ex_vtype=VTYPE_NUM;break;
		case UFLEFT:	strlcpy(slval,arg[0],MAXLLEN);
				if(vv[1]>0 && vv[1]<strlen(arg[0])) slval[(int)vv[1]]=0;
				ex_vtype=VTYPE_STRING;
				value = atof(slval);
				break;
		case UFRIGHT:	strlcpy(slval, &arg[0][strlen(arg[0])-(int)vv[1]],MAXLLEN);ex_vtype=VTYPE_STRING;
			break;
		case UFMID:	strlcpy(slval, &arg[0][(int)vv[1]-1],MAXLLEN);
				slval[(int)vv[2]]=0;
				ex_vtype=VTYPE_STRING;
				value = atof(slval);
				break;
		case UFUPPER:{
				get_uppercase_string(slval,arg[0]);
				ex_vtype=VTYPE_STRING;
				};
				break;
		case UFLOWER:{
				get_lowercase_string(slval,arg[0]);
				ex_vtype=VTYPE_STRING;
				};
				break;
		case UFASCII:	value=arg[0][0];ex_vtype=VTYPE_NUM;
				break;
		case UFCHR:	slval[0] = (int)vv[0];
				slval[1] = 0;
				value = vv[0];
				ex_vtype=VTYPE_STRING;
				break;
		case UFGTKEY:
				if(execmd) {
					slval[0]=getchar();
				} else {
					slval[0] = getcmd();
				};
				slval[1] = 0;
				ex_vtype=VTYPE_STRING;
				break;
		case UFRND:	/* returns big! int */
				value = rand();ex_vtype=VTYPE_NUM;break;
		case UFABS:	value = fabs(vv[0]);ex_vtype=VTYPE_NUM;break;
		case UFSINDEX:	/* segmentation */
				value = sindex(arg[0], arg[1]);
				ex_vtype=VTYPE_NUM;
				break;
		case UFSTRING:	/* string of a value */
				snprintf(slval,MAXLLEN,"%f",vv[0]);
				ex_vtype=VTYPE_STRING;
				value = vv[0];
				break;
		case UFMESSAGE: if(arg[0][0] !=0 ) stat=snprintf(sout,MAXLLEN,"[%s]",arg[0]);
				else stat=snprintf(sout,MAXLLEN,"<%f>",vv[0]);
				if(strlen(sout)>80) sout[79]=0;
				msg_line(sout);
				events_flush();
				break;
		case UFERROR: if(arg[0][0] !=0 ) stat=snprintf(sout,MAXLLEN,"[%s]",arg[0]);
				else stat=snprintf(sout,MAXLLEN,"<%f>",vv[0]);
				if(strlen(sout)>80) sout[79]=0;
				error_line(sout);
				events_flush();
				break;
		case UFWAIT: 
				if(arg[0][0] !=0 ) stat=snprintf(sout,MAXLLEN,"[%s] waiting.. ",arg[0]);
				else stat=snprintf(sout,MAXLLEN,"<%f> wait for key",vv[0]);
				if(execmd) {
					printf("%s",sout);
					slval[0]=getc(stdin);
				} else {
					if(strlen(sout)>80) sout[79]=0;
					msg_line(sout);
					events_flush();
					entry_mode=KENTRY;
					slval[0] = getcmd();
					entry_mode=KNORMAL;
				};
				slval[1] = 0;
				ex_value=slval[0];
				ex_vtype=VTYPE_STRING;
				break;
		case UFINPUT:   
				if(arg[0][0]==0) strlcpy(sout,"Input :",MAXLLEN);
				else strlcpy(sout,arg[0],MAXLLEN);
				entry_mode=KENTRY;	/* get input from screen */
				getstring(sout,slval,80,true);
				if(execmd) slval[strlen(slval)-1]=0;
				ex_vtype=VTYPE_STRING;
				value=0;
				break;
		case UFDINPUT:
				if(arg[0][0]==0) strlcpy(sout,"DInput :",MAXLLEN);else strlcpy(sout,arg[0],MAXLLEN);
//				entry_mode=KENTRY;	/* get input from the screen */
				getstring(sout,slval,80,true);
				value=atof(slval);ex_vtype=VTYPE_NUM;
				slval[0]=0;
				break;
		case UFINIT:	initialize_vars();
				break;
		case UFVAL:	value=atof(arg[0]);ex_vtype=VTYPE_NUM;
				break;
		case UFSQRT: value=sqrt(vv[0]);ex_vtype=VTYPE_NUM;
				break;
		case UFSIN: 
				value=sin(vv[0]);ex_vtype=VTYPE_NUM;
				break;
		case UFCOS: value=cos(vv[0]);ex_vtype=VTYPE_NUM;
				break;
		case UFTAN: value=tan(vv[0]);ex_vtype=VTYPE_NUM;
				break;
		case UFLOG10: 
				value=log10(vv[0]);
				ex_vtype=VTYPE_NUM;
				break;
		case UFATAN:
				 value=atan(vv[0]);ex_vtype=VTYPE_NUM;
				break;
		case UFLOGNAT:
				value=log(vv[0]);
				ex_vtype=VTYPE_NUM;
				break;
		case UFPRINT:	// to stdio, be carefull CHECK !!!!
				// MESG("UFPRINT: arr=%d",arr);
				if(arr) print_array1("",arr);
				else {
				if(arg[0][0] !=0 ) {
					if(strlen(arg[0])>MAXLLEN-1) arg[0][MAXLLEN-1]=0;
					strlcpy(sout,arg[0],MAXLLEN);
					ex_vtype=VTYPE_STRING;
					strlcpy(slval,arg[0],MAXLLEN);
					if(xwin && !execmd) MESG(slval);
				} else {
					snprintf(sout,MAXLLEN,": %f",vv[0]); 
					ex_vtype=VTYPE_NUM;
					value=vv[0];
					if(xwin && !execmd) MESG(": %f",vv[0]);
				};
				out_print(sout,1);
				};
				break;
		case UFGETPOINT:
				break;
		case UFSTIME:
			strlcpy(slval,arg[0],MAXLLEN);
			value=show_time(slval,vv[1]);
			ex_vtype=VTYPE_NUM;
			break;
		case UFDEQ:
			value=deq(vv[0],vv[1]);
			ex_vtype=VTYPE_NUM;
			break;
		case UFCLS:
			cls_fout("[out]");
			break;
		case UFATBOF:
			return(FBof(cbfp));
		case UFATEOF:
			return(FEof(cbfp));
		case UFATBOL:
			return(FBolAt(cbfp,Offset()));
		case UFATEOL:
			return(FEolAt(cbfp,Offset()));
		default:
			value=0.0;
			ex_vtype=VTYPE_NUM;
	};
	if(stat>MAXLLEN) MESG("truncated string eval function");
	RTRN(value);
}


double exec_function(FILEBUF *bp,MVAR *vargs,int nargs)
{
	double value=0;
	static long level=0;
	level++;

	tok_data *old_symbol_table=current_stable;

	tok=bp->tok_table;	/* start of function  */

	current_stable=new_symbol_table(bp->symbol_tree->items);	/* create new symbol table  */

	if(current_stable==NULL) { 
		err_num=208;
		ERROR("cannot create new symbol table! proc level is %ld",level);
		is_break1=1;
		level=0;
		return 0;
	};

	assign_args1(vargs,current_stable,nargs);
	value=exec_sentence1();

	/* remove local variable tree and restore the old one  */
	delete_symbol_table(current_stable,bp->symbol_tree->items);
	current_stable=old_symbol_table;

	clear_args(vargs,0);// allocated args already cleared above in delete_symbol_table!
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
	while(1) {
		if(tok->ttype==TOK_SEP) { 
			NTOKEN2;
			continue;
		};
		value=num_expression();
		if(adat->atype==VTYPE_ARRAY){
			if(adat->rows>1 && adat->cols>1) {
				adat->dval2[j][i]=value;
			} else {
				if(adat->cols>1) adat->dval[i]=value;
				else adat->dval[j]=value;
			};
		};
		if(adat->atype==VTYPE_SARRAY) {
			// MESG("	add row %d col=%d [%s]",i,j,slval);
			if(slval!=NULL)	adat->sval[cols*j+i]=strdup(slval);
		};
		i++;if(i>cols) cols=i;
		if(tok->ttype==TOK_SHOW || tok->ttype==TOK_RBRAKET) {
			cdim=0;break;
		};
		if(tok->ttype==TOK_COMMA) {
			i=0;
			j++;
			cdim++;if(cdim>rows) rows=cdim;
			NTOKEN2;};
	};cdim--;
	};
	ex_array->astat=1;
	ex_vtype=adat->atype;
	print_array1("",adat);
	NTOKEN2;
	RTRN(1.2);
}

double factor_variable()
{
	lsslot=&current_stable[tok->tind];	/* symbol table variable's slot  */
	ex_vtype=lsslot->vtype;
	// MESG("	factor_variable: ind=%d type=%d",lsslot->ind,lsslot->vtype);
	switch(lsslot->vtype) {
		case VTYPE_NUM: 
			NTOKEN2;
			RTRN(lsslot->dval);
		case VTYPE_STRING:
			strlcpy(slval,lsslot->sval,MAXLLEN);
			NTOKEN2;
			RTRN(lsslot->dval);
		case VTYPE_ARRAY:
		case VTYPE_SARRAY:
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
	int ind1,ind2;
	double **dval2;
	double value=0;
	tok_data *array_slot;
	array_dat *adat;
	array_slot=&current_stable[tok->tind];
	adat=array_slot->adat;
	ex_vtype=VTYPE_NUM;
	if(adat!=NULL) {
		dval2 = adat->dval2;
		NTOKEN2;
		ind1=(int)FACTOR_FUNCTION;
		ind2=(int)FACTOR_FUNCTION;
		value=dval2[ind1][ind2];
		array_slot->pdval=&dval2[ind1][ind2];
	} else {
		syntax_error("no data in the array!!",209);
		NTOKEN2;
	}
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
		strlcpy(slval,bte->sval,MAXLLEN);
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
	array_slot=&current_stable[tok->tind];
	// MESG("factor_array1:ind=%d ",array_slot->ind);
	NTOKEN2;
	ind1=(int)FACTOR_FUNCTION;
	// MESG("	ind1=%d",ind1);
	if(array_slot->adat == NULL) {
		ex_nums=1;
		array_slot->adat=new_array(ind1+1,1);
		array_slot->vtype=VTYPE_ARRAY;
		allocate_array(array_slot->adat);	/*   */
	} else {
		
	};
	// MESG("-----");
	dval = array_slot->adat->dval;
	// MESG("	ind1=%d value=%f",ind1,value);
	value=dval[ind1];
		array_slot->pdval=&dval[ind1];
		lsslot=array_slot;
	ex_vtype=VTYPE_NUM;
	// MESG("factor_array1:ind1=%d lsslot ind=%d type=%d end!",ind1,lsslot->ind,lsslot->vtype);
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
	
	var_index = tok->tnode->node_index;
	ed_command = ftable+var_index;
	NTOKEN2;
	save_macro_exec=macro_exec;
	macro_exec=MACRO_MODE2;

	if(ed_command->arg) {
		if(ed_command->arg>2 || ed_command->arg<0) {
			NTOKEN2;	/* skip parenthesis  */
			return ed_command->n_func(1);
		};
		check_par=1;	/* we need parenthesis if arguments.  */
		NTOKEN2;

		value=num_expression();
		ex_value=value;
		switch(ed_command->arg) {
			case 1:{ /* one argument */
				if(ex_vtype==VTYPE_STRING) { value=1;};
				break;
			};
			case 2:	/* two arguments  */
			{
				NTOKEN2;
				value=1;
				break;
			};
		};

	} else {	/* no argument  */
		if(tok->ttype==TOK_LPAR) {	/* we can have parenthesis or not. */
			NTOKEN2;
			check_par=1;
		};
		value=1;
	};

	macro_exec = MACRO_MODE2;

	err_num=0;
	err_line=tok->tline;
	err_str=NULL;

	status=ed_command->n_func((int)value);
//	MESG("TOC_CMD: status=%d",status);
	ex_value=status;
//	editor command returns a numeric value
	slval[0]=0;
	ex_vtype=VTYPE_NUM;

	macro_exec = save_macro_exec;

	if(check_par) { NTOKEN2;};

	if(err_num>0) {
		// ERROR("error %d after function [%s] at line %d: %s",err_num,ftable[var_index].n_name,err_line,err_str);
		show_error("Factor");
		RTRN(status);
	};
//	MESG("factor_cmd: value=%f ex_value=%f",value,ex_value);
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
	ex_vtype=VTYPE_STRING;
	strlcpy(slval,(char *)tok->tname,MAXLLEN);
	NTOKEN2;
	RTRN(0);		/* 0 value for string variables  */
}

double factor_bquote()
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
	NTOKEN2;
	NTOKEN2;	/* this is left parenthesis */
	/* function */
	MVAR *vargs = NULL;
	vargs = push_args_1(tok0->tind);

	after_proc=tok;
	value=exec_function(tok0->tbuf,vargs,tok0->tind);
	tok=after_proc;

	current_active_flag=1;	/* start checking again  */

	RTRN(value);
}

double factor_func()
{
	BTNODE *bte; 
	double value;

	ex_vtype=VTYPE_NUM;
	bte=tok->tnode;
	NTOKEN2;
	value = eval_fun1(bte->node_index);
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
			if(loc_array->astat==1) loc_array=dup_array_power(loc_array,v2);
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
			if(loc_array->astat==1) loc_array=dup_array_mod1(loc_array,v2);
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
				ex_name = "Multiply to numeric";
				ex_vtype=VTYPE_ARRAY;
				return 1;
		};
	};
	if(ex_vtype==VTYPE_ARRAY){
		array_dat *loc_array = ex_array;
		NTOKEN2;
		v2 = num_term2();
		if(ex_vtype==VTYPE_NUM) {
			if(loc_array->astat==1) loc_array=dup_array_mul1(loc_array,v2);
			else array_mul1(loc_array,v2);
			ex_array=loc_array;
			ex_vtype=VTYPE_ARRAY;
			ex_name="array multiply numeric";
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
					// MESG("v1=%f",v1);
					ex_vtype=VTYPE_NUM;
					// free old ex_array ???
					ex_array=NULL;
					ex_name="--";
					RTRN(v1);
				};
				syntax_error("array error",213);
				ex_vtype=VTYPE_NUM;
				RTRN(v1);
			} else {
				array_dat *loc_array2;
				loc_array2=array_mul2(loc_array,ex_array);
				ex_vtype=VTYPE_ARRAY;
				
				if(loc_array->astat==3) {	/* free this one!!  */
				};
				ex_array=loc_array2;
				ex_name="Multiply to array";
				RTRN(v1);
			};
			return v1;
		};
	};
	syntax_error("wrong multiply ",2101);
	set_break();
	return(1);
}

static double term1_div(double v1)
{
 double v2;
	if(ex_vtype==VTYPE_NUM){
//		MESG("term1_div:1 [%s]",tok_info(tok));
		NTOKEN2;
		v2=num_term2();
//		MESG("term1_div:2 [%s] v2=%f",tok_info(tok),v2);
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
					current_active_flag=0;
					RTRN(v1);
				case VTYPE_ARRAY:	// numeric * array
					ex_array = dup_array_mul1(ex_array,1/v1);
					ex_name = "Multiply to numeric";
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
			current_active_flag=0;
			RTRN(v1);
		};

		if(ex_vtype==VTYPE_NUM) {
			if(loc_array->astat==1) loc_array=dup_array_mul1(loc_array,1/v2);
			else array_mul1(loc_array,1/v2);
			ex_array=loc_array;
			ex_vtype=VTYPE_ARRAY;
			ex_name="array multiply numeric";
			return 1;
		};
		if(ex_vtype==VTYPE_ARRAY) {
				ERROR("div arrays not implemmented yet!");
				RTRN(v1);
		};
	};
	err_num=216;
	err_line=tok->tline;
	set_break();
	ERROR("division operation not_supported err %d",err_num);
	RTRN(v1);
}

FFunction factor_funcs[] = {
	factor_none,	// TOK_NONE
	factor_none,	// TOK_SEP
	factor_none,	// TOK_SPACE
	factor_none,	// TOK_LETTER
	
	factor_none,	// TOK_LCURL	,
	factor_none,	// TOK_RCURL	,
	factor_quote,	// TOK_QUOTE
	factor_lpar,	// TOK_LPAR
	factor_error,	// TOK_RPAR	
	factor_none,	// TOK_SHOW
	factor_none,	// TOK_COMMENT	,
	factor_variable,	// TOK_VAR	level 0 variable
	factor_option,	// TOK_OPTION	,	// editor option
	factor_cmd,		// TOK_CMD		,	// editor commands
	factor_func,	// TOK_FUNC	,	// function
	factor_proc,	// TOK_PROC	,
	factor_env,		// TOK_ENV		,	// editor environment function
	factor_none,	// TOK_TERM0	term0 group
	factor_none,	// TOK_TERM	,	// term operators (+,-)
	factor_none,	// TOK_TERM1	,	// term1 operators (%,^)
	factor_none,	// TOK_TERM2	,	// term2 operators (*,/)
	factor_none,	// TOK_ASSIGN	,	// assignment
	factor_none,	// TOK_EOF		,	// end of file token
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
	factor_none,	// TOK_AT			,
	factor_none,	// TOK_RANGE		,
	factor_bquote,	// TOK_BQUOTE
	factor_none,	// TOK_DOLAR		,
	factor_none,	// TOK_TILDA		,
	increase_val,	// TOK_INCREASE	,
	decrease_val,	// TOK_DECREASE	,
	increase_by,	// TOK_INCREASEBY
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


double term_plus(double value)
{
 double d1;
 if(ex_vtype==VTYPE_NUM) {
	NTOKEN2;
	d1=num_term1();
		switch(ex_vtype) {
			case VTYPE_NUM: {	/* numeric addition  */
				return (value+d1);
				};
			case VTYPE_STRING: {	/* numeric + string  */
				char svalue[MAXLLEN];
				int stat;
				stat=snprintf(svalue,MAXLLEN,"%f%s",value,slval);
				if(stat>MAXLLEN) MESG("truncated 2");
				strlcpy(slval,svalue,MAXLLEN);
				return 0;
				};
			case VTYPE_ARRAY: { // num + array
				if(ex_array->astat==1) {
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
			default: {
				err_num=217;
				err_line=tok->tline;
				ERROR("operation not supported err %d",err_num);
				RTRN(0);
			};
		};
 };
 if(ex_vtype==VTYPE_STRING) {	// set local value
	char svalue[MAXLLEN];
	strlcpy(svalue,slval,MAXLLEN);

		NTOKEN2;
		d1=num_term1();
		 /* catanate string */
			if(ex_vtype==VTYPE_STRING) {	/* string catanate  */
				strcat(svalue,slval);
				value=atof(svalue);
				strlcpy(slval,svalue,MAXLLEN);
				return value;
			};
			if(ex_vtype==VTYPE_NUM)	{	/* string, numeric catanate  */
				long l0;
				ex_vtype=VTYPE_STRING;
				l0=d1;
				if(l0==d1)snprintf(slval,MAXLLEN,"%ld",l0);
				else snprintf(slval,MAXLLEN,"%f",d1);
				strcat(svalue,slval);
				value=0;
				strlcpy(slval,svalue,MAXLLEN);
				return value;
			};
			if(ex_vtype==VTYPE_ARRAY) {
				err_num=219;
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
					if(loc_array->astat==1) {
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
	switch(ex_vtype) {
	case VTYPE_NUM: { 
		return (value-d1);
		};
	case VTYPE_ARRAY: {
		if(ex_array->astat==1) {
			ex_array=dup_array_sub1(ex_array,value);
			ex_name="New array,subtract from numeric";
		} else {
			array_sub1(ex_array,value);
			ex_name="Subtract from numeric";
		};
		ex_vtype = VTYPE_ARRAY;
		return 0;
		};
	default:
		err_num=218;
		err_line=tok->tline;
		ERROR("operation not supported err %d",err_num);
		RTRN(value);
	}
 };
 if(ex_vtype==VTYPE_STRING) {	// set local value
	char svalue[MAXLLEN];
	strlcpy(svalue,slval,MAXLLEN);
		NTOKEN2;
		// operator on first chars of strings. numeric result
			d1=num_term1();
			if(ex_vtype==VTYPE_STRING) {
				value=svalue[0]-slval[0];
				slval[0]=0;
			} else {
				value=svalue[0]-d1;
				slval[0]=0;
			};
			ex_vtype=VTYPE_NUM;
			return value;
 };
 if(ex_vtype==VTYPE_ARRAY) {
  	array_dat *loc_array=ex_array;
	NTOKEN2;
	d1=num_term1();
	if(ex_vtype==VTYPE_NUM) {	// subtruct numeric from array
		if(loc_array->astat==1) {
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
 double v1 = FACTOR_FUNCTION;
	 while(tok->tgroup==TOK_TERM2)
	 {
		v1 = tok->term_function(v1);
	 };
 RTRN(v1);
}

/* multiply & divide */
double num_term1()
{
 TDS("num_term1");
 double v1 = num_term2();
	 while(tok->tgroup==TOK_TERM1)
	 {
		v1 = tok->term_function(v1);
	 };
 RTRN(v1);
}

/* numerical only expression  */
double num_expression()
{
 double value;
 TDS("num_expression");
 ex_vtype=VTYPE_NUM;
 ex_value=0;
 slval[0]=0;
 value = num_term1();
 while(tok->tgroup==TOK_TERM) {
	value = tok->term_function(value);
 };
 return value;
}

double expression(char *title)
{
	return num_expression();
}

double cexpression();

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
			if(slval[0]!=0) var_node->sval=strdup(slval);
			else {
				err_num=2221;
				ERROR("error: Null string ",err_num);
				var_node->sval=NULL;
			};
		} else {
			if(slval[0]!=0) {
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
 value = cexpression();
// MESG("lexpression : [%s] cexpression result = %f",tok_info(tok),value);
 if(tok->tgroup == TOK_TERM0){
	tok_struct *tok0=tok;
 	NTOKEN2;
	RTRN(tok0->term_function(value));
 };
 while(tok->tgroup==TOK_BOOL){
	tok_struct *tok0=tok;
//	MESG("lexpression in loop!: [%s] value=%f",tok_info(tok),value);
 	NTOKEN2;
	value =tok0->term_function(value);
//	MESG("lexression result is %f",value);
	ex_vtype = VTYPE_NUM;
 };
//	MESG("lexpression return value %f",value);	
	RTRN(value);
}

double cexpression()
{
 double value;
 tok_struct *tok0;
 TDS("cexpression");
 value = num_expression();

 if(tok->tgroup!=TOK_COMPARE) RTRN(value);
 tok0=tok;
 NTOKEN2;
 if(ex_vtype==VTYPE_STRING) {
	static char svalue[MAXLLEN];
	 
	strlcpy(svalue,slval,MAXLLEN);
	num_expression();
	if(ex_vtype!=VTYPE_STRING) {
		syntax_error("string comparison error",223);
		ex_vtype=VTYPE_NUM;
		RTRN(0);	/* it is an error to compare string with number  */
	};
	ex_vtype=VTYPE_NUM;
	int lresult=scmp(svalue,slval);
	strcpy(slval,"");
	RTRN(tok0->cexpr_function(lresult,0));
 } else {
	double v2;
	v2=num_expression();
	RTRN(tok0->cexpr_function(value,v2));
 }
}

double assign_env(double none)
{
	double v1;
	int left_index;
	TDS("assign_env");
	left_index=var_node->node_index;
	v1=lexpression();
	set_env(left_index,slval,v1);
	var_node=NULL;
	return(v1);
}


double assign_val(double none)
{
	double v1;
	tok_data *sslot;
	TDS("assign_val");
	sslot=lsslot;
	// MESG("assign_val: ind=%d type=%d",sslot->ind,sslot->vtype);
	v1=lexpression();
	if(sslot->vtype!=ex_vtype){
		if(sslot->vtype==VTYPE_STRING) {
			free(sslot->sval);
		} else {
			if(sslot->vtype!=VTYPE_ARRAY && sslot->vtype!=VTYPE_SARRAY)	/* added to handle arrays (v698l) but CHECK!!!!  */
				sslot->vtype=ex_vtype;
			if(ex_vtype==VTYPE_NUM) {
				*sslot->pdval=v1;
				return(v1);
			};
			if(ex_vtype==VTYPE_STRING) {
				sslot->sval=strdup(slval);
				return(0);
			};
			if(ex_vtype==VTYPE_ARRAY || ex_vtype==VTYPE_SARRAY) {
				// MESG("assign array to var!");
				sslot->adat=ex_array;
				sslot->vtype=ex_array->atype;
				if(ex_array->astat==3) ex_array->astat=1;	/* make it local to variable  */
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
				if(ex_array->astat==3) ex_array->astat=1;	/* make it local to variable  */
			};
			return(v1);
		};
		if(sslot->vtype==VTYPE_STRING) {
			free(sslot->sval);
			sslot->sval=strdup(slval);
			return(v1);
		};
		return(v1);
	};
}

int assign_args1(MVAR *va,tok_data *symbols,int nargs)
{
 int i=0;
 TDS("assign_args1");

 NTOKEN2; /* skip name */

 if(va) {
	
	NTOKEN2;
	for(i=0;i<nargs;i++,va++) {
		tok_data *arg_dat=&symbols[tok->tind];
		arg_dat->vtype=va->vtype;
		switch(va->vtype) {
			case VTYPE_NUM:
				arg_dat->dval=va->dval;break;
			case VTYPE_STRING:
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

		if(tok->ttype==TOK_RPAR) break;
		NTOKEN2;
	};
 } else { // we send no arguments!
	// skip till end parenthesis setting default values for arguments!!??
	i=0;

	if(tok->ttype==TOK_LPAR) NTOKEN2;
	do {
		if(tok->ttype==TOK_RPAR) break;
		NTOKEN2;	/* skip semicolon or end parenthesis */

		if(tok->ttype==TOK_RPAR) break;
		NTOKEN2;
		i++;
	} while(1) ;
 };
 NTOKEN2;
// show_token(tok,"assign_args: end after assign %d args",i);
 return(1);
}

#include "mlang_parser.c"

// skip next sentence in a list
void skip_sentence1()
{
 int plevel=0;
 TDS("skip_sentence1");

 if(tok->ttype==TOK_LCURL) {
		tok=tok->match_tok; 
		NTOKEN2;
		return; 
 };

 for(;tok->ttype!=TOK_EOF;NTOKEN2)
 {
	switch(tok->ttype) {
		case TOK_DIR_ELSE:	/* this one starts a new sentence!!  */
		case TOK_SEP:
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

 if(!discmd) return;
 // MESG("refresh_ddot:");
 int precision=bt_dval("print_precision");
 int show_hex=bt_dval("show_hex");
 
 tp=(TextPoint *)tok->tname;
 buf=tp->fp;

 ddot_position=tp_offset(tp);
 
 line_end=FLineEnd(buf,ddot_position);

 if(buf->b_state & FS_VIEW) return; // no refresh in view mode

 old_fp=cbfp; // keep the old buffer just in case

 sfb(buf);
 dsize=line_end-ddot_position-1;

 textpoint_set(buf->tp_current,ddot_position+1);
#if	1	/* check , remove!  */
 if(slval[0]!=0 && ex_vtype!=VTYPE_STRING) {
 	ERROR("ddot:error: vtype=%d slval=[%s]",ex_vtype,slval);
 };
#endif
 if(ex_vtype==VTYPE_STRING) {	/* string value  */
 	if(strlen(slval)>MAXSLEN-20) slval[MAXSLEN-20]=0;
	if(value!=0) stat=snprintf(sout,MAXLLEN," <%5.5f>[%s]",value,slval);
	else stat=snprintf(sout,MAXLLEN," \"%s\"",slval);
	
 }  else if(ex_vtype==VTYPE_NUM) {	/* numeric value  */
	long int d = (long int)value;
	if(d==value) {	/* an integer/double value!  */
		if(show_hex) stat=snprintf(sout,MAXLLEN," %5.0f | 0x%llX | 0o%llo",value,(unsigned long long)value,(unsigned long long)value);
		else stat=snprintf(sout,MAXLLEN," %5.*f",1,value);
	} else {	/* a decimal value!  */
		stat=snprintf(sout,MAXLLEN," %5.*f",precision,value);
	};
 } else if(ex_vtype==VTYPE_ARRAY || ex_vtype==VTYPE_SARRAY) {
	array_dat *adat = ex_array;
 	stat=snprintf(sout,MAXLLEN,"array %d, slot %ld rows %d,cols %d",adat->anum,lsslot-current_stable,adat->rows,adat->cols);
	print_array1(":",adat);
 };
 if(stat>MAXLLEN) MESG("truncated");
// replace text
 DeleteBlock(0,dsize);
 insert_string(cbfp,sout,strlen(sout));
 if(err_num>0) {
 	insert_string(cbfp," ,err ",6);
	if(err_str!=NULL) insert_string(cbfp,err_str,strlen(err_str));
 };
 sfb(old_fp);
}

double tok_dir_if()
{
 double val=0;
	int exec_else=0;

	NTOKEN2;	/* go to next token after if */
	NTOKEN2;	/* skip left parenthesis  */
	val=lexpression();

	NTOKEN2;	/* skip right parenthesis  */
	if(val) {
		val=exec_sentence1();
	} else {
		skip_sentence1();	/* at the begin of next blocl/sentence  */
		exec_else=1;
	}
	// check for else statement!
	if(check_skip_token1(TOK_DIR_ELSE))
	{
		if(exec_else)	{
			val=exec_sentence1();	/* eval else statement */
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
	NTOKEN2;	/* skip left parenthesis  */
	lexpression();	/* initial   */
	NTOKEN2;	/* skip separator! */
	// set check_list
	check_element=tok;
	skip_sentence1();	/* skip check element  */
	// set loop_list
	loop_element=tok;
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

	while(1) {
		// set tlist to check pointer
		double val;
		tok=check_element;
		val=lexpression();
		// check for interruption!
		if(val 
//			&& current_active_flag
		) {
			tok=start_block;
			exec_sentence1();
//			MESG("before loop: break is %d active=%d",is_break1,current_active_flag);
			if(current_active_flag==0) {
				tok=end_block;
				break;			
			};
			tok=loop_element;
			val=lexpression();	/* exec for loop  */
//			MESG("loop value %f",val);
		} else {
			if(is_break1) { tok=cbfp->end_token;return(0);};
			// end for loop skipping one sentence or curl block
			tok=end_block;
			break;
		};
//		MESG("before next loop: val=%f",val);		
	};
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
	NTOKEN2;	/* skip left parenthesis  */
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
	if(dstep>0 && dmax > *pdval) {
		for(;*pdval < dmax; *pdval +=dstep) {

			tok=start_block;
			exec_sentence1();
			if(current_active_flag==0) {
				if(is_break1) { tok=cbfp->end_token;return(0);};
				tok=end_block;
				break;
			};
		};
	} else if(dstep<0 && dmax< *pdval) {
		for(; *pdval > dmax; *pdval +=dstep) {

			tok=start_block;
			exec_sentence1();
			if(current_active_flag==0) {
				if(is_break1) { tok=cbfp->end_token;return(0);};
				tok=end_block;
				break;
			};
		};
	} else {
		err_num=226;
		ERROR("error: infinite fori loop %d",err_num);
		tok=end_block;
	};
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
	NTOKEN2;	/* skip left parenthesis  */

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
		double val=0;
		// set tlist to tok pointer
		tok=check_element;
		val=lexpression();	/* CHECK should be lexpression  */
		// on the block start
		tok=start_block;

		if(val) {
			exec_sentence1();
			if(current_active_flag==0) {	/* only after break  */
				if(is_break1) { tok=cbfp->end_token;return(0);};
				tok=end_block;
				break;
			};
		} else {
			// skip one sentence or curl block
			tok=end_block;
			break;
		};

	} while(1); 
	current_active_flag=old_active_flag;
	return 1;
}

/*
 execute a sentence from a token list.
 pointer is already be set. 
 Stops on next separator (comma, semicolon, same level of parentesis or curl)
*/

double exec_sentence1()
{
 TDS("exec_sentence1");
 switch(tok->ttype) {
	case TOK_EOF:
	case TOK_RCURL:	
		return 0;
 	case TOK_LCURL:	/* we are at the start of the block  */
		NTOKEN2;
		return exec_block1(tok->level);
	case TOK_DIR_IF:
		return tok_dir_if();
	case TOK_DIR_FOR:
		return tok_dir_for();
	case TOK_DIR_FORI:
		return tok_dir_fori();
	case TOK_DIR_WHILE:
		return tok_dir_while();
	case TOK_DIR_BREAK:
		NTOKEN2;	/*   */
		current_active_flag=0;
		return(0);
	case TOK_DIR_RETURN:
	{	double val;
		NTOKEN2;
		val=lexpression();
		current_active_flag=0;	/* skip rest of function  */
		return(val);
	}
	default:
		return lexpression();
 }; 

 return(0); 
}

/* exec multiple sentences at the same level */
double exec_block1(int level)
{
 double val=0;
 stage_level=0;
 TDS("exec_block1");
   while(1) 
   {
	switch(tok->ttype){
		case TOK_EOF: return(val);
		case TOK_SEP:
		case TOK_RPAR:
			NTOKEN2;
			continue;
		case TOK_COMMA:
			NTOKEN2;
			break;
		case TOK_RCURL:
			NTOKEN2;
			return(val);
		case TOK_SHOW: {
			// insert result at offset!!
			// MESG("call refresh_ddot:");
			refresh_ddot_1(val);
			NTOKEN2;
			continue;
			};
	 };
	if(drv_check_break_key()) {
		syntax_error("user interruption",100);
		if(is_break1) return 0;
	};

 	val=exec_sentence1();
	if(!current_active_flag) return(val);
   };
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
 // MESG("compute_block: %s",bp->b_fname);
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

 if(bp->m_mode<2)	/* if not already checked!  */
 {
	if((err_num=check_init(bp))>0) {
// 		mesg_out("Error %d %s line %d ex_vtype=%d ex_value=%f slval=[%s]!",err_num,err_str,err_line,ex_vtype,ex_value,slval);
		show_error("Check init");
		return(0);
	};

	init_exec_flags();
	tok=bp->tok_table;
	drv_start_checking_break();
	val=exec_block1(0);
	drv_stop_checking_break();

	if(start) {
		delete_symbol_table(local_symbols,bp->symbol_tree->items);
		current_stable=old_symbol_table;
	};

	if(ex_vtype==VTYPE_STRING) msg_line("Result is \"%s\"",slval);
	if(ex_vtype==VTYPE_NUM) msg_line("Result is [%f]",val);
 } else {
 	msg_line("parse error %d on %s ",err_num,bp->b_fname);
	val=0;
 };
 tok=old_tok;
 return(val); 
}

int empty_tok_table(FILEBUF *fp)
{
 tok_struct *table= fp->tok_table;
 tok_struct *tokdel;
 if(table==NULL) return(0);
 tokdel=table;
 for(tokdel=table;tokdel->ttype!=TOK_EOF;tokdel++){
	if(tokdel->ttype==TOK_VAR || tokdel->ttype==TOK_QUOTE){
 		if(tokdel->tname!=NULL) free(tokdel->tname);
	};
 };
 free(table);
 fp->tok_table=NULL;
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
 parse_block1(fp,fp->symbol_tree,1,100);	/* init tree,extra 100 symbols  */

 if(err_num<1){	/* if no errors  */
	fp->symbol_table=new_symbol_table(fp->symbol_tree->max_items);
	current_stable=fp->symbol_table;

 	msg_line("checking ...");
	drv_start_checking_break();
	if(check_init(fp)>0) {
		drv_stop_checking_break();
		show_error("refresh buffer1");
		// msg_line("syntax error %d line %d [%s]",err_num,err_line,err_str);
		// mesg_out("syntax error %d line %d [%s]",err_num,err_line,err_str);
		return(0);
	};
 	msg_line("evaluating ...");
	init_exec_flags();
	tok=fp->tok_table;
	val=exec_block1(0);
	drv_stop_checking_break();
	if(err_num>0) {
		show_error("refresh buffer");
		// msg_line("Error %d [%s] at line %d",err_num,err_str,err_line);
		// mesg_out("Error %d [%s] at line %d",err_num,err_str,err_line);
	} else {
		if(ex_vtype==VTYPE_STRING) msg_line("Result is \"%s\"",slval);
		if(ex_vtype==VTYPE_NUM) msg_line("Result is [%f]",val);
		msg_line("Result is [%s %f]",slval,val);
	};
 } else {
 	msg_line("parse error %d ",err_num);
	return(0);
 };

 if(plot_on()) plot_redraw();
 textpoint_set_lc(cwp->tp_current,curline,0);
 msg_line("");
 return(1);
}
// parse, check current buffer
int parse_check_current_buffer(int n)
{
 FILEBUF *fp=cbfp;

 err_num=0;
 err_line=0;
 show_stage=1;
 if(!is_mlang(fp)) return 0;

 /* clear parse list  */
 empty_tok_table(fp);
 stage_level=0;
 // clear out buffer
 cls_fout("[out]");
 err_num=check_init(fp);
 if(err_num>0) {
	macro_exec=0;
	msg_line("syntax error %d line %d [%s]",err_num,err_line,err_str);
	show_error("parse_check");
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
 if(tok_ind==NULL) {
	msg_line("parsing buffer produced no table!");
	return(0);
 };
 out_print("-------- Token list -----------------------------------",1);
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
	show_error("parse_buffer_show_token");
	// msg_line("syntax error %d line %d [%s]",err_num,err_line,err_str);
	igotolinecol(err_line+1,1,1);
 	return(0);
 } else {
 	msg_line("No errors!");
 	return(1);
 };
}

char * tok_info(tok_struct *tok)
{
 static char stok[MAXLLEN];
 int dat=0;
	if(tok->tnode!=NULL) dat=1;
	if(tok->adat) dat=2;

	if(tok->tname!=NULL){
		switch(tok->ttype){
			case TOK_SHOW:		
				snprintf(stok,MAXLLEN,"%3d %4d %3d  %3d   [%2d=%12s] [:]",tok->tnum,tok->tline,tok->tind,tok->level,tok->ttype,TNAME);
				break;
			case TOK_LCURL:
			case TOK_RCURL:
				snprintf(stok,MAXLLEN,"%3d %4d %3d  %3d   [%2d=%12s] %s other is %d",tok->tnum,tok->tline,tok->tind,tok->level,tok->ttype,TNAME,(char *)tok->tname,tok->match_tok->tnum);
				break;
				break;
			default:
				if(tok->tgroup>0)
					snprintf(stok,MAXLLEN,"%3d %4d %3d  %3d   [%2d=%12s] [%s] group [%d:%s]",tok->tnum,tok->tline,tok->tind,tok->level,tok->ttype,TNAME,(char *)tok->tname,tok->tgroup,tok_name[tok->tgroup]);
				else snprintf(stok,MAXLLEN,"%3d %4d %3d  %3d   [%2d=%12s] [%s]",tok->tnum,tok->tline,tok->tind,tok->level,tok->ttype,TNAME,(char *)tok->tname);
		};
	} else {
		switch(tok->ttype){
			case TOK_LBRAKET:
			case TOK_RBRAKET:
				snprintf(stok,MAXLLEN,"%3d %4d %3d  %3d   [%2d=%12s] dat=%d",tok->tnum,tok->tline,tok->tind,tok->level,tok->ttype,TNAME,dat);
				break;
			default:
				snprintf(stok,MAXLLEN,"%3d %4d %3d  %3d   [%2d=%12s] [%f]",tok->tnum,tok->tline,tok->tind,tok->level,tok->ttype,TNAME,tok->dval);
		};
	};
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
 ex_value=num_expression();
 return (slval);
}

double get_val()
{
	return (ex_value);
}

char *get_sval()
{
	return(slval);
}

double next_value()
{
 double v;
 v=atof(slval);
 return(v);
}

void set_sval(char *s)
{
 strlcpy(slval,s,MAXLLEN);
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
	if (macro_exec == FALSE) {
		if(getstring(prompt, buffer, size,show)!=FALSE) {
			ex_value = atof(buffer);
		} else {
			set_update(cwp,UPD_MOVE);
			return(FALSE);
		};
	} else {
		/* slval has already the next argument */
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
