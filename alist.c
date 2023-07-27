/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

	Link list (double linked lists) support functions
	Copywrite Kostas Triantafillou 2005
*/
/* version 10/8/2022 */

#define	FULL 0
#include "alist.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if	defined _MESG_DEFINED
void MESG(const char *fmt, ...);
#else
#define MESG(format,...) { \
    {\
	fprintf(stderr, format, ##__VA_ARGS__);\
    fputc('\n', stderr);\
	}\
}
#endif

/* simple string compare */
int scmp(const char *s1,const char *s2)
{
	while(*s1 && *s1 == *s2){
		s1++; s2++;
	}
	return (*s1 - *s2);
}

#if	_ASTACK
astack *new_astack(void *dat)
{
 astack *as;
 as = (astack *)malloc(sizeof(astack));
 as->next=NULL;
 as->head=dat;
 return as;
}
#endif

alist *new_list(int id1,char *title)
{
 alist *li;
 static int id=0;
 li = (alist *)malloc(sizeof(alist));
 li->head = NULL;
 li->last = NULL;
 li->current = NULL;
 if(id1!=0) li->id = id1;
 else { 
 	id++;
	li->id = id;
 };
 li->size=0;
 li->data=NULL;
 li->array_valid=0;
#if	_ASTACK
 li->cstack = new_astack(li->current);
#endif
 return (li);
}

_el *new_element()
{
 void *dat;
 dat=malloc(sizeof(_el));
 return ((_el *)dat);
}

_el * add_element_to_list(void *p, alist *pl)
{
 _el *new_pel;

 /* create the new element */
 new_pel = (struct _el *)malloc(sizeof(struct _el));

 if(new_pel==NULL) return NULL;
 new_pel->data = (void *)p;
 if(pl->head==NULL) {
 	pl->head=new_pel;
	pl->last=new_pel;
	new_pel->prev=NULL;
 } else {
	new_pel->prev=pl->last;
	pl->last->next=new_pel;
 	pl->last=new_pel;
 };
 new_pel->next=NULL;
 if(pl->current==NULL) pl->current=new_pel;

 pl->size++;
 pl->array_valid=0;
 return new_pel;
}

#if	_ASTACK
/* put current list element to list's stack */
void push_current(alist *li)
{
 astack *cs;
 cs = new_astack(li->current);
 cs->next = li->cstack;
 li->cstack = cs;
}

sstack * push_stack(sstack *stack,void *data)
{
	sstack *new;
	// printf("push_stack:\n");
	if(stack->data==NULL)  new=stack;
	else {
		new=(sstack *)malloc(sizeof(sstack));
		new->previous=stack;
	};
	new->data=data;
	return new;
}

sstack *pop_stack(sstack *s,void **dat)
{
	*dat = s->data;
	sstack *prev;
	// printf("pop_stack: %s\n",(char *)*dat);
	if(s->previous!=NULL) {
		// printf("prev not null!\n");
		prev=s->previous;
		free(s);
	} else {	// this is the last!
		// printf("prev null!\n");
		s->data=NULL;
		prev=s;
	};
	return prev;
}

sstack *new_stack()
{
	sstack *stack = malloc(sizeof(struct sstack));
	// printf("new_stack:\n");
	stack->data=NULL;
	stack->previous=NULL;
	return stack;
}

/* pop an element from list's stack and set it as current element */
void pop_current(alist *li)
{
 astack *old;
 old = li->cstack;
 if(old!=NULL) {
	li->current = (_el *)li->cstack->head;
	li->cstack = li->cstack->next;
	free(old);
 };
}
#endif

/* insert at current position (ie before current element!) */
_el * linsert(alist *list,void *d)
{
 _el *e1,*ei;
 e1 = list->current;
 if(e1==NULL) return(add_element_to_list(d,list));
 ei = (_el *)malloc(sizeof(struct _el));
 ei->data=d;

 if(e1->prev==NULL)	/* this is the head!  */
 {
	ei->prev=NULL;	/* still the head!  */
	ei->next = list->current;
	list->current->prev=ei;
 	list->head=ei;
	list->size++;
 } else {
	ei->next = list->current;
	ei->prev = list->current->prev;
	list->current->prev=ei;
	list->size++;
 }
 return ei;
}

