#include <glad/glad.h>
#include "menu.h"
#include <GLFW/glfw3.h>
#include <print>
#include <fstream>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>
#include "shader.h"
#include "font.h"
#include "texture.h"
#include "calculate.h"
#include "atlas.h"
#include "window.h"
#include "menu/menu_input.h"
#include "core/quad.h"

AppState Menu(GameWindow window) {
  auto font_atlas = LoadFontAtlas("assets/fonts/font.xml");
  auto shader_atlas = LoadShaderAtlas("assets/shaders/shader.xml");
  auto texture_atlas = LoadTextureAtlas("assets/textures/texture.xml");
  Font font(font_atlas.at("Special").file, 56);
  Shader text_shader(shader_atlas.at("Text").vertex_file, shader_atlas.at("Text").fragment_file);
  text_shader.Use();
  text_shader.SetUniform("character", 0);
  Shader shader(shader_atlas.at("Sprite").vertex_file, shader_atlas.at("Sprite").fragment_file);
  shader.Use();
  shader.SetUniform("texture1", 0);
  Texture banner_texture(texture_atlas.at("banner").path);

  AppState state = AppState::MENU;
  while (!glfwWindowShouldClose(window.window) && state == AppState::MENU) {
    glfwPollEvents();
    menu::input::Update(state, window.window);
    while (window.IsMinimized()) {
      // GLFW will update the window size
      glfwPollEvents();
    }

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(window.width), 0.0f, static_cast<float>(window.height));

    glClear(GL_COLOR_BUFFER_BIT);
    banner_texture.Render(
      shader,
      projection,
      glm::mat4(1.0f),
      CalculateModelMatrix(
        glm::vec2(window.width / 2.0f, window.height / 2.0f),
        0.0f,
        glm::vec2(window.width, window.height)
      )
    );
    core::quad::Render(core::quad::QuadType::WITH_TEXCOORDS);

    const float menu_y = (float)window.height / 4.0f;
    constexpr int padding = 50;

    #ifdef NDEBUG
    int text_width = font.GetWidth("Play", 1.0f) + padding + font.GetWidth("Quit", 1.0f);
    #else
    int text_width = font.GetWidth("Play", 1.0f) + padding + font.GetWidth("Quit", 1.0f) + padding + font.GetWidth("Level Editor", 1.0f);
    #endif
    int x_pos = (window.width - text_width) / 2;

    font.Render("Play", glm::vec2(x_pos, menu_y), 1.0f, (menu::input::focus_x == 0) ? glm::vec3(1.0f, 1.0f, 0.0f) : glm::vec3(1.0f),
      text_shader, projection);
    x_pos += font.GetWidth("Play", 1.0f) + padding;
    font.Render("Quit", glm::vec2(x_pos, menu_y), 1.0f, (menu::input::focus_x == 1) ? glm::vec3(1.0f, 1.0f, 0.0f) : glm::vec3(1.0f),
      text_shader, projection);
    #ifndef NDEBUG
    x_pos += font.GetWidth("Quit", 1.0f) + padding;
    font.Render("Level Editor", glm::vec2(x_pos, menu_y), 1.0f, (menu::input::focus_x == 2) ? glm::vec3(1.0f, 1.0f, 0.0f) : glm::vec3(1.0f),
      text_shader, projection);
    #endif
    glfwSwapBuffers(window.window);
  }

  if (state == AppState::MENU) {
    state = AppState::EXIT;
  }
  return state;
}