#include "servix_socket.h"



int
svx_sock_setblock (svx_socket *psock)
{
	int nb = 0 ;

	if (0 == psock->m_isnoblock)
		return 0 ;

	return ioctl (psock->m_sock, FIONBIO, &nb) ;
}





int
svx_sock_setnoblock (svx_socket *psock)
{
	int nb = 1 ;

	if (1 == psock->m_isnoblock)
		return 0 ;

	psock->m_isnoblock = 1 ;
	return ioctl (psock->m_sock, FIONBIO, &nb) ;
}





int
svx_sock_setnopush (svx_socket *psock)
{
	int cork = 1 ;

	if (0 == psock->m_isnopush)
		return 0 ;

	psock->m_isnopush = 1 ;
	return setsockopt (psock->m_sock, IPPROTO_TCP, TCP_CORK,
					(const void*)&cork, sizeof (int)) ;
}




int
svx_sock_setpush (svx_socket *psock)
{
	int cork = 0 ;

	if (1 == psock->m_isnopush)
		return 0 ;

	return setsockopt(psock->m_sock, IPPROTO_TCP, TCP_CORK, 
					(const void *) &cork, sizeof(int));
}





svx_socket *
svx_sock_create (int type, svx_socket *sock)
{
	int _type ;

	if (NULL == sock)
		sock = malloc (sizeof (svx_socket)) ;
	if (NULL == sock)
		svxe_exit () ;

	switch (type) {
		case SOCK_TCP :
			_type = SOCK_STREAM ;
			sock->m_istcp = 1 ;
			break ;
		case SOCK_UDP :
			_type = SOCK_DGRAM ;
			sock->m_isudp = 1 ;
			break ;
		default :
			svxe_exit () ;
	}
	// initialize the whole socket
	memset (sock, 0, sizeof (svx_socket)) ;

	sock->m_sock = socket (AF_INET, _type, 0) ;
	sock->m_isopen = 1 ;
	return sock ;
}




svx_errno_t
svx_sock_destroy (svx_socket *sock)
{
	assert (NULL != sock) ;

	if (!sock->m_isopen) {
		svxe_log (LOG_DEBUG, "socket [%d] is not openning,"
				"but try to close it", sock->m_sock) ;

		return SVX_DEBUG ;
	}

	if (0 > close (sock->m_sock)) {
		svxe_log (LOG_EMGER, "socket [%d] close failed", sock->m_sock) ;
		return SVX_ERROR ;
	}

	sock->m_isopen = 0 ;
	return SVX_NORMAL ;
}





int
svx_sock_get_sndbuf (svx_socket *psock)
{
	int value ;

	if (0 > getsockopt (psock->m_sock, SOL_SOCKET, SO_SNDBUF,
		(const void*)&value, sizeof (int))) {

		return -1 ;
	}

	return value ;
}



int
svx_sock_get_rcvbuf (svx_socket *psock)
{
	int value ;

	if (0 > getsockopt (psock->m_sock, SOL_SOCKET, SO_RCVBUf,
			(const void*)&value, sizeof (int))) {

		return -1 ;
	}

	return value ;
}





svx_addr *
svx_addrv4_create (const char *ip_dot, int port, svx_addr *addr)
{
	/*
	 * 	if (svx_ip_valid_dot (ip_dot))
	 */
	if (NULL == addr)
		addr = malloc (sizeof (svx_addr)) ;
	else
		memset (addr, 0, sizeof(svx_addr)) ;

	addr->m_ip_dot = ip_dot ;
	addr->m_port = port ;
	addr->m_family = AF_INET ;

	if (0 > inet_pton (AF_INET, ip_dot, &addr->m_addr.sin_addr)) {
		svxe_log (LOG_EMGER, "inet_pton() the address %s failed", ip_dot) ;
		return -1 ;
	}

	addr->m_addr.sin_family = addr->m_family ;
	addr->m_addr.sin_port = htons (port) ;
	addr->m_socklen = (socklen_t) sizeof(struct sockaddr_in) ;
	return addr ;
}


