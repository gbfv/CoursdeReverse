#include "stdafx.h"
#include "string.h"
char *signature = "\x6a\x0c\x68\x60\x57\xb0\x78\xe8\xc0\xb5\xfa\xff";
typedef int (*type_printf)(const char *, ...);
int _tmain(int argc, _TCHAR *argv[]){
    /* compute signature
    char *p = (char *) printf;
    for(int i = 0 ; i < 12 ; ++i)
       printf("\\x%02x", p[i]);
    */
    char *p = (char *)scanf;
    int found = 0;
    while (!found){
        if (memcmp(p, signature, 12) == 0){found = 1; }
        else{ p--; }
   }
    type_printf f = (type_printf) p;
    f("Hello world");
    while(1);
    return 0;
}
