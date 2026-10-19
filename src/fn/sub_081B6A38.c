#define ActorEffect_UpdatePositionTrail sub_081B6A38
#include "global.h"
struct EffectPosition {
    s32 x, y;
};
struct EffectTrailSprite {
    u8 pad[0x1c];
    struct EffectPosition position;
    u8 tail[8];
};
struct ActorEffect {
    u8 pad0[6];
    u16 flags;
    u8 trailIndex;
    u8 pad9[0x14 - 9];
    struct EffectPosition position;
    struct EffectPosition history[6];
    u8 pad4c[0x12c - 0x4c];
    struct EffectPosition savedPosition;
    u8 pad134[8];
    struct EffectTrailSprite trails[5];
};
void sub_081B6A38(struct ActorEffect *effect)
{
    s32 index, count;
    struct EffectTrailSprite *sprite;
    if (!(effect->flags & 1))
        effect->savedPosition = effect->position;
    index = effect->trailIndex - 1;
    if (index < 0)
        index = 5;
    sprite = effect->trails;
    count = 4;
    do {
        sprite->position = ((struct ActorEffect *)(index * 8 + (u32)effect))->history[0];
        index--;
        if (index < 0)
            index = 5;
        count--;
        sprite++;
    } while (count >= 0);
    if (!(effect->flags & 1)) {
        ((struct ActorEffect *)(effect->trailIndex * 8 + (u32)effect))->history[0] =
            effect->savedPosition;
        effect->trailIndex++;
        if (effect->trailIndex > 5)
            effect->trailIndex = 0;
    }
}
