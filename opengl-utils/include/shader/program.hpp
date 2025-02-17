// inspired by: https://learnopengl.com/Getting-started/Shaders
#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>

#include "shader/uniforms.hpp"

struct Program {
  GLuint id;

  Program(const std::string& path_vertex, const std::string& path_fragment, bool instancing = false);
  void use();
  void unuse();
  void free();
  bool has_failed();
  GLuint define_attribute(const std::string& attribute) const;
  void set_uniforms(const Uniforms& uniforms);

private:
  void set_bool(const std::string& name, bool value);
  void set_int(const std::string& name, int value);
  void set_float(const std::string& name, float value);
  void set_vec2(const std::string& name, const glm::vec2& vect);
  void set_vec3(const std::string& name, const glm::vec3& vect);
  void set_mat4(const std::string& name, const glm::mat4& mat);
};

#endif // PROGRAM_HPP
