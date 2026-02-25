#ifndef CORE_QUAD_H
#define CORE_QUAD_H

namespace core::quad {
namespace internal {
extern unsigned int quad_vertex_attrib, quad_vertex_buffer, quad_index_buffer;
extern unsigned int tex_vertex_attrib, tex_vertex_buffer, tex_index_buffer;
extern unsigned int full_quad_vertex_attrib, full_quad_vertex_buffer, full_quad_index_buffer;
} // namespace internal
enum QuadType {
  QUAD_ONLY,
  WITH_TEXCOORDS,
  FULL_QUAD
};
void Init();
void Render(QuadType type);
void Quit();
} // namespace core::quad

#endif  // CORE_QUAD_H