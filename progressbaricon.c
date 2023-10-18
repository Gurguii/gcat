#include <stdio.h>
#include <unistd.h>

int main()
{
  char sequence[] = "\\|/-\\|-";
  for(;;)
  {
    for(int i = 0; i < sizeof(sequence); ++i){
      write(0,sequence+i,1);
      write(0,"\r",1);
      usleep(1000 * 100);
    }
  }
}