/* insert after current element */
_el * linsert_after(alist *list,void *d)
{
 _el *e1,*e2,*ei;
 e1 = list->current;
 if(e1==list->last) {
	return(add_element_to_list(d,list));
 };
 e2 = e1->next;
 ei = (_el *)malloc(sizeof(struct _el));
 e1->next = ei;
 e2->prev = ei;
 ei->prev=e1;
 ei->next=e2;
 ei->data = d;
 list->size ++;
 return(ei);
}


_el * insert_beetween(alist *list,void *d,void *d1,void *d2)
{
 _el *e1,*e2,*ei;
 e1 = find_el(d1,list);
 if(!e1) return(0);
 e2 = e1->next;
 if(e2->data!=d2) {
 
 	e1=find_el(d2,list);
	if(!e1) return(0);
	e2=e1->next;
	if(e2->data!=d1) {
// 	printf("we have a mismatch sequence !!!!!!!!!!!!!\n");
 	return(NULL);	
	};
 };
 ei = (_el *)malloc(sizeof(struct _el));
 if(ei==NULL) return NULL;
 e1->next = ei;
 e2->prev = ei;
 ei->prev=e1;
 ei->next=e2;
 ei->data = d;
 list->size++;
 list->array_valid=0;
 return(ei);
}

/* move current to head of the list */
void lbegin(alist *l)
{
 l->current = l->head;
}

/* move current to end of the list */
void lend(alist *l)
{
 l->current = l->last;
}

// get current, move to next
void *lget(alist *l)
{
 void *d;
 if(l->current == NULL) return NULL;
 d=l->current->data;
 l->current=l->current->next;
 return(d);
} 

#if	NUSE
// get last no move
void *lget_last_nomove(alist *l)
{
 void *d;
 if(l->last == NULL) return NULL;
 d=l->last->data;
 return(d);
}
#endif

// get last, and remove it from list
void *lpop(alist *l)
{
 void *d;
 _el *p;
 if(l->last == NULL) return NULL;
 d=l->last->data;
 p=l->last;
// printf("lpop: remove last from list!\n");
 // remove it from list
 if(l->last==l->head) {
 	l->last=l->head=NULL;
	l->current=NULL;
//	if(l->size!=1) MESG("lpop: error this is not the last!");
 } else {
	_el *prev;
	prev=p->prev;
	prev->next=NULL;

//	if the current element is the last them we must change it!
//	otherwise dont change it
	if(l->current==p) 
		l->current=prev;
	l->last=prev;
	if(p!=NULL) free(p);
 };
 l->size--;
 l->array_valid=0;
// if(l->size < 0) MESG("lpop: error zero size!");
 return(d);
}

// get current, no move
void *lget_current(alist *l)
{
 if(l==NULL) { 	/* List is empty!  */
	return NULL;
 };
 if(l->current == NULL) {
	return NULL;
 };
 return(l->current->data);
}

// get next, no move
void *lget_next_nmv(alist *l)
{
 if(l->current == NULL) return NULL;
 if(l->current->next == NULL) return NULL;
 return(l->current->next->data);
}

void set_current(alist *l, _el *el)
{
 l->current = el;
}

_el *get_current(alist *l)
{
 return(l->current);
}

#if	NUSE
void find_current(alist *l, _el *el)
{
 _el *pel;
 for(pel=l->head;pel!=l->last;pel=pel->next) {
	if(pel==el) break;
 };
 l->current = el;
}
#endif

/* set data of current */
void lset_current(alist *l,void *data)
{
 if(l->current==NULL) { /* this is an empty list */
 	add_element_to_list(data,l);
 } else;
 l->current->data = data;
}

// move to next
void lmove_to_next(alist *l,int circ)
{
 if(l->current == l->last && circ) { 
 		l->current = l->head;
 } else {
	if(l->current!=NULL) l->current = l->current->next;
 }
}

// move to next not circular
void lmove_to_nextnc(alist *l)
{
	if(l->current!=NULL) l->current = l->current->next;
}


