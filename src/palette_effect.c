#include "palette_effect.h"

static inline struct DisplayAnimationResource *
AnimationPalette(struct DisplaySequenceSprite *animation)
{
    return (struct DisplayAnimationResource *)animation->resource;
}

struct PaletteEffect *PaletteEffect_Create(struct DisplaySequenceSprite *anim,
                                           struct ProjectedDisplayEntry *sprite, u32 hold,
                                           s32 count, s32 start)
{
    struct PaletteEffect *effect =
        Task_Create(10, count * 32 + (u32) & ((struct PaletteEffect *)0)->buffer);
    if (!effect)
        return NULL;
    if (anim) {
        effect->target = (u32)anim;
        effect->savedPaletteIndex = AnimationPalette(anim)->paletteIndex;
        effect->savedPalette = AnimationPalette(anim)->paletteData;
    } else if (sprite) {
        effect->target = (u32)sprite | 1;
        effect->savedPaletteIndex = sprite->paletteIndex;
        effect->savedPalette = sprite->paletteData;
    } else {
        effect->savedPaletteIndex = (u32)sprite;
        effect->savedPalette = (u8 *)sprite;
    }
    effect->state = 0;
    effect->phase = 0;
    effect->hold = hold;
    effect->bankCount = count;
    if (count > 0 && effect->savedPalette)
        CpuFastSet(effect->savedPalette + start * 32, effect->buffer, (count * 8) & 0x1fffff);
    return effect;
}

void PaletteEffect_ApplyBufferedPalette(struct PaletteEffect *effect, s32 firstBank)
{
    u32 target = effect->target;
    struct DisplaySequenceSprite *anim =
        (target & 1) ? NULL : (struct DisplaySequenceSprite *)target;
    struct ProjectedDisplayEntry *sprite =
        (target & 1) ? (struct ProjectedDisplayEntry *)(target & ~1) : NULL;
    u8 *tiles = effect->buffer - firstBank * 32;
    if (anim) {
        AnimationPalette(anim)->paletteIndex = 0x8000 | effect->savedPaletteIndex;
        AnimationPalette(anim)->paletteData = tiles;
    } else if (sprite) {
        sprite->paletteIndex = 0x8000 | effect->savedPaletteIndex;
        sprite->paletteData = tiles;
    }
}

void PaletteEffect_Destroy(void *task)
{
    struct PaletteEffect *effect = task;
    u32 target = effect->target;
    struct DisplaySequenceSprite *anim =
        (target & 1) ? NULL : (struct DisplaySequenceSprite *)target;
    struct ProjectedDisplayEntry *sprite =
        (target & 1) ? (struct ProjectedDisplayEntry *)(target & ~1) : NULL;
    if (anim) {
        AnimationPalette(anim)->paletteIndex = effect->savedPaletteIndex;
        AnimationPalette(anim)->paletteData = effect->savedPalette;
    } else if (sprite) {
        sprite->paletteIndex = effect->savedPaletteIndex;
        sprite->paletteData = effect->savedPalette;
    }
}

void PaletteEffect_UpdateSharedOverride(void *task)
{
    struct PaletteEffect *effect = task;
    u32 target = effect->target;
    struct DisplaySequenceSprite *anim =
        (target & 1) ? NULL : (struct DisplaySequenceSprite *)target;
    struct ProjectedDisplayEntry *sprite =
        (target & 1) ? (struct ProjectedDisplayEntry *)(target & ~1) : NULL;
    if (anim) {
        if (AnimationPalette(anim)->paletteIndex != 0x192) {
            effect->savedPaletteIndex = AnimationPalette(anim)->paletteIndex;
            {
                struct DisplayAnimationResource *resource = AnimationPalette(anim);
                effect->savedPalette = resource->paletteData;
                resource->paletteIndex = 0x192;
            }
            {
                u8 *tiles = gPaletteBankData + 0x3240;
                AnimationPalette(anim)->paletteData = tiles;
            }
        }
    } else if (sprite) {
        if (sprite->paletteIndex != 0x192) {
            effect->savedPaletteIndex = sprite->paletteIndex;
            effect->savedPalette = sprite->paletteData;
            sprite->paletteIndex = 0x192;
            sprite->paletteData = gPaletteBankData + 0x3240;
        }
    }
    if (effect->hold == 0)
        Task_Kill(task);
    else
        effect->hold--;
}

/* PaletteEffect_StartAnimationOverride: Creates a timed shared palette override for an animation */

void PaletteEffect_StartAnimationOverride(struct DisplaySequenceSprite *anim, u32 hold)
{
    void *task = PaletteEffect_Create(anim, NULL, hold, 0, 0);
    if (task) {
        Task_SetCallbacks(task, PaletteEffect_UpdateSharedOverride, PaletteEffect_Destroy);
        {
            u16 id = 0x192;
            u8 *tiles = gPaletteBankData + 0x3240;
            AnimationPalette(anim)->paletteIndex = id;
            AnimationPalette(anim)->paletteData = tiles;
        }
    }
}

