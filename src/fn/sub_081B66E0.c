// MATCHING_CONCESSION: register-pinning
#include "vanargand_effect.h"

/* Tentative Vanargand family; the two vectors are copied as eight-byte records. */

s32 AnimData_Load(void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void Sprite_Init(void *, void *, u32);

s32 VanargandEffect_Initialize(struct VanargandEffect *effect, s32 variant, s32 delay,
                               const struct MotionTrackVector *position,
                               const struct MotionTrackVector *target, u32 alternate)
{
    u32 palette;
    AnimData_Load(effect->graphics, 0x89CB);
    Sprite_Init(&effect->sprite, effect->graphics, 0);
    if (delay > 0)
        effect->sprite.flags |= 1;
    if (alternate)
        effect->sprite.flags |= 4;
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
    AnimData_SelectPaletteBank(effect->graphics, palette);
    {
        u16 *dest = &effect->variant;
        *dest = variant;
    }
    {
        u16 *dest = &effect->delay;
        *dest = delay;
    }
    {
        u16 *dest = &effect->lifetime;
        *dest = 30;
    }
    asm("" ::: "r1");
    {
        struct MotionTrackVector *origin = &effect->sprite.position;
        *origin = *position;
    }
    MotionTrack_Reset(&effect->motion);
    MotionTrack_Initialize(&effect->motion, &effect->sprite.position, 0, target, 0);
    return 0;
}
