#include "servix_global.h"

struct servix_buffer {

	void		* m_data ;
	uint32_t	m_len ;

	uint32_t	m_elem_size ;
	uint32_t	m_elem_num ;
} ;


/*	name : svx_buf_create
 *	para : elem_size, elem_num
 *	function : create a buffer
 */
svx_buff *
svx_buf_create (int elem_num, uint32_t elem_size) ;


/*	name : svx_buf_destroy
 *	para : buf
 *	function : destroy a buffer
 */
int
svx_buf_destroy (svx_buff *buf) ;


/*	name : SVX_BUF_DECLARATION
 *	para : elem_size, elem_num
 *	function : declare a buffer
 */
#define SVX_BUF_DECLARATION(s, n, name)\
	char x [s * n] ;\
	svx_buff name = {x, sizeof(x), s, n} ;
