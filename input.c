/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

*/
/*	INPUT:	
	keyboard input routines
	and common key tables

	OUTPUT:
	send output to buffer or stdout
*/

#include	"xe.h"
#include	"keytable.h"
#include	"mlang.h"
#include	"display_driver.h"

#if	USE_GLIB
/* Translate output to ISO-8859-7 , IBMPC 737 */
char x_out[2][128] = {
 {
 193,194,195,196,197,198,199,200, 201,202,203,204,205,206,207,208,
 209,211,212,213,214,215,216,217, 225,226,227,228,229,230,231,232,
 233,234,235,236,237,238,239,240,241,243,242,244,245,246,247,248,
 2  ,2  ,2  , 25,22 ,22 ,22 , 12,12 ,22 ,25 ,12 ,11 ,11 ,11 ,12 ,
// 14 , 23, 24, 21, 18,15 ,21 , 21,14 ,13, 23, 24, 21, 18 ,15, 23 ,
 131 , 23, 24, 21, 18,143 ,21 , 21,131 ,13, 23, 24, 21, 18 ,143, 23 ,
// 23 , 24, 24, 14, 14, 13, 13, 15, 15,11 , 13, 2 ,2  ,2 , 2 , 2,
 23 , 24, 24, 131, 131, 134, 134, 143, 143,11 , 134, 2 ,2  ,2 , 2 , 2,
 249,220,221,222,250,223,252,253,251,254,182,184,185,186,188,190,
 191,177, 27, 26,1  , 1 , 1,1,176,183,31,164,1,178,1,1    
 },
 {
 176,177,178,179,180,181,183,183, 184,185,186,187,188,189,190,191, /* 128 */
 192,193,194,195,196,197,198,199, 200,201,202,203,204,205,206,207,
 208,209,210,211,212,213,214,215, 216,217,218,219,220,221,222,223, /* 160 */
 48, 49, 50, 51, 52, 53, 54, 55,  56, 57, 58, 59, 60, 61, 62, 63,  /* 186 */
 0xE5,'A','B',0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E, /* 192 */
 0x8F,0x90,32 ,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x88,0x93,0xE1,0xE2,0xE3,0xE5, /* 208 */
 0xE7,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6, /* 224 */
 0xA7,0xA8,0xAA,0xA9,0xAB,0xAC,0xAD,0xAE,0xAF,0xE0,0xE4,0xE8,0xE6,0xE7,0xE9,255  /* 240 */
 },
};
#endif


char ascii_cap[256] = {
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,
	26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,
	49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,
	72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,
	95,96,
	65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,
	89,90,123,124,125,126,127,
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
	16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
	32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
	'0','+','2','3',' ',' ','A',' ','E','H','I','>','O',' ','Y','V',
	'I','A','B','G','D','E','Z','H','U','I','K','L','M','N','J','o',
	'P','R','S','S','T','Y','F','X','C','V','I','Y','A','E','H','I',
	'Y','A','B','G','D','E','Z','H','U','I','K','L','M','N','J','O',
	'P','R','S','S','T','Y','F','X','C','V','I','Y','O','Y','V',' '
};

KEYTAB *key_table;

int tstr_to_command(char *tstr);
char *cmd_to_tstr(int cmd);
BTNODE *find_bt_element(char *value);	/* find an element on main bt table  */
extern FILEBUF *cbfp;
extern	FUNCS ftable[];			/* name to function table	*/

extern char *key_str1();
extern alist *local_key_list;
extern int new_in_key_list;
extern int in_menu;

char key_string[128];
char * xe_key_name(int c);
int	ctlxc = CNTRL | 'X';		/* current control X prefix char */
char utfokey[16];
int utflen=0;
int	tlist=0 ;
int liston=0;

extern char subname[MAXFLEN];

#define	NOFUNCTION	(int(*)())(-1)

/* input lists */

extern alist *var_list;
extern alist *shell_list;
extern alist *pat_list;
extern alist *macro_list;

inline int normalize(int c);

