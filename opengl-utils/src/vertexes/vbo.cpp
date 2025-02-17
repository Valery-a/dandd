#include <vertexes/vbo.hpp>

/**
 * No polymorphism if `geometry` wasn't passed by ref (or as a pointer)
 * https://stackoverflow.com/a/15188950/2228912
 * 07-01-23: use std::move to avoid copy of vector items via copy constructor
 * @param is_dynamic Only true for <fps>/TextRenderer (Same vbo for each glyph with updated geometry eachtime)
 */
VBO::VBO(const Geometry& geometry, bool is_dynamic):
  positions(std::move(geometry.get_positions())),
  n_elements(geometry.get_n_elements())
{
  generate();
  bind();

  // transfer geometry vertexes to bound VBO (only reserve space for dynamic text glyphs)
  std::vector<float> vertexes = std::move(geometry.get_vertexes());
  GLenum type = is_dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
  const GLvoid* data = is_dynamic ? NULL : vertexes.data();
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexes.size(), data, type);

  // transfer vertexes indices if geometry has any
  std::vector<unsigned int> indices = std::move(geometry.get_indices());
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), type);

  unbind();
}

void VBO::generate() {
  glGenBuffers(1, &m_id_vbo);
  glGenBuffers(1, &m_id_ebo);
}

/**
 * Update vbo with vertexes from geometery (indices already set in ctor)
 * Used for rendering glyphs in <fps>`TextRenderer`
 */
void VBO::update(const Geometry& geometry) {
  std::vector<float> vertexes = std::move(geometry.get_vertexes());
  n_elements = geometry.get_n_elements();
  bind();
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * vertexes.size(), vertexes.data());
  unbind();
}

void VBO::bind() {
  glBindBuffer(GL_ARRAY_BUFFER, m_id_vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id_ebo);
}

void VBO::unbind() {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void VBO::free() {
  glDeleteBuffers(1, &m_id_vbo);
  glDeleteBuffers(1, &m_id_ebo);
}
