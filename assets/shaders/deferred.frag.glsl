#version 330 core

struct Light {
  int type;
  vec2 position;
  float intensity;
  float radial_falloff;
  float volumetric_intensity;
  vec3 color;
};

const int MAX_LIGHTS = 32;

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D color_texture;
uniform sampler2D normal_texture;
uniform int light_count;
uniform Light lights[MAX_LIGHTS];

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
      vec2 light_direction = normalize(light_offset);
      float distance = length(light_offset);
      float attenuation = lights[i].intensity / (1.0 + lights[i].radial_falloff * distance * distance);
      attenuation = max(attenuation, 0.0);
      total_lighting += color * lights[i].color * attenuation;
      float volumetric = lights[i].volumetric_intensity / (1.0 + distance * distance);
      total_lighting += lights[i].color * volumetric * attenuation;
    }
  }

  FragColor = vec4(total_lighting, 1.0);
}