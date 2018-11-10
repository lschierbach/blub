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

#include <chrono>
#include "controller.h"
#include "game/force.hpp"
#include "game/gamemath.hpp"

void Controller::init()
{
  struct {
    unsigned int windowWidth;
    unsigned int windowHeight;
    bool fullscreen;
  } args = {800, 600, false};
  
  global::tickCount = 0;
  global::lastTickDuration = .0f;
  
  m_Model = new Model();
  m_Renderer = new Renderer(args.windowWidth, args.windowHeight, args.fullscreen, m_Model->getMap());
  m_Quit = false;
  
  m_Renderer->addCamera(0, 0, 1, 1, m_IdealCameraScale);
  m_Renderer->moveCamera(0, game::math::chunkSize / 2.f, game::math::chunkSize / 2.f);
  
  SDL_SetRelativeMouseMode(SDL_FALSE);
}

Entity* selectedEntity = nullptr;

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
      case SDL_MOUSEBUTTONDOWN:
        if (evt.button.clicks == 1)
        {
          vec2<float> clickXY = m_Renderer->pixelToXYAuto(vec2<float>(static_cast<float>(evt.button.x), static_cast<float>(evt.button.y)));
          
          auto* entity = m_Model->getMap()->get_entity_at<Entity>(clickXY);
          
          if (entity != nullptr)
          {
            if (selectedEntity != nullptr)
            {
              selectedEntity->setSprite(nullptr);
            }
            selectedEntity = entity;
            
            selectedEntity->setSprite(std::make_shared<SimpleSprite>("data/img/testEntitySelected.png"));
          }
          else
          {
            if (selectedEntity != nullptr)
            {
              selectedEntity->setSprite(nullptr);
            }
            else
            {
              m_Model->getMap()->for_each_entity_in_range<PhysicsEntity>(clickXY, 20.f, 
                [&](auto& entity) -> void
                {
                  auto diff = (entity.getPos() - clickXY);
                  auto forcedir = game::math::norm(diff);
                  entity.addForce(game::Force( forcedir * -50.f, .0f)); 
                }
              );
            }
            selectedEntity = nullptr;
          }
        }
        break;
    }
  }
}

void Controller::quit() 
{
  delete m_Model;
  delete m_Renderer;
}


void Controller::handleInput()
{
  const auto* keystate = SDL_GetKeyboardState(NULL);
  auto camSpeed = 30.f * global::lastTickDuration;
  auto moveForce = 4.f;
  
  auto moveVec = game::vec2<float>(.0f, .0f);
  
  if (keystate[SDL_SCANCODE_W]) moveVec += { .0f, -1.f};
  if (keystate[SDL_SCANCODE_A]) moveVec += {-1.f,  .0f};
  if (keystate[SDL_SCANCODE_S]) moveVec += { .0f,  1.f};
  if (keystate[SDL_SCANCODE_D]) moveVec += { 1.f,  .0f};
  if (keystate[SDL_SCANCODE_E]) m_Renderer->toggleFullscreen();
  if (keystate[SDL_SCANCODE_Q]) m_Quit = true;
  if (keystate[SDL_SCANCODE_R]) m_Renderer->zoomCamera(0, 0.9);
  if (keystate[SDL_SCANCODE_F]) m_Renderer->zoomCamera(0, 1.111111111111111);
  if (keystate[SDL_SCANCODE_V]) m_Renderer->setScale(0, m_IdealCameraScale);
  
  moveVec.norm();
  
  if (selectedEntity != nullptr)
  {
    selectedEntity->setPos(selectedEntity->getPos() + (moveVec * 0.2f));
  }
  else
  {
    m_Renderer->moveCamera(0, moveVec[0] * camSpeed, moveVec[1] * camSpeed);
  }
  
  //m_Renderer->moveCamera(0, m_MouseMotion.xrel * camSpeed * m_Renderer->getCameraScale(0), m_MouseMotion.yrel * camSpeed * m_Renderer->getCameraScale(0));
}

// @todo: rewrite smarter
float FPSSum = 0.f;

bool Controller::tick()
{
  auto time1 = SDL_GetTicks();
  FPSSum += global::lastTickDuration;
  
  if (global::tickCount != 0 && global::tickCount % 100 == 0)
  {
    printf("FPS:\t%.1f\n", 1.f / (FPSSum / 100.f));
    FPSSum = 0.f;
  }
  
  handleSDLEvents();
  handleInput();
  
  if (m_Quit)
  {
    quit();
    return false;
  }
  
  m_Model->tick();
  m_Renderer->tick(0.01);
  
  m_Renderer->show();
  
  auto time2 = SDL_GetTicks();
  
  
  if (time2 - time1 < m_IdealFrameTime)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(m_IdealFrameTime - (time2 - time1)));
  }
  
  global::lastTickDuration = (SDL_GetTicks() - time1) / 1000.f;
  
  global::tickCount++;
  return true;
}
