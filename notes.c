/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

	Editor Notes support
*/
#include "xe.h"


extern FILEBUF *cbfp;
#define	MD_NOTES	1
#define	TITLE_FIRST	1

/* ----- other utility functions ------ */
#if	TNOTES
offs find_str_reol(FILEBUF *fp, offs start, char *needle_string, char *return_string,int max_len);
#endif
extern int scratch_files[];
int bom_type(int file_id);

#if	TNOTES
notes_struct *init_note();
int get_current_tag_id();
char *get_current_tag_name();
time_t get_note_timestamp(char *note_name);
long int query_int(sqlite3 *db,char *sql_string);
void set_highlight(FILEBUF *bf,int type);
void init_note_keys();

int insert_preamble(FILEBUF *fp,int type)
{
 char date_header[MAXSLEN];
 if(fp->b_note==NULL) fp->b_note=init_note();


#if	MD_NOTES
	insert_string_nl(fp,"---");
#endif
	sprintf(date_header,"#Date: %s",date_string(2));
	//date_header[strlen(date_header)-1]=0;	/* remove new line at the end!  */
	insert_string_nl(fp,date_header);

	if(type<2) type=1;
	switch(type) {
		case 1: {	/* normal note */
			insert_string_nl(fp,"#Name: ");
			insert_string_nl(fp,"#Title: ");
			insert_string_nl(fp,"#Category: ");	/* Directory!  */
			insert_string_nl(fp,"#Tags: ");
#if	!MD_NOTES
			insert_string_nl(fp,"");
#endif			
			};break;
		case 2: {	/* calendar note */
			strlcpy(fp->b_note->n_name,fp->b_fname,sizeof(fp->b_note->n_name));
#if	MD_NOTES
			strlcat(fp->b_note->n_name,"_cal.md",sizeof(fp->b_note->n_name));
#else
			strlcat(fp->b_note->n_name,".cal",sizeof(fp->b_note->n_name));
#endif
			strlcpy(fp->b_note->n_date,fp->b_fname,sizeof(fp->b_note->n_date));
			insert_string_nl(fp,"# Untitled note");
			};break;
		case 3: {	/* todo note */
			strlcpy(fp->b_note->n_name,fp->b_fname,sizeof(fp->b_note->n_name));
#if	MD_NOTES
			strlcat(fp->b_note->n_name,"_todo.md",sizeof(fp->b_note->n_name));
#else
			strlcat(fp->b_note->n_name,".todo",sizeof(fp->b_note->n_name));
#endif
			strlcpy(fp->b_note->n_date,fp->b_fname,sizeof(fp->b_note->n_date));
			insert_string_nl(fp,"#Title: ");

			insert_string_nl(fp,"#Tags: todo");
#if	!MD_NOTES
			insert_string_nl(fp,"");
#endif
			};break;
	};
#if	MD_NOTES
	insert_string_nl(fp,"---");
#endif
	return(true);
}

char *sql_sanitize(char *sql_string)
{
 static char s_string[1024];

 if(sindex(sql_string,"'")) {
 	int i,i1=0;
	for(i=0;i<strlen(sql_string);i++,i1++) {
		if(sql_string[i]=='\'') {
			s_string[i1++]='\'';
		};
		s_string[i1]=sql_string[i];
	};
	s_string[i1]=0;
 	return s_string;
 } else
 return sql_string;
}

int new_note(num type)
{
 int stat=0;
 char scratch_name[24];
 int scratch_ind=99;
 int res;
 int i;
 for(i=0;i<100;i++) {
 	if(scratch_files[i]==0) {
		scratch_ind=i+1;
		scratch_files[i]=scratch_ind;
		break;
	};
 };
 if(type<2) type=1;
 if(type==1) snprintf(scratch_name,sizeof(scratch_name),"[note %d]",scratch_ind);
 if(type==2) { 
#if	MD_NOTES
	res=snprintf(scratch_name,sizeof(scratch_name),"%s_cal.md",date_string(3));
#else
	res=snprintf(scratch_name,sizeof(scratch_name),"%s.cal",date_string(3));
#endif
	if(res==25) { error_line("cal name truncated");return false;};
 };
 if(type==3) { 
	res=snprintf(scratch_name,sizeof(scratch_name),"%s",date_string(3));
	if(res==25) { error_line("todo name truncated");return false;};
 };

 stat=goto_file(scratch_name);
 // MESG("new_note: stat=%d",stat);
 if(stat) {
	update_lines(cbfp);
	// cbfp->scratch_num=scratch_ind;
	insert_preamble(cbfp,type);
	// goto name field
	goto_bof(1);

	if(type==1) {
		cbfp->b_type=NOTE_TYPE;
	};
	if(type==2) {
		cbfp->b_type=NOTE_CAL_TYPE+highlight_type("cal");
	};
	if(type==3) {
		cbfp->b_type=NOTE_TODO_TYPE+highlight_type("todo");
	}
#if	MD_NOTES
	int type=0;
	set_highlight(cbfp,highlight_index("MD",&type));
#endif

#if	MD_NOTES
	next_line(2);
#else
	next_line(1);
#endif
	goto_eol(1);

	// MESG("new_note: b_type=0x%X",cbfp->b_type);
	set_hmark(0,"new_note:");
	msg_line("note created b_type=%X b_flag=%X",cbfp->b_type,cbfp->b_flag);
 };
	return stat;
}

notes_struct *init_note()
{
 notes_struct *note;
	note=malloc(sizeof(struct notes_struct));
	if(note) {
		note->n_date[0]=0;
		note->n_name[0]=0;
		note->n_title[0]=0;
		note->n_cat[0]=0;
		note->n_tags[0]=0;
		// note->encrypt_flag=0;
		note->timestamp=0;
	};
	return note;
}

int new_cal_note(num n)
{
	return new_note(2);
}

int new_todo_note(num n)
{
	return new_note(5);
}
#endif

offs fast_scanner4 (FILEBUF *fp, offs buff_size, char *pat, int patlen,offs start);
#if	TNOTES
offs find_str_reol(FILEBUF *fp, offs start, char *needle_string, char *return_string,int max_len)
{
	offs ptr = start;
	//int len=0;
	int flen=strlen(needle_string);	/* to find string len  */
	// MESG("	find_str_reol: [%s] max_len=%d",needle_string,max_len);
	ptr = fast_scanner4(fp, FSize(fp), needle_string,flen,start);
	char *rs=return_string;
	if(ptr) {
		// MESG("	pre=%ld eof=%ld",ptr,FSize(fp));
		while(!FEolAt(fp,ptr)){
			if(rs-return_string<max_len-1) {
				char ch = FCharAt(fp,ptr);
				// MESG("		%ld  %c %X",rs-return_string,ch,ch);
				*rs++ = ch; ptr++; 
				// len++;
			} else ptr++;
		};
		*rs=0;
		ptr+=fp->EolSize;
		// MESG("Found [%s] value = [%s] next line at %ld len=%d",needle_string,return_string,ptr,len);
	} else {
		// MESG("string [%s] not found!",needle_string);
		ptr=0;
	};
	return ptr;
}

int c_in_tag(byte c)
{
	if(c=='#') return FALSE;
	if(c=='_') return TRUE;
	if(c>34 && c<39) return TRUE;
	if(c>='a' && c<='z') return TRUE;
	if(c>='A' && c<='Z') return TRUE;
	if(c>=128 && c<255) return TRUE;
	return FALSE;
}

void add_infile_tags(FILEBUF *fp, offs start, char *return_string,int max_len)
{
 offs ptr=start;
 // int found_tags=0;
 int len=strlen(return_string);
 int prev_ok=1;
 // MESG("add_infile_tags: start=%ld",start);
 while(!FEofAt(fp,ptr)) {
	byte ch=FCharAt(fp,ptr);
	// MESG("	check %c at %ld",ch,ptr);
	if(ch=='#' && prev_ok) {
		// MESG("	found # at %ld",ptr);
		ptr++;
		if(!FEofAt(fp,ptr)) {
			ch=FCharAt(fp,ptr);
			// MESG("	found ch=%c at %ld",ch,ptr);
			if(c_in_tag(ch) ) {
				// add next word to tags;
				// MESG("	add  %c to tag ptr=%ld",ch,ptr);
				if(len>=max_len) return;
				int len1=len;
				if(len>0) return_string[len++]=',';
				return_string[len++]=ch;
				// MESG("	0 add [%c]",ch);
				// found_tags++;
				ptr++;
				while(!FEofAt(fp,ptr)) {
					ch=FCharAt(fp,ptr);
					if(ch<33) { 
						// MESG("	end if tag at %ld",ptr);
						ptr++;
						break;
					};
					if(!c_in_tag(ch)) {
						ptr++;
						return_string[len1]=0;
						len=len1;
						break;
					};
					if(len>max_len) { return_string[len1]=0;return;};
					return_string[len++]=ch;
					// MESG("	2 add [%c]",ch);
					ptr++;
				};
				return_string[len]=0;
				// MESG("	added tag -> [%s]",return_string);
			} 
		} else break;
	} else {
		ptr++; 
		if(ch<33) prev_ok=1;else prev_ok=0;
	};
 };
 // MESG("infile tags %d result [%s]",found_tags,return_string);
}

#endif

#if	TNOTES
// returns multiple rows
char ***query_rows(sqlite3 *db, char *sql)
{
	return NULL;
}

// returns one row
char **query_row(sqlite3 *db, char *sql)
{
	return NULL;
}

