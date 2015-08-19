
#include "odt.h"



int main ()
{
    int n ;

    char test[1024] ;

    for (n=0; n<1024; n++)
        test[n] = rand()%26+'a' ;

    odt_log_init () ;
    for (n=0; n<100*1024; n++) {

        ODT_LOG_INFO (test) ;
    }
    return 0 ;
}
