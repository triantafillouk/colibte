/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

	Parser, Evaluator using tokens
*/

#include "alist.h"

#define	ARRAY_UNALLOCATED	0
#define	ARRAY_LOCAL			1
#define	ARRAY_ALLOCATED		3

typedef	alist * TLIST;
typedef double (*FFunction)();
typedef double (*TFunction)(double v1);
typedef double (*EFunction)(double v1,double v2);

typedef struct tok_struct {
	union {
	short tind;	/* token subtype and variable number */
	short t_nargs; 	/* number of arguments  */
	};
	short tline;	/* line for debugging  */
	short tnum;	/* token number for debugging  */
	short ttype;	/* token type */
	short tgroup;	/* token group  */
	char *tname;	// token name or string value
	double dval;	// double value
	union {
		FFunction factor_function;
		TFunction term_function;
		EFunction cexpr_function;
	};
	FFunction directive;
	union {	
		int	number_of_args;
		struct BTNODE *tok_node;
		struct curl_struct *tcurl;
		struct tok_struct *match_tok;	/* for curl, parenthesis, bracket  */
		struct tok_struct *next_tok;	/* for directives  */
		TextPoint *ddot;				/* ddot text point  */
		struct array_dat *tok_adat;
		struct FILEBUF *tbuf;
	};
} tok_struct;

typedef struct MVAR {
	short	var_type;
	short	var_index;	/* ??? possible use ??  */
	int		var_len;	/* ??? possible use ??  */
	union {
		double dval;
		char *sval;
		struct array_dat *adat;
	};
} MVAR;

typedef struct array_dat {
	short atype;
	short anum;
	int rows;
	int cols;
	short astat;	/* array allocation status  */
	union {
		char *array_name;
		BTREE *var_tree;
	};
	union {
		void *dat;
		double *dval;
		double **dval2;
		char **sval;
		struct MVAR *mval;
	};
} array_dat;

typedef struct curl_struct {
	short level;
	short num;	
	struct _el *ocurl;
} curl_struct;

typedef struct key_type {
	char *name;
	short atype;
} key_type;

// #define SEP_FUNCTIONS	1

/*	list of recognized user type functions	*/
typedef struct m_function {
	char *f_name;	/* function name  */
	short f_args;	/* number of function arguments  */
	FFunction ffunction;
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
	TOK_NONE	,	// 0
	TOK_SEP		,	// 1
	TOK_SPACE	,	// 2
	TOK_LETTER	,	
	TOK_LCURL	,
	TOK_RCURL	,	// 5
	TOK_QUOTE	,
	TOK_LPAR	,	// 7
	TOK_RPAR	,	// 8
	TOK_SHOW	,	// 9
	TOK_COMMENT	,	// 10
	TOK_VAR		,	// 11 level 0 variable
	TOK_OPTION	,	// 12 editor option
	TOK_CMD		,	// 13 editor commands
	TOK_FUNC	,	// 14 function
	TOK_PROC	,	// 15
	TOK_ENV		,	// editor environment function
	TOK_TERM0	,	// term0 group
	TOK_TERM	,	// term operators (+,-)
	TOK_TERM1	,	// 19 term2 operators (*,/) 
	TOK_TERM2	,	// 20 term1 operators (%,^)
	TOK_ASSIGN	,	// 21 assignment
	TOK_EOF		,	// 22 end of file token
	TOK_NUM		,	// 23 numeric
	TOK_DIR		,	// 24 directive
	TOK_DIR_IF	,	// 25 dir if
	TOK_DIR_ELSE	,	// 26 dir else
	TOK_DIR_BREAK	,	// 27
	TOK_DIR_RETURN	,	// 28
	TOK_DIR_WHILE	,	// 29
	TOK_DIR_FOR		,	// 30
	TOK_COMMA		,	// 31
	TOK_DIR_FORI	,	// 32
	/* compare operators  */
	TOK_COMPARE		,	// 33
	TOK_NOTEQUAL	,	// 34
	TOK_SMALLER		,	/* <  */
	TOK_BIGGER		,	/* >  */
	TOK_EQUAL		,	/* ==  */
	TOK_SMALLEREQ	,	/* <=  */
	TOK_BIGGEREQ	,	/* >=  */
	/* Bool operators  */
	TOK_BOOL		,	// 40
	TOK_AND			,	/* &  */
	TOK_OR			,	/* |  */
	TOK_NOT			,	/* !  */
	TOK_NAND		,	/* !&  */
	TOK_NOR			,	/* !|  */
	TOK_XOR			,	/* ^  */
	/* term operators  */
	TOK_PLUS		,	// 47
	TOK_MINUS		,	// 48
	TOK_POWER		,	// 49
	TOK_MOD			,	// 50
	TOK_MUL			,	// 51
	TOK_DIV			,	// 52
	TOK_LBRAKET		,	// 53
	TOK_RBRAKET		,	// 54
	TOK_SQUOTE		,	// 55
	TOK_AT			,	// 56
	TOK_RANGE		,	// 57
	TOK_BQUOTE		,	// 58
	TOK_DOLAR		,	// 59
	TOK_TILDA		,	// 60
	TOK_INCREASE	,	
	TOK_DECREASE	,
	TOK_INCREASEBY	,
	TOK_MULBY		,
	TOK_DECREASEBY	,	// 65
	TOK_BSLASH		,
	TOK_NL				,
	TOK_DIR_CONTINUE	,
	TOK_DIR_FOREACH		,
	TOK_DIR_TYPE		,
	TOK_ARRAY1		,	// 71
	TOK_ARRAY2		,
	TOK_ARRAY3		,
	TOK_ASSIGNENV	,
	TOK_ASSIGNOPT	,
	TOK_END,			/* @> */
	TOK_DEFINE_TYPE,	// 77
	TOK_ASSIGN_TYPE,	// 78
	TOK_TYPE_ELEMENT,	// 79
	TOK_DOT,
	TOK_OTHER			// 81
};


