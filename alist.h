/*
	Link list (double linked lists) support functions
	Copyright K.Triantafillou
	GPL2 lib license
*/
#if	0
 ;aslkd ;alskd;alsk d;laskd;laskd;lask ;lsakd ;alskd ;aslkd askld'asadsadad'
 if int else struct
#endif
/* version  10/8/2022 */

#ifndef __ALIST_KT__
#define	__ALIST_KT__
/* list structure definitions */


#define _BTREE	1
#define	RB_BALANCE	0
#define	AVL_BALANCE	1
#define A_DEBUG	0
#define	_ASTACK	0
#define	FULL		0
#define LIST_HAS_NAME	0

typedef struct _el
{
 struct _el *next;
 struct _el *prev;
 void *data;
} _el;

#if	_ASTACK
typedef struct astack {
 void *head;
 struct astack *next;
} astack;

typedef struct sstack {
 void *data;
 struct sstack *previous;
} sstack;
#endif

typedef struct alist
{
 short id;	// list id
 short array_valid;
 int size;
#if	LIST_HAS_NAME
 char *name;
#endif
 struct _el *head;
 struct _el *last;
 struct _el *current;
 void **data;
#if	_ASTACK
 astack *cstack;	// current stack
#endif
} alist;


/* function declarations */
int scmp(const char *s1,const char *s2);
alist *new_list(int id,char *title); // create a list
#if	_ASTACK
astack *new_astack(void *dat);	// create a stack
sstack *new_stack();
sstack *pop_stack(sstack *s,void **dat);
sstack * push_stack(sstack *stack,void *data);
#endif

void push_current(alist *li);
void pop_current(alist *li);
_el * add_element_to_list(void *p, alist *pl); // add an element at the end
void lbegin(alist *l); // set current to the start of list
void lend(alist *l); /* set current to end of the list */
void *lget(alist *l); // get current and move to next
void *lget_current(alist *l);
void *lget_next(alist *l);
void *lget_next_nmv(alist *l);
void *lget_previous(alist *l);
void *lget_last(alist *l); // get last data
void *lget_last_no_move(alist *l); // get last data
void *lpop(alist *l); // get last and remove it
#define lpush(p,l) add_element_to_list(p,l) 
void lset_current(alist *l,void *d); // find and set current data
void set_current(alist *l, _el *el); /* set current element no check */
#if	NUSE
void find_current(alist *l, _el *el);// set current element
#endif
void *lget_circ(alist *l);
void *lget_prev_circ(alist *l); 
void lmove_to_previous(alist *l,int circ); // move current pointer to previous
void lmove_to_next(alist *l,int circ); // move current pointer to next
void lmove_to_nextnc(alist *l); /* move current pointer to next, no circular */
void *lget_num(alist *pl,int n); // get the n-th element
int  remove_from_list(void *p, alist *pl); // find and remove
int remove_current_from_list(alist *pl);
_el *find_el(void *p, alist *pl); // find an element that matches data
_el * insert_beetween(alist *l,void *d,void *d1,void *d2); // find and insert beetween 2 others
_el * linsert_after(alist *l,void *d);// insert after current element
_el * linsert(alist *list,void *d);// insert
_el *get_current(alist *l); /* get current element */
void lfind_data(void *obj, alist *pl);
void empty_list(alist *l);	// empty list
int move_up(void *p,alist *l);
int move_down(void *p,alist *l);
void free_list(alist *l,char *);
void remove_elements(alist *list);
int list_is_empty(alist *pl);
#if	NUSE
int  delete_current(alist *list);
#endif
#if	FULL
void empty_list1(alist *list,int (*ff)(void *));
#endif
void set_last(alist *);
unsigned int list_size(alist *);
unsigned int on_last(alist *);
unsigned int on_begin(alist *l);
void **array_data(alist *list);
alist *alist_from_slist(char **slist,int n);
int str_in_list(alist *list,char *str,int start);
void show_string_list(alist *list,char *title);

#if	_BTREE

typedef struct BTNODE {
	struct BTNODE *left;
	struct BTNODE *right;
#if	RB_BALANCE
	struct BTNODE *up;	/* for rb trees  */
	int color;	/* for rb trees  */
#endif
	short node_type;
	short node_index;
	short node_vtype;
#if	AVL_BALANCE
	short balance;	/* for avl trees  */
#endif
	char *node_name;
	double node_val;
	char *sval;
} BTNODE;

typedef struct BTREE {
	struct BTNODE *root;
	char *tree_name;
	int	new_flag;
	// int level;
	long items;
	long max_items;
} BTREE;

// Initialize a btree table
void init_btree_table();

/* insert a new node in main table (bt_table) and initialize */
BTNODE *insert_bt_element(BTREE *table,char *name,int type,int index);

// add a new node and rebalance the tree
BTNODE *add_btnode(BTREE *bt,char *name);
#if	0
// get a double value of name from bt_table
double bt_dval(char *name);
// get a string value of name from bt_table
char * bt_sval(char *name);
void set_btval(char *name,int type,char *sval,double val);
#endif

BTNODE *set_btdval(BTREE *bt, char *name,double value);

int set_btsval(BTREE *bt, int type,char *name,char * sval,double val);
void set_bt_num_val(char *name,double val);
void init_btvars();
int set_btnsval(BTNODE *btn, char * sval);
int set_btndval(BTNODE *btn, double val);
/* get a double value from a node named name  */
double btndval(BTREE *bt, char *name);
/* get a string value from a node named name  */
char * btnsval(BTREE *bt, char *name);

BTREE *new_btree(char *name,int level);
void free_btree(BTREE *);
int delete_btnn(BTREE *bt, char *name);
void free_btnode(BTNODE *btn);
BTNODE *new_btnode();
BTNODE *find_btnode(BTREE *t,char *name);
void btn_free(BTNODE *btn);
#if	NUSE
void delete_avl_node(BTNODE *node);
#endif
#endif

#endif
