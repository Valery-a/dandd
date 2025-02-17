#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <string>
#include <vector>

/* Wrapper around image pointer returned by `stbi_load()` */
struct Image {
  int width;
  int height;
  int n_channels;
  unsigned char* data;
  std::string path;

  Image();
  Image(const std::string& p, bool flip=true);
  Image(int w, int h, int n, unsigned char* ptr, bool needs_free=true);
  void free() const;

  void save(const std::string& filename);
  std::vector<unsigned char> get_pixel_value(unsigned int i_pixel);

  unsigned char** to_2d_array() const;
  static Image from_2d_array(unsigned char** data_2d, int width, int height, int n_channels);

private:
  /* Avoids double-free for font bitmaps */
  bool m_needs_free;
};

#endif // IMAGE_HPP
