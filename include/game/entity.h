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

using vec2 = game::Vector<2>;

namespace game::math {
  const static float hexWidth = 0.86602540378f;
  const static float hexHeight = 1.f;
  const static float hexPointInset = 0.25f;

  /*
   * xVector: width of hex, 0
   * yVector: width / 2, height - point inset
   * pVector/qVector: inverse matrix of p&q
   */
  const static vec2 pVector{1.154700538f, 0.f};              // x -> p
  const static vec2 qVector{-0.666666666f, 1.33333333333f}; // y -> q
  const static vec2 xVector{0.86602540378f, 0.f};            // p -> x
  const static vec2 yVector{0.433012702f, 0.75f};           // q -> y

}

class Entity {
    private:
        vec2 size; // w h
        vec2 anchor; //fraction of size

        //Basis vectors, used for conversion axial <-> cartesian

    public:
        float p;
        float q;

        Entity();
        Entity(vec2 p, vec2 s, vec2 a);
        Entity(int p, int q);
        ~Entity();

        static vec2 axialToCartesian(const vec2& axial);
        static vec2 cartesianToAxial(const vec2& cartesian);

        void tick(float tickTime);
        void setPQ(const vec2& pq);
        void setP(float p);
        void setQ(float q);
        void setXY(const vec2& xy);
        void setX(float x);
        void setY(float y);
        void setSize(const vec2& s);
        void setAnchor(const vec2& a);
        vec2 getPQ() const;
        vec2 getXY() const;
        vec2 getSize() const;
        vec2 getAnchor() const;
        game::Vector<2, int> getChunkPQ() const;
        void modXY(const vec2& xy);
        void modPQ(const vec2& pq);
};

#endif /* ENTITY_H */
