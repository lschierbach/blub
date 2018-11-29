#include "editor/editor.h"
#include <iostream>


Editor::Editor(Map* map, Renderer* renderer) : m_Map(map), m_Renderer(renderer)
{
  m_TilesetSelection = {GPU_LoadImage("data/img/testEntity.png"), 0, 0, -1, 1, true};
  m_Renderer->addOverlay(0, &m_TilesetSelection);
  
  m_LastChunkPos = game::vec2<int> { -100, -100 };
}

void Editor::tick()
{
}

void Editor::handleKeyState(const Uint8* keystate)
{
  if (game::math::entityToChunkPos(m_Renderer->getCamera(0).camera.get()->getPos()) != m_LastChunkPos)
  {
    m_LastChunkPos = game::math::entityToChunkPos(m_Renderer->getCamera(0).camera.get()->getPos());
  }
  m_TilesetSelection.visible = false;

  if (keystate[SDL_SCANCODE_SPACE]) tileSelectionTick();
  else selectingTile = false;
  if (keystate[SDL_SCANCODE_0]) m_SelectedTilesetId = 0u;
  if (keystate[SDL_SCANCODE_1]) m_SelectedTilesetId = 1u;
  if (keystate[SDL_SCANCODE_2]) m_SelectedTilesetId = 2u;
  if (keystate[SDL_SCANCODE_3]) m_SelectedTilesetId = 3u;
  if (keystate[SDL_SCANCODE_4]) m_SelectedTilesetId = 4u;
  if (keystate[SDL_SCANCODE_5]) m_SelectedTilesetId = 5u;
  if (keystate[SDL_SCANCODE_6]) m_SelectedTilesetId = 6u;
  if (keystate[SDL_SCANCODE_7]) m_SelectedTilesetId = 7u;
  if (keystate[SDL_SCANCODE_8]) m_SelectedTilesetId = 8u;
  if (keystate[SDL_SCANCODE_9]) m_SelectedTilesetId = 9u;
  if (keystate[SDL_SCANCODE_N]) addTileset();
  
  
  auto chunkLock = m_Map->getIdealChunk(m_Renderer->getCamera(0).camera.get()->getPos());
  if (chunkLock)
  {
    Chunk* chunk = chunkLock->get();
    m_SelectedTileset = getTilesetById(m_SelectedTilesetId, chunk);
    if (m_SelectedTileset)
    {
      Tileset* tileset = *m_SelectedTileset;
      auto* tilesetImg = m_Renderer->getTilesetImage(tileset->imgName);
      m_TilesetSelection.image = tilesetImg;
    }
    else
    {
      m_TilesetSelection.image = GPU_LoadImage("data/img/testEntity.png");
    }
  }
  
  if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_MIDDLE))
  {
    if (shiftingView)
    {
      game::vec2<float> diff = m_Renderer->pixelToXYAuto(mousePosition) - shiftStartMousPos;

      m_Renderer->getCamera(0).camera.get()->setPos(shiftStartPos - diff);
      shiftStartMousPos = m_Renderer->pixelToXYAuto(mousePosition);
      shiftStartPos = m_Renderer->getCamera(0).camera.get()->getPos();
    }
    else
    {
      shiftingView = true;
      shiftStartMousPos = m_Renderer->pixelToXYAuto(mousePosition);
      shiftStartPos = m_Renderer->getCamera(0).camera.get()->getPos();
    }
  }
  else
  {
    if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT) && !selectingTile)
    {
      if (tileSelectionPos != game::vec2<int> { -1, -1 })
      {
        for (auto x = 0u; x <= tileSelectionSize[0]; x++)
        {
          for (auto y = 0u; y <= tileSelectionSize[1]; y++)
          {
            auto chunkLock = m_Map->getIdealChunk(m_Renderer->pixelToXYAuto(mousePosition) + game::vec2<float> { x, y });
            if (chunkLock)
            {
              Chunk* chunk = chunkLock->get();
              if (chunk->getPos() == m_LastChunkPos)
              {
                auto tilePos = game::vec2<float>{m_Renderer->pixelToXYAuto(mousePosition)[0] + x, m_Renderer->pixelToXYAuto(mousePosition)[1] + y} - game::math::chunkToEntityPos(chunk->getPos());
                auto tileset = getTilesetById(m_SelectedTilesetId, chunk);
                if (tileset)
                {
                  (*tileset)->tileData[static_cast<int>(tilePos[1]) % game::math::chunkSize][static_cast<int>(tilePos[0]) % game::math::chunkSize] = selectedTiles[x][y];
                }
              }
            }
          }
        }
      }
      else if (selectingMapTile)
      {
        auto prev = mapTileSelectionStartPos - game::math::chunkToEntityPos(m_LastChunkPos);
        auto now = game::vec2<float>{m_Renderer->pixelToXYAuto(mousePosition)[0], m_Renderer->pixelToXYAuto(mousePosition)[1]} - game::math::chunkToEntityPos(m_LastChunkPos);
        auto diff = now - prev;

        /*ERSETZEN DURCH RICHTIGE WERTE*/
        m_Renderer->renderBox2(m_Renderer->worldToPixel(0, {0.f,0.f})[0],
                              m_Renderer->worldToPixel(0, {0.f,0.f})[1],
                              m_Renderer->worldToPixel(0, {1.5f,1.5f})[0],
                              m_Renderer->worldToPixel(0, {1.5f,1.5f})[1]);
      }
      else
      {
        mapTileSelectionStartPos = m_Renderer->pixelToXYAuto(mousePosition);
        selectingMapTile = true;
      }
    }
    else if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT) && !selectingTile)
    {
      tileSelectionPos = { -1, -1 };
    }
    shiftingView = false;
  }
}

