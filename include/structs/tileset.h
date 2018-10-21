#ifndef TILELAYER_H
#define TILELAYER_H

#include <vector>
#include <string>

#include "structs/tile.h"
#include "game/saveable.h"
#include "game/filesystem.hpp"

struct Tileset : public Saveable
{
  float offsetX;
  float offsetY;

  float scale;

  std::vector<std::vector<Tile>> tileData;

  std::string imgName;

  Tileset(float offsetX, float offsetY, float scale, const std::string& imgName, std::vector<std::vector<Tile>>& tileData)
  {
    this->offsetX = offsetX;
    this->offsetY = offsetY;
    this->scale = scale;

    this->imgName = imgName;

    this->tileData = tileData;
  }

  Tileset() {}

  void printTiles() const
  {
    for (auto& tilevec : tileData)
    {
      for (auto& tile : tilevec)
      {
        printf("%c ", tile.index, tile.rot);
      }
      printf("\n");
    }
  }

  void write(std::ofstream& out) override
  {
    // write pod
    filesystem::writeStruct(out, offsetX);
    filesystem::writeStruct(out, offsetY);
    filesystem::writeStruct(out, scale);

    // write tiledata
    filesystem::writeRange(out, tileData);

    // write imgName
    filesystem::writeRange(out, imgName);
  }

  void read(std::ifstream& in) override
  {
    // read pod
    filesystem::readStruct(in, offsetX);
    filesystem::readStruct(in, offsetY);
    filesystem::readStruct(in, scale);

    // read tileData
    filesystem::readRange(in, tileData);

    // read imgName
    filesystem::readRange(in, imgName);
  }
};


#endif /* TILELAYER_H */
