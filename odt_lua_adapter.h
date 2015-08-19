
#ifndef __ODT_LUA_ADAPTER_HEAD__
#define __ODT_LUA_ADAPTER_HEAD__


#include "odt.h"




// Writen by myself, just for laziness
#define lua_getlglobal(L, s, n) \
        ( lua_pushlstring (L, s, n), lua_gettable (L, LUA_GLOBALSINDEX))





static inline int
odt_lua_new_msg (lua_State *L)
{
    odt_rtsp_t *msg ;

    if (1 > lua_gettop (L)) {

        ODT_LOG_STDERR ("Need more arguments in 'new_msg'") ;
        exit (1) ;
    }

    lua_pushboolean (L, 1) ;
    msg = (odt_rtsp_t *) lua_newuserdata (L, sizeof(odt_rtsp_t)) ;

    if (-1 == odt_rtsp_init (msg)) {

        ODT_LOG_ERR("Initialize the massege structure failed") ;
        lua_pushboolean (L, 0) ;
        return 1 ;
    }

    msg->req = (int) lua_toboolean(L, 1) ;

    return 2 ;
}



static inline int
odt_lua_fill_reqline (lua_State *L)
{
    odt_rtsp_t *msg ;
    char *verb, *url_path, *type, *version ;
    size_t verb_len, url_path_len, type_len, version_len, url_port ;
    odt_string_t string ;


    if (6 > lua_gettop (L)) {

        ODT_LOG_STDERR ("Need more arguments in 'fill_reqlin'") ;
        exit (1) ;
    }

    msg = (odt_rtsp_t *) lua_touserdata (L, 1) ;
    verb = lua_tostring (L, 2) ;
    verb_len = lua_strlen(L, 2) ;
    url_path = lua_tostring (L, 3) ;
    url_path_len = lua_strlen(L, 3) ;
    url_port = (size_t) lua_tonumber (L, 4) ;
    type = lua_tostring (L, 5) ;
    type_len = lua_strlen(L, 5) ;
    version = lua_tostring (L, 6) ;
    version_len = lua_strlen(L, 6) ;

    if (NULL == msg || !msg->req) {

        lua_pushboolean (L, 0) ;
        return 1 ;
    }


    string = odt_nstring (verb, verb_len) ;
    odt_rtsp_set_verb (msg, &string) ;

    string = odt_nstring (url_path, url_path_len) ;
    odt_rtsp_set_url_path (msg, &string) ;

    odt_rtsp_set_url_port (msg, url_port) ;

    string = odt_nstring (type, type_len) ;
    odt_rtsp_set_type (msg, &string) ;

    string = odt_nstring (version, version_len) ;
    odt_rtsp_set_version (msg, &string) ;

    lua_pushboolean (L, 1) ;
    return 1 ;
}



static inline int
odt_lua_fill_statusline (lua_State *L)
{
    odt_rtsp_t *msg ;
    char *type, *version, *desc ;
    size_t type_len, version_len, desc_len, code ;
    odt_string_t string ;


    if (5 > lua_gettop (L)) {

        ODT_LOG_STDERR ("Need more arguments in 'fill_statusline'") ;
        exit (1) ;
    }

    msg = (odt_rtsp_t *) lua_touserdata(L, 1) ;
    type = lua_tostring (L, 2) ;
    type_len = lua_strlen(L, 2) ;
    version = lua_tostring (L, 3) ;
    version_len = lua_strlen(L, 3) ;
    code = (size_t) lua_tonumber (L, 4) ;
    desc = lua_tostring (L, 5) ;
    desc_len = lua_strlen(L, 5) ;

    if (NULL == msg || msg->req) {

        lua_pushboolean(L, 0) ;
        return 1 ;
    }

    string = odt_nstring (type, type_len) ;
    odt_rtsp_set_type (msg, &string) ;

    string = odt_nstring (version, version_len) ;
    odt_rtsp_set_version (msg, &string) ;

    odt_rtsp_set_code (msg, code) ;

    string = odt_nstring (desc, desc_len) ;
    odt_rtsp_set_desc (msg, &string) ;

    lua_pushboolean (L, 1) ;
    return 1 ;
}



static inline int
odt_lua_fill_hdr (lua_State *L)
{
    odt_rtsp_t *msg ;
    char *key, *value ;
    size_t key_len, value_len ;
    odt_string_t skey, svalue ;


    if (3 > lua_gettop (L)) {

        ODT_LOG_STDERR ("Need more arguments in 'fill_hdr'") ;
        exit (1) ;
    }

    msg = (odt_rtsp_t *)lua_touserdata(L, 1) ;
    key = lua_tostring (L, 2) ;
    key_len = lua_strlen(L, 2) ;
    value = lua_tostring (L, 3) ;
    value_len = lua_strlen(L, 3) ;

    if (NULL == msg) {

        lua_pushboolean(L, 0) ;
        return 1 ;
    }

    skey = odt_nstring (key, key_len) ;
    svalue = odt_nstring (value, value_len) ;

    odt_rtsp_set_hdr (msg, &skey, &svalue) ;

    lua_pushboolean(L, 1) ;
    return 1 ;
}




