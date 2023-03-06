#include "stdafx.h"
#include "string.h"
#include <Windows.h>
typedef int (*type_printf)(const char *, ...);
int _tmain(int argc, _TCHAR *argv[]){
    HMODULE msvcrt = LoadLibraryA("msvcrt");
    type_printf f = (type_printf)GetProcAddress(msvcrt, "printf");
    f("Hello world");
    while (1);
    return 0;
}
