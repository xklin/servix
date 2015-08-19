
#include "odt.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>




/* VERB of RTSP message :
 *     SETUP            5
 *     TEARDOWN         8
 *     PLAY             4
 *     PAUSE            5
 *     GET_PARAMETER    13
 *     SET_PARAMETER    13
 *     ANNOUNCE         8
 */



int _odt_rtsp_parse (odt_rtsp_t *) ;
odt_rtsp_verb_t odt_rtsp_parse_verb (const char *, size_t) ;
odt_rtsp_type_t odt_rtsp_parse_type (const char *, size_t) ;
int odt_rtsp_parse_field(odt_rtsp_t*, const char*, size_t) ;
int odt_rtsp_parse_url(odt_rtsp_t *, const char *, size_t) ;


int
odt_rtsp_init (odt_rtsp_t *msg)
{
   odt_assert (NULL!=msg) ;

   memset (msg, 0, sizeof(odt_rtsp_t)) ;

   msg->fields = odt_rbtree_create(sizeof(odt_string_t), odt_strcmp_callback) ;
   INIT_LIST_HEAD (&msg->fields_list) ;

   if (NULL == msg->fields) {

       ODT_LOG_ERR("Create rtsp message rbtree failed") ;
       return -1 ;
   }

   return 0 ;
}






/*
 * Clean up the contents of fields, contents of status/request line
 * and contents of 'parsed_msg'. Remain the 'msg' because it's the
 * buffer of all the messages recieved. Messages doesn't be parsed
 * should be retained.
 */
void
odt_rtsp_cleanup (odt_rtsp_t *msg)
{
    odt_assert (NULL != msg) ;

    rtsp_fields_t *pos, *next ;


    // Copy the rest contents to the begin of the string
    if (msg->msg.m_len > msg->parsed_msg.m_len)
        odt_string_update (&msg->msg, msg->msg.m_data+msg->last_point,
                       msg->msg.m_len-msg->last_point) ;
    else
        odt_string_clean (&msg->msg) ;


    if (msg->fields) {

        list_for_each_entry_safe (pos, next, &msg->fields_list, nob) {

            odt_string_clean (&pos->key);
            odt_string_clean (&pos->value) ;
            odt_free (pos) ;
        }

        INIT_LIST_HEAD (&msg->fields_list) ;
        odt_rbtree_destroy (msg->fields) ;
    }


    odt_string_clean (&msg->verb) ;
    odt_string_clean (&msg->url_port) ;
    odt_string_clean (&msg->url_path) ;
    odt_string_clean (&msg->type) ;
    odt_string_clean (&msg->version) ;
    odt_string_clean (&msg->body) ;
    odt_string_clean (&msg->code) ;
    odt_string_clean (&msg->desc) ;
    odt_string_clean (&msg->parsed_msg) ;


    msg->last_point = 0;
    msg->last_head = 0;
    msg->state = 0;
    msg->last_state = 0;
    msg->content_length = 0;
    msg->parse_done = 0;
    msg->req = -1;
    msg->verbd = 0;
    msg->typed = 0;
}



void
odt_rtsp_destroy (odt_rtsp_t *msg)
{
    odt_assert (NULL != msg) ;

    odt_rtsp_cleanup (msg) ;
    odt_string_clean (&msg->msg) ;
    odt_free (msg->fields) ;
}





odt_rtsp_verb_t
odt_rtsp_parse_verb (const char *str, size_t len)
{
    odt_assert (NULL != str) ;

    switch (len) {

    case 4:
        if (odt_str4cmp(str, 'P', 'L', 'A', 'Y'))
            return RTSP_PLAY ;
        else
            return RTSP_OTHER ;

    case 5:
        if (odt_str5cmp(str, 'S', 'E', 'T', 'U', 'P'))
            return RTSP_SETUP ;
        else if (odt_str5cmp(str, 'P', 'A', 'U', 'S', 'E'))
            return RTSP_PAUSE ;
        else
            return RTSP_OTHER ;

    case 8:
        if (odt_str8cmp(str, 'T', 'E', 'A', 'R', 'D', 'O', 'W', 'N'))
            return RTSP_TEARDOWN ;
        else if (odt_str8cmp(str, 'A', 'N', 'N', 'O', 'U', 'N', 'C', 'E'))
            return RTSP_ANNOUNCE ;
        else
            return RTSP_OTHER ;

    case 13:
        if (odt_str13cmp(str, 'G', 'E', 'T', '_', 'P', 'A', 'R', 'A',
                         'M', 'E', 'T', 'E', 'R'))
            return RTSP_GET_PARAMETER ;
        else if (odt_str13cmp(str, 'S', 'E', 'T', '_', 'P', 'A', 'R',
                         'A', 'M', 'E', 'T', 'E', 'R'))
            return RTSP_SET_PARAMETER ;
        else
            return RTSP_OTHER ;

    default:
        return RTSP_OTHER ;
    }
}


