#include <glad/glad.h>
#include "level_editor.h"
#include "rect.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"
#include <vector>
#include <print>
#include <fstream>
#include "font.h"
#include "object.h"
#include "atlas.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "level_utils.h"

int GRID_SIZE = 5;
static int selected_object_index = -1;
static std::vector<Object> editor_objects;
glm::vec3 editor_camera_position = glm::vec3(0.0f, 0.0f, 1.0f);
enum class GizmoAxis {
  NONE,
  X,
  Y
};
static GizmoAxis active_axis = GizmoAxis::NONE;
static bool dragging = false;
glm::dvec2 mouse_position, last_mouse_position;

void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
  GRID_SIZE = std::max(5, GRID_SIZE + static_cast<int>(yoffset) * 5);
}

void HandleLevelEditorInput(GLFWwindow *window, AppState &app_state) {
  glm::ivec2 screen_dimensions;
  glfwGetFramebufferSize(window, &screen_dimensions.x, &screen_dimensions.y);
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    app_state = AppState::PLAYING;
  }
  glfwGetCursorPos(window, &mouse_position.x, &mouse_position.y);
  bool already_exists = false;
  glm::ivec2 mouse_world_position = glm::ivec2(
      static_cast<int>(mouse_position.x + editor_camera_position.x),
      static_cast<int>(screen_dimensions.y - mouse_position.y + editor_camera_position.y));
  glm::ivec2 grid_position = glm::ivec2(
      mouse_world_position.x / GRID_SIZE,
      mouse_world_position.y / GRID_SIZE);
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
    editor_camera_position += glm::vec3(
        static_cast<float>(last_mouse_position.x - mouse_position.x),
        -static_cast<float>(last_mouse_position.y - mouse_position.y),
        0.0f);
  }
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
    selected_object_index = -1;
    for (int i = 0; i < editor_objects.size(); ++i) {
      auto &obj = editor_objects[i];
      if (mouse_world_position.x >= obj.position.x &&
          mouse_world_position.x <= obj.position.x + obj.scale.x &&
          mouse_world_position.y >= obj.position.y &&
          mouse_world_position.y <= obj.position.y + obj.scale.y) {
        selected_object_index = i;
        break;
      }
    }
    if (selected_object_index == -1){
      Object object{};
      object.position = glm::vec3(
          grid_position.x * GRID_SIZE,
          grid_position.y * GRID_SIZE,
          1.0f);
      object.scale = glm::vec3(
          static_cast<float>(GRID_SIZE),
          static_cast<float>(GRID_SIZE),
          1.0f);
      editor_objects.emplace_back(object);
      SaveLevel("level.txt", editor_objects);
    }
  }
  if (selected_object_index != -1){
    auto &obj = editor_objects[selected_object_index];
    glm::vec2 center = glm::vec2(
        obj.position.x + obj.scale.x * 0.5f,
        obj.position.y + obj.scale.y * 0.5f);
    glm::vec2 mouse = glm::vec2(mouse_world_position);

    if (mouse.x > center.x && mouse.x < center.x + 40 && abs(mouse.y - center.y) < 4) {
      active_axis = GizmoAxis::X;
    }
    if (mouse.y > center.y && mouse.y < center.y + 40 && abs(mouse.x - center.x) < 4) {
      active_axis = GizmoAxis::Y;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
      dragging = true;
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
      dragging = false;
      active_axis = GizmoAxis::NONE;
    }
  }
  last_mouse_position = mouse_position;
}

