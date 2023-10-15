#include <stdio.h>
#include <sys/ioctl.h>
#include <stdint.h>

uint64_t is_stdin_empty() {
  uint64_t bytes;
  ioctl(0,FIONREAD, &bytes);
  return bytes;
}

int main(int argc, const char **args)
{
  uint64_t available_bytes = is_stdin_empty();
  if(available_bytes == 0){
    printf("empty: %lu\n",available_bytes);
  }else{
    printf("%lu available bytes\n",available_bytes);
  }
  return 0;
}
