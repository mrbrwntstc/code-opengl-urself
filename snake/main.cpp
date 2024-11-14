#include "window.h"

int main(void)
{
  window::init();

  while (!window::should_close())
  {
    window::clear_screen();
    window::flush();
  }

  window::cleanup();
  return 0;
}