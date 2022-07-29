/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

*/
/* version 14/10/2020 */

/* red/black tree rebalancing */
#if	RB_BALANCE
#define	RB_BLACK	0
#define	RB_RED		1

static BTNODE *sibling(BTNODE *node)
{
	return node == node->up->left ? node->up->right : node->up->left;
}

static int is_right_child(BTNODE *node)
{
	return (node->up->right == node);
}

static int is_left_child(BTNODE *node)
{
	return (node->up->left == node);
}

/*         left rotate
 *
 *    (P)                (Q)
 *   /   \              /   \
 *  1    (Q)    ==>   (P)    3
 *      /   \        /   \
 *     2     3      1     2
 *
 */
static void left_rotate(BTREE *tree, BTNODE *p)
{
	BTNODE *q = p->right;
	BTNODE **sup;
	
	if (p->up)
		sup = is_left_child(p) ? &(p->up->left) : &(p->up->right);
	else
		sup = &tree->root;

	p->right = q->left;
	if (p->right) p->right->up = p;
	q->left = p;
	q->up = p->up;
	p->up = q;
	*sup = q;
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
static void right_rotate(BTREE *tree, BTNODE *p)
{
	BTNODE *q = p->left;
	BTNODE **sup;
	
	if (p->up)
		sup = is_left_child(p) ? &(p->up->left) : &(p->up->right);
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
static void rebalance(BTREE *tree, BTNODE *node)
{
	BTNODE *up = node->up;
	if (up == NULL || up->color == RB_BLACK) return;
//	BTNODE *sibling_up = ((up == up->up->left) ? up->up->right : up->up->left);
	BTNODE *sibling_up = sibling(up);
	if (sibling_up && sibling_up->color == RB_RED)
	{
		up->color = RB_BLACK;
//		sibling(up)->color = RB_BLACK;
		sibling_up->color = RB_BLACK;
		if (up->up->up)
		{
			up->up->color = RB_RED;
			rebalance(tree, up->up);
		}
	}
	else
	{
		if (is_left_child(node) && is_right_child(up))
		{
			right_rotate(tree, up);
			node = node->right;
		}
		else if (is_right_child(node) && is_left_child(up))
		{
			left_rotate(tree, up);
			node = node->left;
		}

		node->up->color = RB_BLACK;
		node->up->up->color = RB_RED;

		if (is_left_child(node)) // && is_left_child(node->up)
			right_rotate(tree, node->up->up);
		else 
			left_rotate(tree, node->up->up);
	}
}
#endif

