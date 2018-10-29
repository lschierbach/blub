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

class Renderer
{
  private:
    //std::map<std::string, GPU_Image*> tilesetImgs;

    /*
    Data structure: cameras. Vector of Tuples saving
    * 1. the camera itself as SharedEntityPtr and
    * 2. the x and y, w and h *relative* to complete renderer's target, as fraction of 1.
    *   -> i.e. 0.5 0 0.5 1 means "camera starts at half the window's width (from the left), top of the window, half as wide as window and as tall as window".
    */
    std::list<CameraEntry> cameras;

    GPU_Target* renderTarget;
    const static GPU_InitFlagEnum RENDERER_INIT_FLAGS = GPU_DEFAULT_INIT_FLAGS;
    Map* map;
    bool isFullscreen;
    void resizeCameras();
    void renderCamera(CameraEntry& camera);
    void renderCameraEntities(CameraEntry& camera);
    size_t getCameraId() const;
    CameraEntry getCamera(size_t index);
    bool chunkInBounds(const Chunk& chunk, const CameraEntry& camera);

    GPU_Image* LoadImageWithMipmaps(const char* filename);

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
    void cameraTrack(size_t cameraId, Map::SharedEntityPtr entity);
    void zoomCamera(size_t cameraId, float factor);
    
    float getCameraScale(size_t cameraId);

    void addOverlay(size_t cameraId, const Overlay* const o);
    void removeOverlay(size_t cameraId, const Overlay* const element);
    void clearOverlays(size_t cameraId);

    void tick(const float tickTime);
};

#endif /* RENDERER_H */
