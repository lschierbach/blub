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

}

Map::~Map()
{

}

/**
    watches for removed entities and adjusts chunks
*/
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

  #ifdef DEBUG_MAP_PRINT
    print();
  #endif /* DEBUG_MAP_PRINT */
}

void Map::tickChunks()
{
  for (auto& chunkEntry : m_Chunks)
  {
    auto& chunkPtrArray = chunkEntry.second;
    for (auto p = 0u; p < containerLength; p++)
    {
      for (auto q = 0u; q < containerLength; q++)
      {
        if (chunkPtrArray[p][q].get()->getLastTick() != global::tickCount)
        {
          chunkPtrArray[p][q].get()->tick();
        }
      }
    }
  }
}

/**
    Adds an entity to the map and allocates memory for its chunk

    @param entity   the entity to be added to the map
*/
void Map::addEntity(SharedEntityPtr entity)
{

  game::Vector<2, int> entityPos = entity.get()->getChunkPQ();

  // don't add doubled element
  if (m_Chunks.count(entity) > 0)
  {
    return;
  }

  SharedChunkPtrArr chunks;
  for (size_t p = 0; p < containerLength; p++)
  {
    for (size_t q = 0; q < containerLength; q++)
    {
      chunks[p][q] = std::make_shared<Chunk>(entityPos[0] - loadingDistance + p, entityPos[1] - loadingDistance + q);
    }
  }
  m_Chunks.insert(std::make_pair(entity, chunks));

  // delete references so update doesn't get confused with use_count while updating entity
  chunks = { nullptr };

  updateEntity(entity, true);
}

/**
    Removes an entity and its chunk from the map

    @param entity   the entity to be removed
*/
void Map::removeEntity(SharedEntityPtr entity)
{
  auto it = m_Chunks.find(entity);

  // don't try to remove entity which doesn't exist
  if (it == m_Chunks.end())
  {
    return;
  }

  const auto& chunks = it->second;

  for (size_t p = 0; p < containerLength; p++)
  {
    for (size_t q = 0; q < containerLength; q++)
    {
      if (chunks[p][q].use_count() > 1)
      {
        m_unusedChunks.pop();
      }
    }
  }

  m_Chunks.erase(it);
}

