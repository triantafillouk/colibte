inline static void update_ddot_var_position();
inline static void bnf_refresh_ddot();
inline static void bnf_factor_var();
inline static void bnf_factor_num();
inline static void bnf_factor_quote();
inline static void bnf_factor_not();
inline static void bnf_factor_negate();
inline static void bnf_factor_pp_plus();
inline static void bnf_factor_pp_minus();
inline static void bnf_factor_nn_plus();
inline static void bnf_factor_nn_minus();
inline static void bnf_factor_pn_plus();
inline static void bnf_factor_pn_minus();
inline static void bnf_factor_np_plus();
inline static void bnf_factor_np_minus();
inline static void bnf_factor_spn_plus();
inline static void bnf_factor_plus();
inline static void bnf_factor_minus();
inline static void bnf_factor_np_num_mul();
inline static void bnf_factor_pn_num_mul();
inline static void bnf_factor_nn_num_mul();
inline static void bnf_factor_pp_num_mul();

inline static void bnf_nump_plus();
inline static void bnf_numn_plus();
inline static void bnf_num_plus();
inline static void bnf_num_minus();
inline static void bnf_nump_mul();
inline static void bnf_numn_mul();
inline static void bnf_num_mul();
inline static void bnf_num_div();
inline static void bnf_var_mul_num();
inline static void bnf_var_div_num();
inline static void bnf_var_mul_nump();
inline static void bnf_var_div_nump();
inline static void bnf_var_nn_plus();
inline static void bnf_var_mul();
inline static void bnf_var_pn_plus();
inline static void bnf_var_nn_minus();
inline static void bnf_var_pn_minus();
inline static void bnf_var_plus();
inline static void bnf_var_minus();
inline static void bnf_var_div();
inline static void bnf_divby_pp_num();
inline static void bnf_divby_pn_num();
inline static void bnf_factor_array_l2_update();
inline static void bnf_type_l2_result_update();

inline static void bnf_factor_mul();
inline static void bnf_factor_div();
inline static void bnf_factor_modulo();
inline static void bnf_factor_power();
inline static void bnf_factor_smaller_pn();
inline static void bnf_factor_smaller_np();
inline static void bnf_factor_smaller();
inline static void bnf_factor_bigger_pn();
inline static void bnf_factor_bigger_np();
inline static void bnf_factor_bigger();
inline static void bnf_factor_smallereq();
inline static void bnf_factor_biggereq();
inline static void bnf_factor_equal();
inline static void bnf_factor_notequal();
inline static void bnf_factor_and();
inline static void bnf_factor_nand();
inline static void bnf_factor_or();
inline static void bnf_factor_nor();
inline static void bnf_factor_xor();
inline static void bnf_factor_none();
inline static void bnf_factor_dummy();
inline static void bnf_update_val();
inline static void bnf_update_array1();
inline static void bnf_increaseby_pp_num();
inline static void bnf_increaseby_pn_num();
inline static void bnf_decreaseby_pp_num();
inline static void bnf_decreaseby_pn_num();
inline static void bnf_increaseby();
inline static void bnf_decreaseby();
inline static void bnf_mulby_pp_num();
inline static void bnf_mulby_pn_num();
inline static void bnf_mulby();

