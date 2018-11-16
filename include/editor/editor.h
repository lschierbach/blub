#ifndef EDITOR_H
#define EDITOR_H

#include "logic/map.h"
#include "renderer/renderer.h"

class Editor
{
  Overlay m_TilesetSelection;
  int m_SelectedTileset;

  game::vec2<int> m_LastChunkPos;
  
  void changeSelectedTileset(int newTileset);
  void addTileset();
  
public:
  Editor(Map* map, Renderer* renderer);
  ~Editor()                         = default;
  Editor(const Editor&)             = delete;
  Editor(Editor&&)                  = delete;
  Editor& operator=(const Editor&)  = delete;
  Editor& operator=(Editor&&)       = delete;

  Map* m_Map;
  Renderer* m_Renderer;



  void tick();

  void handleKeyState(const Uint8* keystate);
};

#endif
