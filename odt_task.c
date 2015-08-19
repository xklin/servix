
#include "odt.h"




void task_on_triggled (int what, void *arg) ;
void exception_on_triggled (int what, void *arg) ;




void
deal_connection_error (int what, odt_task_t *task)
{
    odt_assert (NULL != task) ;

    int fd , error, len ;

    fd = task->conn.m_fd ;

    len = sizeof(int) ;
    if (-1 == getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len)) {

        ODT_LOG_EMERG("'getsockopt' failed, errno=%d", errno) ;
        return ;
    }

    ODT_LOG_INFO("Socket error detected, errno=%d", error) ;
}




void do_send_msg (odt_task_t *task)
{
	odt_assert (NULL != task) ;

	int ret ;
	int fd ;
    char *data ;
    typeof(task->send_msg_buffer) *sbuf = &task->send_msg_buffer ;



	fd = task->conn.m_fd ;
    data = sbuf->m_data.m_data ;

    if (sbuf->m_data.m_len == sbuf->m_pos)
        goto sending_complete ;

	ret = send (fd, data+sbuf->m_pos, sbuf->m_data.m_len - sbuf->m_pos, 0) ;

	if (0 > ret) {

		if (errno != EINPROGRESS && errno != EAGAIN) {

            ODT_LOG_ERR("'send' failed, errno=%d", errno) ;
			goto fail_label ;
		}

		ret = 0 ;
	}

	sbuf->m_pos += ret ;
	if (sbuf->m_data.m_len == sbuf->m_pos) {

		/* Sending completely */
sending_complete :

        // Clean up send buffer
        odt_string_clean_without_buffer (&sbuf->m_data) ;
        sbuf->m_pos = 0 ;

		if (-1 == odt_event_add (task->evm, &task->ev, ODT_EV_READ, -1))
			goto fail_label ;


        if (task->handle_send_done)
            task->handle_send_done (ODT_EV_WRITE, task->send_out_data) ;

		return ;
	}


	/* Need more loops to send the rest messages */

    sbuf->m_tv = CYCLE_SEC()*1000 + CYCLE_MSEC() - 
                (sbuf->m_reg_tv.tv_sec*1000 +
                 sbuf->m_reg_tv.tv_usec/1000) ;

	sbuf->m_tv = (sbuf->m_tv < 0) ? 0 : sbuf->m_tv ;
    cycle2timeval (&sbuf->m_reg_tv) ;

	return ;
	

fail_label :
    if (task->handle_send_fail)
        task->handle_send_fail (ODT_EV_WRITE, task->send_out_data);
}



void do_send_msg_fail (odt_task_t *task)
{
    odt_assert (NULL != task) ;
    typeof(task->send_msg_buffer) *sbuf = &task->send_msg_buffer ;


    odt_string_clean (&sbuf->m_data) ;
	if (-1 == odt_event_del (task->evm, &task->ev))
        goto fail_label ;

	/* Register readable event */

	odt_event_add (task->evm, &task->ev, ODT_EV_READ, -1) ;

fail_label :
    if (task->handle_send_fail)
        task->handle_send_fail (ODT_EV_WRITE, task->send_out_data);
}



int odt_task_send_msg (odt_task_t *task,
                       odt_string_t *msg,
                       odt_callback_func cb,
                       odt_callback_func cb_fail,
                       void *arg,
                       long tv)
{
    odt_string_t *str ;
    typeof(task->send_msg_buffer) *sbuf = &task->send_msg_buffer ;


	/* It's not connected */
	if (!task->conn.m_connect) {

        ODT_LOG_ERR("Try to send datas on a disconnected socket") ;
		return -1 ;
    }

	if (-1 == odt_event_add (task->evm, &task->ev, ODT_EV_RDWR, tv)) {

        ODT_LOG_ERR("Add event faild") ;
		return -1 ;
	}



	if (tv >= 0)
        cycle2timeval (&sbuf->m_reg_tv) ;


    if (0 > odt_string_append_len (&sbuf->m_data, msg->m_data,
                        msg->m_len)) {

        ODT_LOG_ERR("Update string failed") ;
        return -1 ;
    }

    sbuf->m_tv = tv ;
    sbuf->m_reg_tv = (struct timeval) {0} ;
    task->handle_send_done = cb ;
    task->handle_send_fail = cb_fail ;
    task->send_out_data = arg ;

	return 0 ;
}




void
do_recv_msg (odt_task_t *task)
{
    odt_assert (NULL != task) ;

    int fd ;
    int ret ;
    odt_rtsp_t *msg ;
    char buf [1024] ;
    odt_msg_parser *parser ;

    fd = task->conn.m_fd ;
    parser = &task->parser ;


    do {

        ret = recv (fd, buf, 1024, 0) ;

        if (0 > ret) {

            if (errno == EINTR) {

                ODT_LOG_WARN ("'recv' interrupted");
                continue ;
            } else if (errno == EINPROGRESS || errno == EAGAIN) {

                ODT_LOG_DEBUG ("'recv' meet EAGAIN");
                break ;
            } else {

                ODT_LOG_EMERG("'recv' failed, errno=%d", errno) ;
                goto fail_label ;
            }
        } else if (0 == ret) {

            /* Closed by peer */
            if (-1 == odt_event_del (task->evm, &task->ev))
                ODT_LOG_ERR("Socket closed by peer, delete event failed") ;

            goto fail_label ;
        }

        if (-1 == parser->append (parser, buf, ret)) {

            ODT_LOG_ERR("Append message failed");
            goto fail_label ;
        }

        if (-1 == parser->parse (parser)) {

            ODT_LOG_ERR("Parse message failed");
            goto fail_label ;
        }
    } while (ret >= 1024) ;

    return ;


fail_label :
    if (parser->handle_parse_fail)
        parser->handle_parse_fail (parser) ;
}



