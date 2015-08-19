
#include "odt.h"
#include <sys/time.h>
#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <netinet/tcp.h>






void
odt_connection_def_cb (int what, void *arg)
{
	odt_connection_t *conn = (odt_connection_t *)arg ;

	int err , l ;


    /* Ready task posted */
    if (what & ODT_EV_READY) {

        conn->m_connect = 1 ;
        if (conn->cb)
            conn->cb (ODT_EV_WRITE, conn->cb_arg) ;
        return ;
    }



	/* Delete the event */
	if (-1 == odt_event_del (conn->m_evm, &conn->m_ev)) {

        ODT_LOG_ERR("Delete event failed") ;
		return ;
	}


    /* Connection failed */
	if (what & ODT_EV_ERR) {

		l = sizeof(err) ;
		if (-1 == getsockopt (conn->m_fd, SOL_SOCKET, SO_ERROR,	&err, &l)) {

            ODT_LOG_EMERG("'getsockopt' failed, errno=%d", errno) ;
			return ;
		}

		if (err > 0) {

			/* Connection failed */
			conn->m_connect = 0 ;
			if (conn->cb_conn_fail)
			    conn->cb_conn_fail (what, conn->cb_conn_fail_arg) ;
			return ;
		}else{

			/* value of err is invalid */
			odt_assert (err > 0) ;
		}
	}


	/* Connect success */
	if (what & ODT_EV_WRITE) {

		/* Print log */
		conn->m_connect = 1 ;
		if (conn->cb)
            conn->cb (what, conn->cb_arg) ;
		return ;
	}
}


void
odt_connection_def_cb_timeout (int what, void *arg)
{
	odt_connection_t *conn = (odt_connection_t *)arg ;


	conn->m_timeout = 1 ;
	conn->m_connect = 0 ;

    /* Ready task posted */
    if (what & ODT_EV_READY_FAIL) {

        if (!conn->cb_conn_fail)
            conn->cb_conn_fail (ODT_EV_ERR, conn->cb_conn_fail_arg) ;
        return ;
    }

	/* Delete the event */
	if (-1 == odt_event_del (conn->m_evm, &conn->m_ev)) {

        ODT_LOG_ERR("Delete event failed") ;
		return ;
	}

	if (!conn->cb_timeout)
	    conn->cb_timeout (what, conn->cb_timeout_arg) ;
}


int
odt_connection_init (odt_connection_t *conn, odt_event_manager_t *evm)
{
	odt_assert (NULL != conn);
	odt_assert (NULL != evm) ;

	struct sockaddr_in addr ;
	int val ;

	/* Create socket */
	if (-1 == (conn->m_fd = socket (AF_INET, SOCK_STREAM, 0))) {

        ODT_LOG_EMERG("'socket' failed, errno=%d", errno) ;
		return -1 ;
	}

	/* Set the peer address */
	memset (&addr, 0, sizeof(struct sockaddr_in)) ;

	addr.sin_family = AF_INET ;
	addr.sin_port = htons (conn->m_port) ;

	if (-1 == inet_pton (AF_INET, conn->m_ip, &addr.sin_addr)) {

        ODT_LOG_EMERG("'inet_pton' failed, errno=%d", errno) ;
		return -1 ;
	}

	conn->m_addr = addr ;


	if (conn->m_nonblock) {
		if (-1 == (val = fcntl (conn->m_fd, F_GETFL, 0))) {

            ODT_LOG_EMERG("'fcntl' failed, errno=%d", errno) ;
			return -1 ;
		}

		/* Set socket to non-block */
		if (-1 == fcntl (conn->m_fd, F_SETFL, val | O_NONBLOCK)) {

            ODT_LOG_EMERG("'fcntl' failed, errno=%d", errno) ;
			return -1 ;
		}
	}

	/* Set socket options */

	if (conn->m_linger) {
		struct linger l ;
		l.l_onoff = 1 ;
		l.l_linger = 0 ;
		if (-1 == setsockopt (conn->m_fd, SOL_SOCKET, SO_LINGER, 
					&l, sizeof(struct linger))) {

            ODT_LOG_EMERG("'setsockopt' failed, errno=%d",errno) ;
			return -1 ;
		}
	}

	if (conn->m_cork) {
		val = 1 ;
		if (-1 == setsockopt (conn->m_fd, IPPROTO_TCP, TCP_CORK,
					&val, sizeof(val))) {

            ODT_LOG_EMERG("'setsockopt' failed, errno=%d",errno) ;
			return -1 ;
		}
	}

	if (conn->m_sndbuf > 0) {
		if (-1 == setsockopt (conn->m_fd, SOL_SOCKET, SO_SNDBUF,
					&conn->m_sndbuf, sizeof(int))) {

            ODT_LOG_EMERG("'setsockopt' failed, errno=%d",errno) ;
			return -1 ;
		}
	}

	if (conn->m_recvbuf > 0) {
		if (-1 == setsockopt (conn->m_fd, SOL_SOCKET, SO_RCVBUF,
					&conn->m_recvbuf, sizeof(int))) {

            ODT_LOG_EMERG("'setsockopt' failed, errno=%d",errno) ;
			return -1 ;
		}
	}

	conn->m_timeout = 0 ;
	conn->m_connect = 0 ;

	conn->m_evm = evm ;
}




int
odt_connection_doconn (odt_connection_t *conn, odt_event_manager_t *evm)
{
	odt_assert (NULL != conn) ;


	if (-1 == odt_connection_init (conn, evm)) {

        ODT_LOG_ERR("Initialize connection failed") ;
		return -1 ;
	}

    odt_event_new (&conn->m_ev, conn->m_fd, odt_connection_def_cb,
                   conn, odt_connection_def_cb_timeout, conn) ;


	/* Call 'connect' API */
	if (0 > connect (conn->m_fd, (struct sockaddr *)&conn->m_addr,
					sizeof(struct sockaddr_in))) {

		if (!conn->m_nonblock || errno != EINPROGRESS) {

            ODT_LOG_EMERG("'connect' failed, errno=%d", errno) ;

            /* Add it to ready list with EV_READY_FAIL */
            odt_event_post (evm, &conn->m_ev, ODT_EV_READY_FAIL) ;
            return 0 ;
		}

		if (-1 == odt_event_add (evm, &conn->m_ev, ODT_EV_WRITE,
                        conn->m_tv_ms)) {

            ODT_LOG_ERR ("Add event failed");
			return -1 ;
        }
	}else{
		/* Connection complete immediately, so add it to ready list */

		conn->m_connect = 1 ;
        odt_event_post (evm, &conn->m_ev, ODT_EV_READY) ;
	}

	return 0 ;
}



int
odt_connection_disconn (odt_connection_t *conn)
{
	odt_assert (NULL != conn) ;

	if (-1 == close (conn->m_fd)) {

        ODT_LOG_EMERG("'close' failed, errno=%d", errno) ;
		return -1 ;
	}

	conn->m_connect = 0 ;
}

