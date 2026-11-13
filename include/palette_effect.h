#ifndef GUARD_PALETTE_EFFECT_H
#define GUARD_PALETTE_EFFECT_H
#include "display_animation.h"
#include "projected_display.h"

/* target bit zero distinguishes a projected sprite from a sequence sprite.
 * The palette storage follows the fixed header and contains bankCount banks. */
struct PaletteEffect {
    struct Task task;
    u32 target;
    u8 state, phase, hold, bankCount;
    u16 savedPaletteIndex;
    u8 unknown22[2];
    const u8 *savedPalette;
    u8 buffer[1];
};
struct PaletteEffect *PaletteEffect_Create(struct DisplaySequenceSprite *,
                                           struct ProjectedDisplayEntry *, u32, s32, s32);
void PaletteEffect_ApplyBufferedPalette(struct PaletteEffect *, s32);
void PaletteEffect_Destroy(void *);
void PaletteEffect_UpdateSharedOverride(void *);
void PaletteEffect_UpdateRedPulse(void *);
void PaletteEffect_UpdateWhiteFlash(void *);
void PaletteEffect_StartAnimationOverride(struct DisplaySequenceSprite *, u32);
void PaletteEffect_StartSpriteOverride(struct ProjectedDisplayEntry *, s32, s32);
void PaletteEffect_StartAnimationRedPulse(struct DisplaySequenceSprite *, u32, s32);
void PaletteEffect_StartSpriteRedPulse(struct ProjectedDisplayEntry *, u32, s32, s32);
void PaletteEffect_StartAnimationWhiteFlash(struct DisplaySequenceSprite *, s32);
void PaletteEffect_StartSpriteWhiteFlash(struct ProjectedDisplayEntry *, s32, s32);
#endif
