/*
 *  FILENAME:      Camera.h
 *
 *  DESCRIPTION:
 *      Various mid-level render tasks, rendering to the internal image
 *
 *  AUTHOR:         Tobias Fey      DATE: 01.10.2018
 *
 *  DEBUG:          DEBUG_CAMERA
 *                  DEBUG_CAMERA_VERBOSE
 *                  DEBUG_CAMERA_BOUNDING_BOXES
 *  NOTES:
 *      THE DEBUG_CAMERA_BOUNDING_BOXES flag will interfere with the rendering pipeline, reducing performance immensely.
 */

#ifndef CAMERA_H
#define CAMERA_H

 #ifdef DEBUG_CAMERA
     #define DEBUG_CAMERA_VERBOSE
     #define DEBUG_CAMERA_BOUNDING_BOXES
 #endif

#include <list>

#include "SDL_gpu.h"
#include "game/entity.h"
#include "logic/map.h"
#include "renderer/overlay.h"
#include "game/entities/renderentity.h"

class Camera: public Entity
{
    private:
        GPU_Image* image;
        float scale;
        Map::SharedEntityPtr tracked;

        std::list<const Overlay*> overlays;

        GPU_Rect getTile(GPU_Image* img, unsigned char index, unsigned char inset);

  public:
        Camera();
        Camera(float x, float y, float w, float h, float scale);
        ~Camera();

        void tick(const float tickTime);

        void setSize(const vec2<float> s); //Override to recreate image
        void setSize(const float w, const float h);
        void setScale(const float s);

        void addOverlay(const Overlay* const o);
        void removeOverlay(const Overlay* const element);
        void clearOverlays();

        float getScale() const;
        float pixelsInUnit() const;
        float unitsInPixel() const;
        float unitsX() const;
        float unitsY() const;

        GPU_Image* getRender() const;

        void clearRender();
        void renderTileset(const Tileset& ts, GPU_Image* img, float factor_width, float factor_height, float x_offset, float y_offset);
        void render2dMap(int* data, SDL_Color (*conversion)(int), size_t w, size_t h);
        void renderEntity(RenderEntity e);
        void renderEntity(Entity e);
        void renderOverlays();

        void track(Map::SharedEntityPtr entity);
};

#endif /* CAMERA_H */
