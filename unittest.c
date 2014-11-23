
#include "servix_global.h"

#define IP	"172.20.30.104"
#define	PORT	10086

int main ()
{
	svx_socket sock ;
	svx_addr addr ;

	if (NULL == svx_sock_create (SOCK_TCP, &sock))
		exit (1) ;

	if (NULL == svx_addrv4_create (IP, PORT, &addr))
		exit (1) ;

	svx_sock_setnoblock (&sock) ;
//	svx_sock_setblock (&sock) ;

	int fd = sock.m_sock ;
	struct sockaddr_in *serv = &addr.m_addr ;
	if (0 > connect (fd, serv, sizeof(struct sockaddr_in)))
		perror ("connect failed") ;

	return 0 ;
}
