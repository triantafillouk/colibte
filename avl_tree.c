
#if	AVL_BALANCE
	/* avl balanced trees  */


#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))									  

/*         left rotate
 *
 *    (P)                (Q)
 *   /   \              /   \
 *  1    (Q)    ==>   (P)    3
 *      /   \        /   \
 *     2     3      1     2
 *
 */
static void left_rotate(BTNODE **node)
{
	BTNODE *p = *node;
	BTNODE *q = p->right;

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
static void right_rotate(BTNODE **node)
{
	BTNODE *p = *node;
	BTNODE *q = p->left;
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
static void left_right_rotate(BTNODE **node)
{
	BTNODE *p = *node;
	BTNODE *q = (*node)->left;
	BTNODE *r = q->right;

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
static void right_left_rotate(BTNODE **node)
{
	BTNODE *p = *node;
	BTNODE *q = (*node)->right;
	BTNODE *r = q->left;

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
static void rebalance(BTNODE **node)
{
	if ((*node)->balance == -2)
	{
		if ((*node)->left->balance == 1)
			left_right_rotate(node);
		else
			right_rotate(node);
	}
	else if ((*node)->balance == 2)
	{
		if ((*node)->right->balance == -1)
			right_left_rotate(node);
		else
			left_rotate(node);
	}
}


#if	A_DEBUG
int depth=0;
#endif

static 
 BTNODE * avl_insert(BTREE *tree, 	      /* the tree */
		      		  	BTNODE **node, 	      /* current node */
					  	BTNODE *parent,       /* parent node */
					  	char *key)   /* new entry */
{
	BTNODE *res = NULL;
	int before = (*node)->balance;
	int cmp = strcmp((*node)->node_name, key);

	if (cmp == 0) return ( *node);

	if (cmp > 0)
	{ 	/* go left */
		if ((*node)->left) 
		{
#if	A_DEBUG
			depth++;
#endif
			res = avl_insert(tree, &(*node)->left, *node, key);
		}
		else
		{
			(*node)->left = new_btnode();
			if ((*node)->left == NULL) return NULL;
			res = (*node)->left;
			res->node_name=strdup(key);
#if	A_DEBUG
			MESG(" avl add_btnode: new [%s] depth=%d",key,depth); 
#endif
			tree->new_flag=1;
			(*node)->balance--;
			tree->items++;
		}
	} else			/* go right */
	{
		if ((*node)->right) 
		{
#if	A_DEBUG
			depth++;
#endif
			res = avl_insert(tree, &(*node)->right, *node, key);
		}
		else
		{
			(*node)->right = new_btnode();
			if ((*node)->right == NULL) return NULL;
			res = (*node)->right;
			res->node_name=strdup(key);
#if	A_DEBUG
			MESG(" avl add_btnode: new [%s] depth=%d",key,depth); 
#endif
			tree->new_flag=1;
			(*node)->balance++;
			tree->items++;
		}
	}
	
	if (parent && (*node)->balance && before == 0)
		parent->balance += (parent->left == *node ? (-1) : (+1));

	rebalance(node);

	return res;
}

/* BTREE_insert recurses through the tree, finds where the new node fits
 * in, puts it there, and balances the tree on the way out of the recursion.
 *
 * if the insertion is successful, the node pointer is returned.
 */
BTNODE * add_btnode(BTREE *tree, char *name)
{
	BTNODE *res = NULL;

	res=find_btnode(tree,name);
	if(res) return(res);

	/* perform insert */
	if (tree->root)
	{
		res = avl_insert(tree, &tree->root, NULL, name);
	}
	else
	{
		tree->root = new_btnode();
		if (tree->root)
		{
			tree->items++;
			res = tree->root;
			res->node_name=strdup(name);
			tree->new_flag=1;
#if	A_DEBUG
			MESG(" avl add_btnode: new [%s] depth=%d",name,depth); 
#endif
		}
	}

	return res;
}

#endif
