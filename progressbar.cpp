#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
void progress_bar(uint64_t total, uint64_t read, char *progress_bar_buff, uint64_t size, int fd = 0) // 0 -> stdout 1 -> stderr
{
  write(fd,progress_bar_buff,floor(read/(total/size)));
  write(fd,"\r",1);
}

int main()
{
  uint64_t data = 1000;
  uint64_t datacpy = data;
  uint64_t read = 0;
  char pbar[100];
  memset(pbar,'#',sizeof(pbar));
  for(uint64_t read = 0; data; data-=100,read+=100){
    progress_bar(datacpy,read,pbar,sizeof(pbar),0);
    sleep(1);
  }
}
