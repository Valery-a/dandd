#include <vector>
#include <iostream>

#include "shader/shader.hpp"

Shader::Shader(const std::string& source, GLenum type):
  id(0)
{
  // The Cherno: https://www.youtube.com/watch?v=71BLZwRGUJE
  const char* source_shader_char = source.c_str();

  // compile shader (glCreateShader() returns 0 on failure)
  m_type = type;
  id = glCreateShader(m_type);
  glShaderSource(id, 1, &source_shader_char, NULL);
  glCompileShader(id);

  // error handling for shader compilation
  GLint result;
  glGetShaderiv(id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    GLint length;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
    std::vector<GLchar> message(length);
    glGetShaderInfoLog(id, length, NULL, message.data());
    std::string type_shader_str = (m_type == GL_VERTEX_SHADER ? "vertex" : "fragment");
    std::cout << "Shader " << type_shader_str << ": " << message.data() << "\n";

    free();
  }
}

void Shader::free() {
  glDeleteShader(id);
}
