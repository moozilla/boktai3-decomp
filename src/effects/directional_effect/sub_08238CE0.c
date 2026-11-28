#include "player/player.h"
#include "engine/engine.h"
#include "effects/bursts/spiral_burst.h"

void DirectionalEffect_DecodeAnimationMode(u32, u32 *, u32 *);
void Player_ShowDirectionalEffect(struct Player *);

// confidence=low: Player_StartDirectionalEffect with a size class: 0 = lift 160, radius 450;
// 1 = lift 140, radius 338; otherwise lift 120, radius 224.
void sub_08238CE0(struct Player *player, u32 mode, union DirectionalSpritePosition *position,
                  u32 size)
{
    struct DirectionalSprite *sprite = &player->directionalEffect[0].sprite;
    s32 angle;
    u32 variant, group;
    s32 lift, radius;

    if (size == 0) {
        lift = 160;
        radius = 450;
    } else if (size == 1) {
        lift = 140;
        radius = 338;
    } else {
        lift = 120;
        radius = 224;
    }
    angle = ((player->facing + 5) & 7) << 5;

    sprite->position = *position;
    sprite->position.axis.height += lift;
    sprite->position.axis.x += SpiralBurst_ScaleOffset(radius * gUnk_086149C4[(angle + 64) & 255]);
    sprite->position.axis.depth += SpiralBurst_ScaleOffset(radius * gUnk_086149C4[angle & 255]);
    player->directionalEffect[0].velocityX =
        SpiralBurst_ScaleOffset(10 * gUnk_086149C4[(angle + 64) & 255]);
    player->directionalEffect[0].velocityY =
        SpiralBurst_ScaleOffset(10 * gUnk_086149C4[angle & 255]);
    DirectionalEffect_DecodeAnimationMode(mode, &variant, &group);
    Anim_SetSequence(&player->directionalEffect[0].animation,
                     player->directionalEffect[0].sequenceResource, 1, (u8)variant, (u8)group);
    Player_ShowDirectionalEffect(player);
}
