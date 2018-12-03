#include "game/entities/lightentity.h"

LightEntity::LightEntity()
{
}

float LightEntity::getRadius()
{
  return radius;
}

void LightEntity::setRadius(float r)
{
  radius = r;
}

game::Vector<3,float> LightEntity::getColor()
{
  return color;
}

void LightEntity::setColor(game::Vector<3,float> c)
{
  color = c;
}
