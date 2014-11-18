
#include "servix_global.h"

#define SERVER_IP	"127.0.0.1"
#define SERVER_PORT	80


// Messages
char VALID_GET_MSG [] = "" ;
#define VALID_GET_MSG_LEN	sizeof(VALID_GET_MSG)
char INVALID_GET_MSG [] = "" ;
#define INVALID_GET_MSG_LEN	sizeof(INVALID_GET_MSG)

char *ARR[] = {VALID_GET_MSG, INVALID_GET_MSG } ;
#define _ARR_LEN_  2


int wait_msg ()
{
	int n ;
	scanf ("%d", &n) ;
	if (n >= _ARR_LEN_)
		exit (1) ;
	return n ;
}


int main (int argc, char *argv[])
{
	svx_socket sock ;
	svx_addr addr ;

	svx_sock_create (SOCK_TCP, &sock) ;
	svx_addrv4_create (SERVER_IP, SERVER_PORT, &addr) ;

	// Connect to the server
	if (0 > svx_connect (&sock, &addr))
		svxe_exit () ;

	while (1) {
		printf ("Choose msg\n") ;
		char *msg = ARR [wait_msg ()] ;
		// Build buffer
		SVX_BUF_DECLARATION (1, VALID_GET_MSG_LEN, buff) ;
		// Send message
		if (0 > svx_send (&sock, &addr, &buff))
			svxe_exit () ;
	}
	return 0 ;
}
