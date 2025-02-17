#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vector>

#include "shader/program.hpp"
#include "shader/uniforms.hpp"

#include "vertexes/attributes.hpp"
#include "vertexes/vao.hpp"
#include "vertexes/vbo.hpp"

#include "math/transformation.hpp"

struct Renderer {
  /* accessed in derived class `TextRenderer` & in `Player` */
  VBO vbo;

  /* used to switch shaders in <imgui-paint> */
  Program program;

  Renderer(const Program& pgm, const Geometry& geometry, const std::vector<Attribute>& attributes, bool is_text=false);
  virtual void free() final;

  void set_transform(const Transformation& transform);

  void draw(const Uniforms& u);
  void draw_plane(const Uniforms& u);
  void draw_lines(const Uniforms& u, unsigned int n_elements=0, size_t offset=0);
  void draw_with_outlines(const Uniforms& u);

  template <typename T>
  void set_uniform_arr(const std::string& name, const std::vector<T>& u);
private:
  VAO m_vao;
  Uniforms m_uniforms;

  /* # of instances of same geometry rendered at once: https://learnopengl.com/Advanced-OpenGL/Instancing */
  GLsizei m_n_instances;

  void _draw(const Uniforms& u, GLenum mode, unsigned int n_elements=0, size_t offset=0);
};

#endif // RENDERER_HPP
