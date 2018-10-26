#ifndef CAMERAENTRY_H
#define CAMERAENTRY_H

#include "logic/map.h"

struct CameraEntry
{
  Map::SharedEntityPtr camera;
  std::array<float,4> data;
  size_t id;
};

#endif /* CAMERAENTRY_H */
