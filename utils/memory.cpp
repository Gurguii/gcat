#include <stdio.h>
#include <sys/stat.h>
#include "../messages.cpp"

#define MAX_DYNAMIC_MEM (4096 * 4096) // 16Mb
#define BUFF_SIZE (4096 * 1024) // 4Mb

uint32_t TAKEN_MEM = 0;

void* allocate_cache_list(const char *filepath){
  FILE *file = fopen(filepath,"rb");
  if(file == nullptr){
    msg("couldn't open file\n",stdout,RED);
    return nullptr;
  }
  struct stat st;
  if(stat(filepath,&st)){
    fclose(file);
    return nullptr;
  };
  uint64_t size = st.st_size;
    if((TAKEN_MEM+size) > MAX_DYNAMIC_MEM){
    msg("allocating cache_list buffer exceeds MAX_DYNAMIC_MEM\n",stdout,RED);
    fclose(file);
    return nullptr;
  }
  return malloc(size); 
}
