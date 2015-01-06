#ifndef __SERVIX_HASH_INCLUDED__
#define __SERVIX_HASH_INCLUDED__


#include "servix_global.h"


struct servix_string_hash_elt {

	void		*m_value ;
	int			m_len ;
	char		m_next ;
} ;



struct servix_string_hash_table {

	void		**m_buckets ;
	int			m_buckets_n ;
} ;

struct servix_string_hash_key {

	svx_array	m_string ;
	svx_array	m_value ;
	int			m_eltn ;
} ;



/*	name : svx_str_hash
 *	function :
		compute the hash value of a string
 */
svx_hash_t		svx_str_hash (const char *string, int len) ;



/*	name : svx_str_hash_init
 *	function :
		initialize the hash table of by a given 'svx_str_hash_key'
 */
svx_errno_t		svx_str_hash_init (svx_str_hash_key *key,
								svx_str_hash_table *hash_table,
								unsigned int respect_size) ;


/*	name : svx_str_hash_find
 *	function :
		find the slot of a given string in the given 'svx_str_hash_table'
		hash table
 */
svx_str_hash_elt	*svx_str_hash_find (char *str,
						svx_str_hash_table *hash_table) ;


/*	name : svx_str_hash_key_add
 *	function :
		add a key-value pair into the 'svx_str_hash_key'
 */
svx_errno_t		svx_str_hash_key_add (char *string,
									int len,
									svx_str_hash_key *key) ;



/*	name : svx_str_hash_key_init
 *	function :
		initialize the string hash table
 */
svx_errno_t		svx_str_hash_key_init (svx_str_hash_key *key,
									unsigned int origin) ;



#endif
