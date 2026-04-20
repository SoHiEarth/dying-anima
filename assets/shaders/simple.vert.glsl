#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexcoord;
out float z;
out vec2 TexCoord;
uniform mat4 mvp;
uniform float z_index;
void main() {
  TexCoord = aTexcoord;
  gl_Position = mvp * vec4(aPos.x, aPos.y, aPos.z, 1.0);
  z = z_index;
}
