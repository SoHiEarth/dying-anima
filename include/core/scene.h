#ifndef CORE_SCENE_H_
#define CORE_SCENE_H_

#include <memory>
#include <stack>
#include "core/window.h"

struct SceneManager;

struct Scene {
 public:
  // Optional Field
  virtual std::string Name() { return "BaseScene"; };
  explicit Scene(SceneManager& manager) : scene_manager(manager) {}
  virtual ~Scene() = default;
  virtual void Init() {}
  virtual void Quit() {}
  virtual void Update(double dt) {}
  virtual void Render(GameWindow& window) {}
  virtual bool IsTransparent() { return false; };

 protected:
   SceneManager& scene_manager;
};

enum class SceneChange {
  NONE,
  PUSH,
  POP
};

struct SceneManager {
 public:
  void PushScene(std::unique_ptr<Scene> scene) {
    if (scene == nullptr) return;
    pending_changes.push_back({SceneChange::PUSH, std::move(scene)});
  }
  void PopScene() {
    pending_changes.push_back({SceneChange::POP, nullptr});
  }
  bool NoScenes() const {
    return scenes.empty();
  }
  void Update(double dt) {
    if (scenes.empty()) return;
    scenes.back()->Update(dt);
  } 
  void Render(GameWindow& window) {
    if (scenes.empty()) return;
    size_t start = 0;
    for (auto i = scenes.size() - 1; i >= 0; i--) {
      if (!scenes[i]->IsTransparent()) {
        start = i;
        break;
      }
    }
    for (auto i = start; i < scenes.size(); i++) {
      scenes[i]->Render(window);
    }
  }
  void ProcessSceneChanges() {
    for (auto& [type, scene] : pending_changes) {
      switch (type) {
        case SceneChange::PUSH:
          scenes.push_back(std::move(scene));
          scenes.back()->Init();
          break;
        case SceneChange::POP:
          if (scenes.empty()) return;
          scenes.back()->Quit();
          scenes.pop_back();
          break;
        default:
          break;
      }
    }
    pending_changes.clear();
  }
 private:
  std::vector<std::unique_ptr<Scene>> scenes;
  std::vector<std::pair<SceneChange, std::unique_ptr<Scene>>> pending_changes;
};

#endif