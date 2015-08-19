
#include "odt.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>




void
odt_csv_parse_amount (const char *csv_buf, size_t buf_len, odt_csv_t *csv)
{
    odt_assert (NULL != csv && NULL != csv_buf) ;

    const char *p ;
    int res , mask ;
    int count ;


    mask = '\n' | ('\n' << 8) | ('\n' << 16) | ('\n' << 24) ;

    /* Read the whole file for checking the amount of entries */

    for (p=csv_buf, count=0; p<csv_buf+buf_len; p+=4) {

        res = *((int *)p) ^ mask ;
        if (!((res & 0xff) && ((res >> 8) & 0xff) &&
           ((res >> 16) & 0xff) && ((res >> 24) & 0xff)))
            ++ count ;
    }

    csv->amount = -- count ;
    csv->entries = odt_malloc (sizeof(odt_media_entry_t) * count) ;
}





int
odt_csv_parse_construction (const char *csv_buf, size_t buf_len)
{
    odt_assert (NULL != csv_buf) ;

    const char *p ;
    int state ;

    for (p=csv_buf, state=0; p<csv_buf+buf_len && *p!='\n'; ++p) {

        switch (state) {

        case 0 :
            if (isblank(*p))
                break ;

            if (!odt_strstr7cmp(p, "List_ID"))
                return -1 ;

            p += (sizeof("List_ID") - 1) ;
            state = 1 ;

        case 1 :
            if (isblank(*p))
                break ;
            else if (*p != ',')
                return -1 ;

            state = 2 ;
            break ;

        case 2 :
            if (isblank(*p))
                break ;

            if (!odt_strstr10cmp(p, "Provide_ID"))
                return -1 ;

            p += (sizeof("Provide_ID")-1) ;
            state = 3 ;

        case 3 :
            if (isblank(*p))
                break ;
            else if (*p != ',')
                return -1 ;

            state = 4 ;
            break ;

        case 4 :
            if (isblank(*p))
                break ;

            if (!odt_strstr8cmp(p, "Asset_ID"))
                return -1 ;

            p += (sizeof("Asset_ID")-1) ;
            state = 5 ;

        case 5 :
            if (isblank(*p))
                break ;
            else if (*p != ',')
                return -1 ;

            state = 6 ;
            break ;

        case 6 :
            if (isblank(*p))
                break ;

            if (!odt_strstr5cmp(p, "Range"))
                return -1 ;

            p += (sizeof("Range")-1) ;
            state = 7 ;

        case 7 :
            if (isblank(*p))
                break ;
            else if (*p != ',')
                return -1 ;

            state = 8 ;
            break ;

        case 8 :
            if (isblank(*p))
                break ;

            if (!odt_strstr6cmp(p, "Tricks"))
                return -1 ;

            p += (sizeof("Tricks")-1) ;
            state = 9 ;

        case 9 :
            if (isblank(*p))
                break ;
            else if (*p != ',')
                return -1 ;

            state = 10 ;
            break ;

        case 10 :
            if (isblank(*p))
                break ;

            if (!odt_strstr4cmp(p, "Heat"))
                return -1 ;

            p += (sizeof("Heat")-1) ;
            state = 11 ;

        case 11 :
            if (isblank(*p))
                break ;
            else if (*p != ',')
                return -1 ;

            state = 12 ;
            break ;

        case 12 :
            if (isblank(*p))
                break ;

            if (!odt_strstr6cmp(p, "Enable"))
                return -1 ;

            p += (sizeof("Enable")-1) ;
            state = 13 ;

        case 13 :
            if (isblank(*p))
                break ;
            else if (*p != '\n' && *p != '\r')
                return -1 ;

            return ;
        }
    }
}




enum {

    LIST_ID_B, LIST_ID, LIST_ID_COLO, 
    PID_B, PID, PID_COLO,
    AID_B, AID, AID_COLO,
    RANGE_B, RANGE_LEFT, RANGE_RIGHT, RANGE_COLO,
    TRICKS_B, TRICKS_F, TRICKS_P, TRICKS_R, TRICKS_COLO,
    HEAT_B, HEAT, HEAT_COLO,
    ENABLE,
    LINE_FEED,
} ;

#define CSV_PARSE_STATE_START    LIST_ID_B