// move to previous 
void lmove_to_previous(alist *l,int circ)
{
 if(l->current == NULL) { // this list is empty
	return;
 }
 if(l->current == l->head) {
 	if(circ) l->current = l->last;
 } else {
 	l->current = l->current->prev;
 };
}

/* get next from current and move to this */
/* return always data except in an empty list */
/* at last position returns always the same */
void *lget_next(alist *l)
{
 if(l->current!=NULL) {
 	if(l->current->next!=NULL)	l->current=l->current->next;
 } else return NULL;
 return l->current->data;
}

/* get previous from current and move to this */
/* return always data except in an empty list */
/* at first position returns always the same */
void *lget_previous(alist *l)
{
 if(l->current!=NULL) {
	if(l->current->prev!=NULL)	l->current=l->current->prev;
 } else return NULL;
 return l->current->data;
}

// get current, move to next circularly
void *lget_circ(alist *l)
{
 void *d;
 if(l->current == NULL) {
 	return NULL;
 };
 d=l->current->data;
// in circular lists
 if(l->current == l->last) 
 	l->current=l->head;
 else
	l->current=l->current->next;
 return(d);
} 

// get current, move to previous circularly
void *lget_prev_circ(alist *l)
{
 void *d;
 if(l->current == NULL) return NULL;
 d=l->current->data;
// in circular lists
 if(l->current == l->head) 
	l->current=l->last;
 else
	l->current=l->current->prev;
 return(d);
} 

/* return the n-th element of a list */
void *lget_num(alist *pl,int n)
{
 int i;
 _el *pel;
 void *p;
 p=NULL;
// check !!!!!!!!!!!!!!!!
 for(i=0,pel=pl->head;i<n+1;i++,pel=pel->next) {
	if(pel==NULL) break;
	if(i==n) p=pel->data;
 };
 if(pel!=NULL) pl->current=pel;
 else pl->current=NULL;
 return (p);
}

/* find the element of a object in a list */
_el *find_el(void *obj, alist *pl)
{
 _el *pi;
 for(pi=pl->head;pi!=NULL;pi=pi->next) {
 	if(pi->data == obj) return pi;
 };
 return NULL;
}

/* find the the data and set current element */
void lfind_data(void *obj, alist *pl)
{
 pl->current = find_el(obj,pl);
}

/* remove an object from a list */
/* just deletes the element from that list not the data! */
/* returns 1 if deleted,0 if not found */
int  remove_from_list(void *p, alist *list)
{
 _el *pi,*prev;

 pi = find_el(p,list);
 if(pi) {
 	if(list->head == pi) {	// this is the head to remove
		if(list->head == list->last) { // this is the only element!
			list->head=NULL;
			list->last=NULL;
			list->current=NULL;
		} else {
		list->head = list->head->next;
		list->head->prev=NULL;
		if(list->last->data == p) { // this is a closed list
			list->last->data = list->head->data;	// close this properly too
		};
//		if(pi == list->last) list->last=NULL;
		};
	} else 
	if(list->last == pi) {	// this hapens only in not closed list!
		prev = pi->prev;
		prev->next = NULL;
		list->last=pi->prev;
	} else {
		pi->prev->next = pi->next;
		pi->next->prev = pi->prev;
	};
	if(pi!=NULL) free(pi);
	list->size--;
	list->array_valid=0;
	return 1;
 };
 return 0;
}

void show_string_list(alist *list,char *title)
{
 _el *current = list->current;
 
 list->current=list->head;
 MESG("== %s ==",title);
 while(list->current){
	MESG("- [%s]",(char *)list->current->data); 	
	list->current = list->current->next;
 };
 list->current=current;
}

