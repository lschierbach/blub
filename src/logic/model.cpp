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
  
  handleMapCollision();
}

void Model::handleMapCollision() 
{ 
  m_Map->for_each_entity<PhysicsEntity>
  (
    [&](auto&& entity) -> void
    {
      auto entityTopLeft = entity.getPos() + game::vec2<float> {
        -entity.getSize()[0] * entity.getAnchor()[0], 
        -entity.getSize()[1] * entity.getAnchor()[1]
      };
      auto entityTopRight = entity.getPos() + game::vec2<float> {
        entity.getSize()[0] * entity.getAnchor()[0], 
        -entity.getSize()[1] * entity.getAnchor()[1]
      };
      auto entityBottomLeft = entity.getPos() + game::vec2<float> {
        -entity.getSize()[0] * entity.getAnchor()[0], 
        entity.getSize()[1] * entity.getAnchor()[1]
      };
      auto entityBottomRight = entity.getPos() + game::vec2<float> {
        entity.getSize()[0] * entity.getAnchor()[0], 
        entity.getSize()[1] * entity.getAnchor()[1]
      };
      
      bool topLeftCollision = m_Map->getGamelayerIdAt(entityTopLeft) == 1;
      bool topRightCollision = m_Map->getGamelayerIdAt(entityTopRight) == 1;
      bool bottomLeftCollision = m_Map->getGamelayerIdAt(entityBottomLeft) == 1;
      bool bottomRightCollision = m_Map->getGamelayerIdAt(entityBottomRight) == 1;
      
      if (topLeftCollision || topRightCollision || bottomLeftCollision || bottomRightCollision)
      {
        
      }
    }
  );
}
