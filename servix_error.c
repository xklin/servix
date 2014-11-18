#include "servix_global.h"
FILE *G_ERR_STREAM ;
 
void
svxe_exit ()
{
	fprintf (G_ERR_STREAM, "Servix exiting...\n") ;
	exit (1) ;
}
