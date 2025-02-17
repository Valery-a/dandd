#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <glm/glm.hpp>

struct Light {
  glm::vec3 position;
  glm::vec3 color; // needed to show light source
  glm::vec3 ambiant;
  glm::vec3 diffuse;
  glm::vec3 specular;

  Light(const glm::vec3& p, const glm::vec3& c);
};

#endif // LIGHT_HPP
