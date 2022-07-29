// C test file to view various highlight items in C mode 
// c++ comment
C comment	/*   */
/* Multiline c commments
	ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz
	1234567890 αβγδεζηθικλμνξοπρστυφχψω
*/ 
/* tabs and spaces utf8 and control characters */
	_ = αβγ 95 5F	[1;3A alt-up		[1;3D	alt-left
	- = 123 45 2D	[1;3B alt-down		[1;3C  alt-right 
	+ = αβγ 43 2B
#if 1
#include "test.h"
#define TEST "double quotes"
#define TEST2 'single quotes'
/* words1 */ 	if (124.3) else while { return("aa") ; }; 
/* words2 */  	struct void int double char short 
typedef struct aa {
	int b;
	char cc[30];
	double aa;
} aa;
	if(a==0x44ABC) { test=3;} else { int k=0o11;}	/* words1  */
#endif
in line special chars in C && +++ -------- **** ======= +++++++ [ { ( ) } ]
/* numbers, numerics */
123456789|1234567890123456789|123456789|123456789|1234567890123456789|123456789|1234567890|1234567890|1234567890|1234567890|1234567890|1234567890|1234567890|1234567890
         1         2         3         4         5         6         7         8          8          9          10         11
	Normal text 
Greek letters and other utf characters 
	αβγδεζηθικλμνξοπρστυφχψω άέήίόύώ ΆΈΉΊΌΎΏ àèìòù©˙˚¬≈ç√∫ãµ
	ΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠΡΣΤΥΦΧΨΩ ϊϋ €·§¹²³£§¶°•“ € …†®çœ∑Çé ¿

control characters []

// page separator (^L) (This is a valid character in c programs! (show it as a line break)


normal text
1234567890
------------
1234567890
==========
