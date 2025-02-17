#include "texture/texture_3d.hpp"

void Texture3D::from_images(const std::vector<Image>& images) {
  bind();

  // 6-sided texture cube using given images
  for (size_t i_texture = 0; i_texture < images.size(); i_texture++) {
    Image image = images[i_texture];
    set_format(image.n_channels);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i_texture, 0, format, image.width, image.height, 0, format, GL_UNSIGNED_BYTE, image.data);
  }

  unbind();

  // free images pointers
  for (const Image& image : images) {
    image.free();

    // free only first image when duplicated on 6 faces (avoids double-free)
    if (m_is_same_image)
      break;
  }
}

/**
 * Used to init all faces textures to same image
 * `std::vector` creates distinct image copies (copy by value) but with same data ptr
 *   => free only once to avoid double-free
 */
Texture3D::Texture3D(const Image& image, GLenum index, Wrapping wrapping):
  Texture3D(std::vector<Image>(6, image), index, wrapping, true)
{
}

/* Used also as a delegating constructor */
Texture3D::Texture3D(const std::vector<Image>& images, GLenum index, Wrapping wrapping, bool is_same_image):
  Texture(GL_TEXTURE_CUBE_MAP, index, wrapping, images[0].path),
  m_is_same_image(is_same_image)
{
  generate();
  configure();
  from_images(images);
}