static inline int
odt_lua_fill_body (lua_State *L)
{
    odt_rtsp_t *msg ;
    const char *body , *type ;
    odt_string_t string, string2;
    int body_len, type_len ;

    if (3 > lua_gettop (L)) {

        ODT_LOG_STDERR ("Need more argument") ;
        exit (1) ;
    }

    msg = (odt_rtsp_t *) lua_touserdata(L, 1) ;
    body = lua_tostring(L,2) ;
    body_len = lua_strlen(L, 2) ;
    type = lua_tostring (L, 3) ;
    type_len = lua_strlen (L, 3) ;

    if (NULL == msg) {

        lua_pushboolean(L, 0) ;
        return 1 ;
    }

    string = odt_nstring (body, body_len) ;
    if (-1 == odt_rtsp_set_body (msg, &string)) {

        lua_pushboolean (L, 0) ;
        return 1 ;
    }


    // Fill the text type Header
    string2 = odt_nstring (type, type_len) ;
    string = odt_string (RTSP_CONTENT_TYPE) ;

    if (-1 == odt_rtsp_set_hdr (msg, &string, &string2)) {

        lua_pushboolean (L, 0) ;
        return 1 ;
    }

    lua_pushboolean(L, 1) ;
    return 1 ;
}



static inline int
odt_lua_get_addr (lua_State *L)
{
    odt_task_t *task ;
    odt_connection_t *conn ;

    if (1 > lua_gettop (L)) {

        ODT_LOG_STDERR ("Need more arguments in 'get_addr'");
        exit (1) ;
    }

    task = (odt_task_t *) lua_touserdata (L, 1);
    conn = &task->conn ;

    lua_pushstring (L, conn->m_ip);
    lua_pushnumber (L, conn->m_port);
    return 2 ;
}




typedef struct {

    char           cbs [8][32] ;
    int            cbslen [8] ;
    odt_session_t *session ;
    odt_task_t    *task ;
    lua_State     *L ;
    void          *arg;
} lua_callback_ctx_t ;



static void
lua_pcall_print_error (lua_State *L, const char *func)
{
    const char *err ;

    err = lua_tostring (L, -1) ;
    ODT_LOG_STDERR ("Lua script error : %s [%s]", err, func) ;
    lua_pop (L, 1);
}



static inline void
odt_lua_send_callback_func (int what, void *arg)
{
    lua_callback_ctx_t *ctx ;
    odt_task_t *task ;
    int pos ;

    if (!arg)
        return ;

    ctx = (lua_callback_ctx_t *) arg ;
    task = ctx->task ;
    
    lua_getlglobal (ctx->L, ctx->cbs[0], ctx->cbslen[0]) ;
    lua_pushlightuserdata (ctx->L, task) ;
    lua_pushlightuserdata (ctx->L, ctx->arg) ;

    if (0 != lua_pcall (ctx->L, 2, 0, 0))
        lua_pcall_print_error (ctx->L, ctx->cbs[0]) ;

    odt_free (ctx) ;
}



static inline void
odt_lua_send_fail_callback_func (int what, void *arg)
{
    lua_callback_ctx_t *ctx ;
    odt_task_t *task ;

    if (!arg)
        return ;

    ctx = (lua_callback_ctx_t *) arg ;
    task = ctx->task ;

    lua_getlglobal (ctx->L, ctx->cbs[1], ctx->cbslen[1]) ;
    lua_pushlightuserdata (ctx->L, task) ;
    lua_pushlightuserdata (ctx->L, ctx->arg) ;
    if (0 != lua_pcall (ctx->L, 2, 0, 0))
        lua_pcall_print_error (ctx->L, ctx->cbs[1]) ;

    odt_free (ctx) ;
}




