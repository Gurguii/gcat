#include <cstdint>
#include <memory>
#include <cstdlib>
#include <cstring>
#include <iostream>

class ProgressBar
{
  private:
  char *progress_bar_buffer = nullptr;
  uint64_t pbar_size = 0,total = 0,_read = 0;
  char icon;

  public:
  ~ProgressBar(){
    delete(progress_bar_buffer);
    progress_bar_buffer = nullptr;
  }
  ProgressBar(uint64_t total, char icon = '#'):total(total),icon(icon){};
  int initialize(uint64_t progress_bar_size = 100)
  {
    if (progress_bar_size <= 0) {
      return -1;
    }
    progress_bar_buffer = (char*)std::malloc(progress_bar_size);
    if (progress_bar_buffer == nullptr) {
      return -1;
    }
    memset(progress_bar_buffer,0,progress_bar_size);
    pbar_size = progress_bar_size;
    return 0;
  }
  const char* update(uint64_t read){
    _read += read;
    if(_read <= total){
      return (const char*)std::memset(progress_bar_buffer, icon, _read / (total/pbar_size));
    }
    return nullptr;
  }
  const char* get(){
    return progress_bar_buffer;
  }
};
