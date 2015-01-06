#ifndef __SERVIX_BUFFER_INCLUED__
#define __SERVIX_BUFFER_INCLUED__


#include "servix_global.h"

struct servix_buffer {

	void		* m_data ;
	uint32_t	m_len ;

	uint32_t	m_elem_size ;
	uint32_t	m_elem_num ;
} ;

#define SVX_BUFF_INIT	{NULL, 0, 0, 0}


/*	name : svx_buff_create
 *	para : elem_size, elem_num
 *	function : create a buffer
 */
svx_buff *
svx_buff_create (int elem_num, uint32_t elem_size) ;


/*	name : svx_buff_destroy
 *	para : buf
 *	function : destroy a buffer
 */
int
svx_buff_destroy (svx_buff *buf) ;


/*	name : SVX_BUFF_DECLARATION
 *	para : elem_size, elem_num
 *	function : declare a buffer
 */
#define SVX_BUFF_DECLARATION(s, n, name)\
	char x [s * n] ;\
	svx_buff name = {x, s*n, s, n} ;


#endif
