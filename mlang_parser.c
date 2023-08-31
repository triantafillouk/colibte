/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

	interpreter parser
*/

#define	TEST_PARSER	0

void set_tok_table(FILEBUF *bf, TLIST lex_parser);
void skip_space1(FILEBUF *bf);

offs foffset=0;

/* get next character to parse */
int getnc1(FILEBUF *bf, int *cc, int *cmask)
{
 if(!FEofAt(bf,foffset)) {
 	*cc=FCharAt(bf,foffset++);
	*cmask=tok_mask[*cc];

	if(*cc==0) return(0);
	// MESG("	get nc1 [%c] type=%d",*cc,*cmask);
	return(1);
 } else {
 	return(0);
 };
}

/* Find next token type  */
int next_token_type(FILEBUF *bf)
{
 int c1=0;
 skip_space1(bf);

 if(!FEofAt(bf,foffset)) {
 	c1=tok_mask[FCharAt(bf,foffset)];
	// MESG("	ntt=%d",c1);
 } else {
 	c1=TOK_NONE;
 };
 return(c1);
}

#define ADD_TOKEN tok=add_token(lex_parser,tok_type,cc,nword);

tok_struct *add_token(TLIST lex_parser,int tok_type,int cc,char *label)
{
 tok_struct *tok=NULL;

	tok=new_tok();
	add_element_to_list((void *)tok,lex_parser);
	tok->tnum=lex_parser->size-1;
	// MESG("	; add token %3d: ind=%d type=[%s] [%s]",tok->tnum,cc,tname(tok_type),label);
 return tok;
}

/* skip one line or till next separator */
void skip_line1(FILEBUF *bf,int cc)
{
 int nc;
 int cmask;
 while(!FEofAt(bf,foffset)) 
 {
 	nc=FCharAt(bf,foffset);
	cmask=tok_mask[nc];
	if(cmask!=TOK_NL) return;
	if(nc==cc) return;
	foffset++;
 };
}


BTNODE * add_to_symbol_tree(BTREE *stable,char *name,int type)
{
	BTNODE *btn=NULL;
	/* create the variable in the symbol_table!  */
	// MESG("add_to_symbol_tree: %s table items=%d",name,stable->items);
	btn=add_btnode(stable,name);
	if(stable->new_flag) {
		btn->node_type=type;
#if	1
		btn->node_vtype=VTYPE_NUM;
		btn->node_dval=0;
		btn->node_index=stable->items-1;	/*  variable index  */
#else
		btn->node_val=0;
		btn->node_index=stable->items-1;	/*  variable index  */
		btn->sval=NULL;
#endif
		stable->new_flag=0;
		// MESG("- this is new! index=%d",btn->node_index+1);
	};
	return btn;
}

/* skip up to new line */
void skip_2nl(FILEBUF *bf)
{
 int nc,cmask;

 while(!FEofAt(bf,foffset))
 {
 	nc=FCharAt(bf,foffset);
	cmask=tok_mask[nc];
	if(cmask==TOK_NL) {
		return;
	};
	foffset++; 
 };
}

/* get next word, return length */
int getnword1(FILEBUF *bf, int cc,char *word)
{
 int nc=0;int cmask=0;
 char *str=word;
 int len=1;
 *str++ = cc;
 while(!FEofAt(bf,foffset))
 {
 	nc=FCharAt(bf,foffset);
	cmask=tok_mask[nc];
	// MESG("	gnw: %d [%c]",cmask,nc);
	if(cmask!=TOK_LETTER && cmask!=TOK_NUM) break;
	if(len<255) { *str++ = nc; len++;};
	foffset++;
 };
 *str=0;
 // MESG("getnword [%s] len=%d cmask=%d",word,len,cmask);
 return(len);	// returns string length
}

/* get next quoted string */
int getnstr1(FILEBUF *bf, int cc, char *str)
{
 int nc;
 char *s=str;
 int len=0;
 int pbslash=0;
 while(!FEofAt(bf,foffset))
 {
	nc=FCharAt(bf,foffset);
	foffset++;
	if(nc==CHR_BSLASH) { pbslash=1;continue;};
	if(nc==cc && pbslash==0) break;
	if(nc==CHR_LINE) break;
	if(nc=='@') break;
	// printf("%c %d ",nc,nc);
	*s++ = nc;len++;
	pbslash=0;
 };
 // printf("\n");
 *s=0;
 return(len);
}

