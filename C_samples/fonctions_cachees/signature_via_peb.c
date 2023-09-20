#include "stdafx.h"
#include "string.h"
#include "Windows.h"
#include "winternl.h"
char *signature = "\x6a\x0c\x68\x60\x57\xb0\x78\xe8\xc0\xb5\xfa\xff";
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

int _tmain(int argc, _TCHAR *argv[]){
	PROCESS_BASIC_INFORMATION basicinfo;
	HMODULE ntdll = LoadLibraryW(L"ntdll.dll");
	ptNtQueryInformationProcess query = NULL;
    query = (ptNtQueryInformationProcess)GetProcAddress(ntdll, "NtQueryInformationProcess");
	query(GetCurrentProcess(), ProcessBasicInformation, 
		  & basicinfo, sizeof(PROCESS_BASIC_INFORMATION), NULL);
	PPEB ppeb  = basicinfo.PebBaseAddress;
	PLDR_DATA_TABLE_ENTRY ple  = get_dll_handle(ppeb, L"MSVCR");
    char *p = (char *) ple->DllBase;
    int found = 0;
    while (!found){
        if (memcmp(p, signature, 12) == 0){ found = 1; }
		else{ p++; }
   }
    type_printf f = (type_printf) p;
    f("Hello world");
    while(1);
    return 0;
}