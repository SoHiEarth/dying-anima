#version 330 core
layout (location = 0) in vec3 aPosition;
out vec4 color;
uniform mat4 mvp;
uniform vec4 rect_color;
void main() {
  color = rect_color;
  gl_Position = mvp * vec4(aPosition.xyz, 1.0);
}
