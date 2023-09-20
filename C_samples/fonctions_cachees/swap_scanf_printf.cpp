#include "stdafx.h"
#include "string.h"
#include "Windows.h"
#include "winternl.h"
#include <stdint.h>

uint32_t * get_fun(uint32_t fun, void * base_pointer){
	uint32_t *p = (uint32_t *) base_pointer;
	while (1) {
	  if (*p == fun) return p;
	  p++;
	}
	return NULL;
}

int _tmain(int argc, _TCHAR* argv[]){
	uint32_t atscan = (uint32_t) scanf, atprint = (uint32_t) printf;
	uint32_t * pos_printf = get_fun(atprint, _tmain );
	uint32_t * pos_scanf = get_fun(atscan, _tmain);
	DWORD old;
	VirtualProtect(pos_printf, 0x10, PAGE_EXECUTE_READWRITE, &old);
	*pos_printf = atscan;
	*pos_scanf = atprint;
	int sum = argc; //pour éviter une optim
	for(int i = argc ; i < 10*argc ; ++i) { sum += i;}
	scanf("Hello world %d", sum);
	while(1);
	return 0;
}
