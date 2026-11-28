#include "player/player.h"

void Player_ShowDirectionalEffect(struct Player *player)
{
    player->directionalEffect[0].spriteResource = player->animation.paletteBank;
    player->directionalEffect[0].variantResource = (u32)player->animation.palette;
    player->directionalEffect[0].sprite.flags &= ~1;
    player->directionalEffect[0].active = 1;
}
