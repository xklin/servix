
#ifndef __VSS_ODT_HEAD__
#define __VSS_ODT_HEAD__

#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/timerfd.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "list.h"


void *odt_malloc (size_t size) ;
void odt_free (void *dst) ;


#define odt_assert(expr)\
	assert (expr)



typedef void (*odt_callback_func) (int, void *) ;


#ifndef INT_MAX
#define INT_MAX    ((~0x0) ^ (0x1 << 31))
#endif


#define EPOLL_LIST_SIZE                    1024

#define KB                                 (1024)
#define MB                                 (1024*KB)
#define GB                                 (1024*MB)
#define ODT_SYSLOG_DEF_MAX_LIMIT           (1*MB)


#define ODT_SESSION_ODSID_SLEN             10
#define ODT_STRING_MINSIZE_LIMIT           16


/* Log and config is standing off, so its type is undefined,
   they will be defined when used */
typedef int odt_config_t ;


#include "odt_string.h"
#include "odt_log.h"
#include "odt_rbtree.h"
#include "odt_event.h"
#include "odt_connect.h"
#include "odt_protocol_parser.h"
#include "odt_csv.h"
#include "odt_task.h"
#include "odt_session.h"
#include "odt_time.h"




// Global configuration
extern odt_config_t          odt_g_config ;
// Global log time
extern odt_string_t          odt_cached_log_time ;
// Global event manager
extern odt_event_manager_t  *odt_g_evm ;
// Global lua state
extern lua_State            *odt_g_lua_state ;




#define odt_isletter(ch)                                                       \
    ((ch >= 'A') && (ch <= 'Z') ||                                             \
     (ch >= 'a') && (ch <= 'z') ||                                             \
     (ch >= '0') && (ch <= '9') ||                                             \
     (ch == '_'))





#define odt_rand()       rand()








#endif
