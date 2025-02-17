#include "framebuffer/framebuffer_exception.hpp"

FramebufferException::FramebufferException():
  m_message("Framebuffer not complete")
{
}

const char* FramebufferException::what() const noexcept {
  return m_message.c_str();
}
