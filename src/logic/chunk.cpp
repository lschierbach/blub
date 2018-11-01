/*
 *  FILENAME:      chunk.cpp
 *
 *  AUTHOR:        Leon Schierbach     DATE: 12.09.2018
 *
 *  CHANGES:
 *      LS, 20.09.2018
 *                 implemented save/load using filesystem
 *
 */

#include <string>
#include <vector>
#include <iostream>

#include "game/global.h"
#include "logic/chunk.h"
#include "game/filesystem.hpp"
#include "game/generator.hpp"
#include "game/gamemath.h"

Chunk::Chunk(int p, int q) : m_pos({p, q})
{
  reload();
}

Chunk::~Chunk()
{
  joinThreads();
}

/**
    returns p
*/
int Chunk::getP() const
{
  return m_pos[0];
}

/**
    returns q
*/
int Chunk::getQ() const
{
  return m_pos[1];
}

game::Vector<2, int> Chunk::getPos() const
{
  return m_pos;
}

/**
    returns m_tilesets threadsafe
*/
Chunk::Data Chunk::getData()
{
  std::lock_guard<std::mutex> lock(m_DataMutex);
  return m_Data;
}

/**
    Changes position of chunk on the world.
    If the new position doesn't equal the old, the chunk will save its current m_Data and load/generate new.

    @param p    p
    @param q    q
*/
void Chunk::setPos(game::Vector<2, int> pos)
{
  // don't reload if position hasn't changed
  if(pos == this->m_pos)
  {
    return;
  }

  // make sure no one is accessing tilesetm_Data at the moment
  joinThreads();

  // on start no m_Data is in m_tilesets...
  if (!m_Data.m_Tilesets.empty())
  {
    save();
  }

  this->m_pos = pos;

  m_reloadThread = std::thread(&Chunk::reload, this);
}

/**
    Saves the m_Data of the tilesets on the disk
*/
void Chunk::save()
{
  #ifdef DEBUG_CHUNK_RELOAD
    printf("[CHUNK]SAVE\n");
  #endif /* DEBUG_CHUNK_RELOAD */

  #ifdef DEBUG_CHUNK_TILESET
    for(const auto& set : m_Data.m_Tilesets)
    {
      std::cout << "Tileset\t" << set.imgName << std::endl;
      set.printTiles();
    }
  #endif /* DEBUG_CHUNK_TILESET */
  #ifdef DEBUG_CHUNK_ENTITY
  for (const auto& ent : m_Data.m_Entities)
  {
    printf("Entity with pos, anchor, size:\n");
    ent.getPQ().print();
    ent.getAnchor().print();
    ent.getSize().print();
  }
  #endif /* DEBUG_CHUNK_ENTITY */

  // get unique path
  std::string path = chunkFolder + std::to_string(m_pos[0]) + "." + std::to_string(m_pos[1]);

  Data temp;

  // copy threadsafe
  std::unique_lock<std::mutex> lock(m_DataMutex);

  temp = m_Data;

  lock.unlock();

  // write m_Data into .tdat file
  filesystem::writeStruct(path + ".tdat", temp);
}

/**
    Loads m_Data from the disk, depenting on the position, into its tilesets
*/
void Chunk::load()
{
  // get unique path
  std::string path = chunkFolder + std::to_string(m_pos[0]) + "." + std::to_string(m_pos[1]);

  Data temp;

  // read m_tilesets from .tdat file
  filesystem::readStruct(path + ".tdat", temp);
  // copy it threadsafe
  std::lock_guard<std::mutex> lock(m_DataMutex);

  m_Data = temp;


  #ifdef DEBUG_CHUNK_RELOAD
    printf("[CHUNK]\tLOAD\n");
  #endif /* DEBUG_CHUNK_RELOAD */

  #ifdef DEBUG_CHUNK_TILESET
  for (const auto& set : m_Data.m_Tilesets)
  {
    std::cout << "Tileset\t" << set.imgName << std::endl;
    set.printTiles();
  }
  #endif /* DEBUG_CHUNK_TILESET */
  #ifdef DEBUG_CHUNK_ENTITY
  for (const auto& ent : m_Data.m_Entities)
  {
    printf("Entity with pos, anchor, size:\n");
    ent.getPQ().print();
    ent.getAnchor().print();
    ent.getSize().print();
  }
  #endif /* DEBUG_CHUNK_ENTITY */
}

/**
    Generates a new chunk
*/
void Chunk::generate()
{
  // dummy m_Data
  std::lock_guard<std::mutex> lock(m_DataMutex);

  generator::generateChunk<game::math::chunkSize>(m_Data, m_pos[0], m_pos[1], game::math::chunkSize);
  
  #ifdef DEBUG_CHUNK_RELOAD
    printf("[CHUNK]GENERATE\n");
  #endif /* DEBUG_CHUNK_RELOAD */

}

/**
    clears tilesets and overwrites it
*/
void Chunk::reload()
{
  // remove old tileset m_Data
  m_Data.m_Tilesets.clear();
  m_Data.m_Entities.clear();
  
  // if chunk-m_Data exists
  if (filesystem::fileExists(chunkFolder + std::to_string(m_pos[0]) + "." + std::to_string(m_pos[1]) + ".tdat"))
  {
    // load it from disk
    load();
  }
  else
  {
    // else generate chunk
    generate();

    // and save it on disk
    m_saveThread = std::thread(&Chunk::save, this);
  }
}

/**
    joins threads
*/
void Chunk::joinThreads()
{
  // always reloadThread first, because he can spawn savethread!
  if (m_reloadThread.joinable())
  {
    m_reloadThread.join();
  }
  if (m_saveThread.joinable())
  {
    m_saveThread.join();
  }
}

void Chunk::tick()
{
  // @todo: don't lock if chunk is saving/loading, instead skip tick
  std::lock_guard<std::mutex> lock(m_DataMutex);
  for (auto& entity : m_Data.m_Entities)
  {
    entity.tick(0.f);
  }
  
  m_LastTick = global::tickCount;
}

uint32_t Chunk::getLastTick() const
{
  return m_LastTick;
}
Chunk::Chunk(Chunk& cpy)
{
  this->m_pos = cpy.m_pos;
  this->m_Data = cpy.getData();
}

