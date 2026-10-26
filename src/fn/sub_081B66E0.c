// MATCHING_CONCESSION: register-pinning
#include "global.h"

/* Tentative Vanargand family; the two vectors are copied as eight-byte records. */

struct MotionTrackVector {
    s16 x, y, z, padding;
};
struct MotionTrack {
    const void *data;
    u8 unknown4[8];
    struct MotionTrackVector origin, target;
    u8 unknown1C[12];
    u8 mode, active;
    u8 unknown2A[10];
    u32 cursor;
};
struct VanargandEffect {
    u8 unknown0[0x18];
    u32 spriteFlags;
    u8 unknown1C[0x18];
    struct MotionTrackVector position;
    u8 unknown3C[8];
    u8 animation[0x1C];
    struct MotionTrack motion;
    u16 delay;
    u16 lifetime;
    u16 variant;
};
s32 AnimData_Load(void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void Sprite_Init(void *, void *, u32);
void MotionTrack_Initialize(struct MotionTrack *, const void *, const struct MotionTrackVector *,
                            const struct MotionTrackVector *, u32);

s32 VanargandEffect_Initialize(struct VanargandEffect *effect, s32 variant, s32 delay,
                               const struct MotionTrackVector *position,
                               const struct MotionTrackVector *target, u32 alternate)
{
    u32 palette;
    AnimData_Load(effect->animation, 0x89CB);
    Sprite_Init(&effect->spriteFlags, effect->animation, 0);
    if (delay > 0)
        effect->spriteFlags |= 1;
    if (alternate)
        effect->spriteFlags |= 4;
    if (variant == 0)
        palette = 0x73;
    else if (variant == 1) {
        palette = 0x71;
        if (alternate)
            palette = 0x70;
    } else if (variant == 2) {
        palette = 0x81;
        if (alternate)
            palette = 0x80;
    } else {
        palette = 0x84;
        if (alternate)
            palette = 0x83;
    }
    AnimData_SelectPaletteBank(effect->animation, palette);
    {
        typeof(effect->variant) *variantPtr = &effect->variant;
        *variantPtr = variant;
    }
    {
        typeof(effect->delay) *delayPtr = &effect->delay;
        *delayPtr = delay;
    }
    {
        typeof(effect->lifetime) *lifetimePtr = &effect->lifetime;
        *lifetimePtr = 30;
        asm("" ::: "r1");
    }
    {
        typeof(effect->position) *effectPosition = &effect->position;
        *effectPosition = *position;
        {
            struct MotionTrack *motion = &effect->motion;
            const void **data = &motion->data;
            *data = 0;
            motion->active = 0;
            motion->mode = 0;
            motion->cursor = 0;
        }
        MotionTrack_Initialize(&effect->motion, &effect->position, 0, target, 0);
        return 0;
    }
}
