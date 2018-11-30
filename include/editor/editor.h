#ifndef EDITOR_H
#define EDITOR_H

#include <optional>
#include "logic/map.h"
#include "renderer/renderer.h"

class Editor
{
  game::vec2<int> m_LastChunkPos;
  
  Overlay m_TilesetSelection;
  void tileSelectionTick();
  void addTileset();
  void addTilesetWithId(unsigned id, Chunk* chunk);
  game::vec2<int> mouseToTilePos(game::vec2<float> mousePos);
  game::vec2<int> tileToMousePos(game::vec2<int> tilePos);
  bool m_LastTickTilesetChanged = false;
  bool selectingTile = false;
  bool selectingMapTile = false;
  game::vec2<float> tileSelectionStartPos { 0.f, 0.f };
  
  unsigned m_SelectedTilesetId { 0 };
  
  game::vec2<float> mapTileSelectionStartPos { 0.f, 0.f };
  
  
  std::vector<std::vector<Tile>> selectedTiles;
  
  game::vec2<int> tileSelectionPos { -1, -1 };
  game::vec2<int> tileSelectionSize { 0, 0 };
  
  
  bool shiftingView = false;
  game::vec2<float> shiftStartPos { 0.f, 0.f };
  game::vec2<float> shiftStartMousPos { 0.f, 0.f };
  game::vec2<float> mousePosition { 0.f, 0.f };
  
  std::optional<Tileset*> getTilesetById(unsigned id, Chunk* chunk);
  
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
  void mouseMotionEvent(SDL_MouseMotionEvent& mouseMotion);
  void mouseButtonEvent(SDL_MouseButtonEvent& mouseButton);
  void mouseWheelDirectionEvent(SDL_MouseWheelDirection& mouseWheelDirection);
  void mouseWheelEvent(SDL_MouseWheelEvent& mouseWheel);
  
};

#endif
