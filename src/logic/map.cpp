/*
 *  FILENAME:      map.cpp
 *
 *  AUTHOR:        Leon Schierbach     DATE: 12.09.2018
 *
 *  CHANGES:
 *      LS, 18.09.2018
 *                 updateEntity: before taking the first item on stack m_unusedChunks, it will check whether the needed chunk already lays on it
 *
 *  TODO:
 *    -Add entity loading
 */

#include "logic/map.h"
#include "game/gamemath.hpp"

#include <memory> //std::shared_ptr, std::make_shared
#include <array>  //std::array, std::make_pair
#include <vector> //std::vector
#include <stack>  //std::stack
#include <iostream>

int mod(int a, int b)
{
  return (b + (a % b)) % b;
}

Map::Map() 
{
  if (filesystem::fileExists(m_MapFolder + "data.dat"))
  {
    filesystem::readStruct(m_MapFolder + "data.dat", m_Data);
  }
  else
  {
    init();
  }
}

void Map::init() 
{
  m_Data.m_EntityCount = 1;
}


Map::~Map() 
{
  filesystem::writeStruct(m_MapFolder + "data.dat", m_Data);
}

unsigned int Map::getNextEntityId() 
{
  std::scoped_lock(m_DataMutex);
  return m_Data.m_EntityCount++;
}

unsigned int Map::addNewTileset(const std::string& imgName) 
{
  std::scoped_lock(m_DataMutex);
  m_Data.m_TileSetImgs.push_back(imgName);
  return m_Data.m_TileSetImgs.size() - 1;
}

std::optional<std::string> Map::getTilesetImgName(unsigned id) 
{
  std::scoped_lock(m_DataMutex);
  if (m_Data.m_TileSetImgs.size() > id)
  {
    return { m_Data.m_TileSetImgs[id] };
  }
  else
  {
    return { };
  }
}


void Map::tick()
{
  std::stack<SharedEntityPtr> unusedEntites;

  // tick each entity
  for (const auto& it : m_Chunks)
  {
    const SharedEntityPtr& entity = it.first;
    // if just map references the entity it was destroyed elsewhere and should be removed from the map
    if (entity.use_count() < 2)
    {
      unusedEntites.push(entity);
    }
    else
    {
      updateEntity(entity);
    }
  }

  // remove unused entities
  while(unusedEntites.size() > 0)
  {
    removeEntity(unusedEntites.top());
    unusedEntites.pop();
  }
  
  tickChunks();
}

void Map::tickChunks()
{
  for (auto& chunkEntry : m_Chunks)
  {
    auto& chunkPtrArray = chunkEntry.second;
    for (auto x = 1u; x < containerLength-1; x++)
    {
      for (auto y = 1u; y < containerLength-1; y++)
      {
        if (chunkPtrArray[x][y].get()->getLastTick() != global::tickCount)
        {
          auto entitiesChangedPosition = chunkPtrArray[x][y].get()->tick();
          
          for (auto& entityVariant : entitiesChangedPosition)
          {
            auto* entity = game::getEntityPtr<Entity>(entityVariant);
            auto chunk = getIdealChunk(entity->getPos());
            if (chunk)
            {
              chunk->get()->m_Data.m_Entities.push_back(entityVariant);
            }
          }
        }
      }
    }
  }
}

void Map::addEntity(SharedEntityPtr entity)
{

  auto entityPos = game::math::entityToChunkPos(entity.get()->getPos());

  // don't add doubled element
  if (m_Chunks.count(entity) > 0)
  {
    return;
  }
  
  {
    SharedChunkPtrArr chunks;
    for (auto x = 0u; x < containerLength; x++)
    {
      for (auto y = 0u; y < containerLength; y++)
      {
        chunks[x][y].chunk = std::make_shared<Chunk>(entityPos[0] - loadingDistance + x, entityPos[1] - loadingDistance + y, this);
      }
    }
    m_Chunks.insert(std::make_pair(entity, chunks));

    // delete references so update doesn't get confused with use_count while updating entity
    //chunks = { nullptr };
  }
  updateEntity(entity, true);
}

void Map::removeEntity(SharedEntityPtr entity)
{
  auto it = m_Chunks.find(entity);

  if (it == m_Chunks.end())
  {
    return;
  }

  const auto& chunks = it->second;

  for (auto x = 0u; x < containerLength; x++)
  {
    for (auto y = 0u; y < containerLength; y++)
    {
      if (chunks[x][y].chunk.use_count() > 1)
      {
        m_unusedChunks.pop();
      }
    }
  }

  m_Chunks.erase(it);
}

