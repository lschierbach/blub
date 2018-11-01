#ifndef PHYSICSENTITY_H
#define PHYSICSENTITY_H

#include "game/entity.h"
#include "game/vector.hpp"

class PhysicsEntity : public Entity
{
  private:
    game::vec2<float> m_Velocity;
    game::vec2<float> m_Position;
    
  public:
    PhysicsEntity(int p, int q) : Entity(p, q) {};
};

#endif /* PHYSICSENTITY_H */
