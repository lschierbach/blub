#ifndef FORCE_HPP
#define FORCE_HPP

#include "game/vector.hpp"

struct Force
{
  Force(game::Vector<float> force, float lifeTime)
  {
    m_Force = force;
    m_Dir = game::math::norm(force);
    m_LifeTime = lifeTime;
  }
  
  float m_LifeTime;
  game::vec2<float> m_Force;
  game::vec2<float> m_Dir;
};

#endif