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
}