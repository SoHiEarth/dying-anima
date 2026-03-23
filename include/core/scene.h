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
  explicit Scene(SceneManager& manager) : scene_manager_(manager) {}
  virtual ~Scene() = default;
  virtual void Init() {}
  virtual void Quit() {}
  virtual void Update(double /* dt */) {}
  virtual void Render(GameWindow& /* window */) {}
  virtual bool IsTransparent() { return false; };

 protected:
  SceneManager& scene_manager_;
};

enum class SceneChange { kNone, kPush, kPop };

struct SceneManager {
 public:
  void PushScene(std::unique_ptr<Scene> scene) {
    if (scene == nullptr) return;
    pending_changes_.emplace_back(SceneChange::kPush, std::move(scene));
  }
  void PopScene() { pending_changes_.emplace_back(SceneChange::kPop, nullptr); }
  bool NoScenes() const { return scenes_.empty(); }
  void Update(double dt) {
    if (scenes_.empty()) return;
    scenes_.back()->Update(dt);
  }
  void Render(GameWindow& window) {
    if (scenes_.empty()) return;
    size_t start = 0;
    for (auto i = static_cast<int>(scenes_.size()) - 1; i >= 0; i--) {
      if (!scenes_[i]->IsTransparent()) {
        start = i;
        break;
      }
    }
    for (auto i = start; i < scenes_.size(); i++) {
      scenes_[i]->Render(window);
    }
  }
  void ProcessSceneChanges() {
    for (auto& [type, scene] : pending_changes_) {
      switch (type) {
        case SceneChange::kPush:
          scenes_.push_back(std::move(scene));
          scenes_.back()->Init();
          break;
        case SceneChange::kPop:
          if (scenes_.empty()) return;
          scenes_.back()->Quit();
          scenes_.pop_back();
          break;
        default:
          break;
      }
    }
    pending_changes_.clear();
  }

 private:
  std::vector<std::unique_ptr<Scene>> scenes_;
  std::vector<std::pair<SceneChange, std::unique_ptr<Scene>>> pending_changes_;
};

#endif