inline static void bnf_divby();
inline static void bnf_factor_sep0();
inline static void bnf_factor_end();
inline static void bnf_factor_sep1();
inline static void bnf_factor_comma();
inline static void bnf_factor_eof();
inline static void bnf_factor_sep();
inline static void bnf_factor_rcurl0();
inline static void bnf_factor_rcurl_no();
inline static void bnf_factor_rcurl();
inline static void bnf_factor_error();	/* rbracket  */
inline static void bnf_factor_assign_var_num();
inline static void bnf_factor_assign_var_nump();
inline static void bnf_factor_assign_var();
inline static void bnf_factor_assign_var_f();
inline static void bnf_factor_env();
inline static void bnf_assign_env();
inline static void bnf_assign_opt();
inline static void bnf_factor_option();
inline static void bnf_factor_lpar();
inline static void bnf_factor_rpar();
inline static void bnf_dir_lcurl();
inline static void bnf_dir_lcurl_break();
inline static void bnf_dir_break();
inline static void bnf_dir_fori();
inline static void bnf_dir_for();
inline static void bnf_dir_while()	/* TBC  */;
inline static void bnf_dir_return_value();
inline static void bnf_dir_return_novalue();
inline static void bnf_dir_return();
inline static void bnf_factor_proc();
#if	TFUNC
inline static void bnf_dir_if_then();
#else
inline static void bnf_dir_if();
#endif
inline static void bnf_dir_else();
inline static void bnf_factor_line_array();
inline static void bnf_assign_array1_array();
inline static void bnf_assign_array1_arrayp();
inline static void bnf_assign_array1();
inline static void bnf_increaseby_array1();
inline static void bnf_decreaseby_array1();
inline static void bnf_mulby_array1();
inline static void bnf_divby_array1();
inline static void bnf_assign_array2();
inline static void bnf_increaseby_array2();
inline static void bnf_decreaseby_array2();
inline static void bnf_mulby_array2();
inline static void bnf_divby_array2();
inline static void bnf_factor_array2();
inline static void bnf_factor_array_l1();
inline static void bnf_factor_array_l1_tba_array();
inline static void bnf_factor_array_l1_tba();
inline static void bnf_factor_array_l2_tba();
inline static void bnf_factor_array_l2();
inline static void bnf_factor_cmd();
inline static void bnf_assign_type();
inline static void bnf_type_element();
inline static void bnf_type_element_l1_tba();
inline static void bnf_assign_type_el1();
inline static void bnf_type_l1_result();
inline static void bnf_assign_element();
inline static void bnf_increaseby_element();
inline static void bnf_increaseby_element0();
inline static void bnf_deccreaseby_element0();
inline static void bnf_mulby_element0();
inline static void bnf_divby_element0();
inline static void bnf_decreaseby_element();
inline static void bnf_mulby_element();
inline static void bnf_divby_element();
inline static void bnf_type_l2_result();
inline static void bnf_factor_at();

