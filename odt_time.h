
#ifndef __ODT_CACHED_TIME_HEAD__
#define __ODT_CACHED_TIME_HEAD__

#include "odt.h"

typedef struct {

    unsigned             tv_sec ;

    // tv_msec tv_usec tv_nsec is three type of same value
    unsigned long int    tv_msec ;
    unsigned long int    tv_usec ;
    unsigned long int    tv_nsec ;
} odt_cached_time_t ;


// Global cycle time
extern odt_cached_time_t     odt_cached_cycle_time ;


inline void cycle_add2timeval (struct timeval *, long int) ;
inline void cycle_add2timespec (struct timespec *, long int) ;
inline unsigned long int CYCLE_SEC () ;
inline unsigned long int CYCLE_MSEC () ;
inline unsigned long int CYCLE_USEC () ;
inline unsigned long int CYCLE_NSEC () ;
int odt_update_cached_time () ;
int odt_cached_time_init () ;



#endif
