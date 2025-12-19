/* 
 	Preference properties init/save functions 
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.
*/


#include "xe.h"

const char cfg_group[] = APPLICATION_NAME;
extern alist *local_key_list;


extern int color_scheme_ind;

extern VAR option_names[];

char *find_file(char *subdir,char *fname,int cflag,int create_if_not_found);
void check_config_dir();

void load_config()
{
 int i=0;
 char *fname = find_file("",CONFIGFILE,0,0);
 check_config_dir();
 if(fname) {
 char **name_array;
 char **value_array;
 char *flag_name;
 int pair_nums=0;
 // fprintf(stderr,"load_config: from [%s]\n",fname);
 pair_nums=read_pairs(fname,'=',&name_array,&value_array);
 if(pair_nums) {
	for(flag_name=name_array[i];(flag_name=name_array[i])!=NULL;i++){
		// set_btval(flag_name,-1,NULL,atoi(value_array[i]));
		set_bt_num_val(flag_name,atoi(value_array[i]));
		// fprintf(stderr,"	- %20s %d\n",flag_name,atoi(value_array[i]));
	};
	// MESG("configuration loaded! (%d) pairs",pair_nums);
	// fprintf(stderr,"configuration loaded! (%d) pairs\n",pair_nums);
 };

 sarray_clear(name_array);
 sarray_clear(value_array);
 };
 set_key_emulation((int)bt_dval("keyboard_emulation"));

}

// check if config dir exists and if not create it!
void check_config_dir()
{
 struct stat s;

 char app_dir[MAXFLEN];
 int status=snprintf(app_dir,sizeof(app_dir),"%s/%s",getenv("HOME"),APPLICATION_DOT_DIR);
 if(status!=strlen(app_dir)) { fprintf(stderr,"home dir name to big!\n");exit(1); };

 status = stat(app_dir,&s);
	// fprintf(stderr,"check %s status=%d\n",app_dir, status);
	// fprintf(stderr,"check dir %d\n",S_ISDIR(s.st_mode));
 if(status == 0 && S_ISDIR(s.st_mode)) return;
 status = mkdir(app_dir,S_IRWXU);
 if(status!=0) {
 	fprintf(stderr,"cannot create dir %s\n",app_dir);
	exit(2);
 };
#if	0
	fprintf(stderr,"new application dir '%s' created\n",app_dir);
	status = stat(app_dir,&s);
	fprintf(stderr,"check %s status=%d\n",app_dir, status);
	fprintf(stderr,"check dir %d\n",S_ISDIR(s.st_mode));
#endif
// MESG("new application home dir created!");
}

void save_config()
{
 char *fname = find_file("",CONFIGFILE,0,1);
 FILE *f = fopen(fname,"w");
 if(f) {
 int i=0;
 MESG("save_confg: to [%s]",fname);
 fprintf(f,"[%s]\n",APPLICATION_NAME);
 VAR *var;
 i=0;var=&option_names[i];
 while(var->name) {
	// fprintf(stderr,"%s=%d\n",var->name,(int)bt_dval(var->name));
	fprintf(f,"%s=%d\n",var->name,(int)bt_dval(var->name));
	i++;var=&option_names[i];
 };
 fclose(f);
 MESG("configuration saved to %s",fname);
 } else {
 	MESG("cannot write to configuration file %s",fname);
 }
}

int save_keys()
{
 FILE *f1;
 char *fname;
 int stat=0;
 char key_line[MAXLLEN];
 AKEYS *key;

 // MESG("save_keys:");
 fname=find_file("",APPLICATION_KEYS,0,1);
 f1=fopen(fname,"w");
 if(f1!=NULL) MESG("save_keys: to [%s]",fname);
 else { MESG("Cannot create save_keys: file [%s]",fname);return 0;};
 lbegin(local_key_list);
 while( (key=(AKEYS *)lget(local_key_list))!=NULL)
 {
 	snprintf(key_line,sizeof(key_line),"%s=%d=%s",key->a_function_name,key->c,key->a_key);
	fprintf(f1,"%s\n",key_line);
 };
 fclose(f1);
 return stat;
}