odt_rtsp_type_t
odt_rtsp_parse_type (const char *str, size_t len)
{
    odt_assert (NULL != str) ;

    switch (len) {

    case 4 :
        if (odt_str4cmp(str, 'R', 'T', 'S', 'p'))
            return RTSP_TYPE ;
        else if (odt_str4cmp(str, 'H', 'T', 'T', 'P'))
            return HTTP_TYPE ;
        break ;

    default :
        break ;
    } ;

    return OTHER_TYPE ;
}





int
odt_rtsp_parse_url(odt_rtsp_t *msg, const char *str, size_t len)
{
    odt_assert (NULL != msg) ;
    odt_assert (NULL != str) ;

    char *p, *l ;
    const char *tail ;
    int  state ;

    if (len <= 4 || !odt_str4cmp(str, 'r', 't', 's', 'p'))
        return S_ERROR ;

    for (p=l=str+4, tail=str+len, state=0; p<tail; ++p) {

        switch (state) {

        case 0 :
            if (*p != ':')
                return S_ERROR ;
            state = 1 ;
            break ;

        case 1 :
            if (*p != '/')
                return S_ERROR ;
            state = 2 ;
            break ;

        case 2 :
            if (*p != '/')
                return S_ERROR ;
            state = 3 ;
            break ;

        case 3 :
            if (*p == ':')
                return S_ERROR ;
            l = p ;
            state = 4 ;
            break ;

        case 4 :
            if (!odt_isletter(*p) && *p != '.' && *p != ':')
                return S_ERROR ;

            if (*p == ':') {

                odt_string_create_char(&msg->url_path, l, p-l) ;
                state = 5 ;
            }
            break ;

        case 5 :
            if (!isdigit(*p))
                return S_ERROR ;

            l = p ;
            state = 6 ;
            break ;

        case 6 :
            if (isdigit(*p))
                break ;

            if (*p != '/')
                return S_ERROR ;

            odt_string_create_char(&msg->url_port, l, p-l);
            return S_ERROR + 1;

        default:
            break ;
        }
    }

    return S_ERROR ;
}




int
odt_rtsp_parse_field(odt_rtsp_t *msg, const char *str, size_t len)
{
    odt_assert (NULL != msg) ;
    odt_assert (NULL != str) ;

    int state ;
    const char *key, *value, *p ;
    size_t key_len ;
    rtsp_fields_t *field ;

    for (state=0, p=key=value=str; p<str+len; ++p) {

        switch (state) {

        case 0 :     //Initialize
            if (odt_isletter(*p)) {

                key = p ;
                state = 1 ;
                break ;
            }

            return S_ERROR ;

        case 1 :
            if (isblank(*p)) {

                key_len = p - key ;
                state = 2 ;
            } else if (*p == ':') {

                key_len = p - key ;
                state = 3 ;
            } else if (!odt_isletter(*p) && *p != '-')
                return S_ERROR ;

            break ;

        case 2 :
            if (*p == ':')
                state = 3 ;
            else if (!isblank(*p))
                return S_ERROR ;

            break ;

        case 3 :
            if (!isblank(*p)) {

                value = p ;
                if (*p == '\r')
                    state = 5 ;
                else
                    state = 4 ;
            }

            break ;

        case 4 :
            if (*p=='\r')
                state = 5 ;

            break ;

        case 5 :
            if (*p!='\n') {

                state = 4 ;
                break ;
            }

            field = odt_malloc (sizeof(rtsp_fields_t)) ;
            odt_string_create_char (&field->key, key, key_len) ;
            odt_string_create_char (&field->value, value, p-1-value) ;

            if (0 == strncmp(RTSP_CONTENT_LENGTH, key, key_len))
                msg->content_length = odt_natoi(value, p-1-value);

            break ;

        default :
            break ;
        }
    }

    if (state < 5)
         return S_ERROR ;

    /* Insert into the rbtree */
    if (-1 == odt_rbtree_insert (msg->fields, field, &field->key)) {

        ODT_LOG_ERR("Insert node into rbtree failed") ;
        return -1 ;
    }

    /* Insert into the llist */
    list_add (&field->nob, &msg->fields_list) ;

    return S_ERROR+1 ;
}





