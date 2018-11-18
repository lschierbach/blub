#version 130

in vec4 color;
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D tex;

in float ftime;

uniform float outerBound;
uniform float innerBound;
uniform float aspect;

uniform vec3 outerColor;
uniform vec3 innerColor;

uniform float noiseFalloff;
uniform float noiseRatio;

float rand(float seed) {
  return fract(sin(seed)*584216.0);
}

float rand2d(vec2 seed) {
  return fract(sin(dot(seed, vec2(62.2473,24.9312)))*236236.6231 + fract(ftime*2.0));
}

void main(void)
{

  vec2 tmpCoord = texCoord - vec2(0.5, 0.5);
  tmpCoord.x = tmpCoord.x*aspect;
  float dist = sqrt(abs(tmpCoord.x*tmpCoord.x) + abs(tmpCoord.y*tmpCoord.y));
  float boundsFac = smoothstep(outerBound/2.0, innerBound/2.0, dist);

  float noise = (rand2d(texCoord)-0.5) * (1.0-boundsFac*noiseFalloff) * noiseRatio;
  vec4 tmpColor = texture2D(tex, texCoord);

  fragColor = (tmpColor*vec4(innerColor, 1.0))*boundsFac + (tmpColor*vec4(outerColor, 1.0))*(1.0-boundsFac) + vec4(noise, noise, noise, 0.0);
}