void set_list_type(int type)
{
	tlist=type;
}

int check_list_type(int type)
{
	return tlist==type;
}


void activate_list_mode()
{
	liston=1;
}

int list_on()
{
	return liston;
}

void list_off()
{
	liston=0;
}

/*
 * Ask a confirmation from the user.
 * Abort is like no.
 */
/* 
 * complete: TODO: show semicompleted list
*/
/* complete a name from a null terminated list */
int complete(char *buf)
{
 FUNCS *fp;
 int i,i1=0,i2=0,j;
 int max_common=0;
 char *elm,*elm1=NULL;
 for(fp=&ftable[0],i=1;fp->n_func!=NULL;fp++,i++) {
	elm=fp->n_name;
 	if(memcmp(buf,elm,strlen(buf))==0) {
		if(i1==0) { i1=i;elm1 = elm;}	// set the index of first found
		else { // another canditat, find the common part
			for(j=0;elm1[j]!=0 && elm[j]!=0;j++) {
				if(elm1[j]!=elm[j]) break;
			};
			if(i2==0) { max_common=j;i2=i;}
			else if(j<max_common) {	
				max_common=j;
				i2=i;	// set onother
			};
		};
	};
 };
 if(i2==0 && i1!=0) {	// this is the only one
 	strlcpy(buf,elm1,MAXFLEN);
	return(strlen(buf));
 } else if(i2>0) {;
 	strlcpy(buf,elm1,MAXFLEN);
	if(max_common<MAXFLEN) 	buf[max_common]=0;
	else buf[MAXFLEN]=0;	/* ERROR !!!!  */
 	return(strlen(buf));
 };
 return 0;
}


/*	GET1KEY:	Get one keystroke. 
 The only prefixs legal here is  CNTRL.
*/
int get1key()
{
 int c0=0;

 c0= drv_getc(0);

 if (c0>=0x00 && c0<=0x1F) {                /* C0 control -> C-     */
                c0 = CNTRL | (c0+'@');
 }
#if	PCURSES
 else {
	int  c=0;
	if(utflen==0 && c0<0xFF ) {
		int n=utf8_countbytes(c0)-1;
		int c1;
		utflen=n+1;
		if(n>0){
			utfokey[0]=c0;
			for(n=1;n<utflen-1;n++) {
				c1=drv_getc(0);
				utfokey[n]=c1;
			};
			c=drv_getc(0);
			utfokey[utflen-1]=c;
			utfokey[utflen]=0;
#if	USE_GLIB
			if(cbfp->b_lang && entry_mode==KNORMAL) {
				char *outkey;
				gsize r,w;
				outkey=g_convert(utfokey,-1,codepage_str[cbfp->b_lang],"UTF-8",&r,&w,NULL);
				if(outkey!=NULL) {
					c0=outkey[0];
					g_free(outkey);
				};
				utflen=0;
				utfokey[0]=0;
			};
#endif
		} else utflen=0;
	};
 };
#endif
// MESG("		-: %d %X [%c]",c0,c0,c0);
 return (c0);
}

extern char *kbdptr;

int set_record_key(int key)
{
	if(!entry_mode) 
		record_key(key);
	return key;
}

/*	
 * GETCMD:	Get a complete command from the keyboard.
 this is just a hack, not a serious way, but it works for most terminal types
*/

