#version 130

in vec4 color;
in vec2 texCoord;
out vec4 fragColor;
in float ftime;

uniform sampler2D tex;
uniform vec3 ambient;

uniform float pixelsInUnit;
uniform vec3[12] lights;
uniform vec3[12] lightColors;
uniform int numLights;

vec3 lightFac() {
  float fac = 0.0;
  vec3 avgColor = vec3(0.0, 0.0, 0.0);
  for(int i=0; i<numLights; i++) {
    vec3 lightUnits = lights[i]*pixelsInUnit;
    float dist = distance(gl_FragCoord.xy, lightUnits.xy);
    float newfac = smoothstep(lightUnits.z,0.0,dist);
    fac = max(fac, newfac);
    avgColor += lightColors[i]*newfac;
  }
  //avgColor /= numLights;
  return fac*avgColor + (1.0-fac)*ambient;
}

void main(void)
{
  fragColor = texture2D(tex, texCoord) * vec4(lightFac(), 1.0);
}

/*
TODO:
-Add light class in renderer/somewhere accessible to renderer
-Also use the light shader for entities (combine renderer loops?)
-Add Z index for entities or other way for background/foreground tilesets
-Make separate function to load shaders (why isn't it working with refs?)
-Normal maps? Maybe use SDL_gpu texture slots? Dot product?
-Basic Vertex Animation? (Probably not, eating cpu)
-Add raw pointer to CameraEntry
*/