/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

	Parser, Evaluator using tokens
*/

#include "alist.h"

typedef	alist * TLIST;
typedef double (*FFunction)();
typedef double (*TFunction)(double v1);
typedef double (*EFunction)(double v1,double v2);

typedef struct tok_struct {
	short level;		// curl level
	short tind;	/* token subtype  */
	short tline;	/* line for debugging  */
	short tnum;	/* token number for debugging  */
	void *tname;	// token name or string value
	double dval;	// double value
	int ttype;	/* token type */
	int tgroup;	/* token group  */
	union {
		FFunction factor_function;
		TFunction term_function;
		EFunction cexpr_function;
	};
	union {	
	struct BTNODE *tnode;
	struct curl_struct *tcurl;
	struct tok_struct *match_tok;
	struct array_dat *adat;
	struct FILEBUF *tbuf;
//	struct tok_data *tdata;
	};
} tok_struct;

typedef struct MVAR {
	int	vtype;
	union {
		double dval;
		char *sval;
		struct array_dat *adat;
	};
} MVAR;

typedef struct array_dat {
	int atype;
	int anum;
	int rows;
	int cols;
	int astat;
	union {
		void *dat;
		double *dval;
		double **dval2;
		char **sval;
		struct MVAR *mval;
	};
} array_dat;

typedef struct curl_struct {
	int level;
	int type;
	int mline;
	int num;	
	int active;
	struct _el *ocurl;
} curl_struct;

/*	list of recognized user type functions	*/
typedef struct m_function  {
	char *f_name;	/* function name  */
	int f_args;	/* number of function arguments  */
} m_function;

typedef struct term_type {
	char *term_name;
	short term_type;
	short term_group;
} term_type;

void init_token_mask();
double assign(int is_edenv);
int parse_block1(FILEBUF *bf,BTREE *stree,int start,int extra);
int assign_args1( MVAR *va,tok_data *td,int nargs);
double factor_1();
double term1_1();
double term2_1();
double expression_1();
double lterm1_1();
double lexpression_1();
double assign1(int is_edenv);
double command1();
double exec_block1();
double exec_sentence1();
int check_next_token(int ind);
int check_skip_token(int type);
int check_skip_token_err(int type,char *mesg,int err);
int check_token(int type);
int check_next_token_type(int type);
tok_struct *gettok1();
void syntax_error(char *description,int err);
void set_error(tok_struct *tok,int index,char *description);
int plot_on();
void plot_redraw();
BTNODE *find_bt_element(char *value);
struct array_dat * new_list_array(int cols);
void allocate_array(struct array_dat *adat);

// Token type enumerator
enum {
	TOK_NONE	,
	TOK_SEP		,
	TOK_SPACE	,
	TOK_LETTER	,
	TOK_LCURL	,
	TOK_RCURL	,
	TOK_QUOTE	,
	TOK_LPAR	,
	TOK_RPAR	,
	TOK_SHOW	,
	TOK_COMMENT	,
	TOK_VAR		,	// level 0 variable
	TOK_OPTION	,	// editor option
	TOK_CMD		,	// editor commands
	TOK_FUNC	,	// function
	TOK_PROC	,
	TOK_ENV		,	// editor environment function
	TOK_TERM0	,	// term0 group
	TOK_TERM	,	// term operators (+,-)
	TOK_TERM1	,	// term2 operators (*,/) 
	TOK_TERM2	,	// term1 operators (%,^)
	TOK_ASSIGN	,	// assignment
	TOK_EOF		,	// end of file token
	TOK_NUM		,	// numeric
	TOK_DIR		,	// directive
	TOK_DIR_IF	,	// dir if
	TOK_DIR_ELSE	,	// dir else
	TOK_DIR_BREAK	,
	TOK_DIR_RETURN	,
	TOK_DIR_WHILE	,
	TOK_DIR_FOR		,
	TOK_COMMA		,
	TOK_DIR_FORI	,
	/* compare operators  */
	TOK_COMPARE		,
	TOK_NOTEQUAL	,
	TOK_SMALLER		,	/* <  */
	TOK_BIGGER		,	/* >  */
	TOK_EQUAL		,	/* ==  */
	TOK_SMALLEREQ	,	/* <=  */
	TOK_BIGGEREQ	,	/* >=  */
	/* Bool operators  */
	TOK_BOOL		,
	TOK_AND			,	/* &  */
	TOK_OR			,	/* |  */
	TOK_NOT			,	/* !  */
	TOK_NAND		,	/* !&  */
	TOK_NOR			,	/* !|  */
	TOK_XOR			,	/* ^  */
	/* term operators  */
	TOK_PLUS		,
	TOK_MINUS		,
	TOK_POWER		,
	TOK_MOD			,
	TOK_MUL			,
	TOK_DIV			,

	TOK_LBRAKET		,
	TOK_RBRAKET		,
	TOK_SQUOTE		,
	TOK_AT			,
	TOK_RANGE		,
	TOK_BQUOTE		,
	TOK_DOLAR		,
	TOK_TILDA		,
	TOK_INCREASE	,
	TOK_DECREASE	,
	TOK_INCREASEBY	,
	TOK_DECREASEBY	,
	TOK_BSLASH		,

	TOK_NL				,
	TOK_DIR_CONTINUE	,
	TOK_DIR_FOREACH		,

	TOK_ARRAY1		,
	TOK_ARRAY2		,
	TOK_ARRAY3		,
	TOK_ASSIGNENV	,
	TOK_ASSIGNOPT	,
//	TOK_END_ARG,	// end of function arguments
	TOK_START,	/* <@ */
	TOK_END,	/* @> */
	TOK_OTHER,
};

