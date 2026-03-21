#include <glad/glad.h>
// code block

#include <imgui.h>

#include <algorithm>

#include "core/camera.h"
#include "core/light.h"
#include "core/quad.h"
#include "core/render.h"
#include "core/resource_manager.h"
#include "core/shader.h"
#include "core/transform.h"
#include "core/window.h"
#include "sprite.h"
#include "util/calculate.h"

namespace {
std::vector<std::shared_ptr<Framebuffer>> framebuffers;
std::shared_ptr<Framebuffer> default_framebuffer, color_framebuffer,
    normal_framebuffer, bloom_framebuffer, bloom_framebuffer_2,
    combine_framebuffer;
std::shared_ptr<Shader> deferred_shader;
std::shared_ptr<Shader> fullscreen_shader;
std::shared_ptr<Shader> sprite_shader;
std::shared_ptr<Shader> bloom_separate_shader;
std::shared_ptr<Shader> bloom_blur_shader;
std::shared_ptr<Shader> combine_shader;
}  // namespace
float render::exposure = 1.0F;

Framebuffer::Framebuffer(int w, int h, unsigned int i)
    : width(w), height(h), id(i) {
  glGenFramebuffers(1, &id);
  glBindFramebuffer(GL_FRAMEBUFFER, id);
  glGenTextures(1, &colorbuffer);
  glBindTexture(GL_TEXTURE_2D, colorbuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA,
               GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         colorbuffer, 0);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error("Error: Framebuffer is not complete!\n");
  }
}

void render::Render(entt::registry& registry) {
  std::vector<entt::entity> sorted_entities;  // For sorting by z-index
  auto sprite_view = registry.view<Transform, Sprite>();
  for (auto entity : sprite_view) {
    sorted_entities.push_back(entity);
  }
  std::ranges::sort(sorted_entities,
                    [&registry](entt::entity a, entt::entity b) {
                      return registry.get<Transform>(a).z_index <
                             registry.get<Transform>(b).z_index;
                    });

  BindFramebuffer(color_framebuffer);
  glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
  glClear(GL_COLOR_BUFFER_BIT);
  for (auto entity : sorted_entities) {
    if (registry.get<Sprite>(entity).texture) {
      registry.get<Sprite>(entity).texture->Render(
          sprite_shader, CalculateModelMatrix(registry.get<Transform>(entity)));
    }
  }
  UnbindFramebuffer();

  BindFramebuffer(normal_framebuffer);
  glClear(GL_COLOR_BUFFER_BIT);
  for (auto entity : sorted_entities) {
    if (registry.get<Sprite>(entity).normal) {
      registry.get<Sprite>(entity).normal->Render(
          sprite_shader, CalculateModelMatrix(registry.get<Transform>(entity)));
    }
  }
  UnbindFramebuffer();

  BindFramebuffer(default_framebuffer);
  glClear(GL_COLOR_BUFFER_BIT);
  deferred_shader->Use();

  auto view = GetCamera().GetView();
  glm::mat4 projection = GetGameWindow().GetProjection();
  auto light_view = registry.view<Transform, Light>();
  int light_count = 0;
  for (auto entity : light_view) {
    auto [transform, light] = light_view.get<Transform, Light>(entity);
    glm::vec3 light_world_pos = glm::vec3(transform.position, 1.0F);
    glm::vec4 light_clip_pos =
        projection * view * glm::vec4(light_world_pos, 1.0F);
    glm::vec2 light_texcoord =
        ((glm::vec2(light_clip_pos) / light_clip_pos.w) + 1.0F) * 0.5F;
    deferred_shader->SetUniform(
        "lights[" + std::to_string(light_count) + "].position", light_texcoord);
    deferred_shader->SetUniform(
        "lights[" + std::to_string(light_count) + "].type",
        static_cast<int>(light.type));
    deferred_shader->SetUniform(
        "lights[" + std::to_string(light_count) + "].intensity",
        light.intensity);
    deferred_shader->SetUniform(
        "lights[" + std::to_string(light_count) + "].radial_falloff",
        light.radial_falloff);
    deferred_shader->SetUniform(
        "lights[" + std::to_string(light_count) + "].volumetric_intensity",
        light.volumetric_intensity);
    deferred_shader->SetUniform(
        "lights[" + std::to_string(light_count) + "].color", light.color);
    light_count++;
  }
  deferred_shader->SetUniform("light_count", light_count);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, color_framebuffer->colorbuffer);
  deferred_shader->SetUniform("color_texture", 0);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, normal_framebuffer->colorbuffer);
  deferred_shader->SetUniform("normal_texture", 1);
  float aspect_ratio = static_cast<float>(GetGameWindow().width) /
                       static_cast<float>(GetGameWindow().height);
  deferred_shader->SetUniform("aspect_ratio", aspect_ratio);
  core::quad::Render(core::quad::FULL_QUAD);

  // bloom stuff
  BindFramebuffer(bloom_framebuffer);
  glClear(GL_COLOR_BUFFER_BIT);
  bloom_separate_shader->Use();
  glActiveTexture(GL_TEXTURE0);
  bloom_separate_shader->SetUniform("source", 0);
  glBindTexture(GL_TEXTURE_2D, default_framebuffer->colorbuffer);
  core::quad::Render(core::quad::FULL_QUAD);

  bloom_blur_shader->Use();
  bool horizontal = true;
  for (int i = 0; i < 10; i++) {
    auto& source = horizontal ? bloom_framebuffer : bloom_framebuffer_2;
    auto& target = horizontal ? bloom_framebuffer_2 : bloom_framebuffer;
    BindFramebuffer(target);
    bloom_blur_shader->SetUniform("horizontal", static_cast<int>(horizontal));
    glActiveTexture(GL_TEXTURE0);
    bloom_blur_shader->SetUniform("source", 0);
    glBindTexture(GL_TEXTURE_2D, source->colorbuffer);
    core::quad::Render(core::quad::FULL_QUAD);
    horizontal = !horizontal;
  }

  BindFramebuffer(combine_framebuffer);
  combine_shader->Use();
  deferred_shader->SetUniform("exposure", exposure);
  glActiveTexture(GL_TEXTURE0);
  combine_shader->SetUniform("bloom_texture", 0);
  glBindTexture(GL_TEXTURE_2D, horizontal ? bloom_framebuffer->colorbuffer
                                          : bloom_framebuffer_2->colorbuffer);
  glActiveTexture(GL_TEXTURE1);
  combine_shader->SetUniform("scene_texture", 1);
  glBindTexture(GL_TEXTURE_2D, default_framebuffer->colorbuffer);
  core::quad::Render(core::quad::FULL_QUAD);

  UnbindFramebuffer();
  fullscreen_shader->Use();
  fullscreen_shader->SetUniform("screen", 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, combine_framebuffer->colorbuffer);
  core::quad::Render(core::quad::FULL_QUAD);
}

