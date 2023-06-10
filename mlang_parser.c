/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

	interpreter parser
*/

void set_tok_table(FILEBUF *bf, TLIST lex_parser);

offs foffset=0;

/* get next character to parse */
int getnc1(FILEBUF *bf, int *cc, int *cmask)
{
 if(!FEofAt(bf,foffset)) {
 	*cc=FCharAt(bf,foffset++);
	*cmask=tok_mask[*cc];

	if(*cc==0) return(0);
	return(1);
 } else {
 	return(0);
 };
}

/* Find next token type  */
int next_token_type(FILEBUF *bf)
{
 int c1=0;
 if(!FEofAt(bf,foffset)) {
 	c1=tok_mask[FCharAt(bf,foffset)];
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
	// MESG("added token %3d: ind=%d type=[%s] [%s]",tok->tnum,cc,tok_name[tok_type],label);
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


int add_to_symbol_tree(BTREE *stable,char *name)
{
	BTNODE *btn=NULL;
	/* create the variable in the symbol_table!  */
	// MESG("add_to_symbol_tree: %s table items=%d",name,stable->items);
	btn=add_btnode(stable,name);
	if(stable->new_flag) {
		btn->node_type=TOK_VAR;
		btn->val=0;
		btn->node_index=stable->items-1;	/*  variable index  */
		btn->sval=NULL;
		stable->new_flag=0;
		// MESG("- this is new! index=%d",btn->node_index+1);
		return (btn->node_index+1);
	} else {
		// found 
		// MESG(" - this found! index=%d type=%d subtype=%d",btn->node_index+1,btn->node_type,btn->node_index);
		return -(btn->node_index+1);
	}
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
 int nc,cmask;
 char *str=word;
 int len=1;
 *str++ = cc;
 while(!FEofAt(bf,foffset))
 {
 	nc=FCharAt(bf,foffset);
	cmask=tok_mask[nc];
	if(cmask!=TOK_LETTER && cmask!=TOK_NUM) break;
	if(len<255) { *str++ = nc; len++;};
	foffset++;
 };
 *str=0;

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
	if(nc=='.'||nc==',') {
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
 int ind;
 	tok->ttype=TOK_VAR;
	// MESG("set_var: name=%s",name);
	ind=add_to_symbol_tree(stree,name);
	ex_edenv=TOK_VAR;
	if(ind>0) { 	/* this is a new one  */
		tok->tind=ind-1;
		if(stree->max_items<ind) ERROR("exceeded item list of %d !! CHECK!",stree->max_items);
	} else {	/* found  */
		tok->tind = -ind-1;
	};
//	tok->tdata = &current_stable[tok->tind];
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
 int after_rpar=0;
 TLIST cstack; // curl stack
 int store_level=0;
 int save_stage_level;
 offs start_proc_offset=0;
 offs ddot_offset=0;
 char *proc_name=NULL;
 alist *lex_parser=NULL;
 BTREE *stree=use_stree;
 tok_struct *tok=NULL;

 int tok_type=0;
 int next_tok_type=0;
 int script_active=0;

 // MESG("parse_block1: file_type=%d [%s]",bf->b_type,bf->b_fname);
 if(
 	file_type_is("CMD",bf->b_type)
	|| file_type_is("DOT",bf->b_type)
	// || file_type_is("GTEXT",bf->b_type) 
	// || bf->b_type==0
 ) script_active=1;

 if(bf->tok_table !=NULL && init==0) 
 {
	return(0);	// no change, already parsed
 } 
 if(init && bf->tok_table!=NULL) {
 	/* we must free the previous parse  */
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
 
 {	/* clear ddot textpoints  */
	TextPoint *scan,*nscan;
	for(scan=bf->tp_last; scan;) {
		nscan=scan->t_next;
		if(scan->tp_type == TP_DDOT) textpoint_delete(scan);
		scan=nscan;
	};
 };

 foffset=0;	/* goto to the beginning of the buffer  */
 cstack=new_list(0,"curles_stack"); // create curles stack 
 // MESG("parse_block1: before looping: script_active=%d",script_active);
 err_num=0;
 err_line=0;
 tok_line=0;
 save_stage_level=stage_level;

 while(getnc1(bf,&cc,&tok_type))
 {
	if(script_active) {
		if(tok_type==TOK_BQUOTE) { 
			script_active=0;
			continue;
		}; 
	} else {
		if(tok_type==TOK_BQUOTE) {
			script_active=1;
			continue;
		};
		// if(tok_type==TOK_NL) {  MESG(" - newline in comment %d",tok_line);tok_line++;};
		continue;
	};
#if	0
 if(tok_type==TOK_NL) MESG(" - New line -------  type=%d %s line=%d",tok_type,tok_name[tok_type],tok_line);
 MESG(" - %d [%c] type=%d %s line=%d",cc, cc,tok_type,tok_name[tok_type],tok_line);
#endif
 if(err_num>0) return 0.0;
	value=0;
	nword[0]=0;
	switch(tok_type) {
		case TOK_SEP:
			if(is_now_sep) continue;
			else {
				if(is_now_curl) continue;
				is_now_sep=1;
				break;
			};

		case TOK_NL: 
			tok_line++;
			if(cc!=';') {
				last_correct_line=tok_line;
				skip_line1(bf,cc);
				if(is_now_sep || after_rpar) continue;
				is_now_sep=1;
				tok_type=TOK_SEP;
				break;
			} ;
			if(is_now_sep) continue;
			else {
				if(is_now_curl) continue;
				is_now_sep=1;
				break;
			};
		case TOK_SPACE: 
			skip_space1(bf);
			continue;
		case TOK_LETTER: 
			slen=getnword1(bf,cc,nword);
			break;
		case TOK_NUM:
			// MESG("TOK_NUM: old num=%d type=%d name %s,new type %d ",tok->tnum,tok->ttype,tok->tname,tok_type);
			value=getnum1(bf,cc,tok);
			// MESG("TOK_NUM: num=%d type=%d val=%f",tok->tnum,tok->ttype,value);
			// tok->tname = " numeric!! ";
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
			cc=0;break;
		case TOK_RPAR:
			par_level--;
			cc=1;break;
		case TOK_LBRAKET:
			cc=0;
			break;
		case TOK_RBRAKET:
			cc=1;
			break;
		case TOK_QUOTE: // string start
			slen=getnstr1(bf,cc,nword);
			break;
		case TOK_SHOW:
			ddot_offset=foffset;
			skip_2nl(bf);
			break;
		case TOK_COMMENT:
			skip_2nl(bf);
			tok_line++;
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
	// MESG("- token type=%d %s",tok_type,tok_name[tok_type]);
	if(tok_type==TOK_RPAR || !strcmp(nword,"else")) after_rpar=1;else after_rpar=0;
	if(!is_storelines) {
	if(!(is_now_sep && (tok_type==TOK_LCURL||tok_type==TOK_RCURL) )){
			/* check for else statement!!  */
		if(!(tok_type==TOK_LETTER && !strcmp(nword,"else"))){
			ADD_TOKEN;
		} else {
			if(is_now_sep) {
				tok_type=TOK_DIR_ELSE;
				is_now_sep=0;
			} else {
				ADD_TOKEN;
			};
		};
	};
	if(tok_type!=TOK_SEP) is_now_sep=0;
	if(tok_type!=TOK_LCURL && tok_type!=TOK_RCURL) is_now_curl=0;

	tok->ttype=tok_type;
	tok->dval=value;
	tok->tline=tok_line;
	tok->level=curl_level;

	if(tok->ttype==TOK_SHOW) {
		tok->tname=(void *)new_textpoint_at(bf,TP_DDOT,ddot_offset-1);
	};
	if(tok->ttype==TOK_QUOTE) {
		tok->tname=strdup(nword);
	};
	if(tok->ttype==TOK_AT) {
		tok->tname=strdup(nword);
	};

	if(tok->ttype==TOK_LPAR) tok->tname=" ( ";
	if(tok->ttype==TOK_RPAR) tok->tname=" ) ";
	if(tok->ttype==TOK_SEP) tok->tname=" ; ";
	if(tok->ttype==TOK_NUM) tok->tname="numeric";
	if(tok->ttype==TOK_ASSIGN) { tok->tname=" = ";tok->tgroup=TOK_TERM0;};
	if(tok->ttype==TOK_ASSIGNENV) { tok->tname=" setenv ";tok->tgroup=TOK_TERM0;};
	if(tok->ttype==TOK_ASSIGNOPT) { tok->tname=" setoption ";tok->tgroup=TOK_TERM0;};
	if(tok->ttype==TOK_INCREASEBY) { tok->tname=" += ";tok->tgroup=TOK_TERM0;};
	if(tok->ttype==TOK_DECREASEBY) { tok->tname=" -= ";tok->tgroup=TOK_TERM0;};
	/* term1  */
	if(tok->ttype==TOK_PLUS) {tok->tname=" + ";tok->tgroup=TOK_TERM;};
	if(tok->ttype==TOK_MINUS) {tok->tname=" - ";tok->tgroup=TOK_TERM;};
	/* term2  */
	if(tok->ttype==TOK_DIV) {tok->tname=" / ";tok->tgroup=TOK_TERM1;};
	if(tok->ttype==TOK_MUL) {tok->tname=" * "; tok->tgroup=TOK_TERM1;};
	/* term3  */
	if(tok->ttype==TOK_MOD) {tok->tname=" % ";tok->tgroup=TOK_TERM2;};
	if(tok->ttype==TOK_POWER) {tok->tname=" ** "; tok->tgroup=TOK_TERM2;};
	if(tok->ttype==TOK_INCREASE) { tok->tname=" INC ";tok->tgroup=0;};
	if(tok->ttype==TOK_DECREASE) { tok->tname=" DEC ";};
	/* boolean tokens  */
	if(tok->ttype==TOK_AND) { tok->tname=" AND ";tok->tgroup=TOK_TERM0;};
	if(tok->ttype==TOK_OR) { tok->tname=" OR ";tok->tgroup=TOK_BOOL;};
	if(tok->ttype==TOK_XOR) { tok->tname=" XOR ";tok->tgroup=TOK_BOOL;};
	if(tok->ttype==TOK_NAND) { tok->tname=" NAND ";tok->tgroup=TOK_TERM0;};
	if(tok->ttype==TOK_NOR) { tok->tname=" NOR ";tok->tgroup=TOK_TERM0;};
	/* comparison tokens  */
	if(tok->ttype==TOK_EQUAL) {tok->tname=" == ";tok->tgroup=TOK_COMPARE;};
	if(tok->ttype==TOK_NOTEQUAL ) {tok->tname=" != ";tok->tgroup=TOK_COMPARE;};
	if(tok->ttype==TOK_SMALLER ) {tok->tname=" < ";tok->tgroup=TOK_COMPARE;};
	if(tok->ttype==TOK_SMALLEREQ ) {tok->tname=" <= ";tok->tgroup=TOK_COMPARE;};
	if(tok->ttype==TOK_BIGGER ) {tok->tname=" > ";tok->tgroup=TOK_COMPARE;};
	if(tok->ttype==TOK_BIGGEREQ ) {tok->tname=" >= ";tok->tgroup=TOK_COMPARE;};
	if(tok->tgroup==TOK_COMPARE) {
		tok->cexpr_function = factor_funcs[tok->ttype];
	};
	if(tok->ttype==TOK_LCURL) {
		struct curl_struct *tcl;
			tcl=new_curl(curl_level,tok_line,lex_parser->last);
			tcl->num=tok->tnum;
			tok->tname="{";
			lpush(tcl,cstack);
			tok->tcurl=tcl;
	} else
	if(tok->ttype==TOK_RCURL) {
		struct curl_struct *tcl,*tcr;
		struct _el *tt;
			tcr=new_curl(curl_level,tok_line,lex_parser->last);
			tcl=(curl_struct *)lpop(cstack);
			tok->tcurl=tcr;
			tok->tname="}";
			// swap curl pointers
			tt=tcl->ocurl;tcl->ocurl=tcr->ocurl;tcr->ocurl=tt;
			tcr->num=tcl->num;
			tcl->num=tok->tnum;
	};
	};
	
	if(tok_type==TOK_LETTER) {
		tok->tnode = find_bt_element(nword); // check main table
		
		if(is_storelines && proc_name==NULL && tok->tnode!=NULL) {	/* function already register!  */
			proc_name=strdup(nword);
		};
		if(tok->tnode==NULL) { // a variable name or directive
			if(is_storelines) {
				if(proc_name==NULL) {
					proc_name=strdup(nword);
				};
			};
			tok->tnode=find_btnode(directiv_table,nword);
			if(tok->tnode==NULL){	/* this is a variable  */
				tok->tname=strdup(nword);
				tok->tind=slen;
				if(is_storelines) {
					tok->ttype=TOK_PROC;
				}
				else {
					tok_struct *tok_var=tok;
					int index=0;
					set_var(stree,tok,nword);

					while(next_token_type(bf)==TOK_LBRAKET) {
						// MESG("array ");
						tok_var->ttype=TOK_ARRAY1+index;	/* set it as array index  */
						getnc1(bf,&cc,&tok_type);// skip it
						getnc1(bf,&cc,&tok_type);// get the index!


						switch(tok_type){
							case TOK_NUM:
								tok->tname="numeric2";
								ADD_TOKEN;
								value=getnum1(bf,cc,tok);
								tok->ttype=TOK_NUM;
								tok->tname="numeric3";
								tok->dval=value;
								break;
							case TOK_LETTER:
								slen=getnword1(bf,cc,nword);
								{ // this must be a variable !!
								ADD_TOKEN;
								tok->tnode=find_btnode(directiv_table,nword);
								if(tok->tnode!=NULL) { break;}  // this is an error ;
									{ // this is a variable , we normally should check for an existing one!!
										tok->tname=strdup(nword);
										tok->tind=slen;
										set_var(stree,tok,nword);
									}
								}
								break;
							default:
							// this is an ERROR !!
							err_num=105;
							err_str="wrong character in table definition";
							ERROR("wrong character on table definition !");
						};
						getnc1(bf,&cc,&tok_type);	/* this should be rbracket !  */
						// MESG("tok: %c type=%d",cc,tok_type);

						index++;	/* array dimension  */
						if(index>1) break;	/* for the moment only 2 dimensional arrays!!  */
					};
				}
			} else { // we have a directive
				tok->tname=tok->tnode->node_name;
				tok->tgroup=tok->tnode->node_type;
				tok->tind = tok->tnode->node_index;
				tok->ttype=tok->tnode->node_index;
//				MESG("directiv:0:[%s] ttype=%d, tind=%d, tgroup=%d",tok->tname,tok->ttype,tok->tind,tok->tgroup);
				if(tok->tind==TOK_PROC) 
				{
					is_storelines=1;
					store_level=curl_level;
					start_proc_offset=foffset;
				};
			};
		} else {
			tok->tname=tok->tnode->node_name;

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
					break;
				case TOK_CMD:	/* 3  */
					tok->ttype=TOK_CMD;	// editor commands
					break;
				case TOK_PROC:	/* 4  */
					tok->ttype=TOK_PROC;	// 
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
	if(err_num>0) {ERROR("ERROR: line=%d %d type=%d [%s]",last_correct_line,err_line,err_num,err_str);break;};
 };
 // MESG("parse_block1: END of parsing! type=%d level=%d",tok_type,curl_level);
 {	/* add eof token!  */
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
		err_num=104;err_line=last_correct_line;err_str="parse error: invalid number of pars";
		ERROR("parenthesis error: line %d",tok_line);
	};
	bf->m_mode=M_PARSED;	
 };
 if(init) {
	bf->symbol_tree->max_items = bf->symbol_tree->items+extra;
 };

 // MESG("parse_block1: create token table from token list");
 set_tok_table(bf, lex_parser);

 free_list(lex_parser,"lex_parser");
 free_list(cstack,"cstack");
 stage_level=save_stage_level;
 // MESG("parse_block1:[%s] end toke items %d",bf->b_fname,bf->symbol_tree->max_items);
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
 if(bf->tok_table != NULL) free(bf->tok_table);
// fprintf(stderr,"token list size = %d\n",lex_parser->size);
 tok_table=(void *)malloc(sizeof(struct tok_struct)*(lex_parser->size+1));
 bf->tok_table = (void *) tok_table;

 lbegin(lex_parser);
 tlist=lex_parser;
 tok_to = tok_table;
 while(tlist->current)
 {
	tok=(tok_struct *)tlist->current->data;
	memcpy((void *)tok_to,(void *)tok,sizeof(tok_struct));
	if(tok->ttype==TOK_LCURL || tok->ttype==TOK_RCURL) {
		tok_to->match_tok = tok_table + tok_to->tcurl->num;
	};
	tlist->current=tlist->current->next;
	isize++;
	tok_to++;
 };
 bf->end_token=tok_table+(isize-2);	/* save end token, just before EOF  */
}
