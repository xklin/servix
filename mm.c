
#include "odt.h"
#include <stdlib.h>
#include <stdio.h>

void
*odt_malloc (size_t size)
{
	void *dst ;
	dst = malloc (size) ;
	odt_assert (NULL != dst) ;
	return dst ;
}


void
odt_free (void *dst)
{
	odt_assert (NULL != dst) ;
	free (dst) ;
}
