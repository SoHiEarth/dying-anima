#version 330 core

in vec2 TexCoord;
uniform sampler2D bloom_texture;
uniform sampler2D scene_texture;
uniform float bloom_strength = 0.5;
uniform float exposure = 1.0; 

out vec4 FragColor;

void main() {
  vec4 bloom = texture(bloom_texture, TexCoord);
  vec4 original = texture(scene_texture, TexCoord);
  vec3 hdr_color = original.rgb + bloom.rgb * bloom_strength;
  vec3 mapped = vec3(1.0) - exp(-hdr_color * exposure);
  const float gamma = 2.2;
  mapped = pow(mapped, vec3(1.0 / gamma));  
  FragColor = vec4(mapped, 1.0);
}