void Editor::tileSelectionTick() 
{
  m_TilesetSelection.visible = true;
  if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))
  {
    if (selectingTile)
    {
      auto prev = mouseToTilePos(tileSelectionStartPos);
      auto now = mouseToTilePos(mousePosition) + game::vec2<int>{ 1, 1 };
      
      auto diff = now - prev;
      
      auto rectangleCorner = tileToMousePos(prev);
      auto rectangleSize = tileToMousePos(diff);

      float tileSizeF = m_Renderer->getHeight()/16;
      int tileSizePx = static_cast<int>(tileSizeF);
    
      if (rectangleSize[0] <= 0)
      {
        rectangleCorner = {rectangleCorner[0] + rectangleSize[0] - tileSizePx, rectangleCorner[1]};
        rectangleSize = {rectangleSize[0] * -1 + 2*tileSizePx, rectangleSize[1]};
      }
      
      if (rectangleSize[1] <= 0)
      {
        rectangleCorner = {rectangleCorner[0], rectangleCorner[1] + rectangleSize[1] - tileSizePx};
        rectangleSize = {rectangleSize[0], rectangleSize[1] * -1 + 2*tileSizePx};
      }
      
      m_Renderer->renderBox(rectangleCorner[0],
                            rectangleCorner[1],
                            rectangleSize[0],
                            rectangleSize[1]);
      
    }
    else
    {
      tileSelectionStartPos = mousePosition;
      selectingTile = true;
    }
  }
  else
  {
    if (selectingTile)
    {
      auto now = mouseToTilePos(mousePosition);
      
      tileSelectionPos = mouseToTilePos(tileSelectionStartPos);
      tileSelectionSize = now - tileSelectionPos;
      
      if (tileSelectionSize[0] < 0)
      {
        tileSelectionPos = {tileSelectionPos[0] + tileSelectionSize[0], tileSelectionPos[1]};
        tileSelectionSize = {tileSelectionSize[0] * -1, tileSelectionSize[1]};
      }
      
      if (tileSelectionSize[1] < 0)
      {
        tileSelectionPos = {tileSelectionPos[0], tileSelectionPos[1] + tileSelectionSize[1]};
        tileSelectionSize = {tileSelectionSize[0], tileSelectionSize[1] * -1};
      }
      
      selectedTiles.clear();
      
      for (auto x = 0u; x <= tileSelectionSize[0]; x++)
      {
        std::vector<Tile> temp;
        for (auto y = 0u; y <= tileSelectionSize[1]; y++)
        {
          temp.push_back(Tile {tileSelectionPos[0] + x + ( 16 * ( tileSelectionPos[1] + y)), 0.f});
        }
        selectedTiles.push_back(temp);
      }
      
      selectingTile = false;
    }
  }
}

