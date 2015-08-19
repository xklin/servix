
#include <sys/epoll.h>
#include <string.h>
#include <sys/timerfd.h>
#include <time.h>
#include "odt.h"



odt_event_manager_t *odt_g_evm ;
unsigned int odt_ev_ready_flag, odt_ev_ready_fail_flag ;


int odt_event_timer_key_cmp (void *, void *) ;
int odt_event_timers_tree_init (odt_rbtree_t **) ;
int odt_event_timers_tree_destroy (odt_rbtree_t *) ;
int odt_event_timers_tree_add (odt_event_manager_t *, odt_event_t *, long) ;
int odt_event_timers_tree_del (odt_event_manager_t *, odt_event_t *) ;
int odt_event_timerfd_set (odt_event_manager_t *) ;
int odt_event_timeout_callback (int, odt_event_manager_t *) ;





/* Timer key compare function */
int
odt_event_timer_key_cmp (void *key1, void *key2)
{
    struct timeval *k1 = (struct timeval *)key1 ;
    struct timeval *k2 = (struct timeval *)key2 ;

    if (k1->tv_sec > k2->tv_sec)
        return 1 ;
    else if (k1->tv_sec < k2->tv_sec)
        return -1 ;

    if (k1->tv_usec > k2->tv_usec)
        return 1 ;
    else if (k1->tv_usec < k2->tv_usec)
        return -1 ;
    else
        return 0 ;
}


/* Initialize timer */
int
odt_event_timers_tree_init (odt_rbtree_t **tree)
{
    *tree = odt_rbtree_create (sizeof(odt_event_t),
                        odt_event_timer_key_cmp) ;

    if (NULL == tree)
        return -1 ;

    return 0 ;
}


/* Destroy timer */
int
odt_event_timers_tree_destroy (odt_rbtree_t *tree)
{
    odt_rbtree_destroy (tree) ;
    return 0 ;
}


/* Add a timer */
int
odt_event_timers_tree_add (odt_event_manager_t *evm,
                            odt_event_t *ev,
                            long tv_ms)
{
    odt_assert (NULL != evm) ;
    odt_assert (NULL != ev) ;

    struct timeval *tv = &ev->m_tv ;


    /* The timeout value is added to current time value */
    cycle_add2timeval (tv, tv_ms) ;

    /* If all the events have the same timeout value in a cycle,
        then only one node will be in the tree, others will be
        linked after this node, so change the abs present time
        in a small rate */
    tv->tv_usec ++ ;


    if (-1 == odt_rbtree_insert (evm->m_timers_tree, ev, tv)) {

        ODT_LOG_ERR("Insert node into rbtree failed") ;
        return -1 ;
    }

    return 0 ;
}

/* Delete a timer */
int
odt_event_timers_tree_del (odt_event_manager_t *evm, odt_event_t *ev)
{
    odt_rbtree_node_t *pt ;


    /* Search the node from the timer tree */
    if (NULL == (pt = odt_rbtree_search (evm->m_timers_tree,
                    ev, &ev->m_tv))) {

        ODT_LOG_ERR("Search node from rbtree failed") ;
        return -1 ;
    }

    if (-1 == odt_rbtree_remove (evm->m_timers_tree, pt)) {

        ODT_LOG_ERR("Remove node from rbtree failed") ;
        return -1 ;
    }


    odt_free (pt) ;

    return 0 ;
}


/* Set timer */
int
odt_event_timerfd_set (odt_event_manager_t *evm)
{
    odt_assert (NULL != evm) ;

    odt_rbtree_node_t *pt ;
    struct itimerspec ispec ;
    struct timeval *tv ;
    struct epoll_event ep_ev ;
    int op ;


    /* Set the timer's value */

    pt = odt_rbtree_min (evm->m_timers_tree) ;

    /* The timers tree is empty */
    if (!pt)
        return 0 ;

    tv = (struct timeval *)pt->m_key ;
    ispec.it_interval = (struct timespec){0} ;
    ispec.it_value = (struct timespec) {tv->tv_sec, tv->tv_usec*1000} ;


    if (-1 == timerfd_settime (evm->m_timerfd,
                            TFD_TIMER_ABSTIME,
                            &ispec, NULL)) {

        ODT_LOG_EMERG("'timerfd_settime' failed, errno=%d", errno) ;
        return -1 ;
    }


    op = evm->m_timerfd_add ? EPOLL_CTL_MOD : EPOLL_CTL_ADD ;
    ep_ev.events = EPOLLIN | EPOLLONESHOT ;
    ep_ev.data.ptr = &evm->m_timerfd ;

    /* Enable the timer fd */
    if (-1 == epoll_ctl (evm->m_ep_fd, op, evm->m_timerfd, &ep_ev)) {

        ODT_LOG_EMERG("'epoll_ctl' failed, errno=%d", errno) ;
        return -1 ;
    }

    evm->m_timerfd_add = 1 ;

    return 0 ;
}



