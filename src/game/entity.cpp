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
#include "logic/chunk.h"
#include "game/global.h"

Entity::Entity(vec2<float> p, vec2<float> s, vec2<float> a)
{
    this->p = p[0];
    this->q = p[1];
    size = s;
    anchor = a;
}

Entity::Entity(int p, int q) {
    this->p = (float)p;
    this->q = (float)q;
}

Entity::Entity()
{
}

Entity::~Entity()
{
}

void Entity::setPQ(const vec2<float>& pq)
{
    #ifdef DEBUG_ENTITY_VERBOSE
        std::cout << "[ENTITY] set axial " << pq[0] << "|" << pq[1] << std::endl;
    #endif
    this->p = pq[0];
    this->q = pq[1];
}

void Entity::setP(float p)
{
    this->p = p;
}

void Entity::setQ(float q)
{
    this->q = q;
}

void Entity::setXY(const vec2<float>& xy)
{
    setPQ(cartesianToAxial(xy));
}

void Entity::setX(float x)
{
    setPQ(cartesianToAxial(vec2<float>(x, getXY()[1])));
}

void Entity::setY(float y)
{
    setPQ(cartesianToAxial(vec2<float>(getXY()[0], y)));
}

void Entity::setSize(const vec2<float>& s)
{
    size = s;
}

void Entity::setAnchor(const vec2<float>& a)
{
    anchor = a;
}

vec2<float> Entity::getSize() const
{
    return size;
}

vec2<float> Entity::getAnchor() const
{
    return anchor;
}

void Entity::tick(float tickTime)
{
  //printf("tick %lu\n", global::tickCount);
}

vec2<float> Entity::cartesianToAxial(const vec2<float>& cartesian)
{
    #ifdef DEBUG_ENTITY_VERBOSE
        std::cout << "[ENTITY] making axial from cartesian " << cartesian[0] << "|" << cartesian[1] << std::endl;
    #endif
    return (cartesian[0] * game::math::pVector + cartesian[1] * game::math::qVector);
}

vec2<float> Entity::axialToCartesian(const vec2<float>& axial)
{
    return (axial[0] * game::math::xVector + axial[1] * game::math::yVector);
}

vec2<float> Entity::getPQ() const
{
    return {p, q};
}

vec2<float> Entity::getXY() const
{
    return axialToCartesian(vec2<float>(p,q));
}

vec2<int> Entity::getChunkPQ() const {
    return game::Vector<2, int>(static_cast<int>((p/Chunk::size)), static_cast<int>(q/Chunk::size));
}

void Entity::modXY(const vec2<float>& xy)
{
    setXY(getXY()+xy);
}

void Entity::modPQ(const vec2<float>& pq)

{
    setPQ(getPQ()+pq);
}
