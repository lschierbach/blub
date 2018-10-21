/*
 * FILENAME:        RenderEntity.h
 *
 */

#ifndef RENDERENTITY_H
#define RENDERENTITY_H

#include "game/entity.h"
#include "SDL_gpu.h"

class RenderEntity: public Entity
{
    private:
        GPU_Image* sprite;
        size_t numFrames;
        float animTime; //seconds

    public:
        RenderEntity(vec2 p, vec2 s, vec2 a, GPU_Image* sprite);
        RenderEntity(vec2 p, vec2 s, vec2 a, GPU_Image* sprite, size_t frames, float time);
}

#endif /* RENDERENTITY_H */
