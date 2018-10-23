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
#include "logic/chunk.h"

namespace game::math {
  const static float hexWidth = 0.86602540378f;
  const static float hexHeight = 1.f;
  const static float hexPointInset = 0.25f;

  /*
   * xVector: width of hex, 0
   * yVector: width / 2, height - point inset
   * pVector/qVector: inverse matrix of p&q
   */
  const static vec2<float> pVector{1.154700538f, 0.f};              // x -> p
  const static vec2<float> qVector{-0.666666666f, 1.33333333333f}; // y -> q
  const static vec2<float> xVector{0.86602540378f, 0.f};            // p -> x
  const static vec2<float> yVector{0.433012702f, 0.75f};           // q -> y

}

using game::vec2;

class Entity {

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

    static vec2<float> axialToCartesian(const vec2<float>& axial);
    static vec2<float> cartesianToAxial(const vec2<float>& cartesian);

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
};

#endif /* ENTITY_H */
