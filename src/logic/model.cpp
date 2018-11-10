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
      auto entityBottomRight = entity.getPos() + game::vec2<float> {
              entity.getSize()[0] * entity.getAnchor()[0], 
              entity.getSize()[1] * entity.getAnchor()[1]
      };
      auto entityTopLeft = entity.getPos() - game::vec2<float> {
        entity.getSize()[0] * entity.getAnchor()[0], 
        entity.getSize()[1] * entity.getAnchor()[1]
      };
      
      auto topLeftChunk = m_Map->getIdealChunk(entityTopLeft);
      auto bottomRightChunk = m_Map->getIdealChunk(entityTopLeft);
      
      auto tilePosTopLeft = topLeftChunk->worldToTilePosition(entityTopLeft);
      auto tilePosBottomRight = bottomRightChunk->worldToTilePosition(entityBottomRight);
      
      if (topLeftChunk->m_Data.m_GameLayer[tilePosTopLeft[0]][tilePosTopLeft[1]] == 1 ||
          bottomRightChunk->m_Data.m_GameLayer[tilePosBottomRight[0]][tilePosBottomRight[1]] == 1)
      {
        printf("collision\n");
      }
    }
  );
}
