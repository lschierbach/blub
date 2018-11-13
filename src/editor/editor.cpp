#include "editor/editor.h"


Editor::Editor(Map* map, Renderer* renderer) : m_Map(map), m_Renderer(renderer)
{
  m_TilesetSelection = {GPU_LoadImage("data/img/testEntity.png"), 0, 0, 1, -1, true};
  m_Renderer->addOverlay(0, &m_TilesetSelection);
}

void Editor::tick()
{

}

void Editor::handleKeyState(const Uint8* keystate)
{
  m_TilesetSelection.visible = false;

  if (keystate[SDL_SCANCODE_SPACE]) m_TilesetSelection.visible = true;
}