alist *query_string_column(sqlite3 *db, char *sql)
{
// MESG("query_string_column:[%s]",sql);
 int stat;
 sqlite3_stmt *res;
 int step;
  
 stat = sqlite3_prepare_v2(db, sql, -1, &res, 0);
 if(stat==SQLITE_OK) {
	alist *a=new_list(0,"query_column");
	while((step=sqlite3_step(res))==SQLITE_ROW) {
		char *txt = strdup((char *)sqlite3_column_text(res,0));
		add_element_to_list((void *)txt,a);
	};
	sqlite3_finalize(res);
	return a;
 } else { error_line("query_string_column: error [%s]",sql);};
 sqlite3_finalize(res); 
 return NULL;
}

alist *query_string_columns(sqlite3 *db, char *sql,int *widths)
{
 int stat;
 sqlite3_stmt *res;
 int step;
 char txt[MAXLLEN];
 char space[512];

 txt[0]=0;
 // MESG("query_string_columns: sql=[%s]",sql);
 stat = sqlite3_prepare_v2(db, sql, -1, &res, 0);
 alist *a=new_list(0,"query_column");
 if(stat==SQLITE_OK) {
	// int rows=0;
	while((step=sqlite3_step(res))==SQLITE_ROW) {
		int column;
		txt[0]=0;
		// rows++;
		int row_id = atoi((char *)sqlite3_column_text(res,0));
		// MESG(" row %2d : id=%d\n",rows,row_id);
		for(column=1;column<widths[0]+1;column++) {
			char col_txt[256];
			strlcpy(col_txt,(char *)sqlite3_column_text(res,column),256);
			utf_string_break(col_txt,widths[column]-1);
			int utf_len = utf_num_chars(col_txt);
			strlcat(txt," ",MAXLLEN);
			strlcat(txt,col_txt,MAXLLEN);
			int space_i;
			for(space_i=0;space_i< widths[column]-utf_len;space_i++) { space[space_i]=' '; }; space[space_i]=0;
			strlcat(txt,space,MAXLLEN);strlcat(txt," │",MAXLLEN);
			// MESG("	%d : %d : [%s]",column,widths[column],txt);
		};
		// MESG("%3d : [%s]",rows,txt);
		istr *data = malloc(sizeof(int)*2+strlen(txt)+1);
		data->index = row_id;
		data->selection_tag = 0;
		memcpy(&data->start,txt,strlen(txt)+1);
		// MESG("%3d : %s",row_id,txt);
		add_element_to_list((void *)data,a);
	};
 } else {
 	error_line("query_string_columns: error [%s]!",sql);
 };
 sqlite3_finalize(res); 
 return a;
}

// return a numeric array (first value is the number of elements)
double *query_num_column(sqlite3 *db, char *sql)
{
// MESG("query_column:[%s]",sql);
 int stat;
 sqlite3_stmt *res;
 int step;
 double *darray=NULL;
 stat = sqlite3_prepare_v2(db, sql, -1, &res, 0);
 if(stat==SQLITE_OK) {
	// MESG("query prepared!");
	alist *a=new_list(0,"query_column");
	int i=0;
	while((step=sqlite3_step(res))==SQLITE_ROW) {
		double num = sqlite3_column_double(res,0);
		// MESG("	step %d num = %f",i,num);
		void *data = malloc(sizeof(double));
		memcpy(data,(void *)&num,sizeof(double));
		add_element_to_list((void *)data,a);
	} ;
	sqlite3_finalize(res);
	darray = malloc(sizeof(double)*(a->size+1));
	double *da= darray;
	i=0;
	da[i++]=a->size;
	// MESG("query_num_column: size=%f",da[0]);
	lbegin(a);
	void *data=NULL;
	while((data=lget(a))!=NULL){
		memcpy((void *)(&da[i++]),data,sizeof(double));
//		MESG(" %2d : %f",i-1,da[i-1]);
	};
	empty_list(a);
	free_list(a,"query_column");
	return darray;
 } else error_line(" query_num_column: error [%s]",sql);
 return NULL;
}

int *query_int_column(sqlite3 *db, char *sql,int *count)
{
// MESG("query_column:[%s]",sql);
 int stat;
 sqlite3_stmt *res;
 int step;
 int *iarray=NULL;
 stat = sqlite3_prepare_v2(db, sql, -1, &res, 0);
 if(stat==SQLITE_OK) {
	alist *a=new_list(0,"query_column");
	int i=0;
	while((step=sqlite3_step(res))==SQLITE_ROW) {
		double num = sqlite3_column_double(res,0);
		// MESG("	step %d num = %f",i,num);
		void *data = malloc(sizeof(double));
		memcpy(data,(void *)&num,sizeof(double));
		add_element_to_list((void *)data,a);
	} ;
	sqlite3_finalize(res);
	iarray = malloc(sizeof(int)*(a->size+1));
	i=0;
	*count=a->size;
//	MESG("query_int_column: [%s] %d",sql,a->size);

	lbegin(a);
	void *data=NULL;
	while((data=lget(a))!=NULL){
		double da;
		memcpy((void *)(&da),data,sizeof(double));
		iarray[i++]=(int) da;
		// MESG(" %2d : %f",i-1,da[i-1]);
	};
	empty_list(a);
	free_list(a,"query_column");
	return iarray;
 } else error_line("query_int_column: error [%s]",sql); 
 return NULL;
}

char *query_string(sqlite3 *db,char *sql,int *index)
{
 int stat;
 sqlite3_stmt *res;
 int step;
 static char return_string[512];
 // MESG("query string: [%s]",sql);
 *index=0;

 stat = sqlite3_prepare_v2(db, sql, -1, &res, 0);
 if(stat==SQLITE_OK) {
	step = sqlite3_step(res);

	if(step==SQLITE_ROW) {
		strlcpy(return_string,(char *)sqlite3_column_text(res,0),512);
		*index = sqlite3_column_int(res,1);;
		// MESG("query_string: result: %s row=%d",return_string,*index);
		sqlite3_finalize(res);
		return return_string;
	} else {
//		MESG("	not found! %d",*index);		
		sqlite3_finalize(res);
		return NULL;
	};
 } else {
 	// MESG("query_string: error %d [%s]",stat,sql);
 	error_line("sql error: %d %s", sql);
	return NULL;
 };
 return NULL;	
}

int query_rowid(sqlite3 *db,char *sql)
{
 int stat;
 sqlite3_stmt *res;
 int step;
 int row_id=0;
// MESG("query row_id: [%s]",sql);

 if(db==NULL) { msg_line("db is NULL!!!!!");return 0;};
 stat = sqlite3_prepare_v2(db, sql, -1, &res, 0);
 if(stat==SQLITE_OK) {
	step = sqlite3_step(res);

	if(step==SQLITE_ROW) {
//		MESG("	found: %s len=%d",sqlite3_column_text(res,0),strlen((char *)sqlite3_column_text(res,0)));
		
		row_id = sqlite3_column_int(res,0);
		sqlite3_finalize(res);
		return row_id;
	} else {
		sqlite3_finalize(res);
//		MESG("	not found!");
		return row_id;
	};
 } else {
 	// MESG("query_rowid: error %d [%s]",stat,sql);
 	return error_line("query_rowid: %d %s",stat,sql);
 };
 return 0;	
}

long int query_int(sqlite3 *db,char *sql)
{
 int stat;
 sqlite3_stmt *res;
 int step;
 long int ivalue=0;
 // MESG("query_int: [%s]",sql);

 if(db==NULL) { error_line("db is NULL!!!!!");return 0;};
 stat = sqlite3_prepare_v2(db, sql, -1, &res, 0);
 if(stat==SQLITE_OK) {
	step = sqlite3_step(res);

	if(step==SQLITE_ROW) {
//		MESG("	found: %s len=%d",sqlite3_column_text(res,0),strlen((char *)sqlite3_column_text(res,0)));
		// MESG("	found");
		ivalue = sqlite3_column_int(res,1);
		sqlite3_finalize(res);
		
		return ivalue;
	} else {
		MESG("	not found!");
		sqlite3_finalize(res);
		return ivalue;
	};
 } else {
 	// MESG("query_int: error %d [%s]",stat,sql);
	error_line("sql error:%d %s", stat,sql);
	return 0;
 };
 return 0;	
}

int query_int1(sqlite3 *db,char *sql)
{
 int num1;
 int index;
 const char *sout = (const char *)query_string(db,sql,&index);
 
	if(sout != NULL) {
		num1 = atoi(sout);
		return num1;
	} else return 0;
}

int sql_exec(sqlite3 *db, char *sql,int ignore)
{
 char *err_msg=NULL;
 // MESG("sql_exec: [%s]",sql);
 if(sqlite3_exec(db, sql, 0, 0, &err_msg)!=SQLITE_OK)
 {
 	if(ignore==0) error_line("sql_exec:[%s] error %s",sql, err_msg);
	sqlite3_free(err_msg);
	return false;
 };
 return true;
}
int opened_files=0;

void notes_db_close(sqlite3 *db)
{
	// MESG("notes_db_close:>");
	opened_files--;
	sqlite3_close(db);
}

sqlite3 * notes_db_open(char *message)
{
 char db_file_name[256];
 sqlite3 *db;
 set_bfname(db_file_name,NOTES_DBFILE);
 // MESG("notes_db_open:< [%s] opened=%d",db_file_name,opened_files);
 if(sqlite3_open(db_file_name,&db)!=SQLITE_OK) {
	error_line("Notes db [%s]: %s",db_file_name,sqlite3_errmsg(db));
	// notes_db_close(db);
	return NULL;
	if(init_notes_db(1)) {
		if(sqlite3_open(db_file_name,&db)!=SQLITE_OK){
			error_line("Cannot create Notes db [%s] at [%s]: %s",db_file_name,message,sqlite3_errmsg(db));
			return NULL;
		};
	} else return NULL;
 } else opened_files++ ;
 // MESG("notes_db_open: [%s] opened files %d",message,opened_files);
 return db;
}

