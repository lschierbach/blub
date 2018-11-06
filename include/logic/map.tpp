// @todo: remove redundancy

template<typename EntityType>
auto Map::getEntityAt(game::vec2<float> pos) -> EntityType*
{

  std::vector<Chunk*> possibleChunks;
  
  auto topLeftChunkPos     = game::math::entityToChunkPos(pos + game::vec2<float>(-1.f, -1.f));
  auto bottomRightChunkPos = game::math::entityToChunkPos(pos + game::vec2<float>( 1.f,  1.f));
  
  for (auto x = topLeftChunkPos[0]; x <= bottomRightChunkPos[0]; x++)
  {
    for (auto y = topLeftChunkPos[1]; y <= bottomRightChunkPos[1]; y++)
    {
      possibleChunks.push_back(getIdealChunk(game::vec2<int>(x, y)));
    }
  }
  
  for (auto* chunk : possibleChunks)
  {
    if (chunk != nullptr)
    {
      for (auto& entityVariant : chunk->m_Data.m_Entities)
      {
        auto* entity = game::getEntityPtr<EntityType>(entityVariant);

        if (entity != nullptr)
        {
          auto entityPos = entity->getPos(); 
          auto topLeft = entityPos - game::vec2<float>(entity->getSize()[0] * entity->getAnchor()[0], entity->getSize()[1] * entity->getAnchor()[1]);
          auto bottomRight = entityPos + game::vec2<float>(entity->getSize()[0] * entity->getAnchor()[0], entity->getSize()[1] * entity->getAnchor()[1]);

          if(pos[0] >= topLeft[0] && pos[0] <= bottomRight[0] &&
             pos[1] >= topLeft[1] && pos[1] <= bottomRight[1])
          {
            return entity;
          }
        }
      }
    }
  }
  return nullptr;
}

template<typename EntityType>
auto Map::getEntitiesAt(game::vec2<float> pos, float radius) -> std::vector<EntityType*>
{
  auto entityVector = std::vector<EntityType*> {};
  
  std::vector<Chunk*> possibleChunks;
  
  auto topLeftChunkPos     = game::math::entityToChunkPos(pos + game::vec2<float>(-radius, -radius));
  auto bottomRightChunkPos = game::math::entityToChunkPos(pos + game::vec2<float>( radius,  radius));
  
  for (auto x = topLeftChunkPos[0]; x <= bottomRightChunkPos[0]; x++)
  {
    for (auto y = topLeftChunkPos[1]; y <= bottomRightChunkPos[1]; y++)
    {
      possibleChunks.push_back(getIdealChunk(game::vec2<int>(x, y)));
    }
  }
  
  for (auto* chunk : possibleChunks)
  {
    if (chunk != nullptr)
    {
      for (auto& entityVariant : chunk->m_Data.m_Entities)
      {
        auto* entity = game::getEntityPtr<EntityType>(entityVariant);
        
        if (entity != nullptr)
        {
          auto diff = pos - entity->getPos(); 
          if(diff <= vec2<float>(radius, 0.f))
          {
            entityVector.push_back(entity);
          }
        }
      }
    }
  }
  return entityVector;
}
