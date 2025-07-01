

typedef struct AVL_INODE {
	struct AVL_INODE *left;
	struct AVL_INODE *right;
	int node_index;
	short balance;	/* for avl trees  */
	int ival;
} AVL_INODE;

typedef struct AVL_ITREE {
	struct AVL_INODE *root;
	char *tree_name;
	int	new_flag;
	long items;
} AVL_ITREE;

/* integer avl balanced trees  */

#if	!defined MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif
#if	!defined MAX
#define MAX(x, y) ((x) > (y) ? (x) : (y))  
#endif

/*         left rotate
 *
 *    (P)                (Q)
 *   /   \              /   \
 *  1    (Q)    ==>   (P)    3
 *      /   \        /   \
 *     2     3      1     2
 *
 */
static void avl_left_rotate(AVL_INODE **node)
{
	AVL_INODE *p = *node;
	AVL_INODE *q = p->right;

	p->right = q->left;
	q->left = p;
	*node = q;

	p->balance -= 1 + MAX(0, q->balance);
	q->balance -= 1 - MIN(0, p->balance);
}

/*           right rotate
 *  
 *       (P)                (Q)
 *      /   \              /   \
 *    (Q)    3    ==>     1    (P)  
 *   /   \                    /   \
 *  1     2                  2     3
 *
 */
static void avl_right_rotate(AVL_INODE **node)
{
	AVL_INODE *p = *node;
	AVL_INODE *q = p->left;
	p->left = q->right;
	q->right = p;
	*node = q;

	p->balance += 1 - MIN(0, q->balance);
	q->balance += 1 + MAX(0, p->balance);
}


/*            left-right rotate
 * 
 *          (P)                (R)
 *         /   \              /   \
 *      (Q)     4   ==>     (Q)    (P)  
 *      / \                /  \    /  \
 *     1  (R)             1    2  3    4
 *        / \
 *       2   3
 *
 *  note that left-right rotate could be implemented as a call to 
 *  left_rotate(Q) followed by a call to right_rotate(P).
 */
static void avl_left_right_rotate(AVL_INODE **node)
{
	AVL_INODE *p = *node;
	AVL_INODE *q = (*node)->left;
	AVL_INODE *r = q->right;

	q->right = r->left;
	p->left = r->right;
	r->right = p;
	r->left = q;

	*node = r;

	q->balance -= 1 + MAX(0, r->balance);
	p->balance += 1 - MIN(MIN(0, r->balance) - 1, r->balance + q->balance);
	r->balance += MAX(0, p->balance) + MIN(0, q->balance);
}

/*              right-left rotate
 * 
 *          (P)                   (R)
 *         /   \                 /   \
 *        1     (Q)     ==>    (P)    (Q)  
 *             /  \           /  \    /  \
 *           (R)   4         1    2  3    4
 *           / \
 *          2   3
 *
 *  note that right-left rotate could be implemented as a call to 
 *  right_rotate(Q) followed by a call to left_rotate(P).
 */
static void avl_right_left_rotate(AVL_INODE **node)
{
	AVL_INODE *p = *node;
	AVL_INODE *q = (*node)->right;
	AVL_INODE *r = q->left;

	q->left = r->right;
	p->right = r->left;
	r->left = p;
	r->right = q;

	*node = r;

	q->balance += 1 - MIN(0, r->balance);
	p->balance -= 1 + MAX(MAX(0, r->balance) + 1, r->balance + q->balance);
	r->balance += MAX(0, q->balance) + MIN(0, p->balance);
}

/*
 *  check balances and rotate as required. 
 */
static void avl_irebalance(AVL_INODE **node)
{
	if ((*node)->balance == -2)
	{
		if ((*node)->left->balance == 1)
			avl_left_right_rotate(node);
		else
			avl_right_rotate(node);
	}
	else if ((*node)->balance == 2)
	{
		if ((*node)->right->balance == -1)
			avl_right_left_rotate(node);
		else
			avl_left_rotate(node);
	}
}

