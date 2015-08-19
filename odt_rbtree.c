#include <string.h>
#include "rbtree.h"
#include "odt.h"


int
odt_rbtree_default_cmp (void *key1, void *key2)
{
	return 0 ;
}


odt_rbtree_t
*odt_rbtree_create (unsigned int elt_len, odt_rbtree_cmp_func cmp)
{
	odt_assert (0 <= elt_len) ;

	odt_rbtree_t *newtree ;

	newtree = odt_malloc (sizeof(odt_rbtree_t)) ;

	newtree->m_root.rb_node = NULL ;
	newtree->m_elt_len = elt_len ;

	if (!cmp)
		newtree->m_cmp = odt_rbtree_default_cmp ;
	else
		newtree->m_cmp = cmp ;

	return newtree ;
}




int
__odt_rbtree_insert (odt_rbtree_t *tree, odt_rbtree_node_t *new)
{
	struct rb_node **slot = &tree->m_root.rb_node ;
	struct rb_node *parent = NULL ;
    odt_rbtree_node_t *pnode ;
	int ret ;


	while (*slot != NULL) {

		pnode = rb_entry (*slot, odt_rbtree_node_t, m_node) ;
		parent = *slot ;

		/* Compare two keys */
		ret = tree->m_cmp (pnode->m_key, new->m_key) ;

		if (0 < ret)
			slot = &((*slot)->rb_left) ;
		else if (0 > ret)
			slot = &((*slot)->rb_right) ;
		else
			goto __same_key ;
	}

	rb_link_node (&new->m_node, parent, slot) ;
	rb_insert_color (&new->m_node, &tree->m_root) ;

	return 0 ;

__same_key :
	/* Print : Insert a element with the same key */

	while (pnode->sib)
		pnode = (odt_rbtree_node_t *)pnode->sib ;

	pnode->sib = new ;
	new->prev = pnode ;

	return 0 ;
}



int
odt_rbtree_insert (odt_rbtree_t *tree, void *elt, void *key)
{
	assert (NULL != tree) ;

	odt_rbtree_node_t *pnode ;
	odt_rbtree_node_t *newnode ;


	newnode = odt_malloc (sizeof(odt_rbtree_node_t)) ;
    *newnode = (odt_rbtree_node_t) {

        .m_elt = elt,
        .m_key = key,
        .prev = NULL,
        .sib = NULL,
    } ;
    rb_init_node (&newnode->m_node) ;

    return __odt_rbtree_insert (tree, newnode) ;
}


int
odt_rbtree_remove (odt_rbtree_t *tree, odt_rbtree_node_t *node)
{
	odt_assert (NULL != tree) ;
	odt_assert (NULL != node) ;

	odt_rbtree_node_t *sib, *prev ;
	struct rb_node **link, *parent ;

	prev = (odt_rbtree_node_t *)node->prev ;
	sib = (odt_rbtree_node_t *)node->sib ;

	if (node->prev) {

		/* This is a sibling of a node in the tree */
		prev->sib = sib ;

		if (sib)
			sib->prev = node->prev ;
	}else if (node->sib) {

		/* This is the first node in the slot with the key, so 
           delete it from the rbtree and unlink it from the
           same-key-list, and then insert its sib to rbtree */


/*
		parent = rb_parent(&node->m_node) ;
        if (!parent) {

            link = &tree->m_root.rb_node ;
        } else {        

		    link = ((long)parent->rb_left ^ (long)&node->m_node) ?
					(&parent->rb_right) : (&parent->rb_left) ;
        }
*/

        rb_erase (&node->m_node, &tree->m_root) ;
        sib->prev = NULL ;

        return __odt_rbtree_insert (tree, sib) ;

/*
	    rb_link_node (&sib->m_node, parent, link) ;
        rb_insert_color (&sib->m_node, &tree->m_root) ;
*/
	}else {

		rb_erase (&node->m_node, &tree->m_root) ;
    }

	/* The node's space should be freed by caller */

    return 0 ;
}


odt_rbtree_node_t
*odt_rbtree_search (odt_rbtree_t *tree, void *elt, void *key)
{
	assert (NULL != tree) ;

	struct rb_node *node = tree->m_root.rb_node ;
	odt_rbtree_node_t *p ;
	int ret ;

	while (node) {
		p = rb_entry (node, odt_rbtree_node_t, m_node) ;

		ret = tree->m_cmp (p->m_key, key) ;

		if (0 < ret)
			node = node->rb_left ;
		else if (0 > ret)
			node = node->rb_right ;
		else
			goto find_node ;
	}

	return NULL ;

find_node :
	for (; elt && p && p->m_elt != elt; p=p->sib) ;

	return p ;
}


odt_rbtree_node_t
*odt_rbtree_min (odt_rbtree_t *tree)
{
	return (odt_rbtree_node_t *)rb_entry (rb_first (&tree->m_root),
					odt_rbtree_node_t, m_node) ;
}


void
odt_rbtree_destroy (odt_rbtree_t *tree)
{
	odt_assert (NULL != tree) ;

    odt_rbtree_node_t *node ;

    while (node = odt_rbtree_min (tree)) {

        odt_rbtree_remove (tree, node) ;
        odt_free (node) ;
    }
}
