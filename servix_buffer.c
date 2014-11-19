#include "servix_global.h"

/*	name : svx_buf_create
 *	para : elem_size, elem_num
 *	function : create a buffer
 */
svx_buff *
svx_buf_create (int elem_num, uint32_t elem_size)
{
	svx_buff *buffer ;
	char *data ;
	/* Allocate spaces for the buffer */
	buffer = malloc (sizeof(svx_buff)) ;
	data = malloc (elem_num * elem_size) ;

	if (NULL == data || NULL == buffer) {
		perror ("Malloc") ;
		return NULL ;
	}

	buffer->m_data = data ;
	buffer->m_len = elem_num * elem_size ;
	buffer->m_elem_size = elem_size ;
	buffer->m_elem_num = elem_num ;
	return buffer ;
}

/*	name : svx_buf_destroy
 *	para : buf
 *	function : destroy a buffer
 */
int
svx_buf_destroy (svx_buff *buf)
{
	assert (NULL != buf) ;

	if (NULL != buf->m_data)
		free (buf->m_data) ;

	free (buf) ;
	return 0 ;
}
