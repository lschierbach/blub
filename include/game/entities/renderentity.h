/*
 * FILENAME:        RenderEntity.h
 *
 */

#ifndef RENDERENTITY_H
#define RENDERENTITY_H

#include <vector>

#include "SDL_gpu.h"

#include "game/entity.h"
#include "renderer/animation.hpp"

class RenderEntity: public Entity
{

  private:
    std::vector<game::SharedAnimPtr> animations;
    float stage;
    size_t currentAnimation;

  public:
    RenderEntity(vec2<> p, vec2<> s, vec2<> a, GPU_Image* sprite);
    RenderEntity(vec2<> p, vec2<> s, vec2<> a, std::vector<game::SharedAnimPtr>&);

    void tick(float tickTime);
    int setCurrentAnimation(size_t index);
    void addAnimation(game::SharedAnimPtr);

    game::SharedAnimPtr getCurrentAnimation();
    size_t numAnimations();
    float getStage();
};

#endif /* RENDERENTITY_H */