/* Call back function for timer fd */
int
odt_event_timeout_callback (int what, odt_event_manager_t *evm)
{
    odt_rbtree_node_t *pt ;
    odt_event_t *ev ;
    int exp ;

    if (what & (EPOLLERR | EPOLLHUP)) {

        ODT_LOG_ERR("Get EPOLLERR&EPOOLLHUP in callback function") ;
        return -1 ;
    }



    /* No timers are in the tree */
    if (NULL == (pt = odt_rbtree_min (evm->m_timers_tree)))
        goto read_timer_fd ;



/*
    // Check if the min event is timeout
    if (0 > odt_event_timer_key_cmp (pt->m_key, &evm->m_timerfd_tv))
        goto read_timer_fd ;
*/


    while (pt) {
        /* Call timeout call back functions for the events that are timeout */

        if (NULL == (ev = pt->m_elt) || !ev->m_add) {
            /* Timeout call back function is empty */
        }else {
            if (!ev->cb_timeout)
                return 0 ;
            ev->cb_timeout (what, ev->cb_timeout_arg) ;
        }

        pt = pt->sib ;
    }

read_timer_fd :

    /* Read the timerfd, let the event in epoll be untriggled */
    if (-1 == read (evm->m_timerfd, &exp, sizeof(uint64_t))) {

        ODT_LOG_ERR("'read' timerfd failed, errno=%d", errno) ;
        return -1 ;
    }

    return 0 ;
}




int
odt_event_post (odt_event_manager_t *evm,
                odt_event_t         *ev,
                int                  events)
{
    odt_assert (NULL != evm) ;
    odt_assert (NULL != ev);

    ev->m_events = events ;
    list_add_tail (&ev->m_ready_nob, &evm->m_ready_list) ;
    return 0 ;
}




odt_event_manager_t *
odt_event_manager_init ()
{
    odt_event_manager_t *evm ;
    struct epoll_event ep_ev ;


    evm = odt_malloc (sizeof(odt_event_manager_t)) ;

    /* Create a epoll fd */
    evm->m_ep_fd = epoll_create (1) ;
    if (-1 == evm->m_ep_fd) {
        /* Print log */
        return NULL ;
    }

    /* Initialize the timer fd */
    evm->m_timerfd = timerfd_create (CLOCK_MONOTONIC, 0) ;
    if (-1 == evm->m_timerfd) {

        ODT_LOG_EMERG("'timerfd_create' failed, errno=%d",errno) ;
        return NULL ;
    }

    evm->m_timerfd_add = 0 ;


    /* Initialize the timers */
    if (-1 == odt_event_timers_tree_init (&evm->m_timers_tree)) {

        ODT_LOG_EMERG("Initialize timer tree failed") ;
        return NULL ;
    }


    /* Initialize the ready list */
    INIT_LIST_HEAD (&evm->m_ready_list) ;

    evm->m_ev_n = 0 ;



    /* Initialize 'odt_ev_ready_flag' and 'odt_ev_ready_fail_flag' */
    unsigned int i, n, m, flag ;
    for (n = 0, i = 0, flag = ODT_EPOLL_ALL, m = 0x1;
         n < 32; ++n, m <<= 1) {

        if (flag & m)
            continue ;

        if (i) {

            odt_ev_ready_fail_flag = m ;
            break ;
        } else {

            odt_ev_ready_flag = m ;
            ++ i;
        }
    }

    if (!(odt_ev_ready_fail_flag && odt_ev_ready_flag)) {

        ODT_LOG_ALERT ("No avaliable event flag, so exit");
        exit (1) ;
    }
    
    return evm ;
}




/* New a event */
odt_event_t *
odt_event_new (odt_event_t *ev,
                int fd,
                odt_callback_func cb,
                void *cb_arg,
                odt_callback_func timeo_cb,
                void *timeo_cb_arg)
{
    if (!ev)
        ev = odt_malloc (sizeof(odt_event_t)) ;

    *ev = (odt_event_t) {

        .m_fd = fd,
        .cb = cb,
        .cb_arg = cb_arg,
        .cb_timeout = timeo_cb,
        .cb_timeout_arg = timeo_cb_arg
    } ;

    INIT_LIST_HEAD (&ev->m_ready_nob) ;

    return ev ;
}




int
odt_event_manager_handle_post (odt_event_manager_t *evm)
{
    odt_assert (NULL != evm) ;

    odt_event_t *ev , *ev_next;
    int events ;

    list_for_each_entry (ev, &evm->m_ready_list, m_ready_nob) {

        if (ev->cb && (ev->m_events & ODT_EV_READY))
            ev->cb (ev->m_events, ev->cb_arg) ;
        else if (ev->cb_timeout && (ev->m_events & ODT_EV_READY_FAIL))
            ev->cb_timeout (ev->m_events, ev->cb_timeout_arg) ;
    }

    // Delete all the node
    INIT_LIST_HEAD (&evm->m_ready_list) ;
}




/* User should fill the event by itself */