int
odt_rtsp_append (odt_rtsp_t *msg, const char *str, int len)
{
    odt_assert (0 <= len) ;
    odt_assert (NULL != str) ;

    if (msg->msg.m_len == 0) {

        odt_string_create_char(&msg->msg, str, len) ;
        msg->last_point = 0 ;
    } else {

        if (-1 == odt_string_append_len(&msg->msg, str, len)) {

            ODT_LOG_ERR("Append string into tail of message failed") ;
            return -1 ;
        }
    }

    return 0 ;
}

int
odt_rtsp_parse (odt_rtsp_t *msg)
{
    odt_assert (NULL != msg) ;

    int ret ;

    if (msg->msg.m_len <= 4)
        return 0 ;

    if (msg->last_point == 0) {

        if (msg->msg.m_data[4] == '/') {

            msg->last_state = msg->state = S_RES_INIT ;
		    msg->req = 0 ;
	    } else {

	        msg->last_state = msg->state = S_REQ_INIT ;
	        msg->req = 1 ;
	    }
    }

    ret = _odt_rtsp_parse(msg) ;

    if (msg->state == S_ERROR) {

        ODT_LOG_ERR("Parse message failed, error point:\n%s",
                       msg->msg.m_data+msg->last_point) ;
        return -1 ;
    } else if (msg->state == S_COMPLETE) {

        odt_string_update (&msg->parsed_msg, msg->msg.m_data,
                           msg->last_point) ;
        msg->parse_done = 1 ;
    } else
        msg->parse_done = 0 ;

    return ret ;
}