#if	NUSE
/* delete current element, next element becomes current */
/* frees also the data */
/* use remove_current_from_list instead! */
int  delete_current(alist *list)
{
 _el *pi,*prev;
// MESG("delete_current: id=%d [%s]",list->id,(char *)(list->current->data));
 pi = list->current;
 if(pi==NULL) return 0;
 	 if(list->head == pi) {	// this is the head to remove
		if(list->head == list->last) { // this is the only element!
			list->head=NULL;
			list->last=NULL;
			list->current=NULL;
		} else {
			list->head = list->head->next;
			list->head->prev=NULL;
			if(list->last->next == pi) { // this is a closed list
				list->last->next = list->head;	// close this properly too
			};
		};
	 } else {
		if(list->last == pi) {	// this is the last
			prev = pi->prev;
			prev->next = NULL;
			list->last=prev;
			list->current=prev;
		} else {
			pi->prev->next = pi->next;
			pi->next->prev = pi->prev;
			list->current=pi->next;
		};
	}
	list->size--;
	list->array_valid=0;
	if(pi!=NULL) free(pi);
	return 1;
return 0;
}
#endif

#if	FULL
// move element data to previous in list
int move_up(void *data,alist *pl)
{
 _el *p1;
 p1 = find_el(data,pl);
 if(p1==pl->head) return (0);
 p1->data = p1->prev->data;
 p1->prev->data=data;
 return(1);
}

// move element data to next in list
int move_down(void *data,alist *pl)
{
 _el *p1;
 p1 = find_el(data,pl);
 if(p1==pl->last) return (0);
 p1->data = p1->next->data;
 p1->next->data=data;
 return(1);
}
#endif

// remove current element from list, returns true if list non empty
int  remove_current_from_list(alist *list)
{
 _el *pi,*prev;

 pi = list->current;
 if(pi==NULL) return 0;	// empty list
 if(list->head == pi) {	// this is the head to remove
		if(list->head == list->last) { // this is the only element!
			list->head=NULL;
			list->last=NULL;
			list->current = NULL;
		} else {
		list->head = list->head->next;
		list->head->prev=NULL;
		list->current=list->head;
		};
	} else 
	if(list->last == pi) {	// this hapens only in not closed list!
		prev = pi->prev;
		prev->next = NULL;
		list->last=pi->prev;
		list->current=list->last;
//		MESG("list current to last");
	} else {
		list->current = pi->next;
		pi->prev->next = pi->next;
		pi->next->prev = pi->prev;
//		MESG("list %d current to next %lX",list->id,(long)list->current);
	};
	if(pi!=NULL) free(pi);
	list->size--;
	list->array_valid=0;
	if(list->current!=NULL) return 1; 
	else return 0;
}


/* free element and data from a list */
void empty_list(alist *list)
{
 _el *pel;
 _el *peln;
 if(list==NULL) return;
 for(pel=list->head;pel!=NULL;pel=peln) {
 	peln=pel->next;
	if(pel->data!=NULL) free(pel->data);
	free(pel);
 };
 list->head=NULL;
 list->last=NULL;
 list->current=NULL;
 list->size=0;
 list->array_valid=0;
}

#if	FULL
/* use a function to free the element data */
void empty_list1(alist *list,int (*ff)(void *))
{
 _el *pel;
 _el *peln;
 if(list==NULL) return;
 for(pel=list->head;pel!=NULL;pel=peln) {
 	peln=pel->next;
	if(pel->data!=NULL) ff(pel->data);
	free(pel);
 };
 list->head=NULL;
 list->last=NULL;
 list->current=NULL;
 list->size=0;
 list->array_valid=0;
// printf("empty_list1:id=%d\n",list->id);
}
#endif

/* remove elements from a list */
/* no element data is touched! */
void remove_elements(alist *list)
{
 _el *pel;
 _el *peln;
 if(list==NULL) return;
 for(pel=list->head;pel!=NULL;pel=peln) {
 	peln=pel->next;
//	if(pel->data!=NULL) free(pel->data);
	free(pel);
 };
 list->head=NULL;
 list->last=NULL;
 list->current=NULL;
 list->size=0;
 list->array_valid=0;
}

void free_list(alist *list,char *title)
{
 _el *pel;
 _el *peln;
 if(list==NULL) return;
 for(pel=list->head;pel!=NULL;pel=peln) {
 	peln=pel->next;
	free(pel);
 };
#if	_ASTACK
 if(list->cstack != NULL) free(list->cstack);
#endif
 free(list);
}

