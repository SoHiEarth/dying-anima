#include "game/intro.h"

#include "core/camera.h"
#include "core/input.h"
#include "core/rect.h"
#include "core/resource_manager.h"
#include "game/game.h"
#include "game/progression.h"

std::string current_text = "";
int character_index = 0;
int text_index = 0;

void IntroScene::Init() {
  text_shader = ResourceManager::GetShader("Text").shader;
  rect_shader = ResourceManager::GetShader("Rect").shader;
  font = ResourceManager::GetFont("Special").font;
}

void IntroScene::Quit() {
  game::save_data.completion_markers.push_back(
      Progression::INTRO_COMPLETE_MARKER);
}

bool update_text = false;

void IntroScene::Update(double dt) {
  static double timer = 0.0f;
  static double time_since_sentence_complete = 0.0f;
  timer += dt;
  if (current_text.size() == intro_text[text_index].size()) {
    time_since_sentence_complete += dt;
    if (time_since_sentence_complete >= 1.5f) {
      update_text = true;
      time_since_sentence_complete = 0.0f;
      current_text = "";
      character_index = 0;
    }
  }
  // 0.5 seconds
  if (timer >= 0.075f) {
    character_index++;
    timer = 0.0f;
    if (character_index > intro_text[text_index].size()) {
      character_index = intro_text[text_index].size();
    }
    current_text = intro_text[text_index].substr(0, character_index);
  }

  if (core::input::IsKeyPressedThisFrame(GLFW_KEY_SPACE) || update_text) {
    text_index++;
    update_text = false;
    if (text_index >= intro_text.size()) {
      scene_manager.PopScene();
    }
  }
}

void IntroScene::Render(GameWindow& window) {
  window.SetProjection(ProjectionType::SCREEN_SPACE);
  GetCamera().SetType(CameraType::UI);
  Rect rect;
  rect.position = glm::vec2(window.width / 2.0f, window.height / 2.0f);
  rect.scale = {window.width, window.height};
  rect.color = glm::vec4(0.0f, 0.0f, 0.0f, 0.8f);
  rect.Render(rect_shader);
  font->Render(current_text, {window.width / 2.0f, window.height / 2.0f},
               glm::vec2(0.75f), glm::vec3(1.0f), text_shader);
  font->Render(
      "Press Space To Continue",
      {window.width / 2.0f, font->GetHeight("Press Space To Continue")},
      glm::vec2(0.5f), glm::vec3(0.5f), text_shader);
}