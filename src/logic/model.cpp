/*
 *  FILENAME:      model.cpp
 *
 *  DESCRIPTION:
 *      
 *
 *  NOTES:
 *    
 *
 *  AUTHOR:        Leon Schierbach     DATE: 21.10.2018
 *
 *  CHANGES:
 *
 *  TODO:
 *    -implement entities
 */

#include <iostream>

#include "logic/model.h"

Model::Model()
{
  m_Map = new Map();
}

Model::~Model()
{
  delete m_Map;
}

void Model::addEntity(std::shared_ptr<Entity> entityPtr, bool track = false)
{
  if (track)
  {
    m_Map->addEntity(entityPtr);
  }
}

void Model::removeEntity(std::shared_ptr<Entity> entityPtr)
{
  m_Map->removeEntity(entityPtr);
}

Map* Model::getMap()
{
  return m_Map;
}

void Model::tick()
{
  m_Map->tick();
  
  // handle collision
  m_Map->for_each_entity<PhysicsEntity>
  (
    [&](auto&& entity) -> void
    {
      auto entityTopLeft = entity.getPos() - game::vec2<float> {
        entity.getSize()[0] * entity.getAnchor()[0], 
        entity.getSize()[1] * entity.getAnchor()[1]
      };
      auto entityBottomRight = entityTopLeft + entity.getSize();
      std::cout << entity.getSize()[0] << std::endl;
      
      auto topLeftChunk = m_Map->getIdealChunk(entityTopLeft);
      auto bottomRightChunk = m_Map->getIdealChunk(entityBottomRight);
      
      //auto tilePosTopLeft = topLeftChunk->worldToTilePosition(entityTopLeft);
      //auto tilePosBottomRight = bottomRightChunk->worldToTilePosition(entityBottomRight);

      //get position *inside* chunk (make gamemath/chunk method?)
      auto tilePosTopLeftF = entityTopLeft - game::math::chunkToEntityPos(topLeftChunk->getPos());
      auto tilePosBottomRightF = entityBottomRight - game::math::chunkToEntityPos(bottomRightChunk->getPos());

      //convert to integer (needed?)
      auto tilePosTopLeftI = game::vec2<int>(
        static_cast<int>(floor(tilePosTopLeftF[0])),
        static_cast<int>(floor(tilePosTopLeftF[1]))
      );
      auto tilePosBottomRightI = game::vec2<int>(
        static_cast<int>(floor(tilePosBottomRightF[0])),
        static_cast<int>(floor(tilePosBottomRightF[1]))
      );
      
      if (topLeftChunk->m_Data.m_GameLayer[tilePosTopLeftI[0]][tilePosTopLeftI[1]] == 1 ||
          bottomRightChunk->m_Data.m_GameLayer[tilePosBottomRightI[0]][tilePosBottomRightI[1]] == 1)
      {
        printf("collision\n");
      }
    }
  );
}
