
#include "odt.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>


int main ()
{
    odt_csv_t *csv ;
    odt_media_entry_t *media ;
    odt_string_t *str ;
    int n ;

    csv = odt_csv_parse("a.csv") ;

/*
        media = odt_csv_fetch_playlist (csv, 521) ;
        str = odt_playlist2string (media) ;
        write (STDOUT_FILENO, str->m_data, str->m_len) ;
        write (STDOUT_FILENO, "\n", 1) ;
*/
    for (n=0; n<100; ++n) {

        media = odt_csv_fetch_media (csv, MEDIA_POLL) ;
        str = odt_media2string (media) ;
        write (STDOUT_FILENO, str->m_data, str->m_len) ;
        write (STDOUT_FILENO, "\n", 1) ;
    }

    while (1) ;
}
