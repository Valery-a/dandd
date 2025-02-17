// inspired by: https://learnopengl.com/Getting-started/Shaders
#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h>
#include <string>

struct Shader {
  GLuint id;

  Shader(const std::string& source, GLenum type);
  void free();

private:
  GLenum m_type;
};

#endif // SHADER_HPP
