/*
 *  FILENAME:      Overlay.h
 *
 *  DESCRIPTION:
 *      Overlay for layering on top of camera's rendered image
 *
 *  AUTHOR:         Tobias Fey      DATE: 17.10.2018
 *
 */

#ifndef OVERLAY_H
#define OVERLAY_H

#include "SDL_gpu.h"

struct Overlay
{
    GPU_Image* image;
    float x;
    float y;
    float w;
    float h;
};

#endif /* OVERLAY_H */
