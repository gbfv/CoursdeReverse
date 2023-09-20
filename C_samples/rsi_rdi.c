#include <stdio.h>
#include <string.h>
int main(int argc, char *argv[]){
  char *c = "abcdefg";
  if (memcmp(argv[1],c,5) == 0)
    printf("found");
  return 0;
}