/* skip white space */
void skip_space1(FILEBUF *bf)
{
 int nc,cmask;
 while(!FEofAt(bf,foffset))
 {
 	// MESG("	ss");
	nc=FCharAt(bf,foffset);
	cmask=tok_mask[nc];
	if(cmask!=TOK_SPACE) return;
	foffset++;
 }
}

/* get next number, return it as double */
double getnum1(FILEBUF *bf, int cc,tok_struct *tok)
{
 int nc,cmask;
 int base=DEC;
 double v2;
 double vbd=0;
 double vad=0;
 double mul=DEC;
 int after_dot=0;
 // MESG("getnum1: cc=%c",cc);
 if(cc=='0') {
 	if(!FEofAt(bf,foffset)){
		nc=FCharAt(bf,foffset);
		cmask=tok_mask[nc];

		if(cmask!=TOK_NUM) {
			if(nc=='x'||nc=='X') base=HEX;
			else if(nc=='b'||nc=='B') base=BIN;
			else if(nc=='o'||nc=='O') base=OCT;
			else if(nc=='d'||nc=='D') base=DEC;
			else if(nc=='.') base=DEC;
			else {
				if(cmask==TOK_LETTER) set_error(tok,101,"numeric error notation");
				return(0);
			};
			if(nc!='.') foffset++;
		};
	} 
 } else {
	vbd=cc-'0'; 
 };
 mul=base;

 while(!FEofAt(bf,foffset)) {
	
	nc=FCharAt(bf,foffset);
	cmask=tok_mask[nc];
	if(nc=='.'/* ||nc==','*/) {
		if(after_dot==0) {
			after_dot=1;
		} else { 
			set_error(tok,102,"numeric error. Multiple dots");
			return(vbd+vad);
		};
	} else
	if(cmask==TOK_LETTER){
		if(base==HEX) {
			if(nc>='A'&&nc<='F') v2=nc-'A'+10;
			else if(nc>='a'&&nc<='f') v2=nc-'a'+10;
			else { 
				set_error(tok,103,"numeric error in hex notation");
				return(vbd+vad);
			}
		} else { 
			// MESG("nc = [%c]",nc);
			set_error(tok,104,"wrong variable name, must start with a letter");
			return(vbd+vad);
		}
		if(after_dot) {
			vad += v2/mul;
			mul = mul*base;
		} else {
			vbd = vbd*base+v2;
		};
	} else if(cmask==TOK_NUM) {
		v2=nc-'0';
		if(v2>base-1) 
		{
			set_error(tok,105,"numeric error. invalid number");
			return(vbd+vad);
		}
		if(after_dot) {
			vad += v2/mul;
			mul = mul*base;
		} else {
			vbd = vbd*base+v2;
		};
	} else { // end of num
		return(vbd+vad);
	};
	foffset++;
 } 
 return(vbd+vad);
}

void set_var(BTREE *stree, tok_struct *tok, char *name)
{

	// BTNODE *btn = find_btnode(stree,name);
	// if(btn==0) MESG("	set_var: [%s] not found in stree!");else MESG("	set_var: [%s] found in stree!");
	BTNODE *btn=add_to_symbol_tree(stree,name,TOK_VAR);
	tok->tind=btn->node_index;
	tok->ttype=btn->node_type;
	ex_edenv=tok->ttype;
		// MESG("	set_var: new name=%s tind=%d",name,tok->tind);
	if(stree->max_items < tok->tind) ERROR("exceeded item list of %d !! CHECK!",stree->max_items);
}

int change_script_state(int tok_type,int *script_active) 
{
	if(*script_active) {
		if(tok_type==TOK_BQUOTE) { 
			*script_active=0;
			return 1;
		}; 
	} else {
		if(tok_type==TOK_BQUOTE) {
			*script_active=1;
			return 1;
		};
		return 1;
	};
	return 0;
}

