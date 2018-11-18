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
#include <string>
#include <fstream>

#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"

#include "renderer/renderer.h"
#include "game/entity.h"
#include "game/gamemath.hpp"

Renderer::Renderer(float w, float h, bool fullscreen, Map* map)
{
  renderTarget = NULL;
  //Add error handling!
  renderTarget = GPU_Init(w, h, RENDERER_INIT_FLAGS);
  setFullscreen(fullscreen);
  this->map = map;
//#ifdef DEBUG_RENDERER_PRINTID
  std::cout << "[RENDERER] Current rendering backend: " << GPU_GetCurrentRenderer()->id.name << " (major Version " << GPU_GetCurrentRenderer()->id.major_version << "; minor version " << GPU_GetCurrentRenderer()->id.minor_version << ")" << std::endl;
//#endif

  //////////////////////////////////// SHADERS ////////////////////////////////////

  auto vs = GPU_LoadShader(GPU_VERTEX_SHADER, "data/shader/common.vs.glsl");
  if(!vs) { std::cout << "Loading/compiling vertex shader failed" << std::endl; }

  auto fs = GPU_LoadShader(GPU_FRAGMENT_SHADER, "data/shader/common.fs.glsl");
  if(!fs) { std::cout << "Loading/compiling fragment shader failed" << std::endl; }

  sp = GPU_LinkShaders(vs, fs);
  if(!sp) { std::cout << "GLSL linking failed" << std::endl; }

  if(vs && fs && sp) {
    auto defaultShaderBlock = GPU_GetShaderBlock();
    block = GPU_LoadShaderBlock(sp, "gpu_Vertex", "gpu_TexCoord", "gpu_Color", "gpu_ModelViewProjectionMatrix");
  }

  GPU_ActivateShaderProgram(sp, &block);

  //Set static uniforms
  GPU_SetUniformf(GPU_GetUniformLocation(sp, "outerBound"), 1.5);
  GPU_SetUniformf(GPU_GetUniformLocation(sp, "innerBound"), 0.9);
  float outerColor[] = {1.1,0.6,0.6};
  float innerColor[] = {1.0,1.0,1.0};
  GPU_SetUniformfv(GPU_GetUniformLocation(sp, "outerColor"), 3, 1, outerColor);
  GPU_SetUniformfv(GPU_GetUniformLocation(sp, "innerColor"), 3, 1, innerColor);
  GPU_SetUniformf(GPU_GetUniformLocation(sp, "noiseFalloff"), 0.2);
  GPU_SetUniformf(GPU_GetUniformLocation(sp, "noiseRatio"), 0.1);

  GPU_DeactivateShaderProgram();
  
  auto vs_tile = GPU_LoadShader(GPU_VERTEX_SHADER, "data/shader/common.vs.glsl");
  if(!vs_tile) { std::cout << "Loading/compiling vertex shader failed:" << std::endl << GPU_GetShaderMessage() << std::endl; }

  auto fs_tile = GPU_LoadShader(GPU_FRAGMENT_SHADER, "data/shader/tile.fs.glsl");
  if(!fs_tile) { std::cout << "Loading/compiling fragment shader failed" << std::endl  << GPU_GetShaderMessage() << std::endl; }

  sp_tile = GPU_LinkShaders(vs_tile, fs_tile);
  if(!sp_tile) { std::cout << "GLSL linking failed" << std::endl << GPU_GetShaderMessage() << std::endl; }

  if(vs_tile && fs_tile && sp_tile) {
    auto defaultShaderBlock = GPU_GetShaderBlock();
    block_tile = GPU_LoadShaderBlock(sp_tile, "gpu_Vertex", "gpu_TexCoord", "gpu_Color", "gpu_ModelViewProjectionMatrix");
  }

  GPU_ActivateShaderProgram(sp_tile, &block_tile);

  float ambient[] = {
    0.8f, 0.8f, 0.8f
  };
  float lights[] = {
    1.0, 1.0, 4.0,
    10.0, 10.0, 2.0,
    20.0, 5.0, 6.0
  };
  float lightColors[] = {
    2.0, 0.0, 0.0,
    0.0, 2.0, 0.0,
    0.0, 0.0, 2.0
  };

  //Set static uniforms
  GPU_SetUniformfv(GPU_GetUniformLocation(sp_tile, "ambient"), 3, 1, ambient);
  GPU_SetUniformfv(GPU_GetUniformLocation(sp_tile, "lights"), 3, 3, lights);
  GPU_SetUniformfv(GPU_GetUniformLocation(sp_tile, "lightColors"), 3, 3, lightColors);
  GPU_SetUniformi(GPU_GetUniformLocation(sp_tile, "numLights"), 3);

  GPU_DeactivateShaderProgram();

  ////////////////////////////////// SHADERS END //////////////////////////////////

  std::ifstream tsJson("data/img/tileset/tilesets.json");

  rapidjson::IStreamWrapper isw {tsJson};
  rapidjson::Document tsDoc;
  tsDoc.ParseStream(isw);

  assert(tsDoc.IsArray());
  for(const auto& entry: tsDoc.GetArray()) {
    auto value = entry["tileset"].GetString();
    std::cout << "Loading tileset \"" << value << "\"..." << std::endl;

    std::vector<std::string> paths;
    std::vector<std::string> paths_n;

    for(const auto& image: entry["image"].GetArray()) {
      paths.push_back(tilesetDirectory + std::string(image.GetString()));
    }
    for(const auto& image: entry["normal_image"].GetArray()) {
      paths_n.push_back(tilesetDirectory + std::string(image.GetString()));
    }

    LODImage newlod(&paths[0], paths.size(), entry["min_resolution"].GetInt());
    tilesetImgs.insert(std::make_pair(
        std::string(entry["tileset"].GetString()),
        newlod
    ));

    LODImage newlod_n(&paths_n[0], paths_n.size(), entry["min_resolution"].GetInt());
    tilesetNormals.insert(std::make_pair(
        std::string(entry["tileset"].GetString()),
        newlod_n
    ));
  }
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
  for(auto camIt = cameras.begin(); camIt != cameras.end(); ++camIt)
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
  
  for(CameraEntry& camera: cameras)
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
  
  GPU_ActivateShaderProgram(sp_tile, &block_tile);
  GPU_SetUniformf(GPU_GetUniformLocation(sp_tile, "time"), SDL_GetTicks()/1000.f);
  //tiles & entities
  for(CameraEntry& camera: cameras)
  {
    std::shared_ptr camcast = std::static_pointer_cast<Camera>(camera.camera);

    GPU_SetUniformf(GPU_GetUniformLocation(sp_tile, "pixelsInUnit"), camcast.get()->pixelsInUnit());
    renderCamera(camera);
  }

  for(CameraEntry& camera: cameras)
  {
    renderCameraEntities(camera);
  }
  GPU_DeactivateShaderProgram();

  //overlays
  for(CameraEntry& camera: cameras)
  {
    std::shared_ptr camcast = std::static_pointer_cast<Camera>(camera.camera);

    camcast.get()->renderOverlays();
  }

  //final blitting of cameras to window

  //first enable the custom GLSL shaders
  GPU_ActivateShaderProgram(sp, &block);

  //set uniforms
  GPU_SetUniformf(GPU_GetUniformLocation(sp, "time"), SDL_GetTicks()/1000.f);
  GPU_SetUniformf(GPU_GetUniformLocation(sp, "aspect"), static_cast<float>(renderTarget->w)/renderTarget->h);

  for(CameraEntry& camera: cameras)
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
  //disable shaders when done
  GPU_DeactivateShaderProgram();

  //draw miscellaneous items
  drawBoxes();

}

