#include <cmath>

#include "game/entities/renderentity.h"

RenderEntity::RenderEntity(vec2<> p, vec2<> s, vec2<> a, GPU_Image* sprite)
{
  this->p = p[0];
  this->q = p[1];
  setSize(s);
  setAnchor(a);
  auto anim = std::make_shared<Animation>(
    sprite,
    1,
    -1.f
  );
  this->animations = std::vector<game::SharedAnimPtr>(1, anim);
  this->stage = 0.f;
}

RenderEntity::RenderEntity(vec2<> p, vec2<> s, vec2<> a, std::vector<game::SharedAnimPtr>& animations)
{
  this->p = p[0];
  this->q = p[1];
  setSize(s);
  setAnchor(a);
  this->animations = animations;
  this->stage = 0.f;
}

game::SharedAnimPtr RenderEntity::getCurrentAnimation()
{
  return animations[currentAnimation];
}

size_t RenderEntity::numAnimations()
{
  return animations.size();
}

int RenderEntity::setCurrentAnimation(size_t index)
{
  if(index < animations.size()) {
    currentAnimation = index;
    return index;
  }
  return -1;
}

float RenderEntity::getStage() {
  return stage;
}

void RenderEntity::tick(float tickTime)
{
  Animation* animPtr = animations[currentAnimation].get();
  if(animPtr->numFrames > 1 && animPtr->time != 0.f) {
    stage = fmod(stage+tickTime, animations[currentAnimation].get()->time);
  }
}
