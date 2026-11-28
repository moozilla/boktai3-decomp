#include "player/player.h"
#include "engine/engine.h"
#include "effects/bursts/spiral_burst.h"

void DirectionalEffect_DecodeAnimationMode(u32, u32 *, u32 *);
void Player_ShowDirectionalEffect(struct Player *);

/* Tentative role: place and launch a directional effect. */
void Player_StartDirectionalEffect(struct Player *player, u32 mode,
                                   union DirectionalSpritePosition *position)
{
    struct DirectionalSprite *sprite = &player->directionalEffect[0].sprite;
    s32 angle = ((player->facing + 5) & 7) << 5;
    u32 variant, group;

    sprite->position = *position;
    sprite->position.axis.height += 190;
    sprite->position.axis.x += SpiralBurst_ScaleOffset(240 * gUnk_086149C4[(angle + 64) & 255]);
    sprite->position.axis.depth += SpiralBurst_ScaleOffset(240 * gUnk_086149C4[angle & 255]);
    player->directionalEffect[0].velocityX =
        SpiralBurst_ScaleOffset(10 * gUnk_086149C4[(angle + 64) & 255]);
    player->directionalEffect[0].velocityY =
        SpiralBurst_ScaleOffset(10 * gUnk_086149C4[angle & 255]);
    DirectionalEffect_DecodeAnimationMode(mode, &variant, &group);
    Anim_SetSequence(&player->directionalEffect[0].animation,
                     player->directionalEffect[0].sequenceResource, 0, (u8)variant, (u8)group);
    Player_ShowDirectionalEffect(player);
}
