#ifndef	_BTFUNC
#define _BTFUNC

// get a double value of name from bt_table
double bt_dval(char *name);

// get a string value of name from bt_table
char * bt_sval(char *name);

// set a string and double value in bt_table
void set_btval(char *name,int type,char *sval,double val);


#endif