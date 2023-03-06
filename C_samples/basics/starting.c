#include "stdafx.h"
#include <stdlib.h>
char a ='A',b='B';
int _tmain(int argc, char *argv[]){
	int i = 42, j;
	int *p =(int *) malloc(sizeof(int));
	printf("[%p] = %d\n", &i, i);
	printf("[%p] = %c\n", &a, a);
	printf("[%p] = %x\n", p, *p);
	printf("_tmain = %p\n", _tmain);
	printf("printf = %p\n", printf);
	while(1);
	return 0;
}