static inline int
odt_lua_send_msg (lua_State *L)
{
    odt_rtsp_t *msg ;
    odt_task_t *task ;
    char *cb , *cb_fail ;
    int cb_slen, cb_fail_slen ;
    int tv ;
    lua_callback_ctx_t *ctx ;
    odt_string_t *string ;
    void *arg ;


    if (6 > lua_gettop (L)) {

        ODT_LOG_STDERR ("Need more arguments in 'send_msg'") ;
        exit (1) ;
    }

    msg = (odt_rtsp_t *) lua_touserdata(L, 1) ;
    task = (odt_task_t *) lua_touserdata(L, 2);
    cb = lua_tostring (L, 3) ;
    cb_slen = lua_strlen (L, 3) ;
    cb_fail = lua_tostring (L, 4) ;
    cb_fail_slen = lua_strlen (L, 4) ;
    tv = (int) lua_tonumber (L, 5) ;
    arg = lua_touserdata (L, 6);


    ctx = odt_malloc (sizeof(lua_callback_ctx_t)) ;
    *ctx = (lua_callback_ctx_t) {

        .task = task,
        .L = L,
        .arg = arg,
    } ;

    memcpy (ctx->cbs[0], cb, cb_slen) ;
    ctx->cbslen[0] = cb_slen ;
    memcpy (ctx->cbs[1], cb_fail, cb_fail_slen) ;
    ctx->cbslen[1] = cb_fail_slen ;


    string = odt_rtsp_get_msg (msg) ;
    if (NULL == string) {

        odt_free (ctx) ;
        lua_pushboolean (L, 0) ;
        return 1 ;
    }

    if (-1 == odt_task_send_msg (task, string,
          odt_lua_send_callback_func,
          odt_lua_send_fail_callback_func,
          ctx, tv)) {

        odt_free (ctx) ;
        lua_pushboolean (L, 0) ;
        return 1 ;
    }


    // Cleanup 'msg'
    odt_rtsp_destroy (msg) ;

    lua_pushboolean (L, 1) ;
    return 1 ;
}



static inline void
odt_lua_recv_ack_callback_func (int what, void *arg)
{
    lua_callback_ctx_t *ctx ;
    odt_task_t *task ;
    odt_rtsp_t *msg ;

    if (!arg)
        return ;

    ctx = (lua_callback_ctx_t *) arg ;
    task = ctx->task ;

    if (!task) {

        ODT_LOG_STDERR ("'session' or 'task' is NULL in ack") ;
        exit (1) ;
    }

    msg = (odt_rtsp_t *) task->parser.msg ;

    lua_getlglobal (ctx->L, ctx->cbs[0], ctx->cbslen[0]) ;
    lua_pushlightuserdata (ctx->L, task) ;
    lua_pushlightuserdata (ctx->L, msg) ;
    lua_pushlightuserdata (ctx->L, ctx->arg) ;
    if (-1 ==lua_pcall (ctx->L, 3, 0, 0))
        lua_pcall_print_error (ctx->L, ctx->cbs[0]) ;
}




static inline void
odt_lua_recv_announce_callback_func (int what, void *arg)
{
    lua_callback_ctx_t *ctx ;
    odt_task_t *task ;
    odt_rtsp_t *msg ;

    if (!arg)
        return ;

    ctx = (lua_callback_ctx_t *) arg ;
    task = ctx->task ;

    if (!task) {

        ODT_LOG_STDERR ("'session' or 'task' is NULL in announce") ;
        exit (1) ;
    }

    msg = (odt_rtsp_t *) task->parser.msg ;

    lua_getlglobal (ctx->L, ctx->cbs[1], ctx->cbslen[1]) ;
    lua_pushlightuserdata (ctx->L, task) ;
    lua_pushlightuserdata (ctx->L, msg) ;
    lua_pushlightuserdata (ctx->L, ctx->arg) ;
    if (0 != lua_pcall (ctx->L, 3, 0, 0))
        lua_pcall_print_error (ctx->L, ctx->cbs[1]) ;
}




static inline void
odt_lua_recv_fail_callback_func (int what, void *arg)
{
    lua_callback_ctx_t *ctx ;

    if (!arg)
        return ;

    ctx = (lua_callback_ctx_t *) arg ;

    lua_getlglobal (ctx->L, ctx->cbs[2], ctx->cbslen[2]) ;
    lua_pushlightuserdata (ctx->L, ctx->task) ;
    lua_pushlightuserdata (ctx->L, ctx->arg) ;
    if (0 != lua_pcall (ctx->L, 2, 0, 0))
        lua_pcall_print_error (ctx->L, ctx->cbs[2]) ;
}



/*
 * This is a callback function for session module to 
 * destroy a specific 'odt_session_recv_interface_t'
 * data structure
 */
static inline void
odt_lua_recv_interface_destroy (odt_session_recv_interface_t *inte)
{
    odt_free (inte) ;
}




static inline int
odt_lua_reg_on_recv (lua_State *L)
{
    char *ack, *announce, *fail ;
    int ack_slen, announce_slen, fail_slen ;
    odt_task_t *task ;
    lua_callback_ctx_t *ctx ;
    odt_session_recv_interface_t *inte;
    odt_msg_parser *parser ;
    void *arg ;

    if (5 > lua_gettop (L)) {

        ODT_LOG_STDERR ("Need more arguments in 'reg_on_recv'") ;
        exit (1) ;
    }

    task = (odt_task_t *) lua_touserdata (L, 1) ;
    ack = lua_tostring (L, 2);
    ack_slen = lua_strlen (L, 2);
    announce = lua_tostring (L, 3) ;
    announce_slen = lua_strlen (L, 3);
    fail = lua_tostring (L, 4);
    fail_slen = lua_strlen (L, 4) ;
    arg = lua_touserdata (L, 5) ;

    parser = &task->parser ;
    inte = (odt_session_recv_interface_t *) parser->arg ;
    ctx = (lua_callback_ctx_t *) inte->arg ;

    strncpy (ctx->cbs[0], ack, ack_slen) ;
    ctx->cbslen[0] = ack_slen ;
    strncpy (ctx->cbs[1], announce, announce_slen);
    ctx->cbslen[1] = announce_slen ;
    strncpy (ctx->cbs[2], fail, fail_slen) ;
    ctx->cbslen[2] = fail_slen ;
    ctx->arg = arg ;

    lua_pushboolean (L, 1) ;
    return 1 ;
}




