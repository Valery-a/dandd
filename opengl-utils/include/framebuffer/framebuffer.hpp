#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include "glad/glad.h"
#include "texture/texture_2d.hpp"

/**
 * Wrapper around OpenGL's framebuffer object (FBO)
 * https://learnopengl.com/Advanced-OpenGL/Framebuffers
 */
struct Framebuffer {
  int width;
  int height;
  int n_channels;

  Framebuffer();
  bool is_complete();
  void free();
  void bind() const;
  void unbind() const;
  void clear(const glm::vec4& color);
  void get_pixel_value(int x, int y, unsigned char* data);
  void attach_texture(const Texture2D& texture);

private:
  GLuint m_id;
  GLenum m_format;

  void generate();
};

#endif
