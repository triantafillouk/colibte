
typedef struct IBTNODE {
	struct IBTNODE *left;
	struct IBTNODE *right;
	struct IBTNODE *up;	/* for rb trees  */
	int color;	/* for rb trees  */

	int  node_index;
	int node_ival;
} IBTNODE;

typedef struct IBTREE {
	struct IBTNODE *root;
	char *tree_name;
	int	new_flag;
	long items;
} IBTREE;

#define	RB_BLACK	0
#define	RB_RED		1


IBTNODE *find_ibtnode(IBTREE *bt,int index);
void irebalance(IBTREE *tree, IBTNODE *node);

IBTNODE *isibling(IBTNODE *node)
{
#if	0
	if(node!=NULL) MESG("		isibling node ok");
	else 	 MESG("		isibling node not ok!");
	
	if(node->up !=NULL ) MESG("		up ok!");
	else 
	if(node->up->left)	MESG("		up left ok!");
	if(node->up->right)	MESG("		up right ok!");
#endif
	IBTNODE *sibling = (node == node->up->left) ? node->up->right : node->up->left;
	return sibling;
}

int is_right_ichild(IBTNODE *node)
{
	return (node->up->right == node);
}

int is_left_ichild(IBTNODE *node)
{
	return (node->up->left == node);
}

void left_irotate(IBTREE *tree, IBTNODE *p)
{
	IBTNODE *q = p->right;
	IBTNODE **sup;
	// MESG("		left rotate");	
	if (p->up)
		sup = is_left_ichild(p) ? &(p->up->left) : &(p->up->right);
	else
		sup = &tree->root;

	p->right = q->left;
	if (p->right) p->right->up = p;
	q->left = p;
	q->up = p->up;
	p->up = q;
	*sup = q;
}

void right_irotate(IBTREE *tree, IBTNODE *p)
{
	IBTNODE *q = p->left;
	IBTNODE **sup;
	// MESG("		right rotate");	
	if (p->up)
		sup = is_left_ichild(p) ? &(p->up->left) : &(p->up->right);
	else
		sup = &tree->root;

	p->left = q->right;
	if (p->left) p->left->up = p;
	q->right = p;
	q->up = p->up;
	p->up = q;
	*sup = q;
}


/*
 * newly entered node is RED; check balance recursively as required 
 */
void irebalance(IBTREE *tree, IBTNODE *node)
{
	IBTNODE *up = node->up;
	if (up == NULL || up->color == RB_BLACK) return;
	// MESG("		irebalance");
	IBTNODE *sibling_up = isibling(up);
	if (sibling_up && sibling_up->color == RB_RED)
	{
		up->color = RB_BLACK;
		sibling_up->color = RB_BLACK;
		if (up->up->up)
		{
			up->up->color = RB_RED;
			irebalance(tree, up->up);
		}
	}
	else
	{
		if (is_left_ichild(node) && is_right_ichild(up))
		{
			right_irotate(tree, up);
			node = node->right;
		}
		else if (is_right_ichild(node) && is_left_ichild(up))
		{
			left_irotate(tree, up);
			node = node->left;
		}

		node->up->color = RB_BLACK;
		node->up->up->color = RB_RED;

		if (is_left_ichild(node)) // && is_left_child(node->up)
			right_irotate(tree, node->up->up);
		else 
			left_irotate(tree, node->up->up);
	}
}

IBTREE *new_ibtree(char *name)
{
 IBTREE *bt;
 bt=(IBTREE *)malloc(sizeof(struct IBTREE));
 bt->root=NULL;
 bt->tree_name=strdup(name);
 bt->items=0;
 // MESG("		new_ibtree: ok!");
 return(bt);
}

IBTNODE *new_ibtnode()
{
 IBTNODE *btn;
 btn=(IBTNODE *)malloc(sizeof(struct IBTNODE));

 btn->left=NULL;
 btn->right=NULL;
 btn->up=NULL;
 btn->color=0;
 btn->node_index=0;
 btn->node_ival=0;
 // MESG("		created: new node");
 return(btn);
}

IBTNODE *add_ibtnode(IBTREE *bt,int index)
{
 IBTNODE *btn;	/* new node  */
 int depth=0;
	// MESG("		add %X",index);
 	if(bt->root==NULL) {
		btn=new_ibtnode();
		// MESG("		new root created");
		btn->node_index=index;
		bt->root=btn;
		bt->root->up=NULL;	/* RB  */
		bt->new_flag=1;
		return(btn);
	} else btn=bt->root;

	while(btn!=NULL) {
		if(index==btn->node_index) { 
			return(btn);
		};
		if(index>btn->node_index) {
			if(btn->right==NULL) {
				btn->right = new_ibtnode();
				// MESG("		new right");
				btn->right->up=btn;	/* RB  */
				btn->right->color=1;
				btn=btn->right;
				btn->node_index=index;
				bt->new_flag=1;
				irebalance(bt,btn);
				return(btn);
			} else	btn=btn->right;
		} else {
			if(btn->left==NULL) {
				btn->left = new_ibtnode();
				// MESG("		new left");
				btn->left->up=btn;	/* RB  */
				btn->left->color=1;
				btn=btn->left;
				btn->node_index=index;
				bt->new_flag=1;
				irebalance(bt,btn);
				return(btn);
			} else btn=btn->left;
		};
		depth++;
	};
	/* should not pass from here!! */
	return(NULL);
}



/* insert a new node in btree table and initialize */
IBTNODE * insert_bt_ielement(IBTREE *bt,int index,int ival)
{
 IBTNODE *node;
	node = add_ibtnode(bt,index);
	// MESG("		new ielement: %X %d",index,ival);
	if(bt->new_flag) {
		node->node_index=index;
		node->node_ival=ival;
		bt->new_flag=0;
//		MESG("insert:b index=%d: type=%d [%s]",index,type,value);
	};
	// MESG("	inserted: index=%d ival=%d",index,ival);
	return(node);
}

int btnival(IBTREE *bt, int ind)
{
 IBTNODE *btn;
 btn = find_ibtnode(bt,ind);
 if(btn) {
 	return(btn->node_ival);
 } else {
 	return(-1);
 };
}

IBTNODE *find_ibtnode(IBTREE *bt,int index)
{
 IBTNODE *btn;
	btn=bt->root;

	while(btn!=NULL) {
		if(index==btn->node_index) { 
			return(btn);
		};
		if(index>btn->node_index) {
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

