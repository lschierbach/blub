/*
 *  FILENAME:      model.h
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
 */

#ifndef MODEL_H
#define MODEL_H

#include "logic/map.h"


class Model
{
  private:
    Map* m_Map;
    
    void handleMapCollision();
    
  public:
    
    Model();
    ~Model();
    
    void addEntity(std::shared_ptr<Entity> entityPtr, bool track);
    void removeEntity(std::shared_ptr<Entity> entityPtr);
   
    Map* getMap();
    
    void tick();
};

#endif /* MODEL_H */