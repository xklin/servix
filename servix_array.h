#ifndef __SERVIX_ARRAY_INCLUDED__
#define __SERVIX_ARRAY_INCLUDED__

#include "servix_global.h"


struct servix_array {

	void			*m_array ;
	svx_uint8_t		m_elsz ;
	svx_uint8_t		m_eln ;
	svx_uint8_t		m_alloc ;
} ;


#define	SVX_ARRAY_INIT	{NULL, 0, 0, 0}

/*	name : svx_array_get
 *	function :
		Get the point of the specific element in the array
 */
#define svx_array_get(array, i)\
	((array)->m_array + (array)->m_elsz * (i))


/*	name : svx_array_destroy
 *	function :
		Free the space of the array, then the spaces of the member
		'm_array' and the array itself must from dynamic allocation
 */
void svx_array_destroy (svx_array *array) ;



/*	name : svx_array_create
 *	function :
		Allocate spaces for the array and its member 'm_array', then
		initialize the array itself
 */
svx_array	*svx_array_create (svx_uint8_t size,
							svx_uint8_t n,
							svx_array *array) ;


/*	name : svx_array_add
 *	function :
		Add a new element to the array, and the point of the element
		will be returned
 */
void *svx_array_add (svx_array *array) ;



/*	name : svx_array_add_n
 *	function :
		Add n new elements to the array, and the point of the first
		new element will be returned
 */
void *svx_array_add_n (svx_array *array, int n) ;



/*	name : SVX_ARRAY_DECLARE
 *	function :
		Be same to the 'svx_array_create', but this macro is used within
		a funciton's scope, and the array should not be used when the
		function is over
 */
#define SVX_ARRAY_DECLARE(s, n, name)\
		char __ARR [s*n] ;\
		svx_array name = {__ARR, s, 0, n} ;




#endif