int getcmd()
{
	int c;		/* fetched keystroke */
	int	c1=0,c0;
	int n1=0;
	int n2=0;


	if(kbdmode==PLAY) {
		int key;
		key = *kbdptr++;
		return key;
	};

	/* get initial character */

	c = get1key();

	/* process META prefix */
	if (c == CHR_META) {

		c = get1key();

	    if(c<255 && c!='o') c = normalize(c);		/* Force to upper */

		if(c==CHR_LBRA) {
			c = get1key();
			if(text_mouse_key(&c)){	/* this can be a mouse key!!  */
			  return(set_record_key(c));
			};
			if(utflen>0) {
				utflen=0;
				utfokey[0]=0;
				c='0';
			};
			if ( c == CHR_LBRA ) {
				c1 = SPEC;
				c = get1key();
			} else {
			c1=SPEC1;
			if (c == (CNTRL | '@')) {
				 c='@';
			};
// get first numeric
			if(c>='0' && c<='9') {
			  c -= '0';
			  n1 = c;
			  c0=get1key();
			  if(c0>='0' && c0<='9') { // second digit of first numeric
			 	c*=10 ;c+= c0-'0';
				n1 = c;
				 	c0=get1key();
			  } ;
			  c1 |= c;
			  // MESG("c1 = %X",c1);
			  if(c0=='^') { c1 |= 128;}
			  else {
			  	if(c0 ==';') { // we need a second numeric
					c1 = SPEC2;
					c=get1key();
					c -= '0';
					n2 = c; // second numeric first digit
					c0=get1key();
					if(c0 >= 'a') c0 -= 'a';
					else c0 -= 'A';
					// MESG("spec2: c1=%X c=%X c0=%X",c1,c,c0);
					c1 = SPEC2 + 100*n1+32*n2+c0;	
				}
			  }
			  return(set_record_key(c1));
			  };
			} ;
			c1 |= c;
			return(set_record_key(c1));
		} else if(c=='O') {
				c1=SPEC;
			c = get1key();
			/* translate greek chars */
			return(set_record_key(c1|c));
		};

		return(set_record_key(META|c));
	}
	/* process CTLX prefix */
	if((int)bt_dval("keyboard_emulation") == 1){
	if (c == ctlxc) {
		c = get1key();
		// MESG("ctlx %d",c);
		if(c<256) c = normalize(c);
		return(set_record_key(CTLX|c));
	}
	}
	return(set_record_key(c));
}

int isspecialchar(char *st)
{
 while(*st) {
  if(*st == '*' || *st=='?') return(TRUE);
  st++;
 };
 return(FALSE);
}

/* normalize a character */
inline int normalize(int c)
{
	return ascii_cap[c % 256];
}

inline int capital(int c)
{
	if(c>128)	return c;	/* this is needed for utf!  */
	return ascii_cap[c];
}

#if	USE_GLIB
inline int xlate(int lang, int c)
{
	return x_out[lang][c-128];
}
#endif

//returns item of key-function table
KEYTAB *key_item(KEYTAB *key_table,int c)
{
 KEYTAB *kp=NULL;
 for(kp = key_table;kp->k_fp != NULL;kp++){
		if (kp->k_code == c) {
			// MESG("key_item: found k_code=%d [%s]\n",kp->k_code,kp->macro_name);
			break;
		};
 };
 return(kp);
}

int set_key_function2(int (*kfunc)(num), int c,char *name,KEYTAB *keytab)
{
	KEYTAB *ktp;

	if(kfunc == NULL) 	return FALSE;
	ktp = key_item(keytab,c);
	
	if (ktp->k_fp!=NULL) {	/* it exists, just change it then */
		ktp->k_fp = kfunc;
		if(name!=NULL) ktp->macro_name=strdup(name);
	} else {	/* otherwise we need to add it to the end */
		if (ktp >= keytab+MAXASSIGNS) {
			ERROR("key table FULL\n");
			msg_line("key table FULL!");
			return(FALSE);
		}

		ktp->k_code = c;	/* add keycode */
		ktp->k_fp = kfunc;	/* and the function pointer */
		ktp->macro_name=strdup(name);
		++ktp;			/* and make sure the next is null */
		ktp->k_code = 0;
		ktp->k_fp = NULL;
	}
	return(TRUE);
}

/* assign a function to a keyboard control sequence (c) */
int set_key_function(int (*kfunc)(num), int c,char *name)
{
 int stat=0;
	stat += set_key_function2(kfunc,c,name,keytab_emacs);
	stat += set_key_function2(kfunc,c,name,keytab_win);
	if(stat == 2) return(TRUE);
	else return (FALSE);
}