static inline int
odt_lua_is_req (lua_State *L)
{
    odt_rtsp_t *msg ;

    msg = (odt_rtsp_t *) lua_touserdata(L, 1) ;
    if (!msg) {

        lua_pushboolean(L, 0) ;
        return 1 ;
    }

    lua_pushboolean (L, 1) ;

    if (msg->req)
        lua_pushboolean(L, 1) ;
    else
        lua_pushboolean(L, 0) ;

    return 2 ;
}



static inline int
odt_lua_get_reqline (lua_State *L)
{
    odt_rtsp_t *msg ;
    odt_string_t *p ;
    int n ;

    msg = (odt_rtsp_t *) lua_touserdata(L, 1) ;

    if (!msg || !msg->req) {

        lua_pushboolean(L, 0) ;
        return 1 ;
    }

    lua_pushboolean(L, 1) ;

    p = odt_rtsp_get_verb(msg) ;
    lua_pushlstring (L, p->m_data, p->m_len) ;

    p = odt_rtsp_get_url_path(msg) ;
    lua_pushlstring (L, p->m_data, p->m_len) ;

    n = odt_rtsp_get_url_port(msg) ;
    lua_pushnumber (L, n) ;

    p = odt_rtsp_get_type(msg) ;
    lua_pushlstring (L, p->m_data, p->m_len) ;

    p = odt_rtsp_get_version(msg) ;
    lua_pushlstring (L, p->m_data, p->m_len) ;

    return 6 ;
}




static inline int
odt_lua_get_statusline (lua_State *L)
{
    odt_rtsp_t *msg ;
    odt_string_t *p ;
    int n ;

    msg = (odt_rtsp_t *) lua_touserdata (L, 1) ;

    if (!msg || msg->req) {

        lua_pushboolean (L, 0) ;
        return 1 ;
    }

    lua_pushboolean (L, 1) ;

    p = odt_rtsp_get_type(msg) ;
    lua_pushlstring (L, p->m_data, p->m_len) ;

    p = odt_rtsp_get_version(msg) ;
    lua_pushlstring (L, p->m_data, p->m_len) ;

    n = odt_rtsp_get_code(msg) ;
    lua_pushnumber (L, n) ;

    p = odt_rtsp_get_desc(msg) ;
    lua_pushlstring (L, p->m_data, p->m_len) ;

    return 5 ;
}




static inline int
odt_lua_get_hdr (lua_State *L)
{
    odt_rtsp_t *msg ;
    odt_string_t *p ;
    char *key ;
    size_t key_len ;
    odt_string_t skey , *svalue ;

    msg = (odt_rtsp_t *) lua_touserdata (L, 1) ;
    key = lua_tostring (L, 2) ;
    key_len = lua_strlen(L, 2) ;

    if (!msg) {

        lua_pushboolean(L, 0) ;
        return 1 ;
    }


    skey = odt_nstring (key, key_len) ;
    svalue = odt_rtsp_get_hdr (msg, &skey) ;

    if (!svalue) {

        lua_pushboolean (L, 0) ;
        return 1 ;
    }

    lua_pushboolean (L, 1) ;
    lua_pushlstring (L, svalue->m_data, svalue->m_len) ;

    return 2 ;
}



static inline int
odt_lua_get_body (lua_State *L)
{
    odt_rtsp_t *msg ;
    odt_string_t *string ;


    if (1 > lua_gettop (L)) {

        ODT_LOG_STDERR ("Need more arguments in 'get_body'") ;
        exit (1) ;
    }

    msg = (odt_rtsp_t *) lua_touserdata(L, 1) ;
    if (!msg) {

        lua_pushboolean(L, 0) ; 
        return 1 ;
    }

    lua_pushboolean(L, 1) ;

    string = odt_rtsp_get_body (msg) ;
    lua_pushlstring (L, string->m_data, string->m_len) ;

    return 2 ;
}


static inline int
odt_lua_get_msg (lua_State *L)
{
    odt_rtsp_t *msg ;
    odt_string_t *string ;


    if (1 > lua_gettop (L)) {

        ODT_LOG_STDERR ("Need more arguments in 'get_msg'") ;
        exit (1) ;
    }

    msg = (odt_rtsp_t *) lua_touserdata (L, 1) ;

    if (NULL == (string = odt_rtsp_get_msg (msg))) {

        lua_pushboolean (L, 0);
        return 1 ;
    }

    lua_pushboolean (L, 1) ;
    lua_pushlstring (L, string->m_data, string->m_len) ;
    return 2 ;
}




