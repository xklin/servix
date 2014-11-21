

#include "servix_global.h"


svx_array	svx_listen_array ;


/*	name : svx_init_listen
 *	para :
 *	function : init the listen list
 */
int svx_init_listen (char *ip, int port)
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


/*	name : svx_prepare_listen
 *	para : sock, addr
 *	function : prepare listening
 */
int
svx_prepare_listen (svx_listen *ls)
{
	assert (NULL != sock && NULL != addr && NULL != ls) ;

	
}



/*	name : svx_listen
 *	para : sock, addr
 *	function : listen to the socket
 */
int
svx_listen (svx_sock *sock, svx_addr *addr)
{

}
