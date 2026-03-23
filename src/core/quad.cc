#include "core/quad.h"

#include <glad/glad.h>

namespace core::quad::internal {
unsigned int full_quad_vertex_attrib, full_quad_vertex_buffer,
    full_quad_index_buffer;
unsigned int quad_vertex_attrib, quad_vertex_buffer, quad_index_buffer;
unsigned int tex_vertex_attrib, tex_vertex_buffer, tex_index_buffer;
}  // namespace core::quad::internal

const float kTexVertices[] = {0.5F, 0.5F,  0.0F, 1.0F,  1.0F,  0.5F, -0.5F,
                              0.0F, 1.0F,  0.0F, -0.5F, -0.5F, 0.0F, 0.0F,
                              0.0F, -0.5F, 0.5F, 0.0F,  0.0F,  1.0F};

const float kQuadVertices[] = {0.5F,  0.5F,  0.0F, 0.5F,  -0.5F, 0.0F,
                               -0.5F, -0.5F, 0.0F, -0.5F, 0.5F,  0.0F};

const float kFullVertices[] = {1.0F, 1.0F,  0.0F, 1.0F,  1.0F,  1.0F, -1.0F,
                               0.0F, 1.0F,  0.0F, -1.0F, -1.0F, 0.0F, 0.0F,
                               0.0F, -1.0F, 1.0F, 0.0F,  0.0F,  1.0F};

const unsigned int kIndices[] = {0, 1, 3, 1, 2, 3};

void core::quad::Init() {
  glGenVertexArrays(1, &internal::tex_vertex_attrib);
  glGenBuffers(1, &internal::tex_vertex_buffer);
  glGenBuffers(1, &internal::tex_index_buffer);
  glBindVertexArray(internal::tex_vertex_attrib);
  glBindBuffer(GL_ARRAY_BUFFER, internal::tex_vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(kTexVertices), kTexVertices,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, internal::tex_index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kIndices), kIndices,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void*)nullptr);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glGenVertexArrays(1, &internal::quad_vertex_attrib);
  glGenBuffers(1, &internal::quad_vertex_buffer);
  glGenBuffers(1, &internal::quad_index_buffer);
  glBindVertexArray(internal::quad_vertex_attrib);
  glBindBuffer(GL_ARRAY_BUFFER, internal::quad_vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(kQuadVertices), kQuadVertices,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, internal::quad_index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kIndices), kIndices,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                        (void*)nullptr);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glGenVertexArrays(1, &internal::full_quad_vertex_attrib);
  glGenBuffers(1, &internal::full_quad_vertex_buffer);
  glGenBuffers(1, &internal::full_quad_index_buffer);
  glBindVertexArray(internal::full_quad_vertex_attrib);
  glBindBuffer(GL_ARRAY_BUFFER, internal::full_quad_vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(kFullVertices), kFullVertices,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, internal::full_quad_index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kIndices), kIndices,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void*)nullptr);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void core::quad::Render(QuadType type) {
  switch (type) {
    case QuadType::kQuadOnly:
      glBindVertexArray(internal::quad_vertex_attrib);
      break;
    case QuadType::kWithTexcoords:
      glBindVertexArray(internal::tex_vertex_attrib);
      break;
    case QuadType::kFullQuad:
      glBindVertexArray(internal::full_quad_vertex_attrib);
      break;
  }
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void core::quad::Quit() {
  glDeleteBuffers(1, &internal::full_quad_vertex_buffer);
  glDeleteBuffers(1, &internal::full_quad_index_buffer);
  glDeleteBuffers(1, &internal::quad_vertex_buffer);
  glDeleteBuffers(1, &internal::quad_index_buffer);
  glDeleteBuffers(1, &internal::tex_vertex_buffer);
  glDeleteBuffers(1, &internal::tex_index_buffer);
  glDeleteVertexArrays(1, &internal::full_quad_vertex_attrib);
  glDeleteVertexArrays(1, &internal::quad_vertex_attrib);
  glDeleteVertexArrays(1, &internal::tex_vertex_attrib);
}