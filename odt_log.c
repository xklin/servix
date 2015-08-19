
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>

#include "odt_log.h"
#include "odt_string.h"


extern odt_string_t    odt_cached_log_time ;
odt_log_t              odt_g_log ;


void
odt_runlog_core(unsigned int level, const char *fmt, ...)
{

}




const odt_string_t err_level[] = {
    odt_string("PANIC"),
    odt_string("EMERG"),
    odt_string("ALERT"),
    odt_string("ERROR"),
    odt_string("WARN"),
    odt_string("INFO"),
    odt_string("DEBUG")
} ;




#define odt_syslog_level_filte(G, l)  ( (G)->m_syslog_level >= l )
#define odt_runlog_level_filte(G, l)  ( (G)->m_runlog_level >= l )


int
odt_syslog_split_file (odt_log_t *log)
{
    int n ;
    char old [256] = {ODT_SYSLOG_PATH} ;
    char new [256] = {ODT_SYSLOG_PATH} ;


    log->m_syslog_split_count ++ ;
    close (log->m_syslog_fd) ;

    for (n = log->m_syslog_split_count; n > 0; --n) {

        if (n > 1)
            sprintf (old + sizeof(ODT_SYSLOG_PATH) - 1, "%d", n-1) ;
        else
            strcpy (old, ODT_SYSLOG_PATH);

        sprintf (new + sizeof(ODT_SYSLOG_PATH) - 1, "%d", n) ;

        if (0 > rename (old, new)) {

            printf ("'rename' log file failed, errno=%d", errno) ;
            return -1 ;
        }
    }

    if (0 > (log->m_syslog_fd = open (ODT_SYSLOG_PATH, O_WRONLY|
             O_APPEND|O_CREAT, 0644))) {

        printf ("'open' log file failed, errno=%d", errno) ;
        return -1 ;
    }

    return 0 ;
}


int
odt_syslog_write (const u_char data[], unsigned int len, odt_log_t *log)
{
    odt_assert (NULL != data) ;    
    odt_assert (0 <= len) ;


    int ret, towrite, file_cap, written;
    struct stat statbuf ;


    if (0 > fstat (log->m_syslog_fd, &statbuf)) {

        printf ("'fstat' failed, errno=%d", errno);
        return -1 ;
    }

    file_cap = log->m_syslog_max_limitation - statbuf.st_size ;


    /* Why doesn't I update file capacity in the circle? Because maximum
       size limitation of a log file must be at least 1MB. I don't think
       a log can be over 1MB, so if a log is over the current capacity,
       I simply split the log file and write the rest contents to next file */
    for (towrite = len > file_cap ? file_cap : len
         ;
         ; len=towrite) {

        for (written=0; towrite > written;) {

            ret = write (log->m_syslog_fd, data, towrite) ;
            if (0 > ret && errno != EINTR) {

                printf ("'write' failed, errno=%d", errno) ;
                return -1 ;
            } else {

                written += ret ;
            }
        }

        if (towrite = len - towrite) {

            if (0 > odt_syslog_split_file (log)) {

                printf ("Split syslog file failed") ;
                return -1 ;
            }
        } else {

            break ;
        }
    }

    return 0 ;
}


void
odt_syslog_core(odt_log_t    *log,
                unsigned int level,
                int          line,
                const char   *file,
                const char   *fmt, ...)
{
    odt_assert (NULL != log) ;

    va_list    ap ;
    u_char     *p, *last ;
    u_char     errstr[ODT_MAX_SYSLOG_SIZE];
    ssize_t    n ;



    if (!odt_syslog_level_filte (&odt_g_log, level))
        return ;


    last = errstr + ODT_MAX_SYSLOG_SIZE ;


    if (odt_cached_log_time.m_len <= 0) {

        strcpy (errstr, "NO_TIMESTAMP") ;
        p = errstr + sizeof("NO_TIMESTAMP") - 1 ;
    } else {

        memcpy(errstr, odt_cached_log_time.m_data, odt_cached_log_time.m_len);
        p = errstr + odt_cached_log_time.m_len ;
    }

    sprintf (p, " %s ", err_level[level].m_data) ;
    p += err_level[level].m_len + 2 ;      // Add two ' ' space

    va_start (ap, fmt);
    n = vsnprintf (p, last-p, fmt, ap) ;
    va_end (ap) ;

    p += n ;

    n = snprintf (p, last-p, " [%s:%d]\n", file, line) ;
    p += n ;


    if (0 > odt_syslog_write (errstr, p-errstr, log))
        odt_assert ("write syslog failed" && 0) ;

    if (level == ODT_SYSLOG_STDERR) {

        fwrite ("ODTool: ", 1, sizeof("ODTool: ")-1, stderr) ;
        fwrite (errstr, sizeof(u_char), p-errstr, stderr) ;
    }
}



int
odt_log_instart (const char *fmt, ...)
{
    char tmp [ODT_MAX_SYSLOG_SIZE] ;
    va_list ap ;
    int n ;

    va_start (ap, fmt) ;
    n = vsnprintf (tmp, ODT_MAX_SYSLOG_SIZE, fmt, ap) ;
    va_end (ap) ;

    fwrite (tmp, 1, n, stderr) ;

    return 0 ;
}






int
odt_log_init ()
{
    struct stat statbuf ;
    int dirflag ;
    int ret ;
    int fd ;

    ret = stat (ODT_SYSLOG_DIR, &statbuf) ;
    if (-1 == ret || ENOENT == errno) {

        dirflag = S_IRWXU | S_IRGRP ;
        if (-1 == mkdir (ODT_SYSLOG_DIR, dirflag)) {

            ODT_LOG_INSTART ("'mkdir' log directory failed, errno=%d", errno) ;
            return -1 ;
        }

    } else if (-1 != ret && !S_ISDIR(statbuf.st_mode)) {

        ODT_LOG_INSTART ("A file named 'log' exist") ;
        return -1 ;
    }


    fd = open (ODT_SYSLOG_PATH, O_WRONLY|O_APPEND|O_EXCL|O_CREAT, 0644) ;
    if (-1 == fd && EEXIST != errno) {

        ODT_LOG_INSTART ("'open' log file failed, errno=%d", errno) ;
        return -1 ;

    } else if (-1 == fd) {

        fd = open (ODT_SYSLOG_PATH, O_WRONLY|O_APPEND) ;
        if (-1 == fd) {

            ODT_LOG_INSTART ("'open' log file failed, errno=%d",errno) ;
            return -1 ;
        }
    }

    odt_g_log = (odt_log_t) {

        .m_syslog_fd = fd,
        .m_syslog_level = ODT_SYSLOG_DEBUG,
        .m_syslog_split_count = 0,
        .m_syslog_max_limitation = ODT_SYSLOG_DEF_MAX_LIMIT,
    } ;

    return 0 ;
}



void
odt_log_exit (odt_log_t *log)
{
    close (log->m_syslog_fd) ;
}



int odt_syslog_set_level (int level)
{
    switch (level) {

    case ODT_SYSLOG_STDERR :
    case ODT_SYSLOG_EMERG :
    case ODT_SYSLOG_ALERT :
    case ODT_SYSLOG_ERR :
    case ODT_SYSLOG_WARN :
    case ODT_SYSLOG_INFO :
    case ODT_SYSLOG_DEBUG :
        break ;

    default :
        ODT_LOG_ERR ("<LOG MODULE> Undefined log level") ;
        return -1 ;
    }

    odt_g_log.m_syslog_level = level ;
    return 0 ;
}



int
odt_syslog_set_limit (size_t limit)
{
    return (odt_g_log.m_syslog_max_limitation = limit) ;
}
