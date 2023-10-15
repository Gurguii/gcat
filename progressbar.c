#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <memory.h>
#include <math.h>

#define PROGRESSION_BAR_LENGTH 200

int main() {
  char pbar[PROGRESSION_BAR_LENGTH];
  memset(pbar,0,sizeof(pbar));
  int total = 2000;
  int _total = total;
  int read = 0;
  int chunk_val = total / PROGRESSION_BAR_LENGTH;
  int chunks = 0;
  char bar[PROGRESSION_BAR_LENGTH * 2];
  memset(bar,0,sizeof(bar));
  while (total) {
    sleep(1);
    read += 100;
    total-=100;
    // Calculate the chars of data that has been processed
    chunks = read/chunk_val;
    memset(pbar,'#',chunks);
    snprintf(bar,sizeof(bar),"[%i:%i] %s\r",read,_total,pbar);
    write(0, bar, sizeof(bar));
  }
  // Write the completion message to the console
  write(0, "\nComplete!\n", 10);

  return 0;
}

