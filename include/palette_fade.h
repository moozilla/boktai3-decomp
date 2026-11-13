#ifndef GUARD_PALETTE_FADE_H
#define GUARD_PALETTE_FADE_H
#include "engine.h"
/* Palette assets have a five-word header followed by packed RGB555 colors. */
struct PaletteFadePaletteResource {
    u32 header[5];
    u16 colors[1];
};
struct PaletteFade {
    struct Task task;
    void (*updateScroll)(struct PaletteFade *);
    void *background;
    union {
        struct {
            u16 *source;
            u16 *targets;
        } banks;
        u16 *palettes[2];
    } resources;
    s32 deltaR[16][16], deltaG[16][16], deltaB[16][16];
    s32 red[16][16], green[16][16], blue[16][16];
    s32 remainingFrames, unknown182C, duration, slot, target, remainingTargets;
    u32 flags;
    u16 horizontalSpeed, verticalSpeed, scroll, unknown184A;
    u8 layer, moving;
    s16 lastMapX, remaining;
    u16 destination;
    s32 speed;
    u16 offsets[160], phases[160];
    union {
        struct {
            u16 speedTop, speedBottom, firstLine, split;
        } wave;
        u16 waveParameters[4];
    } scanline;
    u32 mode;
};
void ScanlineScroll_AdvancePhases(struct PaletteFade *);
void ScanlineScroll_Update(struct PaletteFade *);
void BackgroundScroll_UpdateHorizontal(struct PaletteFade *);
void PaletteFade_UpdateTransition(struct PaletteFade *);
void PaletteFade_AdvanceColors(struct PaletteFade *);
void PaletteFade_BeginNextTarget(struct PaletteFade *);
void PaletteFade_SetTransitionParameters(struct PaletteFade *, u32, u32, u32, u32);
void PaletteFade_SetupBackground(struct PaletteFade *, u32, u32, s32, s32);
void PaletteFade_LoadPaletteResources(struct PaletteFade *, u32, u32);
s32 PaletteFade_InitFromScript(struct PaletteFade *);
u32 PaletteFade_OnTaskUpdate(struct PaletteFade *);
u32 PaletteFade_OnTaskDestroy(struct PaletteFade *);
#endif
