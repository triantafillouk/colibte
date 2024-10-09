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
						// "~/.xe/Notes"
#define NOTES_DIR		NOTES_PARENT"/Notes"
#define NOTES_DBFILE	NOTES_PARENT"/notes.db"
#define NOTES_DIR0		"Notes"

#define	TAG_UNSELECT_ALL		0
#define TAG_SELECT_CURRENT		1
#define	TAG_SELECT_TOGGLE		2
#define	TAG_UNSELECT_CURRENT	3

// open an sqlite3 database
sqlite3 * notes_db_open();
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
int insert_preamble();
int new_note(int n);
#if	TNOTES2
int save_to_db(sqlite3 *db,notes_struct *note);
#else
int save_to_db(notes_struct *note);
#endif
int save_tag(sqlite3 *db,int notes_id,char *tag);
// Parse note and create db fields
int parse_note(FILEBUF *fp);
int save_note();
int open_notes_dir(int n);
//int search_notes_dir(int n);	/* TBD  */
int search_notes_tagged(int n);	/* TBD  */
int view_notes_by_date(int n);	/* TBD  */
// Initialize notes database 
int init_notes_db(int n);
int recreate_notes_db(int n);

int show_tag_view(int n);
int create_note_with_current_file(int n);

int toggle_tagnote(int n);
int toggle_tag(int n);
int unselect_tag(int n);
int remove_tags(int n);
int select_tag(int n);

/* edit, view note */
int edit_note(int n);
int view_note(int n);
int notes_menu_command(int n);
int delete_tagnote(int n);
int save_as_note(int n);
char *get_notes_key();
void set_local_notes_key(char *key);
int set_notes_key(int n);

