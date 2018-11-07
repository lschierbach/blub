template<typename EntityType>
auto Map::get_entity_at(game::vec2<float> pos) -> EntityType*
{
  auto topLeftChunkPos     = game::math::entityToChunkPos(pos + game::vec2<float>(-1.f, -1.f));
  auto bottomRightChunkPos = game::math::entityToChunkPos(pos + game::vec2<float>( 1.f,  1.f));
  
  for (auto x = topLeftChunkPos[0]; x <= bottomRightChunkPos[0]; x++)
  {
    for (auto y = topLeftChunkPos[1]; y <= bottomRightChunkPos[1]; y++)
    {
      auto* chunk = getIdealChunk(game::vec2<int>(x, y));
      if (chunk != nullptr)
      {
        EntityType* result = game::find_in_variant_by_type<EntityType>(chunk->m_Data.m_Entities, 
          [&](auto &entity) -> bool
          {
            auto entityPos = entity.getPos(); 
            auto topLeft = entityPos - game::vec2<float>(entity.getSize()[0] * entity.getAnchor()[0], entity.getSize()[1] * entity.getAnchor()[1]);
            auto bottomRight = entityPos + game::vec2<float>(entity.getSize()[0] * entity.getAnchor()[0], entity.getSize()[1] * entity.getAnchor()[1]);

            return (pos[0] >= topLeft[0] && pos[0] <= bottomRight[0] &&
                    pos[1] >= topLeft[1] && pos[1] <= bottomRight[1]);
          }
        );
        if (result != nullptr)
        {
          return result;
        }
      }
    }
  }
  
  return nullptr;
}

template<typename EntityType, typename Lambda>
auto Map::for_each_entity_in_range(game::vec2<float> pos, float radius, Lambda&& lam) -> void
{
  auto radiusVec = game::vec2<float>(radius, 0.f);
  
  auto topLeftChunkPos     = game::math::entityToChunkPos(pos + game::vec2<float>(-radius, -radius));
  auto bottomRightChunkPos = game::math::entityToChunkPos(pos + game::vec2<float>( radius,  radius));
  
  for (auto x = topLeftChunkPos[0]; x <= bottomRightChunkPos[0]; x++)
  {
    for (auto y = topLeftChunkPos[1]; y <= bottomRightChunkPos[1]; y++)
    {
      auto* chunk = getIdealChunk(game::vec2<int>(x, y));
      if (chunk != nullptr)
      {
        game::for_each_variant_by_type<EntityType>(chunk->m_Data.m_Entities, 
          [&](auto& entity) -> void
          {
            if (pos - entity.getPos() < radiusVec)
            {
              lam(entity);
            }
          }
        );
      }
    }
  }
}

template<typename EntityType, typename Lambda>
auto Map::for_each_entity_in_box(game::vec2<float> boxTopLeft, game::vec2<float> size, Lambda&& lam) -> void
{
  auto topLeftChunkPos     = game::math::entityToChunkPos(boxTopLeft);
  auto bottomRightChunkPos = game::math::entityToChunkPos(boxTopLeft + size);
  
  auto boxBottomRight = boxTopLeft + size;
  
  for (auto x = topLeftChunkPos[0]; x <= bottomRightChunkPos[0]; x++)
  {
    for (auto y = topLeftChunkPos[1]; y <= bottomRightChunkPos[1]; y++)
    {
      auto* chunk = getIdealChunk(game::vec2<int>(x, y));
      if (chunk != nullptr)
      {
        game::for_each_variant_by_type<EntityType>(chunk->m_Data.m_Entities, 
          [&](auto& entity) -> void
          {
            auto entityBottomRight = entity.getPos() + game::vec2<float> {
              entity.getSize()[0] * entity.getAnchor()[0], 
              entity.getSize()[1] * entity.getAnchor()[1]
            };
            auto entityTopLeft = entity.getPos() - game::vec2<float> {
              entity.getSize()[0] * entity.getAnchor()[0], 
              entity.getSize()[1] * entity.getAnchor()[1]
            };
            
            if (entityTopLeft[0] >= boxTopLeft[0] && entityBottomRight[0] <= boxBottomRight[0] &&
                entityTopLeft[1] >= boxTopLeft[1] && entityBottomRight[1] <= boxBottomRight[1])
            {
              lam(entity);
            }
          }
        );
      }
    }
  }
}
