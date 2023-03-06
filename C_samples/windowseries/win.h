#include "stdafx.h"
#include "Windows.h"
#include "winternl.h"

typedef struct{
    PIMAGE_DOS_HEADER dos_header;
    PIMAGE_NT_HEADERS nt_header;
    PIMAGE_SECTION_HEADER sections;
} pefile32_t;

typedef PIMAGE_EXPORT_DIRECTORY PED;

void *get_fun_address_by_name(pefile32_t &pe, const char *name);
int parse_pe_in_mem(void *p, pefile32_t &pe);
