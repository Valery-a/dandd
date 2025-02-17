#ifndef UNIFORMS_MANAGER_HPP
#define UNIFORMS_MANAGER_HPP

#include "shader/uniforms.hpp"

/* Uniforms settings delegator (used internally by Renderer) */
namespace UniformsManager {
  void copy(Uniforms& uniforms_dst, const Uniforms& uniforms_src);

  template <typename T>
  void set_arr(Uniforms& uniforms_dst, const std::string& name, const std::vector<T>& values);
};

#endif // UNIFORMS_MANAGER_HPP