int
odt_csv_parse_entry (const char *csv_buf, size_t buf_len, odt_csv_t *csv)
{
    odt_assert (NULL != csv_buf) ;
    odt_assert (NULL != csv) ;

    const char *p , *l ;
    int state ;
    odt_media_entry_t *mep , *tmep ;
    odt_rbtree_node_t *node ;


    if (buf_len == 0) {

        ODT_LOG_ERR("Empty CSV file") ;
        return -1 ;
    }


    /* Initialize the root of entries */
    csv->entries_root = odt_rbtree_create (sizeof(odt_media_entry_t),
                                           odt_digit_cmp_func) ;

    if (!csv->entries_root) {

        ODT_LOG_ERR("Create rbtree failed") ;
        return -1 ;
    }


    /* Skip the first line */
    for (p=csv_buf; *p!='\n' && p<csv_buf+buf_len; ++p) ;


    for (++p, state = CSV_PARSE_STATE_START, mep=csv->entries;
         p < csv_buf + buf_len; ++ p) {

        switch (state) {

        case LIST_ID_B :
            if (isblank(*p))
                break ;
            else if (!isdigit(*p))
                goto __skip_line ;

            l = p ;
            state = LIST_ID ;
            break ;

        case LIST_ID :
            if (isdigit(*p))
                break ;

            mep->list_id = odt_natoi (l, p-l) ;
            state = LIST_ID_COLO ;

        case LIST_ID_COLO :
            if (isblank(*p))
                break ;

            if (*p != ',')
                goto __skip_line ;

            state = PID_B ;
            break ;

        case PID_B :
            if (isblank(*p))
                break ;
            else if (!isdigit(*p) && !isalpha(*p))
                goto __skip_line ;

            l = p ;
            state = PID ;
            break ;

        case PID :
            if (isdigit(*p) || isalpha(*p))
                break ;

            memcpy (mep->pid, l, p-l) ;
            state = PID_COLO ;

        case PID_COLO :
            if (isblank(*p))
                break ;

            if (*p != ',')
                goto __skip_line ;

            state = AID_B ;
            break ;
 
        case AID_B :
            if (isblank(*p))
                break ;
            else if (!isdigit(*p) && !isalpha(*p))
                goto __skip_line ;

            l = p ;
            state = AID ;
            break ;

        case AID :
            if (isdigit(*p) || isalpha(*p))
                break ;

            memcpy (mep->aid, l, p-l) ;
            state = AID_COLO ;

        case AID_COLO :
            if (isblank(*p))
                break ;

            if (*p != ',')
                goto __skip_line ;

            state = RANGE_B ;
            break ;

        case RANGE_B :
            if (isblank(*p))
                break ;
            else if (!isdigit(*p))
                goto __skip_line ;

            l = p ;
            state = RANGE_LEFT ;
            break ;

        case RANGE_LEFT :
            if (isdigit(*p))
                break ;

            if (*p != '-')
                goto __skip_line ;

            state = RANGE_RIGHT ;
            break ;

        case RANGE_RIGHT :
            if (isdigit(*p))
                break ;

            memcpy (mep->range, l, p-l) ;
            state = RANGE_COLO ;

        case RANGE_COLO :
            if (isblank(*p))
                break;

            if (*p != ',')
                goto __skip_line ;

            state = TRICKS_B ;
            break ;

        case TRICKS_B :
            if (isblank(*p))
                break ;

            switch (*p) {

            case 'F' :
                state = TRICKS_F ;
                break ;
            case 'P' :
                state = TRICKS_P ;
                break ;
            case 'R' :
                state = TRICKS_R ;
                break ;
            default :
                -- p ;
                state = TRICKS_COLO ;
            }

            l = p ;
            break ;

        case TRICKS_F :
            switch (*p) {

            case 'P' :
                state = TRICKS_P ;
                break ;
            case 'R' :
                state = TRICKS_R ;
                break ;
            default :
                -- p ;
                state = TRICKS_R ;
            }

            break ;

        case TRICKS_P :
            switch (*p) {

            case 'R' :
                state = TRICKS_R ;
                break ;
            default :
                state = TRICKS_R ;
                -- p ;
            }

            break ;

        case TRICKS_R :
            memcpy (mep->tricks, l, p-l) ;
            state = TRICKS_COLO ;

        case TRICKS_COLO :
            if (isblank(*p))
                break ;

            if (*p != ',')
                goto __skip_line ;

            state = HEAT_B ;
            break ;

        case HEAT_B :
            if (isblank(*p))
                break ;
            else if (*p == ',') {
                state = ENABLE ;
                break ;
            } else if (!isdigit(*p))
                goto __skip_line ;

            l = p ;
            state = HEAT ;
            break ;

        case HEAT :
            if (isdigit(*p))
                break ;

            mep->heat = odt_natoi(l, p-l) ;
            state = HEAT_COLO ;

        case HEAT_COLO :
            if (isblank(*p))
                break ;
            else if (*p != ',')
                goto __skip_line ;

            state = ENABLE ;
            break ;

        case ENABLE :
            if (isblank(*p))
                break ;

            switch (*p) {

            case '0' :
                goto __skip_line ;
            case '1' :
                break ;
            default :
                goto __skip_line ;
            }

            state = LINE_FEED ;
            break ;

        case LINE_FEED :
            if (isblank(*p) || *p == '\r')
                break ;
            else if (*p != '\n')
                goto __skip_line ;

            node = odt_rbtree_search (csv->entries_root, NULL, (void *)mep->list_id) ;

            if (!node) {

                odt_rbtree_insert (csv->entries_root, mep, (void *)mep->list_id) ;
                INIT_LIST_HEAD (&mep->nob) ;
            } else {

                tmep = (odt_media_entry_t *) node->m_elt ;
                list_add (&mep->nob, &tmep->nob) ;
            }

            state = LIST_ID_B ;
            ++ mep ;
            break ;
        }

        continue ;

__skip_line :
        -- csv->amount ;
        memset (mep, 0, sizeof(odt_media_entry_t)) ;
        for (; p < csv_buf + buf_len && *p != '\n'; ++p) ;
        state = LIST_ID_B ;
    }

    odt_assert (0 <= csv->amount) ;
    if (csv->amount == 0) {

        ODT_LOG_ERR("Number of valid entries is 0") ;
        return -1 ;
    }

    return 0 ;
}




