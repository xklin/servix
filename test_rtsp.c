
#include "odt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

/*
void
print_msg(odt_rtsp_t *msg)
{
    odt_rbtree_node_t *node ;
    char buf [128] ;
    rtsp_fields_t *field ;

    printf ("verb: ") ;
    strncpy (buf, msg->verb.m_data,127) ;
    buf[msg->verb.m_len]=0 ;
    printf ("[%s]  ", buf) ;

    printf ("url path: ") ;
    strncpy (buf, msg->url_path.m_data,127) ;
    buf[msg->url_path.m_len]=0 ;
    printf ("[%s]  ", buf) ;

    printf ("url port: ") ;
    strncpy (buf, msg->url_port.m_data,127) ;
    buf[msg->url_port.m_len]=0 ;
    printf ("[%s]  ", buf) ;
    printf ("type: ") ;
    strncpy (buf, msg->type.m_data,127) ;
    buf[msg->type.m_len]=0 ;
    printf ("[%s]  ", buf) ;

    printf ("version: ") ;
    strncpy (buf, msg->version.m_data,127) ;
    buf[msg->version.m_len]=0 ;
    printf ("[%s]  \n", buf) ;

    printf ("code: ") ;
    strncpy (buf, msg->code.m_data, 127) ;
    buf[msg->code.m_len] = 0 ;
    printf ("[%s]  \n", buf) ;

    printf ("desc: ") ;
    strncpy (buf, msg->desc.m_data, 127) ;
    buf[msg->desc.m_len] = 0 ;
    printf ("[%s]  \n", buf) ;

    while (node = odt_rbtree_min(msg->fields)) {

        strncpy (buf, ((odt_string_t *)node->m_key)->m_data,127) ;
        buf[((odt_string_t *)node->m_key)->m_len]=0 ;
        printf ("key: [%s]  ", buf) ;

        field = (rtsp_fields_t*) node->m_elt ;
        strncpy (buf, field->value.m_data,127) ;
        buf[field->value.m_len]=0 ;
        printf ("value: [%s]\n", buf) ;

        if (-1 == odt_rbtree_remove (msg->fields, node))
            printf ("remove node from tree failed\n") ;
    }

    printf ("Body:\n%s\n", msg->body.m_data) ;
}



int main ()
{
    int n ;
    int ret ;
    odt_rtsp_t msg ;
    char *p ;
    odt_string_t *string ;
    char buf [1024] = {
    //"TEARDOWN      rtsp://172.30.19.112:9099/ RTSP/1.0\r\n"
    "RTSP/1.0               200 OK\r\n"
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
    "abc\r\n"
    } ;

    assert (-1 != odt_rtsp_init (&msg)) ;


    for (p=buf; msg.state!=S_COMPLETE; ++p) {

        odt_assert (p-buf <= strlen(buf)) ;
        if (-1 == odt_rtsp_parse(&msg, p, 1))
            exit (1) ;

        printf ("state : %d\n", msg.state) ;
        printf ("content_length: %d\n", msg.content_length) ;

    }

    print_msg (&msg) ;
}


*/


char *fields_char [] = {

    "StartPoint", "1 0",
    "CSeq", "1",
    "Content-Type", "text/xml",
    "Session", "31123774991783737648",
    "Content-length", "1065",
    "Require", "sessionctl.v1",
    "OnDemandSessionId", "aeohy982qhfhao982afeh",
    NULL
} ;


int main ()
{
    odt_rtsp_t msg ;
    odt_string_t str ;
    odt_string_t key, value;
    odt_string_t *message ;
    int n ;

    odt_rtsp_init (&msg) ;
    msg.req = 1 ;

    str = odt_string("TEARDOWN") ;
    odt_rtsp_set_verb (&msg, &str) ;

    str = odt_string("10.10.64.162") ;
    odt_rtsp_set_url_path (&msg, &str) ;

    str = odt_string("10086") ;
    odt_rtsp_set_url_port (&msg, &str) ;

    str = odt_string("RTSP") ;
    odt_rtsp_set_type(&msg, &str) ;

    str = odt_string("1.0") ;
    odt_rtsp_set_version(&msg, &str) ;

    for (n=0; fields_char[n]; n+=2) {

        key = odt_nstring(fields_char[n], strlen(fields_char[n]));
        value = odt_nstring(fields_char[n+1], strlen(fields_char[n+1]));

        odt_rtsp_set_hdr (&msg, &key, &value) ;
    }

    str = odt_string("189123h 23rh0hf 83\r\no3hq83 38qhnf he3qa0fh3haifu \r\naeohafehafehj1012734098230hr308hf");
    odt_rtsp_set_body (&msg, &str) ;

    message = odt_rtsp_get_msg (&msg) ;
    message->m_data[message->m_len] = 0 ;

    printf ("%s\n", message->m_data) ;
}
