#include "stdafx.h"
typedef int (*type_printf)(const char *, ...);
int _tmain(int argc, _TCHAR *argv[]){
    /* compute delta 
    int delta = ((int) printf) - ((int) scanf)
    */
    int delta = -1200;
    unsigned int address = (unsigned int) scanf;
    address += delta;
    type_printf f = (type_printf) address;
    f("Hello world");
    while(1);
    return 0;
}