#if	TBNF
VFunction factor_bnf_funcs[] = {
	bnf_factor_none,	// TOK_NONE
	bnf_factor_sep,		// TOK_SEP
	bnf_factor_none,	// TOK_SPACE
	bnf_factor_none,	// TOK_LETTER
	bnf_dir_lcurl,		// TOK_LCURL	,
	bnf_factor_rcurl,	// TOK_RCURL	,
	bnf_factor_quote,	// TOK_QUOTE
	bnf_factor_lpar,	// TOK_LPAR
	bnf_factor_rpar,	// TOK_RPAR	
	bnf_refresh_ddot,	// TOK_SHOW
	bnf_factor_none,	// TOK_COMMENT	,
	bnf_factor_var,	// TOK_VAR	level 0 variable
	bnf_factor_option,	// TOK_OPTION	,	// editor option
	bnf_factor_cmd,		// TOK_CMD		,	// editor commands
	bnf_factor_none,	// TOK_FUNC	,	// function
	bnf_factor_proc,	// TOK_PROC	,
	bnf_factor_env,		// TOK_ENV		,	// editor environment function
	bnf_factor_none,	// TOK_TERM0	term0 group
	bnf_factor_none,	// TOK_TERM	,	// term operators (+,-)
	bnf_factor_none,	// TOK_TERM1	,	// term1 operators (%,^)
	bnf_factor_none,	// TOK_TERM2	,	// term2 operators (*,/)
	bnf_factor_assign_var,	// TOK_ASSIGN	,	// assignament
	bnf_factor_eof,		// TOK_EOF		,	// end of file token
	bnf_factor_num,		// TOK_NUM, numeric value

	bnf_factor_none,	// TOK_DIR		,	// directive
#if	TFUNC
	bnf_dir_if_then,	// TOK_DIR_IF	,	// dir if
#else
	bnf_dir_if,	// TOK_DIR_IF	,	// dir if
#endif
	bnf_dir_else,	// TOK_DIR_ELSE	,	// dir else
	bnf_dir_break,	// TOK_DIR_BREAK	,
	bnf_dir_return,	// TOK_DIR_RETURN	,
	bnf_dir_while,	// TOK_DIR_WHILE	,
	bnf_dir_for,	// TOK_DIR_FOR		,
	bnf_factor_comma,	// TOK_COMMA		,
	bnf_dir_fori,	// TOK_DIR_FORI	,

	/* bool operators  */
	bnf_factor_none,	// TOK_COMPARE		,33

	bnf_factor_notequal,	// TOK_NOTEQUAL	,
	bnf_factor_smaller,	// TOK_SMALLER		,	/* <  */
	bnf_factor_bigger,	// TOK_BIGGER		,	/* >  */
	bnf_factor_equal,	// TOK_EQUAL		,	/* ==  */
	bnf_factor_smallereq,	// TOK_SMALLEREQ	,	/* <=  */
	bnf_factor_biggereq,	// TOK_BIGGEREQ	,	/* >=  */

	bnf_factor_none,	// TOK_BOOL		,40
	bnf_factor_and,	// TOK_AND			,	/* &  */
	bnf_factor_or,	// TOK_OR			,	/* |  */
	bnf_factor_not,		// TOK_NOT	/* !  */
	bnf_factor_nand,	// TOK_NAND		,	/* !&  */
	bnf_factor_nor,	// TOK_NOR			,	/* !|  */
	bnf_factor_xor,	// TOK_XOR			,	/* ^  */
	/* term operators  */
	bnf_factor_plus,	// TOK_PLUS		,47
	bnf_factor_minus,	// TOK_MINUS		,
	bnf_factor_power,	// TOK_POWER		,	/* ** */
	bnf_factor_modulo,	// TOK_MOD			,	/* %  */
	bnf_factor_mul,	// TOK_MUL			,
	bnf_factor_div,	// TOK_DIV			,

	bnf_factor_line_array,	// TOK_LBRAKET		,53
	bnf_factor_error,	// TOK_RBRAKET		,
	bnf_factor_none,	// TOK_SQUOTE		,
	bnf_factor_at,		// TOK_AT			,
	bnf_factor_none,	// TOK_RANGE		,
	bnf_factor_none,	// TOK_BQUOTE
	bnf_factor_none,	// TOK_DOLAR		,
	bnf_factor_none,	// TOK_TILDA		,
	bnf_update_val,		// TOK_INCREASE	,61
	bnf_update_val,		// TOK_DECREASE	,62
	bnf_increaseby,	// TOK_INCREASEBY 63
	bnf_mulby,			// TOK_MULBY
	bnf_divby,			// TOK_DIVBY,
	bnf_decreaseby,	// TOK_DECREASEBY
	bnf_factor_none,	// TOK_BSLASH		,

	bnf_factor_none,	// TOK_NL				,
	bnf_factor_none,	// TOK_DIR_CONTINUE	,
	bnf_factor_none,	// TOK_DIR_FOREACH		,
	bnf_factor_none,	// TOK_DIR_TYPE,
	bnf_factor_array_l1_tba,	// TOK_ARRAY1
	bnf_factor_array2,	// TOK_ARRAY2
	bnf_factor_array_l1,// TOK_ARRAY_L1
	bnf_factor_array_l2,// TOK_ARRAY_L2
	bnf_assign_env,	// TOK_ASSIGNENV	,
	bnf_assign_opt,	// TOK_ASSIGNOPT	,
	bnf_factor_end,	// TOK_END,
	bnf_factor_none,	// TOK_DEFINE_TYPE,
#if	USE_TYPE_VARS
	bnf_assign_type,	// TOK_ASSIGN_TYPE,
	bnf_type_element,	// TOK_TYPE_ELEMENT
#else
	bnf_factor_none,	// TOK_ASSIGN_TYPE,
	bnf_factor_none,	// TOK_TYPE_ELEMENT
#endif
	bnf_factor_none,	// TOK_DOT,
	bnf_factor_none,	// TOK_INCBEFORE
	bnf_factor_none,	// TOK_DECBEFORE
	bnf_assign_array1,	// TOK_ASSIGN_ARRAY1
	bnf_assign_array2,	// TOK_ASSIGN_ARRAY2
	bnf_update_val,		// TOK_INCREASE_ARRAY1,
	bnf_update_val,		// TOK_INCREASE_ARRAY2,
	bnf_update_val,		// TOK_DECREASE_ARRAY1,
	bnf_update_val,		// TOK_DECREASE_ARRAY2,
	bnf_factor_negate,	// TOK_NEGATE,
	bnf_factor_none,	// TOK_OPNUM,
	bnf_factor_none		// TOK_OTHER,
};

