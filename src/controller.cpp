/*
 *  FILENAME:      controller.cpp
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
 *  TODO: 
 *    -logic/renderer multithreading
 *    -debug messages
 *
 */

#include "controller.h"

void Controller::init()
{
  struct {
    unsigned int windowWidth;
    unsigned int windowHeight;
    bool fullscreen;
  } args = {1600, 900, false};
  
  m_Model = new Model();
  m_Renderer = new Renderer(args.windowWidth, args.windowHeight, args.fullscreen, m_Model->getMap());
  m_Quit = false;
  
  m_Renderer->addCamera(0, 0, 1, 1, 25);
  
  SDL_SetRelativeMouseMode(SDL_TRUE);
}

void Controller::handleSDLEvents()
{
  SDL_Event evt;
  
  m_MouseMotion.xrel = 0;
  m_MouseMotion.yrel = 0;
  
  while (SDL_PollEvent(&evt))
  {
    switch (evt.type)
    {
      case SDL_QUIT:
        m_Quit = true;
        break;
      case SDL_MOUSEMOTION:
        m_MouseMotion = evt.motion;
        break;
    }
  }
}

void Controller::handleInput()
{
  const auto* keystate = SDL_GetKeyboardState(NULL);
  
  if (keystate[SDL_SCANCODE_W]) m_Renderer->moveCamera(0, 0.0, -0.002 * m_Renderer->getCameraScale(0));
  if (keystate[SDL_SCANCODE_A]) m_Renderer->moveCamera(0, -0.002 * m_Renderer->getCameraScale(0), 0.0);
  if (keystate[SDL_SCANCODE_S]) m_Renderer->moveCamera(0, 0.0, 0.002 * m_Renderer->getCameraScale(0));
  if (keystate[SDL_SCANCODE_D]) m_Renderer->moveCamera(0, 0.002 * m_Renderer->getCameraScale(0), 0.0);
  if (keystate[SDL_SCANCODE_E]) m_Renderer->toggleFullscreen();
  if (keystate[SDL_SCANCODE_Q]) m_Quit = true;
  if (keystate[SDL_SCANCODE_R]) m_Renderer->zoomCamera(0, 0.9);
  if (keystate[SDL_SCANCODE_F]) m_Renderer->zoomCamera(0, 1.111111111111111);
  
  m_Renderer->moveCamera(0, m_MouseMotion.xrel * 0.002 * m_Renderer->getCameraScale(0), m_MouseMotion.yrel * 0.002 * m_Renderer->getCameraScale(0));
}

bool Controller::tick()
{
  handleSDLEvents();
  handleInput();
  
  m_Model->tick();
  m_Renderer->tick(0.01);
  
  m_Renderer->show();
  
  return !m_Quit;
}