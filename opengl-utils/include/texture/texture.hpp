#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <vector>
#include <string>
#include <glm/glm.hpp>

#include "glad/glad.h"
#include "wrapping.hpp"

/* Abstract class (cannot be instantiated) */
struct Texture {
  /* Needed in `Framebuffer` class */
  GLuint id;
  GLuint type;
  GLenum format;

  /* useful to debug */
  std::string name;

  GLenum get_index() const;
  void attach();
  int get_n_channels() const;
  void free() const;

  /**
   * Virtual dtor required in polymorphic base class, otherwise derived class' dtor not called in:
   * Texture *tex = new Texture2D(); delete tex; // clang throws a warning
   * https://stackoverflow.com/a/15903538/2228912
   */
  virtual ~Texture() = default;

protected:
  GLenum m_index;

  /* whether texture is repeated, stretched or set to black beyond [0, 1] */
  Wrapping m_wrapping;

  void generate();
  void configure();
  void bind();
  void unbind();
  void set_format(int n_channels);

  /**
   * Protected ctors (to show explicitely the class is abstract)
   * Default ctor mandatory for derived class Texture2D's default ctor
   */
  Texture();
  Texture(GLuint t, GLenum index=GL_TEXTURE0, Wrapping wrapping=Wrapping::REPEAT, const std::string& path="");
};

#endif // TEXTURE_HPP