int
_odt_rtsp_parse (odt_rtsp_t *msg)
{
    odt_assert (NULL != msg);

    char    *p, *tail, *l;
    int     state , last_state ;
    int     ret ;


    // Point to the next pos of the tail from last parsing
    p = msg->msg.m_data + msg->last_point ;


    l = msg->msg.m_data + msg->last_head ;
    tail = msg->msg.m_data + msg->msg.m_len ;
    state = msg->state ;

    for (; p < tail; ++p) {

        switch (state) {

        case S_VERB_B :
            if (!odt_isletter(*p)) {

                last_state = state;
                state = S_ERROR;
                break ;
            }

            state = S_VERB ;
            l = p ;
            break ;

        case S_VERB :
            if (!isblank(*p))
                break ;

            ret = odt_rtsp_parse_verb(l, p-l);
            if (ret == RTSP_OTHER) {
                last_state = state ;
                state = S_ERROR ;
            }

            odt_string_create_char(&msg->verb, l, p-l) ;
            msg->verbd = ret ;
            state = S_URL_B ;
            break ;

        case S_URL_B :
            if (isblank(*p))
                break ;

            l = p ;
            state = S_URL ;
            break ;

        case S_URL :
            if (!isblank(*p))
                break ;

            ret = odt_rtsp_parse_url(msg, l, p-l);
            if (S_ERROR == ret) {

                last_state = state ;
                state = S_ERROR ;
            } else
                state = S_TYPE_B ;

            break ;

        case S_TYPE_B :
            if (!odt_isletter(*p)) {

                last_state = state ;
                state = S_ERROR ;
                break ;
            }

            l = p ;
            state = S_TYPE ;
            break ;

        case S_TYPE :
            if (odt_isletter(*p))
                break ;

            odt_string_create_char(&msg->type, l, p-l) ;

            if (*p != '/'){

                last_state = state ;
                state = S_ERROR ;
            }

            state = S_VERSION_B ;
            break ;

        case S_VERSION_B :
            if (!isdigit(*p)) {

                last_state = state ;
                state = S_ERROR;
            }

            l = p ;
            state = S_VERSION ;
            break ;

        case S_VERSION :
            if (isdigit(*p) || *p == '.')
                break ;

            if (msg->req) {

                if (*p != '\r') {

                    last_state = state ;
                    state = S_ERROR ;
                    break ;
                }

                state = S_FIRST_RN ;
            } else {

                if (!isblank(*p)) {

                    last_state = state ;
                    state = S_ERROR ;
                    break;
                }

                state = S_CODE_B ;
            }

            odt_string_create_char(&msg->version, l, p-l) ;
            break ;

        case S_CODE_B :
            if (isblank(*p)) {

            } else if (!isdigit(*p)) {

                last_state = state ;
                state = S_ERROR ;
            } else {

                l = p ;
                state = S_CODE ;
            }

            break ;

        case S_CODE :
            if (isdigit(*p))
                break ;

            odt_string_create_char(&msg->code, l, p-l) ;
            state = S_DESC_B ;
            break ;

        case S_DESC_B :
            if (isblank(*p)) {

            } else if (!odt_isletter(*p)) {

                last_state = state ;
                state = S_ERROR ;
            } else {

                l = p ;
                state = S_DESC ;
            }

            break ;

        case S_DESC :
            if (*p != '\r')
                break ;

            odt_string_create_char(&msg->desc, l, p-l) ;
            state = S_FIRST_RN ;
            break ;

        case S_FIRST_RN :
            if (*p != '\n') {

                last_state = state ;
                state = S_ERROR ;
            } else
                state = S_HDR_B ;

            break ;

        case S_HDR_B :
            l = p ;

            if (!odt_isletter(*p)) {

                last_state = state ;
                state = S_ERROR ;
                break ;
            }

            state = S_HDR ;
            break ;

        case S_HDR :
            if (*p != '\r')
                break ;

            state = S_HDR_RN ;
            break ;

        case S_HDR_RN :
            if (*p == '\n') {

                ret = odt_rtsp_parse_field (msg, l, p-l+1) ;
                if (-1 == ret)
                    return -1 ;
                else if (S_ERROR == ret)
                    state = S_ERROR ;
                else
                    state = S_DOUBLE_RN ;
            } else
                state = S_HDR;

            break ;

        case S_DOUBLE_RN :
            if (*p == '\r')
                state = S_DOUBLE_RN_R ;
            else {

                l = p ;
                state = S_HDR ;
            }

            break ;

        case S_DOUBLE_RN_R :
            if (*p == '\n') {

                if (msg->content_length <= 0) {

                    p ++ ;      // Point to the next pos of the tail
                    state = S_COMPLETE ;
                } else
                    state = S_BODY_B ;
            } else {

                last_state = state ;
                state = S_ERROR ;
            }

            break ;

        case S_BODY_B :
           l = p ;
           if (l + msg->content_length <= tail)
               state = S_BODY ;
           else
               state = S_BODY_B_ ;

           break ;

        case S_BODY_B_ :
           if (l + msg->content_length <= tail) {

               state = S_BODY ;
               --p ;
           }
           break ;

        case S_BODY :
            odt_string_create_char(&msg->body, l, msg->content_length) ;
            p = l + msg->content_length ;
            state = S_COMPLETE ;
            break ;

        default :
            break ;
        }

        switch (state) {

        case S_ERROR :
        case S_COMPLETE :
            goto __out_loop ;

        default :
            break ;
        }
    }


__out_loop :
    msg->state = state ;
    ret = p - msg->msg.m_data - msg->last_point ;
    msg->last_head = l - msg->msg.m_data ;
    msg->last_point = p - msg->msg.m_data ;

    return ret ;
}



