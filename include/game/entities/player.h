#ifndef PLAYER_H
#define PLAYER_H

#include "game/entity.h"

class Player : public Entity
{
  public:
    Player(int p, int q) : Entity(p, q) {};
};

#endif /* PLAYER_H */
