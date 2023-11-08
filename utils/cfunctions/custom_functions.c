#include "custom_functions.h"
uint8_t gstrcmp(const char *buff1, const char* buff2, int n){
  for(int i = 0; i < n; ++i)
  {
    if(*(buff1+i) != *(buff2+i)){
      return 0;
    }
  }
  return 1;
}
