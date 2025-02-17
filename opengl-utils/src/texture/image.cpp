#include <iostream>
#include <cstring>

#include "texture/image.hpp"
#include "texture/image_exception.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

/**
 * Default constructor needed because lvalue in assignment `map[key] = value` (source: models/models.cpp) evals to a reference
 * and Texture's default constructor requires that Image has one too
 * https://stackoverflow.com/a/29826440/2228912
 */
Image::Image()
{}

/**
 * Load image
 * @param p Image path
 * @param flip: images (origin at upper-left) need to be flipped horizontally in OpenGL 3D (origin at bottom)
 * but not in ImGui because of 2D projection matrix used in project <imgui-example>
 */
Image::Image(const std::string& p, bool flip):
  path(p),
  m_needs_free(true)
{
  // opengl origin at lower-left corner of image
  if (flip) {
    stbi_set_flip_vertically_on_load(true);
  }

  // load image using its path
  std::cout << "Loading image: " << path << "\n";
  int desired_channels = 0;
  data = stbi_load(path.c_str(), &width, &height, &n_channels, desired_channels);

  if (data == nullptr) {
    throw ImageException();
  }
}

/**
 * Used to load glyph bitmap for a font into image
 * Also used by `Image::from_2d_array()` (<imgui-paint>)
 */
Image::Image(int w, int h, int n, unsigned char* ptr, bool needs_free):
  width(w),
  height(h),
  n_channels(n),
  data(ptr),
  path(""),
  m_needs_free(needs_free)
{
}

/* Save image in jpeg format */
void Image::save(const std::string& filename) {
  stbi_write_jpg(filename.c_str(), width, height, n_channels, data, 90);
}

void Image::free() const {
  // doesn't run for glyph bitmap (avoid double free, as freed auto by freetype)
  if (m_needs_free) {
    stbi_image_free(data);
  }
}

/**
 * Get pixel value at position i_pixel
 * @returns Vector containing `n_channels` components (monochrome, rgb, or rgba)
 */
std::vector<unsigned char> Image::get_pixel_value(unsigned int i_pixel) {
  switch (n_channels){
    case 4: // rgba
      return {
        data[n_channels*i_pixel],
        data[n_channels*i_pixel + 1],
        data[n_channels*i_pixel + 2],
        data[n_channels*i_pixel + 3],
      };
    case 3: // rgb
      return {
        data[n_channels*i_pixel],
        data[n_channels*i_pixel + 1],
        data[n_channels*i_pixel + 2],
      };
  }

  // monochrome
  return {
    data[i_pixel],
  };
}

/**
 * Transform 1D image vector data to 2D array
 * Easier to calculate local average on 2D array (than on 1D array) in <imgui-paint>
 */
unsigned char** Image::to_2d_array() const {
  size_t n_bytes_row = width * n_channels;
  unsigned char** data_2d = new unsigned char*[height];
  size_t offset = 0;

  for (size_t i_height = 0; i_height < height; i_height++) {
    data_2d[i_height] = new unsigned char[n_bytes_row];
    std::memcpy(data_2d[i_height], data + offset, n_bytes_row);
    offset += n_bytes_row;
  }

  return data_2d;
}

/**
 * Construct image after transforming 2D array image to a 1D data vector (to save with stb_image)
 * Needed in <imgui-paint>
 * @param width/height Passed as arg as `data_2d` is an array of pointers
 * cannot work out size of image beneath it from # of bytes (= size of pointers not content)
 */
Image Image::from_2d_array(unsigned char** data_2d, int width, int height, int n_channels) {
  size_t n_bytes = width * height * n_channels;
  size_t n_bytes_row = width * n_channels;
  unsigned char* data_out = new unsigned char[n_bytes];

  size_t offset = 0;
  for (size_t i_height = 0; i_height < height; i_height++) {
    std::memcpy(data_out + offset, data_2d[i_height], n_bytes_row);
    offset += n_bytes_row;
  }

  // construct image (path not null so data can be freed later)
  Image image_out(width, height, n_channels, data_out);
  return image_out;
}
