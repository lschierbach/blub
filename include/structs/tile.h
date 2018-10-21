#ifndef TILE_H
#define TILE_H

struct Tile
{
  char index;
  float rot;
  
  Tile (char index, float rot)
  {
    this->index = index;
    this->rot = rot;
  }
  
  Tile() {}
};

#endif /* TILE_H */