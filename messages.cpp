#include <iostream>
#include <cstdint>

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"


void msg(const char *color_code, const char *msg, FILE* target){
  fprintf(target,"%s%s%s\n",color_code,msg,RESET);
}
