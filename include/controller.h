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

#include "logic/model.h"
#include "renderer/renderer.h"

class Controller
{
  private:
    void handleInput();
    void handleSDLEvents();
    
    bool m_Quit;
    
    SDL_MouseMotionEvent m_MouseMotion;
    
  public:
    void init();
      
    Model* m_Model;
    Renderer* m_Renderer;
    
    bool tick();
};

#endif /* CONTROLLER_H */