/*
 *  FILENAME:      chunk.h
 *
 *  DESCRIPTION:
 *      Saves/Loads chunk-data (tilesets) from disk and generates new chunks
 *
 *  PUBLIC FUNCTIONS:
 *      void        setPos(int p, int q)
 *
 *  NOTES:
 *      When changing the position, the chunk will first save its data and then load the new one. This is done in "void reload()".
 *      Also, when the chunk is destroyed, it will save its data as well
 *
 *  AUTHOR:         Leon Schierbach     DATE: 12.09.2018
 *
 *  DEBUG:          DEBUG_CHUNK
 *                  DEBUG_CHUNK_TILESET
 *                  DEBUG_CHUNK_RELOAD
 *
 *  CHANGES:
 *      AUTHOR:     LS
 *      DATE:       18.09.2018
 *      DESC:       implemented save/load of tiles
 */

#ifndef CHUNK_H
#define CHUNK_H

#ifdef DEBUG_CHUNK
  #define DEBUG_CHUNK_TILESET
  #define DEBUG_CHUNK_RELOAD
  #define DEBUG_CHUNK_ENTITY
#endif

#include <thread>   //std::thread
#include <mutex>    //std::mutex

#include <vector>   //std::vector

#include "structs/tileset.h"
#include "game/vector.hpp"
#include "game/entities/physicsEntity.h"

class Chunk
{
  private:
  
    const std::string chunkFolder = "data/chunks/";
      
    std::thread m_saveThread;
    std::thread m_reloadThread;

    void joinThreads();

    using tilesetVector = std::vector<Tileset>;
    using entityVector = std::vector<PhysicsEntity>;

    void reload();

    void save();
    void load();
    void generate();

    game::vec2<int> m_pos;
    
    std::mutex    m_DataMutex;

    
    uint32_t m_LastTick;
 
  public:
 
    struct Data : public Saveable
    {
      tilesetVector m_Tilesets;
      entityVector m_Entities;
      
      void write(std::ofstream& out) override
      {
        filesystem::writeRange(out, m_Tilesets);
        filesystem::writeRange(out, m_Entities);
      }
      
      void read(std::ifstream& in) override
      {
        filesystem::readRange(in, m_Tilesets);
        filesystem::readRange(in, m_Entities);
      }
    };

  private:

    Data getData();
  
  public:

    Chunk(int p, int q);
    Chunk(Chunk& cpy);
    ~Chunk();

    int getQ() const;
    int getP() const;
    
    uint32_t getLastTick() const;

    void setPos(game::vec2<int> pos);
    void tick();
    
    Data m_Data;
    
    game::vec2<int> getPos() const;
    
    void lockData();
    void unlockData();
};

#endif /* CHUNK_H */
