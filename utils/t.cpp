#include <iostream>
#include "progressbar.cpp"
#include <unistd.h>
int main()
{
  ProgressBar bar(1000);
  if(bar.initialize()){
    std::cerr << "initialize failed\n";
    return -1;
  };
  while(bar.update(100)){
    std::cout << bar.get() << "\r" << std::flush;
    sleep(1);
  }
}
