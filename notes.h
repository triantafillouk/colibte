/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

	Notes support
*/

/*	----- sqlite3 utility functions ---- */
#include <sqlite3.h>

#define	NOTES_PARENT	"~/.colibte"
						// "~/.colibte/Notes"
#define NOTES_DIR		NOTES_PARENT"/Notes"
#define NOTES_DBFILE	NOTES_PARENT"/notes.db"
#define NOTES_DIR0		"Notes"

#define	TAG_UNSELECT_ALL		0
#define TAG_SELECT_CURRENT		1
#define	TAG_SELECT_TOGGLE		2
#define	TAG_UNSELECT_CURRENT	3

// open an sqlite3 database
sqlite3 * notes_db_open(char *message);
// returns multiple rowss
char ***query_rows(sqlite3 *db, char *sql);
// returns one row
char **query_row(sqlite3 *db, char *sql);
// returns one column in a list
alist *query_string_column(sqlite3 *db, char *sql);
// return a numeric array (first value is the number of elements)
double *query_num_column(sqlite3 *db, char *sql);
// return a single string
char *query_string(sqlite3 *db,char *sql,int *index);
// return the index
int query_rowid(sqlite3 *db,char *sql);
// return a single int
int query_int1(sqlite3 *db,char *sql);
// execute an sql query with specific returns
int sql_exec(sqlite3 *db, char *sql,int ignore);
int callback_print(void *NotUsed, int argc, char **argv, char **azColName);
// show tables
int show_sqlite_tables(char *fname);

/* ----- notes utility functions ----- */
int insert_preamble(FILEBUF *fp,int type);
int new_note(num n);
int save_to_db(notes_struct *note);
int save_tag(sqlite3 *db,int notes_id,char *tag);
// Parse note and create db fields
int parse_note(FILEBUF *fp);
int save_note();
int open_notes_dir(num n);
//int search_notes_dir(num n);	/* TBD  */
int search_notes_tagged(num n);	/* TBD  */
int view_notes_by_date(num n);	/* TBD  */
// Initialize notes database 
int init_notes_db(num n);
int recreate_notes_db(num n);

int show_tag_view(num n);
int create_note_with_current_file(num n);

int toggle_tagnote(num n);
int toggle_tag(num n);
int unselect_tag(num n);
int remove_tags(num n);
int select_tag(num n);

/* edit, view note */
int edit_note(num n);
int view_note(num n);
int notes_menu_command(num n);
int delete_tagnote(num n);
int save_as_note(num n);
char *get_notes_key(int key_type);
void set_local_notes_key(char *key,int key_type);
int set_notes_key(num key_type);

