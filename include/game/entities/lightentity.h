#ifndef LIGHTENTITY_H
#define LIGHTENTITY_H

#include "game/entity.h"

class LightEntity: public Entity
{
  private:
    float radius;
    game::Vector<3,float> color;
  public:
    LightEntity();
    LightEntity(vec2<float> p, game::Vector<3, float> c, float r, unsigned int id):
      Entity::Entity(p, {0.f,0.f}, {0.5f,0.5f}, id)
    {
      radius = r;
      color = c;
    }
    float getRadius();
    void setRadius(float r);
    game::Vector<3,float> getColor();
    void setColor(game::Vector<3,float> c);

    void write(std::ofstream& out) override
    {
      Entity::write(out);
      filesystem::writeStruct(out, color);
      filesystem::writeStruct(out, radius);
    }

    void read(std::ifstream& in) override
    {
      Entity::read(in);
      filesystem::readStruct(in, color);
      filesystem::readStruct(in, radius);
    }
};

#endif /* LIGHTENTITY_H */
