#include "window.h"
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

std::string tetrominos[7];
constexpr int length_block = 30;
constexpr int width_field = 12;
constexpr int height_field = 20;
constexpr int start_field = 220;
constexpr int end_field = 580;

unsigned char* field = nullptr;
struct vertex
{
    float pos[2];
    float color[3];
};
std::vector<vertex> vertices_field;
unsigned int vao_field;
unsigned int vbo_field;
unsigned int ebo_field;
constexpr int num_max_quads = width_field * height_field;
constexpr int num_max_vertices = num_max_quads * 4; // 4 points per quad
constexpr int num_max_indices = num_max_quads * 6; // 6 indices per quad

unsigned int shader_program_field;
const char* source_shader_vertex_field =
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

const char* source_shader_fragment_field =
"#version 330 core\n"
"in vec3 color;\n"
"out vec4 fragment;\n"
"void main()\n"
"{\n"
" fragment = vec4(color, 1.0);\n"
"}\n";

int rotate(int px, int py, int orientation)
{
    int pi = 0;
    switch (orientation % 4)
    {
        case 0: pi = py * 4 + px; break;
        case 1: pi = 12 + py - (px * 4); break;
        case 2: pi = 15 - (py * 4) - px; break;
        case 3: pi = 3 - py + (px * 4); break;
    }
    return pi;
}

bool does_piece_fit(int index_tetrominos, int orientation, int x_topleft, int y_topleft)
{
  for (int px = 0; px < 4; px++)
  {
    for (int py = 0; py < 4; py++)
    {
      int index_piece = rotate(px, py, orientation);
      int index_field = (y_topleft + py) * width_field + (x_topleft + px);
      if (x_topleft + px >= 0 && x_topleft + px < width_field)
      {
        if (y_topleft + py >= 0 && y_topleft + py <= height_field)
        {
          if (tetrominos[index_tetrominos][index_piece] != '.' && field[index_field] != 0)
            return false;
        }
      }
    }
  }
  return true;
}

void initialize_tetrominos()
{
  // Define the shapes of tetrominos
	tetrominos[0].append("..X...X...X...X.");
	tetrominos[1].append("..X..XX...X.....");
	tetrominos[2].append(".....XX..XX.....");
	tetrominos[3].append("..X..XX..X......");
	tetrominos[4].append(".X...XX...X.....");
	tetrominos[5].append(".X...X...XX.....");
	tetrominos[6].append("..X...X..XX.....");
}

void initialize_field()
{
  field = new unsigned char[width_field * height_field + 1];
  for(int col = 0; col < width_field; col++)
  {
    for(int row = 0; row <= height_field; row++)
    {
      field[row * width_field + col] = ((col-1) == -1 || (col+1) == width_field || row == height_field) ? 9 : 0;
    }
  }
}

void setup_shaders()
{
    unsigned int shader_vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shader_vertex, 1, &source_shader_vertex_field, NULL);
    glCompileShader(shader_vertex);
    int success;
    char infoLog[512];
    glGetShaderiv(shader_vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader_vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int shader_fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shader_fragment, 1, &source_shader_fragment_field, NULL);
    glCompileShader(shader_fragment);
    glGetShaderiv(shader_fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader_fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    shader_program_field = glCreateProgram();
    glAttachShader(shader_program_field, shader_vertex);
    glAttachShader(shader_program_field, shader_fragment);
    glLinkProgram(shader_program_field);
    glGetProgramiv(shader_program_field, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader_program_field, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(shader_vertex);
    glDeleteShader(shader_fragment);

    // projection matrix
    glUseProgram(shader_program_field);
    glm::mat4 projection = glm::ortho(0.0f, (float)window::width, (float)window::height, 0.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader_program_field, "projection"), 1, GL_FALSE, &projection[0][0]);
}

void setup_buffers()
{
    glGenVertexArrays(1, &vao_field);
    glGenBuffers(1, &vbo_field);
    glGenBuffers(1, &ebo_field);

    glBindVertexArray(vao_field);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_field);
    glBufferData(GL_ARRAY_BUFFER, num_max_vertices * sizeof(vertex), NULL, GL_DYNAMIC_DRAW);

    unsigned int indices[num_max_indices];
    for (int i = 0, offset = 0; i < num_max_indices; i += 6, offset += 4)
    {
        indices[i + 0] = offset + 0;
        indices[i + 1] = offset + 1;
        indices[i + 2] = offset + 2;
        indices[i + 3] = offset + 2;
        indices[i + 4] = offset + 3;
        indices[i + 5] = offset + 0;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_field);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_max_indices * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, pos));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, color));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void render_batch()
{
    window::clear_screen();

    glBindVertexArray(vao_field);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_field);

    // Only update the buffer with the current batch of vertices
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices_field.size() * sizeof(vertex), vertices_field.data());

    glUseProgram(shader_program_field);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_field);

    // Calculate how many indices to draw based on number of quads
    int num_indices_to_draw = (vertices_field.size() >> 2) * 6;
    glDrawElements(GL_TRIANGLES, num_indices_to_draw, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

int main()
{
    window::init();
    initialize_tetrominos();
    initialize_field();
    setup_shaders();
    setup_buffers();

    int index_tetromino = 6;
    int orientation_current = 0;
    int x_current = width_field / 2;
    int y_current = 0;
    bool rotate_hold = true;

    while (!window::should_close())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        window::input::update();

        // handle player input
        // Handle player movement
		    x_current += (window::input::right && does_piece_fit(index_tetromino, orientation_current, x_current + 1, y_current)) ? 1 : 0;
		    x_current -= (window::input::left && does_piece_fit(index_tetromino, orientation_current, x_current - 1, y_current)) ? 1 : 0;		
		    y_current += (window::input::down && does_piece_fit(index_tetromino, orientation_current, x_current, y_current + 1)) ? 1 : 0;

        if(window::input::rotate)
        {
          orientation_current += (rotate_hold &&  does_piece_fit(index_tetromino, orientation_current + 1, x_current, y_current)) ? 1: 0;
          rotate_hold = false;
        }
        else
        {
          rotate_hold = true;
        }

        // draw current tetromino
        // ---
        for(int px = 0; px < 4; px++)
        {
          for(int py = 0; py < 4; py++)
          {
            if (tetrominos[index_tetromino][rotate(px, py, orientation_current)] != '.')
            {
              // Calculate the base position of the tetromino block
              float xPos = start_field + (x_current + px) * length_block;
              float yPos = (y_current + py) * length_block;

              // Create the 4 vertices for the quad (tetromino block)
              vertex topleft = { {xPos, yPos}, {1.f, 0.f, 0.f} };
              vertex topright = { {xPos + length_block, yPos}, {1.f, 0.f, 0.f} };
              vertex bottomright = { {xPos + length_block, yPos + length_block}, {1.f, 0.f, 0.f} };
              vertex bottomleft = { {xPos, yPos + length_block}, {1.f, 0.f, 0.f} };

              // Add the vertices for the current tetromino
              vertices_field.push_back(topleft);
              vertices_field.push_back(topright);
              vertices_field.push_back(bottomright);
              vertices_field.push_back(bottomleft);
            }
          }
        }
        // ---

        // Render all accumulated quads (tetrominoes)
        render_batch();

        // Clear the vertices after rendering
        vertices_field.clear();

        window::flush();
    }

    window::cleanup();
    delete [] field;
    return 0;
}
