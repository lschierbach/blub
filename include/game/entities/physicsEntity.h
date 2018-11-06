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
    forceVector m_Forces;
    float m_MaxSpeed;
    
  public:
    PhysicsEntity() { m_MaxSpeed = 4.f; };
    PhysicsEntity(vec2<float> p, vec2<float> s, vec2<float> a);
    
    virtual void tick() override;
    void physicsTick();
    
    void addForce(game::Force force);
    
    void write(std::ofstream& out) override
    {
      Entity::write(out);
      filesystem::writeStruct(out, m_Mass);
      filesystem::writeStruct(out, m_Velocity);
      filesystem::writeStruct(out, m_MaxSpeed);
      filesystem::writeRange(out, m_Forces);
    }
    
    void read(std::ifstream& in) override
    {
      Entity::read(in);
      filesystem::readStruct(in, m_Mass);
      filesystem::readStruct(in, m_Velocity);
      filesystem::readStruct(in, m_MaxSpeed);
      filesystem::readRange(in, m_Forces);
    }
};

#endif /* PHYSICSENTITY_H */
