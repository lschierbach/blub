#version 110

attribute vec3 gpu_Vertex;
attribute vec2 gpu_TexCoord;
attribute vec4 gpu_Color;
uniform mat4 gpu_ModelViewProjectionMatrix;

varying vec4 color;
varying vec2 texCoord;
varying float ftime;
varying float pixelsInUnit;

uniform float time;
uniform float piu;

float rand(float seed) {
  return fract(sin(seed)*584216.0);
}

void main(void)
{
  ftime = time;
  pixelsInUnit = piu;
  color = gpu_Color;
  texCoord = gpu_TexCoord;
  //gl_Position = gpu_ModelViewProjectionMatrix * vec4(gpu_Vertex.xyz, 1.0) + vec4(((gpu_TexCoord.y-0.0625)*piu*0.002)*sin(ftime*2.0), 0.0, 0.0, 0.0);
  gl_Position = gpu_ModelViewProjectionMatrix * vec4(gpu_Vertex.xyz, 1.0);
}