#if	A_DEBUG
/* show debuging info of a list */
void show_alist(alist *pl)
{
 _el *pel;
 for(pel=pl->head;pel!=NULL;pel=pel->next) {
  MESG(": %ld p=%ld n=%ld",(long)pel,(long)pel->prev,(long)pel->next);
 };
 if(pl->last!=NULL) MESG("last: %ld p=%ld n=%ld",(long)pl->last,(long)pl->last->prev,(long)pl->last->next);
}
#endif

int list_is_empty(alist *pl)
{
 if(pl->head==NULL) return 1;
 else return 0;
}

/* go to the end of the list */
void set_last(alist *l)
{
 l->current=l->last;
}

/* check if at the beginning of the list */
unsigned int on_begin(alist *l)
{
 return(l->current == l->head);
}

/* check if at the end of the list */
unsigned int on_last(alist *l)
{
 return(l->current == l->last);
}


void *lget_last(alist *l)
{
// MESG("lget_last:");
 if(l->last==NULL) return NULL;
 return(l->last->data);
}

unsigned int list_size(alist *l)
{
	return(l->size);
}

/* create/return an array with list's data */
void **array_data(alist *list)
{
 int i;
 _el *pel;

 if(!list->array_valid) {
 	if(list->data!=NULL) free(list->data);
	list->data = (void **) malloc(sizeof(void*)*(list->size+1));
	 i=0;
	 for(pel=list->head;pel!=NULL;pel=pel->next) {
//		MESG("%d: %s",i,(char *)(pel->data));
		list->data[i++] = pel->data;
 	};
	list->data[i]=NULL;
	list->array_valid=1;

 };
 return(list->data);
}

/* create a list from a string list */
alist *alist_from_slist(char **slist,int n)
{
 alist *al;
 int i;
 al = new_list(0,"alist_from_slist");

 for(i=0;i<n;i++) {
	 add_element_to_list((void *)slist[i],al);
 };
 free(slist);
 return al;
}

int str_in_list(alist *list,char *str,int start)
{
	char *st;
	// MESG("str_in_list: %s start=%d",str,start);
	if(list) {
	lbegin(list);
	while((st=(char *)lget_current(list))!=NULL){
		// MESG("	check [%s]",st);
		if(strlen(st)>start){
			if(!strcmp(st+start,str)) return 1; 
		}
		lmove_to_next(list,0);
	}}
	return 0;
}

#if	_BTREE

#include "rb_tree.c"
#include "avl_tree.c"

BTREE *new_btree(char *name,int level);
BTNODE *add_btnode(BTREE *bt,char *name);
int delete_btnode_named(BTREE *bt,char *name);
BTNODE *find_btnode(BTREE *bt,char *name);
void show_subtree(BTNODE *node);
void show_ordered_subtree(BTNODE *node);

// must be moved to eval.c

BTREE *bt_function=NULL;
#if	NUSE
BTREE *bt_common=NULL;
#endif
long common_index=0;


BTREE *new_btree(char *name,int level)
{
 BTREE *bt;
 bt=(BTREE *)malloc(sizeof(struct BTREE));
 bt->root=NULL;
 bt->tree_name=strdup(name);
 bt->level=level;
 bt->items=0;
 return(bt);
}

BTNODE *new_btnode()
{
 BTNODE *btn;
 btn=(BTNODE *)malloc(sizeof(struct BTNODE));
 // MESG("new_btnode: size is %ld %ld %ld %ld %ld",sizeof(struct BTNODE),sizeof(short),sizeof(int),sizeof(double),sizeof(void *));
 btn->left=NULL;
 btn->right=NULL;
 btn->node_name=NULL;
 btn->node_type=0;
 btn->node_index=0;
 btn->val=0;
 btn->sval=NULL;
#if	RB_BALANCE
 btn->up=NULL;
#endif
#if	AVL_BALANCE
 btn->balance=0;
#endif
 return(btn);
}

void btn_free(BTNODE *btn)
{
	if(btn){
		if(btn->node_name) free(btn->node_name);
		free(btn);
	};
}

