
/*
 *	Base type
 */

#ifndef __ODT_STRING_H__
#define __ODT_STRING_H__

typedef struct {

	int m_len ;       // Length of the payload
	int m_free ;      // Length of the free space
	char *m_data ;    // Main datas
} odt_string_t ;



/* Create a odt string with a const string */
#define odt_string(str)                                                        \
    (odt_string_t) { sizeof(str)-1, 0, str }

/* Create a odt string with a given string */
#define odt_nstring(str, n)                                                    \
    (odt_string_t) { n, 0, str }

#define odt_null_string    (odt_string_t) { 0,0,NULL }


/* Calc the length of the string */
int odt_string_len (odt_string_t *str) ;

/* Calc the free length of the string */
int odt_string_free_len (odt_string_t *str) ;

/* Create a blank string with n free spaces */
odt_string_t *odt_string_create_free (int n) ;

/* Create a string with a charactor array */
odt_string_t *odt_string_create_char (odt_string_t *str,
                                      const char *data,
                                      unsigned int data_len) ;


int odt_string_append_len (odt_string_t *str,
                           const char *data,
                           unsigned int data_len) ;
inline int odt_string_appendv (odt_string_t *str, size_t argc, ...) ;
inline int odt_string_append (odt_string_t *str, const char *data) ;

int odt_string_update (odt_string_t *str,
                       const char *data,
                       unsigned int data_len) ;

void odt_string_clean (odt_string_t *str) ;
void odt_string_clean_without_buffer (odt_string_t *str) ;


static inline int odt_strcmp (odt_string_t *s1, odt_string_t *s2)
{
    char *ss1, *ss2 ;
    int n , l1, l2;
    int ret ;

    ss1 = s1->m_data ;
    ss2 = s2->m_data ;
    l1 = s1->m_len ; 
    l2 = s2->m_len ;

    n = ret = 0 ;

    for (n=ret=0; n<l1 && n<l2 && ret==0; ++n)
        ret = ss1[n] - ss2[n] ;

    if (n<l1 && n>=l2)
        ret = 1 ;
    else if (n<l2 && n>=l1)
        ret = -1 ;

    return ret ;
}


static inline int odt_natoi (const char *atr, size_t len)
{
    char *buf = odt_malloc (len+1) ;
    int ret ;

    memcpy (buf, atr, len) ;
    buf [len] = 0 ;

    ret = atoi (buf) ;
    odt_free (buf) ;

    return ret ;
}

#define odt_str3_cmp(m, c0, c1, c2, c3)                                       \
    (*(uint32_t *) (m) == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0))

#define odt_str4cmp(m, c0, c1, c2, c3)                                        \
    (*(uint32_t *) (m) == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0))

#define odt_str5cmp(m, c0, c1, c2, c3, c4)                                    \
    (*(uint32_t *) (m) == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0)             \
        && m[4] == c4)

#define odt_str6cmp(m, c0, c1, c2, c3, c4, c5)                                \
    (*(uint32_t *) (m) == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0)             \
        && (((uint32_t *) m)[1] & 0xffff) == ((c5 << 8) | c4))

#define odt_str7cmp(m, c0, c1, c2, c3, c4, c5, c6)                             \
    (*(uint32_t *) (m) == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0)             \
        && (((uint32_t *) m)[1] & 0xffffff) == ((c6 << 16) | (c5 << 8) | c4))

#define odt_str8cmp(m, c0, c1, c2, c3, c4, c5, c6, c7)                        \
    (*(uint32_t *) (m) == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0)             \
        && ((uint32_t *) m)[1] == ((c7 << 24) | (c6 << 16) | (c5 << 8) | c4))

#define odt_str9cmp(m, c0, c1, c2, c3, c4, c5, c6, c7, c8)                    \
    (*(uint32_t *) (m) == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0)             \
        && ((uint32_t *) m)[1] == ((c7 << 24) | (c6 << 16) | (c5 << 8) | c4)  \
        && m[8] == c8)

#define odt_str10cmp(m, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9)                    \
    (*(uint32_t *) (m) == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0)             \
        && ((uint32_t *) m)[1] == ((c7 << 24) | (c6 << 16) | (c5 << 8) | c4)  \
        && (((uint32_t *) m)[2] & 0xffff) == ((c9 << 8) | c8))

#define odt_str13cmp(m, c0, c1, c2, c3, c4, c5, c6, c7 ,c8, c9, c10,           \
                     c11, c12)                                                 \
    (*(uint32_t *) (m) == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0)             \
        && ((uint32_t *) m)[1] == ((c7 << 24) | (c6 << 16) | (c5 << 8) | c4)   \
        && ((uint32_t *) m)[2] == ((c11 << 24) | (c10 << 16) | (c9 << 8) | c8) \
        && m[12] == c12)


#define odt_strstr4cmp(m, s)                                                   \
    odt_str4cmp (m, s[0], s[1], s[2], s[3])

#define odt_strstr5cmp(m, s)                                                   \
    odt_str5cmp (m, s[0], s[1], s[2], s[3], s[4])

#define odt_strstr6cmp(m, s)                                                   \
    odt_str6cmp (m, s[0], s[1], s[2], s[3], s[4], s[5])

#define odt_strstr7cmp(m, s)                                                   \
    odt_str7cmp (m, s[0], s[1], s[2], s[3], s[4], s[5], s[6])

#define odt_strstr8cmp(m, s)                                                   \
    odt_str8cmp (m, s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7])

#define odt_strstr9cmp(m, s)                                                   \
    odt_str9cmp (m, s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[8])

#define odt_strstr10cmp(m, s)                                                  \
    odt_str10cmp (m, s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[8], s[9])

#define odt_strstr13cmp(m, s)                                                  \
    odt_str13cmp (m, s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[8],     \
                  s[9], s[10], s[11], s[12])



#endif
