
int G_ERR_FD ;
 
void
svxe_exit ()
{
	fprintf (G_ERR_FD, "Servix exiting...\n") ;
	exit (1) ;
}
