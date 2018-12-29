#version 130

in vec4 color;
in vec2 texCoord;
out vec4 fragColor;
in float ftime;
in float pixelsInUnit;

uniform sampler2D tex;
uniform vec3 ambient;

uniform vec3[12] lights;
uniform vec3[12] lightColors;
uniform int numLights;

uniform sampler2D nmap;

float rand(float seed) {
  return fract(sin(seed)*584216.0);
}

float normalFac(int lightIndex) {
  vec3 normalVec = normalize(texture2D(nmap, texCoord).xyz * 2.0 - 1.0);

  vec3 lightVec = normalize(vec3(gl_FragCoord.xy - lights[lightIndex].xy*pixelsInUnit, -1.0));

  //disregard normal map close to source, immitating bounce light and masking ugly normal glitches with 0 size source
  float dist = distance(gl_FragCoord.xy, lights[lightIndex].xy*pixelsInUnit);
  float near = 1.0-smoothstep(0.02*pixelsInUnit, 0.515*pixelsInUnit*lights[lightIndex].z, dist);

  return ( (dot(normalVec, lightVec)*0.5) + 0.5 ) * (1.0-near) + near;
}

vec3 lightFac() {
  float fac = 0.0;
  vec3 avgColor = vec3(0.0, 0.0, 0.0);
  for(int i=0; i<numLights; i++) {
    vec3 lightUnits = lights[i]*pixelsInUnit;
    float dist = distance(gl_FragCoord.xy, lightUnits.xy);
    float newfac = (1.0-smoothstep(0.0,lightUnits.z+0.036*sin(5.0*ftime)*pixelsInUnit,dist)) * normalFac(i);
    fac = max(fac, newfac);
    avgColor += lightColors[i] * newfac;
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
[WORKS]-Normal maps? Maybe use SDL_gpu texture slots? Dot product?
-Basic Vertex Animation? (Probably not, eating cpu)
-Add raw pointer to CameraEntry
*/