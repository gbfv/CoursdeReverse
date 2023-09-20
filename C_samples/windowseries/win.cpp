#include "win.h"

void *get_fun_address_by_name(pefile32_t &pe, const char *name){
    BYTE *ImageBase = (BYTE *)pe.dos_header;
    PIMAGE_DATA_DIRECTORY entries = pe.nt_header->OptionalHeader.DataDirectory;
    uint32_t eat_rva = (uint32_t)entries[0].VirtualAddress;
    PED eat = (PED)((uint32_t)pe.dos_header + eat_rva);
    DWORD *pNameAddr = (DWORD *)(ImageBase + eat->AddressOfNames);
    DWORD *pFuncAddr = (DWORD *)(ImageBase + eat->AddressOfFunctions);
    for (uint32_t i = 0; i < eat->NumberOfFunctions; i++)
    {
        const char *pFunc = (const char *)(ImageBase + pNameAddr[i]);
        PVOID pAddr = (PVOID)(ImageBase + (uint32_t)pFuncAddr[i]);
        if (strcmp(name, pFunc) == 0)
            return pAddr;
    }
    return NULL;
}

int parse_pe_in_mem(void *p, pefile32_t &pe){
    pe.dos_header = (PIMAGE_DOS_HEADER)p;
    pe.nt_header = (PIMAGE_NT_HEADERS)(((char *)p) + pe.dos_header->e_lfanew);
    pe.sections = (PIMAGE_SECTION_HEADER)((char *)pe.nt_header + sizeof(IMAGE_NT_HEADERS));
    return 0;
}