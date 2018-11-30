/*
 *  FILENAME:      camera.cpp
 *
 *  DESCRIPTION:
 *      convenient ways of rendering to a GPU_Image
 *  NOTES:
 *    
 *
 *  AUTHOR:        Tobias Fey     DATE: 01.10.2018
 *
 *  CHANGES:
 *
 *  TODO: Remove redundant constructor code
 *
 */

#include <iostream>

#include "game/entities/camera.h"
#include "game/gamemath.hpp"

Camera::~Camera()
{
  GPU_FreeImage(image);
}

Camera::Camera(float x, float y, float w, float h, float scale)
{
  image = GPU_CreateImage(getSize()[0], getSize()[1], GPU_FORMAT_RGBA);
  GPU_GetTarget(image);
  GPU_SetImageFilter(image, GPU_FILTER_NEAREST); //No blur when scaling up

  setPos(vec2<float>(x,y));
  setSize(vec2<float>(w,h));
  setScale(scale);

  



  

}

void Camera::setSize(vec2<float> s)
{
  GPU_FreeImage(image);
  image = GPU_CreateImage(s[0], s[1], GPU_FORMAT_RGB);
  GPU_SetImageFilter(image, GPU_FILTER_NEAREST); //No blur when scaling up
  GPU_GetTarget(image);
  Entity::setSize(s);
#ifdef DEBUG_CAMERA_VERBOSE
  std::cout << "[CAMERA] set size " << getSize()[0] << "|" << getSize()[1] << std::endl;
#endif
}

void Camera::setScale(float s)
{
  scale = s;
}

float Camera::getScale() const
{
  return scale;
}

float Camera::pixelsInUnit() const
{
  return getSize()[1]/scale;
}

float Camera::unitsInPixel() const
{
  return scale/getSize()[1];
}

float Camera::unitsX() const
{
  return unitsInPixel() * image->w;
}

float Camera::unitsY() const
{
  return unitsInPixel() * image->h;
}

void Camera::clearRender()
{
  GPU_ClearRGB(image->target, 0, 0, 0);
}

GPU_Image* Camera::getRender() const
{
  return image;
}

GPU_Rect Camera::getTile(GPU_Image* img, unsigned char index, unsigned char inset)
{
  char x = index % 16;
  char y = index / 16;

  GPU_Rect r = GPU_MakeRect((img->w/16.f)*x + inset, (img->h/16.f)*y + inset, (img->w/16.f) - inset, (img->h/16.f) - inset);
  return r;
}

void Camera::renderTileset(const Tileset& ts, GPU_Image* img, float pad_x, float pad_y, float x_offset, float y_offset)
{
  float logicalWidth  = game::math::tileWidth  * ts.scale * pixelsInUnit();
  float logicalHeight = game::math::tileHeight * ts.scale * pixelsInUnit();

  float realHeight    = ts.scale * pixelsInUnit() * (1+2*pad_y);
  float realWidth     = realHeight * ((float)img->w / (float)img->h) * (1+2*pad_x);

  float initX = (getSize()[0]/2) - (getPos()[0] - x_offset - pad_x) * pixelsInUnit() * ts.scale;
  float initY = (getSize()[1]/2) - (getPos()[1] - y_offset - pad_y) * pixelsInUnit() * ts.scale;

  GPU_Rect targetRect = GPU_MakeRect(
      initX,
      initY,
      realWidth,
      realHeight
  );

  /*
  GPU_ActivateShaderProgram(sp_tile, &block_tile);

  GPU_SetUniformf(GPU_GetUniformLocation(sp_tile, "time"), SDL_GetTicks()/1000.f);
  GPU_SetUniformf(GPU_GetUniformLocation(sp_tile, "scale"), getScale());
  */
  
  for(int i=0; i<ts.tileData.size(); i++)
  {
    for(int j=0; j<ts.tileData[i].size(); j++)
    {
      char c;
      if((c = ts.tileData[i][j].index) != 0
         && (targetRect.x+targetRect.w > 0 && targetRect.y+targetRect.h > 0)
         && (targetRect.x < image->w && targetRect.y < image ->h) ) 
      {
        GPU_Rect sourceRect = getTile(img, c, 0);
        GPU_Rect roundedTarget = GPU_MakeRect(
          floor(targetRect.x),
          floor(targetRect.y),
          ceil(targetRect.w),
          ceil(targetRect.h)
        );

        GPU_BlitRect(img, &sourceRect, image->target, &roundedTarget); //render from tile on given image to this cam's render image

      }
      targetRect.x += logicalWidth;
    }
    targetRect.y += logicalHeight;
    targetRect.x = initX;
  }
  //GPU_DeactivateShaderProgram();
}