/* assign subroutine to key */
int assign_sub(num n)
{
	int c; /* key to assign the sub */
	char funname[32];
	int(*kfunc)(num);
	int s;
	funname[0]=0;
	// MESG("assign_sub:");
	if((s = nextarg("Assign: subroutine name :", funname, 32,true))!=TRUE) return(s);
//	show_token(cbfp->parser,"assign_sub: after nextarg!");
	kfunc = execsub;
	msg_line("Press the key to assign!");
	c = getckey();
	// show_token(cbfp->parser,"assign_sub: after getkey!");

	// msg_line(xe_key_name(c));
	return(set_key_function(kfunc,c,funname));
}

/* 
 * return the function given the macro name
 */
int (*get_function(char *fname))() 
{
	BTNODE *bte;
	int var_index;

	bte = find_bt_element(fname);
	if(bte) {
		if(bte->node_type==TOK_CMD) {
			var_index=bte->node_index;
			return(ftable[var_index].n_func);
		};
	};
	msg_line("No such Function [%s]",fname);
	return(NULL);
}

void error_skip_token(int index,char *description);

/* ask for a function and a key and bind them */
int assign_function(num n)
{
	int c; /* key to assign the function */
	int (*kfunc)(num n);	/* ptr to the requested function */
	char st[256];
	char func_name[128];
	AKEYS *key_reg;

	/* get the function name */
	set_list_type(LCOM);
	func_name[0]=0;
	nextarg("Assign: function name :",func_name,MAXFLEN,true);
	kfunc = get_function(func_name);
	if (kfunc == NULL) {
		snprintf(st,sizeof(st),"[%s] no such function",func_name);
		msg_line(st);
		if(macro_exec) error_skip_token(302,st);	/* on error reduce second argument!  */
		return(FALSE);
	}
	/* get the key for assignement */
	msg_line("Press the key to assign!");
	c = getckey();

	key_reg=malloc(sizeof(AKEYS));
	key_reg->a_function_name = strdup(func_name);
	key_reg->a_key = strdup(key_string);
	key_reg->c = c;
	add_element_to_list((void *)key_reg,local_key_list);
	new_in_key_list++;

	snprintf(st,sizeof(st),"function assigned to %s",key_string);

	msg_line(st);
	return(set_key_function(kfunc,c,key_string));
}

char **split_2_sarray(char *str,int split_chr);

int load_keys()
{
 FILE *f1;
 char *fname;
 char str_line[MAXLLEN];
 static char name1[MAXFLEN];

 strlcpy(name1,APPLICATION_KEYS,MAXFLEN);
 if((fname=find_file("",name1,0,0)) == NULL) return FALSE;

 f1=fopen(fname,"r");

 while(fgets(str_line,sizeof(str_line),f1)){
	char **a_as;
	int c;
	int (*kfunc)(num n);	/* ptr to the requested function */
	AKEYS *key_reg;

	strtok(str_line, "\n");
	a_as = split_2_sarray(str_line,'=');
	c = atoi(a_as[1]);
	kfunc = get_function(a_as[0]);
	if (kfunc != NULL) {
		key_reg=malloc(sizeof(AKEYS));
		key_reg->a_function_name = strdup(a_as[0]);
		key_reg->a_key = strdup(a_as[2]);
		key_reg->c = c;
		add_element_to_list((void *)key_reg,local_key_list);
		new_in_key_list++;
		set_key_function(kfunc,c,a_as[2]);
	};

	free_sarray(a_as);
 };

 return 1;
}

/* ask for a key and remove assignement */
int unassign_key(num n)
{
	int c;		
	KEYTAB *ktp;
	/* get the key to unassign */
	if(macro_exec) { 	/* works only in interactive mode  */
		char st[8];
		nextarg("key",st,8,true);
		return 0;
	} else {
		msg_line("Unassign key :");
		c = getckey();	
	};
	ktp=key_item(key_table,c);
	/* if no assignement, complain */
	if (ktp->k_fp == NULL) {
		msg_line("[Key not assinged to a function]");
		return(FALSE);
	};
	ktp->k_fp=NOFUNCTION;

	msg_line("Key %s unassigned",xe_key_name(c));
	return(TRUE);
}

