#include <filesystem>

#include "texture/texture.hpp"

namespace fs = std::filesystem;

/**
 * Default constructor: fields not init on purpose (to avoid that texture unit of default normal tex hides that of diffuse in `ModelRenderer`)
 * also needed because lvalue in assignment `map[key] = value` (source: models/models.cpp) evals to a reference
 * https://stackoverflow.com/a/29826440/2228912
 * Update 15-01-23: needed bcoz derived class Texture2D also has a default ctor
 */
Texture::Texture()
{}

/* Used by children constructors to init this class's members */
Texture::Texture(GLuint t, GLenum index, Wrapping wrapping, const std::string& path):
  type(t),
  m_index(index),
  m_wrapping(wrapping),
  name(fs::path(path).stem())
{
}

void Texture::generate() {
  glGenTextures(1, &id);
}

void Texture::configure() {
  bind();

  // interpolation used when down/upscaling
  glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // wrapping method
  GLint wrapping_method;
  switch (m_wrapping) {
    case Wrapping::REPEAT:
      wrapping_method = GL_REPEAT;
      break;
    case Wrapping::STRETCH:
      wrapping_method = GL_CLAMP_TO_EDGE;
      break;
    case Wrapping::BLACK:
      wrapping_method = GL_CLAMP_TO_BORDER;
      break;
  }

  glTexParameteri(type, GL_TEXTURE_WRAP_S, wrapping_method);
  glTexParameteri(type, GL_TEXTURE_WRAP_T, wrapping_method);

  unbind();
}

void Texture::bind() {
  glBindTexture(type, id);
}

void Texture::unbind() {
  glBindTexture(type, 0);
}

/* Attach texture object id to texture unit m_index before `Renderer::draw()` */
void Texture::attach() {
  glActiveTexture(m_index);
  bind();
}

GLenum Texture::get_index() const {
  // used to pass texture index to shaders program
  return m_index - GL_TEXTURE0;
}

/* Get texture format from # of channels */
void Texture::set_format(int n_channels) {
  switch (n_channels) {
    case 1:
      format = GL_RED;
      break;
    case 3:
      format = GL_RGB;
      break;
    default:
      format = GL_RGBA;
  }
}

/* Get # of channels from image format */
int Texture::get_n_channels() const {
  int n;

  switch (format) {
    case GL_RED:
      n = 1;
      break;
    case GL_RGB:
      n = 3;
      break;
    default:
      n = 4;
  }

  return n;
}

/**
 * Delete texture
 */
void Texture::free() const {
  glDeleteTextures(1, &id);
}
