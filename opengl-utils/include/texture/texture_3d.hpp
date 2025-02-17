#ifndef TEXTURE_3D_HPP
#define TEXTURE_3D_HPP

#include <vector>

#include "glad/glad.h"
#include "image.hpp"
#include "wrapping.hpp"
#include "texture.hpp"

struct Texture3D : Texture {
  Texture3D(const std::vector<Image>& images, GLenum index=GL_TEXTURE0, Wrapping wrapping=Wrapping::REPEAT, bool is_same_image=false);
  Texture3D(const Image& image, GLenum index=GL_TEXTURE0, Wrapping wrapping=Wrapping::REPEAT);

private:
  /* Same image data ptr. on all 6 faces (avoids double-free) */
  bool m_is_same_image;

  void from_images(const std::vector<Image>& images);
};

#endif // TEXTURE_3D_HPP