extern int nnarg;

int insert_money_symbol(num n)
{
	// MESG("insert_money_symbol:");
	while(nnarg-- >0)	{insert_string(cbfp,"€",3);}
	set_update(cwp,UPD_EDIT);
	return true;
}

/* Show all assignements in a buffer */
int show_keys(num n)
{
	register KEYTAB *ktp;	/* pointer into the key table */
	register FUNCS *ftp;	/* pointer into the function table */
	register FILEBUF *bp;	/* file buffer to use */
	int is_assigned;	
	KEYTAB *ktbs[5];		/* tables to search */
	char *table_name[] = {"Normal mode","dir mode","view mode","notes mode",NULL};
	int table;
	char sline[MAXLLEN];
	char *description;
	// int emulation = get_cfg_int("keyboard_emulation",0);
	int emulation = (int) bt_dval("keyboard_emulation");
	char *emulation_name[] = {"Native","Micro Emacs",NULL};
	
#if	TNOTES
	int max_keytables=4;
#else
	int max_keytables=3;
#endif
	if (split_window(1) == FALSE)	return(FALSE);
	bp = new_filebuf("[Keyboard list]", FSINVS);

	EmptyText(bp);
	select_filebuf(bp);

	ktbs[0]=key_table;
	ktbs[1]=keytab_dir;
	ktbs[2]=keytab_view;
#if	TNOTES
	ktbs[3]=keytab_notes;
#endif
	if(emulation>1) emulation=0;
	// show by function
	insert_string_nl(bp,"## Key assignements sorted by function (all key tables)");
	snprintf(sline,sizeof(sline),"## emulation %s",emulation_name[emulation]);
	insert_string_nl(bp,sline);
	insert_string_nl(bp,"");
	
	insert_string_nl(bp,"| function name          | key shortcut                  | Mode       | description                                    |");
	insert_string_nl(bp,"| ---------------------- | ----------------------------- | -----------| ---------------------------------------------- |");
	for (ftp = ftable;ftp->n_func != NULL;ftp++) {
		is_assigned=0;
		/* search in keytab */
		for(table=0;table<max_keytables;table++)

		for (ktp = ktbs[table];ktp->k_fp != NULL;ktp++) {
			if (ktp->k_fp == ftp->n_func) {
				if(ktp->k_fp==NOFUNCTION) continue;
				snprintf(sline,sizeof(sline),"| %-22s | %-15s(%12s) | %11s | %s",ftp->n_name,ktp->macro_name,cmd_to_tstr(ktp->k_code),table_name[table],ftp->n_help);
				insert_string_nl(bp,sline);
				is_assigned=1;
			}
		}

		if (!is_assigned) {
			snprintf(sline,sizeof(sline),"| %-22s |      -                        | -           | %s",ftp->n_name,ftp->n_help);
			insert_string_nl(bp,sline);
		}
	}


	// show by key
	insert_string_nl(bp,"\n## Sorted by key");
	for(table=0;table<max_keytables;table++) {
		snprintf(sline,sizeof(sline),"### %s",table_name[table]);
		insert_string_nl(bp,sline);

		insert_string_nl(bp,"| key shortcut    | macro name           | description                          |");
		insert_string_nl(bp,"| --------------- | -------------------- | ------------------------------------ |");
		for (ktp = ktbs[table];ktp->k_fp != NULL;ktp++) {
			{
				if(ktp->k_fp==NOFUNCTION) continue;
				
				if(ktp->k_fp==execsub) snprintf(sline,sizeof(sline),"| %-15s | %-sizeof(20s |  %s",cmd_to_tstr(ktp->k_code),ktp->macro_name,"subroutine");
				else snprintf(sline,sizeof(sline),"| %-15s | %-20s |  %s",ktp->macro_name,function_name(ktp->k_fp,&description),cmd_to_tstr(ktp->k_code));
				insert_string_nl(bp,sline);
				is_assigned=1;
			}
		}
	}
	cbfp->b_state |= FS_VIEW;
	cbfp->b_state &= ~FS_CHG;
	set_Offset(0);
	set_hmark(1,"show_keys");
	update_status();
	return(TRUE);
}

