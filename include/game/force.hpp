#ifndef FORCE_HPP
#define FORCE_HPP

#include "game/vector.hpp"
namespace game
{
  struct Force : public Saveable
  {
    Force() {}
    
    Force(game::vec2<float> force, float lifeTime)
    {
      m_Force = force;
      m_Dir = game::math::norm(force);
      m_LifeTime = lifeTime;
    }

    Force(const Force& cpy)
    {
      m_Force = cpy.m_Force;
      m_Dir = cpy.m_Dir;
      m_LifeTime = cpy.m_LifeTime;
    }
    
    Force& operator=(const Force& asgn)
    {
      m_Force = asgn.m_Force;
      m_Dir = asgn.m_Dir;
      m_LifeTime = asgn.m_LifeTime;
      return *this;
    }
    
    Force(const Force&& rhs)
    {
      m_Force = std::move(rhs.m_Force);
      m_Dir = std::move(rhs.m_Dir);
      m_LifeTime = rhs.m_LifeTime;
    }
    
    Force& operator=(const Force&& rhs)
    {
      m_Force = std::move(rhs.m_Force);
      m_Dir = std::move(rhs.m_Dir);
      m_LifeTime = rhs.m_LifeTime;
      return *this;
    }
    
    void write(std::ofstream& out) override
    {
      filesystem::writeStruct(out, m_LifeTime);
      filesystem::writeStruct(out, m_Force);
      filesystem::writeStruct(out, m_Dir);
    }
    
    void read(std::ifstream& in) override
    {
      filesystem::readStruct(in, m_LifeTime);
      filesystem::readStruct(in, m_Force);
      filesystem::readStruct(in, m_Dir);
    }
    
    
    float m_LifeTime;
    game::vec2<float> m_Force;
    game::vec2<float> m_Dir;
  };
}
#endif