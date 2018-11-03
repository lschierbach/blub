#ifndef SPRITE_H
#define SPRITE_H

#include "SDL_gpu.h"
#include <memory>

class Sprite
{
  public:
    virtual void tick() = 0;

    virtual GPU_Image* getImage() = 0;
    virtual GPU_Rect getFrame() = 0;
};

namespace game
{
  using SharedSpritePtr = std::shared_ptr<Sprite>;
}

#endif /* SPRITE_H */
