#include <memory>

struct Animation {
  GPU_Image* image;
  size_t numFrames;
  float time;

  Animation(GPU_Image* image, size_t numFrames, float time)
  {
		this->image = image;
		this->numFrames = numFrames;
		this->time = time;
  }

  GPU_Rect getFrame(float stage)
  {
    if(stage != 0) {
      return GPU_MakeRect(
        (numFrames / (time/stage)) * (image->w / numFrames),
        0,
        image->w/numFrames,
        image->h
      );
    } else {
      return GPU_MakeRect(
        0,
        0,
        image->w/numFrames,
        image->h
      );
    }
  }
};

namespace game
{
  using SharedAnimPtr = std::shared_ptr<Animation>;
}