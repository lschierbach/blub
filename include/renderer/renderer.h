/*
 *  FILENAME:      Renderer.h
 *
 *  DESCRIPTION:
 *      Class providing unified controls over cameras and various other rendering/display tasks
 *
 *  AUTHOR:         Tobias Fey      DATE: 01.10.2018
 *
 *  DEBUG:          DEBUG_RENDERER
 *                  DEBUG_RENDERER_VERBOSE
 *                  DEBUG_RENDERER_RESIZE
 *                  DEBUG_RENDERER_PREMUL_COORDINATES
 *  NOTES:
 *      DEBUG_RENDERER does not set DEBUG_RENDERER_PREMUL_COORDINATES as it will most likely make the program behave incorrectly and/or unexpectedly.
 */

#ifndef RENDERER_H
#define RENDERER_H

#ifdef DEBUG_RENDERER
    #define DEBUG_RENDERER_VERBOSE
    #define DEBUG_RENDERER_RESIZE
#endif

#include <map>
#include <string>
#include <vector>

#include "SDL_gpu.h"

#include "logic/map.h"
#include "game/entities/camera.h"
#include "renderer/cameraentry.h"
#include "renderer/lodimage.hpp"
#include "renderer/coloredrect.h"

class Renderer
{
  private:

    static constexpr char tilesetDirectory[] = "data/img/tileset/";
    std::map<std::string, LODImage> tilesetImgs;

    std::vector<CameraEntry> cameras;
    std::vector<ColoredRect> boxQueue;

    GPU_Target* renderTarget;
    const static GPU_InitFlagEnum RENDERER_INIT_FLAGS = GPU_DEFAULT_INIT_FLAGS;
    Map* map;
    bool isFullscreen;
    void resizeCameras();
    void renderCamera(CameraEntry& camera);
    void renderCameraEntities(CameraEntry& camera);
    size_t getCameraId() const;
    bool chunkInBounds(const Chunk& chunk, const CameraEntry& camera);
    void drawBoxes();

    GPU_Image* LoadImageWithMipmaps(const char* filename);

    Uint32 sp;
    GPU_ShaderBlock block;
    
    Uint32 sp_tile;
    GPU_ShaderBlock block_tile;

  public:
    Renderer(float w, float h, bool fullscreen, Map* map);
    ~Renderer();

    size_t addCamera(float x, float y, float w, float h, float scale);

    float getWidth() const;
    float getHeight() const;

    void setWidth(float w);
    void setHeight(float h);
    void setSize(float w, float h);
    void setFullscreen(bool fs);
    void toggleFullscreen();

    void renderFrame();
    void show();

    void moveCamera(size_t cameraId, float x, float y);
    void setCameraPos(size_t cameraId, vec2<float> pos);
    void cameraTrack(size_t cameraId, Map::SharedEntityPtr entity);
    void zoomCamera(size_t cameraId, float factor);
    void setScale(size_t cameraId, float scale);
    
    float getCameraScale(size_t cameraId);

    void addOverlay(size_t cameraId, const Overlay* const o);
    void removeOverlay(size_t cameraId, const Overlay* const element);
    void clearOverlays(size_t cameraId);

    void renderBox(float x, float y, float w, float h, SDL_Color borderColor = {0,0,255,255}, SDL_Color areaColor = {0,0,0,0}, float borderRadius = 0.f);

    void tick(const float tickTime);

    vec2<float> pixelToXYAuto(vec2<float> pixel);
    
    CameraEntry getCamera(size_t index);
    
    GPU_Image* getTilesetImage(const std::string& imgName);
    std::map<std::string, LODImage>* getTilesetImgs();

};

#endif /* RENDERER_H */
