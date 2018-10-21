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
#endif

#include <thread>   //std::thread
#include <mutex>    //std::mutex

#include <vector>   //std::vector

#include "structs/tile.h"
#include "structs/tileset.h"

class Chunk
{
  std::thread m_saveThread;
  std::thread m_reloadThread;

  void joinThreads();

  using tilesetVector = std::vector<Tileset>;

  void reload();

  void save();
  void load();
  void generate();

  int m_p;
  int m_q;

  tilesetVector m_tilesets;
  Tileset m_tileset;
  std::mutex    m_tilesets_mutex;

  public:

    static const int size = 64;

    Chunk(int p, int q);
    Chunk(Chunk& cpy);
    ~Chunk();

    int getQ() const;
    int getP() const;

    void setPos(int p, int q);

    tilesetVector getTilesets();
};

#endif /* CHUNK_H */
