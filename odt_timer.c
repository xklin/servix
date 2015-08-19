
#ifndef __ODT_TIMER_HEAD__

#include "odt.h"




/* ODTool Timer can support interval timer.
 * It's implemented by timer_fd and only be
 * supported in the kernel later than 2.6.27
 */



typedef struct {

    odt_event_t          ev ;
    odt_event_manager_t  *evm ;
    odt_callback_func    cb ;
    odt_callback_func    collect_arg_func ;
    void                 *arg ;
    int                  interval_flag ;
} timer_ctx_t ;



void
odt_ontimer (int what, void *arg)
{
    timer_ctx_t *ctx  ;
    uint64_t exp ;

    ctx = (timer_ctx_t *) arg ;

    if (what & (EPOLLERR|EPOLLHUP)) {

        ODT_LOG_ERR("Some error occured in timer[%p]", ctx);

        // Delete the timer to avoid report error repeatedly
        odt_deltimer (ctx) ;
        return ;
    }

    if (!ctx->evm) {

        ODT_LOG_ERR ("No event manager when timer triggled");
        return ;
    }

    // Read timerfd, setting status untriggled
    if (-1 == read (ctx->ev.m_fd, &exp, sizeof(uint64_t))) {

        ODT_LOG_EMERG("'read' timerfd failed, errno=%d", errno) ;
        odt_deltimer (ctx) ;
        return ;
    }


    if (ctx->cb)
        ctx->cb (what, ctx->arg) ;


    if (!ctx->interval_flag)
        odt_deltimer (ctx) ;


    ODT_LOG_DEBUG ("Timer triggered %d", ctx->ev.m_fd) ;

}




/* Settimer
 *
 * Oneshot mode : 'interval' is not positive value
 * Trigger interval mode : 'interval' is positive value
 */
unsigned long int
odt_settimer (odt_event_manager_t *evm,
              long int tv_ms,
              long int interval,
              odt_callback_func cb,
              odt_callback_func collect_arg_func,
              void *arg)
{
    odt_assert (NULL != evm) ;

    int timerfd ;
    timer_ctx_t *ctx ;
    struct itimerspec ispec ;
    struct timeval tv ;


    if (0 > tv_ms) {

        ODT_LOG_ERR ("Negative value for initial expiration of timer");
        return -1 ;
    }

    timerfd = timerfd_create (CLOCK_MONOTONIC, 0);
    if (-1 == timerfd) {

        ODT_LOG_EMERG("'timerfd_create' failed, errno=%d", errno) ;
        return -1 ;
    }


    ctx = odt_malloc(sizeof(timer_ctx_t)) ;
    *ctx = (timer_ctx_t) {
        .cb = cb,
        .collect_arg_func = collect_arg_func,
        .arg = arg,
        .evm = evm
    } ;
    odt_event_new (&ctx->ev, timerfd, odt_ontimer, ctx, NULL, NULL) ;


    cycle_add2timespec (&ispec.it_value, tv_ms) ;

    if (0 < interval) {

        ms2timespec (&ispec.it_interval, interval) ;
        ctx->interval_flag = 1 ;
    } else {

        ispec.it_interval = (struct timespec){0,0} ;
    }

	if (-1 == timerfd_settime (timerfd, TFD_TIMER_ABSTIME, &ispec, NULL)) {

        ODT_LOG_EMERG("'timerfd_settime' failed, errno=%d", errno) ;
        close (timerfd) ;
        odt_free (ctx);
		return -1 ;
	}


    if (-1 == odt_event_add(evm, &ctx->ev, ODT_EV_READ, -1)) {

        ODT_LOG_ERR ("Add event failed when set timer") ;
        close (timerfd) ;
        odt_free (ctx) ;
        return -1 ;
    }


    ODT_LOG_DEBUG ("Set timer success %d initial(%d)  interval(%d)",
                   timerfd, tv_ms, interval) ;
    /* Convert the ctx's pointer into integer and return it as timerd */
    return (unsigned long int) ctx ;
}



int
odt_deltimer (unsigned long int timerd)
{
    odt_assert (NULL != (timer_ctx_t *) timerd) ;

    timer_ctx_t *ctx = (timer_ctx_t *) timerd ;

    if (!ctx->evm) {

        ODT_LOG_ERR ("No event manager for given timer") ;
        return -1 ;
    }

    if (-1 == odt_event_del(ctx->evm, &ctx->ev)) {

        ODT_LOG_ERR("Delete event failed in timer[%d]", ctx->ev.m_fd);
        return -1 ;
    }

    close (ctx->ev.m_fd) ;
    odt_free (ctx) ;

    // Collect arguments
    if (ctx->collect_arg_func)
        ctx->collect_arg_func (0, ctx->arg) ;

    ODT_LOG_DEBUG ("Delete timer success %p", ctx) ;
    return 0 ;
}


#endif