/* get a command key sequence */
int getckey()
{
	int c;
	// MESG("getckey: macro_exec=%d",macro_exec);
	if (macro_exec) {
		strlcpy(key_string,key_str1(),128);
		return (tstr_to_command(key_string));
	};
	c = getcmd();
	strlcpy(key_string,xe_key_name(c),128);
	return(c);
}


// command key to terminfo string
char *cmd_to_tstr(int cmd)
{
 static char tstr[MAXSLEN];
 char st[2];
 int c0,c1;
 tstr[0]=0;
 c0 = cmd & 0xFF;
 if(cmd & SPEC) { 
 	sprintf(tstr,"ALT-O%c",c0);
	return(tstr);
 };
 if(cmd & SPEC1) {
 	if(c0<128) c1='~';
	else { c1='^';c0-=128;};
	if(c0<64) sprintf(tstr,"ALT-[%0d%c",c0,c1);
	else sprintf(tstr,"ALT-[%c",c0);
	return(tstr);
 };
 /* ALT-N starting key codes (Unixware)  */
 if(cmd & SPEC2) {
 	sprintf(tstr,"spec2 %d",c0);
	return(tstr);
 };
 if(cmd & KMOUS) {
 	sprintf(tstr,"mouse %d",c0-KMOUS);
	return(tstr);
 };
// if(cmd & META) strlcat(tstr,"ALT-",MAXSLEN);
 if(cmd & META)  strlcat(tstr,"ALT-",MAXSLEN);
 if(cmd & CTLX) {
 	sprintf(tstr,"^X%c",c0);
	return(tstr);
 };
 if(cmd & CNTRL || cmd==128) { 
 	st[0]=normalize(c0);
 	strlcat(tstr,"CTRL-",MAXSLEN);
 } else {
 	st[0]=c0;
 };
 st[1]=0;
 strlcat(tstr,st,64);
 return(tstr);
}

int set_key_emulation(num emulation)
{
	// MESG("set_key_emulation: %d",emulation);
	set_bt_num_val("keyboard_emulation",emulation);
	if(emulation == 1) key_table = keytab_emacs;
	else key_table = keytab_win;
	return emulation;
}

/*	Return the function assigned to a key in current key assignement table. */
/* if f is 1 then looks only the main key table */
int (*key_function(int c,int f))()
{
	KEYTAB *ktp;
	FILEBUF *fp=cbfp;
	// MESG("key_function: c=%X f=%d b_flag=%X %X",c,f,fp->b_flag,fp->b_state);
	if( fp->b_flag & FSDIRED && f && fp->b_flag & FSNLIST) {
		ktp = key_item(keytab_dir,c);
		if(ktp->k_fp !=NULL && ktp->k_fp!=NOFUNCTION) return(ktp->k_fp);
		// MESG("FSDIRED: no key!");
	}
#if	TNOTES
	 else if( (fp->b_flag >= FSNOTES) && f) { // ???
		ktp = key_item(keytab_notes,c);
		if(ktp->k_fp !=NULL && ktp->k_fp!=NOFUNCTION) return(ktp->k_fp);
	};
#endif
	if( (fp->b_state & FS_VIEW) && f) {
		ktp = key_item(keytab_view,c);
		if(ktp->k_fp !=NULL && ktp->k_fp!=NOFUNCTION) return(ktp->k_fp);
		// MESG("FS_VIEW: no key!");
	};
    
	ktp = key_item(key_table,c);
	if(ktp->macro_name==NULL) return NULL;
	strlcpy(subname,ktp->macro_name,MAXFLEN);
	if(ktp->k_fp !=NULL && ktp->k_fp!=NOFUNCTION) return(ktp->k_fp);
	return NULL;
}

