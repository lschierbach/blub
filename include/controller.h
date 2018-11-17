/*
 *  FILENAME:      controller.h
 *
 *  DESCRIPTION:
 *      
 *
 *  NOTES:
 *      
 *
 *  AUTHOR:        Leon Schierbach     DATE: 21.10.2018
 *
 *  CHANGES:
 *
 */

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "game/global.h"
#include "logic/model.h"
#include "renderer/renderer.h"
#include "editor/editor.h"

class Controller
{
  private:
    void handleInput();
    void handleSDLEvents();
    
    bool m_Quit;
    
    SDL_MouseMotionEvent m_MouseMotion;
    SDL_MouseButtonEvent m_MouseButton;
    SDL_MouseWheelDirection m_MouseWheelDirection;
    SDL_MouseWheelEvent m_MouseWheel;
    
    // at 60fps one fram is about 16ms
    static constexpr int m_IdealFrameTime = 16;
    
    static constexpr float m_IdealCameraScale = 14;
    
  public:
    void init(int argc, char** argv);
      
    Model* m_Model;
    Renderer* m_Renderer;
    Editor* m_Editor;
    
    bool tick();
    
    void quit();
};

#endif /* CONTROLLER_H */