int save_to_db(notes_struct *note)
{
 sqlite3 *db;
 if((db=notes_db_open("save_to_db"))==NULL) return false;

 char sql[MAXLLEN];
 int stat=false;
 int note_id=0;
 // MESG("save_to_db:0 title [%s] cat=[%s] name=[%s]",note->n_title,note->n_cat,note->n_name);
 // check if found
 if(snprintf(sql,sizeof(sql),"SELECT Category,rowid from notes where Name = '%s';",note->n_name)>=sizeof(sql)) {
 	ERROR("%s","Notes name too long!");
	// notes_db_close(db);
	return false ;
 };
 // MESG("	check if old, [%s]",sql);
 char *new_cat = query_string(db,sql,&note_id);
 // MESG(" new_cat=[%s] note_id is %d!",new_cat,note_id);
 
 char **current_tag_array = split_2_sarray(note->n_tags,',');
	 // MESG("	tags array [%s]",note->n_tags);
	char **ta=current_tag_array;
	char *tag;

	//	MESG("--- new tag list ----");
	while((tag = *ta++)!=NULL) {
		string_2_nospaces(tag);
		if(tag[0]=='#') tag++;
		// MESG("	- new tag  [%s]",tag);
	}; 
 
 // MESG("save_to_db:1 note_id=%d name=[%s] cat=[%s] new_cat=[%s]",note_id,note->n_name,note->n_cat,new_cat);
 if(note_id && !strcmp(note->n_cat,(char *)new_cat )) {
	// MESG("same note file! update [%s]/[%s]",new_cat,note->n_name);

	// delete all old tags (no check!!)
	// MESG("	- Delete old tags!");
	sprintf(sql,"delete from tags where note_id = %d;",note_id);
	if(!sql_exec(db,sql,0)){
		error_line("Cannot delete old tags!");
	};

	// save new tags
	// MESG("	- Save new tags!");
	ta = current_tag_array;
	while((tag = *ta++)!=NULL) {
		if(tag[0]=='#') tag++;
		// MESG("	real! tag %s",tag);
		save_tag(db,note_id,tag);
	};
	// update the title and date if[MCP7 changed!
	char *note_title = sql_sanitize(note->n_title);
	if(snprintf(sql,sizeof(sql),"UPDATE notes set Title = \"%s\" where rowid = %d",note_title,note_id)>=sizeof(sql)) {
		MESG("Title truncated!");
	} ;
	// MESG("update title sql=[%s]",sql);
	if(!sql_exec(db,sql,0)){
		error_line("Cannot update title");
	};
	if(strlen(note->n_date)>0)
	if(snprintf(sql,sizeof(sql),"UPDATE notes set Date = \"%s\" where rowid = %d",note->n_date,note_id)>=sizeof(sql)) {
		MESG("Title truncated!");
	} ;
	// MESG("update: %s",sql);
	if(!sql_exec(db,sql,0)){
		error_line("Cannot update date");
	};
 } else {
 	int new_note_id;
	// MESG("new note id! insert [%s]/[%s]",new_cat,note->n_name);
	char time_stamp[64];
	
	strlcpy(sql,"BEGIN TRANSACTION; ",MAXLLEN);
	strlcat(sql,"INSERT INTO notes(Name, Title, Date, Category, Encrypt, timestamp) ",MAXLLEN);
	strlcat(sql,"VALUES ('",MAXLLEN);
	strlcat(sql,note->n_name,MAXLLEN);  strlcat(sql,"','",MAXLLEN);
	strlcat(sql,sql_sanitize(note->n_title),MAXLLEN); strlcat(sql,"','",MAXLLEN);
	strlcat(sql,note->n_date,MAXLLEN);  strlcat(sql,"','",MAXLLEN);
	strlcat(sql,note->n_cat,MAXLLEN);   strlcat(sql,"',0",MAXLLEN);
	sprintf(time_stamp,",%lld);",note->timestamp);
	strlcat(sql,time_stamp,MAXLLEN);
	strlcat(sql,"END TRANSACTION;",MAXLLEN);
	// MESG("save_to_db:2 [%s]",sql);
	if (sql_exec(db,sql,0)){
		msg_line(" Note saved to db!");
		stat=true;
		new_note_id=sqlite3_last_insert_rowid(db);
	} else {
		msg_line("cannot create note record!");
		stat=false;
		new_note_id=0;

	};
	// create tags
 	if(new_note_id) {
		char **ta=current_tag_array;
		char *tag;
		// MESG("create tags!");
		while((tag = *ta++)!=NULL) {
			if(tag[0]=='#') tag++;
			save_tag(db,new_note_id,tag);
		};
 	} else stat=false;
 };
 free_sarray(current_tag_array);
 notes_db_close(db);
 // MESG("-------------------------- end saving");
 return stat;
}

int save_tag(sqlite3 *db,int notes_id,char *tag)
{
	int tag_id;
	char sql[MAXMLEN];
	// MESG("save_tag: %s",tag);
	if(snprintf(sql,sizeof(sql),"INSERT INTO TAG(NAME) VALUES('%s');",tag)>=sizeof(sql)) {
		ERROR("tag truncated!");
		return false;
	};
	if(sql_exec(db,sql,1)) {
		tag_id = sqlite3_last_insert_rowid(db);
		//	MESG("New tag [%s]",tag);
	} else {
		tag_id=0;
		// get tag id!
		if(snprintf(sql,sizeof(sql),"SELECT rowid from tag where name = '%s';",tag)>=sizeof(sql)) MESG("truncated tag!"); 
		tag_id = query_rowid(db,sql);
		//	MESG("tag id is %d",tag_id);
	};
	sprintf(sql,"INSERT INTO TAGS(tag_id,note_id) VALUES ('%d', '%d');",tag_id,notes_id);
	sql_exec(db,sql,0);
 return true;
}
#endif

#if	TNOTES
int string_is_empty(char *str)
{
 if(str==NULL) return true;
 char *s=str;
 
 do {
	if(*s == CHR_CR || *s == CHR_NEWLINE || *s==CHR_NULL) return true;
 	if(*s != CHR_SPACE && *s!=CHR_TAB) return false;
 } while(*s++);
 return true;
}
#endif

