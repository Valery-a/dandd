#ifndef ATTRIBUTES_HPP
#define ATTRIBUTES_HPP

#include <glad/glad.h>
#include <string>
#include <vector>

struct Attribute {
  GLuint id;
  std::string name;
  unsigned int size;
  unsigned int stride;
  unsigned int offset;
};

namespace Attributes {
  unsigned int get_n_coords(const std::string& name_attribute, bool is_2d);
  std::vector<Attribute> get(const std::vector<std::string>& names_attibutes, unsigned int n_coords_vertex=0, bool is_2d=false);
}

#endif // ATTRIBUTES_HPP
