
/* mlang functions */

v_function bnf_functions[] = {
	{"len",1,bnf_len},        /* STRING LENGTH */
	{"upper",1,bnf_upper},        /* UPPERCASE STRING */
    {"lower",1,bnf_lower},        /* LOWER CASE STRING */
	{"left",2,bnf_left},
	{"right",2,bnf_right},
	{"mid",3,bnf_mid},
    {"s_asc",1,bnf_ascii},	/* CHAR TO INTEGER CONVERSION */
    {"chr",1,bnf_chr},		/* INTEGER TO CHAR CONVERSION */
    {"getchar",0,bnf_getchar},	/* GET 1 CHARACTER */
    {"rand",1,bnf_rand},        	/* GET A RANDOM NUMBER */
	{"seed",1,bnf_seed},			/* set sedd for random function  */
    {"abs",1,bnf_abs},        	/* ABSOLUTE VALUE OF A NUMBER */
    {"s_index",2,bnf_sindex},     /* FIND THE INDEX OF ONE STRING IN ANOTHER */
	{"str",1,bnf_string},		/* string of a value */
	{"message",1,bnf_message},	/* show message on screen  */
	{"error_log",1,bnf_error},	/* show message error  */
	{"input",1,bnf_input},		/* string input from screen  */
	{"initialize",0,bnf_init},	/* variables init */
	{"s_val",1,bnf_val},			/* numerical string value  */
	{"sqrt",1,bnf_sqrt},
	{"cbrt",1,bnf_cbrt},
	{"sin",1,bnf_sin},
	{"cos",1,bnf_cos},
	{"tan",1,bnf_tan},
	{"atan",1,bnf_atan},
	{"log10",1,bnf_log10},
	{"log",1,bnf_log},
	{"trunc",1,bnf_trunc},
	{"round",1,bnf_round},
	{"print",-1,bnf_print},	/* out_print, print on out buffer, and stdout if in Xwindows mode  */
	{"getpoint",0,bnf_getpoint},
	{"message_wait",1,bnf_wait},
	{"dinput",1,bnf_dinput},
	{"show_time",2,bnf_show_time},	/* show elapsed time in nanoseconds  */
	{"deq",2,bnf_deq},	/* compare double with limited precission */
	{"cls",0,bnf_cls},	/* clear output buffer  */
	{"DET",1,bnf_determinant},	/* determinant  */
	{"INV",1,bnf_inverse},	/* inverse  */
	{"T",1,bnf_transpose},	/* transpose  */
	{"at_bof",0,bnf_atbof},	/* if at begin of file  */
	{"at_eof",0,bnf_ateof},	/* if at end of file  */
	{"at_bol",0,bnf_atbol},	/* if at begin of line  */
	{"at_eol",0,bnf_ateol},	/* if at end of line  */
	{"args_size",0,bnf_mainargsize},	/* main arguments list size  */
	{"args",1,bnf_mainarg},	/* main  argument at position */
	{"array_cols",1,bnf_array_cols},	/* columns of an an array  */
	{"array_rows",1,bnf_array_rows},	/* rows of an an array  */
	{"dbg_message",1,bnf_dbg_message},	/* show debug message  */
	{"show_vars",0,bnf_show_vars},	/* show local vars  */
	{"list_tokens",0,bnf_list_tokens},	/* list_tokens  */
	{"var_index",0,bnf_var_index},	/* show stack var index  */
	{"test_loop",1,bnf_test_loop},	/* test computational loop  */
	{"to_num_array",1,bnf_to_num_array},	/* convert to numeric array  */
	{"J",2,bnf_new_array_J},	/* new all 1 array  */
	{"I",1,bnf_new_array_I},	/* new identity array  */
	{"do_file",1,bnf_dofile},	/* execute a file  */
	// {"array_fixed_to_num",1,bnf_array_fixed_to_num},
	{NULL,0,NULL}
};
