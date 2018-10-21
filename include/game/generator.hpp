#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <ctime>
#include <cstdlib>

#include "structs/tile.h"
#include "structs/tileset.h"

namespace generator
{
  using tilesetVector = std::vector<Tileset>;

  long seed = 200;

  long pair(long a, long b)
  {
    if(a > 0) a *= 2;
    if(a < 0) a = -a * 2 - 1;
    if(b > 0) b *= 2;
    if(b < 0) b = -b * 2 -1;
    return 0.5 * (a+b)*(a+b+1)+b;
  }

  void generateChunk(tilesetVector& tilesets, int chunkP, int chunkQ, int size)
  {
    std::srand(seed + pair(static_cast<long>(chunkP), static_cast<long>(chunkQ)));

    int chunkType = ( rand() % 5 ) * 62 + 6;

    std::string imgName = "Stein";

    std::vector<std::vector<Tile>> tileData;

    for(int p = 0; p < size; p++)
    {
      std::vector<Tile> temp;
      for(int q = 0; q < size; q++)
      {
        temp.push_back(Tile((rand()+p*p*q*q+q*q) % (chunkType) + 1, (rand()%6)*60.0f));
      }

      tileData.push_back(temp);
    }

    tilesets.push_back(Tileset(0.5f, 1.0f, 1.0f, imgName, tileData));
  }
}

#endif /* GENERATOR_HPP */