static inline int
odt_lua_import_csv_file (lua_State *L)
{
    odt_csv_t *csv ;
    const char *file_name ;


    if (1 > lua_gettop (L)) {

        ODT_LOG_STDERR ("Need more arguments in 'import_csv_file'") ;
        exit (1) ;
    }

    file_name = lua_tostring (L, 1) ;

    if (NULL == (csv = odt_csv_parse (file_name))) {

        lua_pushboolean (L, 0);
        return 1 ;
    }

    lua_pushboolean (L, 1) ;
    lua_pushlightuserdata (L, csv) ;

    return 2 ;
}



static inline int
odt_lua_get_media (lua_State *L)
{
    int algorithm , top, list_id ;
    odt_csv_t *csv ;
    odt_media_entry_t *media_entry ;
    odt_string_t *media_string ;


    if (2 > lua_gettop (L)) {

        ODT_LOG_STDERR ("Need more arguments in 'get_media'") ;
        exit (1) ;
    }

    top = lua_gettop (L) ;
    csv = (odt_csv_t *) lua_touserdata (L, 1) ;
    algorithm = (int) lua_tonumber (L, 2) ;

    if (top == 3) {

        list_id = (int) lua_tonumber (L, 3) ;
        if (NULL == (media_entry = odt_csv_fetch_playlist
                       (csv, list_id))) {

            lua_pushboolean (L, 0) ;
            return 1 ;
        }

        media_string = odt_playlist2string (media_entry) ;
    } else {

        if (NULL == (media_entry = odt_csv_fetch_media
                        (csv, algorithm))) {

            lua_pushboolean (L, 0) ;
            return 1 ;
        }

        media_string = odt_media2string (media_entry) ;
    }

    lua_pushboolean (L, 1) ;
    lua_pushlstring (L, media_string->m_data, media_string->m_len) ;

    odt_free (media_string->m_data) ;
    odt_free (media_string) ;

    return 2 ;
}




static inline int
odt_lua_destroy_csv (lua_State *L)
{
    odt_csv_t *csv ;


    if (1 > lua_gettop (L)) {

        ODT_LOG_STDERR ("Need more arguments in 'destroy_csv'") ;
        exit (1) ;
    }


    csv = (odt_csv_t *) lua_touserdata (L, 1) ;
    if (csv == odt_g_csv)
        odt_g_csv = NULL ;

    odt_csv_destroy (csv) ;

    lua_pushboolean (L, 1) ;
    return 1 ;
}




static inline void
odt_lua_connect_callback_func (int what, void *arg)
{
    odt_task_t *task ;
    odt_session_recv_interface_t *inte ;
    lua_callback_ctx_t *ctx ;
    lua_callback_ctx_t *recv_ctx ;

    if (!arg)
        return ;

    ctx = (lua_callback_ctx_t *) arg ;
    task = ctx->task ;


    recv_ctx = odt_malloc (sizeof(lua_callback_ctx_t)) ;
    *recv_ctx = (lua_callback_ctx_t) {

        .task = task,
        .L = ctx->L,
    } ;


    // Register recv callback functions
    inte = odt_malloc (sizeof(odt_session_recv_interface_t)) ;
    *inte = (odt_session_recv_interface_t) {

        .ack_cb = odt_lua_recv_ack_callback_func,
        .announce_cb = odt_lua_recv_announce_callback_func,
        .fail_cb = odt_lua_recv_fail_callback_func,
        .arg = recv_ctx,
        .destroy = odt_lua_recv_interface_destroy,
    } ;

    if (-1 == odt_task_reg_recv_cb (task, inte)) {

        odt_free (recv_ctx) ;
        odt_free (inte) ;
    }


    lua_getlglobal (ctx->L, ctx->cbs[0], ctx->cbslen[0]) ;
    lua_pushlightuserdata (ctx->L, task) ;
    lua_pushlightuserdata (ctx->L, ctx->arg) ;
    if (0 != lua_pcall (ctx->L, 2, 0, 0))
        lua_pcall_print_error (ctx->L, ctx->cbs[0]) ;

    odt_free (ctx) ;
}

static inline void
odt_lua_connect_callback_fail_func (int what, void *arg)
{
    lua_callback_ctx_t *ctx ;

    if (!arg)
        return ;

    ctx = (lua_callback_ctx_t *) arg ;


    lua_getlglobal (ctx->L, ctx->cbs[1], ctx->cbslen[1]) ;
    lua_pushlightuserdata (ctx->L, ctx->arg) ;
    if (0 != lua_pcall (ctx->L, 1, 0, 0))
        lua_pcall_print_error (ctx->L, ctx->cbs[1]) ;

    odt_free (ctx) ;
}


