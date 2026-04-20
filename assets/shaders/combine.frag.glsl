#version 330 core

in vec2 TexCoord;
uniform sampler2D bloom_texture;
uniform sampler2D scene_texture;
uniform float bloom_strength = 1.0;

out vec4 FragColor;

void main() {
  vec3 bloom = texture(bloom_texture, TexCoord).rgb;
  vec3 original = texture(scene_texture, TexCoord).rgb;
  FragColor = vec4(original + bloom, 1.0);
}
