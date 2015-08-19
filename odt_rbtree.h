
/*
 *	The main code is copy from linux kernel
 *	Some interfaces is implemented as follow
 */
#ifndef __ODT_RBTREE_H__
#define __ODT_RBTREE_H__

#include "rbtree.h"


/* Compare the left to the right, if 'left > right', return 1;
 * if 'left < right', return -1;
 * if 'left == right', return 0.
 */
typedef int (*odt_rbtree_cmp_func) (void *, void *) ;

static inline int
odt_digit_cmp_func (void *key1, void *key2)
{
    int left, right;

    left = (int) key1 ;
    right = (int) key2 ;

    if (left < right)
        return -1 ;
    else if (left > right)
        return 1 ;

    return 0 ;
}





typedef struct {

	struct rb_root m_root ;
	unsigned int m_elt_len ;
	odt_rbtree_cmp_func m_cmp ;
} odt_rbtree_t ;


typedef struct {

	struct rb_node m_node ;
	void *m_elt ;

	void *sib ;
	void *prev ;

	void *m_key ;
} odt_rbtree_node_t ;


/* Judge a tree is empty or not */
#define odt_rbtree_empty(tree)\
	RB_EMPTY_ROOT (&tree->m_root)

#define odt_rbtree_node_parent(node) \
	({struct rb_node *___rb = rb_parent(&node->m_node) ;\
		rb_entry (___rb, odt_rbtree_node_t, m_node);})


#define odt_rbtree_node_create(key_type_len) \
	({ odt_rbtree_node_t *___node = odt_malloc (sizeof(odt_rbtree_node_t)\
										+ key_type_len) ;\
		___node->m_key = ___node + sizeof(odt_rbtree_node_t) ;\
		___node ; })



/* Create a empty tree */
odt_rbtree_t *odt_rbtree_create (unsigned int elt_len,
							odt_rbtree_cmp_func cmp) ;


/* Insert a element into a tree */
int odt_rbtree_insert (odt_rbtree_t *tree, void *elt, void *key) ;


/* Delete a element from a tree */
int odt_rbtree_remove (odt_rbtree_t *tree, odt_rbtree_node_t *node) ;


/* Search the smallest elt */
odt_rbtree_node_t *odt_rbtree_min (odt_rbtree_t *tree) ;


/* Destroy a tree */
void odt_rbtree_destroy (odt_rbtree_t *tree) ;


/* Search for a node by its key */
odt_rbtree_node_t *odt_rbtree_search (odt_rbtree_t *tree, void *elt, void *key) ;

#endif
