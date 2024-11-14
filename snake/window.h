#ifndef __WINDOW_SNAKE_H__
#define __WINDOW_SNAKE_H__

#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>

static void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error: %s\n", description);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

namespace window
{
  constexpr int width = 640;
  constexpr int height = 480;
  GLFWwindow* window = nullptr;

  void init()
  {
    glfwSetErrorCallback(error_callback);
 
    if (!glfwInit())
      exit(EXIT_FAILURE);
 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
 
    window = glfwCreateWindow(width, height, "Snake - OpenGL", NULL, NULL);
    if (!window)
    {
      glfwTerminate();
      exit(EXIT_FAILURE);
    }
 
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
 
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glfwSwapInterval(1);
  }

  bool should_close()
  {
    return glfwWindowShouldClose(window);
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

  void clear_screen()
  {
    glClear(GL_COLOR_BUFFER_BIT);
  }
}

#endif // __WINDOW_SNAKE_H__