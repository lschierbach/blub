#ifndef LODIMAGE_HPP
#define LODIMAGE_HPP

#include <string>
#include "SDL_gpu.h"

#include "game/entities/camera.h"

class LODImage {
  private:
    std::vector<GPU_Image*> images;

    // save last request's data for fast repeated acces
    float lastUnit = -1.f;
    size_t lastIndex = 0;

  public:
    LODImage(const std::string* paths, size_t  numImages, unsigned int minWidth) {
      // initial images
      GPU_Image* img = NULL;
      for(int i=0; i<numImages; i++){
        img = GPU_LoadImage(paths[i].c_str());
        GPU_SetImageFilter(img, GPU_FILTER_NEAREST);
        images.push_back(img);
      }

      for(size_t i=0; img->w/2 >= minWidth; i++) {
        // create next smaller image
        img = GPU_CreateImage(img->w/2, img->h/2, GPU_FORMAT_RGBA);
        GPU_SetImageFilter(img, GPU_FILTER_NEAREST);

        // render old image to new
        GPU_LoadTarget(img);
        GPU_ClearColor(img->target, {0,0,0,0});
        GPU_BlitScale(images[i], NULL, img->target, img->w/2, img->h/2, 0.5f, 0.5f);

        // add to other images
        images.push_back(img);
      }
    }

    LODImage(const char* path, unsigned int minWidth) {
      // initial image
      GPU_Image* img = GPU_LoadImage(path);
      GPU_SetImageFilter(img, GPU_FILTER_NEAREST);
      images.push_back(img);

      for(size_t i=0; img->w/2 >= minWidth; i++) {
        // create next smaller image
        img = GPU_CreateImage(img->w/2, img->h/2, GPU_FORMAT_RGBA);
        GPU_SetImageFilter(img, GPU_FILTER_NEAREST);

        // render old image to new
        GPU_LoadTarget(img);
        GPU_ClearColor(img->target, {0,0,0,0});
        GPU_BlitScale(images[i], NULL, img->target, img->w/2, img->h/2, 0.5f, 0.5f);

        // add to other images
        images.push_back(img);
      }
    }

    GPU_Image* bestImage(Camera* camera) {
      // already requested last time?
      if(camera->pixelsInUnit() == lastUnit) {
        lastUnit = camera->pixelsInUnit();
        return images[lastIndex];
      }
      // if not, update for next time
      lastUnit = camera->pixelsInUnit();

      float optimalW = camera->pixelsInUnit()*16.f;

      for(size_t i=images.size(); i>0; --i) {
        if(images[i-1]->w >= optimalW) {
          // keep track of index for next time
          lastIndex = i-1;
          return images[i-1];
        }
      }

      // if nothing is found, return default first image
      return images[0];
    }
};

#endif /* LODIMAGE_HPP */