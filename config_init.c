/* 
 	Preference properties init/save functions 
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.
*/

#include <glib.h>
#include <glib/gstdio.h>

#include "xe.h"
#define CONFIGFILE "colibte.conf"

const char cfg_group[] = "colibte";
extern alist *local_key_list;

struct {
	GKeyFile *cfg;
	char *configfile;
} colibte;

#if	NUSE
/* get a string from configuration */
char * get_cfg_str(char *label, char *default_str)
{
	char *cfg_val;

	// get a string value
	if (!g_key_file_has_key(colibte.cfg, cfg_group, label, NULL)) {
		g_key_file_set_value(colibte.cfg, cfg_group, label, default_str);
	}
	cfg_val = g_key_file_get_value(colibte.cfg, cfg_group, label, NULL);

	return cfg_val;
}
#endif

#if	NUSE
/* set a string configuration variable */
void set_cfg_str(char *label, char *str_val)
{
	g_key_file_set_value(colibte.cfg, cfg_group, label, str_val);
}
#endif

/* get an int variable from configuration */
int  get_cfg_int(char *label,int default_val)
{
	char *cfg_val;
	int dval;

	if (!g_key_file_has_key(colibte.cfg, cfg_group, label, NULL)) {
		cfg_val=(char *)g_malloc(20);
		snprintf(cfg_val,20,"%d",default_val);
		g_key_file_set_value(colibte.cfg, cfg_group, label, cfg_val);
//		MESG("set_cfg_int: new val for [%s] with default %s",label,cfg_val);
	} else {
		cfg_val = g_key_file_get_value(colibte.cfg, cfg_group, label, NULL);
//		MESG("set_cfg_int: read [%s] val %s",label,cfg_val);
	};
	dval = (int)atof(cfg_val);
	g_free(cfg_val);
//	MESG("get_cfg_int:[%s] %d",label,dval);
	return dval;
}

/* set an int configuration variable */
void set_cfg_int(char *label,int ival)
{
	char *cfg_val;

	cfg_val=(char *)g_malloc(20);
	snprintf(cfg_val,20,"%d",ival);

	g_key_file_set_value(colibte.cfg, cfg_group, label, cfg_val);
	g_free(cfg_val);
}

/* get a double float variable from configuration */
double  get_cfg_val(char *label,double default_val)
{
	char *cfg_val;
	double dval;

	if (!g_key_file_has_key(colibte.cfg, cfg_group, label, NULL)) {
		cfg_val=(char *)g_malloc(20);
		snprintf(cfg_val,20,"%lf",default_val);
		g_key_file_set_value(colibte.cfg, cfg_group, label, cfg_val);
	} else {
		cfg_val = g_key_file_get_value(colibte.cfg, cfg_group, label, NULL);
	};
	dval = atof(cfg_val);
	g_free(cfg_val);
	return dval;
}

extern int color_scheme_ind;

