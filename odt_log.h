
#ifndef __ODT_LOG_HEAD__
#define __ODT_LOG_HEAD__

#include "odt.h"


typedef struct {

    int    m_syslog_fd ;
    int    m_runlog_fd ;
    int    m_syslog_level ;
    int    m_runlog_level ;

    int    m_syslog_split_count ;
    size_t m_syslog_max_limitation ;
} odt_log_t ;

extern odt_log_t    odt_g_log ;

#define odt_syslog(level, ...)    \
    odt_syslog_core(&odt_g_log, level, __LINE__, __FILE__, __VA_ARGS__)

#define odt_runlog(level, ...)    \
    odt_runlog_core(level, __VA_ARGS__)


void odt_syslog_core (odt_log_t    *log,
                      unsigned int level,
                      int          line,
                      const char   *file,
                      const char   *fmt, ...) ;

int  odt_log_instart (const char *fmt, ...) ;

void odt_runlog_core (unsigned int level,
                      const char   *fmt, ...) ;


int odt_syslog_set_level (int level) ;
int odt_log_init () ;
int odt_syslog_set_limit (size_t limit) ;


#define ODT_SYSLOG_STDERR    0
#define ODT_SYSLOG_EMERG     1
#define ODT_SYSLOG_ALERT     2
#define ODT_SYSLOG_ERR       3
#define ODT_SYSLOG_WARN      4
#define ODT_SYSLOG_INFO      5
#define ODT_SYSLOG_DEBUG     6


#define ODT_LOG_STDERR(...)    \
    odt_syslog(ODT_SYSLOG_STDERR, __VA_ARGS__)
#define ODT_LOG_EMERG(...)    \
    odt_syslog(ODT_SYSLOG_EMERG, __VA_ARGS__)
#define ODT_LOG_ALERT(...)    \
    odt_syslog(ODT_SYSLOG_ALERT, __VA_ARGS__)
#define ODT_LOG_ERR(...)    \
    odt_syslog(ODT_SYSLOG_ERR, __VA_ARGS__)
#define ODT_LOG_WARN(...)    \
    odt_syslog(ODT_SYSLOG_WARN, __VA_ARGS__)
#define ODT_LOG_INFO(...)    \
    odt_syslog(ODT_SYSLOG_INFO, __VA_ARGS__)
#define ODT_LOG_DEBUG(...)    \
    odt_syslog(ODT_SYSLOG_DEBUG, __VA_ARGS__)


#define ODT_LOG_INSTART(...)    \
    odt_log_instart(__VA_ARGS__)




#define ODT_MAX_SYSLOG_SIZE    2048
#define ODT_SYSLOG_PATH        "log/syslog"
#define ODT_SYSLOG_DIR         "log"


#endif