/* insert a new node in btree table and initialize */
BTNODE * insert_bt_element(BTREE *bt,char *name,int type,int index)
{
 BTNODE *node;
//	MESG("insert_bt_element: name=[%s] type=%d index=%d",name,type,index); 
	node = add_btnode(bt,name);
	if(bt->new_flag) {
		node->node_type=type;
		node->node_index=index;
		node->val=0;
		node->sval=NULL;
		bt->new_flag=0;
//		MESG("insert:b index=%d: type=%d [%s]",index,type,value);
	};
//	MESG("insert_bt_element: [%s] type=%d index=%d is_new=%d",value,type,index,*is_new);
	return(node);
}

#if	!AVL_BALANCE
/*
	Check if a name exists in a btree table  and if
	not then insert it and return the node.
*/
BTNODE *add_btnode(BTREE *bt,char *name)
{
 BTNODE *btn;	/* new node  */
 int res;
 int depth=0;

//	MESG("add_btnode: [%s]",name);
 	if(bt->root==NULL) {
		btn=new_btnode();
		btn->node_name=strdup(name);
		bt->root=btn;
#if	RB_BALANCE
		bt->root->up=NULL;	/* RB  */
#endif
		bt->new_flag=1;
//		MESG("	add_btnode: on root");
		return(btn);
	} else btn=bt->root;

	while(btn!=NULL) {
		res=scmp(name,btn->node_name);
		if(res==0) { 
//			MESG(" add_btnode: found [%s] depth=%d",name,depth); 
			return(btn);
		};
		if(res>0) {
			if(btn->right==NULL) {
				btn->right = new_btnode();
#if	RB_BALANCE
				btn->right->up=btn;	/* RB  */
				btn->right->color=1;
#endif
				btn=btn->right;
				btn->node_name=strdup(name);
				bt->new_flag=1;
#if	RB_BALANCE
				rebalance(bt,btn);
#endif
//				if(btn->up!=NULL) {MESG(" add_btnode:R new [%s] under [%s] depth=%d color=%d",name,btn->up->node_name,depth,btn->color) }
//				else { MESG(" add_btnode: new [%s] under root depth=%d",name,depth) }
				return(btn);
			} else	btn=btn->right;
		} else {

			if(btn->left==NULL) {
				btn->left = new_btnode();
#if	RB_BALANCE
				btn->left->up=btn;	/* RB  */
				btn->left->color=1;
#endif
				btn=btn->left;
				btn->node_name=strdup(name);
				bt->new_flag=1;
#if	RB_BALANCE
				rebalance(bt,btn);
#endif
				return(btn);
			} else btn=btn->left;
		};
		depth++;
	};
	/* should not pass from here!! */
	return(NULL);
}
#endif

/* get a double value from a node named name  */
double btndval(BTREE *bt, char *name)
{
 BTNODE *btn;
// MESG("check directive: %s",name);
 btn = find_btnode(bt,name);
 if(btn) {
 	return(btn->val);
 } else {
 	return(0);
 };
}

/* get a string value from a node named name  */
char * btnsval(BTREE *bt, char *name)
{
 BTNODE *btn;
 btn = find_btnode(bt,name);
 if(btn) {
 	return(btn->sval);
 } else {
 	return(NULL);
 };
}

/* set double value for a node named name of main table  */
BTNODE *set_btdval(BTREE *bt, char *name,double value)
{
 BTNODE *btn;
 btn = find_btnode(bt,name);
 if(btn) {
	btn->val=value;
	btn->sval=NULL;
 } else {
	btn=add_btnode(bt,name);
	btn->val=value;
	btn->sval=NULL;
 };
 return(btn);
}

/* set type,double and string values for a node named name */
int set_btsval(BTREE *bt, int type,char *name,char * sval,double val)
{
 BTNODE *btn;
 int is_new=0;
 btn = find_btnode(bt,name);
 if(btn==NULL) {
 	btn=add_btnode(bt,name);
	is_new=1;
	btn->node_type=type;
	btn->sval=NULL;
 };

 if(btn->sval!=NULL) { free(btn->sval);btn->sval=NULL;};
 if(sval!=NULL){
	btn->sval=strdup(sval);
 };
 btn->val=val;
 return(is_new);
}

