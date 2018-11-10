#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <ctime>
#include <cstdlib>

#include "structs/tile.h"
#include "structs/tileset.h"
#include "game/simplexnoise.h"
#include "game/entities/physicsEntity.h"
#include "game/gamemath.hpp"
#include "logic/map.h"

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
  void generateChunk(Chunk::Data& chunkData, int chunkP, int chunkQ, int size, Map* map)
  {
    std::array<std::array<bool, SIZE>, SIZE> collisionMap;
    getCollisionMap<SIZE>(collisionMap);
    
    
    std::srand(seed + pair(static_cast<long>(chunkP), static_cast<long>(chunkQ)));

    int chunkType = ( rand() % 5 ) * 62 + 6;

    std::string imgName = "Stein";

    std::vector<std::vector<Tile>> tileData;
    
    // which tiles are open? Starting from top left, going clockwise.
    int open[6] = { 0 };
    
    for(int y = 0; y < size; y++)
    {
      std::vector<Tile> temp;
      for(int x = 0; x < size; x++)
      {
        temp.push_back(Tile(collisionMap[x][y] ? 1 : 0, .0f));
        
        chunkData.m_GameLayer[x][y] = collisionMap[x][y] ? 1 : 0;
        
        if (rand() % 100 <= 10 && !collisionMap[x][y])
        {
          auto entityPos = game::math::chunkToEntityPos( {chunkP, chunkQ} );
          auto pos = entityPos + game::vec2<float>(static_cast<float>(x), static_cast<float>(y));
          
          // @todo: why not += (.5, .5) ?
          pos += game::vec2<float>(.0f, .5f);
          
          auto width = rand() / float(RAND_MAX);
          width += 0.2f;
          if (rand() % 100 > 50)
          {
            PhysicsEntity e(pos, { width, width }, { .5f, .5f }, map->getNextEntityId());
            chunkData.m_Entities.push_back( { e });
          }
          else
          {
            Entity e(pos, { width, width }, { .5f, .5f }, map->getNextEntityId());
            chunkData.m_Entities.push_back( { e });
          }
        }
      }

      tileData.push_back(temp);
    }

    chunkData.m_Tilesets.push_back(Tileset(0.5f, 1.0f, 1.0f, imgName, tileData));
  }
}

#endif /* GENERATOR_HPP */