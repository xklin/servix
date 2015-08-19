
#ifndef __ODT_TASK_HEAD__
#define __ODT_TASK_HEAD__


/*
typedef struct {

    union msg_s {

        odt_rtsp_t _rtsp ;
    }   ;

    odt_event_t            ev ;
    odt_event_manager_t   *evm ;

    odt_callback_func      send_cb ;
    odt_callback_func      send_cb_fail ;
    void                  *send_out_data ;

    struct {

        odt_string_t      *m_data ;
        int                m_pos ;
        struct timeval     m_reg_tv ;
        long               m_tv ;
        union msg_s       *m_msg ;

    }                      send_buf ;

    odt_callback_func      recv_ack_cb ;
    odt_callback_func      recv_announce_cb ;
    odt_callback_func      recv_cb_fail ;
    void                  *recv_out_data ;
    union msg_s            recv_msg ;

    odt_connection_t       conn ;
} odt_task_t ;
*/
typedef struct {

    // Sending part
    odt_callback_func      handle_send_done ;
    odt_callback_func      handle_send_fail ;
    struct {

        odt_string_t       m_data ;
        int                m_pos ;
        struct timeval     m_reg_tv ;
        long int           m_tv ;

    }                      send_msg_buffer ;
    void                  *send_out_data ;

    // Recieving part
    odt_msg_parser         parser ;


    // Terminate part
    odt_callback_func      handle_terminated ;
    void                  *term_out_data ;
    odt_event_t            term_ev ;

    odt_event_manager_t   *evm ;
    odt_event_t            ev ;
    odt_connection_t       conn ;

    unsigned long int      beatheart_timer;
} odt_task_t ;




int odt_task_send_msg (odt_task_t *,
                       odt_string_t *,
                       odt_callback_func ,
                       odt_callback_func ,
                       void *,
                       long);
odt_task_t *odt_task_connect (odt_event_manager_t *,
                              odt_task_t *,
                              const char *,
                              int ,
                              odt_callback_func ,
                              odt_callback_func ,
                              odt_callback_func ,
                              void *,
                              long) ;
int odt_task_reg_recv_cb (odt_task_t *, void *) ;
int odt_task_beatheart (odt_task_t *,
                        odt_callback_func ,
                        odt_callback_func ,
                        long int,
                        void *) ;
int odt_task_terminate (odt_task_t *, odt_callback_func, void *) ;



#endif
