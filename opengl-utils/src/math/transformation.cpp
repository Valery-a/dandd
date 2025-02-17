#include "math/transformation.hpp"

/**
 * Model matrix not needed as we'll never convert back to local space
 * Projection corresponds to projection 3D matrix (not 2D one)
 */
Transformation::Transformation(const std::vector<glm::mat4>& ms, const glm::mat4& v, const glm::mat4& p):
  models(ms),
  view(v),
  projection(p)
{
}
