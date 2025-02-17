// Inspired by https://learnopengl.com/Getting-started/Camera
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>

struct Camera {
  /**
   * Vertical/horizontal angles in radian (rotation around x-axis/y-axis)
   * y-angle used to tie PC's directions of movement to camera (not world axes)
   */
  float pitch;
  float yaw;

  /* used in 3D projection matrix (zoom-in corresponds to lower field-of-view) */
  float fov;

  /* camera position & look at direction vector */
  glm::vec3 position;
  glm::vec3 direction;

  /* Needed to calculate frustum planes normals (by cross-product) */
  glm::vec3 up;

  Camera(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& u);
  glm::mat4 get_view();
  glm::vec3 get_right() const;
};

#endif // CAMERA_HPP
