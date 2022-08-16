/* 
 	Preference properties init/save functions 
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.
*/

#include <glib.h>
#include <glib/gstdio.h>

#include "xe.h"

const char cfg_group[] = APPLICATION_NAME;
extern alist *local_key_list;

struct {
	GKeyFile *cfg;
	char *configfile;
} app_cfg;

#if	NUSE
/* get a string from configuration */
char * get_cfg_str(char *label, char *default_str)
{
	char *cfg_val;

	// get a string value
	if (!g_key_file_has_key(app_cfg.cfg, cfg_group, label, NULL)) {
		g_key_file_set_value(app_cfg.cfg, cfg_group, label, default_str);
	}
	cfg_val = g_key_file_get_value(app_cfg.cfg, cfg_group, label, NULL);

	return cfg_val;
}

/* set a string configuration variable */
void set_cfg_str(char *label, char *str_val)
{
	g_key_file_set_value(app_cfg.cfg, cfg_group, label, str_val);
}
#endif

#if	USE_GLIB0
/* get an int variable from configuration */
int  get_cfg_int(char *label,int default_val)
{
	char *cfg_val;
	int dval;

	if (!g_key_file_has_key(app_cfg.cfg, cfg_group, label, NULL)) {
		cfg_val=(char *)g_malloc(20);
		snprintf(cfg_val,20,"%d",default_val);
		g_key_file_set_value(app_cfg.cfg, cfg_group, label, cfg_val);
//		MESG("set_cfg_int: new val for [%s] with default %s",label,cfg_val);
	} else {
		cfg_val = g_key_file_get_value(app_cfg.cfg, cfg_group, label, NULL);
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

	g_key_file_set_value(app_cfg.cfg, cfg_group, label, cfg_val);
	g_free(cfg_val);
}
#endif

#if	USE_GLIB0
/* get a double float variable from configuration */
double  get_cfg_val(char *label,double default_val)
{
	char *cfg_val;
	double dval;

	if (!g_key_file_has_key(app_cfg.cfg, cfg_group, label, NULL)) {
		cfg_val=(char *)g_malloc(20);
		snprintf(cfg_val,20,"%lf",default_val);
		g_key_file_set_value(app_cfg.cfg, cfg_group, label, cfg_val);
	} else {
		cfg_val = g_key_file_get_value(app_cfg.cfg, cfg_group, label, NULL);
	};
	dval = atof(cfg_val);
	g_free(cfg_val);
	return dval;
}
#endif

extern int color_scheme_ind;

#if	USE_GLIB0
/* load configuration */
void load_config()
{
	GError *gerror=NULL;
	char* configdir = NULL;
//	MESG("load_config:");
	/* Config file initialization*/
	app_cfg.cfg = g_key_file_new();

	configdir = g_build_filename( g_get_user_config_dir(), APPLICATION_NAME, NULL );
	if( ! g_file_test( g_get_user_config_dir(), G_FILE_TEST_EXISTS) )
		g_mkdir( g_get_user_config_dir(), 0755 );
	if( ! g_file_test( configdir, G_FILE_TEST_EXISTS) )
		g_mkdir( configdir, 0755 );

	/* Use more standard-conforming path for config files, if available. */
	app_cfg.configfile=g_build_filename(configdir, CONFIGFILE, NULL);
	g_free(configdir);
	
	MESG("load configuration from %s",app_cfg.configfile);

	if (! g_file_test(app_cfg.configfile, G_FILE_TEST_EXISTS)) {
		char *new_file_contents;

		MESG("No config file yet, create an empty one to %s",app_cfg.configfile);
		new_file_contents=g_strconcat("[",APPLICATION_NAME,"]\n", "", NULL);
		g_file_set_contents(app_cfg.configfile, new_file_contents, strlen(new_file_contents), NULL);
		
		g_free(new_file_contents);
	} else {
		if (!g_key_file_load_from_file(app_cfg.cfg, app_cfg.configfile, 0, &gerror)) {
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
#else
extern VAR option_names[];

void load_config()
{
 int i=0;
 MESG("load_config:");
 char *fname = find_file(NULL,CONFIGFILE,0,0);

 if(fname) {
 char **name_array;
 char **value_array;
 char *flag_name;
 int pair_nums=0;
 pair_nums=read_pairs(fname,'=',&name_array,&value_array);
 if(pair_nums) {
	for(flag_name=name_array[i];(flag_name=name_array[i])!=NULL;i++){
		MESG("	- set [%s] to %d",flag_name,atoi(value_array[i]));
		set_btval(flag_name,-1,NULL,atoi(value_array[i]));
	};
 };

 sarray_clear(name_array);
 sarray_clear(value_array);
 };
 set_key_emulation((int)bt_dval("keyboard_emulation"));

 MESG("configuration loaded!");
}
#endif

#if	USE_GLIB0
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
	
	data = g_key_file_to_data(app_cfg.cfg, &len, &gerror);
//	MESG("write cfg file: len=%ld",(long)len);

	if (!data) {
//		MESG("No data to write to key file");
		exit(1);
	}
	MESG("save configuration to %s",app_cfg.configfile);

	g_file_set_contents(app_cfg.configfile, data, len, NULL);	// write to config file

	g_key_file_free(app_cfg.cfg);
	g_free(app_cfg.configfile);
}
#else
void save_config()
{
 char *fname = find_file(NULL,CONFIGFILE,0,1);
 FILE *f = fopen(fname,"w");
 int i=0;
 // MESG("save_config: to [%s]",fname);

 fprintf(f,"[%s]\n",APPLICATION_NAME);
 VAR *var;
 i=0;var=&option_names[i];
 while(var->name) {
	fprintf(f,"%s=%d\n",var->name,(int)bt_dval(var->name));
	i++;var=&option_names[i];
 };
 fclose(f);
}
#endif

int save_keys()
{
 FILE *f1;
 char *fname;
 int stat=0;
 char key_line[MAXLLEN];
 AKEYS *key;

 fname=find_file(NULL,APPLICATION_KEYS,0,1);
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

