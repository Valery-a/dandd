#ifndef UNIFORMS_HPP
#define UNIFORMS_HPP

#include <variant>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

#include "texture/texture_2d.hpp"
#include "texture/texture_3d.hpp"

// dictionary type for heteregenous uniform values
using KeyUniform = std::string;
using ValueUniform = std::variant<
  bool,
  int,
  float,
  glm::vec2,
  glm::vec3,
  glm::mat4,
  Texture2D,
  Texture3D
>;
using Uniforms = std::unordered_map<KeyUniform, ValueUniform>;

#endif // UNIFORMS_HPP
