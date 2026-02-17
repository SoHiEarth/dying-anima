#pragma once

namespace core::quad {
namespace internal {
extern unsigned int quad_vertex_attrib, quad_vertex_buffer, quad_index_buffer;
extern unsigned int tex_vertex_attrib, tex_vertex_buffer, tex_index_buffer;
} // namespace internal
enum QuadType {
  QUAD_ONLY,
  WITH_TEXCOORDS,
};
void Init();
void Render(QuadType type);
void Quit();
} // namespace core::quad