/*	Returns the key name */
char * xe_key_name(int c)
{
	KEYTAB *ktp;
	ktp = key_item(key_table,c);
	if(ktp==NULL) return (cmd_to_tstr(c));
	if(ktp->macro_name==NULL) return (cmd_to_tstr(c));
	if(ktp->macro_name[0]==0) return (cmd_to_tstr(c));
	return(ktp->macro_name);
}

/* 	
 Returns the macro name of a function.
*/
char *function_name(int (*func)(),char **description)
{
	FUNCS *ftp;

	for(ftp=ftable;ftp->n_func != NULL;ftp++) {
		if (ftp->n_func == func) {
			*description=ftp->n_help;
			return(ftp->n_name);
		};
	};
	*description=NULL;
	return("");
}

/*  
 Convert a terminfo key string description to command key 
*/
int tstr_to_command(char *tstr)
{
 int c=0,cnum=1;
 char *schar;
 for(schar=tstr;*schar!=0;schar++) {
	if(c & SPEC) {
		if(*schar >='0' && *schar<='9') {
			cnum *= *schar;
			continue;
		};
		if(*schar =='~') { c |= cnum; break;};
		if(*schar =='^') { c |= cnum+128; break;};
	};
	if(*schar=='M') {
		schar++;
		if(*schar=='-') { c |= META; continue;};
		c += 'M';
	};
 	if(*schar=='\\') { schar++;
		if(*schar=='E'){ 
			if(*(schar+1)==CHR_LBRA) { c |= SPEC;schar++;continue;};
			c |= META;continue;
		};
		// error ??
	};
	if(*schar=='^') { 
		schar++;
		if(*schar=='X') { c |= CTLX; continue;};
		c |= CNTRL;
		c += normalize(*schar);
		continue;
	};
	if(*schar=='F') {
		schar++;
		if(*schar=='N') { c |= SPEC; continue;};
		if(*schar==CHR_LBRA) { c |= SPEC1; continue;};
		c += 'F';
		continue;
	};
	if(c & META && *schar=='O') {c |= SPEC;continue;};
	c += *schar;
 };
 return(c);
}


/* ask for and 'insert' a string into the current file
   at the current point. Mainly used by macros.
*/
int istring(num n)	
{
	register int status;	/* status return code */
	char tstring[MAXLLEN];	/* string to add */
	// MESG("istring: n=%d",n);
	if (n < 0)	return FALSE;
	/* ask for string to insert */
	tstring[0]=0;
	status = nextarg("String to insert: ", tstring, MAXLLEN,true);
	// MESG("	string to insert [%s] status=%d",tstring,status);
	if (status != TRUE)	return(status);

	/* insert it, no newlines allowed */
	while (n--) {
		if(!insert_string(cbfp,tstring,strlen(tstring))) return(FALSE);
	}
	return(TRUE);
}

char *get_last_in_list(int tlist)
{
 alist * hl;
 char *item=NULL;
 switch(tlist) {
	case LVAR: hl=var_list;break;			/* user variable list */
	case LSHELL: hl=shell_list;break;		/* shell command */
	case LSRC: hl=pat_list;break;			/* search list, previous pattern */
	case LMAC: hl=macro_list;break;		/* macro command */
 	default:
		return NULL;
 };
 item=lget_last(hl);
 if(item!=NULL) return item;
 else return("");
}