odt_string_t
*odt_rtsp_get_msg (odt_rtsp_t *msg)
{
    odt_assert (NULL != msg) ;

    odt_string_t *tmp ;
    rtsp_fields_t *pos ;


    if (msg->parsed_msg.m_len > 0)
        return &msg->parsed_msg ;

    if (msg->req) {

        /* Request line */

        odt_string_update (&msg->msg, msg->verb.m_data, msg->verb.m_len) ;

        odt_string_append (&msg->msg, " rtsp://") ;
        odt_string_append_len (&msg->msg, msg->url_path.m_data,
                               msg->url_path.m_len) ;
        odt_string_append (&msg->msg, "/:") ;
        odt_string_append_len (&msg->msg, msg->url_port.m_data,
                               msg->url_port.m_len) ;
        odt_string_append (&msg->msg, " ") ;
        odt_string_append_len (&msg->msg, msg->type.m_data, msg->type.m_len) ;
        odt_string_append (&msg->msg, "/") ;
        odt_string_append_len (&msg->msg, msg->version.m_data,
                               msg->version.m_len) ;
    } else {

        /* Statues line */

        odt_string_update (&msg->msg, msg->type.m_data, msg->type.m_len) ;
        odt_string_append (&msg->msg, "/") ;
        odt_string_append_len (&msg->msg, msg->version.m_data,
                               msg->version.m_len) ;
        odt_string_append (&msg->msg, " ") ;
        odt_string_append_len (&msg->msg, msg->code.m_data, msg->code.m_len) ;
        odt_string_append (&msg->msg, " ") ;
        odt_string_append_len (&msg->msg, msg->desc.m_data, msg->desc.m_len) ;
    }

    odt_string_append (&msg->msg, "\r\n") ;

    /* Headers */

    list_for_each_entry (pos, &msg->fields_list, nob) {

        tmp = &pos->key ;
        odt_string_append_len (&msg->msg, tmp->m_data, tmp->m_len) ;

        odt_string_append (&msg->msg, ": ") ;
        tmp = &pos->value ;
        odt_string_append_len (&msg->msg, tmp->m_data, tmp->m_len) ;

        odt_string_append (&msg->msg, "\r\n") ;
    }

    odt_string_append (&msg->msg, "\r\n") ;

    /* Body */

    if (msg->body.m_len)
        odt_string_append_len (&msg->msg, msg->body.m_data, msg->body.m_len) ;

    return &msg->msg ;
}




int
odt_rtsp_set_msg (odt_rtsp_t *msg, odt_string_t *str)
{
    odt_assert (NULL != msg) ;
    odt_assert (NULL != str) ;

    return odt_string_update (&msg->msg, str->m_data, str->m_len) ;
}



odt_string_t
*odt_rtsp_get_hdr (odt_rtsp_t *msg, odt_string_t *key)
{
    odt_assert (NULL != msg);
    odt_assert (NULL != key);

    odt_string_t *value ;
    odt_rbtree_node_t *node ;

    node = odt_rbtree_search (msg->fields, NULL, key) ;
    if (!node)
        return NULL ;


    return (odt_string_t *) &(((rtsp_fields_t *) node->m_elt)->value) ;
}



int
odt_rtsp_set_hdr (odt_rtsp_t *msg, odt_string_t *key, odt_string_t *value)
{
    odt_assert (NULL != msg);
    odt_assert (NULL != key);
    odt_assert (NULL != value);

    odt_rbtree_node_t *node ;
    rtsp_fields_t *field ;


    if (NULL != (node = odt_rbtree_search (msg->fields, NULL, key))) {

        field = (rtsp_fields_t *) node->m_elt ;
        odt_string_update (&field->value, value->m_data, value->m_len) ;
        ODT_LOG_WARN("Add fields failed, %20s is exist", key->m_data) ;
        return 0 ;
    }


    field = odt_malloc (sizeof(rtsp_fields_t)) ;
    odt_string_create_char (&field->key, key->m_data, key->m_len);
    odt_string_create_char (&field->value, value->m_data, value->m_len) ;

    if (-1 == odt_rbtree_insert(msg->fields, field, &field->key)) {

        ODT_LOG_ERR("Add fields failed") ;
        return -1 ;
    }

    list_add (&field->nob, &msg->fields_list) ;

    return 0 ;
}




odt_string_t
*odt_rtsp_get_body (odt_rtsp_t *msg)
{
    odt_assert (NULL != msg) ;

    return &msg->body ;
}