#if	TNOTES
// Parse note and create db fields
int parse_note(FILEBUF *fp)
{
 notes_struct *note=fp->b_note;
 offs ptr=0;
 int errors=0;
	if(!strcmp(fp->b_fname,".DS_Store")) {
		// MESG("	This is a mac ds_store file");
		return false;
	};
	if(note==NULL) note=fp->b_note=init_note();
	// MESG("!parse_note: --- b_type=%X dir=[%s] name=[%s] tags=[%s] lines=%d",fp->b_type,fp->b_dname,fp->b_fname,note->n_tags,fp->lines);
	if(fp->lines<2) {
		error_line("### parse error, too few lines (%d)!!",fp->lines);
		return false;
	};
	if(FSize(fp)<10) {
		error_line("### parse error size < 10",FSize(fp));
		return false;
	};
	if(fp->b_type & NOTE_CAL_TYPE) {
		// MESG("	--- NOTE_CAL_TYPE:fname=[%s]",fp->b_fname);
		strlcpy(note->n_name,fp->b_fname,sizeof(note->n_name));
		strlcpy(note->n_date,fp->b_fname,sizeof(note->n_date));
		// MESG("parse_note: calendar:  n_date=[%s] n_name=[%s]",note->n_date,note->n_name);
		if(strlen(note->n_name)==0)	{
#if	MD_NOTES
			if(snprintf(note->n_name,sizeof(note->n_name),"%s_cal.md",fp->b_fname)>sizeof(note->n_name)) MESG("note name truncated!");
#else
			if(snprintf(note->n_name,sizeof(note->n_name),"%s.cal",fp->b_fname)>sizeof(note->n_name)) MESG("note name truncated!");
#endif
		};
		// MESG("parse_note:2 n_name=[%s]",note->n_name);
		ptr = find_str_reol(fp,ptr,"# ",note->n_title,sizeof(note->n_title));
		if(ptr==0) find_str_reol(fp,ptr,"#Title: ",note->n_title,sizeof(note->n_title));
		// MESG("calendar, title = %s",note->n_title);
		strcpy(note->n_cat,"calendar/");
		memcpy(note->n_cat+9,fp->b_fname,4);note->n_cat[13]=0;	// add the year to calendar category
		// MESG("calendar cat [%s]",note->n_cat);

		note->n_tags[0]=0;
		ptr = find_str_reol(fp,ptr,"#Tags: ",note->n_tags,sizeof(note->n_tags));
		if(!strstr(note->n_tags,"calendar")){
			if(strlen(note->n_tags)>0) strncat(note->n_tags,",",sizeof(note->n_tags));
			strncat(note->n_tags,"calendar",sizeof(note->n_tags));
		};
		// MESG("start infile 1");
		add_infile_tags(fp,ptr,note->n_tags,sizeof(note->n_tags));
		return true;
	};

	if(fp->b_type & NOTE_TODO_TYPE) {
		// MESG("	--- NOTE_TODO_TYPE:");
		if(note==NULL) {
			// MESG("TODO note is null!!!");
			note=fp->b_note=init_note();
			strlcpy(note->n_name,fp->b_fname,sizeof(note->n_name));
			strlcpy(note->n_date,fp->b_fname,sizeof(note->n_date));
			// MESG("parse_note: todo:  n_date=[%s] n_name=[%s]",note->n_date,note->n_name);
		};

		if(strlen(note->n_name)==0)	{
#if	MD_NOTES
			if(!strstr(fp->b_fname,"_todo.md")){
				if(snprintf(note->n_name,sizeof(note->n_name),"%s_todo.md",fp->b_fname)>sizeof(note->n_name)) MESG("note name truncated!");
			} else {
				if(snprintf(note->n_name,sizeof(note->n_name),"%s",fp->b_fname)>sizeof(note->n_name)) MESG("note name truncated!");
			};
#else
			if(snprintf(note->n_name,sizeof(note->n_name),"%s.todo",fp->b_fname)>sizeof(note->n_name)) MESG("note name truncated!");
#endif
		};
		// MESG("todo, name [%s]",note->n_name);
		ptr = find_str_reol(fp,ptr,"#Title: ",note->n_title,sizeof(note->n_title));
		if(ptr==0) errors++;
		// MESG("	title = %s",note->n_title);
		strcpy(note->n_cat,"todo/");
		memcpy(note->n_cat+5,fp->b_fname,4);note->n_cat[9]=0;
		// MESG("	category 2 todo [%s]",note->n_cat);
		note->n_tags[0]=0;
		ptr = find_str_reol(fp,ptr,"#Tags:",note->n_tags,sizeof(note->n_tags));
		if(!strstr(note->n_tags,"todo")){
			if(strlen(note->n_tags)>0) strncat(note->n_tags,",",sizeof(note->n_tags));
			strncat(note->n_tags,"todo",sizeof(note->n_tags));
		};
		// MESG("	tags: [%s]",note->n_tags);
		// MESG("start infile 2");
		add_infile_tags(fp,ptr,note->n_tags,sizeof(note->n_tags));
		return true;
	};

	ptr = find_str_reol(fp,ptr,"#Date: ",note->n_date,sizeof(note->n_date));
	
	// MESG("Normal note: date ptr=%ld date=[%s]",ptr,note->n_date);
		char *start_cat_name = NULL;
		if((start_cat_name = strstr(fp->b_dname,"calendar")) !=NULL) {
			sprintf(note->n_tags,"calendar");
			if(strstr(fp->b_fname,".cal")||strstr(fp->b_fname,"_cal.md")) 
				strlcpy(note->n_name,fp->b_fname,sizeof(note->n_name));
			// MESG("calendar note: date=[%s]",note->n_date);
		} else 
		if((start_cat_name = strstr(fp->b_dname,"todo")) !=NULL) {
			sprintf(note->n_tags,"todo");
			// MESG("todo note: %s",note->n_date);
#if	1
			strcpy(note->n_cat,"todo/");
			memcpy(note->n_cat+5,fp->b_fname,4);note->n_cat[9]=0;
			strlcpy(note->n_name,fp->b_fname,sizeof(note->n_name));
#else
			strcpy(note->n_cat,"todo");
			if(strstr(fp->b_fname,"todo")) strlcpy(note->n_name,fp->b_fname,sizeof(note->n_name));
#endif
			// MESG("	todo set n_cat=%s n_name=%s",note->n_cat,note->n_name);
		} else { 
			MESG("not a note,calendar,todo!");
		};
	if(ptr==0) {
		errors++;
		strlcpy(note->n_date,fp->b_fname,sizeof(note->n_date));
		char *dot=strchr(note->n_date,'.');
		if(dot) dot[0]=0;
		else strcpy(note->n_date,"date error");
		// this is not a note file !, pass it as a calendar or todo one!!
		// check for title
		ptr = find_str_reol(fp,ptr,"# ",note->n_title,sizeof(note->n_title));
		if(ptr==0) {
			errors++;
			ptr = find_str_reol(fp,ptr,"#Title: ",note->n_title,sizeof(note->n_title));
			if(ptr==0) errors++;
		};
		// get files date as the creation date
		if(snprintf(note->n_cat,sizeof(note->n_cat),"%s",start_cat_name)>=sizeof(note->n_cat)) MESG("note category truncated");
		if(snprintf(note->n_name,sizeof(note->n_name),"%s",fp->b_fname)>=sizeof(note->n_name)) MESG("note name truncated");
		if(strlen(note->n_cat)==0) {
			MESG("### parse error, category is empty!");
			return false;
		};
		// MESG("	- parse_note:4 cat=%s name=%s errors=%d",note->n_cat,note->n_name,errors);
		if(errors>2) return false;
		// MESG("start infile 3");
		add_infile_tags(fp,ptr,note->n_tags,sizeof(note->n_tags));
		return(true);
	};
	// MESG("check name at %ld",ptr);
	if(strlen(note->n_name)==0) {
	ptr = find_str_reol(fp,ptr,"#Name: ",note->n_name,sizeof(note->n_name));
		if(ptr==0 && (strlen(note->n_tags)==0)) {
			MESG("!		name not found!, set to %s",fp->b_fname);
			if(snprintf(note->n_name,sizeof(note->n_name),"%s",fp->b_fname)>=sizeof(note->n_name)) MESG("note name truncated");
		};
	};
	// MESG("check title at %ld",ptr);
	ptr = find_str_reol(fp,ptr,"#Title: ",note->n_title,sizeof(note->n_title));
	if(ptr==0 ) {
		// MESG("	title not found!");
		ptr = find_str_reol(fp,ptr,"# ",note->n_title,sizeof(note->n_title));
		if(ptr==0 ) {
			strcpy(note->n_title,"No title");
			if((strlen(note->n_tags)==0)) {
				MESG("### parse error, No title!");
				return false;
			};
		};
	};

	ptr = find_str_reol(fp,ptr,"#Category: ",note->n_cat,sizeof(note->n_cat));
	if(ptr==0) {
	errors++;
	if(strlen(note->n_cat)==0) {
		char *cat=strstr(fp->b_dname,NOTES_DIR0);
			// MESG("	cat 5 %s",cat);
		if(cat-fp->b_dname==strlen(fp->b_dname)) {
			MESG("wrong category dir name=[%s] category=[%s]",fp->b_dname,cat);
			return false;
		};
		if(cat!=NULL) {
			cat+=strlen(NOTES_DIR0)+1;
			strlcpy(note->n_cat,cat,sizeof(note->n_cat));
			// MESG("	cat 5 %s",note->n_cat);
		};
	}} else { 
#if	1
	// TODO . check if category is same as dir when recreating database!!!
		char *cat=strstr(fp->b_dname,NOTES_DIR0);
		if(cat) {
		cat+=strlen(NOTES_DIR0)+1;
		
		if(strcmp(note->n_cat,cat)) {
			// MESG("	! cat 6 dir=[%s] cat=[%s] cat will be %s",cat,note->n_cat,cat);
			strlcpy(note->n_cat,cat,sizeof(note->n_cat));
		};
		};
#endif
	};
	// MESG("parse_note: Category = [%s] dir=%s name=[%s]",note->n_cat,fp->b_dname,fp->b_fname);
	ptr = find_str_reol(fp,ptr,"#Tags: ",note->n_tags,sizeof(note->n_tags));

	// MESG("start infile 4");
	add_infile_tags(fp,ptr,note->n_tags,sizeof(note->n_tags));

	// MESG("parse_note: tags = [%s]",note->n_tags);
	if(string_is_empty(note->n_cat)) {
		errors++;
		MESG("parse_note: category string is empty!");
#if	0
		goto_bof(1);
		next_line(3);goto_eol(1);
		insert_string(fp,"new",strlen("new"));
#endif
		strcpy(note->n_cat,"new");
	};

	if(string_is_empty(note->n_tags)) {
		// MESG("!	parse_note: tags string is empty!");
#if	0
		goto_bof(1);
		next_line(4);goto_eol(1);
		insert_string(fp,"new",strlen("new"));
#endif
		strcpy(note->n_tags,"new");
	};
	// MESG("	errors %d name=[%s]",errors,note->n_name);
	if(errors>3 || string_is_empty(note->n_name)) {
		MESG("### parse error errors=%d name=[%s]",errors,note->n_name);
		return false;
	};
	return true;
}

void set_notes_subdir(char *dname,char *subdir)
{
	// MESG("set_notes_subdir:[%s],[%s]",dname,subdir);
	set_bfname(dname,NOTES_DIR);
	if(strlen(subdir)>0) {
		strlcat(dname,"/",MAXFLEN);
		strlcat(dname,subdir,MAXFLEN);
	};
}

int save_note()
{
 int status=0;
 FILEBUF *fp=cbfp;
 notes_struct *note;
 // MESG("save_note:0");
	if((!parse_note(fp))) {
		// msg_line("%s not a note file!",fp->b_fname);
		return false;
	};
	if(fp->b_note->n_cat[0]==0) {
		msg_line("No category specified!");
		return false;
	};
	if(fp->b_fname[0]==0) {
		msg_line("can not save note with empty filename!");
		return false;
	};
	note=fp->b_note;
	// create category subdir if not present!
	set_notes_subdir(fp->b_dname,fp->b_note->n_cat);

	// MESG("save_note:1 dir path [%s][%s]",fp->b_dname,fp->b_fname);
	struct stat statd;
	if(stat(fp->b_dname,&statd)) {
		status=mkdirRecursive(fp->b_dname,S_IRWXU);
		if(status!=0) {
			return error_line("cannot create notes dir!");
		};
	};
	// MESG("save_note:2: b_fname=[%s]",fp->b_fname);
	// MESG("save_note:3: n_name=[%s]",note->n_name);
	// MESG("save_note:4: n_title=[%s]",note->n_title);
	strlcpy(fp->b_fname,note->n_name,sizeof(fp->b_fname));
 	if(fp->b_fname[0]!=CHR_LBRA){
		// MESG("save_note: add to recent!");
		add_to_recent_list(get_buf_full_name(fp));
	};
	// MESG("save_note:5 dir=[%s]",fp->b_dname);
	status=chdir(fp->b_dname);
	if(writeu1(fp->b_fname,fp)) {
		// MESG("test1 ------------- write1 ok!");
		// Get file timestamp and add it to notes
		note->timestamp = fp->FileTime;
		status=save_to_db(note);
	} else {
		MESG("cannot write file!");
	};
	return true;
}