int type_init_definition(FILEBUF *bf, alist *lex_parser, tok_struct *tok_var)
{
 int tok_type=TOK_NONE;
 int cc=0;
 char nword[256];
 int slen=0;
 // MESG("type_init_definition: num=%d name=%s",token_var->tind,token_var->tname);

 tok_type=next_token_type(bf);
 if(tok_type!=TOK_LETTER) { 
	set_error(tok_var,108,"type_init_definition parse error");
 	return 0;
 };
 
 getnc1(bf,&cc,&tok_type);

 slen=getnword1(bf,cc,nword);	/* this is the type name  */
 if(show_tokens){
	out_print("---- type definition [",0);
	out_print(nword,0);
	out_print("] ---------",1);
 };
 BTREE *type_dat = new_btree(nword,100);

 BTNODE *type_node = add_btnode(bf->type_tree,nword);
 MESG("Add new type named [%s]",nword);
 if(type_node && bf->type_tree->new_flag) {
 	type_node->node_vtype=VTYPE_TREE;
	type_node->node_dat = type_dat;
	bf->type_tree->new_flag=0;
 } else {
 	set_error(tok_var,108,"duplicate type or other error");
	return 0;
 };

 // MESG("	ttd 2: %d [%s]",slen,nword);
 skip_space1(bf);
 getnc1(bf,&cc,&tok_type);	/* left_par or left braket  */

 // MESG("	ttd 3: %d [%c]",tok_type,cc);
 if(tok_type == TOK_ASSIGN) {
 	getnc1(bf,&cc,&tok_type);	/* '=' set to optional ??  */
	skip_space1(bf);
	 // MESG("		found assing, skipped!");
	getnc1(bf,&cc,&tok_type);
 };
 if(tok_type!=TOK_LPAR && tok_type!=TOK_LBRAKET) { 
 	set_error(tok_var,110,"type_init_definition left par,left braket");
 	return 0; 
 };
 int end_type=0;
 if(tok_type==TOK_LPAR) end_type=TOK_RPAR;else end_type=TOK_RBRAKET;

 int ind=0;
 while(next_token_type(bf)!=end_type) {
 	getnc1(bf,&cc,&tok_type);
	// MESG("	w: cc=[%c] type=%d",cc,tok_type);
	if(tok_type!=TOK_LETTER) { 
		set_error(tok_var,111,"type_init_definition name not letter or closing error!");
	return 0;};
	slen=getnword1(bf,cc,nword);
	
	// MESG("	type add element [%s]",nword);
	BTNODE *node = add_btnode(type_dat,nword);
	if(show_tokens) {
		out_print(nword,0);
		out_print("		",0);
	};
	node->node_index = ind++;
	if(type_dat->new_flag==0) {
		set_error(tok_var,112,"type_init_definition: dublicate definition!");
		return 0;
	};
	getnc1(bf,&cc,&tok_type);
	// MESG("	w: %d [%c]",tok_type,cc);
	if(cc==':')  {
		getnc1(bf,&cc,&tok_type);
		if(cc==':') {
		// MESG("	w2: %d [%c]",tok_type,cc);
		getnc1(bf,&cc,&tok_type);
		};
		// MESG("	w3: %d [%c]",tok_type,cc);
		if(tok_type==TOK_NUM) {
			double val=getnum1(bf,cc,tok);
			// MESG("	element numeric %f",val);
			if(show_tokens) out_print("numeric",1);
			node->node_vtype=VTYPE_NUM;
			node->node_dval=val;			
		} else if (tok_type==TOK_QUOTE) {
			getnstr1(bf,cc,nword);
			// MESG("	element string [%s]",nword);
			if(show_tokens) out_print("string",1);
			node->node_vtype=VTYPE_STRING;
			node->node_sval=strdup(nword);
		} else {
			MESG("	parse type definition error!");
			set_error(tok_var,113,"type_init_definition parse error");
			return 0;
		};
		if(next_token_type(bf)==TOK_COMMA) getnc1(bf,&cc,&tok_type);
	} else if(cc!=' '&&cc!=9 && cc!=',') { return 0;}; 
 };
 getnc1(bf,&cc,&tok_type);
 if(tok_type != end_type) 
 {
 	set_error(tok_var,114,"type_init_definition rpar,rbracket not found!");
	return 0;
 }
 if(show_tokens) out_print("----------------------------------------",1);
 // show_bt_table_ordered(type_dat);
 return 1;
}