int factor_bnf_group[] = {
	0,	// TOK_NONE
	0,		// TOK_SEP
	0,	// TOK_SPACE
	1,	// TOK_LETTER
	0,	// TOK_LCURL	,
	BLOCK_END,	// TOK_RCURL	,
	1,	// TOK_QUOTE
	1,	// TOK_LPAR
	0,	// TOK_RPAR	
	0,	// TOK_SHOW
	0,	// TOK_COMMENT	,
	1,	// TOK_VAR	level 0 variable
	1,	// TOK_OPTION	,	// editor option
	1,		// TOK_CMD		,	// editor commands
	1,	// TOK_FUNC	,	// function
	1,	// TOK_PROC	,
	1,		// TOK_ENV		,	// editor environment function
	1,	// TOK_TERM0	term0 group
	1,	// TOK_TERM	,	// term operators (+,-)
	1,	// TOK_TERM1	,	// term1 operators (%,^)
	1,	// TOK_TERM2	,	// term2 operators (*,/)
	1,		// TOK_ASSIGN	,	// assignment
	BLOCK_END,		// TOK_EOF		,	// end of file token
	1,		// TOK_NUM, numeric value
	0,	// TOK_DIR		,	// directive
	0,	// TOK_DIR_IF	,	// dir if
	0,	// TOK_DIR_ELSE	,	// dir else
	0,	// TOK_DIR_BREAK	,
	0,	// TOK_DIR_RETURN	,
	0,	// TOK_DIR_WHILE	,
	0,	// TOK_DIR_FOR		,
	0,	// TOK_COMMA		,
	0,	// TOK_DIR_FORI	,
	/* bool operators  */
	1,	// TOK_COMPARE		,33
	1,	// TOK_NOTEQUAL	,
	1,	// TOK_SMALLER		,	/* <  */
	1,	// TOK_BIGGER		,	/* >  */
	1,	// TOK_EQUAL		,	/* ==  */
	1,	// TOK_SMALLEREQ	,	/* <=  */
	1,	// TOK_BIGGEREQ	,	/* >=  */
	/* Bool operators  */
	1,	// TOK_BOOL		,40
	1,	// TOK_AND			,	/* &  */
	1,	// TOK_OR			,	/* |  */
	1,		// TOK_NOT	/* !  */
	1,	// TOK_NAND		,	/* !&  */
	1,	// TOK_NOR			,	/* !|  */
	1,	// TOK_XOR			,	/* ^  */
	/* term operators  */
	1,	// TOK_PLUS		,47
	1,	// TOK_MINUS		,
	1,	// TOK_POWER		,	/* ** */
	1,	// TOK_MOD			,	/* %  */
	1,	// TOK_MUL			,
	1,	// TOK_DIV			,
	1,	// TOK_LBRAKET		,53
	0,	// TOK_RBRAKET		,
	1,	// TOK_SQUOTE		,
	0,		// TOK_AT			,
	0,	// TOK_RANGE		,
	0,	// TOK_BQUOTE
	0,	// TOK_DOLAR		,
	0,	// TOK_TILDA		,
	1,		// TOK_INCREASE	,61
	1,		// TOK_DECREASE	,62
	1,	// TOK_INCREASEBY 63
	1,			// TOK_MULBY
	1,		// TOK_DIVBY
	1,	// TOK_DECREASEBY
	0,	// TOK_BSLASH		,
	0,	// TOK_NL				,
	0,	// TOK_DIR_CONTINUE	,
	0,	// TOK_DIR_FOREACH		,
	0,	// TOK_DIR_TYPE,
	1,	// TOK_ARRAY1
	1,	// TOK_ARRAY2
	1,// TOK_ARRAY_L1
	1,// TOK_ARRAY_L2
	1,	// TOK_ASSIGNENV	,
	1,	// TOK_ASSIGNOPT	,
	0,	// TOK_END,
	0,	// TOK_DEFINE_TYPE,
	1,	// TOK_ASSIGN_TYPE,
	1,	// TOK_TYPE_ELEMENT
	0,	// TOK_DOT,
	0,	// TOK_INCBEFORE
	0,	// TOK_DECBEFORE
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	0		// TOK_OTHER,
};

