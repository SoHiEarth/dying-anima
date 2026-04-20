#version 330 core
in float z;
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D texture1;
void main() {
  FragColor = texture(texture1, TexCoord);
  if (z < -1) {
    FragColor.rgb -= (1-z)/8;
  }
}
