#include "servix_socket.h"

/*	name : svx_sock_isblock
 *	author : klin
 *	para : variable typed svx_socket
 *	function : check if a svx_socket is blocking
 */
svx_bool
svx_sock_isblock (svx_socket *psock)
{
	return !(psock->m_isnoblock) ;
}


/*	name : svx_sock_setblock
 *	author : klin
 *	para : variable typed svx_socket
 *	function : set a svx_socket to blocking
 */
int
svx_sock_setblock (svx_socket *psock)
{
	int nb = 0 ;

	if (1 == psock->m_isnoblock)
		return 0 ;

	return ioctl (psock->m_sock, FIONBIO, &nb) ;
}


/*	name : svx_sock_setnoblock
 *	author : klin
 *	para : variable typed svx_socket
 *	function : set a svx_socket to no-blocking
 */
int
svx_sock_setnoblock (svx_socket *psock)
{
	int nb = 1 ;

	if (0 == psock->m_isnoblock)
		return 0 ;

	return ioctl (psock->m_sock, FIONBIO, &nb) ;
}


/*	name : svx_sock_ispush
 *	author : klin
 *	para : variable typed svx_socket
 *	function : check if a svx_socket is push
 */
svx_bool
svx_sock_ispush (svx_socket *psock)
{
	return !((svx_bool)psock->m_isnopush) ;
}


/*	name : svx_sock_nopush
 *	author : klin
 *	para : variable typed svx_socket
 *	function : set a svx_socket to no-pushing
 */
int
svx_sock_setnopush (svx_socket *psock)
{
	int cork = 1 ;

	if (0 == psock->m_isnopush)
		return 0 ;

/*	return setsockopt (psock->m_sock, IPPROTO_TCP, TCP_CORK,
					(const void*)cork, sizeof (int)) ;
*/
}


/*	name : svx_sock_push
 *	author : klin
 *	para : variable typed svx_socket
 *	function : set a svx_socket to pushing
 */
int
svx_sock_setpush (svx_socket *psock)
{
	int cork = 0 ;

	if (1 == psock->m_isnopush)
		return 0 ;

	/*return setsockopt(psock->m_sock, IPPROTO_TCP, TCP_CORK, 
					(const void *) &cork, sizeof(int));
	*/
}

/*	name : svx_sock
 *	author : klin
 *	para : type
 *	function : create a svx_socket
 */
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
	return sock ;
}


/*	name : svx_addrv4_create
 *	author : klin
 *	para : ip_dot, port
 *	function : create a svx_addr
 */
svx_addr *
svx_addrv4_create (const char *ip_dot, int port, svx_addr *addr)
{
	/*
	 * 	if (svx_ip_valid_dot (ip_dot))
	 */
	if (NULL == addr)
		addr = malloc (sizeof (svx_addr)) ;

	addr->m_ip_dot = ip_dot ;
	addr->m_port = port ;
	addr->m_family = AF_INET ;

	if (0 > inet_pton (AF_INET, ip_dot, &addr->m_addr->sin_addr))
		svxe_exit () ;

	addr->m_addr->sin_family = addr->m_family ;
	addr->m_addr->sin_port = htons (port) ;
	addr->m_socklen = (socklen_t) sizeof(struct sockaddr_in) ;
	return addr ;
}





/*	name : svx_connect
 *	author : klin
 *	para :
 *	function : connect to a server
 */
int
svx_connect (svx_socket *sock, svx_addr *peer_addr)
{
	assert (sock != NULL && peer_addr != NULL) ;

	if (0 > connect (sock->m_sock, &peer_addr->m_addr,
			peer_addr->m_socklen)) {

		perror ("ERROR") ;
		return -1 ;
	}
	return 0 ;
}


/*	name : svx_send
 *	author : klin
 *	para :
 *	function : send data to peer socket
 */
int
svx_send (svx_socket *sock, svx_addr *peer_addr, svx_buff *buf)
{
	int ret ;
	assert (NULL != sock && NULL != peer_addr) ;

	ret = send (sock->m_sock, buf->m_data, buf->m_len, 0) ;
	if (0 > ret) {
		perror ("ERROR") ;
		return -1 ;
	}

	buf->m_len -= ret ;
	return ret ;
}


/*	name : svx_listen
 *	para : sock, addr
 *	function : bind and listen
 */
int
svx_listen (svx_socket *sock, svx_addr *addr)
{
	
}


