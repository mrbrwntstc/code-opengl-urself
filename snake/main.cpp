#include "render.h"

int main(void)
{
  render::init();

  std::vector<render::vertex> vertices;
  while (!render::window::should_close())
  {
    // draw screen
    vertices.push_back({{ 0, 100}, {1, 0, 0}});
    vertices.push_back({{ 100, 100}, {1, 0, 0}});
    vertices.push_back({{ 100, 0}, {1, 0, 0}});
    vertices.push_back({{ 0, 0}, {1, 0, 0}});

    render::window::clear_screen();

    render::quad::draw(vertices);
    
    render::window::flush();
    vertices.clear();
  }

  render::cleanup();
  render::window::cleanup();
  return 0;
}