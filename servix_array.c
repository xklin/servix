#include "servix_global.h"

/*	name : svx_create_array
 *	para : size, number, array
 *	function : Create a new array
 */
svx_array *
svx_array_create (svx_uint8_t size, svx_uint8_t n, svx_array *array)
{
	assert (0 < size && 0 < n) ;

	if (NULL == array)
		array = malloc (sizeof (svx_array)) ;
	else if (NULL != array->m_array)
		free (array->m_array) ;

	if (NULL == array) {
		svxe_log (LOG_PANIC, "allocate space failed") ;
		return NULL ;
	}

	array->m_array = malloc (sizeof(size*n)) ;
	if (NULL == array->m_array) {
		svxe_log (LOG_PANIC, "allocate space failed") ;
		return NULL ;
	}

	array->m_elsz = size ;
	array->m_eln = 0 ;
	array->m_alloc = n ;
	return array ;
}


/*	name : svx_array_add
 *	para : array
 *	function : add an element
 */
void *
svx_array_add (svx_array *array)
{
	assert (NULL != array) ;

	void *elt ;
	int size = array->m_elsz*array->m_eln ;

	if (array.m_alloc == array.m_eln) {
		void *new_array ;

		new_array = malloc (sizeof(2*size)) ;
		if (NULL == new_array) {
			svxe_log (LOG_PANIC, "allocate space failed") ;
			return -1 ;
		}

		memcpy (new_array, array->m_array, size) ;
		array->m_array = new_array ;
		array->m_alloc *= 2 ;
	}

	elt = array->m_array + size ;
	array->m_eln ++ ;
	return elt ;
}


/*	name : svx_array_add_n
 *	para : array, number
 *	function : Add n elements
 */
void *
svx_array_add_n (svx_array *array, int n)
{
	assert (NULL != array) ;

	void *elt ;
	int size = array->elsz * array->m_eln ;

	if (array->m_eln == array->m_alloc) {
		void *new_array ;

		n = 2*((n > array->m_alloc) ? n : array->m_alloc) ;
		new_array = malloc (array->m_elsz*n) ;
		if (NULL == new_array) {
			svxe_log (LOG_PANIC, "allocate space failed") ;
			return -1 ;
		}

		memcpy (new_array, array->m_array, size) ;
		array->m_array = new_array ;
		array->m_alloc = n ;
	}

	elt = array->m_array + size ;
	array->eln += n ;
	return elt ;
}


/*	name : svx_array_destroy
 *	para : array
 *	function : Destroy an array
 */
void
svx_array_destroy (svx_array *array)
{
	assert (NULL != array) ;

	free (array->m_array) ;
	free (array) ;
}
