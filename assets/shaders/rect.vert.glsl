#version 330 core
layout (location = 0) in vec2 aPosition;
out vec4 color;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec4 rect_color;
void main() {
  color = rect_color;
  gl_Position = projection * view * model * vec4(aPosition.xy, 0.0, 1.0);
}