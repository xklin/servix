

#include "servix_global.h"

/*
	Module : servix listen module
	Function : read the config file and fill the data structure of
			'svx_listen' type, and open the socket, bind to the destinate
			address, then listen the socket.
	Usage :
			While the user should first fill the 'svx_listen' data
			structure and call the 'svx_listen_init'. Then the user
			must call the 'svx_listen', after that, the socket will
			be in listening state, and if the user want to set the
			options of the socket, he should call the 'svx_config'.
*/


svx_array	svx_listen_array ;




int
svx_listen_udp_config ()
{
	int i ;

	for (i=0; i<g_listen_array.eln; ++i) {

	}
	return 0 ;
}




int
svx_listen_inherited_socket (svx_socket *sock, svx_listen *ls)
{
	
}




int
svx_listen_tcp_config ()
{
	int i ;
	int value ;

	for (i=0; i<g_listen_array.eln; ++i) {

		svx_listen *pls = svx_array_get (&g_listen_array, i) ;

		// Set the sending buffer and the recieving buffer
		if (pls->m_sndsz != -1) {
			pls->m_sndsz = pls->m_sndsz > SVX_TCP_SNDSZ_MAX ? 
							SVX_TCP_SNDSZ_MAX : pls->m_sndsz ;

			if (0 > setsockopt (pls->m_sock.m_sock, SOL_SOCKET, SO_SNDBUF,
					(const void*)&pls->m_sndsz, sizeof(pls->m_sndsz))) {

				value = svx_sock_get_sndbuf (&pls->m_sock) ;
				if (value == -1)
					svxe_log (LOG_WARN, "get length of sending buffer of"
						" %dth listen socket failed", i) ;

				svxe_log (LOG_WARN, "set length of sending buffer of"
						" %dth listen socket failed,"
						" the origin length is %d", i, value) ;
			}

			svxe_log (LOG_NOTICE, "set length of sending buffer of"
						" %dth listen socket to %d", i, pls->m_sndsz) ;
		}

		if (pls->m_rcvsz != -1) {
			pls->m_rcvsz = pls->m_rcvsz > SVX_TCP_RCVSZ_MAX ?
							SVX_TCP_RCVSZ_MAX : pls->m_rcvsz ;

			if (0 > setsockopt (pls->m_sock.m_sock, SOL_SOCKET, SO_RCVBUF,
					(const void*)&pls->m_rcvsz, sizeof(pls->m_rcvsz))) {

				value = svx_sock_get_rcvbuf (&pls->m_sock) ;
				if (-1 == value)
					svxe_log (LOG_WARN, "get length of sending buffer of"
						" %dth listen socket failed", i) ;

				svxe_log (LOG_ERROR, "set length of recieving buffer of"
						" %dth listen socket failed,"
						" the origin length is %d", i, value) ;
			}

			svxe_log (LOG_NOTICE, "set length of recieviing buffer of"
						" %dth listen socket to %d", i, pls->m_rcvsz) ;
		}

		// Set the keepalive
		if (pls->m_is_keepalive) {
			value = 1 ;
			if (0 > setsockopt (pls->m_sock.m_sock, SOL_SOCKET, SO_KEEPALIVE,
					(const void*)&value, sizeof(value))) {

				svxe_log (LOG_ERROR, "set keepalive of"
						" %dth listen socket failed", i) ;
			}

			svxe_log (LOG_NOTICE, "set [keepalive] of %dth listen socket", i) ;
		}

		// Set the fast open
		if (pls->m_is_fast_open) {
			value = 1 ;
			if (0 > setsockopt (pls->m_sock.m_sock, IPPROTO_TCP, TCP_FASTOPEN,
					(const void*)&value, sizeof(value))) {

				svxe_log (LOG_ERROR, "set fast open of"
						" %dth listen socket failed", i) ;
			}

			svxe_log (LOG_NOTICE, "set [fastopen] of %dth listen socket", i) ;
		}

		// Set the defer accept
		if (pls->m_is_defer_accept) {
			value = 1 ; 
			if (0 > setsockopt (pls->m_sock.m_sock, IPPROTO_TCP,
					TCP_DEFER_ACCEPT, (const void*)value, sizeof(value))) {

				svxe_log (LOG_ERROR, "set accept defer of"
						" %dth listen socket failed", i) ;
			}

			svxe_log (LOG_NOTICE, "set [accept-defer] of %dth listen socket", i) ;
		}
	}

	return 0 ;
}





int
svx_listen_tcp ()
{
	int i ;
	svx_listen *pls ;
	int fd ;
	int value ;

	for (i=0; i<g_listen_array.m_eln; ++i) {
		pls = (svx_listen *)svx_array_get (&g_listen_array, i) ;

		// Open the socket
		if (NULL == svx_sock_create (SOCK_TCP, &pls->m_sock)) {
			svxe_log (LOG_EMGER, "create %dth listen socket failed", i) ;
			return SVX_ERROR ;
		}

		// Set the socket resueable
		value = 1 ;
		if (0 > setsockopt (pls->m_sock.m_sock, SOL_SOCKET,
				SO_REUSEADDR, (const void*)&value, sizeof(int))) {

			svxe_log (LOG_EMGER, "set %dth listen socket to reuseaddr"
						"failed", i) ;

			if (0 > svx_sock_destroy (&pls->m_sock))
				svxe_log (LOG_EMGER, "close %dth listen socket failed", i) ;

			return SVX_ERROR ;
		}

		// Bind the address
		if (0 > bind (fd, (struct sockaddr*)pls->m_addr.m_addr,
					(socklen_t) sizeof(struct sockaddr))) {

			svxe_log (LOG_EMGER, "bind address to the %dth listen socket
							failed", i) ;

			if (0 > svx_sock_destroy (&pls->m_sock))
				svxe_log (LOG_EMGER, "close %dth listen socket failed", i) ;

			return SVX_ERROR ;
		}

		// Listen the socket
		if (0 > listen (fd, pls->backlog)) {
			svxe_log (LOG_EMGER, "listen to the %dth listen socket failed", 1) ;

			if (0 > svx_sock_destroy (&pls->m_sock))
				svxe_log (LOG_EMGER, "close %dth listen socket failed", i) ;

			return SVX_ERROR ;
		}
	}
} 