/*
 * In fact, the 'odt_task_t' is based on 'odt_connection_t'
 * but 'task' provided functions to send and recv messages,
 * so it's different to connection. In the user side, 'task'
 * is hard to get, but connection is simply. That is the
 * reason why I named it 'connect' but return a 'task'
 */
static inline int
odt_lua_connect (lua_State *L)
{
    lua_callback_ctx_t *conn_ctx ;
    odt_task_t *task ;
    char *ip, *cb, *cb_fail;
    int cb_slen, cb_fail_slen ;
    int port ;
    long tv ;
    void *arg ;



    if (6 > lua_gettop (L)) {

        ODT_LOG_STDERR ("Need more arguments in 'connect'") ;
        exit (1) ;
    }


    ip = lua_tostring (L, 1) ;
    port = (int) lua_tonumber (L, 2) ;
    cb = lua_tostring (L, 3) ;
    cb_slen = lua_strlen (L, 3);
    cb_fail = lua_tostring (L, 4) ;
    cb_fail_slen = lua_strlen (L, 4);
    tv = (long) lua_tonumber (L, 5) ;
    arg = lua_touserdata (L, 6);


    conn_ctx = odt_malloc (sizeof(lua_callback_ctx_t)) ;
    task = odt_task_connect (odt_g_evm, NULL, ip, port,
                     odt_lua_connect_callback_func,
                     odt_lua_connect_callback_fail_func,
                     odt_lua_connect_callback_fail_func,
                     conn_ctx, tv) ;
    if (NULL == task) {

        odt_free (conn_ctx) ;
        lua_pushboolean (L, 0);
        return 1 ;
    }

    conn_ctx->arg = arg ;
    conn_ctx->L = L ;
    conn_ctx->task = task ;

    // cbs : 0 -- connection success
    //       1 -- connection failed
    memcpy (conn_ctx->cbs[0], cb, cb_slen) ;
    conn_ctx->cbslen[0] = cb_slen ;
    memcpy (conn_ctx->cbs[1], cb_fail, cb_fail_slen) ;
    conn_ctx->cbslen[1] = cb_fail_slen ;


    odt_session_submit_task (task) ;

    lua_pushboolean (L, 1) ;
    lua_pushlightuserdata (L, task) ;
    return 2 ;
}


static inline void
odt_lua_handle_terminated (int what, void *arg)
{
    // Free arg -- task
    odt_free (arg) ;
}

static inline int
odt_lua_disconnect (lua_State *L)
{
    odt_task_t *task ;
    odt_session_recv_interface_t *inte ;

    if (1 > lua_gettop(L)) {

        ODT_LOG_STDERR ("Need more arguments in 'disconnect'");
        exit (1) ;
    }

    task = (odt_task_t *) lua_touserdata(L, 1) ;

    if (0 > odt_task_terminate (task, odt_lua_handle_terminated, task)) {

        lua_pushboolean (L, 0);
        return 1 ;
    }

    lua_pushboolean (L, 1);
    return 1 ;
}



static inline void
odt_lua_beatheart_callback_func (int what, void *arg)
{
    lua_callback_ctx_t *ctx ;

    ctx = (lua_callback_ctx_t *) arg ;

    lua_getlglobal (ctx->L, ctx->cbs[0], ctx->cbslen[0]) ;
    lua_pushlightuserdata (ctx->L, ctx->task);
    lua_pushlightuserdata (ctx->L, ctx->arg);

    if (0 != lua_pcall (ctx->L, 2, 0, 0))
        lua_pcall_print_error (ctx->L, ctx->cbs[0]) ;
}

static inline void
odt_lua_beatheart_collect_arg_func (int what, void *arg)
{
    if (arg)
        odt_free (arg) ;
}


static inline int
odt_lua_beatheart (lua_State *L)
{
    odt_task_t *task ;
    lua_callback_ctx_t *ctx ;
    const char *cb ;
    int cb_slen ;
    int tv ;
    void *arg ;

    if (4 > lua_gettop (L)) {

        ODT_LOG_STDERR ("Need more arguments in 'beatheart'");
        exit (1) ;
    }

    task = (odt_task_t *) lua_touserdata (L, 1) ;
    cb = lua_tostring (L, 2);
    cb_slen = lua_strlen (L, 2);
    tv = (int) lua_tonumber (L, 3);
    arg = lua_touserdata (L, 4);


    ctx = odt_malloc (sizeof(lua_callback_ctx_t)) ;
    *ctx = (lua_callback_ctx_t) {

        .task = task,
        .arg = arg,
        .L = L,
    } ;
    strncpy (ctx->cbs[0], cb, cb_slen);
    ctx->cbslen[0] = cb_slen ;

    if ( 0 > odt_task_beatheart (task, odt_lua_beatheart_callback_func,
               odt_lua_beatheart_collect_arg_func, 4000, ctx)) {

        lua_pushboolean (L, 0);
        return 1 ;
    }

    lua_pushboolean (L, 1);
    return 1 ;
}