int
odt_rtsp_set_body (odt_rtsp_t *msg, odt_string_t *body)
{
    odt_assert (NULL != msg) ;
    odt_assert (NULL != body) ;

    odt_string_t key ;
    odt_rbtree_node_t *node ;
    rtsp_fields_t *field ;
    char tmp [8] ;


    key = odt_string (RTSP_CONTENT_LENGTH) ;
    node = odt_rbtree_search (msg->fields, NULL, &key) ;

    sprintf (tmp, "%d\0", body->m_len) ;

    if (node && msg->content_length != body->m_len) {

        field = (rtsp_fields_t *) node->m_elt ;
        odt_string_update (&field->value, tmp, strlen(tmp)) ;

    } else if (!node) {

        field = odt_malloc (sizeof(rtsp_fields_t)) ;
        odt_string_create_char (&field->key, RTSP_CONTENT_LENGTH,
                                sizeof(RTSP_CONTENT_LENGTH)-1) ;
        odt_string_create_char (&field->value, tmp, strlen(tmp)) ;

        if (-1 == odt_rbtree_insert (msg->fields, field, &field->key)) {

            ODT_LOG_ERR("Add fields failed") ;
            return -1 ;
        }

        list_add (&field->nob, &msg->fields_list) ;
    }

    /* If the body is not empty, clean it and free its memory */

    return odt_string_update (&msg->body, body->m_data, body->m_len) ;
}


int
odt_rtsp_set_verb (odt_rtsp_t *msg, odt_string_t *verb)
{
    if (!msg->req)
        return -1 ;

    return odt_string_update (&msg->verb, verb->m_data, verb->m_len) ;
}

inline odt_string_t
*odt_rtsp_get_verb (odt_rtsp_t *msg)
{
    return ( msg->req ? &msg->verb : NULL ) ;
}

int
odt_rtsp_set_url_path (odt_rtsp_t *msg, odt_string_t *path)
{
    if (!msg->req)
        return -1 ;

    return odt_string_update (&msg->url_path, path->m_data, path->m_len) ;
}

inline odt_string_t
*odt_rtsp_get_url_path (odt_rtsp_t *msg)
{
    return ( msg->req ? &msg->url_path : NULL );
}

inline int
odt_rtsp_set_url_port (odt_rtsp_t *msg, size_t port)
{
    if (!msg->req)
        return -1 ;

    char tmp [6] = {0} ;
    odt_string_t sport ;

    sprintf (tmp, "%u", port) ;
    sport = odt_nstring (tmp, strlen(tmp)) ;

    return odt_string_update (&msg->url_port, sport.m_data, sport.m_len) ;
}

inline int
odt_rtsp_get_url_port (odt_rtsp_t *msg)
{
    return ( msg->req ? odt_natoi(msg->url_port.m_data,
                            msg->url_port.m_len) : -1 );
}

inline int
odt_rtsp_set_type (odt_rtsp_t *msg, odt_string_t *type)
{
    return odt_string_update (&msg->type, type->m_data, type->m_len) ;
}

inline odt_string_t
*odt_rtsp_get_type (odt_rtsp_t *msg)
{
    return &msg->type ;
}

inline int
odt_rtsp_set_version (odt_rtsp_t *msg, odt_string_t *version)
{
    return odt_string_update (&msg->version, version->m_data, version->m_len) ;
}

inline odt_string_t
*odt_rtsp_get_version (odt_rtsp_t *msg)
{
    return &msg->version ;
}

inline int
odt_rtsp_set_code (odt_rtsp_t *msg, size_t code)
{
    if (msg->req)
        return -1 ;

    char tmp [6] = {0};
    odt_string_t scode ;

    sprintf (tmp, "%u", code) ;
    scode = odt_nstring (tmp, strlen(tmp)) ;

    return odt_string_update (&msg->code, scode.m_data, scode.m_len) ;
}

inline int
odt_rtsp_get_code (odt_rtsp_t *msg)
{
    return ( msg->req ? -1 : odt_natoi(msg->code.m_data,
                                 msg->code.m_len) );
}

inline int
odt_rtsp_set_desc (odt_rtsp_t *msg, odt_string_t *desc)
{
    if (msg->req)
        return -1 ;

    return odt_string_update (&msg->desc, desc->m_data, desc->m_len) ;
}

inline odt_string_t
*odt_rtsp_get_desc (odt_rtsp_t *msg)
{
    return ( msg->req ? NULL : &msg->desc ) ;
}
