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

#include <thread>   //std::thread
#include <mutex>    //std::mutex

#include <vector>   //std::vector

#include "structs/tileset.h"
#include "game/vector.hpp"
#include "game/entities/physicsEntity.h"
#include "game/filesystem.hpp"
#include "game/gamemath.hpp"

class Map;

class Chunk
{
  private:
    
    using tilesetVector = std::vector<Tileset>;
    using gameLayer = std::array<std::array<char, game::math::chunkSize>, game::math::chunkSize>;
  
    const std::string chunkFolder = "data/map/chunks/";
      
    std::thread m_saveThread;
    std::thread m_reloadThread;
    std::mutex  m_DataMutex;

    void joinThreads();

    void reload();

    void save();
    void load();
    void generate();

    game::vec2<int> m_pos;
    
    uint32_t m_LastTick;
 
    Map* m_Map;
    
  public:
 
    struct Data : public Saveable
    {
      gameLayer m_GameLayer;
      tilesetVector m_Tilesets;
      game::EntityVector m_Entities;
      
      void write(std::ofstream& out) override
      {
        filesystem::writeRange(out, m_GameLayer);
        filesystem::writeRange(out, m_Tilesets);
        filesystem::writeRange(out, m_Entities);
      }
      
      void read(std::ifstream& in) override
      {
        filesystem::readRange(in, m_GameLayer);
        filesystem::readRange(in, m_Tilesets);
        filesystem::readRange(in, m_Entities);
      }
    };
    
    Chunk(int x, int y, Map* map);
    ~Chunk();
    
    uint32_t getLastTick() const;

    void setPos(game::vec2<int> pos);
    game::EntityVector tick();
    
    Data m_Data;
    
    game::vec2<int> getPos() const;
    
    void lockData();
    void unlockData();
};

#endif /* CHUNK_H */
