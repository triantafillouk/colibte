#include "assign_func.h"


double assign_none(double v1,tok_data *sslot)
{
	return 1;
}

void check_pstring(tok_data *sslot)
{
	if(sslot->vtype==VTYPE_STRING) {
		if(sslot->sval) {
			free(sslot->sval);
			sslot->sval=NULL;
		};
	};
}

double assign_num(double v1,tok_data *sslot)
{
	// check_pstring(sslot);
	sslot->dval=v1;
	sslot->vtype=VTYPE_NUM;
	return v1;
}

double assign_array(double v1,tok_data *sslot)
{
	check_pstring(sslot);

	return 1;
}

double assign_sarray(double v1,tok_data *sslot)
{
	check_pstring(sslot);
	return 1;
}

double assign_list(double v1,tok_data *sslot)
{
	check_pstring(sslot);
	return 1;
}

double assign_slist(double v1,tok_data *sslot)
{
	check_pstring(sslot);
	return 1;
}

double assign_alist(double v1,tok_data *sslot)
{
	check_pstring(sslot);
	return 1;
}

double assign_string(double v1,tok_data *sslot)
{
	return 1;
}

double assign_buffer(double v1,tok_data *sslot)
{
	check_pstring(sslot);
	return 1;
}

double assign_amixed(double v1,tok_data *sslot)
{
	check_pstring(sslot);
	return 1;
}

double assign_dynamic(double v1,tok_data *sslot)
{
	check_pstring(sslot);
	return 1;
}

double assign_tree(double v1,tok_data *sslot)
{
	check_pstring(sslot);
	return 1;
}

double assign_type(double v1,tok_data *sslot)
{
	check_pstring(sslot);
	return 1;
}