void list_select(char *s,int dire)
{
 alist *hl=NULL;
 char *s1;
 int file_flag=0;
 // MESG("list_select: tlist=%d",tlist);
 switch(tlist) {
 case LDIR:{
 	if(is_scratch_buffer(cbfp)) {
		if(chdir(get_start_dir())){
			error_line("cannot get start dir!");
			return;
		};
	};
	set_list_type(LDIR);
	list_dir1(s);
	return;			/* directory list */
	};

 case LBUF:list_filebuf("File list",s,2,&file_flag);return;		/* file buffer list */
 case LCOM:complete(s);return;			/* command list */
 case LVAR: hl=var_list;break;			/* user variable list */
 case LSHELL: hl=shell_list;break;		/* shell command */
 case LSRC: hl=pat_list;break;			/* search list, previous pattern */
 case LMAC: hl=macro_list;break;		/* macro command */
	default:
	return;
 };

 switch(dire) {
 	case 0: s1=lget_last(hl); 
			if(s1) strlcpy(s,s1,MAXLLEN); 
			break;
	case -1: s1=lget_previous(hl); 
		if(s1) strlcpy(s,s1,MAXLLEN); 
			break;
	case 1: s1=lget_next(hl); 
		if(s1) strlcpy(s,s1,MAXLLEN);
		break;
 };
}

int list_update(char *s)
{
 alist *hl;
 switch(tlist) {
 case LVAR: hl=var_list;break;		/* user variable list */
 case LSHELL: hl=shell_list;break;		/* shell command */
 case LSRC: hl=pat_list;break;		/* search list, previous pattern */
 case LMAC: hl=macro_list;break;		/* macro command */
 default: return 0;
 };
 if(s[0]==0) return(0);
 if(hl->last!=NULL) {
	 if(strcmp(hl->last->data,s)==0)  return(0);
 };
 if(add_element_to_list(strdup(s),hl)!=NULL) return TRUE;
 else return FALSE;
}

void init_input()
{
 alist *hl;
 switch(tlist) {
 case LVAR: hl=var_list;break;		/* user variable list */
 case LSHELL: hl=shell_list;break;		/* shell command */
 case LSRC: hl=pat_list;break;		/* search list, previous pattern */
 case LMAC: hl=macro_list;break;		/* macro command */
 default: return;
 };
 set_last(hl);
}


/* --- output funtions --- */

extern alist *window_list;


void out_print(char *s,int nl)
{
	static FILEBUF *bp=NULL;
	time_t t0;
	static time_t ptime=0;
	// FILEBUF *oldfp = cbfp;
	// MESG("out_print: current buffer is [%s] discmd=%d",cbfp->b_fname,discmd);
	if(s==NULL) return;
#if	0
	if(!discmd) return ;
#else
	if(!discmd) {
		if(debug_flag()) {
			if(nl) fprintf(stderr,"%s\n",s);
			else fprintf(stderr,"%s",s);
		} else {
			if(nl) fprintf(stdout,"%s\n",s);
			else fprintf(stdout,"%s",s);
		}
		return;
	};
#endif
	if(debug_flag()) {
		if(nl) fprintf(stderr,"%s\n",s);
		else fprintf(stderr,"%s",s);
	};

	if((bp=new_filebuf("[out]",0)) !=NULL) {
		if(!activate_file(bp)) return;
	} else {
		msg_log(1,"cannot create output buffer [out][%s]",s);
		return;
	};

	// insert string
	insert_string(bp,s,strlen(s));
	if(nl) insert_newline(bp);
	// sfb(oldfp);

	time(&t0);

	if(t0-ptime>2) {
		WINDP *wp;
		lbegin(window_list);
		while((wp=lget(window_list))!=NULL){
			if(wp->w_fp==bp) {
				upd_all_virtual_lines(wp,"out_print");
				wp->w_flag=UPD_WINDOW;
				draw_window(1,wp,"out_print");
			};
		};
		events_flush();
		ptime=t0;
	};
}

void out_fprint(char *bname, char *s,int nl)
{
	FILEBUF *bp;

	if(s==NULL) return;
	if((bp=new_filebuf(bname,0)) !=FALSE) {
		// FILEBUF *oldfp = cbfp;

		if(!activate_file(bp)) return;

		// sfb(bp);

		// insert string
		insert_string(bp,s,strlen(s));
		if(nl) insert_newline(bp);
		// sfb(oldfp);


	} else {
		msg_log(1,"cannot create %s buffer",bname);
	};
}


/* -- */
