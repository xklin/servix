/*
 *    Users can get the media's informations from the CSV file
 *    via these APIs.
 *
 *    Get medias' informations from the CSV file has four methods:
 *        -> poll, get the entries one by one
 *        -> random, get the entries randomly
 *        -> repeat, get the entries repeatedly
 *        -> heat, get the entries by medias' heat
 *        -> play list id, get the entries which has the specific list_id
 */


#include "odt.h"


/*
 * These function will return a variable typed 'odt_string_t' and it's
 * allocated from the heap by user
 */



odt_csv_t    *odt_g_csv ;


odt_string_t
*odt_media2string (odt_media_entry_t *media)
{
    odt_assert (NULL != media) ;

    odt_string_t *string ;

    string = odt_string_create_free(64) ;
    if (NULL == string) {

        ODT_LOG_ERR("Create string failed") ;
        return NULL ;
    }


    if (-1 == odt_string_append (string, "a=X-playlist-item: "))
        return NULL ;

    if (-1 == odt_string_appendv (string, 6, media->pid, " ", 
                    media->aid, " ", media->range, " "))
        return NULL ;

    if (media->tricks[0]) {

        if (-1 == odt_string_appendv (string, 2, "tricks/",
                                 media->tricks))
            return NULL ;
    }

    return string ;
}



odt_string_t
*odt_playlist2string (odt_media_entry_t *playlist)
{
    odt_assert (NULL != playlist) ;

    odt_string_t *string ;
    odt_media_entry_t  *mep, *tmep ;
    struct list_head  *node ;
    int n ;

    string = odt_string_create_free (128) ;
    if (NULL == string) {

        ODT_LOG_ERR("Create string failed") ;
        return NULL ;
    }


    node = &playlist->nob ;

    while (1) {

        mep = list_entry(node, odt_media_entry_t, nob) ;

        if (-1 == odt_string_append (string, "a=X-playlist-item: "))
            return NULL ;

        if (-1 == odt_string_appendv (string, 6, mep->pid, " ", 
                    mep->aid, " ", mep->range, " "))
            return NULL ;

        if (mep->tricks[0]) {

            if (-1 == odt_string_appendv (string, 2, "tricks/",
                                 mep->tricks))
                return NULL ;
        }

        if (node = node->next, node == &playlist->nob)
            break ;
        
        if (-1 == odt_string_append (string, "\r\n"))
                return NULL ;
    }

    return string ;
}





odt_media_entry_t
*odt_csv_fetch_media (odt_csv_t *csv, int algorithm)
{
    odt_assert (NULL != csv) ;

    int i ;

    if (!csv->entries || csv->amount <= 0) {

        ODT_LOG_ERR("No media entries is available in the CSV file") ;
        return NULL ;
    }

    switch (algorithm) {

    case MEDIA_HEAT :
        /* This algorithm is temporarily unavailable, just using poll
          algorithm to instead */
    case MEDIA_POLL :
        i = (csv->last_index<0 || csv->last_index>=csv->amount)
            ? 0 : csv->last_index ;
        csv->last_index = i+1 ;
        break ;

    case MEDIA_RANDOM :
        i = odt_rand () ;
        i = i >= csv->amount ? i % csv->amount : i ;
        break ;

    case MEDIA_REPEAT :
        i = (csv->last_index<0 || csv->last_index>=csv->amount)
            ? 0 : csv->last_index ;
        break ;

    default :
        ODT_LOG_ALERT("Algorithm for fetching media is nonexistent") ;
        return NULL ;
    }

    return csv->entries + i ;
}



odt_media_entry_t
*odt_csv_fetch_playlist (odt_csv_t *csv, size_t list_id)
{
    odt_assert (NULL != csv) ;

    odt_rbtree_node_t *node ;

    node = odt_rbtree_search(csv->entries_root, NULL, (void*) list_id) ;
    if (NULL == node) {

        ODT_LOG_WARN("Cannot find the play list id : %d", list_id) ;
        return NULL ;
    }

    return (odt_media_entry_t *) node->m_elt ;
}
