/*
 *  FILENAME:      map.h
 *
 *  DESCRIPTION:
 *      Manages Chunkmemory and Chunkdata by player positions
 *
 *  PUBLIC FUNCTIONS:
 *      void        addPlayer(SharedEntityPtr)
 *      void        removePlayer(SharedEntityPtr)
 *      void        tick()
 *
 *  NOTES:
 *      When given a player the map allocates enough memory for containerLength*containerLength chunks; When a player is removed, its chunks will be as well.
 *      Most times not every chunk is used. For example, when two players are inside the same chunk. Then, both players will share the data.
 *        The unusued chunk is stored at m_unushedChunks meanwhile.
 *
 *  AUTHOR:         Leon Schierbach     DATE: 12.09.2018
 *
 *  DEBUG:          DEBUG_MAP
 *                  DEBUG_MAP_PRINT
 *                  DEBUG_MAP_UPDATE
 *
 *
 *  CHANGES:
 */

#ifndef MAP_H
#define MAP_H

#ifdef DEBUG_MAP
  #define DEBUG_MAP_PRINT
  #define DEBUG_MAP_UPDATE
#endif

#include "logic/chunk.h"
#include "structs/tileset.h"
#include "game/entity.h"

#include <array>  //std::array
#include <vector> //std::vector
#include <map>    //std::map
#include <stack>  //std::stack

#include <memory> //std::shared_ptr

class Map
{
  private:
    static const size_t loadingDistance = 1;
    static const size_t containerLength = loadingDistance * 2 + 1;

  public:
    using SharedEntityPtr = std::shared_ptr<Entity>;
    using SharedChunkPtr = std::shared_ptr<Chunk>;
    using SharedChunkPtrArr = std::array<std::array<SharedChunkPtr, containerLength>, containerLength>;

  private:
    std::stack<SharedChunkPtr> m_unusedChunks;

    std::map<SharedEntityPtr, SharedChunkPtrArr> m_Chunks;

    void updateEntity(SharedEntityPtr entity, bool firstUpdate = false);

    void print();


  public:
    Map();
    ~Map();

    void tick();

    void addEntity(SharedEntityPtr entity);
    void removeEntity(SharedEntityPtr entity);

    Chunk* getChunk(int relativeP, int relativeQ, SharedEntityPtr entity);
    static size_t getLoadingDistance();
};

#endif /* MAP_H */
