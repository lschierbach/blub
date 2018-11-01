#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <ctime>
#include <cstdlib>

#include "structs/tile.h"
#include "structs/tileset.h"
#include "game/simplexnoise.h"
#include "game/entities/physicsEntity.h"
#include "game/gamemath.h"

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

  template<unsigned SIZE>
  void getCollisionMap(std::array<std::array<bool, SIZE>, SIZE>& arr)
  {
    for (auto p = 0u; p < SIZE; p++)
    {
      for (auto q = 0u; q < SIZE; q++)
      {
        if(p == 0 || p == SIZE - 1 || q == 0 || q == SIZE - 1)
        {
          arr[p][q] = false;
        }
        else
        {
          arr[p][q] = SimplexNoise::noise(p, q) > 0.1;
        }
      }
    }
  }
  
  template<unsigned SIZE>
  void generateChunk(Chunk::Data& chunkData, int chunkP, int chunkQ, int size)
  {
    std::array<std::array<bool, SIZE>, SIZE> collisionMap;
    getCollisionMap<SIZE>(collisionMap);
    
    
    std::srand(seed + pair(static_cast<long>(chunkP), static_cast<long>(chunkQ)));

    int chunkType = ( rand() % 5 ) * 62 + 6;

    std::string imgName = "Stein";

    std::vector<std::vector<Tile>> tileData;
    
    // which tiles are open? Starting from top left, going clockwise.
    int open[6] = { 0 };
    
    for(int q = 0; q < size; q++)
    {
      std::vector<Tile> temp;
      for(int p = 0; p < size; p++)
      {
        char tileIndex = 65;
        
        if(collisionMap[p][q])
        {
          if (q > 0)         open[0] = collisionMap.at(p).at(q-1);
          if (q > 0 && p < size - 1)  open[1] = collisionMap.at(p+1).at(q-1);
          if (p < size - 1)           open[2] = collisionMap.at(p+1).at(q);
          if (q < size - 1)           open[3] = collisionMap.at(p).at(q+1);
          if (p > 0 && q < size - 1)  open[4] = collisionMap.at(p-1).at(q+1);
          if (p > 0)         open[5] = collisionMap.at(p-1).at(q);
          
          tileIndex = (open[0] + (2*open[1]) + (4*open[2]) + (8*open[3]) + (16*open[4]) + (32*open[5])) + 1; 
          // TODO: rewrite smarter... sorry
        }
      
        temp.push_back(Tile(tileIndex, .0f));
      }

      tileData.push_back(temp);
    }

    chunkData.m_Tilesets.push_back(Tileset(0.5f, 1.0f, 1.0f, imgName, tileData));
    
    for (auto i = 0u; i < 5; i++)
    {
      auto p = vec2<float>(
        static_cast<float>(game::math::chunkToAxialP(chunkP) + (rand() % game::math::chunkSize)),
        static_cast<float>(game::math::chunkToAxialQ(chunkQ) + (rand() % game::math::chunkSize))
      );
      PhysicsEntity e(p[0], p[1]);
      chunkData.m_Entities.push_back(e);
    }
  }
}

#endif /* GENERATOR_HPP */