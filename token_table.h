typedef struct TOK_TAB {
	int tok_num;
	char *tok_name;
	int tok_group;
} TOK_TAB;

TOK_TAB token_table[]= {
	{TOK_NONE			,"none",0},	// 0
	{TOK_SEP			," ; " ,0},	// 1
	{TOK_SPACE			," sp " ,0},	// 2
	{TOK_LETTER			,"letter ",0},	
	{TOK_LCURL			," { ",0},
	{TOK_RCURL			," } ",0},	// 5
	{TOK_QUOTE			,"quote",0},
	{TOK_LPAR			," ( ",0},	// 7
	{TOK_RPAR			," ) ",0},	// 8
	{TOK_SHOW			,":",0},	// 9
	{TOK_COMMENT		,"comment",0},	// 10
	{TOK_VAR			,"var",0},	// 11 level 0 variable
	{TOK_OPTION			,"option",0},	// 12 editor option
	{TOK_CMD			,"cmd",0},	// 13 editor commands
	{TOK_FUNC			,"function",0},	// 14 function
	{TOK_PROC			,"proc",0},	// 15
	{TOK_ENV			,"env",0},	// editor environment function
	{TOK_TERM0			,"term0",0},	// term0 group
	{TOK_TERM			,"term",0},	// term operators (+,-)
	{TOK_TERM1			,"term1",0},	// 19 term2 operators (*,/) 
	{TOK_TERM2			,"term2",0},	// 20 term1 operators (%,^)
	{TOK_ASSIGN			," = ", TOK_TERM0},	// 21 assignment
	{TOK_EOF			," EOF ",0},	// 22 end of file token
	{TOK_NUM			,"numeric",0},	// 23 numeric
	{TOK_DIR			,"dir",0},	// 24 directive
	{TOK_DIR_IF			,"if",0},	// 25 dir if
	{TOK_DIR_ELSE		,"else",0},	// 26 dir else
	{TOK_DIR_BREAK		,"break",0},	// 27
	{TOK_DIR_RETURN		,"return",0},	// 28
	{TOK_DIR_WHILE		,"while",0},	// 29
	{TOK_DIR_FOR		,"for",0},	// 30
	{TOK_COMMA			," , ",0},	// 31
	{TOK_DIR_FORI		,"fori",0},	// 32
	{TOK_COMPARE		,"cmp",0},	// 33
	{TOK_NOTEQUAL		," != ",TOK_COMPARE},	// 34
	{TOK_SMALLER		," < ",TOK_COMPARE},	/* <  */
	{TOK_BIGGER			," > ",TOK_COMPARE},	/* >  */
	{TOK_EQUAL			," == ",TOK_COMPARE},	/* ==  */
	{TOK_SMALLEREQ		," <= ",TOK_COMPARE},	/* <=  */
	{TOK_BIGGEREQ		," >= ",TOK_COMPARE},	/* >=  */
	{TOK_BOOL			,"bool",TOK_BOOL},	// 40
	{TOK_AND			,"AND",TOK_TERM0},	/* &  */
	{TOK_OR				," OR ",TOK_BOOL},	/* |  */
	{TOK_NOT			," NOT ",0},	/* !  */
	{TOK_NAND			," NAND ",TOK_TERM0},	/* !&  */
	{TOK_NOR			," NOR ",TOK_TERM0},	/* !|  */
	{TOK_XOR			," XOR ",TOK_BOOL},	/* ^  */
	{TOK_PLUS			," + ",TOK_TERM},	// 47
	{TOK_MINUS			," - ",TOK_TERM},	// 48
	{TOK_POWER			," ** ",TOK_TERM2},	// 49
	{TOK_MOD			," % ",TOK_TERM2},	// 50
	{TOK_MUL			," * ",TOK_TERM1},	// 51
	{TOK_DIV			," / ",TOK_TERM1},	// 52
	{TOK_LBRAKET		," LB ",0},	// 53
	{TOK_RBRAKET		," RB ",0},	// 54
	{TOK_SQUOTE			," ' ",0},	// 55
	{TOK_AT				," @ ",0},	// 56
	{TOK_RANGE			," range ",0},	// 57
	{TOK_BQUOTE			," BQ ",0},	// 58
	{TOK_DOLAR			," $ ",0},	// 59
	{TOK_TILDA			," ~ ",0},	// 60
	{TOK_INCREASE		," INC ",0},	
	{TOK_DECREASE		," DEC ",0},
	{TOK_INCREASEBY		," += ",TOK_TERM0},
	{TOK_MULBY			," *= ",TOK_TERM0},
	{TOK_DECREASEBY		," -= ",TOK_TERM0},	// 65
	{TOK_BSLASH			,"[\\]",0},
	{TOK_NL				," NL ",0},
	{TOK_DIR_CONTINUE	,"continue",0},
	{TOK_DIR_FOREACH	,"foreach",0},
	{TOK_DIR_TYPE		,"type",0},
	{TOK_ARRAY1			,"array1",0},	// 71
	{TOK_ARRAY2			,"array2",0},
	{TOK_ARRAY3			,"array3",0},
	{TOK_ASSIGNENV		,"setenv",TOK_TERM0},
	{TOK_ASSIGNOPT		,"setoption",TOK_TERM0},
	{TOK_START			,"START",0},
	{TOK_END			,"END",0},
	{TOK_DEFINE_TYPE	,"::",0},
	{TOK_OTHER			,"OTHER",0}
};