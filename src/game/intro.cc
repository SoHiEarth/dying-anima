#include "game/intro.h"
#include "core/resource_manager.h"
#include "core/input.h"
#include "camera.h"
#include "rect.h"

void IntroScene::Init() {
	text_shader = ResourceManager::GetShader("Text").shader;
	rect_shader = ResourceManager::GetShader("Rect").shader;
         font = ResourceManager::GetFont("Special").font;
}

void IntroScene::Quit() {

}

bool update_text = false;
void IntroScene::HandleInput() {
  if (core::input::IsKeyPressedThisFrame(GLFW_KEY_SPACE)) {
    update_text = true;
	}
}

int text_index = 0;
void IntroScene::Update(float dt) {
	if (update_text) {
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
  font->Render(intro_text.at(text_index),
               {(window.width - font->GetWidth(intro_text.at(text_index))) / 2.0f, window.height / 2.0f},
               {0.5f, 1.0f},
               {1.0f, 1.0f, 1.0f}, text_shader);
  font->Render("Press Space To Continue",
               {window.width / 2.0f, window.height / 2.0f - 2*font->GetHeight(intro_text.at(text_index))},
               {0.5f, 0.5f},
               {1.0f, 1.0f, 1.0f}, text_shader);
}