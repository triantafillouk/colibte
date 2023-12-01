#include "assign_func.h"


double assign_none(double v1,tok_data *sslot)
{
	return 1;
}

void free_pstring(tok_data *sslot)
{
	if(sslot->sval) {
		free(sslot->sval);
		sslot->sval=NULL;
	};
}

double assign_num(double v1,tok_data *sslot)
{
	sslot->dval=v1;
	return v1;
}

double assign_array(double v1,tok_data *sslot)
{
	return 1;
}

double assign_sarray(double v1,tok_data *sslot)
{
	return 1;
}

double assign_list(double v1,tok_data *sslot)
{
	return 1;
}

double assign_slist(double v1,tok_data *sslot)
{
	return 1;
}

double assign_alist(double v1,tok_data *sslot)
{
	return 1;
}

double assign_string(double v1,tok_data *sslot)
{
	free_pstring(sslot);
	return 1;
}

double assign_buffer(double v1,tok_data *sslot)
{
	return 1;
}

double assign_amixed(double v1,tok_data *sslot)
{
	return 1;
}

double assign_dynamic(double v1,tok_data *sslot)
{
	return 1;
}

double assign_tree(double v1,tok_data *sslot)
{
	return 1;
}

double assign_type(double v1,tok_data *sslot)
{
	return 1;
}

