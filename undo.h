/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

*/
/* undo data structures */
typedef enum type_t { INSERT, DELETE/* , REPLACE */ } type_t;

typedef struct undo_Change {
	unsigned group;
	offs head_position;
	struct undo_Change *head;
	struct undo_Change *next;
	struct undo_Change *prev;
      enum type_t undo_type;
      offs pos;
      char *left;
      offs left_size;
      char *right;
      offs right_size;
} undo_Change;

typedef struct UNDOS {
	undo_Change *chain_head;
	undo_Change *chain_ptr;
	undo_Change *chain_tail;
	offs head_position;
	offs max_size;
	time_t last_change_time;
	int group_open;
	unsigned current_group;
	int locked;
	int enabled;
	int glue_changes;
	int glue_max_time;
	int min_groups;
} UNDOS;



