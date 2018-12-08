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

#include "game/global.h"
#include "logic/chunk.h"
#include "game/gamemath.hpp"

Chunk::Chunk(int x, int y, Map* map) : m_pos({x, y}), m_Map(map)
{
  reload();
}

Chunk::~Chunk()
{
  joinThreads();
  m_saveThread = std::thread(&Chunk::save, this);
  joinThreads();
}

game::vec2<int> Chunk::getPos() const
{
  return m_pos;
}

void Chunk::setPos(game::vec2<int> pos)
{
  if(pos == this->m_pos)
  {
    return;
  }

  // make sure no one is accessing Data at the moment
  joinThreads();

  // on start no m_Data is in m_tilesets...
  if (!m_Data.m_Tilesets.empty())
  {
    save();
  }

  this->m_pos = pos;

  m_reloadThread = std::thread(&Chunk::reload, this);
}

void Chunk::save()
{
  // get unique path
  std::string path = chunkFolder + std::to_string(m_pos[0]) + "." + std::to_string(m_pos[1]);

  Data temp;

  // copy threadsafe
  {
    std::scoped_lock lock(m_DataMutex);
    temp = m_Data;
  }

  // write m_Data into .tdat file
  filesystem::writeStruct(path + ".tdat", temp);
}

void Chunk::load()
{
  // get unique path
  std::string path = chunkFolder + std::to_string(m_pos[0]) + "." + std::to_string(m_pos[1]);

  Data temp;

  // read m_tilesets from .tdat file
  filesystem::readStruct(path + ".tdat", temp);
  // copy it threadsafe
  {
    std::scoped_lock lock(m_DataMutex);
    m_Data = temp;
  }
}

void Chunk::generate()
{
  // ✝ generator - 20.11.2018
}

void Chunk::reload()
{
  {
    std::scoped_lock lock(m_DataMutex);
    m_Data.m_Tilesets.clear();
    m_Data.m_Entities.clear();
  }
  
  if (filesystem::fileExists(chunkFolder + std::to_string(m_pos[0]) + "." + std::to_string(m_pos[1]) + ".tdat"))
  {
    load();
  }
  else
  {
    generate();

    m_saveThread = std::thread(&Chunk::save, this);
  }
}

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

game::EntityVector Chunk::tick()
{
  game::EntityVector entitiesChangedChunk;
  // @todo: don't lock if chunk is saving/loading, instead skip tick
  
  {
    std::scoped_lock lock(m_DataMutex);
    for (auto it = m_Data.m_Entities.begin(); it != m_Data.m_Entities.end();)
    {
      auto* entity = game::getEntityPtr<Entity>(*it);
      
      entity->tick();
      if (game::math::entityToChunkPos(entity->getPos()) != getPos())
      {
        entitiesChangedChunk.push_back(*it);
        it = m_Data.m_Entities.erase(it);
      }
      else
      {
        it++;
      }
    }
  }
  
  m_LastTick = global::tickCount;
  
  return entitiesChangedChunk;
}

uint32_t Chunk::getLastTick() const
{
  return m_LastTick;
}

void Chunk::lockData()
{
  m_DataMutex.lock();
}

void Chunk::unlockData()
{
  m_DataMutex.unlock();
}
