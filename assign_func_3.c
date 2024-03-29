#include "assign_func.h"


double assign_none(double v1,MVAR *sslot)
{
	return 1;
}

void free_pstring(MVAR *sslot)
{
	if(sslot->sval) {
		free(sslot->sval);
		sslot->sval=NULL;
	};
}

double assign_num(double v1,MVAR *sslot)
{
	sslot->dval=v1;
	return v1;
}

double assign_array(double v1,MVAR *sslot)
{
	if(sslot->adat!=NULL){
	if(ex_array->anum != sslot->adat->anum) {
			if(sslot->adat->dval) free_array("assign",sslot->adat);
		if(sslot->adat->sval) {
			// MESG("free string array!");
		};
		sslot->adat=ex_array;
		if(ex_array->astat==ARRAY_ALLOCATED) ex_array->astat=ARRAY_LOCAL;	/* make it local to variable  */
	};} else {
		sslot->adat=ex_array;
	};
	return v1;
}

double assign_sarray(double v1,MVAR *sslot)
{
	return 1;
}

double assign_list(double v1,MVAR *sslot)
{
	return 1;
}

double assign_slist(double v1,MVAR *sslot)
{
	return 1;
}

double assign_alist(double v1,MVAR *sslot)
{
	return 1;
}

double assign_string(double v1,MVAR *sslot)
{
	free_pstring(sslot);
	return 1;
}

double assign_buffer(double v1,MVAR *sslot)
{
	return 1;
}

double assign_amixed(double v1,MVAR *sslot)
{
	return 1;
}

double assign_dynamic(double v1,MVAR *sslot)
{
	return 1;
}

double assign_tree(double v1,MVAR *sslot)
{
	return 1;
}

double assign_type(double v1,MVAR *sslot)
{
	return 1;
}

