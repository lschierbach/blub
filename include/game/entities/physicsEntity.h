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
    PhysicsEntity() {};
    PhysicsEntity(vec2<float> p, vec2<float> s, vec2<float> a);
    
    void tick();
    void physicsTick();
    
    void addForce(game::Force force);
    
    void write(std::ofstream& out) override
    {
      filesystem::writeStruct(out, m_Position);
      filesystem::writeStruct(out, size);
      filesystem::writeStruct(out, anchor);
    }
    
    void read(std::ifstream& in) override
    {
      filesystem::readStruct(in, m_Position);
      filesystem::readStruct(in, size);
      filesystem::readStruct(in, anchor);
    }
};

#endif /* PHYSICSENTITY_H */
