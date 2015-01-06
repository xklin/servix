#ifndef __SERVIX_CONF_INCLUDED__
#define __SERVIX_CONF_INCLUDED__

#include "servix_global.h"


/*	The rule of syntax of the configure file :
 *		CONF			= MODULE;CONF | $
 *		MODULE 			= MODULE_NAME{MODULE_BODY}
 *		MODULE_NAME 	= key
 *		MODULE_BODY 	= (SUB_MODULE | KEY_VALUE_PAIR);
 *							MODULE_BODY; | $
 *		SUB_MODULE 		= MODULE
 *		KEY_VALUE_PAIR	= KEY:VALUE
 *		KEY 			= key
 *		VALUE 			= number | key
 *
 *	The rule of lex token of the configure file :
 *		key 	= [_a-z]key | [_a-z][_a-z0-9]key | $
 *		number	= 0-9
 */


// For 'svx_listen'
#define	CONF_LISTEN_IP				"LOCAL_IP"
#define CONF_LISTEN_PORT			"LOCAL_PORT"
#define CONF_LISTEN_SNDSZ			"SNDBUFSZ"
#define	CONF_LISTEN_RCVSZ			"RCVBUFSZ"
#define CONF_LISTEN_FASTOPEN		"FAST_OPEN_BOOL"
#define CONF_LISTEN_KEEPALIVE		"KEEPALIVE_BOOL"
#define	CONF_LISTEN_DEFERACCEPT		"DEFER_ACCEPT_BOOL"




struct servix_config {

	svx_conf_fd		m_fd ;
	svx_array		m_listen ;

	char			*m_buff ;
	int				m_buff_len ;
	unsigned int	m_buff_pos ;
} ;




#define	SERVIX_LISTEN_BACKLOG	512


/*	name : svx_conf_parse
 *	function :
		Parse the configure file and fill the data structure of
		'svx_config'
 */
svx_errno_t		svx_conf_parse (svx_config *conf) ;


#endif
