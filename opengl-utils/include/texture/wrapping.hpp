#ifndef WRAPPING_HPP
#define WRAPPING_HPP

/* texture sampling when uv-coords outside [0, 1] */
enum class Wrapping {
  REPEAT, // repeat texture by default (suitable for small revolver textures)
  STRETCH, // wrap texture around mesh
  BLACK // uv-coords outside [0, 1] set to black (for tooltip in <imgui-example>)
};

#endif // WRAPPING_HPP
