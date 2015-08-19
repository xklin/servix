
#ifndef __ODT_CSV_HEAD__
#define __ODT_CSV_HEAD__




#include "list.h"

typedef struct {

    int                 list_id ;
    char                pid [32] ;
    char                aid [32] ;
    char                range[64] ;
    char                tricks[4] ;
    int                 heat ;

    struct list_head    nob ;
} odt_media_entry_t ;



enum {

    MEDIA_POLL,
    MEDIA_RANDOM,
    MEDIA_REPEAT,
    MEDIA_HEAT
} ;


typedef struct {

    odt_rbtree_t        *entries_root;
    odt_media_entry_t   *entries;
    size_t               amount ;

    int                  last_index ;
} odt_csv_t ;



extern odt_csv_t    *odt_g_csv ;


odt_csv_t         *odt_csv_parse(const char *csv_path) ;
odt_media_entry_t *odt_csv_fetch_media (odt_csv_t *csv, int algorithm) ;
odt_media_entry_t *odt_csv_fetch_playlist (odt_csv_t *csv, size_t list_id) ;
odt_string_t      *odt_media2string (odt_media_entry_t *media) ;
odt_string_t      *odt_playlist2string (odt_media_entry_t *playlist) ;
void               odt_csv_destroy (odt_csv_t *csv) ;

#endif
