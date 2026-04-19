#include "save_select.h"
#include "core/resource_manager.h"
#include "core/input.h"
#include "core/camera.h"
#include "game/game.h"
#include "game/intro.h"
#include "menu.h"
#include <algorithm>

constexpr int kLabelHeight = 24;

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

  std::ranges::sort(saves,
    [](const auto& a, const auto& b) {
      return std::filesystem::last_write_time(a) > std::filesystem::last_write_time(b);
  });

  
  int i = 0;
  for (const auto& save : saves) {
    layout
          .AddElement(std::make_unique<ui::Button>(
            (i == 0 ? std::string("* ") : std::string("")) + save.stem().string(), font,
              [this, save]() {
                scene_manager_.PopScene();
                scene_manager_.PushScene(
                    std::make_unique<GameScene>(scene_manager_, save));
              }))
          ->size.y = kLabelHeight;
        if (i++ > 4) break;  
  }
  if (saves.empty()) {
    layout.AddElement(std::make_unique<ui::Label>("No existing saves.", font))->size.y = kLabelHeight;
  }
  layout.AddElement(std::make_unique<ui::Button>(
        "[ New Game ]",
        font,
        [this](){
          scene_manager_.PopScene();
          scene_manager_.PushScene(std::make_unique<GameScene>(scene_manager_, ""));
          scene_manager_.PushScene(std::make_unique<IntroScene>(scene_manager_));}
  ))->size.y = kLabelHeight;

  layout.AddElement(std::make_unique<ui::Label>(
        "Select Save",
        font
        ))->size.y = kLabelHeight * 2;
  layout.SetSpacing(kLabelHeight);
}

void SaveSelect::Update(double) {
  if (core::input::IsKeyPressedThisFrame(GLFW_KEY_ESCAPE)) {
    scene_manager_.PopScene();
    scene_manager_.PushScene(std::make_unique<MenuScene>(scene_manager_));
  }
  
  glm::dvec2 mouse_pos{};
  glfwGetCursorPos(GetGameWindow().window, &mouse_pos.x, &mouse_pos.y);

  layout.SetPosition({30, (GetGameWindow().height - layout.GetLayoutSize()) / 2});
  layout.Update(mouse_pos, core::input::IsKeyPressed(GLFW_MOUSE_BUTTON_LEFT));
}

void SaveSelect::Render(GameWindow& window) {
  window.SetProjection(ProjectionType::kScreenSpace);
  GetCamera().SetType(CameraType::kUi);
  layout.Render(text_shader, rect_shader);
}

void SaveSelect::Quit() {
}

