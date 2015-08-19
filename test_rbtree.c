
#include "odt_rbtree.h"


int mycmp (void *key1, void *key2)
{
    return (int)key1 - (int)key2 ;
}



int main ()
{
    odt_rbtree_t *tree = odt_rbtree_create (sizeof(int), mycmp) ;
    odt_rbtree_node_t *node ;
    int n ;

    if (NULL == tree)
        exit (1) ;

    int myelt [] = {

        1, 2, 2, 1, 3, 4, 2, 0, 4, 9, 8, 9, 3, -1
    } ;

    for (n=0; myelt[n] != -1; ++n) {

        if (-1 == odt_rbtree_insert (tree, &myelt[n], myelt[n]))
            exit (1) ;
    }

    odt_rbtree_destroy (tree) ;


    if (NULL != (node = odt_rbtree_search (tree, &myelt[2], 9)))
        exit (1) ;

    if (NULL == (node = odt_rbtree_search (tree, &myelt[2], 2)))
        exit (1) ;

    if (-1 == odt_rbtree_remove (tree, node))
        exit (1) ;

    for (--n; n>= 0; --n) {

        if (NULL == (node = odt_rbtree_search (tree, &myelt[n], myelt[n])))
            exit (1) ;

        if (-1 == odt_rbtree_remove (tree, node))
            exit (1) ;

        odt_free (node) ;

        if (NULL != (node = odt_rbtree_search (tree, &myelt[n], myelt[n])))
            exit (1) ;
    }
}


