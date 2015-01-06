
#ifndef __SERVIX_LISTEN_INCLUDED__
#define __SERVIX_LISTEN_INCLUDED__

#include "servix_global.h"

struct servix_listen {

	svx_socket		m_sock ;
	svx_addr		m_addr ;

	int				m_backlog ;					// queue's backlog of listening
	int				m_sndsz ;					// Send buffer's size
	int				m_rcvsz ;					// Recieve buffer's size

	unsigned		m_is_defer_accept:1 ;
	unsigned		m_is_fast_open:1 ;
	unsigned		m_is_keepalive:1 ;
} ;


#define SVX_LISTEN_BLANK	{SVX_SOCKET_BLANK,\
							SVX_ADDR_BLANK,\
							-1,	-1, -1,	0, 0, 0}
#define SVX_LISTEN_INIT		{SVX_SOCKET_BLAN,\
							SVX_ADDR_BLANK,\
							SVX_LISTEN_BACKLOG,\
							-1,	-1,	0, 0, 0}


/*	Sending buffer's length limitation */
#define	SVX_TCP_SNDSZ_MAX	65535

/*	Recieving buffer's length limitation */
#define SVX_TCP_RCVSZ_MAX	65535



/*	name : svx_listen_init
 *	function :
		Fill the data structure of the 'svx_listen' with
		a variable of 'svx_config'
 */
int svx_listen_init (svx_listen *pls, svx_config *conf) ;


/*	name : svx_listen_config_tcp
 *	function :
		Set the options of the listen tcp socket
 */
int svx_listen_config_tcp (svx_listen *pls) ;



/*	name : svx_listen_tcp
 *	function :
		Open the listen socket, set the necessary options,
		bind to the address and then listen to the address
 */
int svx_listen_tcp (svx_listen *pls) ;


#endif
