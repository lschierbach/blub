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

#include "logic/chunk.h"
#include "game/filesystem.hpp"
#include "game/generator.hpp"

Chunk::Chunk(int p, int q)
{
  this->m_p = p;
  this->m_q = q;
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
  return m_p;
}

/**
    returns q
*/
int Chunk::getQ() const
{
  return m_q;
}

/**
    returns m_tilesets threadsafe
*/
Chunk::tilesetVector Chunk::getTilesets()
{
  std::lock_guard<std::mutex> lock(m_tilesets_mutex);
  return m_tilesets;
}

/**
    Changes position of chunk on the world.
    If the new position doesn't equal the old, the chunk will save its current data and load/generate new.

    @param p    p
    @param q    q
*/
void Chunk::setPos(int p, int q)
{
  // don't reload if position hasn't changed
  if(p == this->m_p && q == this->m_q)
  {
    return;
  }

  // make sure no one is accessing tilesetdata at the moment
  joinThreads();

  // on start no data is in m_tilesets...
  if (!m_tilesets.empty())
  {
    save();
  }


  this->m_p = p;
  this->m_q = q;

  m_reloadThread = std::thread(&Chunk::reload, this);
}

/**
    Saves the data of the tilesets on the disk
*/
void Chunk::save()
{
  #ifdef DEBUG_CHUNK_RELOAD
    printf("[CHUNK]SAVE\n");
  #endif /* DEBUG_CHUNK_RELOAD */

  #ifdef DEBUG_CHUNK_TILESET
    for(const auto& set : m_tilesets)
    {
      std::cout << "Tileset\t" << set.imgName << std::endl;
      set.printTiles();
    }
  #endif /* DEBUG_CHUNK_TILESET */


  // get unique path
  std::string path = "data/chunks/" + std::to_string(m_p) + "." + std::to_string(m_q);

  tilesetVector temp;

  // copy threadsafe
  std::unique_lock<std::mutex> lock(m_tilesets_mutex);

  temp = m_tilesets;

  lock.unlock();

  // write data into .tdat file
  filesystem::writeRange(path + ".tdat", temp);
}

/**
    Loads data from the disk, depenting on the position, into its tilesets
*/
void Chunk::load()
{
  // get unique path
  std::string path = "chunks/" + std::to_string(m_p) + "." + std::to_string(m_q);

  tilesetVector temp;

  // read m_tilesets from .tdat file
  filesystem::readRange(path + ".tdat", temp);

  // copy it threadsafe
  std::lock_guard<std::mutex> lock(m_tilesets_mutex);

  m_tilesets = temp;


  #ifdef DEBUG_CHUNK_RELOAD
    printf("[CHUNK]\tLOAD\n");
  #endif /* DEBUG_CHUNK_RELOAD */

  #ifdef DEBUG_CHUNK_TILESET
  for(const auto& set : m_tilesets)
  {
    std::cout << "Tileset\t" << set.imgName << std::endl;
    set.printTiles();
  }
  #endif /* DEBUG_CHUNK_TILESET */
}

/**
    Generates a new chunk
*/
void Chunk::generate()
{
  // dummy data
  std::lock_guard<std::mutex> lock(m_tilesets_mutex);

  generator::generateChunk(m_tilesets, m_p, m_q, size);

  #ifdef DEBUG_CHUNK_RELOAD
    printf("[CHUNK]GENERATE\n");
  #endif /* DEBUG_CHUNK_RELOAD */

}

/**
    clears tilesets and overwrites it
*/
void Chunk::reload()
{
  // remove old tileset data
  m_tilesets.clear();

  // if chunk-data exists
  if (filesystem::fileExists("chunks/" + std::to_string(m_p) + "." + std::to_string(m_q) + ".tdat"))
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

Chunk::Chunk(Chunk& cpy)
{
  this->m_p = cpy.m_p;
  this->m_q = cpy.m_q;

  this->m_tilesets = cpy.getTilesets();
}

