#ifndef CORE_SCENE_H_
#define CORE_SCENE_H_

#include <memory>
#include <stack>
#include "../window.h"
struct Scene {
  virtual ~Scene() = default;
  virtual void OnEnter() {}
  virtual void OnExit() {}
  virtual void HandleInput() {}
  virtual void Update(float dt) {}
  virtual void Render() {}

  bool is_transparent = false;
};

struct SceneManager {
 public:
   void PushScene(std::unique_ptr<Scene> scene) {
    scenes.push_back(std::move(scene));
    scenes.back()->OnEnter();
  }
  void PopScene() {
    if (scenes.empty())
      return;
    scenes.back()->OnExit();
    scenes.pop_back();
    if (!scenes.empty())
      scenes.back()->OnEnter();
  }
  void HandleInput() {
    if (!scenes.empty())
      scenes.back()->HandleInput();
  }
  void Update(float dt) {
    if (!scenes.empty())
      scenes.back()->Update(dt);
  }
  void Render() {
    std::stack<std::unique_ptr<Scene>> render_stack;
    // Go from top to bottom until we find a non-transparent scene, pushing
    // scenes onto the render stack as we go
    for (auto it = scenes.rbegin(); it != scenes.rend(); ++it) {
      render_stack.push(std::move(*it));
      if (!(*it)->is_transparent)
        break;
    }
  }
 private:
  std::vector<std::unique_ptr<Scene>> scenes;
};

#endif  // CORE_SCENE_H_