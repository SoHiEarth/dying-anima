#version 330 core

in vec2 TexCoord;
uniform sampler2D source;
uniform bool horizontal;
out vec4 FragColor;

const float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main() {
  vec2 tex_offset = 1.0 / textureSize(source, 0);
  vec3 result = texture(source, TexCoord).rgb * weight[0];
  
  if(horizontal) {
    for(int i = 1; i < 5; ++i) {
      result += texture(source, TexCoord + vec2(tex_offset.x * i, 0.0)). rgb * weight[i];
      result += texture(source, TexCoord - vec2(tex_offset. x * i, 0.0)).rgb * weight[i];
    }
  } else {
    for(int i = 1; i < 5; ++i) {
      result += texture(source, TexCoord + vec2(0.0, tex_offset. y * i)).rgb * weight[i];
      result += texture(source, TexCoord - vec2(0.0, tex_offset. y * i)).rgb * weight[i];
    }
  }
  
  FragColor = vec4(result, 1.0);
}