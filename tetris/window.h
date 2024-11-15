#ifndef __WINDOW_TETRIS_H__
#define __WINDOW_TETRIS_H__

#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>

static void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error: %s\n", description);
}

static bool key_left = false;
static bool key_right = false;
static bool key_down = false;
static bool key_rotate = false;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GLFW_TRUE);
  key_left = (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT));
  key_right = (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT));
  key_down = (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT));
  key_rotate = (key == GLFW_KEY_Z && (action == GLFW_PRESS || action == GLFW_REPEAT));
}

namespace window
{
  GLFWwindow* window;
  constexpr int width = 800;
  constexpr int height = 600;
  namespace input
  {
    bool left;
    bool right;
    bool down;
    bool rotate;

    void update()
    {
      left = key_left;
      right = key_right;
      down = key_down;
      rotate = key_rotate;
    }
  }
  void init()
  {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  #endif // __APPLE__

    window = glfwCreateWindow(width, height, "Tetris - OpenGL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGL();
    glfwSwapInterval(1);
  }
  void cleanup()
  {
    glfwDestroyWindow(window);
    glfwTerminate();
  }
  void flush()
  {
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  bool should_close()
  {
    return glfwWindowShouldClose(window);
  }
  void clear_screen()
  {
    glClear(GL_COLOR_BUFFER_BIT);
  }
}

#endif // WINDOW_TETRIS_H__
