#ifndef CORE_SCENE_H_
#define CORE_SCENE_H_

#include <memory>
#include <stack>
#include "../window.h"

struct SceneManager;

struct Scene {
 public:
  // Optional Field
  virtual std::string Name() { return "BaseScene"; };
  explicit Scene(SceneManager& manager) : scene_manager(manager) {}
  virtual ~Scene() = default;
  virtual void Init() {}
  virtual void Quit() {}
  virtual void HandleInput() {}
  virtual void Update(float dt) {}
  virtual void Render(GameWindow& window) {}
  virtual bool IsTransparent() { return false; };

 protected:
   SceneManager& scene_manager;
};

struct SceneManager {
 public:
   void PushScene(std::unique_ptr<Scene> scene) {
    if (scene == nullptr) return;
    scenes.push_back(std::move(scene));
    scenes.back()->Init();
  }
  void PopScene() {
    if (scenes.empty()) return;
    scenes.back()->Quit();
    scenes.pop_back();
    if (scenes.empty()) return;
    scenes.back()->Init();
  }
  void HandleInput() {
    if (scenes.empty()) return;
    scenes.back()->HandleInput();
  }
  void Update(float dt) {
    if (scenes.empty()) return;
    scenes.back()->Update(dt);
  }
  void Render(GameWindow& window) {
    if (scenes.empty()) return;
    for (auto it = scenes.rbegin(); it != scenes.rend(); ++it) {
      if (!(*it)->IsTransparent()) {
        (*it)->Render(window);
        return;
      } else {
        (*it)->Render(window);
      }
    }
  }
 private:
  std::vector<std::unique_ptr<Scene>> scenes;
};

#endif