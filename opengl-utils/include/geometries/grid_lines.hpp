#ifndef GRID_LINES_HPP
#define GRID_LINES_HPP

#include "geometries/geometry.hpp"

/**
 * 2d Grid lying on the horizontal xz plane and consisting of:
 * - 10 lines parallel to x-axis
 * - 10 lines perpendicular to them (parallel to z-axis)
 */
namespace geometry {
  class GridLines : public Geometry {
  public:
    GridLines(unsigned int n_lines);

  private:
    unsigned int m_n_lines;

    void set_vertexes();
    void set_indices();
    void set_n_elements();
  };
}

#endif // GRID_LINES_HPP
