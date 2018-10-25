/*
 *  FILENAME:      renderer.cpp
 *
 *  DESCRIPTION:
 *      
 *
 *  NOTES:
 *    
 *
 *  AUTHOR:        Tobias Fey     DATE: 01.10.2018
 *
 *  CHANGES:
 *
 */

#include <iostream>
#include <utility>
#include <cmath>

#include "renderer/renderer.h"
#include "game/entity.h"

Renderer::Renderer(float w, float h, bool fullscreen, Map* map)
{
  renderTarget = NULL;
  //Add error handling!
  renderTarget = GPU_Init(w, h, RENDERER_INIT_FLAGS);
  setFullscreen(fullscreen);
  this->map = map;
#ifdef DEBUG_RENDERER_PRINTID
  std::cout << "[RENDERER] Current rendering backend: " << GPU_GetCurrentRenderer()->id.name << " (major Version " << GPU_GetCurrentRenderer()->id.major_version << "; minor version " << GPU_GetCurrentRenderer()->id.minor_version << ")" << std::endl;
#endif
}

Renderer::~Renderer()
{
  GPU_FreeTarget(renderTarget);
}

size_t Renderer::getCameraId() const
{
  static size_t id = 0;
  return id++;
}

size_t Renderer::addCamera(float x, float y, float w, float h, float scale)
{
#ifdef DEBUG_RENDERER_VERBOSE
  std::cout << "[RENDERER] making camera" << std::endl;
#endif

  size_t theId = getCameraId();
  cameras.push_back(
    std::make_tuple(
      std::static_pointer_cast<Entity>(
        std::make_shared<Camera>(
          0,
          0,
          getWidth()*w,
          getHeight()*h,
          scale
        )),
        std::array<float, 4>{x, y, w, h},
        theId
      )
    );
    
#ifdef DEBUG_RENDERER_VERBOSE
  std::cout << "[RENDERER] adding camera " << theId << " to map" << std::endl;
#endif
  map->addEntity(std::get<Map::SharedEntityPtr>(getCamera(theId)));
#ifdef DEBUG_RENDERER_VERBOSE
  std::cout << "[RENDERER] done adding camera" << std::endl;
#endif

  return theId;
}

std::tuple<Map::SharedEntityPtr, std::array<float, 4>, size_t> Renderer::getCamera(size_t index)
{
  for(std::list<std::tuple<Map::SharedEntityPtr, std::array<float, 4>, size_t>>::iterator camera = cameras.begin(); camera != cameras.end(); ++camera) 
  {
    if(std::get<size_t>(*camera) == index) 
    {
      return *camera;
    }
  }
  
  return std::make_tuple(nullptr, std::array<float, 4> { 0.f }, 0);
}

float Renderer::getWidth() const
{
  return renderTarget->w;
}

float Renderer::getHeight() const
{
  return renderTarget->h;
}

void Renderer::setWidth(float w)
{
  float h = getHeight();
  setSize(w, h);
}

void Renderer::setHeight(float h)
{
  float w = getWidth();
  setSize(w, h);
}

void Renderer::setSize(float w, float h)
{
#ifdef DEBUG_RENDERER_RESIZE
  std::cout << "resizing to " << w << " x " << h << std::endl;
#endif

  GPU_SetWindowResolution(w, h);

  resizeCameras();
}

void Renderer::setFullscreen(bool fs)
{
  GPU_SetFullscreen(fs, true);
  isFullscreen = fs;
}

void Renderer::toggleFullscreen()
{
  setFullscreen(!isFullscreen);
  resizeCameras();
}

void Renderer::resizeCameras() {
  float w = getWidth();
  float h = getHeight();
  
  for(std::tuple<Map::SharedEntityPtr, std::array<float,4>, size_t> camera: cameras) 
  {
#ifdef DEBUG_RENDERER_RESIZE
  std::cout << "[RENDERER] camera resize to " << w << " x " << h << std::endl;
#endif
  std::static_pointer_cast<Camera>(std::get<Map::SharedEntityPtr>(camera)).get()->setSize(
      vec2<float>(
        w*std::get<std::array<float, 4>>(camera)[2],
        h*std::get<std::array<float, 4>>(camera)[3]
      )
    );
    
  }
}

void Renderer::renderFrame()
{
  GPU_ClearRGB(renderTarget, 50, 50, 50);
  
  for(std::tuple<Map::SharedEntityPtr, std::array<float, 4>, size_t> camera: cameras)
  {
    renderCamera(camera);
  }
  
  for(std::tuple<Map::SharedEntityPtr, std::array<float, 4>, size_t> camera: cameras) 
  {
    std::shared_ptr camcast = std::static_pointer_cast<Camera>(std::get<Map::SharedEntityPtr>(camera));

    std::array<float, 4> camData = std::get<std::array<float,4>>(camera);
    GPU_Rect cameraRect = GPU_MakeRect(
      camData[0]*getWidth(),
      camData[1]*getHeight(),
      camData[2]*getWidth(),
      camData[3]*getHeight()
    );
    camcast.get()->renderOverlays();
    GPU_BlitRect(camcast.get()->getRender(), NULL, renderTarget, &cameraRect);

  }
}

