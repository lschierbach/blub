/*
 *  FILENAME:      Entity.h
 *
 *  DESCRIPTION:
 *      Entity base class providing common functionality
 *
 *  AUTHOR:         Tobias Fey      DATE: 01.10.2018
 *
 *  DEBUG:          DEBUG_ENTITY
 *                  DEBUG_ENTITY_VERBOSE
 *
 */

#ifndef ENTITY_H
#define ENTITY_H

 #ifdef DEBUG_ENTITY
    #define DEBUG_ENTITY_VERBOSE
#endif

#include <cmath>

#include "game/vector.hpp"
#include "game/filesystem.hpp"
#include "game/saveable.h"

using game::vec2;

class Entity : public Saveable {

  private:
    vec2<float> size; // w h
    vec2<float> anchor; //fraction of size

  public:
    float p;
    float q;

    Entity();
    Entity(vec2<float> p, vec2<float> s, vec2<float> a);
    Entity(int p, int q);
    ~Entity();

    void tick(float tickTime);
    void setPQ(const vec2<float>& pq);
    void setP(float p);
    void setQ(float q);
    void setXY(const vec2<float>& xy);
    void setX(float x);
    void setY(float y);
    void setSize(const vec2<float>& s);
    void setAnchor(const vec2<float>& a);
    vec2<float> getPQ() const;
    vec2<float> getXY() const;
    vec2<float> getSize() const;
    vec2<float> getAnchor() const;
    vec2<int> getChunkPQ() const;
    void modXY(const vec2<float>& xy);
    void modPQ(const vec2<float>& pq);
    
    void write(std::ofstream& out) override
    {
      filesystem::writeStruct(out, p);
      filesystem::writeStruct(out, q);
      filesystem::writeStruct(out, size);
      filesystem::writeStruct(out, anchor);
    }
    
    void read(std::ifstream& in) override
    {
      filesystem::readStruct(in, p);
      filesystem::readStruct(in, q);
      filesystem::readStruct(in, size);
      filesystem::readStruct(in, anchor);
    }
};

#endif /* ENTITY_H */
