#include "save_select.h"
#include "core/resource_manager.h"
#include "core/input.h"
#include "core/camera.h"
#include "game/game.h"
#include "game/intro.h"
#include "menu.h"
#include <algorithm>
#include <chrono>

constexpr int kLabelHeight = 24;
namespace {
  int focus_index = 0;
}

void SaveSelect::Init() {
  font = resource_manager::GetFont("menu.ui").font;
  rect_shader = resource_manager::GetShader("Rect").shader;
  text_shader = resource_manager::GetShader("Text").shader;
  text_shader->Use();
  text_shader->SetUniform("character", 0);

  std::vector<std::filesystem::path> saves;
  if (std::filesystem::exists("saves")) {
    for (const auto& save_iterator : std::filesystem::directory_iterator("saves")) {
      if (save_iterator.is_regular_file()) {
        auto save = std::filesystem::path(save_iterator);
        if (save.extension() == ".save") {
          saves.push_back(save);
        }
      }
    }
  }

  std::ranges::sort(saves, [](const auto& a, const auto& b) {
    return std::stoll(a.stem().string()) > std::stoll(b.stem().string());
  });

  int i = 0;
  for (const auto& save : saves) {
    if (i > 4) {
      break;
    }
    auto ns = std::chrono::nanoseconds(std::stoll(save.stem().string()));
          auto sec = std::chrono::time_point<std::chrono::system_clock>(duration_cast<std::chrono::seconds>(ns));
          std::time_t tt = std::chrono::system_clock::to_time_t(sec);
          auto tm = *std::localtime(&tt);
          std::ostringstream oss;
          oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
          layout.AddElement(std::make_unique<ui::Button>(
                "Save: " + oss.str(),
                font,
                [this](){scene_manager_.PopScene(); scene_manager_.PushScene(std::make_unique<GameScene>(scene_manager_));}
          ))->size.y = kLabelHeight;
          i++;
        }
  if (saves.empty()) {
    layout.AddElement(std::make_unique<ui::Label>("No existing saves.", font))->size.y = kLabelHeight;
  }
  layout.AddElement(std::make_unique<ui::Button>(
        "[ New Game ]",
        font,
        [this](){
          scene_manager_.PopScene();
          scene_manager_.PushScene(std::make_unique<GameScene>(scene_manager_));
          scene_manager_.PushScene(std::make_unique<IntroScene>(scene_manager_));}
  ))->size.y = kLabelHeight;

  layout.AddElement(std::make_unique<ui::Label>(
        "Select Save",
        font
        ))->size.y = kLabelHeight * 2;
  layout.SetSpacing(kLabelHeight);
}

void SaveSelect::Update(double dt) {
  if (core::input::IsKeyPressedThisFrame(GLFW_KEY_ESCAPE)) {
    scene_manager_.PopScene();
    scene_manager_.PushScene(std::make_unique<MenuScene>(scene_manager_));
  }
  
  glm::dvec2 mouse_position{};
  glfwGetCursorPos(GetGameWindow().window, &mouse_position.x, &mouse_position.y);
  layout.SetPosition({30, (GetGameWindow().height - layout.GetLayoutSize()) / 2});
  layout.Update(mouse_position, core::input::IsKeyPressed(GLFW_MOUSE_BUTTON_LEFT));
}

void SaveSelect::Render(GameWindow& window) {
  window.SetProjection(ProjectionType::kScreenSpace);
  GetCamera().SetType(CameraType::kUi);
  layout.Render(text_shader, rect_shader);
}

void SaveSelect::Quit() {
}

