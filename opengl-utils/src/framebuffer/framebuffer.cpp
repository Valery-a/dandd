#include "framebuffer/framebuffer.hpp"
#include "framebuffer/framebuffer_exception.hpp"

Framebuffer::Framebuffer() {
  generate();
}

void Framebuffer::generate() {
  glGenFramebuffers(1, &m_id);
}

/* Attach 2D texture as a color buffer to bound framebuffer */
void Framebuffer::attach_texture(const Texture2D& texture) {
  bind();

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture.type, texture.id, 0);
  width = texture.width;
  height = texture.height;
  m_format = texture.format;
  n_channels = texture.get_n_channels();

  if (!is_complete()) {
    throw FramebufferException();
  }

  unbind();
}

/* Check if currently bound framebuffer is ready to use (has at least on buffer attached) */
bool Framebuffer::is_complete() {
  bind();
  bool status = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
  unbind();

  return status;
}

/**
 * Called each frame to clear fbo's color buffer before re-drawing
 * @param color Color components in [0, 1] (alpha = 1 => opaque)
 */
void Framebuffer::clear(const glm::vec4& color) {
  glClearColor(color.r, color.g, color.b, color.a);
  glClear(GL_COLOR_BUFFER_BIT);
}

void Framebuffer::bind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, m_id);
}

void Framebuffer::unbind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * Get pixel value at coords (x, y)
 * Origin at upper-left corner of the canvas
 * @return Contiguous rgb/rgba values (space for data allocated on stack/heap by calling code)
 */
void Framebuffer::get_pixel_value(int x, int y, unsigned char* data) {
  bind();
  glReadPixels(x, y, 1, 1, m_format, GL_UNSIGNED_BYTE, data);
  unbind();
}

void Framebuffer::free() {
  glDeleteFramebuffers(1, &m_id);
}
