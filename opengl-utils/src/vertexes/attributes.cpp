#include <numeric>

#include "vertexes/attributes.hpp"

unsigned int Attributes::get_n_coords(const std::string& name_attribute, bool is_2d) {
  if (name_attribute == "position")
    return (is_2d ? 2 : 3);
  else if (name_attribute == "normal")
    return 3;
  else if (name_attribute == "texture_coord")
    return 2;
  else if (name_attribute == "tangent")
    return 3;
  return 0;
};

/**
 * Used to pass as a 3rd parameter to Renderer::Renderer()
 * @param names_attributes Must provided in the right order
 * @param n_coords_vertex Needed only when geometries' vertexes includes other attributes not provided here
 * @param is_2d Only relevant to surfaces (i.e. 2D positions not 3D)
 */
std::vector<Attribute> Attributes::get(const std::vector<std::string>& names_attibutes, unsigned int n_coords_vertex, bool is_2d) {
  // calculate # of coords if not provided (i.e. n_coords_vertex = 0)
  unsigned int n_coords_total = n_coords_vertex;
  if (n_coords_total == 0) {
    auto sum = [&](unsigned int acc, const std::string& name_attribute) {
      unsigned int n_coords_attribute = get_n_coords(name_attribute, is_2d);
      return acc + n_coords_attribute;
    };
    n_coords_total = std::accumulate(names_attibutes.begin(), names_attibutes.end(), 0, sum);
  }

  size_t n_attributes = names_attibutes.size();
  std::vector<Attribute> attributes(n_attributes);
  unsigned int offset = 0;

  for (unsigned int i_attribute = 0; i_attribute < n_attributes; ++i_attribute) {
    std::string name_attribute = names_attibutes[i_attribute];
    unsigned int n_coords_attribute = get_n_coords(name_attribute, is_2d);
    Attribute attribute = { i_attribute, name_attribute, n_coords_attribute, n_coords_total, offset };
    attributes[i_attribute] = attribute;
    offset += n_coords_attribute;
  }

  return attributes;
}
