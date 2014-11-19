

#include "servix_global.h"


svx_list g_listen_lst ;


/*	name : svx_init_listen
 *	para :
 *	function : init the listen list
 */
int svx_init_listen (svx_conf *conf)
{

}


/*	name : svx_prepare_listen
 *	para : sock, addr
 *	function : prepare listening
 */
int
svx_prepare_listen (svx_sock *sock, svx_addr *addr, svx_listen *ls)
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