void Renderer::renderCamera(std::tuple<Map::SharedEntityPtr, std::array<float, 4>, size_t>& camera)
{
#ifdef DEBUG_RENDERER_VERBOSE
  std::cout << "[RENDERER] Rendering camera frames" << std::endl;
#endif

  static GPU_Image* testImg = GPU_LoadImage("data/img/defaultTileset.png");
  //GPU_SetImageFilter(testImg, GPU_FILTER_NEAREST);
  Map::SharedEntityPtr theCam = std::get<Map::SharedEntityPtr>(camera);
  std::shared_ptr camcast = std::static_pointer_cast<Camera>(theCam);

#ifdef DEBUG_RENDERER_VERBOSE
  std::cout << "[RENDERER] got camera plus metadata. scale " << camcast.get()->getScale() << std::endl;
#endif

  camcast.get()->clearRender();

  for(int i=0-(int)Map::getLoadingDistance(); i<=(int)Map::getLoadingDistance(); i++)
  { //q
    for(int j=0-(int)Map::getLoadingDistance(); j<=(int)Map::getLoadingDistance(); j++)
    { //p

#ifdef DEBUG_RENDERER_PREMUL_COORDINATES
  vec2<float> oldCoord = theCam.get()->getPQ();
  theCam.get()->setPQ(oldCoord/(float)Chunk::size);
#endif

      Chunk c = map->getChunk(j, i, theCam);
#ifdef DEBUG_RENDERER_PREMUL_COORDINATES
  theCam.get()->setPQ(oldCoord);
#endif

#ifdef DEBUG_RENDERER_VERBOSE
  std::cout << "[RENDERER] got chunk" << std::endl;
#endif

      for(Tileset ts: c.m_Data.m_Tilesets)
      {
#ifdef DEBUG_RENDERER_VERBOSE
  std::cout << "[RENDERER] iterating over tilesets" << std::endl;
#endif

        vec2<float> chunkOffset = Entity::axialToCartesian(
          vec2<float>{
            (float)((c.getP() * Chunk::size) + ts.offsetX),
            (float)((c.getQ() * Chunk::size) + ts.offsetY)
          }
        );

        camcast.get()->renderTileset(ts, testImg, 0.f, 0.f, chunkOffset[0], chunkOffset[1]);
      }
    }
  }
}

void Renderer::show()
{
  GPU_Flip(renderTarget);
}

void Renderer::cameraTrack(size_t cameraIndex, Map::SharedEntityPtr entity)
{
  std::static_pointer_cast<Camera>(std::get<Map::SharedEntityPtr>(getCamera(cameraIndex))).get()->track(entity);
}

void Renderer::moveCamera(size_t cameraIndex, float x, float y)
{
  std::static_pointer_cast<Camera>(std::get<Map::SharedEntityPtr>(getCamera(cameraIndex))).get()->modXY(vec2<float>(x,y));
}

void Renderer::zoomCamera(size_t cameraIndex, float factor)
{
  Camera* cam = std::static_pointer_cast<Camera>(std::get<Map::SharedEntityPtr>(getCamera(cameraIndex))).get();
  cam->setScale(cam->getScale()*factor);
}

void Renderer::tick(const float tickTime)
{
  for(std::tuple<Map::SharedEntityPtr,std::array<float,4>, size_t> entry: cameras)
  {
    std::static_pointer_cast<Camera>(std::get<Map::SharedEntityPtr>(entry)).get()->tick(tickTime);
  }

  renderFrame();
}

float Renderer::getCameraScale(size_t cameraIndex)
{
  float ret = std::static_pointer_cast<Camera>(std::get<Map::SharedEntityPtr>(getCamera(cameraIndex))).get()->getScale();
  return ret;
}

void Renderer::addOverlay(size_t cameraIndex, const Overlay* const o)
{
  std::static_pointer_cast<Camera>(std::get<Map::SharedEntityPtr>(getCamera(cameraIndex))).get()->addOverlay(o);
}

void Renderer::removeOverlay(size_t cameraIndex, const Overlay* const element)
{
  std::static_pointer_cast<Camera>(std::get<Map::SharedEntityPtr>(getCamera(cameraIndex))).get()->removeOverlay(element);
}

void Renderer::clearOverlays(size_t cameraIndex)
{
  std::static_pointer_cast<Camera>(std::get<Map::SharedEntityPtr>(getCamera(cameraIndex))).get()->clearOverlays();
}
