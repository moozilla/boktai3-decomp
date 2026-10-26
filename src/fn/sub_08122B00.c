#include "global.h"
#include "ram.h"
struct StateSprite {
    u8 unk0[8];
    u32 flags;
    u8 unkC[0xC8];
    s16 position[4];
    u8 unkDC[0x54];
    u16 variant, animationIndex, active;
};
extern const u8 gUnk_086106F0[];
void Widget_InitLinked(void *, void *, int, int, int, int, int, void *);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
s32 Anim_Step(void *, void *);
void StateSprite_InitAnimation(struct StateSprite *sprite, s32 type, s32 direction, s32 variant)
{
    u8 *data = gAnimatedBodyPool;
    s32 index = direction * 3 + type * 9;
    u16 *indexPtr = &sprite->animationIndex;
    u32 zero = 0;
    *indexPtr = index;
    sprite->variant = variant;
    sprite->active = 1;
    data += 0x18;
    Widget_InitLinked(sprite, data, 13, 0, 1, 2, 0x78, sprite->position);
    Anim_SetKeyframes(sprite, data, gUnk_086106F0[0x12], 2, zero);
    Anim_Step(sprite, data);
    sprite->flags &= ~1;
}
