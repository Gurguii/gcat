#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <sys/unistd.h>

#define BUFF_SIZE 4194304 // 4MB

int main(int argc, const char **args)
{
  char buff[4096];
  // short = uint8_t | double = uint16_t | unsigned long = uint32_t  | unsigned long long = uint64_t

  // check stdin
  uint64_t bytes = 0;
  ioctl(0,FIONREAD,&bytes);
  if(bytes > sizeof(buff)){
    printf("default buffer not big enough, allocating %lu bytes...\n",bytes);
    char *nbuff = (char*)malloc(bytes);
    if(nbuff == NULL){
      fprintf(stderr, "malloc()\n"); return -1;
    }
    printf("loading data ...\n");
    if(read(0,nbuff,bytes) > 0){
      printf("== data read ==\n%s\n",nbuff);
    }; 
    free(nbuff);
  }
  return 0;
}
