#include <glm/gtc/matrix_transform.hpp>

#include "navigation/camera.hpp"

Camera::Camera(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& u):
  position(pos),
  direction(dir),
  up(u),

  pitch(0.0f),
  yaw(0.0f),
  fov(45.0f)
{
}

glm::vec3 Camera::get_right() const {
  return glm::normalize(glm::cross(direction, up));
}

/**
 * View matrix orients a scene in such a way to simulate camera's movement (inversion of camera's transformation matrix)
 * i.e. translation/rotation of scene in opposite directions to camera
 * Important: pitch/yaw angles rotate scene's object not camera
 */
glm::mat4 Camera::get_view() {
  // only camera direction is rotated by mouse in `rotate()`
  glm::mat4 view = glm::lookAt(position, position + direction, up);

  return view;
}