/* set the string value for a node */
int set_btnsval(BTNODE *btn, char * sval)
{
 if(btn==NULL) return(0);

 if(btn->sval!=NULL) { free(btn->sval);btn->sval=NULL;};
 if(sval!=NULL){
	btn->sval=strdup(sval);
 };
// MESG("set_btsval: name[%s] [%s] index=%d type=%d sval=[%s] val=%f is_new=%d",name,btn->node_name,btn->node_index,btn->node_type,btn->sval,btn->val,is_new); 
 return(1);
}

int set_btndval(BTNODE *btn,double val)
{
 if(btn==NULL) return(0);
 btn->val=val;
 return(1);
}

/* returns a node named on a table or NULL  */
BTNODE *find_btnode(BTREE *bt,char *name)
{
 BTNODE *btn;
 int res;
	btn=bt->root;

	while(btn!=NULL) {
		res=scmp(name,btn->node_name);
		if(res==0) { 
			return(btn);
		};
		if(res>0) {
			if(btn->right) {
				btn=btn->right;
			} else return(NULL);
		} else {
			if(btn->left) {
				btn=btn->left;
			} else return(NULL);
		};
	};

	return(NULL);
}

BTNODE *find_minr(BTREE *bt, BTNODE *btn)
{
 BTNODE *fbtn; // free btree node
 BTNODE *pfbtn=btn;
	btn=btn->right;

	while(btn->left)
	{
		fbtn=btn->left;
		pfbtn = btn;	
		btn=fbtn;
	};
	return(pfbtn);
}

/* Find a node and delete recursivelly  */
int delete_btnn(BTREE *bt, char *name)
{
 BTNODE *btn=NULL;
 BTNODE *pbtn=NULL;

 int left=0;
 int res;

	btn=bt->root;
	if(btn==NULL) return(0);
	
	while(btn!=NULL) {
		res=scmp(name,btn->node_name);
		if(res==0) { break;};
		if(res<0) {
			if(btn->right==NULL) {
				return(0);	// not found!
			} else	{
				pbtn=btn;
				btn=btn->right;
				left=0;
			}
		} else {
			if(btn->left==NULL) {
				return(0);	// not found!
			} else {
				pbtn=btn;
				btn=btn->left;
				left=1;
			};
		};
	};

	/* we have here node to delete and its parent  */
	free_btnode(btn);
	/* it is the root to be deleted! */
	if(pbtn==NULL) { 
		bt->root=NULL;
		bt->new_flag=0;
	} else {
		if(left) pbtn->left=NULL;
		else pbtn->right=NULL;	
	};

	return(1);
}

/* delete a single node named */
int delete_btnode_named(BTREE *bt, char *val)
{
 BTNODE *btn=NULL;
 BTNODE *pbtn=NULL;
 BTNODE *cbtn=NULL;

// int left=0;
 int res;

	btn=bt->root;
	if(btn==NULL) return(0);
	
	while(btn!=NULL) {
		res=scmp(val,btn->node_name);
		if(res==0) { break;};
		if(res<0) {
			if(btn->right==NULL) {
				return(0);	// not found!
			} else	{
				pbtn=btn;
				btn=btn->right;
			}
		} else {
			if(btn->left==NULL) {
				return(0);	// not found!
			} else {
				pbtn=btn;
				btn=btn->left;
			};
		};
	};

	/* we have here node to delete and its parent  */
	/* simple case with both leaves null! */
	if(btn->left == NULL && btn->right==0) {
		if(pbtn!=NULL) {
			if(pbtn->left==btn) pbtn->left=NULL;
			else pbtn->right=NULL;
		} else bt->root=NULL;;
		btn_free(btn); 
		return(1);
	};
	
	if(btn->left==NULL || btn->right==NULL) {
		if(btn->left) {
			if(pbtn==NULL) {
				bt->root=btn->left;
			} else {
				if(pbtn->left==btn) pbtn->left=btn->left;
				else pbtn->right=btn->left;
			};
		} else {
			if(pbtn==NULL) {
				bt->root=btn->right;
			} else {
				if(pbtn->left==btn) pbtn->left=btn->right;
				else pbtn->right=btn->right;
			};
		}
		btn_free(btn);
	} else { /* both leaves exist! */
		BTNODE * pcbtn;
		pcbtn = find_minr(bt,btn); 
		
		if(pcbtn->left) cbtn = pcbtn->left;
		else cbtn = pcbtn->right;

		/* put cbtn in place of btn!  */
		if(pbtn){
			if(pbtn->left==btn) pbtn->left=cbtn;
			else pbtn->right=cbtn;

			pcbtn->left = cbtn->right;
			cbtn->left = btn->left;
		} else { // is root
			bt->root=cbtn;
		};

		cbtn->right = btn->right;

		btn_free(btn);
	};

	return(1);
}