static inline int
odt_lua_session_init (lua_State *L)
{
    odt_session_t *session ;
    int gid ;


    if (1 > lua_gettop (L)) {

        ODT_LOG_STDERR ("Need more arguments in 'session_init'") ;
        exit (1) ;
    }

    gid = (int) lua_tonumber (L, 1) ;

    session = odt_session_init (NULL, gid);

    lua_pushboolean (L, 1) ;
    lua_pushlightuserdata (L, session) ;

    return 2 ;
}




static inline int
odt_lua_session_destroy (lua_State *L)
{
    odt_session_t *session ;


    if (1 > lua_gettop (L)) {

        ODT_LOG_STDERR ("Need more arguments in 'session_destroy'") ;
        exit (1) ;
    }

    session = (odt_session_t *) lua_touserdata (L, 1) ;

    odt_session_destroy (session) ;
    odt_free (session) ;
    lua_pushboolean (L, 1) ;
    return 1 ;
}


static inline int
odt_lua_session_getsid (lua_State *L)
{
    odt_session_t *session ;

    if (1 > lua_gettop (L)) {

        ODT_LOG_STDERR ("Need more arguments in 'session_getsid'");
        exit (1) ;
    }

    session = (odt_session_t *) lua_touserdata (L, 1);
    lua_pushlstring (L, session->m_sid.m_data, session->m_sid.m_len) ;
    return 1 ;
}

static inline int
odt_lua_session_setsid (lua_State *L)
{
    odt_session_t *session ;
    const char *sid ;
    int len ;
    if (2 > lua_gettop(L)) {

        ODT_LOG_STDERR ("Need more arguments in 'setsid'");
        exit (1) ;
    }

    session = (odt_session_t *) lua_touserdata (L, 1) ;
    sid = lua_tostring (L, 2);
    len = lua_strlen (L, 2) ;

    if (-1 == odt_string_update (&session->m_sid, sid, len)) {

        lua_pushboolean (L, 0);
        return 1 ;
    }

    lua_pushboolean (L, 1) ;
    return 1 ;
}


static inline int
odt_lua_session_getodsid (lua_State *L)
{
    odt_session_t *session ;

    if (1 > lua_gettop (L)) {

        ODT_LOG_STDERR ("Need more arguments in 'session_getodsid'");
        exit (1) ;
    }

    session = (odt_session_t *) lua_touserdata (L, 1) ;
    lua_pushlstring (L, session->m_odsid.m_data, session->m_odsid.m_len);
    return 1; 
}


static inline int
odt_lua_session_randodsid (lua_State *L)
{
    odt_session_t *session ;
    if (1 > lua_gettop(L)) {

        ODT_LOG_STDERR ("Need more arguments in 'randodsid'") ;
        exit (1) ;
    }

    session = (odt_session_t *) lua_touserdata (L, 1);
    odt_session_rand_odsid (session) ;
    lua_pushlstring (L, session->m_odsid.m_data, session->m_odsid.m_len) ;
    return 1 ;
}

static inline int
odt_lua_session_getid (lua_State *L)
{
    odt_session_t *session ;
    if (1 > lua_gettop(L)) {

        ODT_LOG_STDERR ("Need more arguments in 'getid'");
        exit (1);
    }

    session = (odt_session_t *) lua_touserdata (L, 1) ;
    lua_pushnumber (L, session->m_id);
    return 1 ;
}


static inline int
odt_lua_set_syslog_limit (lua_State *L)
{
    size_t limit ;

    if (1 > lua_gettop (L)) {

        ODT_LOG_STDERR ("Need more argments in 'set_syslog_limit'");
        exit (1);
    }

    limit = (size_t) lua_tonumber (L, 1) ;
    if (-1 == odt_syslog_set_limit (limit)) {

        lua_pushboolean (L, 0);
        return 1 ;
    }

    lua_pushboolean (L, 1) ;
    return 1 ;
}


static inline int
odt_lua_set_syslog_level (lua_State *L)
{
    int level ;

    if (1 > lua_gettop (L)) {

        ODT_LOG_STDERR ("Need more arguments in 'set_syslog_level'");
        exit (1) ;
    }

    level = (int) lua_tonumber (L, 1) ;
    if (-1 == odt_syslog_set_level (level)) {

        lua_pushboolean (L, 0) ;
        return 1 ;
    }

    lua_pushboolean (L, 1) ;
    return 1 ;
}




static inline void
odt_lua_timer_callback_func (int what, void *arg)
{
    lua_callback_ctx_t *ctx ;

    if (!arg)
        return ;

    ctx = (lua_callback_ctx_t *) arg ;

    lua_getlglobal (ctx->L, ctx->cbs[0], ctx->cbslen[0]) ;
    lua_pushlightuserdata (ctx->L, ctx->arg) ;

    if (0 != lua_pcall (ctx->L, 1, 0, 0))
        lua_pcall_print_error (ctx->L, ctx->cbs[0]) ;
}


