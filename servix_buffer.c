#include "servix_global.h"




svx_buff *
svx_buff_create (int elem_num, uint32_t elem_size)
{
	svx_buff *buffer ;
	char *data ;

	/* Allocate spaces for the buffer */

	buffer = malloc (sizeof(svx_buff)) ;
	data = malloc (elem_num * elem_size) ;

	if (NULL == data || NULL == buffer) {
		svxe_log (LOG_PANIC, "malloc() for buffer failed") ;

		if (NULL == data && NULL != buffer)
			free (buffer) ;
		else if (NULL != data && NULL == buffer)
			free (data) ;

		return NULL ;
	}

	buffer->m_data = data ;
	buffer->m_len = elem_num * elem_size ;
	buffer->m_elem_size = elem_size ;
	buffer->m_elem_num = elem_num ;

	return buffer ;
}




int
svx_buff_destroy (svx_buff *buf)
{
	assert (NULL != buf) ;

	if (NULL != buf->m_data)
		free (buf->m_data) ;

	free (buf) ;
	return 0 ;
}