/*
 parse a file buffer,
 create/renew the alist of tokens found
*/
int parse_block1(FILEBUF *bf,BTREE *use_stree,int init,int extra)
{
 double value=0;
 int cc=0;
 char nword[256];
 int slen=0;
 int is_storelines=0;
 int tok_line=1;
 int curl_level=0;
 int is_now_sep=0;
 int is_now_curl=0;
 int par_level=0;
 int list_elements=0;
 int in_type_definition=0;
 int after_rpar=0;
 TLIST curl_stack; // curl stack
 int store_level=0;
 int save_stage_level;
 offs start_proc_offset=0;
 offs ddot_offset=0;
 char *proc_name=NULL;
 alist *lex_parser=NULL;
 BTREE *stree=use_stree;
 tok_struct *tok=NULL;
 // tok_struct *previous_token=NULL;
 int previous_ttype=0;

 int tok_type=0;
 int next_tok_type=0;
 int script_active=0;
 tok_struct *array_tok=NULL;
 int skip_token=0;

 // return if already parsed and not forced to parse
 if(bf->tok_table !=NULL && init==0) return (0);

 // MESG("parse_block1: file_type=%d [%s]",bf->b_type,bf->b_fname);
 if(is_mlang(bf)) script_active=1;	/* initial script state  */

 if(init && bf->tok_table!=NULL) {
 	/* we must free the previous parse  */
	// MESG("parse_block1: table is not NULL, free it!");
	free(bf->tok_table);
	bf->tok_table=NULL;
 };
 lex_parser=new_list(0,"lex_parser");

 if(init==1 || stree==NULL) {	/* create a new symbo table if needed  */
	if(bf->symbol_tree) free_btree(bf->symbol_tree);
	bf->symbol_tree=new_btree(bf->b_fname,0);
	stree=bf->symbol_tree;
	stree->max_items=999999;
 };
 // MESG("parse_block1: pos2");
 {	/* clear ddot textpoints  */
	TextPoint *scan,*nscan;
	for(scan=bf->tp_last; scan;) {
		nscan=scan->t_next;
		if(scan->tp_type == TP_DDOT) textpoint_delete(scan);
		scan=nscan;
	};
 };

 foffset=0;	/* goto to the beginning of the buffer  */
 curl_stack=new_list(0,"curles_stack"); // create curles stack 
 // MESG("parse_block1: before looping: script_active=%d",script_active);
 err_num=0;
 err_line=0;

 save_stage_level=stage_level;

 MESG("--- Start parsing block loop --------------------");
 while(getnc1(bf,&cc,&tok_type))
 {
	if(change_script_state(tok_type,&script_active)) continue;

// 	MESG("parse- cc=%d %c type=%3d [%10s]",cc,cc,tok_type,tname(tok_type));

	if(err_num>0) return 0.0;

	value=0;
	nword[0]=0;
	switch(tok_type) {
		case TOK_SEP:
			if(is_now_sep) continue;
			else {
				if(is_now_curl) continue;
				is_now_sep=1;
				// MESG("TOK_SEP: 1");
				break;
			};

		case TOK_NL: 
			tok_line++;
			// MESG("		line %d [%c]",tok_line,cc);
			last_correct_line=tok_line;
			skip_line1(bf,cc);
			if(is_now_sep || after_rpar) continue;
			is_now_sep=1;
			tok_type=TOK_SEP;
			// MESG("TOK_NL->TOK_SEP");
			break;
		case TOK_SPACE: 
			skip_space1(bf);
			continue;
		case TOK_LETTER: 
			slen=getnword1(bf,cc,nword);
			// MESG("parse: TOK_LETTER 		[%s]",nword);
			break;
		case TOK_NUM:
			// MESG("TOK_NUM: old num=%d type=%d name %s,new type %d ",tok->tnum,tok->ttype,tok->tname,tok_type);
			value=getnum1(bf,cc,tok);
			// MESG("parse: TOK_NUM: num=%d type=%d val=%f",tok->tnum,tok->ttype,value);
			if(err_num>0) return(0);
			break;
		case TOK_LCURL:
			is_now_curl=1;
			curl_level++;cc=0;
			// MESG("left_curl: level=%d",curl_level);
			break;
		case TOK_RCURL:
			{ 
				curl_level--;cc=1;
				if(curl_level<0) {err_num=102;err_str="curls dont match!";return(0);};
				if(curl_level==store_level && is_storelines) {
					// MESG(" TOK_RCURL: go create_function_buffer");
					create_function_buffer(bf,proc_name,start_proc_offset,foffset);

					tok->ttype=TOK_SEP;
					tok->tind='p';	// ??
					is_now_sep=1;
					is_storelines=0;
					// restore stage_level
					free(proc_name);
					proc_name=NULL;
					continue;
				} else {
					int tt=1;
					while(1){	/* remove all separators,white space after rcurl  */
						tt=next_token_type(bf);
						if(tt!=TOK_SEP && tt!=TOK_NL && tt!=TOK_SPACE) {
							break;
						};
						getnc1(bf,&cc,&tok_type);
						if(tok_type==TOK_NL) tok_line++;
					};
					tok_type=TOK_RCURL;
				};
			};
			break;
		case TOK_LPAR:
			par_level++;
			cc=0;

			if(previous_ttype==TOK_DIR_IF
			 ||previous_ttype==TOK_FUNC
			 ||previous_ttype==TOK_DIR_WHILE
			 ||previous_ttype==TOK_DIR_FOR
			 ||previous_ttype==TOK_PROC
			 ||previous_ttype==TOK_CMD
			 ||previous_ttype==TOK_DIR_FORI
			 ||previous_ttype==TOK_DIR_RETURN
			) {
				// MESG("parse: TOK_LPAR skip");
				previous_ttype=0;
				continue;
			}
			else break;
		case TOK_RPAR:
			par_level--;
			cc=1;break;
		case TOK_LBRAKET:
			// MESG("	parser: TOK_LBRAKET skip=%d",skip_token);
			if(skip_token) {
				skip_token=0;
				continue;
			};
			cc=0;
			break;
		case TOK_RBRAKET:
			// MESG("end of array definition cols=%d rows=%d",array_max_cols,array_rows);
			cc=1;
			if(next_token_type(bf)==TOK_LBRAKET) {
				// if(array_tok->ttype==TOK_ARRAY2) 
				array_tok->ttype=TOK_ARRAY2;
				array_tok->tname="index2";
				skip_token=1;
			};
			break;
		case TOK_QUOTE: // string start
			slen=getnstr1(bf,cc,nword);
			break;
		case TOK_SHOW:
			if(next_token_type(bf)==TOK_SHOW) {
				getnc1(bf,&cc,&tok_type);
				tok_type=TOK_DEFINE_TYPE;
				break;
			};
			ddot_offset=foffset;
			skip_2nl(bf);
			break;
		case TOK_COMMENT:
			skip_2nl(bf);
			continue;

		case TOK_NOT:
			next_tok_type=next_token_type(bf);
			if(next_tok_type==TOK_ASSIGN) {
				getnc1(bf,&cc,&tok_type);
				tok_type=TOK_NOTEQUAL;
			};
			if(next_tok_type==TOK_AND) {
				getnc1(bf,&cc,&tok_type);
				tok_type=TOK_NAND;
			};
			if(next_tok_type==TOK_XOR) {
				getnc1(bf,&cc,&tok_type);
				tok_type=TOK_NOR;
			};
			break;
		case TOK_SMALLER:
			if(next_token_type(bf)==TOK_ASSIGN) {
				getnc1(bf,&cc,&tok_type);
				tok_type=TOK_SMALLEREQ;
			} break;
		case TOK_BIGGER:
			if(next_token_type(bf)==TOK_ASSIGN) {
				getnc1(bf,&cc,&tok_type);
				tok_type=TOK_BIGGEREQ;
			} break;
		case TOK_AND:
		case TOK_OR:
		case TOK_XOR:
			break;

		case TOK_MINUS:
			if(next_token_type(bf)==TOK_MINUS) {
				getnc1(bf,&cc,&tok_type);
				tok_type=TOK_DECREASE;
				break;
			};
			if(next_token_type(bf)==TOK_ASSIGN) {
				getnc1(bf,&cc,&tok_type);
				tok_type=TOK_DECREASEBY;
				break;
			};
		case TOK_PLUS:
			if(next_token_type(bf)==TOK_PLUS) {
				getnc1(bf,&cc,&tok_type);
				tok_type=TOK_INCREASE;
				break;
			};
			if(next_token_type(bf)==TOK_ASSIGN) {
				getnc1(bf,&cc,&tok_type);
				tok_type=TOK_INCREASEBY;
				break;
			};

		case TOK_MOD:
			break;
		case TOK_POWER:
			break;
		case TOK_MUL:
			if(next_token_type(bf)==TOK_MUL) {
				getnc1(bf,&cc,&tok_type);
				tok_type=TOK_POWER;
				break;
			} else
			if(next_token_type(bf)==TOK_ASSIGN) {
				getnc1(bf,&cc,&tok_type);
				tok_type=TOK_MULBY;
				break;
			};
			break;
		case TOK_DIV:
			break;
		case TOK_ASSIGN:
			// MESG("TOK_ASSIGN:");
			if(next_token_type(bf)==TOK_ASSIGN) {
				getnc1(bf,&cc,&tok_type);
				tok_type=TOK_EQUAL;
				break;
			};
			if(ex_edenv==TOK_ENV) tok_type=TOK_ASSIGNENV;
			if(ex_edenv==TOK_OPTION) tok_type=TOK_ASSIGNOPT;
			break;
		case TOK_COMMA:{
			if(next_token_type(bf)==TOK_NL
				|| next_token_type(bf)==TOK_SPACE
			) {
				if(next_token_type(bf)==TOK_NL) tok_line++;
				last_correct_line=tok_line;
				getnc1(bf,&cc,&tok_type);
				tok_type=TOK_COMMA;
			};
			};break;
		case TOK_AT:
			slen=getnstr1(bf,cc,nword);
			// printf("tok_at [%s] %d\n",nword, slen);
			// printf("tok_at [%s]\n",(char *)tok->tname);
			break;

		case TOK_DOLAR:
		case TOK_TILDA:
		case TOK_BSLASH:
			break;
		default:
			{
			err_num=103;
			if(tok_line) err_line=tok_line;else err_line=last_correct_line;
			err_str="character unrecognised";
			return(0);
			};
	};

	// MESG("	- token type=[%d %s] previous token is [%d %s]",tok_type,tname(tok_type),previous_ttype,tname(previous_ttype));
	if(tok_type==TOK_RPAR || !strcmp(nword,"else")) after_rpar=1;else after_rpar=0;
	if(!is_storelines) {
	if(!(is_now_sep && (tok_type==TOK_LCURL||tok_type==TOK_RCURL) )){
			/* check for else statement!!  */
		if(!(tok_type==TOK_LETTER && !strcmp(nword,"else"))){
			if(tok_type==TOK_LBRAKET && previous_ttype==TOK_RBRAKET){
				// MESG("skip tok_lbraket!");
				continue;
			} else {
				ADD_TOKEN;
			};
		} else {
			if(is_now_sep) {
				tok_type=TOK_DIR_ELSE;
				is_now_sep=0;
			} else {
				ADD_TOKEN;
			};
		};
	};
	previous_ttype=tok_type;
	// MESG("	2 - token type=%d %s",tok_type,tname(tok_type));

	if(tok_type!=TOK_SEP) is_now_sep=0;
	if(tok_type!=TOK_LCURL && tok_type!=TOK_RCURL) is_now_curl=0;

	tok->ttype=tok_type;
	tok->dval=value;
	tok->tline=tok_line;
	if(tok->ttype==TOK_SHOW) {
		// MESG("	TOK_SHOW");
		tok->ddot=new_textpoint_at(bf,TP_DDOT,ddot_offset-1);
		tok->tname=":";
	};
	if(tok->ttype==TOK_QUOTE) {
		tok->tname=strdup(nword);
	};
	if(tok->ttype==TOK_AT) {
		tok->tname=strdup(nword);
	};

	// MESG("	set token name: %d %s",tok->ttype,token_table[tok->ttype].tok_name);
	/* ------- set token name, group ----------- */
	if(tok->ttype!=TOK_VAR&&tok->ttype!=TOK_QUOTE&&tok->ttype!=TOK_NUM)
		tok->tname=token_table[tok->ttype].tok_name;
	tok->tgroup=token_table[tok->ttype].tok_group;

	if(tok->tgroup==TOK_COMPARE) {
		// tok->cexpr_function = factor_funcs[tok->ttype];
		set_tok_function(tok,1);
	};
	if(tok->ttype==TOK_LCURL) {
		struct curl_struct *tcl;
			tcl=new_curl(curl_level,tok_line,lex_parser->last);
			tcl->num=tok->tnum;
			tok->tname="{";
			lpush(tcl,curl_stack);
			tok->tcurl=tcl;
	} else
	if(tok->ttype==TOK_RCURL) {
		struct curl_struct *tcl,*tcr;
			tcr=new_curl(curl_level,tok_line,lex_parser->last);
			tcl=(curl_struct *)lpop(curl_stack);
			tok->tcurl=tcr;
			tok->tname="}";
			tcr->num=tcl->num;
			tcl->num=tok->tnum;
	};
	};
	
	if(tok_type==TOK_LETTER) {
		// MESG("	parser: TOK_LETTER: check element in bt [%s]",nword);
		tok->tnode = find_bt_element(nword); // check main table
#if	TEST_PARSER
		if(tok->tnode==NULL) {
			BTNODE *var_node = find_btnode(stree,nword);
			if(var_node!=NULL) MESG("		[%s] found variable in stree index = %d type=%d vtype=%d",nword,var_node->node_index,var_node->node_type,var_node->node_vtype);
			else MESG("		[%s] not found!!! in stree",nword);
		}
#endif
		if(is_storelines && proc_name==NULL && tok->tnode!=NULL) {	/* function already register!  */
			proc_name=strdup(nword);
		};
		if(tok->tnode==NULL) { // a NEW variable name or directive
			if(is_storelines) {
				if(proc_name==NULL) {
					proc_name=strdup(nword);
				};
			};
			// MESG("	parser:		check in directiv_table");

			tok->tnode=find_btnode(directiv_table,nword);
			if(tok->tnode==NULL){	/* not a directive but a variable  */

				// CHECK for type variable in bf->type_*** !!!

				// MESG("		[%s] is a variable !!!!!!!!!!",nword);

				tok->tname=strdup(nword);
				tok->tind=slen;
				// MESG("	this is a variable! [%s] slen=%d",nword,slen);
				if(is_storelines) {
					tok->ttype=TOK_PROC;
				} else {
					set_var(stree,tok,nword);
#if	TEST_PARSER
					{
						BTNODE *var_node = find_btnode(stree,nword);
						if(var_node!=NULL) {
							MESG("	[%s] found ADDED variable in stree index = %d type=%d vtype=%d",nword,var_node->node_index,var_node->node_type,var_node->node_vtype);
							if(!strcmp("b",nword)) {
								var_node->node_vtype=TOK_ARRAY2;
								MESG("	set b to type TOK_ARRAY2");
							};
						} else MESG("		[%s] not found!!! in stree",nword);
					}
#endif
					if(next_token_type(bf)==TOK_LBRAKET) {
						// MESG("	parser: next is TOK_LBRAKET !!!!");
						array_tok=tok;	/* variable to set as array!  */
						array_tok->ttype=TOK_ARRAY1;
						skip_token=1;
					};
				}
			} else { // we have a directive
				// MESG("	parser:	directive [%s] found!",nword);
				tok->tname=tok->tnode->node_name;
				tok->tgroup=tok->tnode->node_type;
				tok->tind = tok->tnode->node_index;
				tok->ttype=tok->tnode->node_index;

				// MESG("directiv:0:[%s] ttype=%d, tind=%d, tgroup=%d",tok->tname,tok->ttype,tok->tind,tok->tgroup);
				if(tok->tind==TOK_PROC) 
				{
					is_storelines=1;
					store_level=curl_level;
					start_proc_offset=foffset;
				};

				if(tok->ttype == TOK_DIR_TYPE) {
					// MESG("	found type_definition!");
					if(!type_init_definition(bf,lex_parser,tok)) {
						// set_error(tok,111,"type_definition parse error");
						// MESG("after setting 111 error");
						return 0;
					};
					// MESG("after check type definition!");
				};
				// MESG("this is a directive: [%s]",nword);
			};
		} else {
			tok->tname=tok->tnode->node_name;
			// MESG("	token name is old [%s] found!!!!",tok->tname);
			switch(tok->tnode->node_type) {
				case TOK_VAR:	/* 0  */
					tok->ttype=TOK_VAR; 
					ex_edenv=TOK_VAR;
					break;
				case TOK_ENV:	/* 1  */
					tok->ttype=TOK_ENV; // editor env variables
					ex_edenv=TOK_ENV;
					break;
				case TOK_FUNC:	/* 2  */
					tok->ttype=TOK_FUNC; // editor functions
					// MESG("TOK_FUNC: %s",tok->tname);
					// BTNODE *bte=tok->tnode;
					// MESG("TOK_FUNC: index=%d",bte->node_index);
					// tok->factor_function = m_functions[bte->node_index].ffunction;
					// tok->factor_function = factor_func;
					break;
				case TOK_CMD:	/* 3  */
					tok->ttype=TOK_CMD;	// editor commands
					break;
				case TOK_PROC:	/* 4  */
					tok->ttype=TOK_PROC;	// 
					// MESG("TOK_PROC");
					break;
				case TOK_OPTION:	/* 5  */
					tok->ttype=TOK_OPTION; // editor options
					ex_edenv=TOK_OPTION;
					break;
				default:
				tok->ttype=TOK_LETTER;
			};
		};
	};
	// MESG("parse: end token switch!");
	previous_ttype=tok->ttype;
	if(err_num>0) {ERROR("ERROR: line=%d %d type=%d [%s]",last_correct_line,err_line,err_num,err_str);break;};
 };
  
 	// MESG("parse_block1: END of parsing! type=%d level=%d",tok_type,curl_level);
 	/* add eof token!  */
	if(tok_type!=TOK_SEP) 
	{	
		if(tok_type==0) {
			tok->ttype=TOK_SEP;
			tok->tname="end 0";
		} else {
			ADD_TOKEN;
			tok->ttype=TOK_SEP;
			tok->tind=0;
			tok->tline=tok_line;
			tok->tname="end sep";
		};
	};
	bf->end_token=tok;	/* save end token  */
	ADD_TOKEN
	tok->ttype=TOK_EOF;
	tok->tind=0;
	tok->tline=tok_line;
	tok->tname="eof";
	if(curl_level!=0 && err_num<1) set_error(tok,106,"parse error: invalid number of curls");

	if(par_level!=0 && err_num<1) { 
		err_num=1014;err_line=tok->tline;err_str="parse error: invalid number of pars";
		// ERROR("parenthesis error: line %d",tok_line);
	};
	bf->m_mode=M_PARSED;

 if(init) {
	bf->symbol_tree->max_items = bf->symbol_tree->items+extra;
 };

 // MESG("parse_block1: create token table from token list");
 set_tok_table(bf, lex_parser);

 free_list(lex_parser,"lex_parser");
 free_list(curl_stack,"cstack");
 stage_level=save_stage_level;
 // MESG("parse_block1:[%s] end token items %d",bf->b_fname,bf->symbol_tree->max_items);
 return(TRUE); 
}

