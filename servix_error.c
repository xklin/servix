
#include "servix_global.h"

FILE *G_ERR_STREAM ;
#define SERVIX_LOG_NAME	".servix.log"


void
svxe_open_log ()
{
	G_ERR_STREAM = fopen (SERVIX_LOG_NAME, "a+") ;
	if (NULL == G_ERR_STREAM)
		exit (1) ;
}
 
void
svxe_exit ()
{
	fprintf (G_ERR_STREAM, "Servix exiting...\n") ;
	exit (1) ;
}


void
svxe_log (int log_level, const char *format, ...)
{

}


