#include "player/player.h"

void DirectionalEffect_PlaceSprite(struct Player *player, s32 unused,
                                   union DirectionalSpritePosition *position)
{
    player->directionalEffect[0].sprite.flags &= ~1;
    player->directionalEffect[0].sprite.position = *position;
    player->directionalEffect[0].sprite.position.axis.height += 230;
    player->directionalEffect[0].sprite.angle = player->statModifierContext.contactTimer;
    player->directionalEffect[0].active = 1;
}
