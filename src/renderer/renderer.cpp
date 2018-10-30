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
#include "renderer/lodimage.hpp"
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
  cameras.push_back(CameraEntry{
    std::make_shared<Camera>(
      0,
      0,
      getWidth()*w,
      getHeight()*h,
      scale
    ),

    std::array<float,4>{x, y, w, h},

    theId
  });
    
#ifdef DEBUG_RENDERER_VERBOSE
  std::cout << "[RENDERER] adding camera " << theId << " to map" << std::endl;
#endif
  map->addEntity(getCamera(theId).camera);
#ifdef DEBUG_RENDERER_VERBOSE
  std::cout << "[RENDERER] done adding camera" << std::endl;
#endif

  return theId;
}

CameraEntry Renderer::getCamera(size_t id)
{
  for(std::list<CameraEntry>::iterator camIt = cameras.begin(); camIt != cameras.end(); ++camIt)
  {
    if(camIt->id == id)
    {
      return *camIt;
    }
  }
  
  return CameraEntry{nullptr, std::array<float, 4> { 0.f }, 0};
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
  
  for(CameraEntry camera: cameras)
  {
#ifdef DEBUG_RENDERER_RESIZE
  std::cout << "[RENDERER] camera resize to " << w << " x " << h << std::endl;
#endif
  std::static_pointer_cast<Camera>(camera.camera).get()->setSize(
      vec2<float>(
        w*camera.data[2],
        h*camera.data[3]
      )
    );
    
  }
}

void Renderer::renderFrame()
{
  GPU_ClearRGB(renderTarget, 50, 50, 50);
  
  //tiles & entities
  for(CameraEntry camera: cameras)
  {
    renderCamera(camera);
  }

  for(CameraEntry camera: cameras)
  {
    renderCameraEntities(camera);
  }

  //overlays
  for(CameraEntry camera: cameras)
  {
    std::shared_ptr camcast = std::static_pointer_cast<Camera>(camera.camera);

    camcast.get()->renderOverlays();
  }

  //final blitting
  for(CameraEntry camera: cameras)
  {
    std::shared_ptr camcast = std::static_pointer_cast<Camera>(camera.camera);

    std::array<float,4> camData = camera.data;
    GPU_Rect cameraRect = GPU_MakeRect(
      camData[0]*getWidth(),
      camData[1]*getHeight(),
      camData[2]*getWidth(),
      camData[3]*getHeight()
    );

    GPU_BlitRect(camcast.get()->getRender(), NULL, renderTarget, &cameraRect);

  }
}

bool Renderer::chunkInBounds(const Chunk& chunk, const CameraEntry& camera)
{
  Camera* camptr = std::static_pointer_cast<Camera>(camera.camera).get();

  vec2<> chunkUpperLeft = Entity::axialToCartesian(vec2<>((Chunk::size*chunk.getP())-0.5f,
                                                          (Chunk::size*chunk.getQ())-0.5f));

  vec2<> chunkLowerRight = Entity::axialToCartesian(vec2<>((Chunk::size*chunk.getP())+Chunk::size+0.5f,
                                                           (Chunk::size*chunk.getQ())+Chunk::size+0.5f));

  vec2<> camUpperLeft(camptr->getXY() - 0.5f*camptr->unitsInPixel()*camptr->getSize());
  vec2<> camLowerRight(camptr->getXY() + 0.5f*camptr->unitsInPixel()*camptr->getSize());

  return (chunkUpperLeft[0] <= camLowerRight[0] &&
          chunkUpperLeft[1] <= camLowerRight[1] &&
          chunkLowerRight[0] >= camUpperLeft[0] &&
          chunkLowerRight[0] >= camUpperLeft[0]);
}

GPU_Image* Renderer::LoadImageWithMipmaps(const char* filename)
{
    GPU_Image* img = NULL;
    if((img = GPU_LoadImage(filename)) != NULL) {
        GPU_GenerateMipmaps(img);
        GPU_SetImageFilter(img, GPU_FILTER_LINEAR_MIPMAP);
    }
    return img;
}

