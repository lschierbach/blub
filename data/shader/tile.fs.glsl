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

uniform sampler2D nmap;

float normalFac(int lightIndex) {
  vec3 normalVec = normalize(texture2D(nmap, texCoord).xyz * 2.0 - 1.0);

  vec3 lightVec = normalize(vec3(gl_FragCoord.xy - lights[lightIndex].xy, -1.0));

  return ((dot(normalVec, lightVec)+1.0) / 2.0);
}

vec3 lightFac() {
  float fac = 0.0;
  vec3 avgColor = vec3(0.0, 0.0, 0.0);
  for(int i=0; i<numLights; i++) {
    vec3 lightUnits = lights[i]*pixelsInUnit;
    float dist = distance(gl_FragCoord.xy, lightUnits.xy);
    float newfac = smoothstep(lightUnits.z,0.0,dist) * (normalFac(i)/dist*lightUnits.z);
    fac = max(fac, newfac);
    avgColor += lightColors[i] * newfac;
  }
  avgColor /= numLights;
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