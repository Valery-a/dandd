#include "texture/image_exception.hpp"

ImageException::ImageException():
  m_message("Image not found")
{
}

const char* ImageException::what() const noexcept {
  return m_message.c_str();
}
