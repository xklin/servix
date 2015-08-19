
#ifndef __ODT_SESSION_HEAD__
#define __ODT_SESSION_HEAD__

#include "odt.h"


/*
 * The current version only support R2/C1 interface, so the session
 * data structures are designed for it.
 */


typedef struct {

    int               m_gid ;    // Group ID
    int               m_id ;     // ID

    odt_string_t      m_sid ;    // Session ID from server
    odt_string_t      m_odsid ;  // OnDemandSessionId
} odt_session_t ;



typedef struct odt_session_recv_interface_s odt_session_recv_interface_t;
struct odt_session_recv_interface_s {

    odt_callback_func    ack_cb ;
    odt_callback_func    announce_cb ;
    odt_callback_func    fail_cb ;
    void                *arg ;

    void               (*destroy) (odt_session_recv_interface_t *);
} ;




odt_session_t *odt_session_init (odt_session_t *session, int gid) ;
int odt_session_submit_task (odt_task_t *task) ;
void odt_session_destroy (odt_session_t *session) ;
void odt_session_rand_odsid (odt_session_t *) ;
int odt_session_create_id () ;



#endif