void
do_recv_msg_fail (odt_task_t *task)
{
    odt_assert (NULL != task) ;
    if (task->parser.handle_parse_fail)
        task->parser.handle_parse_fail (&task->parser) ;
}





int
odt_task_reg_recv_cb (odt_task_t       *task,
                      void             *arg)
{
    if (!task->ev.m_add) {

        if (-1 == odt_event_add (task->evm, &task->ev, ODT_EV_READ, -1)) {

            ODT_LOG_ERR("Add event failed") ;
            return -1 ;
        }
    }

    task->parser.init (&task->parser, arg) ;
    return 0 ;
}






/*
 * This function is as the callback function to registered 
 * in an event. Readable and writable events are all registered.
 * While some event triggled, this funcion will be called,
 * it chose to send messages or recieve messages depands on
 * the type of the event. If it should send messages, then it
 * call 'dosendmsg'. Or if it should recieve messages it will
 * try to recieve datas and parse them one by one. The valid
 * messages can be parsed with being marked 'ANNOUNCE' or 'ACK'.
 */

void
exception_on_triggled (int what, void *arg)
{
    odt_task_t *task = (odt_task_t *) arg ;

    if (what & (EPOLLERR | EPOLLHUP))
        deal_connection_error (what, task) ;

    if (what & EPOLLIN)
        do_recv_msg_fail (task) ;
    else if (what & EPOLLOUT)
        do_send_msg_fail (task) ;
    else
        ODT_LOG_EMERG("Unexpected event : %d", what) ;
}

void
task_on_triggled (int what, void *arg)
{
    odt_task_t *task = (odt_task_t *) arg ;

    if (what & (EPOLLERR | EPOLLHUP)) {

        deal_connection_error (what, task) ;
        return ;
    }

    if (what & EPOLLIN)
        do_recv_msg (task) ;
    else if (what & EPOLLOUT)
        do_send_msg (task) ;
    else
        ODT_LOG_EMERG("Unexpected event : %d", what) ;
}



odt_task_t
*odt_task_connect (odt_event_manager_t *evm,
                   odt_task_t *task,
                   const char *ip,
                   int port,
                   odt_callback_func cb,
                   odt_callback_func cb_fail,
                   odt_callback_func cb_timeout,
                   void *arg,
                   long tv)
{
    if (!task)
        task = odt_malloc (sizeof(odt_task_t)) ;

    *task = (odt_task_t) {

        .evm = evm,
        .conn = (odt_connection_t) {

            .m_linger = 1,
            .m_nonblock = 1,
            .m_port = port,
            .cb = cb,
            .cb_conn_fail = cb_fail,
            .cb_timeout = cb_timeout,
            .cb_arg = arg,
            .cb_conn_fail_arg = arg,
            .cb_timeout_arg = arg,
            .m_tv_ms = tv
        },
    } ;

    strncpy (task->conn.m_ip, ip, 16) ;

    if(-1 == odt_connection_doconn (&task->conn, task->evm))
        return NULL ;


    odt_event_new (&task->ev, task->conn.m_fd, task_on_triggled,
                   task, exception_on_triggled, task) ;

    task->parser = (odt_msg_parser) {0} ;

    ODT_LOG_DEBUG ("[event %p] Task connected", &task->ev) ;
    return task ;
}





int
odt_task_beatheart (odt_task_t        *task,
                    odt_callback_func  cb,
                    odt_callback_func  collect,
                    long int           tv,
                    void              *arg)
{
    odt_assert (NULL != task) ;

    tv = tv >= 0 ? tv : 0 ;
    task->beatheart_timer =
           odt_settimer (task->evm, tv, tv, cb, collect, arg) ;
    ODT_LOG_DEBUG ("New beatheart timer : %p", task->beatheart_timer);
    return (task->beatheart_timer != 0) ;
}



void
_odt_task_terminate (int what, void *arg)
{
    odt_task_t *task = (odt_task_t *) arg ;

    // Delete event
    if (-1 == odt_event_del (task->evm, &task->ev)) {

        ODT_LOG_ERR ("Delete event failed when terminate task") ;
        return ;
    }

    if (-1 == odt_connection_disconn (&task->conn)) {

        ODT_LOG_ERR ("Disconnect failed when terminate task");
        return ;
    }

    odt_string_clean (&task->send_msg_buffer.m_data) ;
    task->parser.destroy (&task->parser) ;

    // Delete beatheart timer
    if (task->beatheart_timer) {

        if (0 > odt_deltimer (task->beatheart_timer)) {

            ODT_LOG_ERR ("Delete beatheart timer failed when terminate"
                         " task");
            return ;
        }
    }

    ODT_LOG_INFO ("[event %p] Task terminated", &task->ev) ;

    if (task->handle_terminated)
        task->handle_terminated (0, task->term_out_data) ;
}

int
odt_task_terminate (odt_task_t *task, odt_callback_func cb, void *arg)
{
    ODT_LOG_DEBUG ("[event %p]Require to terminate task", &task->ev) ;

    odt_event_new (&task->term_ev, task->ev.m_fd, _odt_task_terminate,
                   task, NULL, NULL) ;

    if (0 > odt_event_post (task->evm, &task->term_ev, ODT_EV_READY)) {

        ODT_LOG_ERR ("Task termination posted failed");
        return -1 ;
    }

    task->handle_terminated = cb ;
    task->term_out_data = arg ;

    return 0 ;
}

