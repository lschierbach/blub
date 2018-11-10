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
    
  public:
    PhysicsEntity() { };
    PhysicsEntity(vec2<float> p, vec2<float> s, vec2<float> a, unsigned int id);
    
    virtual void tick() override;
    void physicsTick();
    
    void addForce(game::Force force);
    
    void write(std::ofstream& out) override
    {
      Entity::write(out);
      filesystem::writeStruct(out, m_Mass);
      filesystem::writeStruct(out, m_Velocity);
      filesystem::writeRange(out, m_Forces);
    }
    
    void read(std::ifstream& in) override
    {
      Entity::read(in);
      filesystem::readStruct(in, m_Mass);
      filesystem::readStruct(in, m_Velocity);
      filesystem::readRange(in, m_Forces);
    }
};

#endif /* PHYSICSENTITY_H */
