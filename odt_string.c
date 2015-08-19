#include "odt.h"
#include <string.h>
#include <stdarg.h>


void
odt_string_realloc (odt_string_t *str, int len)
{
	odt_assert (0 <= len) ;
	odt_assert (NULL != str) ;

	char *tmp = str->m_data ;
	int old_len = str->m_len > len ? len : str->m_len;

	str->m_data = odt_malloc (len) ;
	if (tmp != NULL) {
		memcpy (str->m_data, tmp, len) ;
		odt_free (tmp) ;
	}

	str->m_len = old_len ;
	str->m_free = len - old_len ;
}


int
S_LEN (odt_string_t *str)
{
	odt_assert (NULL != str) ;
	odt_assert (0 <= str->m_len) ;

	return str->m_len ;
}


int
S_FREE (odt_string_t *str)
{
	odt_assert (NULL != str) ;
	odt_assert (0 < str->m_free) ;

	return str->m_free ;
}


odt_string_t
*odt_string_create_free (int n)
{
	odt_assert (0 <= n) ;

	odt_string_t *str ;

	str = odt_malloc (sizeof(odt_string_t)) ;
	str->m_data = odt_malloc (n) ;
	str->m_free = n ;
	str->m_len = 0 ;

	return str ;
}


odt_string_t
*odt_string_create_char (odt_string_t *str, const char *data, unsigned int data_len)
{
	odt_assert (NULL != data) ;
	odt_assert (0 <= data_len) ;

    int len ;

    // The 'str' must be empty while it's not NULL

    len = (data_len < ODT_STRING_MINSIZE_LIMIT)
           ? ODT_STRING_MINSIZE_LIMIT : data_len ;

    if (!str)
        str = odt_malloc (sizeof(odt_string_t)) ;
	str->m_len = data_len ;
	str->m_data = odt_malloc (len) ;
	memcpy (str->m_data, data, data_len) ;
	str->m_free = len - data_len ;

	return str ;
}




int
odt_string_append_len (odt_string_t *str,
                       const char *data,
                       unsigned int data_len)
{
    odt_assert (NULL != str) ;
    odt_assert (NULL != data) ;
    odt_assert (0 <= data_len) ;

    int len ;
    char *tmp ;


    len = str->m_len + str->m_free ;

    if (len <= 0)
        return odt_string_create_char (str, data, data_len) ;

    if (data_len > str->m_free) {

        len = len < data_len ? data_len : len ;
        tmp = odt_malloc((len*=2)) ;

        memcpy (tmp, str->m_data, str->m_len) ;
        odt_free (str->m_data) ;
        str->m_data = tmp ;
    }

    memcpy (str->m_data+str->m_len, data, data_len) ;
    str->m_len += data_len ;
    str->m_free = len - str->m_len ;

    return 0 ;
}


int
odt_string_append (odt_string_t *str, const char *data)
{
    return odt_string_append_len (str, data, strlen(data)) ;
}


int
odt_string_appendv (odt_string_t *str, size_t argc, ...)
{
    va_list ap ;
    char *data ;

    va_start (ap, argc) ;

    while (argc --) {

        data = va_arg (ap, char*) ;
        if (NULL == data)
            continue ;

        if (-1 == odt_string_append (str, data))
            return -1 ;
    }

    return 0 ;
}




int
odt_string_update (odt_string_t *str,
                   const char *data,
                   unsigned int data_len)
{
    odt_assert (NULL != str);
    odt_assert (NULL != data) ;
    odt_assert (0 <= data_len) ;

    size_t len ;

    len = str->m_len + str->m_free ;

    if (data_len > len) {

        if (NULL != str->m_data)
            odt_free (str->m_data) ;

        str->m_data = odt_malloc(data_len) ;
        len = data_len ;
    }

    // Avoid dumb mistake if 'data' is included in 'str'
    memmove (str->m_data, data, data_len) ;
    str->m_len = data_len ;
    str->m_free = len - data_len ;


    return 0; 
}


void
odt_string_clean (odt_string_t *string)
{
    odt_assert (NULL != string) ;

    if (string->m_data && string->m_len > 0)
        odt_free (string->m_data) ;

    *string = (odt_string_t) {0} ;
}


void
odt_string_clean_without_buffer (odt_string_t *string)
{
    odt_assert (NULL != string) ;

    string->m_free += string->m_len ;
    string->m_len = 0 ;
}
