#ifndef GAMEMATH_H
#define GAMEMATH_H

#include "game/vector.hpp"
#include "entity.h"
#include "entities/physicsEntity.h"
#include <variant>

namespace game 
{
  using EntityVariant = std::variant<Entity, PhysicsEntity>;
  
  using EntityVector = std::vector<EntityVariant>;
  
  template<typename EntityType>
  auto getEntityPtr(EntityVariant& variant) -> EntityType*
  {
    return std::visit([](auto&& arg) -> EntityType*
    {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_same_v<T, EntityType>)
      {
        return &arg;
      }
      else if constexpr (std::is_base_of_v<EntityType, T>)
      {
        return (EntityType*)&arg;
      }
      return nullptr;
    }, variant);
  }

  namespace math
  {
    static const int chunkSize = 32;

    static const float tileHeight = 1.f;
    static const float tileWidth  = 1.f;

    static vec2<float> chunkToEntityPos(const vec2<int>& chunk)
    {
      return vec2<float>(
          static_cast<float>(chunkSize*chunk[0]),
          static_cast<float>(chunkSize*chunk[1])
      );
    }

    static vec2<int> entityToChunkPos(const vec2<float>& entity)
    {
      return vec2<int>(
        static_cast<int>(floor(entity[0]/chunkSize)),
        static_cast<int>(floor(entity[1]/chunkSize))
      );
    }

    static float chunkToEntityX(float entityX) {
      return entityX*chunkSize;
    }

    static float chunkToEntityY(float entityY) {
      return entityY*chunkSize;
    }

    static int entityToChunkX(float chunkX) {
      return static_cast<int>(floor(chunkX/chunkSize));
    }

    static int entityToChunkY(float chunkY) {
      return static_cast<int>(floor(chunkY/chunkSize));
    }
  }
}

#endif /* GAMEMATH_H */
