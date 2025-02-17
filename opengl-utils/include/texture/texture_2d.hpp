#ifndef TEXTURE_2D_HPP
#define TEXTURE_2D_HPP

#include "glad/glad.h"

#include "image.hpp"
#include "wrapping.hpp"
#include "texture.hpp"

struct Texture2D : Texture {
  /* Needed in <imgui-paint> project */
  int width;
  int height;

  /**
   * Default ctor needed by `Mesh::Mesh()` as its textures not init. till `Model` is constructed,
   * also by LevelRenderer::m_textures & FloorsRenderer::m_textures (i.e. map::operator[]())
   */
  Texture2D() = default;
  Texture2D(const Image& img, GLenum index=GL_TEXTURE0, Wrapping wrapping=Wrapping::REPEAT);

  void set_image(const Image& image);
  void set_subimage(const Image& subimage, const glm::uvec2& size, const glm::uvec2& offset);
  Image get_image();
};

#endif // TEXTURE_2D_HPP
