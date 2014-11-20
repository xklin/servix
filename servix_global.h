#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <errno.h>
#include <assert.h>



#ifndef	__SERVIX_GLOBAL_INCLUDED__
#define __SERVIX_GLOBAL_INCLUDED__

typedef struct servix_socket	svx_socket ;
typedef struct servix_addr		svx_addr ;
typedef struct servix_buffer	svx_buff ;
typedef enum socket_type		svx_type ;
typedef struct servix_listen	svx_listen ;
typedef struct servix_conf		svx_conf ;
typedef struct servix_array		svx_array ;

typedef unsigned int			svx_uint32_t ;
typedef unsigned long			svx_uint64_t ;
typedef int						svx_bool ;

#include "servix_socket.h"
#include "servix_buffer.h"
#include "servix_error.h"
#include "servix_conf.h"
#include "servix_listen.h"


#endif