int open_notes_dir(num n)
{
 int status=0;
 char go_name[MAXFLEN];

	set_bfname(go_name,NOTES_DIR);
	// MESG("open_notes: NOTES_DIR=%s",go_name);
	// MESG("open_notes: NOTES_PARENT=%s",NOTES_PARENT);
	status=chdir(go_name);
	if(status!=0) {
		// MESG("create notes dir! [%s]",go_name);
		status=mkdir(go_name,S_IRWXU);
		if(status!=0) {
			return error_line("cannot access Notes dir!");
		};
	};

//	MESG("open_notes_dir: [%s]",go_name);
	status = open_dir(go_name,DIR_NOTES);
	igotolinecol(1,1,-1);
	set_hmark(0,"dir_notes");

	return(status);
}

int search_notes_tagged(num n)
{
	return 1;
}

int view_notes_by_date(num n)
{
	return 1;
}

int show_tags(num n)
{
	return 1;
}

int move_to_notes(num n) 	/* dir mode  */
{
	return 1;
}

#endif

/*
	Notes tables
- Note
	- rowid -> id (unique)
	- name
	- title
	- date	-> n_date
	- category -> n_cat (category, directory)
	- encrypt
	- timestamp
- Tag
	- id (unique)
	- name
- Tags
	- tag_id
	- note_id)

*/


int callback_print(void *NotUsed, int argc, char **argv, 
                    char **azColName) {
    NotUsed = 0;
 	int i;   
    for (i = 0; i < argc; i++) {
        MESG("TABLE: %s", argv[i] ? argv[i] : "NULL");
    };
    return 0;
}

#if	TNOTES
// show tables
int show_sqlite_tables(char *fname)
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open(fname, &db);
    
    if (rc != SQLITE_OK) {
		error_line("Cannot open database: %s", sqlite3_errmsg(db));
        notes_db_close(db);
        return false;
    };
    
    char *sql = "SELECT name FROM sqlite_master WHERE type='table'";
    rc = sqlite3_exec(db, sql, callback_print, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        error_line("SQL error:[%s] %s",sql, err_msg);
        sqlite3_free(err_msg);
        notes_db_close(db);
        return false;
    } 
    
	notes_db_close(db);
    return true;
}

// Initialize notes database 
int init_notes_db(num n)
{
 sqlite3 *db;
 char *err_msg=NULL;
 int stat;

//	MESG("init_notes_db: sqlite3 version is %s",sqlite3_libversion());
	if((db=notes_db_open("init_notes_db"))==NULL) {
		return false;
	};
	// MESG("init_notes_db: db file opened!");
	char *sql = "DROP TABLE IF EXISTS notes;"\
				"CREATE TABLE notes(Id int primary key, Name TEXT, Title TEXT, Date TEXT, Category TEXT, Encrypt int, Timestamp int);"\
				"DROP TABLE IF EXISTS tag;"\
				"CREATE TABLE tag(Id int primary key, Name TEXT unique);"\
				"DROP TABLE IF EXISTS tags;"\
				"CREATE TABLE tags(Tag_id int, Note_id int)";
	// MESG("init sql is [%s]",sql);
	stat = sqlite3_exec(db, sql, 0,0, &err_msg);
	if(stat != SQLITE_OK) {
		error_line("sql error:[%s] %s",sql,err_msg);
		sqlite3_free(err_msg);
		notes_db_close(db);
		return false;
	} else {
		notes_db_close(db);
		msg_line("Notes db initialized!");
//		show_sqlite_tables(db_file_name);
		return true;
	};
}


#define	INIT_DB		1
#define	RESET_KEY	2

// reconstruct notes database from file contents.
int recreate_notes_db(num init_db)
{
 int status;
 char cmd[1024];
 char notes_dir[256];
 char tmp_file[256];
 char **notes_files;
 // FILEBUF *current_buffer=cbfp;
	// MESG("--- recreate_notes_db: ---- n=%d",n);
	set_bt_num_val("notes_recreate",1);
	// create notes db
	if(init_db==INIT_DB){
		MESG("recreate_note_db: Initialize database!");
		if(!init_notes_db(1)) return false;
	};
	set_bfname(notes_dir,NOTES_DIR);
	// set_notes_key(1);
	sprintf(tmp_file,"/tmp/notes_contents.out");
	sprintf(cmd,"find -L %s  >%s 2>/dev/null",notes_dir,tmp_file);
	status = system(cmd);
	if(status!=0) { return error_line("cannot read notes directory %s!",notes_dir);};
	sync();
	int size=0;
	int dirs=0;
	int notes_skipped=0;
	int notes_new=0;
	int notes_same=0;
	int notes_failstat=0;
	int notes_failparse=0;
	int i;
	notes_files = read_sarray(tmp_file,&size);
	int start_notes=0;
	start_notes = sindex(notes_files[0],"/Notes")+6;
	char notes_name[MAXFLEN];
	init_note_keys();
	for(i=0 ;notes_files[i]!=NULL;i++){
		struct stat st;
		// MESG("---- insert %d: [%s]",i,notes_files[i]);
		if(!stat(notes_files[i],&st))
		{
			int flen=strlen(notes_files[i]);
			if(flen>9) 
			if(!strcmp(notes_files[i]+flen-9,".DS_Store")) {
				notes_skipped++;
				MESG("- skip file [%s]",notes_files[i]);
				continue;
			};
			if(!S_ISDIR(st.st_mode)) {
				FILEBUF *bp;
				// MESG("  %3d insert: [%s]",i,notes_files[i]);

				// time_t prev_time ;
				notes_struct *s_note=init_note();
				create_base_name(notes_name,notes_files[i]+start_notes);

				if(!init_db) 
				{
				time_t tstamp = get_note_timestamp(notes_name);
					
				// MESG("---- %3d  %10s [%s] timestamp %ld file timestamp %ld",i,notes_files[i]+start_notes,notes_name,tstamp,st.st_mtime);
					if(tstamp==st.st_mtime) {
						notes_same++;
						continue;
					};
				};


				// MESG("  #%3d: go parse %s",i,notes_files[i]);
				bp=new_filebuf(notes_files[i],0);

				if(init_db == RESET_KEY) {
#if	1
					char fname[MAXFLEN];
					if(snprintf(fname,sizeof(fname),"%s/%s",bp->b_dname,bp->b_fname)<sizeof(fname)) {
					bp->file_id = open(fname,O_RDONLY);
					bp->bom_type = bom_type(bp->file_id);
					if (bp->bom_type == FTYPE_ENCRYPTED) {
						close(bp->file_id);
						// MESG("update crypt password for [%s]",fname);
						set_bt_num_val("notes_key",0);
						activate_file(bp);
						bp->b_mode |= EMCRYPT;
						// MESG("	-1 [%s] b_mode=%X",bp->b_fname,bp->b_mode);
						set_bt_num_val("notes_key",1);
						bp->b_state = FS_CHG;
						bp->b_key[0]=0;
						bp->b_type=NOTE_TYPE;
						// MESG(" -2 [%s] ",fname);
#if	1
						// FILEBUF *ori_buf=cbfp;
						// cbfp=bp;
						writeout(fname,bp);
						// cbfp = ori_buf;
#endif
						MESG("	- file [%s] is encrypted",fname);

						set_bt_num_val("notes_key",0);
						notes_same++;
					}};
					delete_filebuf(bp,1);
					continue;
#endif
				};

				bp->b_note = s_note;
				activate_file(bp);

				if(!parse_note(bp)) {
					msg_line(" file [%s] Not a note file !!!!!!!!!!!!",notes_files[i]);
					delete_filebuf(bp,1);
					// select_filebuf(current_buffer);
					MESG("- failed parse, skip file [%s]",notes_files[i]);
					notes_failparse++;
					continue;					
				};
				MESG("  %3d: add - n=[%s] title=[%s] cat=[%s] tags=[%s] cr=[%s]",i,s_note->n_name,s_note->n_title,s_note->n_cat,s_note->n_tags,bp->b_key);
				s_note->timestamp = st.st_mtime;
				status=save_to_db(s_note);
				notes_new++;
				// select_filebuf(current_buffer);
				delete_filebuf(bp,1);

				// MESG("	+ %3d note added [%s]!",i,notes_files[i]);
			} else {
				dirs++;
				// select_filebuf(current_buffer);
				// MESG("- skip, %s this is dir",notes_files[i]);
			};
		} else {
			MESG("- skip file that cannot stat file %s",notes_files[i]);
			notes_failstat++;
		};
		
	};
	set_bt_num_val("notes_recreate",0);
	free_sarray(notes_files);
	// select_filebuf(current_buffer);
	msg_line("# recreated_notes_db: new %d, dirs %d, skipped %d failed %d same %d failed parse %d",notes_new,dirs,notes_skipped,notes_failstat,notes_same,notes_failparse);
	return true;
}
#endif


// use an int array for row ids for tags (max 2);
#define	MAX_TAGS	10
int num_of_selected_tags=0;
int sel_tags[11];
int num_of_selected_notes=0;
int *sel_notes;