/* create token table from token list  */
void set_tok_table(FILEBUF *bf, TLIST lex_parser)
{
 TLIST tlist;
 tok_struct *tok_to;
 tok_struct *tok;
 tok_struct *tok_table=NULL;
 int isize=0;

 if(bf->tok_table != NULL) {
	// MESG("set_tok_table: tok_table not NULL, free it!");
 	free(bf->tok_table);
 };
 tok_table=(void *)malloc(sizeof(struct tok_struct)*(lex_parser->size+1));
 bf->tok_table = (void *) tok_table;
 // MESG("----> set_tok_table: bf=[%s]",bf->b_fname);
 lbegin(lex_parser);
 tlist=lex_parser;
 tok_to = tok_table;
 while(tlist->current)
 {
	tok=(tok_struct *)tlist->current->data;
	// MESG(" ++	%2d: %3d [%s] %d",tok->tnum,tok->tline,tok->tname,tok->ttype);
	memcpy((void *)tok_to,(void *)tok,sizeof(tok_struct));
	if(tok->ttype==TOK_LCURL || tok->ttype==TOK_RCURL) {
		tok_to->match_tok = tok_table + tok_to->tcurl->num;
	};
	tlist->current=tlist->current->next;
	// MESG(";%3d: t=[%s] ",isize,tok_info(tok));
	isize++;
	tok_to++;
 };
 bf->end_token=tok_table+(isize-2);	/* save end token, just before EOF  */
}
