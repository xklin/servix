
#include "odt.h"




int
odt_session_recv_msg_init (odt_msg_parser *parser,
                           void           *arg)
{
    odt_rtsp_t *msg ;
    parser->arg = arg ;

    msg = odt_malloc (sizeof(odt_rtsp_t)) ;
    if (0 > odt_rtsp_init (msg)) {

        ODT_LOG_ERR ("Initialize 'msg' for parser failed") ;
        return -1 ;
    }

    parser->msg = msg ;
    return 0 ;
}


int
odt_session_recv_msg_append (odt_msg_parser *parser,
                             const char     *str, 
                             int             len)
{
    odt_assert (NULL != parser) ;
    odt_assert (NULL != str) ;
    odt_assert (0 <= len) ;

    odt_rtsp_t *msg = (odt_rtsp_t *) parser->msg ;
    return odt_rtsp_append (msg, str, len) ;
}



int
odt_session_recv_msg_parse (odt_msg_parser *parser)
{
    odt_rtsp_t *msg ;
    odt_session_recv_interface_t *inte ;

    msg = (odt_rtsp_t *) parser->msg ;
    inte = (odt_session_recv_interface_t *) parser->arg ;

    do {

        if (-1 == odt_rtsp_parse (msg)) {

            ODT_LOG_ERR("Parse rtsp message failed") ;
            return -1 ;
        }

        if (!msg->parse_done) {

            break ;
        } else {

            if (msg->req) {

                if (msg->verbd == RTSP_ANNOUNCE) {

                    if (inte->announce_cb)
                        inte->announce_cb (ODT_EV_READ, inte->arg) ;
                } else {

                    ODT_LOG_ERR("Invalid verb of the message recieved") ;
                }
            } else if (inte->ack_cb) {

                inte->ack_cb (ODT_EV_READ, inte->arg) ;
            }

            ODT_LOG_DEBUG ("Recieved and parsed message complete, now "
                           "destroy the buffer") ;

            odt_rtsp_cleanup (msg) ;
        }
    } while (msg->msg.m_len > 0) ;

    return 0 ;
}



void
odt_session_recv_msg_fail (odt_msg_parser *parser)
{
    odt_rtsp_t *msg ;
    odt_session_recv_interface_t *inte ;

    msg = (odt_rtsp_t *) parser->msg ;
    inte = (odt_session_recv_interface_t *) parser->arg ;

    odt_rtsp_cleanup (msg) ;

    if (inte->fail_cb)
        inte->fail_cb (0, inte->arg) ;
}


void
odt_session_recv_msg_destroy (odt_msg_parser *parser)
{
    odt_session_recv_interface_t *inte ;

    odt_rtsp_destroy ((odt_rtsp_t *) parser->msg) ;

    inte = (odt_session_recv_interface_t *) parser->arg ;
    inte->destroy (inte) ;
}



odt_session_t
*odt_session_init (odt_session_t *session, int gid)
{
    int id ;
    if (!session)
        session = odt_malloc (sizeof(odt_session_t)) ;

    id = odt_session_create_id();

    *session = (odt_session_t) {

        .m_gid = gid,
        .m_id = id,
    } ;

    ODT_LOG_INFO ("ID[%d] Session initialized", session->m_id) ;

    return session ;
}



int
odt_session_submit_task (odt_task_t *task)
{
    odt_assert (NULL != task) ;

    task->parser = (odt_msg_parser) {

        .append = odt_session_recv_msg_append,
        .parse = odt_session_recv_msg_parse,
        .init = odt_session_recv_msg_init,
        .destroy = odt_session_recv_msg_destroy,
        .handle_parse_fail = odt_session_recv_msg_fail,
    } ;

    return 0 ;
}



void
odt_session_destroy (odt_session_t *session)
{
    ODT_LOG_INFO ("[%d][%s] Session destroyed",
               session->m_id, session->m_sid.m_data) ;
}




int
odt_session_create_id ()
{
    static int i = 0 ;

    return 1000000 - (++i) ;
}

void
odt_session_rand_odsid (odt_session_t *session)
{
    int n , m ;
    char a [ODT_SESSION_ODSID_SLEN] ;
    for (n=0; n<ODT_SESSION_ODSID_SLEN; ++n) {

        m = odt_rand()%62 ;
        if (m > 9 && m <= 35)
            m = (m - 10) + 'a' ;
        else if (m > 35)
            m = (m - 36) + 'A' ;
        else
            m += '0' ;
        a[n] = m ;
    }

    odt_string_create_char (&session->m_odsid, a, ODT_SESSION_ODSID_SLEN);
}