static inline void
odt_lua_timer_collect_arg_func (int what, void *arg)
{
    if (arg)
        odt_free (arg) ;
}
 


static inline int
odt_lua_set_timer (lua_State *L)
{
    lua_callback_ctx_t *ctx ;
    const char *cb ;
    long int tv, interval;
    int cb_slen;
    unsigned long int timerd ;
    void *arg ;


    if (4 > lua_gettop (L)) {

        ODT_LOG_STDERR ("Need more arguments in 'set_timer'") ;
        lua_pushboolean (L, 0);
        return 1 ;
    }

    cb = lua_tostring (L, 1) ;
    cb_slen = lua_strlen (L, 1) ;
    tv = (long int) lua_tonumber (L, 2) ;
    interval = (long int) lua_tonumber (L, 3) ;
    arg = lua_touserdata (L, 4) ;

    ctx = odt_malloc (sizeof(lua_callback_ctx_t)) ;
    *ctx = (lua_callback_ctx_t) {

        .arg = arg,
        .L = L,
    } ;

    memcpy (ctx->cbs[0], cb, cb_slen) ;
    ctx->cbslen[0] = cb_slen ;

    if (0 > (timerd = odt_settimer (odt_g_evm, tv, interval,
                         odt_lua_timer_callback_func,
                         odt_lua_timer_collect_arg_func,
                         ctx))) {

        ODT_LOG_ERR ("Set timer failed") ;
        odt_free (ctx) ;
        lua_pushboolean (L, 0) ;
        return 1 ;
    }

    lua_pushboolean (L, 1) ;
    lua_pushnumber (L, timerd) ;
    return 2 ;
}


static inline int
odt_lua_del_timer (lua_State *L)
{
    unsigned long int timerd ;

    if (1 > lua_gettop (L)) {

        ODT_LOG_STDERR ("Need more arguments in 'del_timer'") ;
        lua_pushboolean (L, 0) ;
        return 1;
    }

    timerd = lua_tonumber (L, 1) ;
    if (!timerd) {

        ODT_LOG_ERR ("Invalid timerd from user") ;
        lua_pushboolean (L, 0);
        return 1;
    }

    if (0 > odt_deltimer (timerd)) {

        ODT_LOG_ERR ("Delete timer failed") ;
        lua_pushboolean (L, 0) ;
        return 1 ;
    }

    lua_pushboolean (L, 1) ;
    return 1;
}




lua_State *odt_g_lua_state ;



static const struct luaL_reg
lua_global_function [] = {

    {"new_msg",            odt_lua_new_msg},
    {"fill_reqline",       odt_lua_fill_reqline},
    {"fill_statusline",    odt_lua_fill_statusline},
    {"fill_hdr",           odt_lua_fill_hdr},
    {"fill_body",          odt_lua_fill_body},
    {"send_msg",           odt_lua_send_msg},
    {"reg_on_recv",        odt_lua_reg_on_recv},
    {"is_req",             odt_lua_is_req},
    {"get_reqline",        odt_lua_get_reqline},
    {"get_statusline",     odt_lua_get_statusline},
    {"get_hdr",            odt_lua_get_hdr},
    {"get_body",           odt_lua_get_body},
    {"import_csv_file",    odt_lua_import_csv_file},
    {"get_media",          odt_lua_get_media},
    {"destroy_csv",        odt_lua_destroy_csv},
    {"get_msg",            odt_lua_get_msg},
    {"connect",            odt_lua_connect},
    {"disconnect",         odt_lua_disconnect},
    {"set_syslog_level",   odt_lua_set_syslog_level},
    {"set_timer",          odt_lua_set_timer},
    {"del_timer",          odt_lua_del_timer},
    {"set_syslog_limit",   odt_lua_set_syslog_limit},
    {"get_addr",           odt_lua_get_addr},
    {"beatheart",          odt_lua_beatheart},
    {"session_new",        odt_lua_session_init},
    {"session_setsid",     odt_lua_session_setsid},
    {"session_getsid",     odt_lua_session_getsid},
    {"session_getid",      odt_lua_session_getid},
    {"session_getodsid",   odt_lua_session_getodsid},
    {"session_randodsid",  odt_lua_session_randodsid},
    {"session_delete",     odt_lua_session_destroy},
    {NULL, NULL}
} ;


static int
odt_lua_init ()
{
    lua_State *L ;
    int n ;

    L = lua_open () ;
    luaopen_base (L) ;
    luaopen_string (L) ;

    for (n=0; lua_global_function[n].name; ++n) {

        lua_pushcfunction (L, lua_global_function[n].func);
        lua_setglobal (L, lua_global_function[n].name);
    }

    odt_g_lua_state = L ;
}



void
odt_lua_exit ()
{
    lua_close (odt_g_lua_state) ;
}




#endif
