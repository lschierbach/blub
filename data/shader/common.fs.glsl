#version 130

in vec4 color;
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D tex;

void main(void)
{
  float outerBound = 0.8;
  float innerBound = 0.2;
  float aspect = 1.77777777;

  vec3 outerColor = vec3(0.5, 1.0, 0.5);
  vec3 innerColor = vec3(1.0, 0.6, 0.6);

  vec2 tmpCoord = texCoord - vec2(0.5, 0.5);
  tmpCoord.y = tmpCoord.y/aspect;
  float dist = sqrt(abs(tmpCoord.x*tmpCoord.x) + abs(tmpCoord.y*tmpCoord.y));
  float boundsFac = smoothstep(outerBound/2.0, innerBound/2.0, dist);

  vec4 tmpColor = texture2D(tex, texCoord);

  fragColor = (tmpColor*vec4(innerColor, 1.0))*boundsFac + (tmpColor*vec4(outerColor, 1.0))*(1.0-boundsFac);
}