AppState LevelEditor(GLFWwindow *window) {
  auto font_atlas = LoadFontAtlas("assets/fonts/font.xml");
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &imgui_io = ImGui::GetIO();
  imgui_io.Fonts->AddFontFromFileTTF(font_atlas.at("Debug").file.c_str(), 18.5f);
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 150");

  Shader shader("assets/shaders/rect.vert.glsl", "assets/shaders/rect.frag.glsl");
  Shader text_shader("assets/shaders/text.vert.glsl", "assets/shaders/text.frag.glsl");
  text_shader.Use();
  text_shader.SetUniform("character", 0);
  Font font("assets/fonts/Funnel_Display/FunnelDisplay-VariableFont_wght.ttf", 32);

  unsigned int rect_vertex_attrib, rect_vertex_buffer, rect_index_buffer;
  const unsigned int indices[] = {
      0, 1, 3,
      1, 2, 3
  };

  const float rect_vertices[] = {
      0.5f, 0.5f,
      0.5f, -0.5f,
      -0.5f, -0.5f,
      -0.5f, 0.5f
  };

  glGenVertexArrays(1, &rect_vertex_attrib);
  glGenBuffers(1, &rect_vertex_buffer);
  glGenBuffers(1, &rect_index_buffer);
  glBindVertexArray(rect_vertex_attrib);
  glBindBuffer(GL_ARRAY_BUFFER, rect_vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(rect_vertices), rect_vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rect_index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glfwSetScrollCallback(window, ScrollCallback);

  AppState app_state = AppState::LEVEL_EDITOR;
  while (!glfwWindowShouldClose(window) && app_state == AppState::LEVEL_EDITOR) {
    glm::ivec2 screen_dimensions;
    glfwGetFramebufferSize(window, &screen_dimensions.x, &screen_dimensions.y);
    auto projection = glm::ortho(0.0f, static_cast<float>(screen_dimensions.x),
                                 0.0f, static_cast<float>(screen_dimensions.y));
    auto view = glm::translate(glm::mat4(1.0f), -editor_camera_position);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    for (const auto &object : editor_objects) {
      Rect box;
      box.position = glm::vec2(object.position.x + GRID_SIZE / 2.0f, object.position.y + GRID_SIZE / 2.0f);
      box.scale = glm::vec2(object.scale.x, object.scale.y);
      box.color = glm::vec4(1.0f);
      box.Render(rect_vertex_attrib, shader, projection, view);
    }
    
    float left = editor_camera_position.x;
    float right = editor_camera_position.x + screen_dimensions.x;
    float bottom = editor_camera_position.y;
    float top = editor_camera_position.y + screen_dimensions.y;
    int startX = static_cast<int>(std::floor(left / GRID_SIZE)) * GRID_SIZE;
    int endX = static_cast<int>(std::ceil(right / GRID_SIZE)) * GRID_SIZE;
    int startY = static_cast<int>(std::floor(bottom / GRID_SIZE)) * GRID_SIZE;
    int endY = static_cast<int>(std::ceil(top / GRID_SIZE)) * GRID_SIZE;
    for (int x = startX; x <= endX; x += GRID_SIZE) {
      Rect grid_vertical;
      grid_vertical.position = glm::vec2(x, (startY + endY) * 0.5f);
      grid_vertical.scale = glm::vec2(1.0f, endY - startY);
      grid_vertical.color = glm::vec4(0.3f);
      grid_vertical.Render(rect_vertex_attrib, shader, projection, view);
    }

    for (int y = startY; y <= endY; y += GRID_SIZE) {
      Rect grid_horizontal;
      grid_horizontal.position = glm::vec2((startX + endX) * 0.5f, y);
      grid_horizontal.scale = glm::vec2(endX - startX, 1.0f);
      grid_horizontal.color = glm::vec4(0.3f);
      grid_horizontal.Render(rect_vertex_attrib, shader, projection, view);
    }

    // draw origin lines
    Rect origin_vertical, origin_horizontal;
    origin_vertical.position = glm::vec2(0.0f, (startY + endY) * 0.5f);
    origin_vertical.scale = glm::vec2(2.0f, endY - startY);
    origin_vertical.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    origin_vertical.Render(rect_vertex_attrib, shader, projection, view);
    origin_horizontal.position = glm::vec2((startX + endX) * 0.5f, 0.0f);
    origin_horizontal.scale = glm::vec2(endX - startX, 2.0f);
    origin_horizontal.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    origin_horizontal.Render(rect_vertex_attrib, shader, projection, view);

    if (selected_object_index != -1) {
      auto &obj = editor_objects[selected_object_index];
      glm::vec2 center = glm::vec2(
          obj.position.x + obj.scale.x * 0.5f,
          obj.position.y + obj.scale.y * 0.5f);
      Rect x_axis, y_axis;
      x_axis.position = center + glm::vec2(20.0f, 0.0f);
      x_axis.scale = glm::vec2(40.0f, 4.0f);
      x_axis.color = glm::vec4(1, 0, 0, 1);
      x_axis.Render(rect_vertex_attrib, shader, projection, view);
      y_axis.position = center + glm::vec2(0.0f, 20.0f);
      y_axis.scale = glm::vec2(4.0f, 40.0f);
      y_axis.color = glm::vec4(0, 1, 0, 1);
      y_axis.Render(rect_vertex_attrib, shader, projection, view);
    }

    if (dragging && selected_object_index != -1) {
      auto &obj = editor_objects[selected_object_index];
      glm::vec2 delta = glm::vec2(mouse_position.x - last_mouse_position.x, -(mouse_position.y - last_mouse_position.y));
      if (active_axis == GizmoAxis::X) {
        obj.position.x += delta.x;
      }
      if (active_axis == GizmoAxis::Y) {
        obj.position.y += delta.y;
      }
    }

    auto ui_projection = glm::ortho(0.0f, static_cast<float>(screen_dimensions.x),
                                    0.0f, static_cast<float>(screen_dimensions.y));
    font.Render(std::format("Grid Size: {}", GRID_SIZE), glm::vec2(10.0f, 10.0f), 1.0f,
                glm::vec3(1.0f), text_shader, ui_projection);
    glfwSwapBuffers(window);
    glfwPollEvents();
    HandleLevelEditorInput(window, app_state);
  }
  if (app_state == AppState::LEVEL_EDITOR) {
    app_state = AppState::EXIT;
  }
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  return app_state;
}