#ifndef PHYSICSENTITY_H
#define PHYSICSENTITY_H

#include "game/entity.h"
#include "game/vector.hpp"
#include "game/force.hpp"

class PhysicsEntity : public Entity
{
  private:
    using forceVector = std::vector<game::Force>;
    
    float m_Mass;
    game::vec2<float> m_Velocity;
    game::vec2<float> m_Position;
    forceVector m_Forces;
    
  public:
    PhysicsEntity(int p, int q);;
    
    void tick();
    void physicsTick();
    
    void addForce(game::Force force);
};

#endif /* PHYSICSENTITY_H */