void Map::updateEntity(SharedEntityPtr entity, bool firstUpdate)
{
  auto& chunks = m_Chunks.find(entity)->second;

  SharedChunkPtrArr tempChunks = { nullptr };

  auto entityPos = game::math::entityToChunkPos(entity.get()->getPos());
  auto centerChunkPos = chunks[loadingDistance][loadingDistance].get()->getPos();
  
  if (centerChunkPos != entityPos || firstUpdate)
  {
    auto diff = entityPos - centerChunkPos;
    
    for (auto x = 0u; x < containerLength; x++)
    {
      for (auto y = 0u; y < containerLength; y++)
      {
        // get new array-position
        
        auto newChunkArrayPosition = game::vec2<int> {
          mod(x - diff[0], containerLength),
          mod(y - diff[1], containerLength)
        };

        for (const auto& it : m_Chunks)
        {
          auto otherEntity = it.first;
          
          if (otherEntity != entity)
          {
            auto& otherEntityChunks = m_Chunks.find(otherEntity)->second;
            auto otherEntityCenterPos = otherEntityChunks[loadingDistance][loadingDistance].get()->getPos();
            
            // get chunk position of other entity instead of entitypos, in case both player moved the same frame
            auto otherEntityChunkArrayPosition = newChunkArrayPosition + entityPos - otherEntityCenterPos;
            
            
            // check for each dimension if arrayIndex is in bounds
            if (otherEntityChunkArrayPosition[0] > -1 && otherEntityChunkArrayPosition[0] < containerLength &&
                otherEntityChunkArrayPosition[1] > -1 && otherEntityChunkArrayPosition[1] < containerLength)
            {
              tempChunks[newChunkArrayPosition[0]][newChunkArrayPosition[1]] = otherEntityChunks[otherEntityChunkArrayPosition[0]][otherEntityChunkArrayPosition[1]];

              break;
            }
          }
        }

        if (tempChunks[newChunkArrayPosition[0]][newChunkArrayPosition[1]].chunk == nullptr)
        {
          // if no other entity uses current chunk, take one from stack
          if (chunks[x][y].chunk.use_count() > 1)
          {
            // maybe chunk still lays on stack
            bool foundInUnusedChunks  = false;
            std::stack<SharedChunkPtr> tempUnusedChunks;

            while(m_unusedChunks.size() > 0)
            {
              // move everything from main-stack to temp-stack
              // and check if chunk searched for lies on it
              if (m_unusedChunks.top()->getPos() == entityPos + newChunkArrayPosition - game::Vector<2, int>{ static_cast<int>(loadingDistance), static_cast<int>(loadingDistance) })
              {
                foundInUnusedChunks = true;
                tempChunks[newChunkArrayPosition[0]][newChunkArrayPosition[1]].chunk = m_unusedChunks.top();
                m_unusedChunks.pop();
                break;
              }
              else
              {
                tempUnusedChunks.push(m_unusedChunks.top());
                m_unusedChunks.pop();
              }
            }

            // move everything back from temp-stack to main-stack
            while(tempUnusedChunks.size() > 0)
            {
              m_unusedChunks.push(tempUnusedChunks.top());
              tempUnusedChunks.pop();
            }
            // already found everything on stack!
            if (foundInUnusedChunks)
            {
              continue;
            }

            // if not, take first element from stack and overrite it later..
            chunks[x][y].chunk = m_unusedChunks.top();
            m_unusedChunks.pop();
          }

          // check if arrayIndex is out of bounds
          if (x - diff[0] >= containerLength || x - diff[0] < 0 ||
              y - diff[1] >= containerLength || y - diff[1] < 0)
          {
            // override chunk
            chunks[x][y].get()->setPos({
              entityPos[0] + (newChunkArrayPosition[0] - static_cast<int>(loadingDistance)), 
              entityPos[1] + (newChunkArrayPosition[1] - static_cast<int>(loadingDistance)) 
            });
          }
          // move its pointer
          tempChunks[newChunkArrayPosition[0]][newChunkArrayPosition[1]] = chunks[x][y];
        }
        // if it has been found, store old chunk
        else
        {
          // but only, if no other entity uses it
          if (chunks[x][y].chunk.use_count() < 2)
          {
            m_unusedChunks.push(chunks[x][y].chunk);
          }
        }
      }
    }

    chunks = tempChunks;
  }
}

std::optional<Map::ScopedChunkLock>  Map::getIdealChunk(game::vec2<float> pos) 
{
  auto chunkPos = game::math::entityToChunkPos(pos);
  
  return getIdealChunk(chunkPos);
}

std::optional<Map::ScopedChunkLock>  Map::getIdealChunk(game::vec2<int> pos) 
{
  for (auto& chunkEntry : m_Chunks)
  {
    auto& chunks = chunkEntry.second;
    auto chunksCenter = chunks[loadingDistance][loadingDistance].get()->getPos();
    
    // check bounds
    if (pos[0] >= chunksCenter[0] - static_cast<int>(loadingDistance-1) && pos[0] <= chunksCenter[0] + static_cast<int>(loadingDistance-1) &&
        pos[1] >= chunksCenter[1] - static_cast<int>(loadingDistance-1) && pos[1] <= chunksCenter[1] + static_cast<int>(loadingDistance-1))
    {
      return ScopedChunkLock { chunks[loadingDistance + (pos[0] - chunksCenter[0])][loadingDistance + (pos[1] - chunksCenter[1])] };
    }
  }
  
  return { };
}

char Map::getGamelayerIdAt(game::vec2<float> pos)
{
  auto chunkLock = getIdealChunk(pos);

  if (chunkLock)
  {
    auto* chunk = chunkLock->get();
    auto tilePos = pos - game::math::chunkToEntityPos(chunk->getPos());

    auto result = chunkLock->get()->m_Data.m_GameLayer[static_cast<int>(tilePos[0])][static_cast<int>(tilePos[1])];

    return result;
  }
  
  return 0;
}

size_t Map::getLoadingDistance()
{
    return loadingDistance;
}
