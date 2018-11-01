#include "game/entities/physicsEntity.h"
#include "game/global.h"

PhysicsEntity::PhysicsEntity(int p, int q) : Entity(p, q)
{
  
}

void PhysicsEntity::tick()
{
  physicsTick();
  m_Position += m_Velocity * global::lastTickDuration;
}

void PhysicsEntity::physicsTick()
{
  game::vec2<float> acceleration { .0f, .0f };
  
  for (auto it = m_Forces.begin(); it != m_Forces.end();)
  {
    acceleration += it->m_Force;
    it->m_LifeTime -= global::lastTickDuration;
    
    if (it->m_LifeTime <= 0)
    {
      it = m_Forces.erase(it);
    }
    else
    {
      it++;
    }
  }
  m_Velocity += (acceleration * global::lastTickDuration / m_Mass);
}

void PhysicsEntity::addForce(game::Force force)
{
  m_Forces.push_back(force);
}
