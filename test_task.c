
#include "odt.h"

odt_rtsp_t g_msg ;


void send_ (int what, void *arg)
{
    printf ("Sending complete\n") ;
}

void send_fail (int what, void *arg)
{
    printf ("Sending failed") ;
}

void connect_ (int what, void *arg)
{
    ODT_LOG_INFO("Connected to peer") ;

    odt_task_send_msg (*((odt_task_t **) arg),
        &g_msg, send_, send_fail, NULL, 10) ;
}


void connect_fail (int what, void *arg)
{
    printf ("Connection failed") ;
}


int main ()
{
    odt_event_manager_t *evm ;
    odt_string_t         str ;
    odt_task_t          *task ;

    evm = odt_event_manager_init () ;


    str = odt_string("RTSP/1.0               200 OK\r\n"
    "StartPoint: 1 0\r\n"
    "Transport: MP2T/DVBC/UDP;unicast;bandwidth=3750000;destination=80.9.107.1;client_port=1234;sop_name=SOP2\r\n"
    "Content-Type: text/plain\r\n"
    "CSeq: 1\r\n"
    "Session: 3128784676785782803\r\n"
    "Client: SSG:90B11C344423:11966\r\n"
    "Date: 2015-05-07 11:14:51\r\n"
    "Require: sessionctl.v1\r\n"
    "Content-length: 5\r\n"
    "\r\n"
    "abc\r\n") ;

    odt_rtsp_init (&g_msg) ;
    odt_rtsp_set_msg (&g_msg, &str) ;

    task = odt_task_connect (evm, &task, "127.0.0.1", 10086, connect_,
           connect_fail, connect_fail, &task, 1000) ;

    while (1) {

        odt_event_wait (evm) ;
    }
}