void Renderer::renderCamera(CameraEntry& camera)
{
#ifdef DEBUG_RENDERER_VERBOSE
  std::cout << "[RENDERER] Rendering camera frames" << std::endl;
#endif

  static LODImage testImg("data/img/defaultTileset.png", 64);
  Map::SharedEntityPtr theCam = camera.camera;
  std::shared_ptr camcast = std::static_pointer_cast<Camera>(theCam);

#ifdef DEBUG_RENDERER_VERBOSE
  std::cout << "[RENDERER] got camera plus metadata. scale " << camcast.get()->getScale() << std::endl;
#endif

  camcast.get()->clearRender();

  //for all chunks within loading distance
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

      if(chunkInBounds(c, camera))
      {
  #ifdef DEBUG_RENDERER_VERBOSE
    std::cout << "[RENDERER] tilesets for chunk relative " << j << "|" << i << std::endl;
  #endif
        //render all tilesets of current chunk
        for(Tileset ts: c.m_Data.m_Tilesets)
        {

          vec2<float> chunkOffset = Entity::axialToCartesian(
            vec2<float>{
              (float)((c.getP() * Chunk::size) + ts.offsetX),
              (float)((c.getQ() * Chunk::size) + ts.offsetY)
            }
          );

          camcast.get()->renderTileset(ts, testImg.bestImage(camcast.get()), 0.0019f, 0.0017f, chunkOffset[0], chunkOffset[1]);
        }
      }

    }
  }
}

void Renderer::renderCameraEntities(CameraEntry& camera)
{
  Map::SharedEntityPtr theCam = camera.camera;
  std::shared_ptr camcast = std::static_pointer_cast<Camera>(theCam);

  for(int i=0-(int)Map::getLoadingDistance(); i<=(int)Map::getLoadingDistance(); i++)
  { //q
    for(int j=0-(int)Map::getLoadingDistance(); j<=(int)Map::getLoadingDistance(); j++)
    { //p

      Chunk c = map->getChunk(j, i, theCam);

      //render all entities in that chunk
      for(auto entity: c.m_Data.m_Entities)
      {
#ifdef DEBUG_RENDERER_VERBOSE
  std::cout << "[RENDERER] iterating over entities" << std::endl;
#endif
        camcast.get()->renderEntity(entity);
      }
    }
  }
}

void Renderer::show()
{
  GPU_Flip(renderTarget);
}

void Renderer::cameraTrack(size_t cameraId, Map::SharedEntityPtr entity)
{
  std::static_pointer_cast<Camera>(getCamera(cameraId).camera).get()->track(entity);
}

void Renderer::moveCamera(size_t cameraId, float x, float y)
{
  std::static_pointer_cast<Camera>(getCamera(cameraId).camera).get()->modXY(vec2<float>(x,y));
}

void Renderer::zoomCamera(size_t cameraId, float factor)
{
  Camera* cam = std::static_pointer_cast<Camera>(getCamera(cameraId).camera).get();
  cam->setScale(cam->getScale()*factor);
}

void Renderer::tick(const float tickTime)
{
  for(CameraEntry entry: cameras)
  {
    std::static_pointer_cast<Camera>(entry.camera).get()->tick(tickTime);
  }

  renderFrame();
}

float Renderer::getCameraScale(size_t cameraIndex)
{
  float ret = std::static_pointer_cast<Camera>(getCamera(cameraIndex).camera).get()->getScale();
  return ret;
}

void Renderer::addOverlay(size_t cameraIndex, const Overlay* const o)
{
  std::static_pointer_cast<Camera>(getCamera(cameraIndex).camera).get()->addOverlay(o);
}

void Renderer::removeOverlay(size_t cameraIndex, const Overlay* const element)
{
  std::static_pointer_cast<Camera>(getCamera(cameraIndex).camera).get()->removeOverlay(element);
}

void Renderer::clearOverlays(size_t cameraIndex)
{
  std::static_pointer_cast<Camera>(getCamera(cameraIndex).camera).get()->clearOverlays();
}