int
odt_event_add (odt_event_manager_t *evm,
                odt_event_t *ev,
                int what,
                long tv_ms)
{
    odt_assert (NULL != ev) ;
    odt_assert (NULL != evm) ;

    struct epoll_event ep_ev ;
    int op ;


    ev->m_events = 0 ;

    /* Set the triggle object */
    if (what & ODT_EV_READ)
        ev->m_events |= EPOLLIN ;

    if (what & ODT_EV_WRITE)
        ev->m_events |= EPOLLOUT ;



    if (ev->m_add && !ev->m_infinite) {
        /* Delete the timer */
        if (-1 == odt_event_timers_tree_del (evm, ev)) {

            ODT_LOG_ERR ("Delete timer failed\n") ;
            return -1 ;
        }
    }

    /* Compute the timeout value */
    if (tv_ms >= 0) {

        ev->m_infinite = 0 ;
        if (-1 == odt_event_timers_tree_add (evm, ev, tv_ms)) {

            ODT_LOG_ERR ("Add timer failed\n") ;
            return -1 ;
        }    
    }else{

        ev->m_infinite = 1 ;
    }


    if (ev->m_add)
        op = EPOLL_CTL_MOD ;
    else
        op = EPOLL_CTL_ADD ;

    /* Add the fd into the epoll list */

    ep_ev.data.ptr = ev ;
    ep_ev.events = ev->m_events ;

    if (-1 == epoll_ctl (evm->m_ep_fd, op, ev->m_fd, &ep_ev)) {

        ODT_LOG_ERR ("'epoll_ctl' failed, errno=%d", errno) ;
        return -1 ;
    }


    /* If the event is new to added, increase the number of events */
    if (!ev->m_add)
        evm->m_ev_n ++ ;

    ev->m_add = 1 ;

    ODT_LOG_DEBUG ("[event %p] added now : %d", ev, evm->m_ev_n);

    return 0 ;
}





int
odt_event_del (odt_event_manager_t *evm, odt_event_t *ev)
{
    odt_assert (NULL != ev) ;
    odt_assert (NULL != evm) ;

    if (!ev->m_add) {

        ODT_LOG_DEBUG ("[event %p] is deleted", ev);
        /* Has had been deleted */
        return 0 ;
    }


    if (!ev->m_infinite) {

        /* Delete the timer */
        if (-1 == odt_event_timers_tree_del (evm, ev)) {

            ODT_LOG_ERR("Delete the event timer failed") ;
            return -1 ;
        }
    }

    if (-1 == epoll_ctl (evm->m_ep_fd, EPOLL_CTL_DEL,
                    ev->m_fd, NULL)) {

        ODT_LOG_EMERG("'epoll_ctl' failed, errno=%d", errno) ;
        return -1 ;
    }

    ev->m_add = 0 ;
    evm->m_ev_n -- ;

    ODT_LOG_DEBUG ("[event %p] deleted now : %d", ev, evm->m_ev_n) ;

    return 0 ;
}




int
odt_event_wait (odt_event_manager_t *evm)
{
    odt_assert (NULL != evm) ;

    struct epoll_event ep_list[EPOLL_LIST_SIZE] ;
    struct epoll_event ep_ev ;
    odt_event_t *ev ;
    int nready ;
    int timerfd_idx ;
    int events ;
    int eplistsz ;

 
    /* Deal with ready task posted */

    odt_event_manager_handle_post (evm) ;


   
    /* Set the event timer */

    if(-1 == odt_event_timerfd_set (evm))
        return -1 ;


    timerfd_idx = -1 ;


    eplistsz = (evm->m_ev_n > EPOLL_LIST_SIZE) ?
                EPOLL_LIST_SIZE :
                evm->m_ev_n ;
    if (!eplistsz)
        // Happen after handled all posted task which is to
        // delete events from manager
        return 0 ;

    /* Wait epoll */

    while (1) {
        /* Deal with EINTR */

        nready = epoll_wait (evm->m_ep_fd, ep_list, eplistsz, -1) ;

        if (0 > nready && errno == EINTR) {

            continue ;
        } else if (0 > nready) {

            ODT_LOG_EMERG ("'epoll_wait' failed, errno = %d", errno);
            return -1 ;
        }else {

            break ;
        }
    }


    /* Update cached time */
    if (-1 == odt_update_cached_time ())
        ODT_LOG_ERR ("Update cached time failed") ;


    /* Do call back functions */

    int i ;

    for (i=0; i<nready; ++i) {

        /* If timerfd, set the ready flag */
        if (ep_list[i].data.ptr == &evm->m_timerfd) {
            timerfd_idx = i ;
            continue ;
        }

        ev = (odt_event_t *)ep_list[i].data.ptr ;

        /* Check if the event has been deleted */
        if (!ev->m_add)
            continue ;


        /* Call the function */
        ev->cb (ep_list[i].events, ev->cb_arg) ;
    }

    /* Deal with timer fd */
    if (0 <= timerfd_idx) {
        int what = ep_list [timerfd_idx].events ;

        if (-1 == odt_event_timeout_callback (what, evm)) {
            /* Print log */
            return -1 ;
        }
    }

    return 0 ;
}


int
odt_event_manager_destroy (odt_event_manager_t *evm)
{
    close (evm->m_ep_fd) ;
    close (evm->m_timerfd) ;

    if (-1 == odt_event_timers_tree_destroy (evm->m_timers_tree)) {
        /* Print log */
        return -1 ;
    }

    return 0 ;
}
