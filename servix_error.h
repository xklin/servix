#ifndef __SERVIX_ERROR_INCLUED__
#define __SERVIX_ERROR_INCLUED__

/*
 * All the functions for svx error handling will be
 * prefixed with 'svxe'
 */

#include "servix_global.h"
void	svxe_exit () ;

void	svxe_log (int, const char *,...) ;


#define	LOG_PANIC	0	// The program has got a big mistake that it should be terminated
#define LOG_EMGER	1	// The program has got a big mistake that it should be stopped
#define LOG_ERROR	2	// The program has got a big mistake that it cannot provide normal service 
#define LOG_WARN	3	// The program has got a mistake that it cannot work out some features and these features are important to its performance
#define LOG_NOTICE	4	// The program has got a mistake that it cannot work out some features but these features are not so important
#define LOG_DEBUG	5	// The program has some informations should be print while debugging
#define LOG_INFO	6	// The program should print some informations
#define LOG_TRACE	7	// The program should traceing the running route

#endif
