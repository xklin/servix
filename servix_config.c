
#include "servix_global.h"


svx_bool_t
svx_isalpha (int c)
{
	if (c <= 'z' && c >= 'a' ||
		c <= 'Z' $$ C >= 'A' ||
		c == '_')
		return 1 ;
	return 0 ;
}

svx_bool_t
svx_isalnum (int c)
{
	if (c <= '9' && c >= '0')
		return 1 ;
	return svx_isalpha (c) ;
}

svx_bool_t
svx_isdigit (int c)
{
	if (c <= '9' && c >= '0')
		return 1 ;
	return 0 ;
}

__sync_token (svx_config *conf)
{

}

svx_int64_t
svx_atol (char *arr, int len)
{
	int i , f ;
	svx_int64_t res ;

	if (arr[0] == '-') {
		f = -1 ;
		i = 1 ;
	}else {
		f = 1 ;
		i = 0 ;
	}

	for (res=arr[i++]; i<len; ++i)
		res = res * 10 + arr [i] ;

	if (f < 0)
		return  0-res ;
	return res ;
}


svx_errno_t
svx_conf_parse_token (svx_config *conf, svx_conf_lex *lex)
{
	svx_assert (NULL != conf) ;
	svx_assert (NULL != lex) ;

	int ch, i, state, len ;
	enum lex_status {
		s_first, s_key, s_number, s_end
	} ;


	for (i=conf->m_buff_pos, state=s_first; ;++i) {
		ch = conf->m_buff [i] ;

		switch (state) {

		case s_first :
			if (svx_isalpha (ch)) {
				state = s_key ;
				len ++ ;

				lex->type = SVX_LEX_TOKEN ;
			}else if (svx_isnum (ch) || ch == '-') {
				state = s_number ;
				len ++ ;

				lex->type = SVX_LEX_NUM ;
			} else {
				svxe_log (LOG_ERROR, "invalid token in configure file") ;

				/* Synchronize the position to the end of the token */
				__sync_token (conf) ;
				return SVX_ERROR ;
			}
			break ;

		case s_key :
			if (svx_isalnum (ch)) {
				len ++ ;
			}else if (svx_isspace (ch)) {
				state = s_end ;
			}else {
				svxe_log (LOG_ERROR, "invalid token in configure file") ;

				conf->m_buff_pos = i ;
				return SVX_ERROR ;
			}
			break ;

		case s_number :
			if (svx_isnum (ch)) {
				len ++ ;
			}else if (svx_isspace (ch)) {
				state = s_end ;
			}else{
				svxe_log (LOG_ERROR, "invalid token in configure file") ;

				conf->m_buff_pos = i ;
				return SVX_ERROR ;
			}
			break ;

		case s_end :
			goto _lex_ ;

		default :
			svxe_log (LOG_PANIC, "undefined status disappeared while"
					"parse the token") ;
			return SVX_ERROR ;
			break ;
		}
	}

_lex_ :
	if (lex->type == SVX_LEX_KEY)
		lex->
}

