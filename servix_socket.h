
#ifndef __SERVIX_SOCKET_INCLUED__
#define __SERVIX_SOCKET_INCLUED__

#include "servix_global.h"


/*===========================================================
 * servix_socket
 *========================================================*/

enum socket_type {
	SOCK_TCP ,
	SOCK_UDP ,
} ;

/*	name : svx_socket
 *	author : klin
 *	para :
 *	function : The base socket data structure
 */
struct servix_socket {

	int		m_sock ;	// socket fd

	unsigned	m_isopen:1;		// socket is openning	
	unsigned	m_isnoblock:1 ;	// socket is block
	unsigned	m_isnopush:1 ;	// socket is push
	unsigned	m_isudp:1 ;		// socket is type of udp
	unsigned	m_istcp:1 ;		// socket is type of tcp
} ;


#define	SVX_SOCKET_BLANK	{0, 0, 0, 0, 0}


/*	name : svx_sock_create
 *	function : create a svx_socket
 */
svx_socket	*svx_sock_create (int type, svx_socket *sock) ;



/*	name : svx_sock_destroy
 *	function :
		Close the socket.
 */
svx_errno_t	svx_sock_destroy (svx_socket *sock) ;


/*	name : svx_sock_isblock
 *	function : check if a svx_socket is blocking
 */
svx_bool	svx_sock_isblock (svx_socket *psock) ;


/*	name : svx_sock_setblock
 *	function : set a svx_socket to blocking
 */
int			svx_sock_setblock (svx_socket *psock) ;


/*	name : svx_sock_setnoblock
 *	function : set a svx_socket to no-blocking
 */
int			svx_sock_setnoblock (svx_socket *psock) ;


/*	name : svx_sock_ispush
 *	function : check if a svx_socket is push
 */
svx_bool	svx_sock_ispush (svx_socket *psock) ; 


/*	name : svx_sock_nopush 
 *	function : set a svx_socket to no-pushing */
 int			svx_sock_setnopush (svx_socket *psock) ;


/*	name : svx_sock_push
 *	function : set a svx_socket to pushing
 */
int			svx_sock_setpush (svx_socket *psock) ;


/*	name : svx_sock_get_sndbuf
 *	functin : 
		Get the socket's length of its sending buffer
 */
int			svx_sock_get_sndbuf (svx_socket *psock) ;


/*	name : svx_sock_get_rcvbuf
 *	function : 
		Get the socket's length of its recieving buffer
 */
int			svx_sock_get_rcvbuf (svx_socket *psock) ;



/*=================================================================
 * servix_addr
 * ==============================================================*/

/*	name : svx_addr
 *	author : klin
 *	para :
 *	function : servix address data structure
 */
struct servix_addr {

	struct sockaddr_in	m_addr ;	// main address
	socklen_t			m_socklen ;	// length of the addr
	int					m_family ;  // address family
	int					m_port ;	// port

	char				*m_ip_dot;	// ip address by dotation
} ;


#define SVX_ADDR_BLANK	{NULL, 0, 0, 0, NULL}



/*	name : svx_addrv4_create
 *	author : klin
 *	para : dot notation
 *	function : create a svx_addr by a IPv4 address in dot notation
 */
svx_addr *svx_addrv4_create (const char *dotation, int port, svx_addr *addr) ;




#endif

