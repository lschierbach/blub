#ifndef GAMEMATH_H
#define GAMEMATH_H

#include "game/vector.hpp"

namespace game::math {

  static const int chunkSize = 32;

  //hex dimensions in game units
  static const float hexWidth = 0.86602540378f;
  static const float hexHeight = 1.f;

  //how far the "triangles" at top/bottom stick out from the middle rectangle
  static const float hexPointInset = 0.25f;

  /*
   * xVector: width of hex; 0
   * yVector: width / 2; height - point inset
   * pVector/qVector: inverse matrix of p&q
   */
  static const vec2<float> pVector{1.154700538f, 0.f};             // x -> p
  static const vec2<float> qVector{-0.666666666f, 1.33333333333f}; // y -> q
  static const vec2<float> xVector{0.86602540378f, 0.f};           // p -> x
  static const vec2<float> yVector{0.433012702f, 0.75f};           // q -> y



  static vec2<float> axialToCartesian(const vec2<float>& axial)
  {
    return (axial[0] * xVector + axial[1] * yVector);
  }

  static vec2<float> cartesianToAxial(const vec2<float>& cartesian)
  {
    return (cartesian[0] * pVector + cartesian[1] * qVector);
  }

  static vec2<float> chunkToAxial(const vec2<int>& chunk)
  {
    vec2<float> newvec(
      static_cast<float>(chunk[0])*chunkSize,
      static_cast<float>(chunk[1])*chunkSize
    );
    return newvec;
  }

  static vec2<float> chunkToCartesian(const vec2<int>& chunk)
  {
    return axialToCartesian(chunkToAxial(chunk));
  }

  static vec2<int> axialToChunk(const vec2<float>& axial)
  {
    vec2<int> newvec(
      static_cast<int>(axial[0]/chunkSize),
      static_cast<int>(axial[1]/chunkSize)
    );
		return newvec;
  }

  static vec2<int> cartesianToChunk(const vec2<float>& cartesian)
  {
    return axialToChunk(cartesianToAxial(cartesian));
  }

  static float chunkToAxialP(int chunkP) {
  	return chunkP*chunkSize;
  }

  static float chunkToAxialQ(int chunkQ) {
  	return chunkQ*chunkSize;
  }

  static int axialToChunkP(float axialP) {
  	return axialP/chunkSize;
  }

  static int axialToChunkQ(float axialQ) {
  	return axialQ/chunkSize;
  }
}

#endif /* GAMEMATH_H */
