#ifndef SIMPLESPRITE_H
#define SIMPLESPRITE_H

#include <map>

#include "renderer/sprite.h"

class SimpleSprite: public Sprite
{
  private:
    static std::map<const char*, GPU_Image*> loaded;
    const char* path;
    GPU_Image* image;
  public:
    SimpleSprite(const char* imagepath);
    GPU_Image* getImage();
    GPU_Rect getFrame();
    void tick();
};
#endif /* SIMPLESPRITE_H */