AVL_INODE *new_avl_inode()
{
 AVL_INODE *btn;
 btn=(AVL_INODE *)malloc(sizeof(struct AVL_INODE));

 btn->left=NULL;
 btn->right=NULL;
 btn->node_index=0;

 btn->ival=0;
 btn->balance=0;
 return(btn);
}

AVL_ITREE *new_avl_itree(char *name)
{
 AVL_ITREE *bt;
 bt=(AVL_ITREE *)malloc(sizeof(struct AVL_ITREE));
 bt->root=NULL;
 bt->tree_name=strdup(name);
 bt->items=0;
 // MESG("		new_RB_ITREE: ok!");
 return(bt);
}

static 
 AVL_INODE * avl_iinsert(AVL_ITREE *tree, 	      /* the tree */
		      		  	AVL_INODE **node, 	      /* current node */
					  	AVL_INODE *parent,       /* parent node */
					  	int key)   /* new entry */
{
	AVL_INODE *res = NULL;
	int before = (*node)->balance;

	if ((*node)->node_index == key) return ( *node);

	if ((*node)->node_index > key)
	{ 	/* go left */
		if ((*node)->left) 
		{
			res = avl_iinsert(tree, &(*node)->left, *node, key);
		}
		else
		{
			(*node)->left = new_avl_inode();
			if ((*node)->left == NULL) return NULL;
			res = (*node)->left;
			res->node_index=key;
			tree->new_flag=1;
			(*node)->balance--;
			tree->items++;
		}
	} else			/* go right */
	{
		if ((*node)->right) 
		{
			res = avl_iinsert(tree, &(*node)->right, *node, key);
		}
		else
		{
			(*node)->right = new_avl_inode();
			if ((*node)->right == NULL) return NULL;
			res = (*node)->right;
			res->node_index=key;
			tree->new_flag=1;
			(*node)->balance++;
			tree->items++;
		}
	}
	
	if (parent && (*node)->balance && before == 0)
		parent->balance += (parent->left == *node ? (-1) : (+1));

	avl_irebalance(node);

	return res;
}


AVL_INODE *find_avl_inode(AVL_ITREE *bt,int key)
{
 AVL_INODE *btn;
 btn=bt->root;

	while(btn!=NULL) {
		if(key==btn->node_index) { 
			return(btn);
		};
		if(key>btn->node_index) {
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

int get_avl_ival(AVL_ITREE *tree,int key)
{
	AVL_INODE *res = NULL;
	res=find_avl_inode(tree,key);
	if(res) { 
		return(res->ival);
	};
	return -1;
}

/* AVL_ITREE_insert recurses through the tree, finds where the new node fits
 * in, puts it there, and balances the tree on the way out of the recursion.
 *
 * if the insertion is successful, the node pointer is returned.
 */
AVL_INODE * set_avl_ival(AVL_ITREE *tree, int key,int ival)
{
	AVL_INODE *res = NULL;

	res=find_avl_inode(tree,key);
	if(res) { 
		tree->new_flag=0;
		res->ival=ival;
		return(res);
	};

	/* perform insert */
	if (tree->root)
	{
		res = avl_iinsert(tree, &tree->root, NULL, key);
	}
	else
	{
		tree->root = new_avl_inode();
		if (tree->root)
		{
			res = tree->root;
			res->node_index=key;
			tree->new_flag=1;
			tree->items++;
		}
	}
	res->ival=ival;
	return res;
}

#if	NUSE

void ibtn_free(AVL_INODE *btn)
{
	if(btn){
		// if(btn->node_name) free(btn->node_name);
		// if(btn->node_vtype==VTYPE_TREE) btn_free(btn->node_dat);else
		// else if(btn->node_vtype>VTYPE_STRING) free(btn->node_sval); 
		free(btn);
	};
}

void delete_avl_inode(AVL_INODE *node)
{
 if(node->left) {
 	delete_avl_inode(node->left);
	btn_free(node->left);
	node->left=NULL;
 };
 if(node->right) {
 	delete_avl_inode(node->right);
	btn_free(node->right);
	node->right=NULL;
 };
 if(node->sval) free(node->sval);
}
#endif
