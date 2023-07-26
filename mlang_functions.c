/* editor functions */
#include "mlangf.h"

void ntoken()
{
	tok++;
}

int tok_type()
{
	return tok->ttype;
}

m_function m_functions[] = {
	{"len",1,uf_len},        /* STRING LENGTH */
	{"upper",1,uf_upper},        /* UPPERCASE STRING */
    {"lower",1,uf_lower},        /* LOWER CASE STRING */
	{"left",2,uf_left},
	{"right",2,uf_right},
	{"mid",3,uf_mid},
    {"s_asc",1,uf_ascii},	/* CHAR TO INTEGER CONVERSION */
    {"chr",1,uf_chr},		/* INTEGER TO CHAR CONVERSION */
    {"getchar",0,uf_getchar},	/* GET 1 CHARACTER */
    {"rand",1,uf_rand},        	/* GET A RANDOM NUMBER */
	{"seed",1,uf_seed},			/* set sedd for random function  */
    {"abs",1,uf_abs},        	/* ABSOLUTE VALUE OF A NUMBER */
    {"s_index",2,uf_sindex},     /* FIND THE INDEX OF ONE STRING IN ANOTHER */
	{"str",1,uf_string},		/* string of a value */
	{"message",1,uf_message},	/* show message on screen  */
	{"error_log",1,uf_error},	/* show message error  */
	{"input",1,uf_input},		/* string input from screen  */
	{"initialize",0,uf_init},	/* variables init */
	{"s_val",1,uf_val},			/* numerical string value  */
	{"sqrt",1,uf_sqrt},
	{"sin",1,uf_sin},
	{"cos",1,uf_cos},
	{"tan",1,uf_tan},
	{"atan",1,uf_atan},
	{"log10",1,uf_log10},
	{"log",1,uf_log},
	{"trunc",1,uf_trunc},
	{"round",1,uf_round},
	{"print",-1,uf_print},	/* out_print, print on out buffer, and stdout if in Xwindows mode  */
	{"getpoint",0,uf_getpoint},
	{"message_wait",1,uf_wait},
	{"dinput",1,uf_dinput},
	{"show_time",2,uf_show_time},	/* show elapsed time in nanoseconds  */
	{"deq",2,uf_deq},	/* compare double with limited precission */
	{"cls",0,uf_cls},	/* clear output buffer  */
	{"DET",1,uf_determinant},	/* determinant  */
	{"INV",1,uf_inverse},	/* inverse  */
	{"T",1,uf_transpose},	/* transpose  */
	{"at_bof",0,uf_atbof},	/* if at begin of file  */
	{"at_eof",0,uf_ateof},	/* if at end of file  */
	{"at_bol",0,uf_atbol},	/* if at begin of line  */
	{"at_eol",0,uf_ateol},	/* if at end of line  */
	{"args_size",0,uf_mainargsize},	/* main arguments list size  */
	{"args",1,uf_mainarg},	/* main  argument at position */
	{"array_cols",1,uf_array_cols},	/* columns of an an array  */
	{"array_rows",1,uf_array_rows},	/* rows of an an array  */
	{"dbg_message",1,uf_dbg_message},	/* show debug message  */
	{NULL,0,NULL}
};
