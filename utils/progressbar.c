#include <stdint.h>
#include <memory.h>

void get_progress_bar(char bar_icon, uint64_t total, uint64_t read, char *progress_bar_buff, size_t progress_bar_size)
{
  memset(progress_bar_buff,bar_icon,read/(total/progress_bar_size));
}
