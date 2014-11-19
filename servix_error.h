#ifndef __SERVIX_ERROR_INCLUED__
#define __SERVIX_ERROR_INCLUED__

/*
 * All the functions for svx error handling will be
 * prefixed with 'svxe'
 */

#include "servix_global.h"
void	svxe_exit () ;

void	svxe_log (int, const char *,...) ;


#endif
