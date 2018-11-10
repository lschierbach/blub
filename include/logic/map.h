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

#include "game/global.h"
#include "logic/chunk.h"
#include "structs/tileset.h"
#include "game/entity.h"

#include <array>  //std::array
#include <vector> //std::vector
#include <map>    //std::map
#include <stack>  //std::stack
#include <mutex>

#include <memory> //std::shared_ptr

class Map
{
  private:
    static const size_t loadingDistance = 2;
    static const size_t containerLength = loadingDistance * 2 + 1;

  public:
    using SharedEntityPtr = std::shared_ptr<Entity>;
    using SharedChunkPtr = std::shared_ptr<Chunk>;
    using SharedChunkPtrArr = std::array<std::array<SharedChunkPtr, containerLength>, containerLength>;
    
    static size_t getLoadingDistance();

  private:
    std::stack<SharedChunkPtr> m_unusedChunks;

    std::map<SharedEntityPtr, SharedChunkPtrArr> m_Chunks;

    const std::string m_MapFolder = "data/map/";
    
    void init();
    
    struct Data : public Saveable
    {
      unsigned int m_EntityCount;
      
      void write(std::ofstream& out) override
      {
        filesystem::writeStruct(out, m_EntityCount);
      }
      
      void read(std::ifstream& in) override
      {
        filesystem::readStruct(in, m_EntityCount);
      }
    };
    
    Data m_Data;
    
    std::mutex m_DataMutex;
    
    void updateEntity(SharedEntityPtr entity, bool firstUpdate = false);
    void tickChunks();
    
    void print();


  public:
    Map();
    ~Map();
    Map(const Map&) = delete;
    Map(Map&&) = delete;
    Map& operator=(const Map&) = delete;
    Map& operator=(const Map&&) = delete;
    
    Chunk* getIdealChunk(game::vec2<float> pos);
    Chunk* getIdealChunk(game::vec2<int> pos);
    
    void tick();

    void addEntity(SharedEntityPtr entity);
    void removeEntity(SharedEntityPtr entity);

    unsigned int getNextEntityId();
    
    std::vector<PhysicsEntity*> getEntitiesAt(game::vec2<float> pos, float radius);
    
    template<typename EntityType>
    auto get_entity_at(game::vec2<float> pos) -> EntityType*;
    
    template<typename EntityType>
    auto get_entity_by_id(unsigned int id) -> EntityType*;
    
    template<typename EntityType, typename Lambda>
    auto for_each_entity_in_range(game::vec2<float> pos, float radius, Lambda&& lam) -> void;
    
    template<typename EntityType, typename Lambda>
    auto for_each_entity_in_box(game::vec2<float> boxTopLeft, game::vec2<float> size, Lambda&& lam) -> void;
    
    Chunk* getChunk(int relativeP, int relativeQ, SharedEntityPtr entity);
};

#include "map.tpp"

#endif /* MAP_H */