void Camera::render2dMap(int* data, SDL_Color (*conversion)(int), size_t w, size_t h)
{
  float width = getSize()[0]/w, height = getSize()[1]/h;
  for(size_t i=0; i<h; i++)
  {
    for(size_t j=0; j<w; j++)
    {
      SDL_Color col = {0,0,0,255};
      if(conversion == NULL)
      {
        if(data[j+w*i])
        {
          col = {255,255,255,255};
        }
      } 
      else
      {
        col = conversion(data[j+w*i]);
      }

      GPU_RectangleFilled(image->target, j*width, i*height, (j+1)*width, (i+1)*height, col);
    }
  }
}

/*
void Camera::renderEntity(RenderEntity e)
{
#ifdef DEBUG_CAMERA_VERBOSE
  std::cout << "[CAMERA] rendering a RenderEntity." << std::endl;
#endif

  float entityX = (getSize()[0]/2) - (getXY()[0] - e.getXY()[0]) * pixelsInUnit();
  float entityY = (getSize()[1]/2) - (getXY()[1] - e.getXY()[1]) * pixelsInUnit();

  GPU_Rect entityRect = GPU_MakeRect(
    entityX,                         // x1
    entityY,                         // y1
    e.getSize()[0] * pixelsInUnit(), // w
    e.getSize()[1] * pixelsInUnit()  // h
  );

  auto anim = e.getCurrentAnimation().get();

  GPU_Rect animRect = anim->getFrame(e.getStage());

  GPU_BlitRect(anim->image, &animRect, image->target, &entityRect);
}
*/

void Camera::renderEntity(Entity& e)
{
  //get upper left on-screen x and y
  float entityX = (getSize()[0]/2) - (getPos()[0] - e.getPos()[0] + e.getAnchor()[0]*e.getSize()[0]) * pixelsInUnit();
  float entityY = (getSize()[1]/2) - (getPos()[1] - e.getPos()[1] + e.getAnchor()[1]*e.getSize()[1]) * pixelsInUnit();

  if(e.sprite != nullptr) {
    GPU_Rect sourceRect = e.sprite.get()->getFrame();
    GPU_Rect targetRect = GPU_MakeRect(
      entityX,
      entityY,
      e.getSize()[0] * pixelsInUnit(),
      e.getSize()[1] * pixelsInUnit()
    );
    GPU_BlitRect(e.sprite.get()->getImage(), &sourceRect, image->target, &targetRect);
  } else {
    GPU_RectangleFilled(
      image->target,                               // render target
      entityX,                                     // x1
      entityY,                                     // y1
      entityX + (e.getSize()[0] * pixelsInUnit()), // x2
      entityY + (e.getSize()[1] * pixelsInUnit()), // y2
      {255,255,0,255}                                  // color for generic entity
    );
  }
}

void Camera::addOverlay(const Overlay* const o)
{
  overlays.push_back(o);
}

void Camera::removeOverlay(const Overlay* const element)
{
  overlays.remove(element);
}

void Camera::clearOverlays()
{
  overlays.clear();
}

void Camera::renderOverlays()
{
  float w, h;

  for(const Overlay* o: overlays) 
  {
      if(o->visible){
      //If w/h <=0, auto-calculate that dimension
      if(o->w <= 0 && o->h > 0)
      {
        h = o->h;
        w = (o->h * getSize()[1] / o->image->h) * o->image->w / getSize()[0];
      }
      else if(o->h <= 0 && o->w > 0)
      {
        w = o->w;
        //h = (w * wt / wi) * hi / ht
        h = (o->w * getSize()[0] / o->image->w) * o->image->h / getSize()[1];
      }
      else
      {
        w = o->w;
        h = o->h;
      }

      GPU_Rect targetRect = GPU_MakeRect(
        o->x * getSize()[0],
        o->y * getSize()[1],
        w * getSize()[0],
        h * getSize()[1]
      );

      GPU_BlitRect(o->image, NULL, image->target, &targetRect);
    }
  }
}

void Camera::track(Map::SharedEntityPtr entity)
{
  tracked = entity;
}

void Camera::tick() {
  if(tracked.get() != NULL) 
  {
    setPos(tracked.get()->getPos());
  }
}

vec2<float> Camera::pixelToXY(vec2<float> pixel)
{
  return ((pixel - (getSize()/2.f)) * unitsInPixel()) + getPos();
}

vec2<float> Camera::worldToPixel(vec2<float> worldPos) {
  return (worldPos - getPos())*pixelsInUnit() + getSize()/2.f;
}
