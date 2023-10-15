#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define URANDOM "/dev/urandom"
#define BUFFSIZE (4 * 1024 * 1024) // 4MB

void generate_data(uint64_t bytes)
{
  unsigned char buffer[BUFFSIZE]; // 4MB
  unsigned char printable_buffer[BUFFSIZE]; // 4MB
  unsigned char *printptr = printable_buffer;
  unsigned char *buffptr = buffer;
  FILE *src = fopen(URANDOM,"r");
  uint64_t read = 0;

  while (!feof(src) && read < bytes) {
    size_t bytes_read = fread(buffer, 1, BUFFSIZE, src);
    if (bytes_read == 0) {
      break;
    }

    while (buffptr != NULL) {
      if ((*buffptr) < 126 && (*buffptr) > 37) {
        // printable
        memcpy(printptr, buffptr, 1);
        printf("%c", *(buffptr));
        if (++read == bytes) {
          break;
        };
        ++printptr;
      }
      ++buffptr;
    }

    if (read < bytes) {
      // We didn't read all of the requested bytes, so write the remaining printable characters in the buffer
      fwrite(printable_buffer, 1, read, stdout);
    }
  }

  free(printptr);
  free(buffptr);
  fclose(src);
  return;
}

int main(int argc, const char **args)
{
  if (argc != 2) {
    printf("usage: %s <size>g|m|b\ne.g %s 4m - would generate 4 megabytes\n", args[0], args[0]);
    return -1;
  }

  if (fopen(URANDOM, "r") == NULL) {
    fprintf(stderr, "[¡] Char block %s does not exist or is not accesible\n", URANDOM);
    return -1;
  }

  struct stat blockdev;
  stat(URANDOM, &blockdev);
  if ((blockdev.st_mode & S_IFMT) != S_IFCHR) {
    fprintf(stderr, "[¡] '%s' is not a char block\n", URANDOM);
    return -1;
  }

  char unit;
  memcpy(&unit, &args[1][strlen(args[1]) - 1], 1);
  uint64_t size = 0;
  size = strtol(args[1], NULL, 10);
  if (size <= 0) {
    fprintf(stderr, "[¡] Size can't be < 0\n");
    return -1;
  }

  printf("%lu\n", size);

  switch (unit) {
    case 'g':
      // gigabyte
      generate_data(size * 1024 * 1024 * 1024);
      break;
    case 'm':
      // megabyte
      generate_data(size * 1024 * 1024);
      break;
    case 'k':
      // kilobyte
      generate_data(size * 1024);
      break;
    case 'b':
      // byte
      generate_data(size);
      break;
    default:
      printf("unknown unit %c\n", unit);
      break;
  }

  return 0;
}

