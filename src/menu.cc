#include <glad/glad.h>
// Code block

#include "menu.h"

#include <GLFW/glfw3.h>
#include <ft2build.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include FT_FREETYPE_H

#include "atlas.h"
#include "calculate.h"
#include "camera.h"
#include "core/quad.h"
#include "core/resource_manager.h"
#include "font.h"
#include "core/input.h"
#include "shader.h"
#include "texture.h"
#include "window.h"
#include "state.h"
#include "game.h"
#include "level_editor.h"
#include "game/intro.h"

void MenuScene::Init() {
  font = ResourceManager::GetFont("Special").font;
  text_shader = ResourceManager::GetShader("Text").shader;
  sprite_shader = ResourceManager::GetShader("Sprite").shader;
  text_shader->Use();
  text_shader->SetUniform("character", 0);
  sprite_shader->Use();
  sprite_shader->SetUniform("texture1", 0);
  banner_texture = ResourceManager::GetTexture("util.banner").texture;
  selected_texture = ResourceManager::GetTexture("menu.selected").texture;
}

int focus_index = 0;

void MenuScene::HandleInput() {
  if (core::input::IsKeyPressed(GLFW_KEY_ESCAPE)) {
    scene_manager.PopScene();
  }
  if (core::input::IsKeyPressed(GLFW_KEY_ENTER)) {
    switch (static_cast<AppState>(focus_index)) {
      case AppState::PLAYING:
        scene_manager.PushScene(std::make_unique<GameScene>(scene_manager));
        scene_manager.PushScene(std::make_unique<IntroScene>(scene_manager));
        break;
      case AppState::EXIT:
        glfwSetWindowShouldClose(GetGameWindow().window, true);
        break;
      case AppState::LEVEL_EDITOR:
        scene_manager.PushScene(std::make_unique<LevelEditor>(scene_manager));
        break;
    }
  }
  if (core::input::IsKeyPressedThisFrame(GLFW_KEY_LEFT)) {
    focus_index = std::max(focus_index - 1, 0);
  }
  if (core::input::IsKeyPressedThisFrame(GLFW_KEY_RIGHT)) {
#ifdef NDEBUG
    const int max_focus = 1;
#else
    const int max_focus = 2;
#endif
    focus_index = std::min(focus_index + 1, max_focus);
  }
}

void MenuScene::Update(float dt) {}

void MenuScene::Render(GameWindow& window) {
  window.SetProjection(ProjectionType::SCREEN_SPACE);
  GetCamera().SetType(CameraType::UI);
  banner_texture->Render(
      sprite_shader,
      CalculateModelMatrix(glm::vec2{window.width, window.height} * 0.5f, 0.0f,
                           glm::vec2(window.width, window.height)));

  const int menu_y = window.height / 4;
  constexpr int padding = 50;
  constexpr int selected_margin = 30;

#ifdef NDEBUG
  int text_width =
      font->GetWidth("Play", 1.0f) + padding + font->GetWidth("Quit", 1.0f);
#else
  int text_width = font->GetWidth("Play") + padding + font->GetWidth("Quit") +
                   padding + font->GetWidth("Level Editor");
#endif
  int x_pos = (window.width - text_width) / 2;

  if (focus_index == static_cast<int>(AppState::PLAYING)) {
    int width = font->GetWidth("Play") + selected_margin;
    int height = font->GetHeight("Play") + selected_margin;
    glm::ivec2 center_pos = {x_pos + width / 2 - selected_margin / 2,
                             menu_y + height / 2 - selected_margin / 2};
    selected_texture->Render(
        sprite_shader, CalculateModelMatrix(center_pos, 0.0f, {width, height}));
    core::quad::Render(core::quad::QuadType::WITH_TEXCOORDS);
  }
  font->Render("Play", glm::vec2(x_pos, menu_y), glm::vec3(1.0f), text_shader);
  x_pos += font->GetWidth("Play") + padding;

  if (focus_index == static_cast<int>(AppState::EXIT)) {
    int width = font->GetWidth("Quit") + selected_margin;
    int height = font->GetHeight("Quit") + selected_margin;
    glm::vec2 center_pos = {x_pos + width / 2.0f - selected_margin / 2.0f,
                            menu_y + height / 2.0f - selected_margin / 2.0f};
    selected_texture->Render(
        sprite_shader, CalculateModelMatrix(center_pos, 0.0f, {width, height}));
    core::quad::Render(core::quad::QuadType::WITH_TEXCOORDS);
  }
  font->Render("Quit", glm::vec2(x_pos, menu_y), glm::vec3(1.0f), text_shader);

#ifndef NDEBUG
  x_pos += font->GetWidth("Quit") + padding;
  if (focus_index == static_cast<int>(AppState::LEVEL_EDITOR)) {
    int width = font->GetWidth("Level Editor") + selected_margin;
    int height = font->GetHeight("Level Editor") + selected_margin;
    glm::vec2 center_pos = {x_pos + width / 2.0f - selected_margin / 2.0f,
                            menu_y + height / 2.0f - selected_margin / 2.0f};
    selected_texture->Render(
        sprite_shader, CalculateModelMatrix(center_pos, 0.0f, {width, height}));
    core::quad::Render(core::quad::QuadType::WITH_TEXCOORDS);
  }
  font->Render("Level Editor", glm::vec2(x_pos, menu_y), glm::vec3(1.0f),
               text_shader);
#endif
}

void MenuScene::Quit() {}