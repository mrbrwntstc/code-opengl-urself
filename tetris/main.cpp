#include "window.h"

int main(void)
{
  window::init();

  while (!window::should_close())
  {
    window::flush();
  }

  window::cleanup();

  return 0;
}