int
odt_csv_parse_format (const char *csv_path)
{
    const char *p ;

    for (p=csv_path; *p; ++p) ;

    if (-1 == odt_str4cmp (p-4, '.', 'c', 's', 'v'))
        return -1 ;

    return 0 ;
}





void
odt_csv_destroy (odt_csv_t *csv)
{
    odt_rbtree_destroy (csv->entries_root) ;
    odt_free (csv->entries) ;
}





odt_csv_t
*odt_csv_parse (const char *csv_path)
{
    odt_assert (NULL != csv_path) ;

    int fd ;
    char *buf ;
    struct stat statbuf ;
    odt_csv_t *csv ;


    /* Parse the file's format */
    if (-1 == odt_csv_parse_format (csv_path)) {

        ODT_LOG_ERR("file's format is invalid") ;
        return NULL ;
    }

    fd = open (csv_path, O_RDONLY) ;
    if (-1 == fd) {

        ODT_LOG_EMERG("'open' failed, errno=%d", errno) ;
        return NULL ;
    }

    if (-1 == fstat (fd, &statbuf)) {

        ODT_LOG_EMERG("'fstat' failed, errno=%d", errno) ;
        return NULL ;
    }

    buf = odt_malloc (statbuf.st_size) ;
    if (statbuf.st_size > read (fd, buf, statbuf.st_size)) {

        ODT_LOG_ALERT("Read contents from the csv file failed,"
                      "errno=%d", errno) ;
        return NULL ;
    }

    /* Parse the construction */
    if (-1 == odt_csv_parse_construction(buf, statbuf.st_size)) {

        ODT_LOG_ERR("construction of the csv file is invalid") ;
        return NULL ;
    }

    csv = odt_malloc (sizeof(odt_csv_t)) ;

    /* Parse the amount */
    odt_csv_parse_amount (buf, statbuf.st_size, csv) ;
    if (csv->amount <= 0) {

        ODT_LOG_ERR("number of entries is 0") ;
        return NULL ;
    }

    /* Parse the entries */
    if (-1 == odt_csv_parse_entry (buf, statbuf.st_size, csv))
        return NULL ;


    close (fd) ;

    return csv ;
}
