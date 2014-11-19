
#ifndef __SERVIX_LISTEN_INCLUDED__
#define __SERVIX_LISTEN_INCLUDED__

#include "servix_global.h"

struct servix_listen {

	svx_socket		*m_sock ;
	svx_addr		*m_addr ;

<<<<<<< HEAD
	int				m_backlog ;					// queue's backlog of listening
=======
	int				m_backlog ;
>>>>>>> origin/master
	int				m_sndsz ;					// Send buffer's size
	int				m_rcvsz ;					// Recieve buffer's size

	unsigned		m_is_defer_accept:1 ;
	unsigned		m_is_fast_open:1 ;
} ;


/*	name : svx_prepare_listen
 *	para : sock, addr
 *	function : prepare listen
 */
int svx_prepare_listen (svx_listen *listen, svx_socket *sock, svx_addr *addr) ;


/*	name : svx_listen
 *	para : sock, addr
 *	fucntion : bind and listen to socket
 */
int	svx_listen (svx_socket *sock, svx_addr *addr) ;


#endif
