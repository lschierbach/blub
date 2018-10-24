#ifndef TILE_H
#define TILE_H

#include "game/saveable.h"
#include "game/filesystem.hpp"

struct Tile : public Saveable
{
  char index;
  float rot;
  
  Tile (char index, float rot)
  {
    this->index = index;
    this->rot = rot;
  }
  
  Tile() {}
  
  void write(std::ofstream& out) override
  {
    filesystem::writeStruct(out, index);
    filesystem::writeStruct(out, rot);
  }

  void read(std::ifstream& in) override
  {
    filesystem::readStruct(in, index);
    filesystem::readStruct(in, rot);
  }
};

#endif /* TILE_H */