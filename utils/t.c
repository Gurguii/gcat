#include <stdio.h>
#include <math.h>
#include "progressbar.c"
#include <unistd.h>

int main()
{
  char pbar[100];
  int total = 1000;
  int total_copy = total;

  int read = 0;
  do{
    get_progress_bar('#',total_copy,read,pbar,sizeof(pbar));
    printf("%s\r",pbar);
    fflush(stdout);
    total -= 100;
    read += 100;
    sleep(1);
  }while(total > 0);
}
