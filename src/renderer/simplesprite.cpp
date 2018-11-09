#include <iostream>

#include "renderer/sprite.h"
#include "renderer/simplesprite.h"

std::map<const char*, GPU_Image*> SimpleSprite::loaded;

SimpleSprite::SimpleSprite(const char* imagepath)
{
  path = imagepath;
  image = NULL;

  //check if another SimpleSprite already loaded the image, else load
  //map::insert's integrated check too late, LoadImage already called
  if(!loaded.count(imagepath)){
    GPU_Image* img = GPU_LoadImage(imagepath);
    GPU_SetImageFilter(img, GPU_FILTER_NEAREST);
    loaded.insert(std::make_pair(imagepath, img));
  }

  //now save ptr for faster access
  image = std::get<GPU_Image*>(*(loaded.find(path)));
}

void SimpleSprite::tick()
{}

GPU_Image* SimpleSprite::getImage()
{
  return image;
}

GPU_Rect SimpleSprite::getFrame()
{
  GPU_Image* img = getImage();
  return GPU_MakeRect(0.f, 0.f, img->w, img->h);
}