#if	TNOTES
char *sql_note_str(char *query_string)
{
 static char sql_str[MAXLLEN];
 char s_num[16];
 int i;
 strlcpy(sql_str,query_string,MAXLLEN);
 // MESG("sql_note_str:(%s)",query_string);
 	if(num_of_selected_tags>0) {
 	
	strlcat(sql_str," where rowid in (select note_id  from tags group by note_id having count(case when tag_id in (",MAXLLEN);
	sprintf(s_num,"%d",sel_tags[0]);
	strlcat(sql_str,s_num,MAXLLEN);
	for(i=1;i<num_of_selected_tags;i++) {
		sprintf(s_num,"%d",sel_tags[i]);
		strlcat(sql_str,",",MAXLLEN);
		strlcat(sql_str,s_num,MAXLLEN);
	};
	strlcat(sql_str,") then 1 end) = ",MAXLLEN);
	snprintf(s_num,sizeof(s_num),"%d",num_of_selected_tags);
	strlcat(sql_str,s_num,MAXLLEN);
	strlcat(sql_str,")",MAXLLEN);
	};

 // MESG("sql_note_str:->(%s)",sql_str);
 return sql_str;
}

int insert_lines(sqlite3 *db, char *query_string, int *widths)
{
 int line=1;
 istr *row_data;
 char *column_string;
 char sql_str[MAXLLEN];
 strlcpy(sql_str,query_string,MAXLLEN);
 alist *column_list;
 // MESG("insert_lines: sql_str=[%s]",sql_str);
 column_list = query_string_columns(db,sql_str,widths);
 
 cbfp->dir_list_str = column_list;
	line = 1;

	lbegin(column_list);
	while((row_data = (istr *)lget(column_list))!=NULL)
	{
		column_string = &row_data->start;
		// MESG("insert %2d [%s]",line,column_string);
		MoveLineCol(line++,0);
		insert_string(cbfp,column_string,strlen(column_string));
		insert_newline(cbfp);
	};
//	MESG("insert_lines: col=%d tags=%d notes=%d",start_col,num_of_selected_tags,num_of_selected_notes);
	return line;
}


int show_category_list(char *category)
{
	sqlite3 *db;
	char *header = " DATE            │ TITLE                                                                                               ";
	int widths[] = { 2, 15, 65 };
	char sql[MAXLLEN];
	char buffer_name[64];
	int tag_id ;
	FILEBUF *cat_view;

	if((db=notes_db_open("show_category_list"))==NULL) return false;
	// MESG("show_cagegory_list:%s",category);
	char sql_get_todo_tag[MAXLLEN];
	sprintf(sql_get_todo_tag,"SELECT rowid from tag where name = '%s';",category);
	tag_id = query_rowid(db,sql_get_todo_tag);
	// MESG("category id %d",tag_id);
	
	if(tag_id<1) {notes_db_close(db);return 0;};
	sprintf(sql,"SELECT rowid,DATE,TITLE FROM NOTES where rowid in (select note_id from tags where tag_id = %d) order by DATE ASC;",tag_id);
	if(strlen(category)>55) category[55]=0;
	snprintf(buffer_name,sizeof(buffer_name),"[%s view]",category);
	cat_view = cls_fout(buffer_name);

	if(cbfp->b_flag & FSNOTES) {
		// MESG("set connect buffer");
		cat_view->connect_buffer = cbfp;
		cat_view->connect_top_line = cwp->top_tag_line;
		cat_view->connect_top_note = cwp->top_note_line;
		cat_view->connect_line = cwp->current_tag_line;
		cat_view->connect_column = cwp->current_note_line;
		cat_view->b_type = NOTE_TYPE;
	};

	select_filebuf(cat_view);

	cbfp->b_header = header;
	cat_view->b_flag = FSNCALENDAR;

	cat_view->dir_list_str = query_string_columns(db,sql,widths);
	notes_db_close(db);
	cat_view->b_notes = cat_view->dir_list_str->size;
	// MESG("todo entries %d",cat_view->b_notes);
	cat_view->b_state = FS_VIEW|FS_ACTIVE;
	set_update(cwp,UPD_WINDOW);
	return 1;
}

int show_calendar_list(num n)
{
	return show_category_list("calendar");
}

int show_todo_list(num n)
{
	return show_category_list("todo");
}

int show_tag_view(num n)
{
	sqlite3 *db;
	FILEBUF *tag_view;
	// MESG("show_tag_view:");
	if((db=notes_db_open("show_tag_view"))==NULL) return false;
	if(n==2) {
		tag_view = get_filebuf("[Tag view]",NULL,0);
	} else {
		tag_view = cls_fout("[Tag view]");
	};
	select_filebuf(tag_view);
#if	TITLE_FIRST
	cbfp->b_header=" TAG             | TITLE                                                   | CATEGORY           | NAME                ";
#else
	if(cbfp->b_lang==0) {
		cbfp->b_header=" TAG             │ NAME                │ CATEGORY           │ TITLE                                                   │";
	} else {
		cbfp->b_header=" TAG             | NAME                | CATEGORY           | TITLE                                                   ";
	};
#endif
	int tag_width[] = { 1,TAGS_WIDTH };
#if	TITLE_FIRST
	int notes_widths[] = { 3 ,55, 18, 25};
#else
	int notes_widths[] = { 3 ,19, 18, 55};
#endif
	if(n!=2){
		empty_list(tag_view->b_tag_list);
		tag_view->b_tag_list = query_string_columns(db,"SELECT ROWID,NAME FROM TAG",tag_width);
	};

	empty_list(tag_view->dir_list_str);
#if	TITLE_FIRST
	tag_view->dir_list_str = query_string_columns(db,sql_note_str("SELECT ROWID,TITLE,CATEGORY,NAME FROM NOTES"),notes_widths);
#else
	tag_view->dir_list_str = query_string_columns(db,sql_note_str("SELECT ROWID,NAME,CATEGORY,TITLE FROM NOTES"),notes_widths);
#endif
	notes_db_close(db);

	cwp->current_note_line=0;
	cwp->top_note_line=0;
	tag_view->b_tags = tag_view->b_tag_list->size;
	tag_view->b_notes = tag_view->dir_list_str->size;
	
	set_highlight(tag_view,highlight_type("TAGVIEW"));
	tag_view->b_flag = FSNOTES;
	tag_view->b_state = FS_ACTIVE|FS_VIEW;

	msg_line(" Tag column active");	
	set_hmark(0,"show_tag_view");
	set_update(cwp,UPD_EDIT|UPD_WINDOW);
	return 1;
}

int reload_tag_view()
{
	int b_flag=cbfp->b_flag;
	int top_note_line=cwp->top_note_line;
	int note_line=cwp->current_note_line;
	int stat=show_tag_view(1);
	cbfp->b_flag=b_flag;
	if(note_line > cbfp->b_notes) note_line=cbfp->b_notes;
	if(top_note_line > cbfp->b_notes) top_note_line=cbfp->b_notes;

	cwp->current_note_line=note_line;
	cwp->top_note_line=top_note_line;
	return stat;
}

#endif

int set_tag_view_position(int line,int column)
{
#if	TNOTES
 static int prev_line=-1;
 static int prev_col=-1;
#endif
	
	// MESG("set_tag_view_position: line=%d column=%d",line,column);
	if(cwp->w_fp->b_flag & FSNLIST) {
		cwp->current_note_line=line;
		column=0;
		// MoveLineCol(line,0);
	} 
#if	TNOTES
	else
	if(column < NOTES_COLUMN) {
		cwp->w_fp->b_flag = FSNOTES;
		column=0;
		if(line>=0) {
		line+=cwp->top_tag_line;
		cwp->current_tag_line = line;
		// MESG("set tag at line %d",line);
		if(prev_line==line && prev_col==column) {
			// toggle tag 
			if(line>0) select_tag(TAG_SELECT_TOGGLE);
		};
		} else line=cwp->current_tag_line;;
	} else {
		cwp->w_fp->b_flag = FSNOTESN;
		column = NOTES_COLUMN+2;
		if(line>=0) {
		line+=cwp->top_note_line;
		if(prev_line==line && prev_col==column) {
			if(line>0) edit_note(0);
//			MESG("set_tag_view_position: after edit_note!");
		};
	
		cwp->current_note_line = line;
		} else line = cwp->current_note_line;
		// MESG("set note at line %d",line);
	};
	prev_col=column;
	prev_line=line;
	if(prev_line<0) prev_line=0;
#endif
	// MESG("==: b_flag=%X line=%d col=%d",cwp->w_fp->b_flag,line,column);
	set_update(cwp,UPD_MOVE);
	return column;
}

#if	TNOTES
int toggle_tagnote(num n)
{
	// int b_flag = (cbfp->b_flag >>6) << 6;
	int b_flag = cbfp->b_flag;
	if(b_flag & FSNLIST) return FALSE;

	// MESG("toggle_tagnote: b_flag=%X current_tag_line=%d",cbfp->b_flag,cwp->current_tag_line);
	if(b_flag==FSNOTES) {
		cbfp->b_flag=FSNOTESN;
		msg_line("Notes column active");

	} else {
		cbfp->b_flag=FSNOTES;
		msg_line("Tag column active %d",cwp->current_tag_line);
	};
	update_hmark();
	set_update(cwp,UPD_EDIT|UPD_WINDOW);
	return TRUE;
}

void update_tag_linecol()
{
 num line=cwp->current_tag_line;
 num col;
 int b_flag = (cbfp->b_flag >>7) << 7;

	if(b_flag & FSNOTESN) {
		col=NOTES_COLUMN+2;
	} else {
		col=0;
	};
	// MESG("update_tag_linecol: b_flag=%X col=%ld",cbfp->b_flag,col);
	MoveLineCol(line,col);
	update_hmark();
}

int toggle_tag(num n)
{
	return select_tag(TAG_SELECT_TOGGLE);
}

int unselect_tag(num n)
{
	return select_tag(TAG_UNSELECT_CURRENT);
}

int remove_tags(num n)
{
	return select_tag(TAG_UNSELECT_ALL);
}