game::vec2<int> Editor::mouseToTilePos(game::vec2<float> mousePos)
{
  //need float version so rounding errors don't stack up
  float tileSizeF = m_Renderer->getHeight()/16;
  int tileSizePx = static_cast<int>(tileSizeF);

  return game::vec2<int>
  {
    //static_cast<int>( ((static_cast<int>(mousePos[0]) + tileSizePx/2) / tileSizePx) * tileSizeF ),
    //static_cast<int>( ((static_cast<int>(mousePos[1]) + tileSizePx/2) / tileSizePx) * tileSizeF )
    static_cast<int>( ((static_cast<int>(mousePos[0])) / tileSizePx)),
    static_cast<int>( ((static_cast<int>(mousePos[1])) / tileSizePx))
  };
}

game::vec2<int> Editor::tileToMousePos(game::vec2<int> tilePos)
{
  float tileSizeF = m_Renderer->getHeight()/16;
  int tileSizePx = static_cast<int>(tileSizeF);

  return game::vec2<int>
  {
    tilePos[0] * tileSizePx,
    tilePos[1] * tileSizePx
  };
}

std::optional<Tileset*> Editor::getTilesetById(unsigned id, Chunk* chunk)
{
  for(auto& tileset : chunk->m_Data.m_Tilesets)
  {
    if (tileset.id == id)
    {
      return { &tileset };
    }
  }
  return {};
}

void Editor::mouseButtonEvent(SDL_MouseButtonEvent& mouseButton) 
{
}

void Editor::mouseMotionEvent(SDL_MouseMotionEvent& mouseMotion) 
{
  mousePosition = { static_cast<float>(mouseMotion.x), static_cast<float>(mouseMotion.y) };
}

void Editor::mouseWheelDirectionEvent(SDL_MouseWheelDirection& mouseWheelDirection) {
}

void Editor::mouseWheelEvent(SDL_MouseWheelEvent& mouseWheel) 
{
  if (!shiftingView)
  {
    if (mouseWheel.y < 0)
    {
      for (auto i = mouseWheel.y; i < 0; i++)
      {
        m_Renderer->zoomCamera(0, 1.2);
      }
    }
    else if (mouseWheel.y > 0)
    {
      for (auto i = mouseWheel.y; i > 0; i--)
      {
        m_Renderer->zoomCamera(0, 0.8);
      }
    }
  }
}


void Editor::addTileset()
{
  if (m_LastTickTilesetChanged)
  {
    m_LastTickTilesetChanged = false;
    return;
  }
  printf("[EDITOR]\t[TILESET]\tPlease pick a tileset you want to add:\n");
  
  auto* tilesetImgs = m_Renderer->getTilesetImgs();
  auto i = 0u;
  
  for (auto& kv : *tilesetImgs)
  {
    printf("\t(%u)\t%s\n", i++, kv.first.c_str());
  }
  
  printf("\t your Selection:");
  scanf("%u", &i);
  
  auto p = 0u;
  auto it = tilesetImgs->begin(); 
  for (;it != tilesetImgs->end() && p != i; it++)
  {
    p++;
  }
  
  if (it == tilesetImgs->end())
  {
    printf("no valid selection.\n");
  }
  else
  {
    auto tilesetImg = it->first;
    
    auto chunkLock = m_Map->getIdealChunk(m_Renderer->getCamera(0).camera.get()->getPos());
    if (chunkLock)
    {
      Chunk* chunk = chunkLock->get();
      
     
      
      std::vector<std::vector<Tile>> tileData;
      
      for(int y = 0; y < game::math::chunkSize; y++)
      {
        std::vector<Tile> temp;
        temp.resize(game::math::chunkSize, Tile(0, .0f));
        tileData.push_back(temp);
      }
      
      
      chunk->m_Data.m_Tilesets.push_back(Tileset(m_Map->getNextTilesetId(), 0.f, 0.f, 1.0f, tilesetImg, tileData));
      m_LastTickTilesetChanged = true;
    }
  }
}

void Editor::addTilesetWithId(unsigned id, Chunk* chunk)
{
  
}