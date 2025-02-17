#include <sstream>

#include "shader/uniforms_manager.hpp"

/* Append to uniforms_dst from uniforms_src */
void UniformsManager::copy(Uniforms& uniforms_dst, const Uniforms& uniforms_src) {
  for (const auto& item : uniforms_src) {
    KeyUniform key_uniform(item.first);
    ValueUniform value_uniform(item.second);
    uniforms_dst[key_uniform] = value_uniform;
  }
}

/**
 * Append to uniforms_dst each item in values with an indexed name
 * Suitable for instancing cloned object with different props (e.g. positions...)
 */
template <typename T>
void UniformsManager::set_arr(Uniforms& uniforms_dst, const std::string& name, const std::vector<T>& values) {
  // case of array of structs fields (see lights uniform in phong shader)
  size_t i_dot = name.find(".");
  bool is_struct_field = (i_dot != std::string::npos);

  std::string field_name, struct_name;
  if (is_struct_field) {
    struct_name = name.substr(0, i_dot);
    field_name = name.substr(i_dot + 1);
  }

  for (size_t i_instance = 0; i_instance < values.size(); ++i_instance) {
    std::stringstream stream;
    if (!is_struct_field)
      stream << name << "[" << i_instance  << "]";
    else
      stream << struct_name << "[" << i_instance  << "]." << field_name;

    uniforms_dst[stream.str()] = values[i_instance];
  }
}

// template instantiation (avoids linking error)
template void UniformsManager::set_arr(Uniforms&, const std::string&, const std::vector<glm::vec3>&);
template void UniformsManager::set_arr(Uniforms&, const std::string&, const std::vector<glm::mat4>&);
template void UniformsManager::set_arr(Uniforms&, const std::string&, const std::vector<Texture2D>&);
