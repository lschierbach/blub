#include "game/entities/physicsEntity.h"
#include "game/global.h"

PhysicsEntity::PhysicsEntity(vec2<float> p, vec2<float> s, vec2<float> a) : Entity(p, s, a)
{
  m_Mass = 1.f + (s[0] * s[1]);
  m_Velocity = { 0.f, 0.f };
}

void PhysicsEntity::tick()
{
  physicsTick();
  pos += m_Velocity * global::lastTickDuration;
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

  // add friction for next frame
  addForce(game::Force(-5 * m_Velocity, .0f));
}

void PhysicsEntity::addForce(game::Force force)
{
  m_Forces.push_back(force);
}
