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
#include "renderer/sprite.h"
#include "renderer/simplesprite.h"

using game::vec2;

class Entity : public Saveable {

  private:

    unsigned int id;
    
  public:
    game::SharedSpritePtr sprite;
    vec2<float> pos; //x y
    vec2<float> size; // w h
    vec2<float> anchor; //fraction of size

    Entity();
    Entity(vec2<float> p, vec2<float> s, vec2<float> a, unsigned int id);
    Entity(int p, int q);
    ~Entity();

    virtual void tick();
    void setPos(const vec2<float>& xy);
    void setX(float x);
    void setY(float y);
    void setSize(const vec2<float>& s);
    void setAnchor(const vec2<float>& a);
    void setSprite(game::SharedSpritePtr s);
    vec2<float> getPos() const;
    vec2<float> getSize() const;
    vec2<float> getAnchor() const;
    unsigned int getId() const;
    void modXY(const vec2<float>& xy);

    void write(std::ofstream& out) override
    {
      filesystem::writeStruct(out, id);
      filesystem::writeStruct(out, pos);
      filesystem::writeStruct(out, size);
      filesystem::writeStruct(out, anchor);
    }
    
    void read(std::ifstream& in) override
    {
      filesystem::readStruct(in, id);
      filesystem::readStruct(in, pos);
      filesystem::readStruct(in, size);
      filesystem::readStruct(in, anchor);
    }
};

#endif /* ENTITY_H */
