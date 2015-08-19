
#ifndef __ODT_EVENT_H__
#define __ODT_EVENT_H__

#include "odt.h"



#define ODT_EV_READ           EPOLLIN
#define ODT_EV_WRITE          EPOLLOUT
#define ODT_EV_ERR            (EPOLLHUP|EPOLLERR)
#define ODT_EV_RDWR           (EPOLLIN|EPOLLOUT)
#define ODT_EPOLL_ALL         (EPOLLIN|\
                               EPOLLOUT|\
                               EPOLLHUP|\
                               EPOLLERR|\
                               EPOLLET|\
                               EPOLLONESHOT|\
                               EPOLLRDHUP|\
                               EPOLLPRI)

// Custom flag
#define ODT_EV_READY          (odt_ev_ready_flag)
#define ODT_EV_READY_FAIL     (odt_ev_ready_fail_flag)


extern unsigned int odt_ev_ready_flag, odt_ev_ready_fail_flag ;



typedef struct {

    short                m_add:1 ;
    short                m_infinite:1 ;

    int                  m_events ;
    struct timeval       m_tv ;

    int                  m_fd ;
    odt_callback_func    cb ;
    void                *cb_arg ;
    odt_callback_func    cb_timeout ;
    void                *cb_timeout_arg ;

    struct list_head     m_ready_nob ;
} odt_event_t ;


typedef struct {
    int                 m_timerfd ;
    int                 m_timerfd_add ;
    int                 m_ep_fd ;
    odt_rbtree_t       *m_timers_tree ;
    int                 m_ev_n ;
    struct list_head    m_ready_list ;
} odt_event_manager_t ;






/* Get a event manager */
odt_event_manager_t *odt_event_manager_init () ;
#define ODT_EVENT_MANAGER_DECLARE    (odt_event_manager_t){0} ;




odt_event_t *odt_event_new (odt_event_t *ev,
                    int fd,
                    odt_callback_func cb,
                    void *cb_arg ,
                    odt_callback_func cb_timeout,
                    void *cb_timeout_arg) ;


/* Register an event */
int odt_event_add (odt_event_manager_t *evm,
                odt_event_t *event,
                int what,
                long tv_ms) ;


/* Delete an event */
int odt_event_del (odt_event_manager_t *evm, odt_event_t *event) ;



/* Wait for events */
int odt_event_wait (odt_event_manager_t *evm) ;


/* Post ready event. That is to say, if a task using callback
 * strategy but needn't to wait an event, it will post doing
 * its callback functions
 */
int odt_event_post (odt_event_manager_t *evm, odt_event_t *ev, int events) ;




/* Clear the global EVENT_MANAGER */
int odt_event_manager_destroy (odt_event_manager_t *evm) ;


#endif
