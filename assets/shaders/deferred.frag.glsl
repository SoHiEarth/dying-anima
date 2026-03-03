#version 330 core

struct Light {
  int type; // 0 = directional, 1 = point
  vec2 position; // This is TexCoord space. Ignored for directional lights.
  float intensity;
  float radial_falloff; // Only used for point lights
  float volumetric_intensity; // Only used for point lights
  vec3 color;
};

const int MAX_LIGHTS = 32;

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D color_texture;
uniform sampler2D normal_texture;
uniform float aspect_ratio;
uniform int light_count;
uniform Light lights[MAX_LIGHTS];
uniform float exposure;

void main() {
  vec4 sample = texture(color_texture, TexCoord);
  if (sample.a == 0.0) discard;
  vec3 color = sample.rgb; 

  vec3 total_lighting = vec3(0.0);
  for (int i = 0; i < light_count && i < MAX_LIGHTS; i++) {
    if (lights[i].type == 0) {
      total_lighting += color * lights[i].color * lights[i].intensity;
    }
    else if (lights[i].type == 1) {
      vec2 light_offset = TexCoord - lights[i].position;
      light_offset.x *= aspect_ratio; // Correct for non-square aspect ratio
      vec2 light_direction = normalize(light_offset);
      float distance = length(light_offset);
      float attenuation = lights[i].intensity / (1.0 + lights[i].radial_falloff * distance * distance);
      attenuation = max(attenuation, 0.0);
      total_lighting += color * lights[i].color * attenuation;
      float volumetric = lights[i].volumetric_intensity / (1.0 + distance * distance);
      total_lighting += lights[i].color * volumetric * attenuation;
    }
  }

  const float gamma = 2.2;
  vec3 mapped = vec3(1.0) - exp(-total_lighting * exposure);
  mapped = pow(mapped, vec3(1.0 / gamma));
  FragColor = vec4(mapped, 1.0);
}