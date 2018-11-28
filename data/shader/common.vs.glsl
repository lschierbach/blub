#version 110

attribute vec3 gpu_Vertex;
attribute vec2 gpu_TexCoord;
attribute vec4 gpu_Color;
uniform mat4 gpu_ModelViewProjectionMatrix;

varying vec4 color;
varying vec2 texCoord;
varying float ftime;

uniform float time;

float rand(float seed) {
  return fract(sin(seed)*584216.0);
}

void main(void)
{
  ftime = time;
  color = gpu_Color;
  texCoord = gpu_TexCoord;
  gl_Position = gpu_ModelViewProjectionMatrix * vec4(gpu_Vertex.xyz, 1.0);
}