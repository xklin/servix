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
#include <netinet/tcp.h>



#ifndef	__SERVIX_GLOBAL_INCLUDED__
#define __SERVIX_GLOBAL_INCLUDED__

/*	Define the base type of servix */
typedef unsigned int			svx_uint32_t ;
typedef unsigned long			svx_uint64_t ;
typedef long					svx_int64_t ;
typedef int						svx_int32_t ;
typedef int						svx_bool_t ;
typedef unsigned char			svx_uint8_t ;
typedef int						svx_errno_t ;
typedef unsigned int			svx_hash_t ;



#ifndef __SERVIX_DEBUG__
/*	Define the module types of servix */
typedef struct servix_socket				svx_socket ;
typedef struct servix_addr					svx_addr ;
typedef struct servix_buffer				svx_buff ;
typedef enum socket_type					svx_type ;
typedef struct servix_listen				svx_listen ;
typedef struct servix_config				svx_config ;
typedef struct servix_array					svx_array ;
typedef struct servix_string_hash_key		svx_str_hash_key ;
typedef struct servix_string_hash_table		svx_str_hash_table ;
typedef struct servix_string_hash_elt		svx_str_hash_elt ;

#include "servix_socket.h"
#include "servix_buffer.h"
#include "servix_conf.h"
#include "servix_listen.h"
#include "servix_array.h"
#include "servix_hash.h"

#else

#if defined __SERVIX_DEBUG_LISTEN__

#elif defined __SERVIX_DEBUG_SOCKET__
typedef struct servix_socket	svx_socket ;
typedef struct servix_addr		svx_addr ;
#include "servix_socket.h"

#elif defined __SERVIX_DEBUG_ARRAY__
typedef struct servix_array		svx_array ;
#include "servix_array.h"


#elif defined __SERVIX_DEBUG_STRING_HASH__
typedef struct servix_array					svx_array ;
typedef struct servix_string_hash_key		svx_str_hash_key ;
typedef struct servix_string_hash_table		svx_str_hash_table ;
typedef struct servix_string_hash_elt		svx_str_hash_elt ;
#include "servix_array.h"
#include "servix_hash.h"

#endif
#endif	// SERVIX_DEBUG

#include "servix_error.h"

#endif	// SERVIX_GLOBAL_INCLUDED
