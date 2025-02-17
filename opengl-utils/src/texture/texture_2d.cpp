#include "texture/texture_2d.hpp"

Texture2D::Texture2D(const Image& img, GLenum index, Wrapping wrapping):
  Texture(GL_TEXTURE_2D, index, wrapping, img.path)
{
  generate();
  configure();
  set_image(img);
}

/**
 * Retrieve image data from opengl texture (gpu -> cpu)
 * Called before saving image the user painted on with nanovg in <imgui-paint>
 */
Image Texture2D::get_image() {
  bind();

  int n_channels = get_n_channels();
  Image image(width, height, n_channels, nullptr);
  image.data = new unsigned char[width * height * n_channels];
  glGetTexImage(type, 0, format, GL_UNSIGNED_BYTE, image.data);

  unbind();

  return image;
}

/*
 * Update only subset of image texture
 * Used in `imgui-example` project so Brush tool is more fluid (no discontinuities between circles)
 */
void Texture2D::set_subimage(const Image& subimage, const glm::uvec2& size, const glm::uvec2& offset) {
  // copy image subset to gpu (subimage pointer freed from calling code)
  bind();
  glTexSubImage2D(type, 0, offset.x, offset.y, size.x, size.y, format, GL_UNSIGNED_BYTE, subimage.data);
  unbind();
}

/*
 * Set texture image
 * Used to update texture image from loaded path in `imgui-example` project
 */
void Texture2D::set_image(const Image& image) {
  // 2d texture from given image (save width & height for HUD scaling)
  width = image.width;
  height = image.height;
  set_format(image.n_channels);

  // copy image to gpu (image pointer could be freed after `glTexImage2D`)
  bind();
  glTexImage2D(type, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image.data);
  unbind();

  // free image pointer
  image.free();
}
