#ifndef RENDER_H
#define RENDER_H

#include <entt/entt.hpp>

struct Framebuffer {
  Framebuffer(int w, int h, unsigned int i);
	int width = 0, height = 0;
  unsigned int id = 0;
  unsigned int colorbuffer;
};

namespace render {
void Init();
void Quit();
void Render(entt::registry& registry);
void RecreateFramebuffers(int width, int height);
std::shared_ptr<Framebuffer> CreateFramebuffer(int width, int height);
void BindFramebuffer(std::shared_ptr<Framebuffer> framebuffer);
void UnbindFramebuffer();
void DeleteFramebuffer(std::shared_ptr<Framebuffer> framebuffer);
extern float exposure;
}
#endif  // RENDER_H