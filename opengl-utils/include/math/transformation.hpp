#ifndef TRANSFORMATION_HPP
#define TRANSFORMATION_HPP

#include <glm/glm.hpp>
#include <vector>

/**
 * Aggregator for transformation matrices (to avoid passing 3 params to `Renderer`)
 */
struct Transformation {
  std::vector<glm::mat4> models;
  glm::mat4 view;
  glm::mat4 projection;

  Transformation() = default;
  Transformation(const std::vector<glm::mat4>& m, const glm::mat4& v, const glm::mat4& p);
};

#endif // TRANSFORMATION_HPP
