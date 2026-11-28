#include "player/player.h"
#include "engine/engine.h"
#include "effects/bursts/spiral_burst.h"

void Player_ShowDirectionalEffect(struct Player *);

void DirectionalEffect_Spawn(struct Player *player, s32 unused,
                             union DirectionalSpritePosition *position)
{
    struct DirectionalSprite *sprite = &player->directionalEffect[0].sprite;
    s32 angle = ((player->facing + 5) & 7) << 5;

    sprite->position = *position;
    sprite->position.axis.x += SpiralBurst_ScaleOffset(0xB4 * gUnk_086149C4[(angle + 64) & 255]);
    sprite->position.axis.depth += SpiralBurst_ScaleOffset(0xB4 * gUnk_086149C4[angle]);
    player->directionalEffect[0].velocityX = 0;
    player->directionalEffect[0].velocityY = 0;
    Anim_SetSequence(&player->directionalEffect[0].animation,
                     player->directionalEffect[0].sequenceResource, 2, 0, 0);
    Player_ShowDirectionalEffect(player);
}
