#ifndef __ODT_CONNECTION_H__
#define __ODT_CONNECTION_H__

#include "odt.h"


typedef struct {

	int m_timeout:1 ;
	int m_connect:1 ;

	/* If m_linger is set to 1, the close() calling will cause the kernel to
	send a RST to peer. Or the SO_LINGER option will not work */
	int m_linger:1 ;
	int m_nonblock:1 ;
	int m_cork:1 ;

	unsigned int m_port ;
	char m_ip [16] ;
	struct sockaddr_in m_addr ;

	int m_fd ;
	int m_sndbuf ;
	int m_recvbuf ;

	odt_event_t m_ev ;
	odt_event_manager_t *m_evm ;

	odt_callback_func cb ;
	void *cb_arg ;

	odt_callback_func cb_conn_fail ;
	void *cb_conn_fail_arg ;

	odt_callback_func cb_timeout ;
	long m_tv_ms ;
	void *cb_timeout_arg ;
} odt_connection_t ;




/* The connection code will do these jobs :
	* Set the socket to non blocking, and deal with the connection
	  under this situation
	* Set some socket options for connection
	* Sending messages

	This data structure is only a virtual connection between the
	host and the peer, maybe a real connection can serve a group
	of virtual connections, and these virtual connections can be
	differed by its own feature. But connection-reuseable feature
	will not be implemented now.
*/




/* Connect to the peer */
int odt_connection_doconn (odt_connection_t *conn,
						odt_event_manager_t *evm) ;


/* Disconnect */
int odt_connection_disconn (odt_connection_t *conn) ;




#endif
