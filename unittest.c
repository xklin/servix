
#include "servix_global.h"


#ifdef __SERVIX_DEBUG_ARRAY__

#define print_array(A)\
	printf ("Array: %p, Elsz: %d, Eln: %d, Alloc: %d\n", (A)->m_array,\
			(A)->m_elsz, (A)->m_eln, (A)->m_alloc) ;


int test_INIT (svx_array *array)
{
	svx_array temp = SVX_ARRAY_INIT ;
	memcpy (array, &temp, sizeof (svx_array)) ;
	print_array (array) ;
}

int test_create (svx_array *array, int n, int s)
{
	printf ("\n>>>> Test the 'svx_array_create'\n") ;
	assert (NULL != svx_array_create(s, n, array)) ;
	print_array (array) ;
}


int main ()
{
	svx_array temp ;
	int *val ;
	int n ;

	test_INIT (&temp) ;
	test_create (&temp, 1, sizeof(int)) ;

	for (n=0; n<20; ++n) {
		val = svx_array_add (&temp) ;
		*val = n ;
		printf ("%d\n", *val) ;
		print_array (&temp) ;
	}

	for (n=0; n<20; ++n) {
		val = svx_array_get (&temp, n) ;
		printf ("%d ", *val) ;
	}
	return 0 ;
}


#endif






#ifdef __SERVIX_DEBUG_STRING_HASH__

const char *str_table[] = {
	"Hello, world",
	"This is servix",
	"My name is fool",
	"C programming",
	"I like Web",
	"I like basketball",
	"C plus plus",
	"My iPhone 4S",
	"You're stupid",
	"What the fuck",
	"It is the end",
	NULL
} ;

const char *find_table[] = {
	"Hello, world",
	"Hello, Java",
	"This is servix",
	"My name is fool",
	"My name is klin",
	"C programming",
	"I like Web",
	"I like basketball",
	"C plus plus",
	"C sub sub",
	"My iPhone 4S",
	"My iPhone plus",
	"You're stupid",
	"You're mine",
	"What the fuck",
	"What is up",
	"It is the end",
	"It is the DOOM",
	NULL
} ;

int main ()
{
	int i ;
	svx_str_hash_key key_array ;
	svx_str_hash_table hash_table ;
	const char *str ;

	assert (SVX_ERROR != svx_str_hash_key_init (&key_array, 5)) ;

	for (i=0; str_table[i] != NULL; ++i) {

		str = str_table [i] ;
		assert (SVX_ERROR != svx_str_hash_key_add (
				str, strlen(str), &key_array)) ;

		char **s = svx_array_get (&key_array.m_string, i) ;
		str = *s ;
		printf ("\"%s\" is added, it's key is \'%u\'\n", str,
					svx_str_hash (str, strlen(str))) ;
	}

	assert (SVX_ERROR != svx_str_hash_init (&key_array, &hash_table, 10)) ;

	for (i=0; find_table[i] != NULL; ++i) {

		str = find_table[i] ;

		printf ("\"%s\" is checked, it's key is \'%d\'---", str,
					svx_str_hash (str, strlen(str))) ;

		if (NULL == svx_str_hash_find (str, &hash_table))
			printf ("Cannot find\n") ;
		else
			printf ("Yes\n") ;
	}

	return 0 ;
}



#endif




