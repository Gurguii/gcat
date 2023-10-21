#include "../progressbar.cpp"
#include <unistd.h>
int main()
{
  ProgressBar pbar(1000);
  pbar.initialize();
  for(int i = 0; i < 9; ++i){
    printf("Wiski: %s\r", pbar.update(100));
    fflush(stdout);
    sleep(1);
  }
}