void Renderer::drawBoxes() {
  for(auto& box: boxQueue) {
    if(box.corners > 0.f) {
      GPU_RectangleRoundFilled2(renderTarget, box.rect, box.corners, box.area);
      GPU_RectangleRound2(renderTarget, box.rect, box.corners, box.border);
    } else {
      GPU_RectangleFilled2(renderTarget, box.rect, box.area);
      GPU_Rectangle2(renderTarget, box.rect, box.border);
    }
  }
  boxQueue.clear();
}

bool Renderer::chunkInBounds(const Chunk& chunk, const CameraEntry& camera)
{
  Camera* camptr = std::static_pointer_cast<Camera>(camera.camera).get();
  
  auto chunkUpperLeft  = game::math::chunkToEntityPos(chunk.getPos()) - vec2<float>{ .5f, .5f };
  auto chunkLowerRight = game::math::chunkToEntityPos(chunk.getPos()) + ((game::math::chunkSize + .5f) * vec2<float>{ 1.f, 1.f });
  
  vec2<> camUpperLeft(camptr->getPos() - 0.5f*camptr->unitsInPixel()*camptr->getSize());
  vec2<> camLowerRight(camptr->getPos() + 0.5f*camptr->unitsInPixel()*camptr->getSize());

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

LODImage testLoadLOD()
{
  std::string filestrings[3];
  filestrings[0] = std::string("data/img/defaultTileset.png");
  filestrings[1] = std::string("data/img/defaultTileset_512.png");
  filestrings[2] = std::string("data/img/defaultTileset_256.png");

  LODImage theImage(filestrings, 3, 64);
  return theImage;
}

void Renderer::renderCamera(CameraEntry& camera)
{
#ifdef DEBUG_RENDERER_VERBOSE
  std::cout << "[RENDERER] Rendering camera frames" << std::endl;
#endif

  //static LODImage testImg = testLoadLOD();
  Map::SharedEntityPtr theCam = camera.camera;
  std::shared_ptr camcast = std::static_pointer_cast<Camera>(theCam);

#ifdef DEBUG_RENDERER_VERBOSE
  std::cout << "[RENDERER] got camera plus metadata. scale " << camcast.get()->getScale() << std::endl;
#endif

  camcast.get()->clearRender();

  auto topLeftScreen =     pixelToXYAuto(vec2<float>(1.f, 1.f));
  auto bottomRightScreen = pixelToXYAuto(camcast.get()->getSize() - vec2<float>(1.f, 1.f));
  
  map->for_each_chunk_in_box(topLeftScreen, bottomRightScreen - topLeftScreen, 
    [&](auto&& chunk) -> void
    {
      for(const auto& ts: chunk.m_Data.m_Tilesets)
      {
  #ifdef DEBUG_RENDERER_VERBOSE
    std::cout << "[RENDERER] tilesets for chunk relative " << j << "|" << i << std::endl;
  #endif
        //render all tilesets of current chunk

        vec2<float> chunkOffset = game::math::chunkToEntityPos(chunk.getPos()) + vec2<float>(ts.offsetX,ts.offsetY);

        auto iter = tilesetImgs.find(ts.imgName);
        auto iter_n = tilesetNormals.find(ts.imgName);
        auto& tilesetImg = std::get<LODImage>(*(iter));
        auto& tilesetImg_n = std::get<LODImage>(*(iter_n));

        GPU_SetShaderImage(tilesetImg_n.bestImage(camcast.get()), GPU_GetUniformLocation(sp_tile, "nmap"), 1);
        camcast.get()->renderTileset(ts, tilesetImg.bestImage(camcast.get()), 0.f, 0.f, chunkOffset[0], chunkOffset[1]);

      }
    }
  );
  
  return;
}

void Renderer::renderCameraEntities(CameraEntry& camera)
{
  Map::SharedEntityPtr theCam = camera.camera;
  std::shared_ptr camcast = std::static_pointer_cast<Camera>(theCam);

  auto topLeftScreen =     pixelToXYAuto(vec2<float>(1.f, 1.f));
  auto bottomRightScreen = pixelToXYAuto(camcast.get()->getSize() - vec2<float>(1.f, 1.f));
  
  map->for_each_entity_in_box<Entity>(topLeftScreen, bottomRightScreen - topLeftScreen, 
    [&](auto& entity) -> void
    {
      camcast.get()->renderEntity(entity);
    }
  );
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

void Renderer::setCameraPos(size_t cameraId, vec2<float> pos) 
{
  Camera* cam = std::static_pointer_cast<Camera>(getCamera(cameraId).camera).get();
  cam->setPos(pos);
}


void Renderer::zoomCamera(size_t cameraId, float factor)
{
  Camera* cam = std::static_pointer_cast<Camera>(getCamera(cameraId).camera).get();
  cam->setScale(cam->getScale()*factor);
}

void Renderer::setScale(size_t cameraId, float scale) 
{
  Camera* cam = std::static_pointer_cast<Camera>(getCamera(cameraId).camera).get();
  cam->setScale(scale);
}

void Renderer::tick(float tickTime)
{
  for(CameraEntry entry: cameras)
  {
    std::static_pointer_cast<Camera>(entry.camera).get()->tick();
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

void Renderer::renderBox(float x, float y, float w, float h, SDL_Color borderColor, SDL_Color areaColor, float borderRadius)
{
  boxQueue.push_back({x, y, w, h, borderColor, areaColor, borderRadius});
}

vec2<float> Renderer::pixelToXYAuto(vec2<float> pixel)
{
  float ratioX = pixel[0]/renderTarget->w;
  float ratioY = pixel[1]/renderTarget->h;

  for(auto camIt = cameras.begin(); camIt != cameras.end(); ++camIt)
  {
    if(camIt->data[0] < ratioX && camIt->data[2]-camIt->data[0] > ratioX &&
       camIt->data[1] < ratioY && camIt->data[3]-camIt->data[1] > ratioY)
    {
      return std::static_pointer_cast<Camera>(camIt->camera).get()->pixelToXY(vec2<float>(pixel[0]*camIt->data[2],pixel[1]*camIt->data[3]));
    }
  }

  return vec2<float>(NAN, NAN);
}
