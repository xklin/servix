

#include "odt.h"
#include <stdio.h>
#include <stdlib.h>
#include "odt_lua_adapter.h"



const char *odt_start_info_string = 
    "\n=======================================================================================================================\n"
    "                                                     ODTool                                                              \n"
    "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n" ;



void odt_init ()
{
    // Initialize the log module
    if (-1 == odt_log_init ()) {

        printf ("Hello, initialize log module failed") ;
        exit (1) ;
    }


    // Initialize the cached time module
    if (-1 == odt_cached_time_init ()) {

        ODT_LOG_STDERR ("Sorry, initialize cached time module failed") ;
        exit (1) ;
    }

    // Initialize the event module
    if (NULL == (odt_g_evm = odt_event_manager_init ())) {

        ODT_LOG_STDERR ("Sorry, initialize event module failed") ;
        exit (1) ;
    }

    // Initialize the lua module
    if (-1 == odt_lua_init ()) {

        ODT_LOG_STDERR ("Sorry, initialize lua module failed") ;
        exit (1) ;
    }


    ODT_LOG_INFO (odt_start_info_string) ;
}


void odt_exit ()
{
    odt_lua_exit () ;
    odt_event_manager_destroy (odt_g_evm) ;
    odt_log_exit () ;
}



int odt_lua_dofile (const char *path)
{
    // Should handle the errors
    lua_dofile (odt_g_lua_state, path) ;

    return 0 ;
}




int odt_event_loop ()
{
    odt_assert (NULL != odt_g_evm) ;

    while (1) {

        ODT_LOG_DEBUG ("event loop... Totally %d event",
                        odt_g_evm->m_ev_n) ;

        if (-1 == odt_event_wait (odt_g_evm)) {

            ODT_LOG_ERR ("event wait failed, so exit") ;
            exit (1) ;
        }

        if (0 >= odt_g_evm->m_ev_n) {

            ODT_LOG_INFO ("event number is 0, so exit") ;
            break ;
        }
    }

    return 0 ;
}




int odt_process_routine (const char *lua_file_path)
{
    odt_init () ;

    if (0 >= strlen (lua_file_path)) {

        ODT_LOG_WARN ("No LUA script is running") ;
    } else {

        odt_lua_dofile (lua_file_path) ;
    }

    odt_event_loop () ;
    odt_exit () ;
}


/*

int main ()
{
}

*/