int get_id_from_list(alist *list,int ind)
{
 istr *data;
 lbegin(list);
 int i;
 // MESG("get_id_from_list: %d",ind);
 for(i=0;(data=(istr *)lget(list))!=NULL;i++) {
	// MESG("  -- check %d == %d",i,ind);
 	if(i==ind) return data->index;
 };
 // MESG("get_id_from_list: %d not found",ind);
 return 0;
}

// n=0	-> unselect all tags
// n=1	-> select current tag
// n=2	-> unselect current tag

int select_tag(num n)
{
 char *select_word=NULL;
 int tag_id=0;
 // MESG("select_tag: n=%d current_tag=%d",n,cwp->current_tag_line);

	if(cwp->w_fp->b_flag & FSNLIST) {
		if(n==0) {
			istr *dir_str;
			lbegin(cbfp->dir_list_str);
			while((dir_str = (istr *)lget_current(cbfp->dir_list_str))!=NULL) {
				dir_str->selection_tag=0;
				lmove_to_next(cbfp->dir_list_str,0);
			};
			set_update(cwp,UPD_EDIT);
		 	return 1;
		} else return dir_tag(n);
	};

	if(cwp->w_fp->b_flag & FSNOTESN)
	{
		return 0;
	};
	if(n>0) {
		tag_id = get_id_from_list(cwp->w_fp->b_tag_list,cwp->current_tag_line);
		sqlite3 *db;
		char sql[MAXMLEN];		
		if((db=notes_db_open("select_tag"))==NULL) return false; 
		sprintf(sql,"SELECT name from tag where rowid = %d;",tag_id); 
		int tag_id1=0;
		select_word = query_string(db,sql,&tag_id1); 
		notes_db_close(db);
		// MESG("	found select_word=[%s] tag_id=%d",select_word,tag_id);

		istr **row_data = (istr **) array_data(cbfp->b_tag_list);
		istr *tag_istr = row_data[cwp->current_tag_line];

		switch(n) {
			case TAG_SELECT_CURRENT: {
				if(num_of_selected_tags==MAX_TAGS) return 0;
				int ind;
				ind = iarray_index(sel_tags,tag_id,num_of_selected_tags);
				if(ind<0) { 
					// not found, add it
					num_of_selected_tags = iarray_add(sel_tags,tag_id,num_of_selected_tags);
					add2_sarray(cbfp->hl->w1,strdup(select_word));
				};
				tag_istr->selection_tag=1;
			};break;
			case TAG_UNSELECT_CURRENT: {
				int ind;
				ind = iarray_index(sel_tags,tag_id,num_of_selected_tags);
				if(ind>=0) { // it exists, remove it
					num_of_selected_tags = iarray_remove(sel_tags,ind,num_of_selected_tags);
					remove_from_sarray(cbfp->hl->w1,select_word);
				};
				tag_istr->selection_tag=0;
			};break;
			case TAG_SELECT_TOGGLE: {
				int ind;
				ind = iarray_index(sel_tags,tag_id,num_of_selected_tags);
				if(ind>=0) { // it exists, remove it
//					MESG(" toggle remove %s",select_word);
					num_of_selected_tags = iarray_remove(sel_tags,ind,num_of_selected_tags);
					remove_from_sarray(cbfp->hl->w1,select_word);
				} else {	// not found, add it
//					MESG(" toggle add [%s] num_selected_tags=%d",select_word,num_of_selected_tags);
					num_of_selected_tags = iarray_add(sel_tags,tag_id,num_of_selected_tags);
					add2_sarray(cbfp->hl->w1,strdup(select_word));
				};
				// cwp->current_tag_line=0;
				// cwp->top_note_line=0;

				if(tag_istr->selection_tag) tag_istr->selection_tag=0;
				else tag_istr->selection_tag=1;

				return show_tag_view(2);
			};
		}; 
	} else { // TAG_UNSELECT_ALL
//		MESG("select_tag: remove all!!");
		sel_tags[0]=sel_tags[1]=0;
		num_of_selected_tags=0;
		sarray_clear_data(cbfp->hl->w1);
		// cwp->current_tag_line=0;
		// cwp->top_note_line=0;
	};
	return show_tag_view(1);
}


int delete_note_row()
{
	istr *row_data;
	// MESG("delete_note_row: tag_line=%d note_line=%d",cwp->current_tag_line,cwp->current_note_line);
	int line=0;
	// int note_id=-1;

	lbegin(cbfp->dir_list_str);
	while((row_data = (istr *)lget_current(cbfp->dir_list_str))!=NULL) {

		if(line == cwp->current_note_line) {
			// MESG("delete_note_row: line=%d note_id = %d size=%d",line,row_data->index,cbfp->dir_list_str->size);
			remove_current_from_list(cbfp->dir_list_str);
			// MESG("delete_note_row: line=%d note_id = %d size=%d",line,row_data->index,cbfp->dir_list_str->size);
			cbfp->b_state ^= FS_VIEW;
			if(line==cbfp->dir_list_str->size && line>0) cwp->current_note_line--;

			cbfp->b_state |= FS_VIEW;
			set_update(cwp,UPD_EDIT);
			cbfp->b_notes--;
			break;
		};
		line++;
		lmove_to_next(cbfp->dir_list_str,0);
	};

	return true;
}

int get_current_tag_id()
{
 int ind=cwp->current_tag_line;
 // MESG("get_current_tag_id: ind=%d",ind);
 istr **row_data = (istr **)array_data(cbfp->b_tag_list);
 if(row_data==NULL) return -1;
 int tag_id = row_data[ind]->index;
 // MESG("get_current_tag_id: = %d",tag_id);
 return tag_id;
}

char *get_current_tag_name()
{
 char *tag_name;
 char sql[1024];
 sqlite3 *db;
 int tag_id;

 if((db=notes_db_open("get_current_tag_name"))==NULL) return NULL;
 tag_id = get_current_tag_id();
 // MESG("get_current_tag_name: tag_id=%d",tag_id);
 if(tag_id<0) return NULL;

 sprintf(sql,"SELECT name,rowid from tag where rowid = %d",tag_id);

 tag_name = query_string(db,sql,&tag_id);
 notes_db_close(db);

 return tag_name;
}

int get_current_note_id()
{
 int ind=cwp->current_note_line;
 // MESG("get_current_note_id: ind=%d",ind);
 istr **row_data = (istr **)array_data(cbfp->dir_list_str);
 if(row_data==NULL) return -1;
 int note_id=-1;
 if(row_data[ind]) note_id = row_data[ind]->index;
 // MESG("get_current_note_id: = %d",note_id);
 return note_id;
}

time_t get_note_timestamp(char *note_name)
{
 char sql[MAXLLEN];
 sqlite3 *db;
 long int tstamp=0;
 if((db=notes_db_open("get_note_timestamp"))==NULL) return 0;

 if(snprintf(sql,sizeof(sql),"SELECT rowid,timestamp from notes where Name = '%s';",note_name)<sizeof(sql)) {
	tstamp = query_int(db,sql);
 };
 notes_db_close(db);

 return tstamp;
}

char *get_current_note_name()
{
 static char full_name[MAXLLEN];
 char notes_name[MAXLLEN];
 sqlite3 *db;
 int note_id;

 if((db=notes_db_open("get_current_note_name"))==NULL) return NULL;
 note_id = get_current_note_id();
 if(note_id<0) return NULL;
 // MESG("get_current_note_name: note_id=%d",note_id);

 sprintf(full_name,"SELECT category,rowid from notes where rowid = %d",note_id);
 // MESG("	category,rowid = [%s]",full_name);
 // MESG("	NOTES_DIR=[%s]",NOTES_DIR);
 if(snprintf(notes_name,sizeof(notes_name),"%s/%s/",NOTES_DIR,query_string(db,full_name,&note_id))>=sizeof(notes_name)) {
 	error_line("notes file name too long, truncated!"); ; 
 };
 // MESG("	notes_name = [%s]",notes_name);
 sprintf(full_name,"SELECT name,rowid from notes where rowid = %d",note_id);
 // MESG("	full_name = [%s]",full_name);

 strlcat(notes_name,query_string(db,full_name,&note_id),MAXLLEN);
 notes_db_close(db);

 // MESG("	notes_name = [%s]",notes_name);

 set_bfname(full_name,notes_name);
 return full_name;
}


int edit_note(num n)
{
  // MESG("edit_note:");
  char *full_name = get_current_note_name();
 
 if(full_name==NULL) {
 	error_line("edit_note: note does not exist!");
	return false;
 };
 FILEBUF *bp;
 int btype=0;
 
#if	1
	if(!strncmp(".cal",full_name+(strlen(full_name)-4),4) 
		|| !strncmp("_cal.md",full_name+(strlen(full_name)-7),7) 
		){
		btype = NOTE_CAL_TYPE;
	} else if(!strncmp(".todo",full_name+(strlen(full_name)-5),5)
			||!strncmp("_todo.md",full_name+(strlen(full_name)-8),8))
		{
		btype=NOTE_TODO_TYPE;
	} else btype=NOTE_TYPE;
#endif

	// MESG("edit_note:[%s] n=%d b_type=%d",full_name,n,cbfp->b_type);
	// if(edit_file(full_name)) return true;
	// MESG("edit_note: new_filebuf: [%s]",full_name);
	if((bp=new_filebuf(full_name,NOTE_TYPE))==NULL) {
    	error_line("Cannot open note [%s]",full_name);
        return (FALSE);
	};
	// MESG("edit_note: set b_type");
	bp->connect_buffer = cbfp;
	bp->connect_top_line = cwp->top_tag_line;
	bp->connect_top_note = cwp->top_note_line;
	bp->connect_line = cwp->current_tag_line;
	bp->connect_column = cwp->current_note_line;
#if	1
	bp->b_type |= btype;
#else
	if(!strncmp(".cal",bp->b_fname+(strlen(bp->b_fname)-4),4) 
		|| !strncmp("_cal.md",bp->b_fname+(strlen(bp->b_fname)-7),7) 
		){
		bp->b_type|=NOTE_CAL_TYPE;
	} else if(!strncmp(".todo",bp->b_fname+(strlen(bp->b_fname)-5),5)
			||!strncmp("_todo.md",bp->b_fname+(strlen(bp->b_fname)-8),8))
		{
		bp->b_type|=NOTE_TODO_TYPE;
	} else bp->b_type|=NOTE_TYPE;
#endif
	// MESG("edit_note: select_filebuf");
	select_filebuf(bp);

	set_hmark(1,"edit_note");

 if(n==0) bp->b_state |= FS_VIEW;
 return true;
}

