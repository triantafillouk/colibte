/* Red black key implementation with key is an integer
  */
typedef struct RB_INODE {
	struct RB_INODE *left;
	struct RB_INODE *right;
	struct RB_INODE *up;	/* for rb trees  */
	int color;	/* for rb trees  */

	int  node_index;
	int node_ival;
} RB_INODE;

typedef struct RB_ITREE {
	struct RB_INODE *root;
	char *tree_name;
	int	new_flag;
	long items;
} RB_ITREE;

#define	RB_BLACK	0
#define	RB_RED		1


static RB_INODE *find_rb_inode(RB_ITREE *bt,int index);
static void rb_irebalance(RB_ITREE *tree, RB_INODE *node);

inline static RB_INODE *isibling(RB_INODE *node)
{
	RB_INODE *sibling = (node == node->up->left) ? node->up->right : node->up->left;
	return sibling;
}

inline static int is_right_ichild(RB_INODE *node)
{
	return (node->up->right == node);
}

inline static int is_left_ichild(RB_INODE *node)
{
	return (node->up->left == node);
}

static void left_rb_irotate(RB_ITREE *tree, RB_INODE *p)
{
	RB_INODE *q = p->right;
	RB_INODE **sup;
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

static void right_rb_irotate(RB_ITREE *tree, RB_INODE *p)
{
	RB_INODE *q = p->left;
	RB_INODE **sup;
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
static void rb_irebalance(RB_ITREE *tree, RB_INODE *node)
{
	RB_INODE *up = node->up;
	if (up == NULL || up->color == RB_BLACK) return;
	// MESG("		rb_irebalance");
	RB_INODE *sibling_up = isibling(up);
	if (sibling_up && sibling_up->color == RB_RED)
	{
		up->color = RB_BLACK;
		sibling_up->color = RB_BLACK;
		if (up->up->up)
		{
			up->up->color = RB_RED;
			rb_irebalance(tree, up->up);
		}
	}
	else
	{
		if (is_left_ichild(node) && is_right_ichild(up))
		{
			right_rb_irotate(tree, up);
			node = node->right;
		}
		else if (is_right_ichild(node) && is_left_ichild(up))
		{
			left_rb_irotate(tree, up);
			node = node->left;
		}

		node->up->color = RB_BLACK;
		node->up->up->color = RB_RED;

		if (is_left_ichild(node)) // && is_left_child(node->up)
			right_rb_irotate(tree, node->up->up);
		else 
			left_rb_irotate(tree, node->up->up);
	}
}

RB_ITREE *new_rb_itree(char *name)
{
 RB_ITREE *bt;
 bt=(RB_ITREE *)malloc(sizeof(struct RB_ITREE));
 bt->root=NULL;
 bt->tree_name=strdup(name);
 bt->items=0;
 // MESG("		new_RB_ITREE: ok!");
 return(bt);
}

static RB_INODE *new_rb_inode()
{
 RB_INODE *btn;
 btn=(RB_INODE *)malloc(sizeof(struct RB_INODE));

 btn->left=NULL;
 btn->right=NULL;
 btn->up=NULL;
 btn->color=0;
 btn->node_index=0;
 btn->node_ival=0;
 // MESG("		created: new node");
 return(btn);
}

static RB_INODE *add_RB_INODE(RB_ITREE *bt,int index)
{
 RB_INODE *btn;	/* new node  */
 int depth=0;
	// MESG("		add %X",index);
 	if(bt->root==NULL) {
		btn=new_rb_inode();
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
				btn->right = new_rb_inode();
				// MESG("		new right");
				btn->right->up=btn;	/* RB  */
				btn->right->color=1;
				btn=btn->right;
				btn->node_index=index;
				bt->new_flag=1;
				rb_irebalance(bt,btn);
				return(btn);
			} else	btn=btn->right;
		} else {
			if(btn->left==NULL) {
				btn->left = new_rb_inode();
				// MESG("		new left");
				btn->left->up=btn;	/* RB  */
				btn->left->color=1;
				btn=btn->left;
				btn->node_index=index;
				bt->new_flag=1;
				rb_irebalance(bt,btn);
				return(btn);
			} else btn=btn->left;
		};
		depth++;
	};
	/* should not pass from here!! */
	return(NULL);
}



/* insert a new node in btree table and initialize */
void set_rb_ival(RB_ITREE *bt,int index,int ival)
{
 RB_INODE *node;
	node = add_RB_INODE(bt,index);
	// MESG("		new ielement: %X %d",index,ival);
	if(bt->new_flag) {
		node->node_index=index;
		bt->new_flag=0;
//		MESG("insert:b index=%d: type=%d [%s]",index,type,value);
	};
	node->node_ival=ival;
	// MESG("	inserted: index=%d ival=%d",index,ival);
	// return(node);
}

/* returns -1 if error or not found! */
int get_rb_ival(RB_ITREE *bt, int ind)
{
 RB_INODE *btn;
 btn = find_rb_inode(bt,ind);
 if(btn) {
 	return(btn->node_ival);
 } else {
 	return(-1);
 };
}

static RB_INODE *find_rb_inode(RB_ITREE *bt,int index)
{
 RB_INODE *btn;
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
