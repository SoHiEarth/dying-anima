#ifndef RENDER_H
#define RENDER_H

#include <entt/entt.hpp>

struct Framebuffer {
  Framebuffer(int w, int h, unsigned int i);
	int width = 0, height = 0;
  unsigned int id = 0, colorbuffer = 0;
};

namespace render {
void Init();
void Quit();
void AddLight(entt::entity entity);
void Render(entt::registry& registry);
void RecreateFramebuffers(int width, int height);
Framebuffer* CreateFramebuffer(int width, int height);
void BindFramebuffer(Framebuffer* framebuffer);
void UnbindFramebuffer();
void DeleteFramebuffer(Framebuffer*& framebuffer);
void Clear();
}
#endif  // RENDER_H