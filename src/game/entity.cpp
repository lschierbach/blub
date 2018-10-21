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

Entity::Entity(vec2 p, vec2 s, vec2 a)
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

void Entity::setPQ(const vec2& pq)
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

void Entity::setXY(const vec2& xy)
{
    setPQ(cartesianToAxial(xy));
}

void Entity::setX(float x)
{
    setPQ(cartesianToAxial(vec2(x, getXY()[1])));
}

void Entity::setY(float y)
{
    setPQ(cartesianToAxial(vec2(getXY()[0], y)));
}

void Entity::setSize(const vec2& s)
{
    size = s;
}

void Entity::setAnchor(const vec2& a)
{
    anchor = a;
}

vec2 Entity::getSize() const
{
    return size;
}

vec2 Entity::getAnchor() const
{
    return anchor;
}

void Entity::tick(float tickTime)
{

}

vec2 Entity::cartesianToAxial(const vec2& cartesian)
{
    #ifdef DEBUG_ENTITY_VERBOSE
        std::cout << "[ENTITY] making axial from cartesian " << cartesian[0] << "|" << cartesian[1] << std::endl;
    #endif
    return (cartesian[0] * game::math::pVector + cartesian[1] * game::math::qVector);
}

vec2 Entity::axialToCartesian(const vec2& axial)
{
    return (axial[0] * game::math::xVector + axial[1] * game::math::yVector);
}

vec2 Entity::getPQ() const
{
    return {p, q};
}

vec2 Entity::getXY() const
{
    return axialToCartesian(vec2(p,q));
}

game::Vector<2, int> Entity::getChunkPQ() const {
    return game::Vector<2, int>(static_cast<int>((p/Chunk::size)), static_cast<int>(q/Chunk::size));
}

void Entity::modXY(const vec2& xy)
{
    setXY(getXY()+xy);
}

void Entity::modPQ(const vec2& pq)

{
    setPQ(getPQ()+pq);
}
