#ifndef __SERVIX_ARRAY_INCLUDED__
#define __SERVIX_ARRAY_INCLUDED__

#include "servix_global.h"


struct servix_array {

	void			*m_array ;
	svx_uint8_t		m_elsz ;
	svx_uint8_t		m_eln ;
	svx_uint8_t		m_alloc ;
} ;


#define	SVX_ARRAY_INIT	{\
			NULL, 0, 0, 0 }

#define svx_array_get(array, i)\
	((array)->m_array + (array)->m_elsz * (i))


void svx_array_destroy (svx_array *array) ;
svx_array	*svx_array_create (svx_uint8_t size,
							svx_uint8_t n,
							svx_array *array) ;
void *svx_array_add (svx_array *array) ;
void *svx_array_add_n (svx_array *array, int n) ;


#endif
