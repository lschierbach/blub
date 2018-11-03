#ifndef PLAYER_H
#define PLAYER_H

#include "game/entity.h"

class Player : public Entity
{
  public:
    Player(int x, int y) : Entity(x, y) {};
};

#endif /* PLAYER_H */