int factor_bnf_type[] = {
	0,	// TOK_NONE
	0,		// TOK_SEP
	0,	// TOK_SPACE
	TOK_LETTER,	// TOK_LETTER
	0,	// TOK_LCURL	,
	0,	// TOK_RCURL	,
	TOK_QUOTE,	// TOK_QUOTE
	TOK_LPAR,	// TOK_LPAR
	TOK_RPAR,	// TOK_RPAR	
	0,	// TOK_SHOW
	0,	// TOK_COMMENT	,
	TOK_VAR,	// TOK_VAR	level 0 variable
	TOK_OPTION,	// TOK_OPTION	,	// editor option
	TOK_CMD,		// TOK_CMD		,	// editor commands
	TOK_FUNC,	// TOK_FUNC	,	// function
	TOK_PROC,	// TOK_PROC	,
	TOK_ENV,		// TOK_ENV		,	// editor environment function
	TOK_TERM0,	// TOK_TERM0	term0 group
	TOK_TERM,	// TOK_TERM	,	// term operators (+,-)
	TOK_TERM1,	// TOK_TERM1	,	// term1 operators (%,^)
	TOK_TERM2,	// TOK_TERM2	,	// term2 operators (*,/)
	TOK_ASSIGN,		// TOK_ASSIGN	,	// assignment
	0,		// TOK_EOF		,	// end of file token
	TOK_NUM,		// TOK_NUM, numeric value
	0,	// TOK_DIR		,	// directive
	0,	// TOK_DIR_IF	,	// dir if
	0,	// TOK_DIR_ELSE	,	// dir else
	-TOK_DIR_BREAK,	// TOK_DIR_BREAK	,
	-TOK_DIR_RETURN,	// TOK_DIR_RETURN	,
	0,	// TOK_DIR_WHILE	,
	0,	// TOK_DIR_FOR		,
	-TOK_COMMA,	// TOK_COMMA		,
	0,	// TOK_DIR_FORI	,
	/* bool operators  */
	TOK_COMPARE,	// TOK_COMPARE		,33
	TOK_NOTEQUAL,	// TOK_NOTEQUAL	,
	TOK_SMALLER,	// TOK_SMALLER		,	/* <  */
	TOK_BIGGER,	// TOK_BIGGER		,	/* >  */
	TOK_EQUAL,	// TOK_EQUAL		,	/* ==  */
	TOK_SMALLEREQ,	// TOK_SMALLEREQ	,	/* <=  */
	TOK_BIGGEREQ,	// TOK_BIGGEREQ	,	/* >=  */
	/* Bool operators  */
	TOK_BOOL,	// TOK_BOOL		,40
	TOK_AND,	// TOK_AND			,	/* &  */
	TOK_OR,	// TOK_OR			,	/* |  */
	TOK_NOT,		// TOK_NOT	/* !  */
	TOK_NAND,	// TOK_NAND		,	/* !&  */
	TOK_NOR,	// TOK_NOR			,	/* !|  */
	TOK_XOR,	// TOK_XOR			,	/* ^  */
	/* term operators  */
	TOK_PLUS,	// TOK_PLUS		,47
	TOK_MINUS,	// TOK_MINUS		,
	TOK_POWER,	// TOK_POWER		,	/* ** */
	TOK_MOD,	// TOK_MOD			,	/* %  */
	TOK_MUL,	// TOK_MUL			,
	TOK_DIV,	// TOK_DIV			,
	TOK_LBRAKET,	// TOK_LBRAKET		,53
	0,	// TOK_RBRAKET		,
	TOK_SQUOTE,	// TOK_SQUOTE		,
	0,		// TOK_AT			,
	0,	// TOK_RANGE		,
	0,	// TOK_BQUOTE
	0,	// TOK_DOLAR		,
	0,	// TOK_TILDA		,
	TOK_INCREASE,		// TOK_INCREASE	,61
	TOK_DECREASE,		// TOK_DECREASE	,62
	TOK_INCREASEBY,	// TOK_INCREASEBY 63
	TOK_MULBY,			// TOK_MULBY
	TOK_DIVBY,		// TOK_DIVBY
	TOK_DECREASEBY,	// TOK_DECREASEBY
	0,	// TOK_BSLASH		,
	0,	// TOK_NL				,
	0,	// TOK_DIR_CONTINUE	,
	0,	// TOK_DIR_FOREACH		,
	0,	// TOK_DIR_TYPE,
	TOK_ARRAY1,	// TOK_ARRAY1
	TOK_ARRAY2,	// TOK_ARRAY2
	TOK_ARRAY_L1,// TOK_ARRAY_L1
	TOK_ARRAY_L2,// TOK_ARRAY_L2
	TOK_ASSIGNENV,	// TOK_ASSIGNENV	,
	TOK_ASSIGNOPT,	// TOK_ASSIGNOPT	,
	0,	// TOK_END,
	0,	// TOK_DEFINE_TYPE,
	TOK_ASSIGN_TYPE,	// TOK_ASSIGN_TYPE,
	TOK_TYPE_ELEMENT,	// TOK_TYPE_ELEMENT
	0,	// TOK_DOT,
	0,	// TOK_INCBEFORE
	0,	// TOK_DECBEFORE
	TOK_ASSIGN_ARRAY1,
	TOK_ASSIGN_ARRAY2,
	TOK_INCREASE_ARRAY1,
	TOK_INCREASE_ARRAY2,
	TOK_DECREASE_ARRAY1,
	TOK_DECREASE_ARRAY2,
	TOK_NEGATE,
	TOK_OPNUM,
	0		// TOK_OTHER,
};

#endif