void PaletteEffect_StartSpriteOverride(struct ProjectedDisplayEntry *sprite, s32 hold, s32 start)
{
    struct PaletteEffect *effect = PaletteEffect_Create(0, sprite, hold, 0, start);
    if (effect != 0) {
        u8 *palette;
        Task_SetCallbacks(effect, PaletteEffect_UpdateSharedOverride, PaletteEffect_Destroy);
        {
            u16 index = 0x192;
            palette = gPaletteBankData;
            palette += 0x3240;
            palette -= start << 5;
            sprite->paletteIndex = index;
            sprite->paletteData = palette;
        }
    }
}

void Palette_BlendBank(void *, void *, u32, u32, u32);

void PaletteEffect_UpdateRedPulse(void *task)
{
    struct PaletteEffect *effect = task;
    const u8 *src;
    u8 *dst;
    s32 i;
    if (effect->hold == 0) {
        Task_Kill(effect);
        return;
    }
    src = effect->savedPalette;
    dst = effect->buffer;
    if (effect->phase <= 8) {
        for (i = 0; i < effect->bankCount; i++) {
            Palette_BlendBank(dst, src, 16, effect->phase, 3);
            src += 0x20;
            dst += 0x20;
        }
    } else {
        u32 level = 16 - effect->phase;
        for (i = 0; i < effect->bankCount; i++) {
            Palette_BlendBank(dst, src, 16, level, 3);
            src += 0x20;
            dst += 0x20;
        }
    }
    if (++effect->phase > 15)
        effect->phase = 0;
    effect->hold--;
}

/* PaletteEffect_StartAnimationRedPulse: Buffers an animation palette and installs the timed red pulse callback */

void PaletteEffect_StartAnimationRedPulse(struct DisplaySequenceSprite *anim, u32 hold, s32 count)
{
    void *task = PaletteEffect_Create(anim, NULL, hold, count, 0);
    if (task) {
        PaletteEffect_ApplyBufferedPalette(task, 0);
        Task_SetCallbacks(task, PaletteEffect_UpdateRedPulse, PaletteEffect_Destroy);
    }
}

/* PaletteEffect_StartSpriteRedPulse: Buffers selected sprite palette banks and installs the timed red pulse callback */

void PaletteEffect_StartSpriteRedPulse(struct ProjectedDisplayEntry *sprite, u32 hold, s32 count,
                                       s32 start)
{
    void *task = PaletteEffect_Create(NULL, sprite, hold, count, start);
    if (task) {
        PaletteEffect_ApplyBufferedPalette(task, start);
        Task_SetCallbacks(task, PaletteEffect_UpdateRedPulse, PaletteEffect_Destroy);
    }
}

void PaletteEffect_UpdateWhiteFlash(void *task)
{
    struct PaletteEffect *effect = task;
    const u8 *src = effect->savedPalette;
    u8 *dst = effect->buffer;
    s32 i;
    switch (effect->state) {
        case 0:
            for (i = 0; i < effect->bankCount; i++) {
                Palette_BlendBank(dst, src, 0x7fff, effect->phase, 3);
                src += 0x20;
                dst += 0x20;
            }
            if (++effect->phase > 7) {
                effect->state = 1;
                effect->phase = 0;
            }
            break;
        case 1: {
            u32 level = 16 - effect->phase;
            for (i = 0; i < effect->bankCount; i++) {
                Palette_BlendBank(dst, src, 0x7fff, level, 4);
                src += 0x20;
                dst += 0x20;
            }
            if (++effect->phase > 15)
                Task_Kill(task);
            break;
        }
    }
}

/* PaletteEffect_StartAnimationWhiteFlash: Buffers an animation palette and installs the white flash callback */

void PaletteEffect_StartAnimationWhiteFlash(struct DisplaySequenceSprite *anim, s32 count)
{
    void *task = PaletteEffect_Create(anim, NULL, 0, count, 0);
    if (task) {
        PaletteEffect_ApplyBufferedPalette(task, 0);
        Task_SetCallbacks(task, PaletteEffect_UpdateWhiteFlash, PaletteEffect_Destroy);
    }
}

/* PaletteEffect_StartSpriteWhiteFlash: Buffers selected sprite palette banks and installs the white flash callback */

void PaletteEffect_StartSpriteWhiteFlash(struct ProjectedDisplayEntry *sprite, s32 count, s32 start)
{
    void *task = PaletteEffect_Create(NULL, sprite, 0, count, start);
    if (task) {
        PaletteEffect_ApplyBufferedPalette(task, start);
        Task_SetCallbacks(task, PaletteEffect_UpdateWhiteFlash, PaletteEffect_Destroy);
    }
}
