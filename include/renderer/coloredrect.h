#ifndef COLOREDRECT_H
#define COLOREDRECT_H

#include "SDL_gpu.h"

struct ColoredRect {
  GPU_Rect rect;
  SDL_Color border;
  SDL_Color area;
  float corners;

  ColoredRect(float x, float y, float w, float h, SDL_Color borderColor, SDL_Color areaColor, float cornerRadius) {
    rect = GPU_MakeRect(x, y, w, h);
    border = borderColor;
    area = areaColor;
    corners = cornerRadius;
  }
};

#endif /* COLOREDRECT_H */
