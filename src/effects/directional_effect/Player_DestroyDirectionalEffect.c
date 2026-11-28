#include "player/player.h"
#include "engine/engine.h"

void Player_DestroyDirectionalEffect(struct Player *player)
{
    Sprite_Remove(&player->directionalEffect[0].sprite);
}