/* free/delete a node recursivelly  */
void free_btnode(BTNODE *btn)
{
 if(btn){
	if(btn->left!=NULL) free_btnode(btn->left);
	if(btn->right!=NULL) free_btnode(btn->right);
	btn_free(btn);
 };
}

/* free/delete a whole tree  */
void free_btree(BTREE *bt)
{
//	MESG("free_btree: %s %d",bt->node_name,bt->level);
	if(bt){
		free_btnode(bt->root);
		if(bt->tree_name!=NULL) free(bt->tree_name);
		free(bt);
	}
}

int max_depth=0;

int show_bt_table(BTREE *bt)
{
//	MESG("show_bt_table:");
	max_depth=0;
	if(bt) {
	show_subtree(bt->root);
//	show_ordered_subtree(bt->root);
#if	AVL_BALANCE
//	MESG("AVL maximum depth is %d",max_depth);
#else
#if	RB_BALANCE
//	MESG("RB maximum depth is %d",max_depth);
#else
//	MESG("NO balance maximum depth is %d",max_depth);
#endif
#endif
	};
 return max_depth;
}


void show_bt_table_ordered(BTREE *bt)
{
//	MESG("show_bt_table:");
	max_depth=0;
	show_ordered_subtree(bt->root);
	MESG("maximum depth is %d",max_depth);
}


void show_subtree(BTNODE *node)
{
 static int depth=0;
 char *left="",*right="";
 depth++;
// MESG("node:[%s] id=%d type=%d val=%f balance=%d sval=%s",node->node_name,node->node_index,node->node_type,node->val,node->balance,node->sval);
 if(node->left) left = node->left->node_name;
 if(node->right) right = node->right->node_name;
 if(node->left || node->right) {
//	fprintf(stdout,"%d:node[%s] -> [%s] - [%s]\n",depth,node->node_name,left,right);
	if(depth>max_depth) max_depth=depth;
	if(node->sval!=NULL) fprintf(stdout,"%d:node[%s] -> [%s] - [%s] index=%d type=%d val=%f sval=[%s]\n",depth,node->node_name,left,right,node->node_index,node->node_type,node->val,node->sval);
	else fprintf(stdout,"%d:node[%s] -> [%s] - [%s] index=%d type=%d val=%f\n",depth,node->node_name,left,right,node->node_index,node->node_type,node->val);
 };
 if(node->left) show_subtree(node->left);
 if(node->right) show_subtree(node->right);
 depth--;
}

void show_ordered_subtree(BTNODE *node)
{
 static int depth=0;
char *left="",*right="";
 depth++;
 // printf("- node:[%s] id=%d type=%d val=%f balance=%d sval=%s",node->node_name,node->node_index,node->node_type,node->val,node->balance,node->sval);
 if(node->left) {
 	show_ordered_subtree(node->left);
	if(node->left) left = node->left->node_name;
	if(node->right) right = node->right->node_name;
	fprintf(stdout,"%03d:node[%-20s] l=[%-20s] r=[%s]\n",depth,node->node_name,left,right);
	if(node->right) show_ordered_subtree(node->right);
 } else {
	if(node->left) left = node->left->node_name;
	if(node->right) right = node->right->node_name;
 	fprintf(stdout,"%03d:node[%-20s] l=[%-20s] r=[%s]\n",depth,node->node_name,left,right);
	if(node->right) show_ordered_subtree(node->right);
 } 

 depth--;
}

#endif

