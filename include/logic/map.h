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

    using SharedChunkPtr = std::shared_ptr<Chunk>;
    
    struct ChunkLogicLock
    {
      SharedChunkPtr chunk;
      bool isLogicLocked;
      
      void lock()
      {
        isLogicLocked = true;
        chunk.get()->lockData();
      }
      
      void unlock()
      {
        isLogicLocked = false;
        chunk.get()->unlockData();
      }
      
      Chunk* get() const
      {
        return chunk.get();
      }
      
      SharedChunkPtr getSharedPtr() const
      {
        return chunk;
      }
    };
    
    using SharedChunkPtrArr = std::array<std::array<ChunkLogicLock, containerLength>, containerLength>;
    
  public:
    using SharedEntityPtr = std::shared_ptr<Entity>;
    
    static size_t getLoadingDistance();

    
    class ScopedChunkLock
    {
      private:
        ChunkLogicLock* chunkLock;
        bool previouslyLocked;
      
      public:
        // @todo: put this somewhere else 
        ScopedChunkLock(ChunkLogicLock& c) : chunkLock(&c)
        {
          previouslyLocked = chunkLock->isLogicLocked;

          if (!previouslyLocked)
          {
            chunkLock->get()->lockData();
            chunkLock->isLogicLocked = true;
          }
        }
        
        ScopedChunkLock(ScopedChunkLock&& c)
        {
          chunkLock = c.chunkLock;
          c.chunkLock = nullptr;
          previouslyLocked = c.previouslyLocked;
        }
        
        ScopedChunkLock& operator=(ScopedChunkLock&& c)
        {
          chunkLock = c.chunkLock;
          c.chunkLock = nullptr;
          previouslyLocked = c.previouslyLocked;
          return *this;
        }
        
        ~ScopedChunkLock()
        {
          if (!previouslyLocked && chunkLock != nullptr)
          {
            chunkLock->get()->unlockData();
            chunkLock->isLogicLocked = false;
          }
        }

        ScopedChunkLock()                                  = delete;
        ScopedChunkLock(const ScopedChunkLock&)            = delete;
        ScopedChunkLock& operator=(const ScopedChunkLock&) = delete;

        Chunk* operator->() const
        {
          return chunkLock->get();
        }
        
        Chunk* get() const
        {
          return chunkLock->get();
        }
    };
    
  private:
    std::stack<SharedChunkPtr> m_unusedChunks;

    std::map<SharedEntityPtr, SharedChunkPtrArr> m_Chunks;

    const std::string m_MapFolder = "data/map/";
    
    void init();
    
    struct Data : public Saveable
    {
      unsigned int m_EntityCount;
      // @todo: do this in a map, needs filesystem std::map-support
      std::vector<std::string> m_TileSetImgs;

      
      void write(std::ofstream& out) override
      {
        filesystem::writeStruct(out, m_EntityCount);
        filesystem::writeRange(out, m_TileSetImgs);
      }
      
      void read(std::ifstream& in) override
      {
        filesystem::readStruct(in, m_EntityCount);
        filesystem::readRange(in, m_TileSetImgs);
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
    Map(const Map&)             = delete;
    Map(Map&&)                  = delete;
    Map& operator=(const Map&)  = delete;
    Map& operator=(const Map&&) = delete;
    
    std::optional<ScopedChunkLock> getIdealChunk(game::vec2<float> pos);
    std::optional<ScopedChunkLock> getIdealChunk(game::vec2<int> pos);
    
    void tick();

    void addEntity(SharedEntityPtr entity);
    void removeEntity(SharedEntityPtr entity);

    unsigned int getNextEntityId();
    unsigned int addNewTileset(const std::string& imgName);
    
    std::optional<std::string> getTilesetImgName(unsigned id);
    
    char getGamelayerIdAt(game::vec2<float> pos);
    
    template<typename EntityType>
    auto get_entity_at(game::vec2<float> pos) -> EntityType*;
    
    template<typename EntityType>
    auto get_entity_by_id(unsigned int id) -> EntityType*;
    
    template<typename Lambda>
    auto for_each_chunk(Lambda&& lam) -> void;
    
    template<typename Lambda>
    auto for_each_chunk_in_box(game::vec2<float> boxTopLeft, game::vec2<float> size, Lambda&& lam) -> void;
    
    template<typename EntityType, typename Lambda>
    auto for_each_entity(Lambda&& lam) -> void;
    
    template<typename EntityType, typename Lambda>
    auto for_each_entity_in_range(game::vec2<float> pos, float radius, Lambda&& lam) -> void;
    
    template<typename EntityType, typename Lambda>
    auto for_each_entity_in_box(game::vec2<float> boxTopLeft, game::vec2<float> size, Lambda&& lam) -> void;
};

#include "map.tpp"

#endif /* MAP_H */
