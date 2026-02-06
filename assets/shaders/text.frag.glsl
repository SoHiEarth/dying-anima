#version 330 core
in vec2 TexCoords;
out vec4 FragColor;
uniform sampler2D character;
uniform vec3 color;
void main() {
  vec4 sample =  vec4(1.0, 1.0, 1.0, texture(character, TexCoords).r);
  FragColor = vec4(color, 1.0) * sample;
}