#include "servix_global.h"


svx_bool_t
svx_str_hash_cmp (const char *s1, const char *s2)
{
	if (0 == strcmp (s1, s2))
		return 1;
	return 0 ;
}


svx_hash_t
svx_str_hash (const char *string, int len)
{

	/*	The hash algorithm is the BKDR algorithm */

	int i ;
	svx_hash_t key ;

	for (i=0, key=0; i<len; ++i) {
		key = key * 31 + *string ;
	}

	return key ;
}





svx_errno_t
svx_str_hash_init (svx_str_hash_key *key,
					svx_str_hash_table *hash_table,
					unsigned int respect_size)
{
	int recommand_size ;
	void **buckets ;
	svx_array *str, *val ;
	int n , t, s ;
	svx_hash_t *i ;
	char **j ;
	int *bsize ;
	svx_str_hash_elt *slots ;
	svx_str_hash_elt *pelt ;

	/*	Give a recommanded size of the hash table */
	recommand_size = key->m_eltn*3 ;
	if (respect_size < recommand_size)
		recommand_size += key->m_eltn / 10 ;
	else if (respect_size > 10000)
		recommand_size -= 1000 ;
	else
		recommand_size = respect_size ;


	buckets = malloc (recommand_size * sizeof(svx_str_hash_elt*)) ;
	if (NULL == buckets) {

		svxe_log (LOG_PANIC, "allocate %d spaces for hash table failed",
				recommand_size * sizeof(svx_str_hash_elt*)) ;
		return SVX_ERROR ;
	}


	/* The constructure of the hash table :

		slots --> bucket 1 				-->			bucket 2		 -->				bucket 3 --> ...
					|									|									|
		elts -->  [first elt of bucket 1] [] [] [first elt of bucket 2] [] [] [] [] [] [first elt of bucket 3][] [] [] []...

		'bsize' is an array save the length of each buckets.
		'buckets' is an array save all the 'svx_str_hash_elt' .
	*/
	bsize = malloc (recommand_size * sizeof(int)) ;
	if (NULL == bsize) {
		svxe_log (LOG_PANIC, "allocate %d spaces for hash table failed",
				recommand_size * sizeof(int)) ;

		free (buckets) ;
		return SVX_ERROR ;
	}

	/* initialize the 'bsize' */
	for (n=0; n<recommand_size; ++n)
		bsize [n] = 1 ;



	str = &key->m_string ;
	val = &key->m_value ;

	/* compute the length of each buckets */
	for (n=0; n<key->m_eltn; ++n) {
		i = svx_array_get (val, n) ;
		*i = (*i) % recommand_size ;

		bsize [*i] ++ ;
	}

	/* compute the length of the slots */
	for (t=0, n=0; n<recommand_size; ++n)
		t += bsize [n] ;


	/* allocate spaces for the slots */
	slots = malloc (t * sizeof(svx_str_hash_elt)) ;
	if (NULL == slots) {

		svxe_log (LOG_PANIC, "allocate %d spaces for hash table failed",
				t * sizeof(svx_str_hash_elt)) ;

		free (buckets) ;
		free (bsize) ;
		return SVX_ERROR ;
	}


	for (n=0, s=0; n<recommand_size; ++n) {

		if (1 < bsize [n])
			buckets [n] = (void*)& slots [s] ;
		else
			buckets [n] = (void*)0 ;

		s += bsize [n] ;
	}


	memset (bsize, 0, recommand_size * sizeof(int)) ;
	memset (slots, 0, t * sizeof(svx_str_hash_elt)) ;
	for (n=0; n<key->m_eltn; ++n) {

		i = svx_array_get (val, n) ;
		*i = *i % recommand_size ;
		j = svx_array_get (str, n) ;

		pelt = (svx_str_hash_elt*)buckets [*i] + bsize [*i] ;

		pelt->m_value = *j ;
		pelt->m_len = strlen (*j) ;

		bsize [*i] ++ ;
		printf (".. %d\n", *i) ;
	}


	hash_table->m_buckets = buckets ;
	hash_table->m_buckets_n = recommand_size ;

	free (bsize) ;

	return SVX_NORMAL ;
}




svx_str_hash_elt
*svx_str_hash_find (char *str, svx_str_hash_table *hash_table)
{
	svx_str_hash_elt *pelt ;
	svx_hash_t value ;

	value = svx_str_hash(str, strlen(str)) ;
	value %= hash_table->m_buckets_n ;
	pelt = (svx_str_hash_elt *) hash_table->m_buckets [value] ;

	/* not found */
	if (NULL == pelt)
		return NULL ;

	while (pelt->m_value) {

		if (svx_str_hash_cmp (pelt->m_value, str))
			return pelt ;

		pelt ++ ;
	}

	/* not found */
	return NULL ;
}




svx_errno_t
svx_str_hash_key_add (char *string,
					int len,
					svx_str_hash_key *key)
{
	char **s ;
	svx_hash_t *h ;
	svx_hash_t val ;

	s = svx_array_add (&key->m_string) ;
	if (NULL == s) {

		svxe_log (LOG_EMGER, "add 1 string to hash key table failed") ;
		return SVX_ERROR ;
	}

	*s = string ;

	h = svx_array_add (&key->m_value) ;
	if (NULL == h) {

		svxe_log (LOG_EMGER, "add 1 key value to hash key table failed") ;
		return SVX_ERROR ;
	}

	val = svx_str_hash (string, len) ;
	*h = val ;

	key->m_eltn ++ ;

	return SVX_NORMAL ;
}





svx_errno_t
svx_str_hash_key_init (svx_str_hash_key *key, unsigned int origin)
{
	int s ;

	s = sizeof (char *) ;
	
	// Initialize the array with zero length
	if (NULL == svx_array_create (s, origin, &key->m_string)) {
		svxe_log (LOG_ERROR, "create hash string array failed") ;
		return SVX_ERROR ;
	}

	s = sizeof (svx_hash_t) ;

	// Initialize the array with zero length
	if (NULL == svx_array_create (s, origin, &key->m_value)) {
		svxe_log (LOG_ERROR, "create hash key value array failed") ;
		return SVX_ERROR ;
	}

	key->m_eltn = 0 ;

	return SVX_NORMAL ;
}
