#ifndef VBO_HPP
#define VBO_HPP

#include <glad/glad.h>
#include <geometries/geometry.hpp>

/* Manage VBO & EBO and encapsulate mesh vertexes & their indices */
struct VBO {
  /* Needed by `Renderer::draw()` */
  unsigned int n_elements;

  /* needed by <fps>/`Target` class to calculate its bbox */
  std::vector<glm::vec3> positions;

  VBO(const Geometry& geometry, bool is_dynamic=false);
  void free();
  void bind();
  void unbind();
  void generate();
  void update(const Geometry& geometry);

private:
  GLuint m_id_vbo;
  GLuint m_id_ebo;
};

#endif // VBO_HPP
