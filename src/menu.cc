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
#include "menu/menu_input.h"
#include "shader.h"
#include "texture.h"
#include "window.h"

AppState Menu(GameWindow &window) {
  auto font = ResourceManager::GetFont("Special").font;
  auto text_shader = ResourceManager::GetShader("Text").shader,
       sprite_shader = ResourceManager::GetShader("Sprite").shader;
  text_shader->Use();
  text_shader->SetUniform("character", 0);
  sprite_shader->Use();
  sprite_shader->SetUniform("texture1", 0);
  auto banner_texture = ResourceManager::GetTexture("util.banner").texture,
       selected_texture = ResourceManager::GetTexture("menu.selected").texture;

  AppState state = AppState::MENU;
  GetCamera().SetType(CameraType::UI);
  while (!glfwWindowShouldClose(window.window) && state == AppState::MENU) {
    glfwPollEvents();
    menu::input::Update(state, window.window);
    while (window.IsMinimized()) {
      glfwPollEvents();
    }

    glClear(GL_COLOR_BUFFER_BIT);
    window.SetProjection(ProjectionType::SCREEN_SPACE);
    banner_texture->Render(
        sprite_shader, CalculateModelMatrix(glm::vec2{window.width, window.height}*0.5f, 0.0f, glm::vec2(window.width, window.height)));

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


    if (menu::input::focus_index == static_cast<int>(AppState::PLAYING)) {
      float width = font->GetWidth("Play") + selected_margin;
      float height = font->GetHeight("Play") + selected_margin;
      glm::vec2 center_pos = {x_pos + width / 2.0f - selected_margin / 2.0f,
                              menu_y + height / 2.0f - selected_margin / 2.0f};
      selected_texture->Render(
          sprite_shader,
          CalculateModelMatrix(center_pos, 0.0f, {width, height}));
      core::quad::Render(core::quad::QuadType::WITH_TEXCOORDS);
    }
    font->Render("Play", glm::vec2(x_pos, menu_y), glm::vec3(1.0f),
                 text_shader);
    x_pos += font->GetWidth("Play") + padding;

    if (menu::input::focus_index == static_cast<int>(AppState::EXIT)) {
      float width = font->GetWidth("Quit") + selected_margin;
      float height = font->GetHeight("Quit") + selected_margin;
      glm::vec2 center_pos = {x_pos + width / 2.0f - selected_margin / 2.0f,
                              menu_y + height / 2.0f - selected_margin / 2.0f};
      selected_texture->Render(
          sprite_shader,
          CalculateModelMatrix(center_pos, 0.0f, {width, height}));
      core::quad::Render(core::quad::QuadType::WITH_TEXCOORDS);
    }
    font->Render("Quit", glm::vec2(x_pos, menu_y), glm::vec3(1.0f),
                 text_shader);

#ifndef NDEBUG
    x_pos += font->GetWidth("Quit") + padding;
    if (menu::input::focus_index == static_cast<int>(AppState::LEVEL_EDITOR)) {
      float width = font->GetWidth("Level Editor") + selected_margin;
      float height = font->GetHeight("Level Editor") + selected_margin;
      glm::vec2 center_pos = {x_pos + width / 2.0f - selected_margin / 2.0f,
                              menu_y + height / 2.0f - selected_margin / 2.0f};
      selected_texture->Render(
          sprite_shader,
          CalculateModelMatrix(center_pos, 0.0f, {width, height}));
      core::quad::Render(core::quad::QuadType::WITH_TEXCOORDS);
    }
    font->Render("Level Editor", glm::vec2(x_pos, menu_y), glm::vec3(1.0f),
                 text_shader);
#endif
    glfwSwapBuffers(window.window);
  }

  if (state == AppState::MENU) {
    state = AppState::EXIT;
  }
  return state;
}
