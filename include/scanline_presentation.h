#ifndef SCANLINE_PRESENTATION_H
#define SCANLINE_PRESENTATION_H
#include "engine.h"
#include "render.h"

// Tentative: a full-screen effect that fades per-scanline horizontal offsets (a noise ripple or
// wave, two buffers of 160 entries swapped each frame) in and out around a set of six layers.

struct ScanlineLayer {
    u32 pad0[2];
    u32 flags; // bit 0: hidden
    u8 rest[84];
};

struct ScanlineResourceHeader {
    u32 words[8];
};

struct ScanlinePresentation;
typedef void (*ScanlineCallback)(struct ScanlinePresentation *);

struct ScanlinePresentation {
    u8 task[0x18];
    struct ScanlineResourceHeader header; // copy of the script resource's header
    u16 state, timer;                     // state: 0 delay, 1 fading in, 2 hold, 3 fading out
    u32 ticks, script;
    s16 cancellable, mode, phase, delay, hold, duration;
    u32 handle;
    u16 rows[2][160]; // per-scanline values, double buffered
    u16 active;       // buffer being filled
    s16 amplitude, delta;
    u16 start, end; // first and last scanline of the noisy band
    s8 digitsA, digitsB;
    s8 banner; // nonzero: show the banner layer
    u8 unused2E1[3];
    ScanlineCallback fade;
    ScanlineCallback noise;
    struct ScanlineLayer layers[6];
};

extern u16 gPadState[];

void ScanlinePresentation_UpdateNoise(struct ScanlinePresentation *p);
void ScanlinePresentation_UpdateFadeIn(struct ScanlinePresentation *p);
void ScanlinePresentation_UpdateFadeOut(struct ScanlinePresentation *p);
int ScanlinePresentation_Update(struct ScanlinePresentation *p);
int ScanlinePresentation_Destroy(struct ScanlinePresentation *p);
s32 sub_0800DC90(struct ScanlinePresentation *p);
void *ScanlinePresentation_Create(void);

#endif
