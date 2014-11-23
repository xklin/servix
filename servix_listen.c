

#include "servix_global.h"


svx_array	svx_listen_array ;


/*	name : svx_listen_init
 *	para :
 *	function : init the listen list
 */
int svx_listen_init (char *ip, int port)
{
	assert (NULL != conf) ;

	svx_listen temp = SVX_LISTEN_INIT ;
	memcpy (&g_listen, &temp) ;

	if (NULL == svx_addrv4_create (ip, m_port, &g_listen.m_addr)) {
		svxe_log (LOG_PANIC, "Create address error") ;
		return -1 ;
	}

	if (NULL == svx_sock_create (SOCK_TCP, &g_listen.m_sock)) {
		svxe_log (LOG_PANIC, "Create socket failed") ;
		return -1 ;
	}

	if (0 > svx_sock_setnoblock (&g_listen.m_sock)) {
		svxe_log (LOG_PANIC, "Set socket to non-block failed") ;
		return -1 ;
	}

	if (0 > svx_sock_setnopush (&g_listen.m_sock))
		svxe_log (LOG_ERROR, "Set socket to non-push failed") ;

	return 0 ;
}


/*	name : svx_listen_prepare
 *	para : sock, addr
 *	function : prepare listening
 */
int
svx_listen_prepare ()
{
	assert (NULL != sock && NULL != addr && NULL != ls) ;

	int i ;

	for (i=0; i<g_listen_array.eln; ++i) {
		svx_listen *pls = svx_array_get (&g_listen_array, i) ;

		// Set the sending buffer and the recieving buffer
		if (pls->m_sndsz != -1) {
			if (0 > setsockopt (pls->m_sock.m_sock, SOL_SOCKET, SO_SNDBUF,
					(const void*)&pls->m_sndsz, sizeof(pls->m_sndsz)))
				svxe_log (LOG_ERROR, "set sending buffer failed") ;
		}

		if (pls->m_rcvsz != -1) {
			if (0 > setsockopt (pls->m_sock.m_sock, SOL_SOCKET, SO_RCVBUF,
					(const void*)&pls->m_rcvsz, sizeof(pls->m_rcvsz)))
				svxe_log (LOG_ERROR, "set recieving buffer failed") ;
		}

		// Set the keepalive
		if (pls->m_is_keepalive) {
			int value = 1 ;
			if (0 > setsockopt (pls->m_sock.m_sock, SOL_SOCKET, SO_KEEPALIVE,
					(const void*)&value, sizeof(value)))
				svxe_log (LOG_ERROR, "set socket to keepalive failed") ;
		}

		// Set the fast open
		if (pls->m_is_fast_open) {
			int value = 1 ;
			if (0 > setsockopt (pls->m_sock.m_sock, IPPROTO_TCP, TCP_FASTOPEN,
					(const void*)&value, sizeof(value)))
				svxe_log (LOG_ERROR, "set socket to fast open failed") ;
		}

		// Set the defer accept
		if (pls->m_is_defer_accept) {
			int timeout = 1 ;
			if (0 > setsockopt (pls->m_sock.m_sock, IPPROTO_TCP,
			TCP_DEFER_ACCEPT, (const void*)timeout, sizeof(timeout)))
				svxe_log (LOG_ERROR, "set socket to defer accept failed") ;
		}
	}

	return 0 ;
}



/*	name : svx_listen
 *	para : sock, addr
 *	function : listen to the socket
 */
int
svx_listen (svx_sock *sock, svx_addr *addr)
{

}
