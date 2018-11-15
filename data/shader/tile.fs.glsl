#version 130

in vec4 color;
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D tex;

in float ftime;

uniform vec3 tileColor;

void main(void)
{
  fragColor = texture2D(tex, texCoord) * vec4(tileColor, 1.0);
}