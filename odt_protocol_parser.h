#ifndef __ODT_PROTOCOL_PARSER_HEAD__
#define __ODT_PROTOCOL_PARSER_HEAD__



#include "odt.h"
#include "list.h"


static inline int odt_strcmp_callback (void* s1, void* s2)
{
    odt_string_t *_1, *_2 ;

    _1 = (odt_string_t *) s1 ;
    _2 = (odt_string_t *) s2 ;

    return odt_strcmp (_1, _2) ;
}




/* The fixed format of the RTSP message :
 *     VERB [space] URL [space] TYPE [/] VERSION\r\n
 *     HDR_FIELD_1 [:] [space] HDR_VALUE_1\r\n
 *     \r\n
 *     BODY\r\n
 */

#define S_REQ_INIT    S_VERB_B
#define S_RES_INIT    S_TYPE_B



#define RTSP_CONTENT_LENGTH    "Content-Length"
#define RTSP_CONTENT_TYPE      "Content-Type"



typedef enum {

    S_VERB_B, S_VERB,
    S_URL_B, S_URL,

    S_TYPE_B, S_TYPE,        
    S_VERSION_B, S_VERSION, 

    S_CODE_B, S_CODE,
    S_DESC_B, S_DESC,

    S_FIRST_RN,
    S_HDR_B, S_HDR,
    S_HDR_RN,
    S_DOUBLE_RN, S_DOUBLE_RN_R,
    S_BODY_B, S_BODY_B_, S_BODY,
    S_COMPLETE,
    S_ERROR
} odt_rtsp_parse_state ;



typedef enum {

    RTSP_SETUP,
    RTSP_TEARDOWN,
    RTSP_PLAY,
    RTSP_PAUSE,
    RTSP_GET_PARAMETER,
    RTSP_SET_PARAMETER,
    RTSP_ANNOUNCE,
    RTSP_OTHER
} odt_rtsp_verb_t ;


typedef enum {

    RTSP_TYPE ,
    HTTP_TYPE ,
    OTHER_TYPE
} odt_rtsp_type_t ;



typedef struct {

    odt_string_t      key;
    odt_string_t      value;
    struct list_head  nob;
} rtsp_fields_t ;


typedef struct {

    odt_string_t              verb,
                              url_port,
                              url_path,
                              type,
                              version,
                              body,
                              code,
                              desc;


    int                       req ;
    

    odt_rtsp_verb_t           verbd ;
    odt_rtsp_type_t           typed ;

    odt_rbtree_t             *fields ;
    struct list_head          fields_list ;

    odt_string_t              msg ;
    odt_string_t              parsed_msg ;
    int                       last_head ;
    int                       last_point ;

    odt_rtsp_parse_state      state ;
    odt_rtsp_parse_state      last_state ;
    size_t                    content_length ;


    int                       parse_done ;
} odt_rtsp_t ;



typedef struct odt_msg_parser_s odt_msg_parser ;
struct odt_msg_parser_s {

    int       (*append)            (odt_msg_parser *,
                                    const char     *,
                                    int             ) ;
    int       (*parse)             (odt_msg_parser *) ;
    int       (*init)              (odt_msg_parser *,
                                    void           *) ;
    void      (*destroy)           (odt_msg_parser *) ;
    void      (*handle_parse_fail) (odt_msg_parser *) ;
    void       *arg ;         // Maybe 'odt_session_recv_interface'
    void       *msg ;         // Maybe RTSP message type
} ;




int    odt_rtsp_parse  (odt_rtsp_t *);
int    odt_rtsp_append (odt_rtsp_t *, const char *, int) ;

int    odt_rtsp_init   (odt_rtsp_t *msg) ;
void   odt_rtsp_destroy (odt_rtsp_t *msg) ;
void   odt_rtsp_cleanup (odt_rtsp_t *msg) ;


odt_string_t *odt_rtsp_get_msg (odt_rtsp_t *msg) ;
int           odt_rtsp_set_msg (odt_rtsp_t *msg, odt_string_t *str) ;


int    odt_rtsp_set_hdr(odt_rtsp_t     *msg,
                        odt_string_t   *key,
                        odt_string_t   *value) ;

odt_string_t *odt_rtsp_get_hdr(odt_rtsp_t *msg, odt_string_t *key) ;

inline int    odt_rtsp_set_body (odt_rtsp_t *msg, odt_string_t *body) ;
inline odt_string_t *odt_rtsp_get_body (odt_rtsp_t *msg) ;

inline int odt_rtsp_set_verb (odt_rtsp_t *msg, odt_string_t *verb) ;
inline odt_string_t *odt_rtsp_get_verb (odt_rtsp_t *msg) ;

inline int odt_rtsp_set_url_path (odt_rtsp_t *msg, odt_string_t *path) ;
inline odt_string_t *odt_rtsp_get_url_path (odt_rtsp_t *msg) ;

inline int odt_rtsp_set_url_port (odt_rtsp_t *msg, size_t port) ;
inline int odt_rtsp_get_url_port (odt_rtsp_t *msg) ;

inline int odt_rtsp_set_type (odt_rtsp_t *msg, odt_string_t *type) ;
inline odt_string_t *odt_rtsp_get_type (odt_rtsp_t *msg) ;

inline int odt_rtsp_set_version(odt_rtsp_t *msg, odt_string_t *version) ;
inline odt_string_t *odt_rtsp_get_version(odt_rtsp_t *msg) ;

inline int odt_rtsp_set_code(odt_rtsp_t *msg, size_t code) ;
inline int odt_rtsp_get_code(odt_rtsp_t *msg) ;

inline int odt_rtsp_set_desc(odt_rtsp_t *msg, odt_string_t *desc) ;
inline odt_string_t *odt_rtsp_get_desc(odt_rtsp_t *msg) ;
#endif