/* load configuration */
void load_config()
{
	GError *gerror=NULL;
	char* configdir = NULL;
//	MESG("load_config:");
	/* Config file initialization*/
	colibte.cfg = g_key_file_new();

	configdir = g_build_filename( g_get_user_config_dir(), "colibte", NULL );
	if( ! g_file_test( g_get_user_config_dir(), G_FILE_TEST_EXISTS) )
		g_mkdir( g_get_user_config_dir(), 0755 );
	if( ! g_file_test( configdir, G_FILE_TEST_EXISTS) )
		g_mkdir( configdir, 0755 );

	/* Use more standard-conforming path for config files, if available. */
	colibte.configfile=g_build_filename(configdir, CONFIGFILE, NULL);
	g_free(configdir);
	
	MESG("load configuration from %s",colibte.configfile);

	if (! g_file_test(colibte.configfile, G_FILE_TEST_EXISTS)) {
		char *new_file_contents;

		MESG("No config file yet, create an empty one to %s",colibte.configfile);
		new_file_contents=g_strconcat("[colibte]\n", "", NULL);
		g_file_set_contents(colibte.configfile, new_file_contents, strlen(new_file_contents), NULL);
		
		g_free(new_file_contents);
	} else {
		if (!g_key_file_load_from_file(colibte.cfg, colibte.configfile, 0, &gerror)) {
			if(gerror) { error_line("error %s reading config file!",gerror->message);}
			else error_line("error reading config file!");
		};
	};

	// get default values
	set_key_emulation(get_cfg_int("keyboard_emulation",0));
	set_btval("make_backup",-1,NULL,get_cfg_int("make_backup",0));
	set_btval("show_hex",-1,NULL,get_cfg_int("show_hex",0));
	set_btval("fillcol",-1,NULL,get_cfg_int("fillcol",72));
	set_btval("tabsize",-1,NULL,get_cfg_int("tabsize",4));

	set_btval("embed_icons",-1,NULL,get_cfg_int("embed_icons",0));
	set_btval("show_position",-1,NULL,get_cfg_int("show_position",1));
	set_btval("show_vinfo",-1,NULL,get_cfg_int("show_vinfo",0));
	set_btval("show_vmlines",-1,NULL,get_cfg_int("show_vmlines",0));
	set_btval("show_coffset",-1,NULL,get_cfg_int("show_coffset",0));
	set_btval("show_cdata",-1,NULL,get_cfg_int("show_cdata",1));
	set_btval("save_history",-1,NULL,get_cfg_int("save_history",0));
	set_btval("large_toolbar_icons",-1,NULL,get_cfg_int("large_toolbar_icons",1));
	set_btval("safe_ops",-1,NULL,get_cfg_int("safe_ops",1));

	set_btval("use_titlebar",-1,NULL,get_cfg_int("use_titlebar",1));
	set_btval("color_scheme",-1,NULL,get_cfg_int("color_scheme",2));
	set_btval("xcolor_scheme",-1,NULL,get_cfg_int("xcolor_scheme",2));

//	MESG("configuration loaded!");
}

/* save configuration */
void save_config()
{
	GError *gerror = NULL;
	gsize len = 0;
	gchar *data;

	// set last values!
	set_cfg_int("keyboard_emulation",(int)bt_dval("keyboard_emulation"));
 	set_cfg_int("make_backup",(int)bt_dval("make_backup"));
 	set_cfg_int("show_hex",(int)bt_dval("show_hex"));
 	set_cfg_int("fillcol",(int)bt_dval("fillcol"));
 	set_cfg_int("tabsize",(int)bt_dval("tabsize"));
	set_cfg_int("embed_icons",(int)bt_dval("embed_icons"));
	set_cfg_int("show_vinfo",(int)bt_dval("show_vinfo"));
	set_cfg_int("show_vmlines",(int)bt_dval("show_vmlines"));
	set_cfg_int("show_coffset",(int)bt_dval("show_coffset"));
	set_cfg_int("show_cdata",(int)bt_dval("show_cdata"));
 	set_cfg_int("show_position",(int)bt_dval("show_position"));
	set_cfg_int("save_history",(int)bt_dval("save_history"));
	set_cfg_int("large_toolbar_icons",(int)bt_dval("large_toolbar_icons"));
	set_cfg_int("safe_ops",(int)bt_dval("safe_ops"));
	set_cfg_int("use_titlebar",(int)bt_dval("use_titlebar"));
	set_cfg_int("color_scheme",(int)bt_dval("color_scheme"));
	set_cfg_int("xcolor_scheme",(int)bt_dval("xcolor_scheme"));
	
	data = g_key_file_to_data(colibte.cfg, &len, &gerror);
//	MESG("write cfg file: len=%ld",(long)len);

	if (!data) {
//		MESG("No data to write to key file");
		exit(1);
	}
	MESG("save configuration to %s",colibte.configfile);

	g_file_set_contents(colibte.configfile, data, len, NULL);	// write to config file

	g_key_file_free(colibte.cfg);
	g_free(colibte.configfile);
}




int save_keys()
{
 FILE *f1;
 char *fname;
 int stat=0;
 static char name1[MAXFLEN];
 char key_line[MAXLLEN];
 AKEYS *key;

 strcpy(name1,".colibte_keys");
 fname=find_file(NULL,name1,0);
 if(fname==NULL) {
	snprintf(name1,MAXFLEN,"%s/%s",getenv("HOME"),".colibte_keys");
//	MESG("save_keys: to [%s]",name1);
	fname=name1;
 };
 f1=fopen(fname,"w");
 lbegin(local_key_list);
 while( (key=(AKEYS *)lget(local_key_list))!=NULL)
 {
 	snprintf(key_line,MAXLLEN,"%s=%d=%s",key->a_function_name,key->c,key->a_key);
	fprintf(f1,"%s\n",key_line);
 };
 fclose(f1);
 return stat;
}

