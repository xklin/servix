

#include "odt.h"
#include <signal.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>


/* Time for log */
odt_string_t       odt_cached_log_time ;
odt_cached_time_t  odt_cached_cycle_time ;



void
cycle_add2timeval (struct timeval *dst, long int add_ms)
{
    unsigned long sec_part = add_ms/1000 ;
    dst->tv_sec = sec_part + odt_cached_cycle_time.tv_sec ;
    dst->tv_usec = (add_ms - sec_part*1000)*1000 +
                    odt_cached_cycle_time.tv_usec ;
    dst->tv_sec += dst->tv_usec/1000000 ;
    dst->tv_usec = dst->tv_usec%1000000 ;
}



void
cycle_add2timespec (struct timespec *dst, long int add_ms)
{
    unsigned long sec_part = add_ms/1000 ;
    dst->tv_sec = sec_part + odt_cached_cycle_time.tv_sec ;
    dst->tv_nsec = (add_ms - sec_part*1000)*1000*1000 +
                    odt_cached_cycle_time.tv_nsec ;
    dst->tv_sec += dst->tv_nsec/1000000000 ;
    dst->tv_nsec = dst->tv_nsec%1000000000 ;
}


inline unsigned long int
CYCLE_SEC ()
{
    return odt_cached_cycle_time.tv_sec ;
}


inline unsigned long int
CYCLE_MSEC ()
{
    return odt_cached_cycle_time.tv_msec ;
}


inline unsigned long int
CYCLE_USEC ()
{
    return odt_cached_cycle_time.tv_usec ;
}


inline unsigned long int
CYCLE_NSEC ()
{
    return odt_cached_cycle_time.tv_nsec ;
}


inline void
cycle2timeval (struct timeval *tv)
{
    *tv = (struct timeval) {odt_cached_cycle_time.tv_sec,
                            odt_cached_cycle_time.tv_usec} ;
}


inline void
ms2timeval (struct timeval *dst, long int ms)
{
    dst->tv_sec = ms/1000 ;
    dst->tv_usec = (ms%1000)*1000 ;
}


inline void
ms2timespec (struct timespec *dst, long int ms)
{
    dst->tv_sec = ms/1000 ;
    dst->tv_nsec = (ms%1000)*1000*1000 ;
}



/* The time format for log is :
 *     Year/Month/Day Hour:minute:second milisecond
 */
int
odt_cached_log_time_set (struct timeval *spec, struct tm *tm)
{
    odt_assert (NULL != spec) ;
    odt_assert (NULL != tm) ;

    char    str[64] ;
    ssize_t n ;

    n = sprintf (str, "%4d/%02d/%02d %02d:%02d:%02d %03d",
             tm->tm_year + 1900,
             tm->tm_mon + 1,
             tm->tm_mday,
             tm->tm_hour,
             tm->tm_min,
             tm->tm_sec,
             spec->tv_usec/1000) ;


    if (-1 == odt_string_update (&odt_cached_log_time, str, n)) {

        ODT_LOG_ERR("Update cached log time string failed") ;
        return -1 ;
    }

    return 0 ;
}



/* Update strategy :
 *     Do it after each loop. In a other word, this function will
 *     be called after 'epoll_wait' returned
 */
int
odt_update_cached_time ()
{
    struct timespec tspec ;
    struct timeval  tval ;
    time_t          tv ;
    struct tm       *tm ;

    /* Update the cycle time */

    if (-1 == clock_gettime (CLOCK_MONOTONIC, &tspec)) {

        ODT_LOG_EMERG("'clock_gettime' failed, errno=%d", errno) ;
        return -1 ;
    }

    odt_cached_cycle_time.tv_sec = tspec.tv_sec ;
    odt_cached_cycle_time.tv_nsec = tspec.tv_nsec ;
    odt_cached_cycle_time.tv_usec = tspec.tv_nsec/1000 ;
    odt_cached_cycle_time.tv_msec = tspec.tv_nsec/1000000 ;


    /* Update the log time */
    if (-1 == gettimeofday (&tval, NULL)) {

        ODT_LOG_EMERG("'gettimeofday' failed, errno=%d", errno) ;
        return -1 ;
    }

    tv = time (NULL) ;
    if (-1 == tv) {

        ODT_LOG_EMERG("'time' failed, errno=%d", errno) ;
        return -1 ;
    }

    tm = localtime (&tv) ;
    if (NULL == tm) {

        ODT_LOG_EMERG("'gmtime' failed, errno=%d", errno) ;
        return -1 ;
    }

    if (-1 == odt_cached_log_time_set(&tval, tm)) {

        ODT_LOG_ERR("Set cached log time failed") ;
        return -1 ;
    }

    return 0 ;
}


int
odt_cached_time_init ()
{
    struct sigaction sigac ;
    struct itimerval itv ;


    odt_cached_log_time = odt_null_string ;

    /* Initialize the cached time */
    if (-1 == odt_update_cached_time ()) {

        ODT_LOG_ERR ("Update cached time failed when initializing") ;
        return -1 ;
    }


#ifdef SIG_FOR_CACHED_TIME
    /* Set the signal */
    sigac.sa_handler = odt_update_cached_time ;
    sigemptyset (&sigac.sa_mask) ;
    sigac.sa_flags = 0 ;

    if (-1 == sigaction(SIGALRM, &sigac, NULL)) {

        ODT_LOG_EMERG("'sigaction' failed, errno=%d", errno) ;
        return -1 ;
    }

    /* Set timer */
    itv.it_value = (struct timeval) ODT_CACHED_TIME_UPDATE_INTERVAL ;
    itv.it_interval = (struct timeval) ODT_CACHED_TIME_UPDATE_INTERVAL ;

    if (-1 == setitimer(ITIMER_REAL, &itv, NULL)) {

        ODT_LOG_EMERG("'setitimer' failed, errno=%d", errno) ;
        return -1 ;
    }
#endif

    return 0 ;
}
