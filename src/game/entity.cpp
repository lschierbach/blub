/*
 *  FILENAME:      entity.cpp
 *
 *  DESCRIPTION:
 *      
 *
 *  NOTES:
 *    
 *
 *  AUTHOR:        Tobias Fey     DATE: 01.10.2018
 *
 *  CHANGES:
 *
 *  TODO:
 *    -implement physic system
 */

#ifdef DEBUG_ENTITY_VERBOSE
    #include <iostream>
#endif

#include <cmath>

#include "game/entity.h"
#include "game/global.h"
#include "game/gamemath.hpp"
#include "logic/chunk.h"

Entity::Entity(vec2<float> p, vec2<float> s, vec2<float> a, unsigned int id)
{
    pos = p;
    size = s;
    anchor = a;
    this->id = id;
}

Entity::Entity(int x, int y)
{
  pos = vec2<float>(
    static_cast<float>(x),
    static_cast<float>(y)
  );
}

Entity::Entity()
{
}

Entity::~Entity()
{
}

void Entity::setPos(const vec2<float>& xy)
{
  pos = xy;
}

void Entity::setX(float x)
{
  pos = vec2<float>(x, pos[1]);
}

void Entity::setY(float y)
{
  pos = vec2<float>(pos[0], y);
}

void Entity::setSize(const vec2<float>& s)
{
  size = s;
}

void Entity::setAnchor(const vec2<float>& a)
{
  anchor = a;
}

void Entity::setSprite(game::SharedSpritePtr s) {
  sprite = s;
}

vec2<float> Entity::getSize() const
{
  return size;
}

vec2<float> Entity::getAnchor() const
{
  return anchor;
}

vec2<float> Entity::getPos() const
{
    return pos;
}

unsigned int Entity::getId() const 
{
  return id;
}


void Entity::tick()
{
  
}

void Entity::modXY(const vec2<float>& xy)
{
    setPos(getPos()+xy);
}