int view_note(num n)
{
	// int b_flag = (cbfp->b_flag >>8) << 8;
	int b_flag = cbfp->b_flag;
	// MESG("view_note: %X %X FSNOTES=%X",cbfp->b_flag,b_flag, FSNOTES);
	if( b_flag == FSNOTESN
		|| b_flag & FSNLIST
	) {
		// MESG("->edit_note");
		return edit_note(0);
	} else if (b_flag & FSNOTES) {
		// MESG("->show_category");
		return show_category_list(get_current_tag_name());
	} else return false;
}

int delete_note_file(char *full_name)
{
 int stat;
	stat = unlink(full_name);
	if(stat!=0) {
		error_line("cannot delete file [%s]",full_name);
		if( confirm("Cannot delete the note file", "delete note from db?",1)) stat=0;
	};
	return stat;
}

int delete_noteid(int note_id)
{
	sqlite3 *db;
	char sql_str[MAXLLEN];
	if(note_id>0){
	// MESG("delete_noteid: %d",note_id);
		if((db=notes_db_open("delete_note_id"))==NULL) return false;
	
		// delete note
		sprintf(sql_str,"DELETE FROM NOTES WHERE rowid = %d",note_id);
		// MESG("sql [%s]",sql_str);
		if(!sql_exec(db,sql_str,0)) {
			error_line("cannot delete note %d",note_id);
		};
		// in any case delete correspondig tags entries
		sprintf(sql_str,"DELETE FROM TAGS WHERE note_id = %d",note_id);
		if(!sql_exec(db,sql_str,0)) {
			error_line("cannot delete note_id %d from tags!",note_id);
		};

		notes_db_close(db);

		return true;
	} else return false;
}

int delete_tagnote(num force)
{
  int b_flag = (cbfp->b_flag >>8) << 8;
  if(b_flag == FSNLIST) {
	int note_id = get_current_note_id();
	// MESG("delete_tagnote: list");

	char *full_name = get_current_note_name();
	if(!force)
 		if(!confirm("Delete note",full_name,0)) return false;
	if(full_name==NULL) {
	 	MESG("edit_note: note name is null!");
		return false;
	};

	// MESG("delete_tagnote: full_name=[%s] id=%d",full_name,note_id);	
	// delete note file
	if(!delete_note_file(full_name))
	{
		delete_noteid(note_id);
		// delete note row
		delete_note_row();
		return true;
	};		
	return false;
  };
  if(cwp->w_fp->b_flag & FSNOTES) {	/* delete tag  */
  //	delete tag !
  // 	if there are no notes with this tag then delete it.
  //	else ask to delete all notes with this tag ????
	char sql_str[MAXLLEN];
	char tag_name[MAXLLEN];
	int count=0;
	sqlite3 *db;
	
	if((db=notes_db_open("delete_tag_note"))==NULL) return false;

	int tag_id = get_current_tag_id();
	strlcpy(tag_name, get_current_tag_name(),sizeof(tag_name));
	if(!confirm("Delete tag",tag_name,0)) return false;
	// MESG("delete_tagnote: line=%d tag_id=%d name %s",cwp->current_tag_line,tag_id,tag_name);

	sprintf(sql_str,"select count (tag_id) from tags where tag_id = %d;",tag_id);
	count = query_int1(db, sql_str);
	if(count==0) {	// safelly delete the tag 
		sprintf(sql_str, "delete from tag where rowid = %d",tag_id);
		if(!sql_exec(db,sql_str,0)) {
			error_line("cannot delete tag %s with rowid %d",tag_name,tag_id);
		} else {
			msg_line("deleted tag [%s]",tag_name);
		};
	} else {
		msg_line("cannot delete tag [%s], used %d times!",tag_name,count);
		notes_db_close(db);
		return false;
	};
	notes_db_close(db);
	show_tag_view(1);
  } else {		/* delete note  */
	int note_id = get_current_note_id();
	char *full_name = get_current_note_name();
	if(!confirm("Delete note",full_name,0)) return false;
	// delete the file
	if(!delete_note_file(full_name))
	{
		msg_line("deleted note file [%s]",full_name,cwp->current_note_line);
		// delete the note
		// MESG("file %s deleted, delete noteid %d",full_name,note_id);
		if(delete_noteid(note_id)) {
			delete_note_row();
			set_update(cwp,UPD_MOVE|UPD_WINDOW);
			return true;
		} else return false;
	} else {
		error_line("could not delete note file [%s]",full_name);
	};
	// refresh tag view
	set_update(cwp,UPD_MOVE|UPD_WINDOW);
	show_tag_view(1);
  };
  return true;
}

// Convert to note
int save_as_note(num n)
{
 // notes_struct *note=NULL;
	// insert preamble
	if(cbfp->b_flag & FSDIRED) dir_edit(1);
	goto_bof(1);
	insert_preamble(cbfp,1);

	goto_bof(1);
	next_line(1);goto_eol(1);
	insert_string(cbfp,cbfp->b_fname,strlen(cbfp->b_fname));

	goto_bof(1);
	next_line(3);goto_eol(1);
	insert_string(cbfp,"new",strlen("mew"));

	goto_bof(1);
	next_line(4);goto_eol(1);
	insert_string(cbfp,"new",strlen("mew"));

	set_notes_subdir(cbfp->b_dname,"new");
	goto_bof(1);
	next_line(2);goto_eol(1);	/* goto title line  */
	cbfp->b_type=NOTE_TYPE;

	if((!parse_note(cbfp))) {
		msg_line("%s not a note file!",cbfp->b_fname);
		return 0;
	};

	set_update(cwp,UPD_WINDOW);
	return true;
}

char *note_keys[2];

void init_note_keys()
{
	// set_bt_num_val("notes_key",0);
	if(note_keys[0]!=NULL) {
		free(note_keys[0]);
		note_keys[0]=NULL;
	};
	if(note_keys[1]!=NULL) {
		free(note_keys[0]);
		note_keys[1]=NULL;
	};
}

char *get_notes_key(int key_type)
{
	// MESG("get_notes_key: %d",key_type);
	if(note_keys[key_type]==NULL) 
		set_notes_key(key_type);
	return note_keys[key_type];
}

#define	OLD_NOTES_KEY	0
#define	NEW_NOTES_KEY	1

void set_local_notes_key(char *key,int key_type)
{
	if(note_keys[key_type]==NULL) note_keys[key_type]=(char *)malloc(MAXSLEN);
	if(strlen(key)>MAXSLEN) ERROR("notes key too big");
	strlcpy(note_keys[key_type],key,MAXSLEN);	
}


int set_notes_key(num key_type)
{
	char b_key[MAXSLEN];
	int status=1;
	b_key[0]=0;
	// MESG("set_notes_key:");
	if(key_type) 
	status = nextarg("New Notes Encryption String: ", b_key, MAXSLEN,false);
	else
	status = nextarg("Notes Encryption String: ", b_key, MAXSLEN,false);
//	disinp = odisinp;
	if (status != TRUE)  return(status);
	
	/* and encrypt it */
	crypt_string(NULL, 0L);
	crypt_string(b_key, strlen(b_key));
	set_local_notes_key(b_key,key_type);
	// msg_line("notes key %ld set ok!",key_type);
	return true;
}

char *get_notes_status()
{
 static char statuss[MAXLLEN];
 istr **row_data;
 // MESG("get_notes_status:");
 if(cbfp->b_flag & FSNLIST) {
 	// MESG("get_notes_status: FSNLIST");
 	// MESG("get_notes_status: FSNLIST current_note_line=%ld",cwp->current_note_line);
	if(cbfp->dir_list_str){
	row_data = (istr **)array_data(cbfp->dir_list_str);
	if(row_data)
	if(cwp->current_note_line<cbfp->dir_list_str->size)
		sprintf(statuss,"N %3d",row_data[cwp->current_note_line]->index);
	};
 } else 
 if(cbfp->b_flag & FSNOTES) {
 	// MESG("get_notes_status: FSNOTES");
 	// MESG("get_notes_status: FSNOTES %s",cwp->current_tag_line);
	row_data = (istr **)array_data(cbfp->b_tag_list);
	if(row_data)
	if(cwp->current_note_line<cbfp->b_tag_list->size) {
		sprintf(statuss,"T %3d %d",row_data[cwp->current_tag_line]->index,row_data[cwp->current_tag_line]->selection_tag);
	};
 } else {
 	// MESG("get_notes_status: dir list ?");
 	// MESG("get_notes_status: else %d",cwp->current_note_line);
	row_data = (istr **)array_data(cbfp->dir_list_str);
	if(row_data)
	if(cwp->current_note_line<cbfp->dir_list_str->size)
		sprintf(statuss,"N %3d",row_data[cwp->current_note_line]->index);
 };
 sprintf(statuss,"-----");
 return statuss;
}
#endif

/*
	sort notes
		by name,date

	- add file contents to notes
	- save note contents to file
	- copy note contents (...)
	- Separate notes menu !
*/

/* --- */