/**
    Updates the chunks which should be loaded for each entity and manages entities sharing chunks

    @param  entity    the entity that should be updated
*/
void Map::updateEntity(SharedEntityPtr entity, bool firstUpdate)
{
  auto& chunks = m_Chunks.find(entity)->second;

  SharedChunkPtrArr tempChunks = { nullptr };

  auto entityPos = entity.get()->getChunkPQ();
  auto centerChunkPos = chunks[loadingDistance][loadingDistance].get()->getPos();
  
  // has entity moved? Or is it its first update?
  if (centerChunkPos != entityPos || firstUpdate)
  {
    // get diff
    auto diff = entityPos - centerChunkPos;
    
    // iterate over each coordinate
    for (size_t p = 0; p < containerLength; p++)
    {
      for (size_t q = 0; q < containerLength; q++)
      {
        // get new array-position
        
        auto arrayPosition = game::Vector<2, int> {
          mod(p - diff[0], containerLength),
          mod(q - diff[1], containerLength)
        };

        #ifdef DEBUG_MAP_UPDATE
          printf("[MAP]\tChunk %i, %i, arrayPos %i, %i -> %i, %i\n", centerChunkPos[0], centerChunkPos[1], p, q, arrayPosition[0], arrayPosition[1]);
        #endif

        // iterate over each entity
        for (const auto& it : m_Chunks)
        {
          SharedEntityPtr otherEntity = it.first;
          if (otherEntity != entity)
          {
            auto& otherEntityChunks = m_Chunks.find(otherEntity)->second;
            auto otherEntityCenterPos = otherEntityChunks[loadingDistance][loadingDistance].get()->getPos();
            
            // get chunk position of other entity instead of entitypos, in case both player moved the same frame
            auto otherEntityArrayPosition = arrayPosition + entityPos - otherEntityCenterPos;
            
            
            // check for each dimension if arrayIndex is in bounds
            if (otherEntityArrayPosition[0] > -1 && otherEntityArrayPosition[0] < containerLength &&
                otherEntityArrayPosition[1] > -1 && otherEntityArrayPosition[1] < containerLength)
            {
              
              tempChunks[arrayPosition[0]][arrayPosition[1]] = otherEntityChunks[otherEntityArrayPosition[0]][otherEntityArrayPosition[1]];

              #ifdef DEBUG_MAP_UPDATE
                printf("[MAP]\tCopied chunkptr from other entity\n");
              #endif
              break;
            }
          }
        }

        // if chunk hasn't been found at other entity
        if (tempChunks[arrayPosition[0]][arrayPosition[1]] == nullptr)
        {
          // if other entity uses current chunk, take one from stack
          if (chunks[p][q].use_count() > 1)
          {
            // maybe chunk still lays on stack
            bool foundChunk  = false;
            std::stack<SharedChunkPtr> tempUnusedChunks;

            while(m_unusedChunks.size() > 0)
            {
              // move everything from main-stack to temp-stack
              // and check if chunk searched for lies on it
              if (m_unusedChunks.top()->getPos() == entityPos + arrayPosition - game::Vector<2, int>{ static_cast<int>(loadingDistance), static_cast<int>(loadingDistance) })
              {
                foundChunk = true;
                tempChunks[arrayPosition[0]][arrayPosition[1]] = m_unusedChunks.top();
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
            if (foundChunk)
            {
              #ifdef DEBUG_MAP_UPDATE
                printf("[MAP]\tFound chunk on unused Chunk\n");
              #endif
              continue;
            }

            // if not, take first element from stack and overrite it later..
            chunks[p][q] = m_unusedChunks.top();
            m_unusedChunks.pop();

            #ifdef DEBUG_MAP_UPDATE
              printf("[MAP]\tTook chunk from unused Chunk\n");
            #endif
          }

          // check if arrayIndex is out of bounds
          if (p - diff[0] >= containerLength || p - diff[0] < 0 ||
              q - diff[1] >= containerLength || q - diff[1] < 0)
          {
            // override chunk
            chunks[p][q]->setPos({
              entityPos[0] + (arrayPosition[0] - static_cast<int>(loadingDistance)), 
              entityPos[1] + (arrayPosition[1] - static_cast<int>(loadingDistance)) 
            });

            #ifdef DEBUG_MAP_UPDATE
              printf("[MAP]\tOverride Chunk\n");
            #endif
          }
          // move its pointer
          tempChunks[arrayPosition[0]][arrayPosition[1]] = chunks[p][q];

          #ifdef DEBUG_MAP_UPDATE
            printf("[MAP]\tMove Pointer\n");
          #endif
        }
        // if it has been found, store old chunk
        else
        {
          // but only, if no other entity uses it
          if (chunks[p][q].use_count() < 2)
          {
            m_unusedChunks.push(chunks[p][q]);

            #ifdef DEBUG_MAP_UPDATE
              printf("[MAP]\tPushed old Chunk on unused Chunk\n");
            #endif
          }
        }
      }
    }

    chunks = tempChunks;
  }
}

/**
    Debug tool printing the number of entities referencing to a chunk
*/
void Map::print()
{
  printf("[MAP]\tBEGIN_PRINT\n");

  std::array<std::array<int, 20>, 20> mapUsages;

  for (size_t p = 0; p < 20; p++)
  {
    for (size_t q = 0; q < 20; q++)
    {
      mapUsages[p][q] = 0;
    }
  }

  // iterate over each entity
  for (auto& it : m_Chunks)
  {
    const SharedEntityPtr& entity = it.first;
    SharedChunkPtrArr& chunks = it.second;

    for (size_t p = 0; p < containerLength; p++)
    {
      for (size_t q = 0; q < containerLength; q++)
      {
        if (chunks[p][q]->getP() >= 0 && chunks[p][q]->getP() < 20 && chunks[p][q]->getQ() >= 0 && chunks[p][q]->getQ() < 20)
          mapUsages[chunks[p][q]->getP()][chunks[p][q]->getQ()] = chunks[p][q].use_count();
      }
    }
  }

  for (size_t p = 0; p < 20; p++)
  {
    for (size_t q = 0; q < 20; q++)
    {
      printf(" %u ", mapUsages[q][p]);
    }
    printf("\n");
  }

  printf("stackSize:%u\n", m_unusedChunks.size());

  printf("[MAP]\tEND_PRINT\n");
}

Chunk* Map::getChunk(int relativeP, int relativeQ, SharedEntityPtr entity)
{
  auto& chunks = m_Chunks.find(entity)->second;
  return (chunks[loadingDistance + relativeP][loadingDistance + relativeQ].get());
}

std::vector<Entity> Map::getEntitiesAt(game::vec2<float> pos) 
{
  auto entityVector = std::vector<Entity> {};
  
  return entityVector;
}


size_t Map::getLoadingDistance()
{
    return loadingDistance;
}
