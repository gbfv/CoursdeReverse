#include "stdafx.h"
#include "string.h"
#include "Windows.h"
#include "winternl.h"
#include "stdint.h"

typedef struct{
  PIMAGE_DOS_HEADER     dos_header;
  PIMAGE_NT_HEADERS   nt_header;
  PIMAGE_SECTION_HEADER sections;
} pefile32_t;

typedef int (*type_printf)(const char *, ...);

typedef NTSTATUS(NTAPI* ptNtQueryInformationProcess)(HANDLE hProcess, UINT ProcInfoClass, PVOID ProcInfo, ULONG ProcInfoLenght, PULONG returnLenght);

PLDR_DATA_TABLE_ENTRY get_dll_handle(PPEB ppeb, const wchar_t *dll_name){
	PPEB_LDR_DATA ldr = ppeb->Ldr;
	PLIST_ENTRY le = & ldr->InMemoryOrderModuleList;
	le = le->Flink; //le points to exe
	PLIST_ENTRY exe_le = le;
	le = le->Flink; //le points to the first DLL
	while (le != exe_le){ //rotate within PE
		PLDR_DATA_TABLE_ENTRY entry= (PLDR_DATA_TABLE_ENTRY) 
        CONTAINING_RECORD(le, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
		le = le->Flink;	
		if (wcsstr(entry->FullDllName.Buffer, dll_name)){
			return entry;
		}
	}
	return NULL;
}

void * get_fun_address_by_name(pefile32_t &pe,const char *name){
	uint32_t eat_rva = (uint32_t) pe.nt_header->OptionalHeader.DataDirectory[0].VirtualAddress;
	PIMAGE_EXPORT_DIRECTORY eat = (PIMAGE_EXPORT_DIRECTORY) ((uint32_t) pe.dos_header + eat_rva);
	DWORD *pNameAddr = (DWORD *)((BYTE *)pe.dos_header + eat->AddressOfNames);
	DWORD *pFuncAddr = (DWORD *)((BYTE *)pe.dos_header + eat->AddressOfFunctions);
	for(uint32_t i = 0; i < eat->NumberOfFunctions; i++){
        const char *pFunc = (const char *)((BYTE *)pe.dos_header + pNameAddr[i]);
        PVOID pAddr = (PVOID)((PCHAR)pFuncAddr[i] + pe.nt_header->OptionalHeader.ImageBase);
        if(strcmp(name,pFunc) == 0)
			return pAddr;
    }
	return NULL;
}

int parse_pe_in_mem(void *p, pefile32_t & pe){
	pe.dos_header = (PIMAGE_DOS_HEADER) p;
	pe.nt_header = (PIMAGE_NT_HEADERS) (((char *) p) + pe.dos_header->e_lfanew);
	pe.sections = (PIMAGE_SECTION_HEADER) ((char *)pe.nt_header + sizeof(IMAGE_NT_HEADERS));
	return 0;
}


int _tmain(int argc, _TCHAR *argv[]){
	PROCESS_BASIC_INFORMATION basicinfo;
	HMODULE ntdll = LoadLibraryW(L"ntdll.dll");
	ptNtQueryInformationProcess query = NULL;
    query = (ptNtQueryInformationProcess)GetProcAddress(ntdll, "NtQueryInformationProcess");
	query(GetCurrentProcess(), ProcessBasicInformation, 
		  & basicinfo, sizeof(PROCESS_BASIC_INFORMATION), NULL);
	PPEB ppeb  = basicinfo.PebBaseAddress;
	PLDR_DATA_TABLE_ENTRY ple  = get_dll_handle(ppeb, L"MSVCR");
    pefile32_t  msvcr;
	parse_pe_in_mem(ple->DllBase, msvcr);
	printf("%p", get_fun_address_by_name(msvcr, "printf"));
    type_printf f = (type_printf) get_fun_address_by_name(msvcr, "printf");
    f("Hello world");
    while(1);
    return 0;
}