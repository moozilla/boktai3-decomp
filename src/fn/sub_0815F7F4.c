#include "global.h"
#include "player.h"

#define gOrbitSineTable gUnk_086149C4
struct PlayerOrbitParticle {
    u8 unknown0[0x18];
    u16 x, y, z, unknown1E;
    u8 unknown20[9];
    u8 angle, riseSpeed, expandDelay, animationPhase, angularSpeed;
    u8 unknown2E[2];
    u16 radius;
};
struct PlayerOrbitEffect {
    u8 unknown0[0x18];
    struct Player *player;
    u8 unknown1C[0xDC];
    void *resource;
    u8 unknownFC[0x3C0];
    u16 frame;
};
extern const s16 gUnk_086149C4[];
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
static inline s32 TruncateOrbitOffset(s32 value)
{
    return value >= 0 ? value >> 12 : -(-value >> 12);
}
void PlayerOrbitEffect_UpdateParticle(struct PlayerOrbitEffect *effect,
                                      struct PlayerOrbitParticle *p)
{
    s32 value, delta;
    s32 valueZ, deltaZ;
    struct Player *player;
    p->angle += p->angularSpeed;
    value = p->radius * gUnk_086149C4[(p->angle - -0x40) & 0xFF];
    delta = TruncateOrbitOffset(value);
    p->x = (u16)effect->player->position.x + delta;
    valueZ = p->radius * gUnk_086149C4[p->angle];
    deltaZ = TruncateOrbitOffset(valueZ);
    p->z = (u16)effect->player->position.z + deltaZ;
    if (effect->frame >= p->expandDelay) {
        if ((effect->frame & 7) == 0 && p->angularSpeed > 2)
            p->angularSpeed--;
        p->radius += 6;
        p->y += p->riseSpeed;
    } else
        p->radius++;
    if (p->animationPhase == 0)
        SpriteDrawable_SelectTileFrame(p, effect->resource, 4);
    else if (p->animationPhase == 4)
        SpriteDrawable_SelectTileFrame(p, effect->resource, 5);
    p->animationPhase++;
    if (p->animationPhase > 7)
        p->animationPhase = 0;
}
