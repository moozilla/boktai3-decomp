#include "directional_sprite_effect.h"

void DirectionalSpriteEffect_Activate(struct Player *player)
{
    player->directionalEffect[0].spriteResource = player->spriteResource;
    player->directionalEffect[0].variantResource = player->actionVariantResource;
    player->directionalEffect[0].sprite.flags &= ~1;
    player->directionalEffect[0].active = 1;
}

extern const s16 gUnk_086149C4[];

void DirectionalEffect_GetAnimationParams(u32, u32 *, u32 *);

static inline s32 scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

void DirectionalSpriteEffect_LaunchFromFacing(struct Player *player, u32 mode,
                                              union DirectionalSpritePosition *position)
{
    struct DirectionalSprite *sprite = &player->directionalEffect[0].sprite;
    s32 angle;
    u32 a, b;
    angle = ((player->facing + 5) & 7) << 5;
    sprite->position = *position;
    sprite->position.axis.height += 190;
    sprite->position.axis.x += scale(240 * gUnk_086149C4[(angle + 64) & 255]);
    sprite->position.axis.depth += scale(240 * gUnk_086149C4[angle & 255]);
    player->directionalEffect[0].velocityX = scale(10 * gUnk_086149C4[(angle + 64) & 255]);
    player->directionalEffect[0].velocityY = scale(10 * gUnk_086149C4[angle & 255]);
    DirectionalEffect_GetAnimationParams(mode, &a, &b);
    Anim_SetSequence(&player->directionalEffect[0].animation,
                     player->directionalEffect[0].sequenceResource, 0, (u8)a, (u8)b);
    DirectionalSpriteEffect_Activate(player);
}

void DirectionalSpriteEffect_PlaceOppositeAngle(struct Player *player, s32 angle,
                                                union DirectionalSpritePosition *position)
{
    player->directionalEffect[0].sprite.flags &= ~1;
    angle = (angle + 128) & 255;
    player->directionalEffect[0].sprite.position = *position;
    player->directionalEffect[0].sprite.position.axis.x +=
        scale(170 * gUnk_086149C4[(angle + 64) & 255]);
    player->directionalEffect[0].sprite.position.axis.height += 230;
    player->directionalEffect[0].sprite.position.axis.depth +=
        scale(170 * gUnk_086149C4[angle & 255]);
    player->directionalEffect[0].sprite.angle = -96 - angle;
    player->directionalEffect[0].active = 1;
}

void DirectionalSpriteEffect_RemoveSprite(struct Player *player)
{
    Sprite_Remove(&player->directionalEffect[0].sprite);
}

void Player_OnOrbitingEffectIdle(void *);

void DirectionalSpriteEffect_InitSprite(struct Player *player)
{
    struct DirectionalSprite *sprite = &player->directionalEffect[0].sprite;
    void *resource = player->directionalEffect[0].resourceHeader;
    AnimData_Load(resource, 0x2110);
    Sprite_Init(sprite, resource, 67);
    sprite->frame = 4;
    if (player->actionMode == 6)
        AnimData_SelectPaletteBank(resource, 81);
    else
        AnimData_SelectPaletteBank(resource, 75);
    sprite->priority = 1;
    sprite->scaleY = 127;
    sprite->scaleX = 127;
    sprite->position = *(union DirectionalSpritePosition *)&player->position;
    player->directionalEffectCallback = Player_OnOrbitingEffectIdle;
    player->directionalEffect[0].active = 0;
}
