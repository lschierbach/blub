#include "editor/editor.h"


Editor::Editor(Map* map, Renderer* renderer) : m_Map(map), m_Renderer(renderer)
{
  m_TilesetSelection = {GPU_LoadImage("data/img/testEntity.png"), 0, 0, 1, - m_Renderer->getHeight() / m_Renderer->getWidth(), true};
  m_Renderer->addOverlay(0, &m_TilesetSelection);
  
  m_SelectedTileset = -1;
  m_LastChunkPos = game::vec2<int> { -100, -100 };
}

void Editor::tick()
{
}

void Editor::handleKeyState(const Uint8* keystate)
{
  if (game::math::entityToChunkPos(m_Renderer->getCamera(0).camera.get()->getPos()) != m_LastChunkPos)
  {
    changeSelectedTileset(0);
    m_LastChunkPos = game::math::entityToChunkPos(m_Renderer->getCamera(0).camera.get()->getPos());
  }
  m_TilesetSelection.visible = false;

  if (keystate[SDL_SCANCODE_SPACE]) tileSelectionTick();
  if (keystate[SDL_SCANCODE_0]) changeSelectedTileset(0);
  if (keystate[SDL_SCANCODE_1]) changeSelectedTileset(1);
  if (keystate[SDL_SCANCODE_2]) changeSelectedTileset(2);
  if (keystate[SDL_SCANCODE_3]) changeSelectedTileset(3);
  if (keystate[SDL_SCANCODE_N]) addTileset();
  
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
      auto now = mouseToTilePos(mousePosition);
      auto diff = prev - now;
      
      m_Renderer->renderBox(prev[0] * static_cast<int>(m_Renderer->getWidth() / 16), 
                            prev[1] * static_cast<int>(m_Renderer->getWidth() / 16), 
                            diff[0] * static_cast<int>(m_Renderer->getWidth() / 16), 
                            diff[1] * static_cast<int>(m_Renderer->getWidth() / 16));
      
    }
    else
    {
      tileSelectionStartPos = mousePosition;
      selectingTile = true;
    }
  }
  else
  {
    selectingTile = false;
  }
}

game::vec2<int> Editor::mouseToTilePos(game::vec2<float> mousePos)
{
  return game::vec2<int>
  {
    static_cast<int>(mousePos[0]) % static_cast<int>(m_Renderer->getWidth() / 16),
    static_cast<int>(mousePos[1]) % static_cast<int>(m_Renderer->getWidth() / 16)
  };
}

void Editor::changeSelectedTileset(int newTileset) 
{
  if (newTileset == m_SelectedTileset)
  {
    return;
  }
  auto chunkLock = m_Map->getIdealChunk(m_Renderer->getCamera(0).camera.get()->getPos());
  if (chunkLock)
  {
    Chunk* chunk = chunkLock->get();
    if (chunk->m_Data.m_Tilesets.size() - 1 < newTileset)
    {
      printf("[EDITOR]\t[ERR]\tPlease pick a Tileset between 0 and %u.\n", chunk->m_Data.m_Tilesets.size() - 1);
    }
    else
    {      
      auto* tilesetImg = m_Renderer->getTilesetImage(chunk->m_Data.m_Tilesets[newTileset].imgName);
      
      m_TilesetSelection.image = tilesetImg;
      
      m_SelectedTileset = newTileset;
    }
  }
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
      
      
      chunk->m_Data.m_Tilesets.push_back(Tileset(0.f, 0.f, 1.0f, tilesetImg, tileData));
      m_LastTickTilesetChanged = true;
    }
  }
}
