#include "window.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static const char *source_vertex_program_quad =
"#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"uniform mat4 projection;\n"
"out vec3 color;\n"
"void main()\n"
"{\n"
" gl_Position = projection * vec4(aPos, 0.0, 1.0);\n"
" color = aColor;\n"
"}\n";
static const char *source_fragment_program_quad =
"#version 330 core\n"
"in vec3 color;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
" FragColor = vec4(color, 1.0);\n"
"}\n";

namespace render
{
  struct vertex
  {
    float xy[2];
    float rgb[3];
  };

  namespace quad
  {
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;
    constexpr int num_quads_batch = window::width * window::height;
    constexpr int num_vertices_batch = num_quads_batch * 4;
    constexpr int num_indices_batch = num_quads_batch * 6;
    unsigned int program_shader;
    void init()
    {
      // shader program
      {
        int success;
        char infoLog[512];

        unsigned int shader_vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(shader_vertex, 1, &source_vertex_program_quad, NULL);
        glCompileShader(shader_vertex);
        glGetShaderiv(shader_vertex, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader_vertex, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        unsigned int shader_fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(shader_fragment, 1, &source_fragment_program_quad, NULL);
        glCompileShader(shader_fragment);
        glGetShaderiv(shader_fragment, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader_fragment, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        program_shader = glCreateProgram();
        glAttachShader(program_shader, shader_vertex);
        glAttachShader(program_shader, shader_fragment);
        glLinkProgram(program_shader);
        glGetProgramiv(program_shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(program_shader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        glDeleteShader(shader_vertex);
        glDeleteShader(shader_fragment);

        // one-time set uniforms
        glUseProgram(program_shader);
        glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(window::width), static_cast<float>(window::height), 0.0f, -1.0f, 1.0f);
        glUniformMatrix4fv(glGetUniformLocation(program_shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

      } // shader program

      // buffers
      {
        // std::vector<render::vertex> vertices;

        // vertices.push_back({{ 0, 100}, {1, 0, 0}});
        // vertices.push_back({{ 100, 100}, {1, 0, 0}});
        // vertices.push_back({{ 100, 0}, {1, 0, 0}});
        // vertices.push_back({{ 0, 0}, {1, 0, 0}});

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, num_vertices_batch, NULL, GL_DYNAMIC_DRAW);

        unsigned int indices[num_indices_batch];
        for(int i = 0, offset = 0; i < num_indices_batch; i+=6, offset+=4)
        {
          indices[i + 0] = offset + 0;
          indices[i + 1] = offset + 1;
          indices[i + 2] = offset + 2;
          indices[i + 3] = offset + 2;
          indices[i + 4] = offset + 3;
          indices[i + 5] = offset + 0;
        }
        // unsigned int indices[] =
        // {
        //   0, 1, 2,
        //   2, 3, 0
        // };

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, xy));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, rgb));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
      } // buffers
    }

    void cleanup()
    {
      { // shaders
        glDeleteProgram(program_shader);
      }

      { // buffers
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
      }
    }
  
    void draw(std::vector<vertex>& vertices)
    {
      glBindVertexArray(vao);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(vertex), vertices.data());
      glUseProgram(program_shader);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
      glDrawElements(GL_TRIANGLES, (vertices.size() >> 2) * 6, GL_UNSIGNED_INT, 0);
    }
  }

  void init()
  {
    quad::init();
  }

  void cleanup()
  {
    quad::cleanup();
  }
}

int main(void)
{
  window::init();
  render::init();

  std::vector<render::vertex> vertices;
  while (!window::should_close())
  {
    // draw screen
    vertices.push_back({{ 0, 100}, {1, 0, 0}});
    vertices.push_back({{ 100, 100}, {1, 0, 0}});
    vertices.push_back({{ 100, 0}, {1, 0, 0}});
    vertices.push_back({{ 0, 0}, {1, 0, 0}});

    window::clear_screen();

    render::quad::draw(vertices);
    
    window::flush();
    vertices.clear();
  }

  render::cleanup();
  window::cleanup();
  return 0;
}