void render::RecreateFramebuffers(int width, int height) {
  for (auto& framebuffer : framebuffers) {
    if (framebuffer == nullptr) continue;
    glBindTexture(GL_TEXTURE_2D, framebuffer->colorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA,
                 GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
}

std::shared_ptr<Framebuffer> render::CreateFramebuffer(int width, int height) {
  framebuffers.push_back(std::make_shared<Framebuffer>(width, height, 0));
  return framebuffers.back();
}

void render::BindFramebuffer(const std::shared_ptr<Framebuffer>& framebuffer) {
  if (framebuffer) {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);
  }
}

void render::UnbindFramebuffer() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void render::DeleteFramebuffer(std::shared_ptr<Framebuffer> framebuffer) {
  if (framebuffer) {
    glDeleteTextures(1, &framebuffer->colorbuffer);
    glDeleteFramebuffers(1, &framebuffer->id);
    auto it = std::ranges::find(framebuffers, framebuffer);
    if (it != framebuffers.end()) {
      framebuffers.erase(it);
    }
  }
  framebuffer = nullptr;
}

void render::Init() {
  auto width = GetGameWindow().width;
  auto height = GetGameWindow().height;
  default_framebuffer = CreateFramebuffer(width, height);
  color_framebuffer = CreateFramebuffer(width, height);
  normal_framebuffer = CreateFramebuffer(width, height);
  bloom_framebuffer = CreateFramebuffer(width, height);
  bloom_framebuffer_2 = CreateFramebuffer(width, height);
  combine_framebuffer = CreateFramebuffer(width, height);
  deferred_shader = ResourceManager::GetShader("Deferred").shader;
  fullscreen_shader = ResourceManager::GetShader("Fullscreen").shader;
  sprite_shader = ResourceManager::GetShader("Sprite").shader;
  bloom_separate_shader = ResourceManager::GetShader("bloom.separate").shader;
  bloom_blur_shader = ResourceManager::GetShader("bloom.blur").shader;
  combine_shader = ResourceManager::GetShader("bloom.combine").shader;
  render::UnbindFramebuffer();
}

void render::Quit() {
  for (const auto& framebuffer : framebuffers) {
    DeleteFramebuffer(framebuffer);
  }
}
